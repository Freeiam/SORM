; === работа с сессиями СОРМ ==================================================
;
Session_Create  PROC
                @Session_Create                                 ;
                jnc     @@exit                                  ;
                ; --- а теперь вызываем СОРМ
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; --- а был ли горячий телефон ?
                @Try_Hot_tel
                stc                                             ;
                ; ---
@@exit:         ret
Session_Create  ENDP

; -== Super Procedure! ==- {используется при переадресации}
; -- Создает все возможные сессии по номеру С как город., так и УПАТС
; -- а также, сообщения использования ДВО для номеров А и Б
; -- In: dl      - признак отбора
;        gs:[bx] - ук-ль на объект
;        fs:[edi]- ук-ль на соед.
;        eax     - кто в соединении ссылаетя на сессию
Session_RA_Create       PROC
                push    eax ecx edx esi
                ; --- {Paul.N} --- [29/05/13]
                @Ra_Nc_sess_create  called_Ua_Nc_ra_ptr
                ; --- {Paul.N} -- [30/05/13]
                ; -- пробуем создать сессию для №С УПАТС
                push    edi
                add     edi, attr_c - attr_b
                @Nb_from_Soed_to_Log          ; сначала преобразуем №С в Nlog
                pop     edi
                ; -- проверяем есть ли номер C в конфигурации АТС
                call    Find_Tel_Num          ; портит edx и esi
                jnc     @@Not_found
                ; --- пробуем найти номер в УПАТС для №Б
                push    es gs
                mov     es, ss:SEG_NLOG
                mov     gs, ss:SEG_KONF
                mov     eax, es:[esi + 4]                   ; абс. физ. № линии в формате СИДС
                mov     esi, eax
                shr     esi, 9                              ; № ЦК и № блока вместе
                and     esi, 7FFh
                shl     esi, 3                              ; на конфигур. блока
                movzx   bx, BYTE PTR gs:[esi]               ; запомним тип блока
                cmp     bx, 0
                jnz     @@Go_far                            ; не наш блок
                mov     esi, gs:[esi + 1]                   ; адрес линий блока
                cmp     esi, Cfg_Free_Ptr
                jae     @@Go_far                            ; вышли за сегмент конфигурации
                and     eax, 1FFh                           ; получим номер линиии
                mov     cl, 4
                test    FG_PROGRAM, _fg_prg_multy_pref
                jz      @@Old_conf
                mov     cl, TAB_TIP_LN[bx]
   @@Old_conf:  shl     eax, cl                             ; ? байт под конфигурацию линии
                add     esi, eax                            ; на конфигурацию линии
                cmp     esi, Cfg_Free_Ptr
                jb      @@get_upats                     ; good ptr!
    @@Go_far:   pop     gs es
                jmp     @@Not_found
                ; --- Прием № УПАТС для абонента C
 @@get_upats:   cmp     WORD PTR gs:[esi + 5], 0ffffh   ;
                je      @@Go_far                        ;
                mov     eax, DWORD PTR gs:[esi + 5]     ; есть номер в УПАТС
                pop     gs
                ; --- теперь проверим наблюдается ли №С - УПАТС
                call    Ux_Find                                 ;
                pop     es
                jnc     @@Not_found                             ; НЕ НАШЛИ
                ; --- Нашли!!!
                mov     dl, 2                                   ; отбор по №c !
                mov     eax, Uc_calling_ra_ptr                  ; ук-ль в соед.
                or      fs:[edi].soed_flags, sf_RA_UPATS_C
                @Ra_Nc_sess_create upats_ca_ra_ptr
                and     fs:[edi].soed_flags, not sf_RA_UPATS_C
  @@Not_found:  pop     esi edx ecx eax
                ; --- {Paul} -- <THE END>
                ; --- если п/а на IP и линия А не БАЛ - получаем абс. физ. номер первичной линии А
                ; п/а на IP ?
  @@check_IP:   ;;push    bx    ; не нужно, т.к. объект подставим в процессе поиска сессий
                test    fs:[edi].soed_flags, sf_IP_A            ;
                jz      @@mess                                  ;
                ; ДА - линия А БАЛ ?
                movzx   ebx, fs:[edi].ck_a                      ; № ЦК
                shl     ebx, 4                                  ; 16 блоков в ЦК
                mov     dx, fs:[edi].phys_a                     ; \ номер
                shr     dx, 9                                   ; / блока
                or      bx, dx                                  ; № ЦК и № блока вместе
                shl     ebx, 3                                  ; 8 байт на конфигурацию блока ЦК
                push    gs                                      ;
                mov     gs, ss:SEG_KONF                         ;
                cmp     BYTE PTR gs:[ebx], 0                    ;
                pop     gs                                      ;
                je      @@mess                                  ;
                ; --- {Paul.N} --- [28/05/2013]
                test    fs:[edi].soed_flags, sf_ANI
                jz      @@mess                                  ; нечего искать
                ; --- {Paul.N} --- [END]
                ; п/а на IP и линия А не БАЛ - ищем первичную линию в TDM
                mov     ebx, SOED_Next                          ;
                mov     cx,  _SOED_MAX                          ;
@@find:   	sub     ebx, _SOED_SIZE                         ;
                jns     @@tst_rel_flag                          ;
                mov     ebx, _SOED_LAST - _SOED_SIZE            ;
