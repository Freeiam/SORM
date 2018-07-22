                ; === инициализация данных СОРМ =========================================
@SORM_INIT_DATA MACRO
                LOCAL   @@init_soed, @@init_comm
                ; --- инициализация буфера сообщений 1-го канала
                mov     PER_SORM, 0
                mov     POS_SORM, 0
                ; --- инициализация буфера сообщений 2-го канала
                mov     SLED_SOOB, 0
                mov     POS_SOOB,  0
                ; --- восстановление пароля по умолчанию
                mov     eax, OLD_PAROL_0_3
                mov     PAROL_0_3, eax
                mov     ax, OLD_PAROL_4_5
                mov     PAROL_4_5, ax
                ; --- нумерация вызовов СОРМ
                mov     SORM_Call_Num, 0
                mov     SORM_Curr_Call, 0ffffh
                ; --- очистка таблиц КСЛ
                push    ds
                pop     es
                mov     eax, 0ffffffffh
                mov     cx, _KSL_DATA_SIZE
                lea     di, KSL_Group
                rep     stosb
                ; --- очистка таблиц объектов
                mov     Obj_Count, 0            ; для телефонов только
                ;
                mov     es, ss:SEG_OBJ
                mov     cx, _OBJ_SIZE * (_BUNCH_MAX + _OBJ_MAX) - 1
                xor     di, di
                rep     stosb
                ; --- очистка сессий
                mov     es, ss:SEG_SORM
                mov     cx, _SESSION_MAX * _SORM_SIZE
                shr     cx , 2
                xor     di, di
                rep     stosd
                ; --- очистка сегмента соединений от ссылок на объекты СОРМ
                mov     es, ss:SEG_SOED
                mov     cx, _SOED_MAX
                xor     edi, edi
@@init_soed:    mov     es:[edi].bunch_a_ptr,       ax
                mov     es:[edi].bunch_b_ptr,       ax
                mov     es:[edi].calling_ptr,       ax
                mov     es:[edi].calling_short_ptr, ax
                mov     es:[edi].called_ptr,        ax
                mov     es:[edi].called_short_ptr,  ax
                mov     es:[edi].called_mg_ptr,     ax
                mov     es:[edi].ra_ptr,            ax
                mov     es:[edi].ra_mg_ptr,         ax
                mov     es:[edi].bunch_ra_ptr,      ax
                mov     es:[edi].upats_ab_ptr,      ax
                mov     es:[edi].upats_ba_ptr,      ax
                mov     es:[edi].Ub_calling_ptr,    ax
                mov     es:[edi].Ua_called_ptr,     ax
                mov     es:[edi].called_Ua_ptr,     ax
                mov     es:[edi].called_mg_Ua_ptr,  ax
                mov     es:[edi].calling_Ub_ptr,    ax
                mov     es:[edi].called_Ua_Nc_ra_ptr, ax
                mov     es:[edi].Uc_calling_ra_ptr, ax
                mov     es:[edi].upats_ca_ra_ptr,   ax
                add     edi, _SOED_SIZE
                loop    @@init_soed
                ; --- очистка коммутации
                mov     es, ss:SEG_ST
IF _MSP EQ TRUE
                mov     cx, 55;;;!!!28;;;_MAX_E1 * 30
ELSE
                mov     cx, _MAX_E1 * 30
ENDIF
IF _MSP EQ TRUE
                mov     si, ADR_OZU_K1 + ADD_4_KAN
@@init_comm:    mov     BYTE PTR es:[si], 0c0h
                add     si, 4
                loop    @@init_comm
ELSE
                mov     si, ADR_OZU_K1 + 256 + 1   ; в ИКМ0
@@init_comm:    mov     BYTE PTR es:[si], 80h
                add     si, 2
                loop    @@init_comm
ENDIF
                ; --- {Paul.N} --- <01/04/2013>
                ; -- очистим массив структур под сессии СОРМ(3РТУ)
                push    es
                mov     es, ss:SEG_AX
                lea     di, Sess_3PTY
                mov     cx, _sorm_3pty_last
                rep     stosb
                pop     es
                ; --- {Paul.N} --- <END>
                ; ---
                ENDM


