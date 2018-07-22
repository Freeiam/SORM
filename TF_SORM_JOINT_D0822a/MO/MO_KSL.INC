;;; ---------------------------------------------------------------------------
;;; --- работа с КСЛ - НАЧАЛО                                               ---
;;; ---------------------------------------------------------------------------
;
; --- освобождение КСЛ --------------------------------------------------------
; вход:         es:[si]         - ссылка на сессию
;               dh		- код освобождения, если = 0 освобождать КСЛ не надо
;
KSL_Free        PROC
		push	es
		pushad
                ; --- а есть ли КСЛ ?
                mov     ax, WORD PTR es:[si].sorm_ksl_a ;
                cmp     al, 0ffh                        ;
                je      @@exit                          ;
                ; --- сессия завершается ?
                or      dh, dh                          ;
                jz      @@clear_ksl                     ;
                ; --- здесь не завершение сессии, а освобождение КСЛ по той или иной причине
                ; сообщение "освобождение КСЛ"
                mov     es:[si].sorm_opcode, dh         ;
		mov	dl, SO_OSV                      ;
		call	Message_SORM                    ;
                ; изменяем параметры сессии
                mov     WORD PTR es:[si].sorm_ksl_a, 0ffffh ; нет КСЛ
                mov     es:[si].sorm_prior, 0ffh        ; нет приоритета
                mov     dl, es:[si].sorm_get_num        ;
                mov     es:[si].sorm_get_num, 2         ; статика для А
                test    dl, 3                           ;
                jnz     @@clear_ksl                     ;
                mov     es:[si].sorm_get_num, 8         ; статика для Б
                test    dl, 0ch                         ;
                jnz     @@clear_ksl                     ;
                mov     es:[si].sorm_get_num, 20h       ; статика для пучка
                ; --- здесь есть что освобождать - в al № КСЛ-А
@@clear_ksl:    @COMM_RESET                             ; сняли коммутацию
                ; освобождаем КСЛ и разрываем связь КСЛ с сессией
                movzx   bx, al                          ; № КСЛ-А
		mov	KSL_Busy[bx], 255		;
                shl     bx, 1                           ;
                mov     KSL_Session[bx], 0ffffh         ;
                ; ищем связанный тайм-слот модема
@@find_tms_mod: mov     cx, _MAX_MODEM_TS               ;
                xor     bx, bx                          ;
@@ksl_mod_find: cmp     al, Modem_KSL[bx]               ;
                je      @@ksl_mod_found                 ;
                inc     bx                              ;
                loop    @@ksl_mod_find                  ;
                jmp     @@exit                          ;
@@ksl_mod_found:
                ; --- {Paul} --- <12/02/2013>
                test    FG_SORM4, _3_pty_sorm           ; Сессия 3PTY: B(*) -> C ?
                jz      @@go_121                        ; нет, снимем ТЧ на модеме
                and     FG_SORM4, not _3_pty_sorm
                pushad
                mov     eax, PEREM_EBX
                xor     bp, bp
                mov     cx, _sorm_3pty_max
                ; --- нас интересует №А в сессиях 2-го соединения
                mov     edx, DWORD PTR es:[si].sorm_num_a
  @@find_slot:  cmp     Sess_3PTY[bp].perem_ebx_3pty, eax ; это наше соединение, в котором 3PTY?
                jne     @@next_slot                       ; нет
                cmp     Sess_3PTY[bp].sorm_numb_3pty, edx ; номера в сессиях соответствуют ?
                jne     @@next_slot                       ; да
 @@whats_slot:  cmp     Sess_3PTY[bp].perem_tms, 0FFh     ; таймслот прописан?
                jne     @@next_slot                       ; да
                mov     Sess_3PTY[bp].perem_tms, bl       ; ->запомним номер таймслота для данной сессии
                mov     eax, es:[si].sorm_soed            ;\
                mov     Sess_3PTY[bp].soed_ptr, eax       ; ->запомним ук-ль в соед.
                popad
                jmp     @@exit
  @@next_slot:  add     bp, _sorm_3pty_size
                loop    @@find_slot
                popad
                ; --- {Paul} --- <END>
                ; -- если, вдруг, ничего не найдем, тогда
                ; освобождаем связанный тайм-слот модема