@@tst_rel_flag: test    fs:[ebx].soed_flags, sf_REL             ; проверим приём отбоя
                jnz     @@next                                  ;
                mov     eax, DWORD PTR fs:[edi].attr_a          ;
                cmp     eax, DWORD PTR fs:[ebx].attr_a          ;
                jne     @@next                                  ;
                mov     eax, DWORD PTR fs:[edi + 4].attr_a      ;
                cmp     eax, DWORD PTR fs:[ebx + 4].attr_a      ;
                jne     @@next                                  ;
                mov     ax, WORD PTR fs:[edi + 8].attr_a        ;
                cmp     ax, WORD PTR fs:[ebx + 8].attr_a        ;
                jne     @@next                                  ;
                mov     al, fs:[edi + 10].attr_a                ;
                cmp     al, fs:[ebx + 10].attr_a                ;
                je      @@found                                 ;
@@next:         loop    @@find                                  ;
                jmp     @@mess                                  ;
                ; первичная линия А найдена
@@found:        mov     al, fs:[ebx].ck_a                       ;
                mov     es:[si].sorm_ck_a, al                   ;
                mov     ax, fs:[ebx].work_a                     ;
                mov     es:[si].sorm_work_a, ax                 ;
                mov     ax, fs:[ebx].phys_a                     ;
                mov     es:[si].sorm_phys_a, ax                 ;
                ; --- а теперь вызываем СОРМ --------------------
@@mess:         ;;pop     bx                                      ;
                ; --- {Paul.N} --- [29/05/13]
                ; -- поиск сессий
                push    cx bx
                lea     bx, Buff_Sess_RA
                mov     cx, _Sess_RA_MAX
  @@find_sess:  cmp     WORD PTR [bx], 0FFFFh
                jne     @@found_sess
   @@find_far:  add     bx, 2
                loop    @@find_sess
                pop     bx cx
                jmp     @@sess_dvo
  @@found_sess: mov     si, word ptr [bx]                       ; нашли
                push    bx
                mov     bx, es:[si].sorm_obj                    ; возьмем объект
                ; --- создаем сообщение(41)
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                pop     bx
                jmp     @@find_far
  @@sess_dvo:   ; -- и снова поиск сессий
                push    cx bx
                lea     bx, Buff_Sess_RA
                mov     cx, _Sess_RA_MAX
 @@_find_sess:  cmp     WORD PTR [bx], 0FFFFh
                jne     @@_found_sess
  @@_find_far:  add     bx, 2
                loop    @@_find_sess
                pop     bx cx
                jmp     @@exit
@@_found_sess:  mov     si, word ptr [bx]                       ; возьмем ссылку на сессию
                push    bx
                mov     bx, es:[si].sorm_obj                    ; возьмем объект
                ; --- создаем сообщение use dvo(44)
                mov     es:[si].sorm_dvo_phase, PHASE_USE_DVO   ;
                mov     al, fs:[edi].dvo_code                   ;
                mov     es:[si].sorm_dvo_code, al               ;
                or      es:[si].sorm_flags, ssf_SWAP_AB + ssf_RA;
                mov     dl, SO_DVO                              ;
                call    Message_SORM                            ;
                and     es:[si].sorm_flags, not ssf_SWAP_AB
                pop     bx
                mov     word ptr [bx], 0FFFFh                   ; сотрем найденное значение в массиве
                jmp     @@_find_far
                ; --- {Paul.N}  --- [END]
                ; ---
@@exit:         ret
Session_RA_Create       ENDP

; === ДВО: CW, CC, CS =========================================================
;
Session_DVO_A   PROC
                ;
                push    es                                      ;
                pushad                                          ;
                push    ds                                      ;
                pop     es                                      ;
                lea     si, UPATS_Buff                          ;
                mov     eax, DWORD PTR fs:[edi].upats_a_num     ;
                call    Ux_UnPack                               ;
                ;
                lea     si, UPATS_Buff_B                        ;
                mov     eax, DWORD PTR fs:[edi].upats_b_num     ;
                call    Ux_UnPack                               ;
                popad                                           ;
                pop     es                                      ;
                ;
                @Nx_Short   num_a                               ;
                ;
                push    bp
                mov     es, ss:SEG_SORM                         ;
                xor     si, si                                  ;
                mov     dl, SO_DVO                              ;
                mov     cx, _SESSION_MAX                        ;
                mov     bp, WORD PTR fs:[edi].dvo_code          ;
                ;
@@find:         test    es:[si].sorm_flags, ssf_REL             ;
                jnz     @@next                                  ;
                cmp     DWORD PTR es:[si].sorm_num_a, eax       ;
                je      @@cmp_b_full                            ;
                mov     ebx, DWORD PTR UPATS_Buff[2]            ;
                cmp     DWORD PTR es:[si].sorm_num_a, ebx       ;
                jne     @@next                                  ;
                ; --- свое ли соединение нашли по №а (№б тоже наш) ? ---------
@@cmp_b_full:   ; сначала проверяем по полному №б как его набрал абонент
                mov     ebx, DWORD PTR fs:[edi].attr_b          ;
                cmp     ebx, DWORD PTR es:[si].sorm_attr_b      ;
                jne     @@cmp_b_mg                              ;
                mov     ebx, DWORD PTR fs:[edi + 4].attr_b      ;
                cmp     ebx, DWORD PTR es:[si + 4].sorm_attr_b  ;
                jne     @@cmp_b_mg                              ;
                mov     bx, WORD PTR fs:[edi + 8].attr_b        ;
                cmp     bx, WORD PTR es:[si + 8].sorm_attr_b    ;
                jne     @@cmp_b_mg                              ;
                mov     bl, fs:[edi + 10].attr_b                ;
                cmp     bl, es:[si + 10].sorm_attr_b            ;
                jne     @@cmp_b_mg                              ;
                jmp     @@tst_hole                              ;