; === работа с коммутатором =============================================================

; --- проключение ЦТО -------------------------------------------------------------------
;
@COMM_CTO       MACRO
IF _MSP EQ TRUE
                mov     es, ss:SEG_ST
                mov     di, ADR_OZU_K1 + 12
                mov     ax, CHAN_FROM_CTO               ; AL - канал ОЗУБ приема от ЦТО
                or      al, 0e0h                        ; Коммутация "Вх.рег-р в ОЗУБ"
                stosb
                xor     al, al
                stosb
                mov     ax, CHAN_TO_CTO                 ; AL - канал ОЗУБ выдачи в ЦТО
                or      al, 080h                        ; Коммутация "ОЗУБ в Вых.рег-р"
                stosb
ELSE
                ; в ЦТО
                mov     fs, ss:SEG_ST
                mov     al, _BABA_TO_CTO                ; al - канал ОЗУБ выдачи в ЦТО
                mov     ah, 7                           ; номер порта коммутатора - БАБА
                mov     fs:[ADR_OZU_K1 + 6], al         ; \ в третий слот модема
                mov     fs:[ADR_OZU_K1 + 7], ah         ; / из указанного канала БАБЫ
                ; от ЦТО
                mov     di, _BABA_FROM_CTO              ; канал ОЗУБ для приёма ЦТО
                shl     di, 1                           ; ОЗУ коммутации словами
                add     di, ADR_OZU_K1 + 256 * 7        ; сдвигаемся от начала ОЗУК на БАБу
                mov     al, 3                           ; номер слота в модеме
                xor     ah, ah                          ; номер порта коммутатора - модем
                mov     fs:[di],     al                 ; \ в указанный канал БАБы
                mov     fs:[di + 1], ah                 ; / с третьего слота модема
ENDIF
                ENDM

; --- начальная установка коммутации для СОРМа ------------------------------------------
;
@COMM_INIT      MACRO
                LOCAL   @@loop_baba, @@loop_pcm
IF _MSP EQ TRUE
                mov     es, ss:SEG_ST
                mov     di, ADR_OZU_K1 + _MODEM_1ST_TS * 4
                lea     si, BUF_R_BABA
                mov     cx, 28
@@loop_baba:    movsb
                movsb
                add     di, 2
                loop    @@loop_baba
ELSE
                ; --- коммутация из модема в БАБу
                mov     es, ss:SEG_ST
                mov     di, ADR_OZU_K1 + 256 * 7        ; БАБА !
		add     di, _COMM_1ST_BABA * 2          ;;;<mmm>
                mov     ax, _MODEM_1ST_TS               ; начальный канал модема   (al=60,ah=0)
                mov     cx, _COMM_CHNLS                 ; число каналов            (14)
@@loop_baba:    mov     es:[di],     al                 ; \ в канал БАБЫ
                mov     es:[di + 1], ah                 ; / с канала модема (А)
                inc     al                              ;
                mov     es:[di + _COMM_AB_SHIFT * 2],     al ; \ в канал БАБЫ плюс сдвиг
                mov     es:[di + _COMM_AB_SHIFT * 2 + 1], ah ; / со следующего канала модема (Б)
                inc     al                              ;
                add     di, 2                           ; на обработку след. пары модемных каналов
                loop    @@loop_baba
                ; --- коммутации в ИКМ пока нет
                mov     ax, 8000h                       ; код нет коммутации
                mov     cx, _MAX_E1 * 30                ; по всем слотам всех ИКМ-30
                mov     di, ADR_OZU_K1 + 256 * 1        ;
@@loop_pcm:     mov     es:[di],     al
		mov	es:[di + 1], ah                 ; запрещаем коммутацию
                add     di, 2                           ; на следующий канал
                loop    @@loop_pcm
ENDIF
                ENDM

; --- программное суммирование ТЧ для совмещённых каналов -------------------------------
;
@COMM_SUM_AB    MACRO
		LOCAL   @@end, @@next_chnl, @@sum_chnl, @@skip

                push    fs
                mov     es, ss:SEG_BABA
                mov     fs, ss:SEG_AX
                mov     ds, ss:SEG_BUS
                ASSUME  DS:DATA_BUS
                mov     dx, _MAX_MODEM_TS - 1
                mov     bp, POINT_R_W
                add     bp, (_COMM_1ST_BABA + _COMM_SUM_SHIFT) * 512
		; ---