@@go_121:       mov     Modem_KSL[bx], 0ffh             ;
                ; запоминаем абс. рабочий №
                mov     al, es:[si].sorm_ck_a           ;
                mov     SORM_CK, al                     ;
                mov     ax, es:[si].sorm_work_a         ;
                mov     SORM_Work_Num, ax               ;
                ; отправляем МО_121
                @POLUCH_ADR_MO                          ;
                or      dl, dl                          ; проверим есть место ?
                jnz     @@exit                          ; нет - пропустим
                mov     al, 121                         ; сбросить канал
                stosb                                   ;
                mov     al, SORM_CK                     ;
                stosb                                   ;
                mov     ax, SORM_Work_Num               ;
                stosw                                   ;
                mov     ax, bx                          ; bx - порядковый № найденного тайм-слота
                add     ax, ax                          ;
                add     al, _MODEM_1ST_TS               ; базовый № тайм-слота
                mov     ah, al                          ;
                inc     ah                              ;
                stosw                                   ;
                @ZAPIS_OK_MO                            ;
@@exit:         popad
		pop	es
		ret
KSL_Free        ENDP

KSL_Free_Far    PROC    FAR
                call    KSL_Free
                retf
                ENDP


; --- занятие КСЛ с учётом приоритета и формирование при необходимости сообщения "Освобождение КСЛ"
; вход:         gs:[bx]         - ссылка на объект СОРМ или фиктивный объект подключения
;               es:[si]         - ссылка на сессию СОРМ
;               dh              - возможные причины освобождения КСЛ:
;                                 OSV_PRI  - от абонента с высоким приоритетом (автоматическое)
;                                 OSV_KOM7 - при подключении к КСЛ (ручное)
;
KSL_Get         PROC
		push	es
		pushad
                ; --- проверка на статистику только для автоматического выделения КСЛ
		cmp	dh, OSV_PRI			;
                jne     @@ksl_prms                      ;
                ; --- автоматически подключиться к статистике нельзя !
                mov     BYTE PTR es:[si].sorm_opcode, NEPODKL_STAT ;
                cmp     gs:[bx].obj_cat_ctrl, STATIC    ;
                je      @@exit                          ;
                ; --- получаем параметры КСЛ для объекта
@@ksl_prms:     mov     al, gs:[bx].obj_gr_ksl          ;
                mov     dl, gs:[bx].obj_prior           ;
                mov     ah, gs:[bx].obj_get_num         ;
		mov	PEREM_AH, ah                    ;
                ; --- поиск свободной КСЛ в группе с учётом приоритета
                mov     BYTE PTR es:[si].sorm_opcode, NEPODKL_ZAN  ;
                mov     SORM_KSL_Clear, 0ffffh          ; пока ничего не нашли
                mov     cx, 256                         ; по всем возможным КСЛ
		movzx	bp, al                          ;
                inc     KSL_Last[bp]                    ; со следующей КСЛ этой группы
                movzx   bx, KSL_Last[bp]                ; эта КСЛ после КСЛ занимавшейся последней
@@ksl_find:     cmp     KSLs[bx], al                    ; КСЛ входит в нужную группу ?
                jne     @@ksl_next                      ; НЕТ - дальше
                cmp     KSL_A[bx], 0ffh                 ; это КСЛ-А ?
                je      @@ksl_next                      ; НЕТ - дальше
                cmp     KSL_Busy[bx], 0ffh              ; КСЛ свободна ?
                jne     @@ksl_prior                     ; НЕТ - проверим приоритет
                mov     KSL_Last[bp], bl                ; ДА - запомним индекс найденной КСЛ
                jmp     @@ksl_found                     ; НАШЛИ !
@@ksl_prior:    cmp     dl, PRIOR_LO                    ; хотим занять низким приоритетом ?
                je      @@ksl_next                      ; ДА - больше ловить нечего, дальше
                cmp     KSL_Busy[bx], PRIOR_HI          ; КСЛ занята высоким приоритетом ?
                je      @@ksl_next                      ; ДА - дальше
                cmp     SORM_KSL_Clear, 0ffffh          ; уже находили КСЛ занятую низ. приор-том ?
                jne     @@ksl_next                      ; ДА - дальше
                mov     SORM_KSL_Clear, bx              ; запомним КСЛ занятую низким приоритетом
@@ksl_next:     inc     bx                              ; дальше по всем КСЛ
                and     bx, 0ffh                        ; заворот номеров по mod 256
                loop    @@ksl_find                      ;
		; ---
                cmp     SORM_KSL_Clear, 0ffffh          ; нашли в группе КСЛ занятую низ. приор-м ?
                je      @@exit                          ; НЕТ - облом, т.к. все КСЛ под выс. пр-ом
                ; --- освобождение КСЛ занятой низким приоритетом -------------