@@cmp_b_mg:     ; потом проверяем по №б без МГ префикса
                @Nx_Store_and_Del_Pref  len_b                   ;
                jnc     @@cmp_b_upats                           ;
                mov     ebx, DWORD PTR fs:[edi].attr_b          ;
                cmp     ebx, DWORD PTR es:[si].sorm_attr_b      ;
                jne     @@cmp_b_upats                           ;
                mov     ebx, DWORD PTR fs:[edi + 4].attr_b      ;
                cmp     ebx, DWORD PTR es:[si + 4].sorm_attr_b  ;
                jne     @@cmp_b_upats                           ;
                mov     bx, WORD PTR fs:[edi + 8].attr_b        ;
                cmp     bx, WORD PTR es:[si + 8].sorm_attr_b    ;
                jne     @@cmp_b_upats                           ;
                @Nx_Restore  len_b                              ;
                jmp     @@tst_hole                              ;
@@cmp_b_upats:  ; потом проверяем №Б как УПАТС                  ;
                @Nx_Restore  len_b              ; но сначала восстановим №Б
                mov     ebx, DWORD PTR UPATS_Buff_B             ;
                cmp     ebx, DWORD PTR es:[si].sorm_attr_b      ;
                jne     @@next                                  ;
                mov     bl, UPATS_Buff_B[4]                     ;
                cmp     bl, es:[si + 4].sorm_attr_b             ;
                jne     @@next                                  ;
                ; --- проверки слать или не слать конкретное ДВО -------------
@@tst_hole:     ; --- запомним ДВО
		mov	bx, SORM_DVO_Flag			;
		or	es:[si].sorm_flags, bx                  ;
                ; --- защита от HOLE без HOLD                   ;
                cmp     SORM_DVO_Flag, ssf_HOLE                 ;
                jne     @@tst_hold                              ;
                test    es:[si].sorm_flags, ssf_HOLD            ;
                jz      @@next                                  ;
@@tst_hold: 	; --- не слать HOLD/HOLE после CW ---------------
		cmp     SORM_DVO_Flag, ssf_HOLD                 ;
		je	@@tst_hold_cw                           ;
                cmp     SORM_DVO_Flag, ssf_HOLE                 ;
		jne	@@tst_cw                                ;
@@tst_hold_cw:  test    es:[si].sorm_flags, ssf_CW              ;
		jnz	@@next                                  ;
@@tst_cw:       ; --- не слать CW если №а не наблюдается --------
		cmp     SORM_DVO_Flag, ssf_CW                   ;
		jne     @@send_dvo                              ;
		test    es:[si].sorm_get_num, 3                 ;
		jz      @@next                                  ;
@@send_dvo:	; --- отправлем ДВО выполненное абонентом А ------------------
                mov     WORD PTR es:[si].sorm_dvo_code, bp      ;
                call    Message_SORM                            ;
                ; ---
@@next:         add     si, _SORM_SIZE                          ;
                dec     cx                                      ;
                jnz     @@find                                  ;
                pop     bp                                      ;
                ;
                ret
Session_DVO_A   ENDP

Session_DVO_B   PROC
                ;
                push    es                                      ;
                pushad                                          ;
                push    ds                                      ;
                pop     es                                      ;
                lea     si, UPATS_Buff                          ;
                mov     eax, DWORD PTR fs:[edi].upats_a_num     ;
                call    Ux_UnPack                               ;
                ;
                lea     si, UPATS_Buff_B                        ;
                mov     eax, DWORD PTR fs:[edi].upats_b_num     ;
                call    Ux_UnPack                               ;
                popad                                           ;
                pop     es                                      ;
                ;
                @Nx_Short   num_b                               ;
                ;
                push    bp                                      ;
                mov     es, ss:SEG_SORM                         ;
                xor     si, si                                  ;
                mov     dl, SO_DVO                              ;
                mov     cx, _SESSION_MAX                        ;
                mov     bp, WORD PTR fs:[edi].dvo_code          ;
                ;
@@find:         test    es:[si].sorm_flags, ssf_REL             ;
                jnz     @@next                                  ;
if 0            ; -- только в случае работы с IP - станцией(2 ком-ды 112)
                ; --- {Paul.N} --- [22/05/13]
                test    es:[si].sorm_flags, ssf_RA
                jnz     @@go_RA
                cmp     DWORD PTR es:[si].sorm_num_b, eax       ;
                je      @@cmp_a_full                            ;
                jmp     @@go_Upats
     @@go_RA:   cmp     DWORD PTR es:[si].sorm_num_c, eax
                je      @@cmp_a_full
  @@go_Upats:   mov     ebx, DWORD PTR UPATS_Buff_B[2]          ;
                ; --- {Paul.N} --- [END]
endif
                cmp     DWORD PTR es:[si].sorm_num_b, eax       ;
                je      @@cmp_a_full                            ;
                mov     ebx, DWORD PTR UPATS_Buff_B[2]          ;
                cmp     DWORD PTR es:[si].sorm_num_b, ebx       ;
                jne     @@next                                  ;
                ; --- свое ли соединение нашли по №б (№а тоже наш) ? ---------
@@cmp_a_full:   ; сначала совпадает ли полный №а
                mov     ebx, DWORD PTR fs:[edi].attr_a          ;
                cmp     ebx, DWORD PTR es:[si].sorm_attr_a      ;
                jne     @@cmp_a_short                           ;
                mov     ebx, DWORD PTR fs:[edi + 4].attr_a      ;
                cmp     ebx, DWORD PTR es:[si + 4].sorm_attr_a  ;
                jne     @@cmp_a_short                           ;
                mov     bx, WORD PTR fs:[edi + 8].attr_a        ;
                cmp     bx, WORD PTR es:[si + 8].sorm_attr_a    ;
                jne     @@cmp_a_short                           ;
                mov     bl, fs:[edi + 10].attr_a                ;
                cmp     bl, es:[si + 10].sorm_attr_a            ;
                jne     @@cmp_a_short                           ;
                jmp     @@tst_hold                              ;