@@next_chnl:    mov     di, dx
                cmp     fs:Modem_KSL[di], 0ffh
                je      @@skip
		; ---
                shld    di, dx, 25
                add     di, bp
                mov     cx, 16
@@sum_chnl:     movzx   bx, es:[di - _COMM_SUM_SHIFT * 512]
                shl     bx, 1
                mov     si, TAB_AL[bx]
                movzx   bx, es:[di - _COMM_AB_SHIFT * 512]
                shl     bx, 1
                add     si, TAB_AL[bx]
                mov     al, TAB_L[si];
                stosb
                loop    @@sum_chnl
		; ---
@@skip:         dec     dx
                jns     @@next_chnl
                add     POINT_R_W, 16
                cmp     POINT_R_W, 320
                jb      @@end
                mov     POINT_R_W, 0
@@end:          ; ---
                pop     fs
                ENDM


; --- преобразование номера КСЛ 1..28 в номер тайи-слота 0,,15 и 17,,29
; вход:         al - номера КСЛ
; выход:        dh - номер тайм-слота в ИКМ
@KSL_TO_TS      MACRO
                mov     dh, al                  ;
                shr     dh, 4                   ;
                add     dh, al                  ;
                ENDM

; --- установка коммутации в сторону ФСБ ------------------------------------------------
; вход:         ax - номера КСЛ-А и КСЛ-Б
;               dl - номер канала БАБы коммутируемого в ИКМ
;
@COMM_SET       MACRO
                LOCAL   @@combined, @@separate, @@exit
                push    es
                push    ax
                push    dx
                push    di
                ; ---
                mov     es, ss:SEG_ST
                ; --- что ?
IF _MSP EQ TRUE
                add     dh, _nom_ikm                    ; !!!! для работы с другим потоком !!!
ENDIF
                cmp     ah, 0ffh
                je      @@combined
IF _MSP EQ TRUE
                ; --- раздельное наблюдение
@@separate:     ; А
                movzx   di, al                          ; номер КСЛ-А
                dec     di
                shl     di, 3                           ; * 8 т.к. коммутация два двойных слова
                add     di, ADR_OZU_K1 + ADD_4_KAN      ; + база
                or      dl, 80h                         ; метка канала БАБЫ (каналы 0...15)
                mov     es:[di],     dl                 ; читаем из этого канала БАБЫ
                @KSL_TO_TS                              ; преобразуем № КСЛ-А в № ТС
                mov     es:[di + 1], dh                 ; и пишем в полученный ТС ИКМа
                ; Б
                add     di, 4                           ; следующее DD коммутации
                or      dl, 90h                         ; метка канала БАБЫ (каналы 16...31)
                mov     es:[di],     dl                 ; читаем из этого канала БАБЫ
                mov     al, ah                          ; \ преобразуем № КСЛ-Б
                @KSL_TO_TS                              ; / в № ТС
                mov     es:[di + 1], dh                 ; и пишем в полученный ТС ИКМа
                jmp     @@exit
                ; --- совмещённое наблюдение
@@combined:     movzx   di, al                          ;
                dec     di
                shl     di, 3                           ; * 8 т.к. коммутация два двойных слова
                add     di, ADR_OZU_K1 + ADD_4_KAN      ; + база
                or      dl, 0a0h                        ; метка канала БАБЫ (каналы 32...47)
                mov     es:[di],     dl                 ; читаем из этого канала БАБЫ
                @KSL_TO_TS                              ; преобразуем № КСЛ-А в № ТС
                mov     es:[di + 1], dh                 ; и пишем в полученный ТС ИКМа
ELSE
                ; --- раздельное наблюдение
@@separate:     ; А
                cmp	al, 16   			;
		jb	@@sepr_lo_a                     ;
		inc     al				;