@@ksl_free:     mov     bx, SORM_KSL_Clear              ;
                push    si                              ;
                shld    si, bx, 17                      ;
                mov     si, KSL_Session[si]             ;
                call    KSL_Free                        ; dh установлен на входе в KSL_Get
                pop     si                              ;
                ; --- проключение КСЛ -----------------------------------------
@@ksl_found:    ; --- ищем свободное ТЧ на модеме
                xor     bp, bp                          ;
                mov     cx, _MAX_MODEM_TS               ;
@@ksl_mod_find: cmp     Modem_KSL[bp], 0ffh             ;
                je      @@ksl_mod_found                 ;
		inc	bp				;
                loop    @@ksl_mod_find                  ;
                jmp     @@ksl_no_free_ts                ;
@@ksl_mod_found:; -- свободный тайм-слот на модеме найден
                mov     Modem_KSL[bp], bl               ;
                ; запоминаем абс. рабочий номер и тайм-слоты модема
                mov     al, es:[si].sorm_ck_a           ;
                mov     SORM_CK, al                     ;
                mov     ax, es:[si].sorm_work_a         ;
                mov     SORM_Work_Num, ax               ;
                ; вычисляем запрашиваемые тайм-слоты
                mov     ax, bp                          ;
                add     ax, ax                          ;
                add     al, _MODEM_1ST_TS               ;
                mov     ah, al                          ;
                inc     ah                              ;
                mov     SORM_Mod_Nums, ax               ;
                ; отправляем МО_120
                push    es                              ;
                push    dx                              ;
                @POLUCH_ADR_MO                          ;
                or      dl, dl                          ; проверим есть место ?
                jnz     @@ksl_no_mo                     ; нет - пропустим
                mov     al, 120                         ; выдать разговор в канал
                stosb                                   ;
                mov     al, SORM_CK                     ;
                stosb                                   ;
                mov     ax, SORM_Work_Num               ;
                stosw                                   ;
                mov     ax, SORM_Mod_Nums               ;
                stosw                                   ;
                @ZAPIS_OK_MO                            ;
                pop     dx                              ;
                pop     es                              ;
                ; ставим коммутацию
                mov     al, bl                          ;
                mov     ah, KSL_Pair[bx]                ;
		push	dx				;
                mov     dx, bp                          ;
                @COMM_SET                               ;
		pop	dx				;
                ; отмечаем занятие КСЛ
                mov     KSL_Busy[bx], dl                ; приоритет из объекта (подключения !)
                shl     bx, 1                           ;
                mov     KSL_Session[bx], si             ;
                ; изменяем параметры наблюдаемого соединения
                mov     WORD PTR es:[si].sorm_ksl_a, ax ;
                mov     ax, SORM_Mod_Nums               ;
                mov     es:[si].sorm_chnl_a, al         ;
                mov     es:[si].sorm_chnl_b, ah         ;
                mov     es:[si].sorm_prior,  dl         ;
                mov     al, es:[si].sorm_get_num        ;
		mov	ah, PEREM_AH			; obj_get_num
                test    al, 3                           ; отбор по №а ?
                jnz     @@new_get_num                   ; ДА -
                shl     ah, 2                           ;
                test    al, 0ch                         ; отбор по №б ?
                jnz     @@new_get_num                   ; ДА -
                shl     ah, 2                           ; отбор по пучку
@@new_get_num:  mov     es:[si].sorm_get_num, ah        ;
                mov     es:[si].sorm_opcode, NEPODKL_OK ;
                ; сообщение "подключение КСЛ"
                cmp     dh, OSV_PRI                     ;
                je      @@exit                          ; при автоматическом подключении не шлём
                mov     dl, SO_POD                      ;
                call    Message_SORM                    ;
@@exit:		popad
		pop	es
		ret
                ; === выходы при неудаче по техническим причинам ==============
@@ksl_no_mo:    ; --- не подключении КСЛ по тех. причинам
                pop     dx                              ;
                pop     es                              ;
                mov     BYTE PTR es:[si].sorm_opcode, NEPODKL_MO    ;!!!
		popad
                pop     es
		ret
@@ksl_no_free_ts:;--- не подключении КСЛ по тех. причинам
                mov     BYTE PTR es:[si].sorm_opcode, NEPODKL_MODEM ;!!!
		popad
                pop     es
		ret
KSL_Get		ENDP

KSL_Get_Far     PROC    FAR
                call    KSL_Get
                retf
                ENDP

;;; ---------------------------------------------------------------------------
;;; --- работа с КСЛ - КОНЕЦ                                                ---
;;; ---------------------------------------------------------------------------
;