@@cmp_a_short:  ; потом совпадает ли короткий №а
                push    eax                                     ;
                @Nx_Short  num_a                                ;
                mov     ebx, eax                                ;
                pop     eax                                     ;
                cmp     ebx, DWORD PTR es:[si].sorm_num_a       ;
                je      @@tst_hold                              ;
@@cmp_a_upats:  ; потом не является ли №а УПАТСом
                mov     ebx, DWORD PTR UPATS_Buff               ;
                cmp     ebx, DWORD PTR es:[si].sorm_attr_a      ;
                jne     @@next                                  ;
                mov     bl, UPATS_Buff[4]                       ;
                cmp     bl, es:[si + 4].sorm_attr_a             ;
                jne     @@next                                  ;
                ; --- проверки слать или не слать конкретное ДВО -------------
@@tst_hold:     ; --- запомним ДВО
                mov     bx, SORM_DVO_Flag                       ;
                or      es:[si].sorm_flags, bx                  ;
                ; --- не слать HOLD/HOLE после CW ---------------
		cmp     SORM_DVO_Flag, ssf_HOLD                 ;
		je	@@tst_hold_cw                           ;
                cmp     SORM_DVO_Flag, ssf_HOLE                 ;
		jne	@@tst_hole                              ;
@@tst_hold_cw:  test    es:[si].sorm_flags, ssf_CW              ;
		jnz	@@next                                  ;
@@tst_hole:     ; --- защита от HOLE без HOLD
                cmp     SORM_DVO_Flag, ssf_HOLE                 ;
                jne     @@tst_cw                                ;
                test    es:[si].sorm_flags, ssf_HOLD            ;
                jz      @@next                                  ;
@@tst_cw:       ; --- не слать CW если №б не наблюдается
                cmp     SORM_DVO_Flag, ssf_CW                   ;
                jne     @@send_dvo                              ;
                test    es:[si].sorm_get_num, 0ch               ;
                jz      @@next                                  ;
@@send_dvo:	; --- наконец-то можно слать ДВО
                mov     WORD PTR es:[si].sorm_dvo_code, bp      ;
                movzx   bx, es:[si].sorm_get_num                ;
                mov     dh, SORM_DVO_Get_Num[bx]                ;
                cmp     dh, 0
                jne     @@_dh_ok
                mov     dh, 40h
  @@_dh_ok:     mov     es:[si].sorm_get_num, dh                ;
                test    es:[si].sorm_flags, ssf_RA
                jz      @@not_RA
                or      es:[si].sorm_flags, ssf_SWAP_AC + ssf_SWAP_BA
                call    Message_SORM
                and     es:[si].sorm_flags, not ssf_SWAP_AC + ssf_SWAP_BA
                jmp     @@go_far
  @@not_RA:     or      es:[si].sorm_flags, ssf_SWAP_AB + ssf_SWAP_BA
                call    Message_SORM                            ;
                and     es:[si].sorm_flags, not ssf_SWAP_AB + ssf_SWAP_BA
  @@go_far:     mov     es:[si].sorm_get_num, bl                ;
                ;
@@next:         add     si, _SORM_SIZE                          ;
                dec     cx                                      ;
                jnz     @@find                                  ;
                pop     bp                                      ;
                ;
                ret
Session_DVO_B   ENDP


; === ДВО: 3PTY ===============================================================
;
Session_3PTY_CT_A       PROC
                ;
                push    es                                      ;
                pushad                                          ;
                push    ds                                      ;
                pop     es                                      ;
                lea     si, UPATS_Buff                          ;
                mov     eax, DWORD PTR fs:[edi].upats_a_num     ;
                call    Ux_UnPack                               ;
                ;
                lea     si, UPATS_Buff_B                        ;
                mov     eax, DWORD PTR fs:[edi].upats_b_num     ;
                call    Ux_UnPack                               ;
                popad                                           ;
                pop     es                                      ;
                ;
                @Nx_Short   num_a
                ;
		mov	SORM_3PTY_CT, 0
                mov     es, ss:SEG_SORM
                xor     si, si
                mov     cx, _SESSION_MAX
                ;
@@find:         test    es:[si].sorm_flags, ssf_REL
                jnz     @@next
                ; искомый №а в сессии как №а ?
                cmp     DWORD PTR es:[si].sorm_num_a, eax
                je      @@a_found
                mov     edx, DWORD PTR UPATS_Buff[2]
                cmp     DWORD PTR es:[si].sorm_num_a, edx
                je      @@a_found
                ;
@@next:         add     si, _SORM_SIZE
                loop    @@find
                ;
                ret

                ; --- искомый №а в сессии как №а ! ----------------------------
@@a_found:      ; проверим совпадает ли №б из соединения с №б из сессии
                ; проверка по короткому №б не нужна
                ; сначала проверимся по № УПАТС
                mov     edx, DWORD PTR UPATS_Buff_B
                cmp     DWORD PTR es:[si].sorm_attr_b, edx
                jne     @@cmp_b
                mov     dl, UPATS_Buff_B[4]
                cmp     es:[si + 4].sorm_attr_b, dl
                jne     @@cmp_b
                jmp     @@a_and_b
                ; сначала проверяемся по №б как он есть
   @@cmp_b:     mov     edx, DWORD PTR fs:[edi].attr_b
                cmp     edx, DWORD PTR es:[si].sorm_attr_b
                jne     @@cmp_without
                mov     edx, DWORD PTR fs:[edi + 4].attr_b
                cmp     edx, DWORD PTR es:[si + 4].sorm_attr_b
                jne     @@cmp_without
                mov     dx, WORD PTR fs:[edi + 8].attr_b
                cmp     dx, WORD PTR es:[si + 8].sorm_attr_b
                jne     @@cmp_without
                mov     dl, fs:[edi + 10].attr_b
                cmp     dl, es:[si + 10].sorm_attr_b
                jne     @@cmp_without
                jmp     @@a_and_b
                ; проверяемся по №б без префикса