@@sepr_lo_a:    movzx   di, al                          ; номер КСЛ-А
                shl     di, 1                           ; *2 т.к. коммутация словами
                add     di, ADR_OZU_K1 + 256            ; в ИКМ-30 из
		add     dl, _COMM_1ST_BABA              ;
                mov     es:[di],              dl        ;   этого канала
                mov     BYTE PTR es:[di + 1], 7         ;   БАБЫ
                ; Б
                cmp	ah, 16   			;
		jb	@@sepr_lo_b                     ;
		inc     ah				;
@@sepr_lo_b:    movzx   di, ah                          ; номер КСЛ-Б
                shl     di, 1                           ;
                add     di, ADR_OZU_K1 + 256            ;
                add     dl, _COMM_AB_SHIFT              ;
                mov     es:[di],              dl        ;
                mov     BYTE PTR es:[di + 1], 7         ;
                jmp     @@exit
                ; --- совмещённое наблюдение
@@combined:     cmp	al, 16   			;
		jb	@@comb_lo                       ;
		inc     al				;
@@comb_lo:      movzx   di, al                          ;
                shl     di, 1                           ;
                add     di, ADR_OZU_K1 + 256            ;
                add     dl, _COMM_1ST_BABA + _COMM_SUM_SHIFT ;
                mov     es:[di],              dl        ;
                mov     BYTE PTR es:[di + 1], 7         ;
ENDIF
                ; ---
@@exit:         pop     di
                pop     dx
                pop     ax
                pop     es
                ENDM


; --- снятие коммутации в сторону ФСБ ---------------------------------------------------
; вход:         ax - номера КСЛ-А и КСЛ-Б
;
@COMM_RESET     MACRO
                LOCAL   @@combined, @@separate, @@exit
                ; ---
                push    es
                push    ax
                push    di
                mov     es, ss:SEG_ST
                ; --- что ?
                cmp     ah, 0ffh
                je      @@combined
IF _MSP EQ TRUE
                ; --- раздельное наблюдение
@@separate:     ; А
                movzx   di, al                          ; номер КСЛ-А
                dec     di
                shl     di, 3                           ; * 8 т.к. коммутация два двойных слова
                add     di, ADR_OZU_K1 + ADD_4_KAN      ; + база
                mov     BYTE PTR es:[di], 0c0h          ; запрещаем выдачу в ИКМ
                ; Б
                add     di, 4                           ; следующее DD коммутации
                mov     BYTE PTR es:[di], 0c0h          ; запрещаем выдачу в ИКМ
                jmp     @@exit
                ; --- совмещённое наблюдение
@@combined:     movzx   di, al                          ;
                dec     di
                shl     di, 3                           ; * 8 т.к. коммутация два двойных слова
                add     di, ADR_OZU_K1 + ADD_4_KAN      ; + база
                mov     BYTE PTR es:[di], 0c0h          ; запрещаем выдачу в ИКМ
ELSE
                ; --- раздельное наблюдение
@@separate:     ; А
                cmp	al, 16   			;
		jb	@@sepr_lo_a                     ;
		inc     al				;
@@sepr_lo_a:    movzx   di, al                          ; номер КСЛ-А
                shl     di, 1                           ; *2 т.к. коммутация словами
                add     di, ADR_OZU_K1 + 256 + 1        ; в этот тайм-слот ИКМ-30
                mov     BYTE PTR es:[di], 80h           ;   выдачи нет
                ; Б
                cmp	ah, 16   			;
		jb	@@sepr_lo_b                     ;
		inc     ah				;
@@sepr_lo_b:    movzx   di, ah                          ; номер КСЛ-Б
                shl     di, 1                           ;
                add     di, ADR_OZU_K1 + 256 + 1        ;
                mov     BYTE PTR es:[di], 80h           ;
                jmp     @@exit
                ; --- совмещённое наблюдение
@@combined:     cmp	al, 16   			;
		jb	@@comb_lo                       ;
		inc     al				;
@@comb_lo:      movzx   di, al                          ;
                shl     di, 1                           ;
                add     di, ADR_OZU_K1 + 256 + 1        ;
                mov     BYTE PTR es:[di], 80h           ;
ENDIF
                ; ---