@@cmp_without:  @Nx_Store_and_Del_Pref   len_b
                jnc     @@a_and_not_b_2
                mov     edx, DWORD PTR fs:[edi].attr_b
                cmp     edx, DWORD PTR es:[si].sorm_attr_b
                jne     @@a_and_not_b
                mov     edx, DWORD PTR fs:[edi + 4].attr_b
                cmp     edx, DWORD PTR es:[si + 4].sorm_attr_b
                jne     @@a_and_not_b
                mov     dx, WORD PTR fs:[edi + 8].attr_b
                cmp     dx, WORD PTR es:[si + 8].sorm_attr_b
                jne     @@a_and_not_b
                mov     dl, fs:[edi + 10].attr_b
                cmp     dl, es:[si + 10].sorm_attr_b
                jne     @@a_and_not_b
                @Nx_Restore  len_b
                ; --- а и б в сессии и в соединении совпали -------------------
@@a_and_b:      ; переносим №б второго соединения в №с сессии
                ; а номер Б - не УПАТС ли ?
                test    fs:[ebx].soed_flags, sf_UPATS_B         ;
                jz      @@a_and_b_log                           ;
                push    es                                      ;
                pushad                                          ;
                push    ds                                      ;
                pop     es                                      ;
                lea     si, UPATS_Buff_C                        ;
                mov     eax, DWORD PTR fs:[ebx].upats_b_num     ;
                call    Ux_UnPack                               ;
                mov     edx, DWORD PTR UPATS_Buff_C             ;
                mov     DWORD PTR fs:[ebx].attr_b, edx          ;
                mov     dl, UPATS_Buff_C[4]                     ;
                mov     fs:[ebx + 4].attr_b, dl                 ;
                mov     DWORD PTR fs:[ebx + 5].attr_b, 0ffffffffh;
                mov     WORD  PTR fs:[ebx + 9].attr_b, 0ffffh   ;
                popad                                           ;
                pop     es                                      ;
                ; собственно перенос
@@a_and_b_log:  mov     edx, DWORD PTR fs:[ebx].attr_b
                mov     DWORD PTR es:[si].sorm_attr_c, edx
                mov     edx, DWORD PTR fs:[ebx + 4].attr_b
                mov     DWORD PTR es:[si + 4].sorm_attr_c, edx
                mov     dx, WORD PTR fs:[ebx + 8].attr_b
                mov     WORD PTR es:[si + 8].sorm_attr_c, dx
                mov     dl, fs:[ebx + 10].attr_b
                mov     es:[si + 10].sorm_attr_c, dl
                @Nx_Session   sorm_attr_c
                ; метим ДВО
                mov     es:[si].sorm_dvo_phase, PHASE_USE_DVO
                mov     dh, fs:[edi].dvo_code
                mov     es:[si].sorm_dvo_code, dh
                ; --- кто в сессии наблюдается А или Б ?
                test    es:[si].sorm_get_num, 3
                jnz     @@a_watch_and_b
@@a_and_b_watch:; в сессии наблюдается вызываемый (A)
                ;     ***
                ; (A) <-- B --> C
                ; меняем признак отбора №
                push    cx
                mov     cl, 4
                test    es:[si].sorm_get_num, 030h
                jz      @@a_and_b_getn
                mov     cl, 0
@@a_and_b_getn: shl     es:[si].sorm_get_num, cl
                ; отправка сообщения
                or      es:[si].sorm_flags, ssf_SWAP_BC
                mov     dl, SO_DVO
                call    Message_SORM
                ; восстанавливаем параметры сессии
                shr     es:[si].sorm_get_num, cl
                pop     cx
                cmp     dh, _3PTY
                je      @@to_next
                test    es:[si].sorm_get_num, 030h
                jnz     @@a_and_b_flags
                shr     es:[si].sorm_get_num, 2
@@a_and_b_flags:or      es:[si].sorm_flags, ssf_SWAP_AB
		call	ReComm_B
                jmp     @@next
@@to_next:      and     es:[si].sorm_flags, not ssf_SWAP_BC
                jmp     @@next

@@a_watch_and_b:; в сессии наблюдается вызывающий (B)
                ;   ***
                ; A <-- (B) --> C
                cmp     fs:[edi].dvo_code, CT
                je      @@a_watch_and_b_CT
                ;;;cmp     SORM_3PTY_CT, 0        |
                ;;;jne     @@next                 |-> ???
                ;;;mov     SORM_3PTY_CT, 255      |
                ; --- {Paul.N} --- <26/02/2013>
                cmp     fs:[edi].dvo_code, _3PTY
                jne     @@send_mess_A
                push    cx bp
                mov     cx, _sorm_3pty_max
                mov     bp, _sorm_3pty_next
 @@find_slot_A: cmp     bp, _sorm_3pty_last
                jb      @@go_find_A
                xor     bp, bp
   @@go_find_A: cmp     Sess_3PTY[bp].session_ptr, 0ffffh
                je      @@slot_ok_A
                add     bp, _sorm_3pty_size
                loop    @@find_slot_A
                pop     bp cx
                ; --- нет свободных слотов, конференция будет работать неправильно
                jmp     @@send_mess_A