@@exit:         pop     di
                pop     ax
                pop     es
                ENDM




; --- вспомогательные макросы, используемые пр формировании сообщений -------------------
;
@MSG_TIME       MACRO
		PUSH	DX
		MOV	AL,DEN_MES
		MOV	DL,AL
		SHR	AL,4
		SHL	DL,4
		OR	AL,DL
		@_STOSB
		MOV	AL,DEN_MES[3]
		MOV	DL,AL
		SHR	AL,4
		SHL	DL,4
		OR	AL,DL
		@_STOSB
		MOV	AL,DEN_MES[2]
		MOV	DL,AL
		SHR	AL,4
		SHL	DL,4
		OR	AL,DL
		@_STOSB
		MOV	AL,DEN_MES[1]
		MOV	DL,AL
		SHR	AL,4
		SHL	DL,4
		OR	AL,DL
		@_STOSB
		POP	DX
		ENDM

@SOOB_ODNO      MACRO
                MOV     EAX,10001H
                @_STOSD
                MOV     AX,0FFFFH
                @_STOSW
                ENDM


@Value_to_ddebug macro PAR1 ; MVT
		push	eax
		mov	eax,PAR1
		DB	 9Ah
		DW	 OFFSET	value_to_ddebug_proc
		DW	 120
		pop	eax
		endm

; MVT запись величины PAR2 в переменную отладки PAR1
@Value_to_dd	macro PAR1,PAR2
		push	ds
		mov	ds,SS:SEG_AX
		mov	PAR1,PAR2
		pop	ds
		endm

; MVT инкремент переменной отладки PAR
@inc_dd		macro PAR
		push	ds
		mov	ds,SS:SEG_AX
		inc	PAR
		pop	ds
		endm
if _MSP NE TRUE
; MVT запись в отладочный файл строки
; Запись производится при DebugFileOpened =/= 0
; Параметрами является адрес записываемой в файл строки и количество записываемых байт
@String_to_file macro PAR_SEG, PAR_OFF, PAR_COUNT
		push	ds edx ecx
		push	PAR_SEG
		pop	ds
		mov	edx, PAR_OFF
		mov	ecx, PAR_COUNT
		call	write_to_debug_file
		pop	ecx edx ds
		endm

; MVT запись в отладочный файл трассировки
; Запись производится при DebugFileOpened =/= 0
; Параметром является номер трассируемой точки от 0 до 9
@ici_to_file	macro PAR
		push	edx
		mov	edx, PAR
		call	write_ici_to_debug_file
		pop	edx
		endm

; MVT запись двойного слова в отладочный файл
; Запись производится при DebugFileOpened =/= 0
; Параметры: двойное слово и количество сохраняемых тетрад (1-8)
@dword_to_file	macro DD_VALUE,NUM_TETR
		push	ebx ecx
		mov	ebx, DD_VALUE
		mov	ecx, NUM_TETR
		call	write_ebx_to_debug_file
		pop	ecx ebx
		endm

; MVT Запись массива байтов в отладочный файл в виде отображения 16-ричных кодов
; Запись производится при DebugFileOpened =/= 0
; Параметрами является адрес записываемой в файл строки и количество записываемых байт
@Array_to_file	macro PAR_SEG, PAR_OFF, PAR_COUNT
		push	ds esi ecx
		push	PAR_SEG
		pop	ds
		mov	esi, PAR_OFF
		mov	ecx, PAR_COUNT
		call	write_array_to_debug_file
		pop	ecx esi ds
		endm
endif

; MVT сравнение последовательностей байтов
; Вход: SEG1,OFF1,SEG2,OFF2,COUNT - регистры или непосредственные значения
; Выход: zf=1 означает, что строки равны, иначе не равны
@compare_arrays	macro SEG1,OFF1,SEG2,OFF2,COUNT
		push	ds esi es edi ecx
		push	SEG1 ; заносим параметры так, чтобы они не мешали друг другу
		push	OFF1
		push	SEG2
		push	OFF2
		mov	ecx,COUNT
		pop	edi
		pop	es
		pop	esi
		pop	ds
		cld
		repe	cmpsb
		pop	ecx edi es esi ds
		endm