@@slot_ok_A:    ; --- заполним структуру 3PTY для данной сессии
                push    eax
                mov     ax, bp
                add     ax, _sorm_3pty_size
                mov     _sorm_3pty_next, ax
                mov     Sess_3PTY[bp].session_ptr, si           ; ptr to session
                mov     Sess_3PTY[bp].perem_ebx_3pty, ebx       ; prt to 2-d soed
                mov     eax, DWORD PTR es:[si].sorm_num_a
                mov     Sess_3PTY[bp].sorm_numb_3pty, eax       ; subscriber of 3PTY
                pop     eax
                pop     bp cx
                ; --- <End - Paul.N> ---
                ; отправка сообщения
 @@send_mess_A: or      es:[si].sorm_flags, ssf_SWAP_BC
                mov     dl, SO_DVO
                call    Message_SORM
                ; восстанавливаем параметры сессии
                and     es:[si].sorm_flags, not ssf_SWAP_BC
                jmp     @@next
@@a_watch_and_b_CT:
                mov     es:[si].sorm_opcode, PRI_RAZ    ;
                xor     dh, dh                          ;
                call    KSL_Free                        ;
                mov     dl, SO_RAZ
                call    Message_SORM
                or      es:[si].sorm_flags, ssf_REL     ;
                jmp     @@next

                ; --- а в сессии и в соединении совпадает, а б нет ------------
@@a_and_not_b:  @Nx_Restore  len_b
@@a_and_not_b_2:; переносим №б первого соединения в №с сессии
                ; а номер Б - не УПАТС ли ?
                test    fs:[edi].soed_flags, sf_UPATS_B         ;
                jz      @@a_and_not_b_log                       ;
                push    es                                      ;
                pushad                                          ;
                push    ds                                      ;
                pop     es                                      ;
                lea     si, UPATS_Buff_C                        ;
                mov     eax, DWORD PTR fs:[edi].upats_b_num     ;
                call    Ux_UnPack                               ;
                mov     edx, DWORD PTR UPATS_Buff_C             ;
                mov     DWORD PTR fs:[edi].attr_b, edx          ;
                mov     dl, UPATS_Buff_C[4]                     ;
                mov     fs:[edi + 4].attr_b, dl                 ;
                mov     DWORD PTR fs:[edi + 5].attr_b, 0ffffffffh;
                mov     WORD  PTR fs:[edi + 9].attr_b, 0ffffh   ;
                popad                                           ;
                pop     es                                      ;
                ; собственно перенос
@@a_and_not_b_log:
                mov     edx, DWORD PTR fs:[edi].attr_b
                mov     DWORD PTR es:[si].sorm_attr_c, edx
                mov     edx, DWORD PTR fs:[edi + 4].attr_b
                mov     DWORD PTR es:[si + 4].sorm_attr_c, edx
                mov     dx, WORD PTR fs:[edi + 8].attr_b
                mov     WORD PTR es:[si + 8].sorm_attr_c, dx
                mov     dl, fs:[edi + 10].attr_b
                mov     es:[si + 10].sorm_attr_c, dl
                @Nx_Session   sorm_attr_c
                ; метим ДВО
                mov     es:[si].sorm_dvo_phase, PHASE_USE_DVO
                mov     dh, fs:[edi].dvo_code
                mov     es:[si].sorm_dvo_code, dh
                ; --- кто в сессии наблюдается А или Б ?
                test    es:[si].sorm_get_num, 3
                jnz     @@a_watch_and_not_b
@@a_and_not_b_watch:; в сессии наблюдается вызываемый (C)
                ;        ***
                ; A <-- B --> (C)
                ; меняем признак отбора №
                push    cx
                mov     cl, 4
                test    es:[si].sorm_get_num, 030h
                jz      @@a_and_not_b_get
                mov     cl, 0
@@a_and_not_b_get:
                shl     es:[si].sorm_get_num, cl
                ; отправка сообщения
                or      es:[si].sorm_flags, ssf_SWAP_BC
                mov     dl, SO_DVO
                call    Message_SORM
                ; восстанавливаем параметры сессии
                shr     es:[si].sorm_get_num, cl
                pop     cx
                and     es:[si].sorm_flags, not ssf_SWAP_BC
                cmp     dh, _3PTY
		je	@@next
                or      es:[si].sorm_flags, ssf_SWAP_AC
		call	ReComm_B
                jmp     @@next

@@a_watch_and_not_b:; в сессии наблюдается вызывающий (B) - ничего не делаем
                ;           ***
                ; A <-- (B) --> C
                ; отправка сообщения
                cmp     fs:[edi].dvo_code, _3PTY
                jne     @@normal_3pty_A;;;@@next
                or      FG_SORM4, _3_pty_sorm             ; отметим эту сессию
                jmp     @@3pty_A_rel
                ; отправка сообщения
@@normal_3pty_A:
                or      es:[si].sorm_flags, ssf_SWAP_BC
                mov     dl, SO_DVO
                call    Message_SORM
                ; восстанавливаем параметры сессии
                and     es:[si].sorm_flags, not ssf_SWAP_BC
@@3pty_A_rel:   mov     es:[si].sorm_opcode, PRI_RAZ    ;
                xor     dh, dh                          ;
                call    KSL_Free                        ;
                mov     dl, SO_RAZ
                call    Message_SORM
                or      es:[si].sorm_flags, ssf_REL     ;
                jmp     @@next
Session_3PTY_CT_A       ENDP

Session_3PTY_CT_B       PROC
                ;
                push    es                                      ;
                pushad                                          ;
                push    ds                                      ;
                pop     es                                      ;
                lea     si, UPATS_Buff_B                        ;
                mov     eax, DWORD PTR fs:[edi].upats_b_num     ;
                call    Ux_UnPack                               ;
                popad                                           ;
                pop     es                                      ;
                ;
                @Nx_Short   num_b                               ;
                ;
                ; --- Поехали  ---
                mov     es, ss:SEG_SORM                         ;
                xor     si, si                                  ;
                mov     cx, _SESSION_MAX                        ;
                ;
@@find:         test    es:[si].sorm_flags, ssf_REL             ;
                jnz     @@next                                  ;
                ; искомый №б в сессии как №б ?
                cmp     DWORD PTR es:[si].sorm_num_b, eax       ;
                je      @@b_found                               ;
                ; искомый №б в сессии как №а ?
                cmp     DWORD PTR es:[si].sorm_num_a, eax       ;
                je      @@a_found                               ;
                ;
                mov     edx, DWORD PTR UPATS_Buff_B[2]          ;
                cmp     DWORD PTR es:[si].sorm_num_b, edx       ;
                je      @@b_found                               ;
                cmp     DWORD PTR es:[si].sorm_num_a, edx       ;
                je      @@a_found                               ;
                ;
@@next:         add     si, _SORM_SIZE                          ;
                loop    @@find                                  ;
                ;
                ret

                ; --- искомый №б в сессии как №б ! ----------------------------
@@b_found:      ; а номер Б во 2 -ом соед. - не УПАТС ли ?
                test    fs:[ebx].soed_flags, sf_UPATS_B         ;
                jz      @@b_log                                 ;
                push    es                                      ;
                pushad                                          ;
                push    ds                                      ;
                pop     es                                      ;
                lea     si, UPATS_Buff_C                        ;
                mov     eax, DWORD PTR fs:[ebx].upats_b_num     ;
                call    Ux_UnPack                               ;
                mov     edx, DWORD PTR UPATS_Buff_C             ;
                mov     DWORD PTR fs:[ebx].attr_b, edx          ;
                mov     dl, UPATS_Buff_C[4]                     ;
                mov     fs:[ebx + 4].attr_b, dl                 ;
                mov     DWORD PTR fs:[ebx + 5].attr_b, 0ffffffffh;
                mov     WORD  PTR fs:[ebx + 9].attr_b, 0ffffh   ;
                popad                                           ;
                pop     es                                      ;
                ; переносим №б второго соединения в №с сессии
     @@b_log:   mov     edx, DWORD PTR fs:[ebx].attr_b          ;
                mov     DWORD PTR es:[si].sorm_attr_c, edx      ;
                mov     edx, DWORD PTR fs:[ebx + 4].attr_b      ;
                mov     DWORD PTR es:[si + 4].sorm_attr_c, edx  ;
                mov     dx, WORD PTR fs:[ebx + 8].attr_b        ;
                mov     WORD PTR es:[si + 8].sorm_attr_c, dx    ;
                mov     dl, fs:[ebx + 10].attr_b                ;
                mov     es:[si + 10].sorm_attr_c, dl            ;
                @Nx_Session   sorm_attr_c                       ;
                ; метим ДВО
                mov     es:[si].sorm_dvo_phase, PHASE_USE_DVO   ;
                mov     dh, fs:[edi].dvo_code                   ;
                mov     es:[si].sorm_dvo_code, dh               ;
                ; кто в сессии наблюдается А, Б или пучок ?
                test    es:[si].sorm_get_num, 030h              ;
                jnz     @@b_and_a                               ;
                test    es:[si].sorm_get_num, 3                 ;
                jnz     @@b_and_a                               ;
@@b_and_b:      ; в сессии наблюдается вызываемый (B)
                ;   ***
                ; A --> (B) --> C
                cmp     fs:[edi].dvo_code, CT                   ;
                je      @@b_and_b_CT                            ;
                ; --- {Paul.N} --- <26/02/2013>
                cmp     fs:[edi].dvo_code, _3PTY
                jne     @@send_mess
                push    cx bp
                mov     cx, _sorm_3pty_max
                mov     bp, _sorm_3pty_next
 @@find_slot:   cmp     bp, _sorm_3pty_last
                jb      @@go_find
                xor     bp, bp
   @@go_find:   cmp     Sess_3PTY[bp].session_ptr, 0ffffh
                je      @@slot_ok
                add     bp, _sorm_3pty_size
                loop    @@find_slot
                pop     bp cx
                ; --- нет свободных слотов, конференция будет работать не правильно
                jmp     @@send_mess
@@slot_ok:      ; --- заполним структуру 3PTY для данной сессии
                push    eax
                mov     ax, bp
                add     ax, _sorm_3pty_size
                mov     _sorm_3pty_next, ax
                mov     Sess_3PTY[bp].session_ptr, si           ; ptr to session
                mov     Sess_3PTY[bp].perem_ebx_3pty, ebx       ; ptr to 2-d soed
                mov     eax, DWORD PTR es:[si].sorm_num_b       ; №Б from 1-st soed. sess.
                mov     Sess_3PTY[bp].sorm_numb_3pty, eax       ; subscriber of 3pty
                pop     eax
                pop     bp cx
                ; --- <End - Paul.N> ---
                ; отправка сообщения
  @@send_mess:  mov     dh, es:[si].sorm_get_num                ;
                mov     dl, dh                                  ;
                shr     dl, 2                                   ;
                test    dh, 030h                                ;
                jz      @@b_and_b_getn                          ;
                shl     dl, 0                                   ;
@@b_and_b_getn: mov     es:[si].sorm_get_num, dl                ;
                or      es:[si].sorm_flags, ssf_SWAP_AB + ssf_SWAP_BC
                mov     dl, SO_DVO                              ;
                call    Message_SORM                            ;
                ; восстанавливаем параметры сессии
                and     es:[si].sorm_flags, not ssf_SWAP_AB + ssf_SWAP_BC
                mov     es:[si].sorm_get_num, dh                ;
                jmp     @@next                                  ;
@@b_and_b_CT:   mov     es:[si].sorm_opcode, PRI_RAZ            ;
                xor     dh, dh                                  ;
                call    KSL_Free                                ;
                mov     dl, SO_RAZ                              ;
                call    Message_SORM                            ;
                or      es:[si].sorm_flags, ssf_REL             ;
                jmp     @@next                                  ;

@@b_and_a:      ; в сессии наблюдается вызывающий (A)
                ;     ***
                ; (A) --> B --> C
                ; меняем признак отбора №
                push    cx
                mov     cl, 6
                test    es:[si].sorm_get_num, 030h
                jz      @@b_and_a_getn
                mov     cl, 0
@@b_and_a_getn: shl     es:[si].sorm_get_num, cl
                ; отправка сообщения
                or      es:[si].sorm_flags, ssf_SWAP_AB + ssf_SWAP_BC
                mov     dl, SO_DVO
                call    Message_SORM
                ; восстанавливаем параметры сессии
                shr     es:[si].sorm_get_num, cl
                pop     cx
                and     es:[si].sorm_flags, not ssf_SWAP_AB
                cmp     dh, CT
                je      @@recomm
                and     es:[si].sorm_flags, not ssf_SWAP_BC
                jmp     @@next
@@recomm:       call    ReComm
                jmp     @@next

                ; --- искомый №б в сессии как №а ! ----------------------------
@@a_found:      ; а номер А в 1-ом соед. - не УПАТС ли ?
                test    fs:[edi].soed_flags, sf_UPATS_A         ;
                jz      @@a_log                                 ;
                push    es                                      ;
                pushad                                          ;
                push    ds                                      ;
                pop     es                                      ;
                lea     si, UPATS_Buff_C                        ;
                mov     eax, DWORD PTR fs:[edi].upats_a_num     ;
                call    Ux_UnPack                               ;
                mov     edx, DWORD PTR UPATS_Buff_C             ;
                mov     DWORD PTR fs:[edi].attr_a, edx          ;
                mov     dl, UPATS_Buff_C[4]                     ;
                mov     fs:[edi + 4].attr_a, dl                 ;
                mov     DWORD PTR fs:[edi + 5].attr_a, 0ffffffffh;
                mov     WORD  PTR fs:[edi + 9].attr_a, 0ffffh   ;
                popad                                           ;
                pop     es                                      ;
                ; переносим №а первого соединения в №с сессии
@@a_log:        mov     edx, DWORD PTR fs:[edi].attr_a
                mov     DWORD PTR es:[si].sorm_attr_c, edx
                mov     edx, DWORD PTR fs:[edi + 4].attr_a
                mov     DWORD PTR es:[si + 4].sorm_attr_c, edx
                mov     dx, WORD PTR fs:[edi + 8].attr_a
                mov     WORD PTR es:[si + 8].sorm_attr_c, dx
                mov     dl, fs:[edi + 10].attr_a
                mov     es:[si + 10].sorm_attr_c, dl
                @Nx_Session   sorm_attr_c
                ; метим ДВО
                mov     es:[si].sorm_dvo_phase, PHASE_USE_DVO
                mov     dh, fs:[edi].dvo_code
                mov     es:[si].sorm_dvo_code, dh
                ; кто в этой сессии наблюдается А или Б ?
                test    es:[si].sorm_get_num, 3
                jnz     @@a_and_a
                ; меняем признак отбора №
@@a_and_b:      ; в сессии наблюдается вызываемый (С)
                ;         ***
                ; A --> B --> (C)
		push    cx
                mov     cl, 4
                test    es:[si].sorm_get_num, 030h
                jz      @@a_and_b_get
                mov     cl, 0
@@a_and_b_get:  shl     es:[si].sorm_get_num, cl
                ; отправка сообщения
                or      es:[si].sorm_flags, ssf_SWAP_BC
                mov     dl, SO_DVO
                call    Message_SORM
                ; восстанавливаем параметры сессии
                shr     es:[si].sorm_get_num, cl
                pop     cx
                and     es:[si].sorm_flags, not ssf_SWAP_BC
                cmp     dh, _3PTY
                je      @@next
                or      es:[si].sorm_flags, ssf_SWAP_AC
		call	ReComm_B
                jmp     @@next
@@a_and_a:      ; в сессии наблюдается вызывающий (B) - ничего не делаем
                ;           ***
                ; A --> (B) --> C
                cmp     fs:[edi].dvo_code, _3PTY
                jne     @@normal_3pty_B ;;;@@next
                or      FG_SORM4, _3_pty_sorm             ; отметим эту сессию
                jmp     @@3pty_B_rel
                ; отправка сообщения
@@normal_3pty_B:
                or      es:[si].sorm_flags, ssf_SWAP_BC
                mov     dl, SO_DVO
                call    Message_SORM
                ; восстанавливаем параметры сессии
                and     es:[si].sorm_flags, not ssf_SWAP_BC
@@3pty_B_rel:   mov     es:[si].sorm_opcode, PRI_RAZ    ;
                xor     dh, dh                          ;
                call    KSL_Free                        ;
                mov     dl, SO_RAZ
                call    Message_SORM
                or      es:[si].sorm_flags, ssf_REL     ;
                jmp     @@next
                ;
Session_3PTY_CT_B       ENDP






