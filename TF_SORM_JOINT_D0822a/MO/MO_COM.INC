include mo\mo_proc.asm


;;; ---------------------------------------------------------------------------
;;; --- разное - НАЧАЛО                                                     ---
;;; ---------------------------------------------------------------------------


; ----= m62 =------------------------------------------------------------------
MO_37:          ; прими дату/время
		add	bp, 6
		jmp	SLED_MO

MO_62:          ; контроль зависания каналов модема
                @POLUCH_ADR_MO
                CMP     DL,0
                JNE     SHORT END_MO_62
                MOV     AL,63
                STOSB
                @ZAPIS_OK_MO
END_MO_62:      INC     BP
                JMP     SLED_MO

MO_63:          ; команда 62 обработана
                inc     bp
                JMP     SLED_MO

MO_64:          ; пересброс блока от ЦК
                test    FG_PROGRAM, _fg_prg_ck0 ; работаем с ЦК0 ?
                jnz     END_MO_64               ; ДА - на пересброс не реагируем (ПОКА ???)
                jmp     Reboot                  ; НЕТ - перезапустимся !
END_MO_64:      inc     bp
                JMP     SLED_MO



;├────┼────────────────────────────┼───────┼──────────────────────────────┤
;│ 132│ Протоколирование работы    │ Т->ТТ │ "132"                        │
;│    │ оператора                  │ Т->Т  │ "Nт"            -1б          │
;│    │                            │       │ "Пароль доступа"-2б          │
;│    │                            │       │ "Вид действия"  -1б          │
;│    │                            │       │ "Информация"    -18б         │
;│    │                            │       │  (см.ПРИМ.1)                 │
;├────┼────────────────────────────┼───────┼──────────────────────────────┤
MO_132:         call    PUMA_Start_Tar
                mov     al, 93h
                @_STOSB
                call    TimestampToPUMA
                lea     esi, ds:[bp + 1]
                mov     ecx, 22
                DB      67H
                rep     movsb
                call    PUMA_Send_Tar
END_MO_132:     add     bp, 23
                jmp     SLED_MO


;├────┼────────────────────────────┼───────┼──────────────────────────────┤
;│ 135│ Аварийная ситуация         │ Т->ТТ │ "135"                        │
;│    │                            │ Т->Т  │ "Nт"   -1б                   │
;│    │                            │       │ "Дата" -4б(BCD)              │
;│    │                            │       │ "Время"-2б(BCD)              │
;├────┼────────────────────────────┼───────┼──────────────────────────────┤
MO_135:         ; ---
                test    FG_PROGRAM, _fg_prg_ck0; работаем с ЦК0 ?
                jz      END_MO_135             ; НЕТ - время от ЦК не принимаем
                ; ---
                mov     al, ds:[bp + 2]
                mov     GOD_, al
                mov     al, ds:[bp + 3]
                mov     GOD, al
                mov     ax, ds:[bp + 4]
                mov     WORD PTR MES, ax
                mov     ax, ds:[bp + 6]
                mov     WORD PTR MIN, ax
                mov     SECUNDA, 0
                ; ---
END_MO_135:     add     bp, 8
                jmp     SLED_MO

;├────┼────────────────────────────┼───────┼──────────────────────────────┤
;│ 136│ Аварийная ситуация         │ Т->ТТ │ "136"                        │
;│    │                            │ Т->Т  │ "Nт"       -1б               │
;│    │                            │       │ "Nблока"   -1б               │
;│    │                            │       │ "Группа ав"-1б               │
;│    │                            │       │ "Nситуации"-1б  (см.ПРИМ.2)  │
;├────┼────────────────────────────┼───────┼──────────────────────────────┤
MO_136:         call    PUMA_Start_Tar
                mov     al, 092h                ; код сообщения об аварийной ситуации
                @_STOSB
                call    TimestampToPUMA         ; короткая временная метка события
                mov     eax, DS:[bp + 1]        ; № ЦК, № блока, группа и код аварии
                @_STOSD
                call    PUMA_Send_Tar
                add     bp, 5
                jmp     SLED_MO


;;; ---------------------------------------------------------------------------
;;; --- разное - КОНЕЦ                                                      ---
;;; ---------------------------------------------------------------------------


;;; ---------------------------------------------------------------------------
;;; --- старая конфигурация - НАЧАЛО                                        ---
;;; ---------------------------------------------------------------------------

                ; --- общая конфигурация ЦК
MO_125GO        PROC
                movzx   ebx, BYTE PTR ds:[bp + 1]       ; номер ЦК
		; --- работа с ЦК0
		test	FG_PROGRAM, _fg_prg_ck0
		jz	MO_125_Standart
		mov	Cfg_Curr_CK, bl
		mov	es, ss:SEG_AXD
		mov	es:TIS_TIS[bx], 0
		; ---
MO_125_Standart:;;;cmp     bl, Cfg_Curr_CK                 ; тот что ждём ?
;;;!!!                jne     @@bad                           ; НЕТ -
                mov     gs, SS:SEG_KONF
                push    gs
                pop     es
                ; --- формируем конфигурацию для каждого блока ЦК
                shl     ebx, 7                          ; 128 байт конфигурации на ЦК
                mov     edi, ebx
                mov     cx, 16                          ; 16 блоков на ЦК
                push    bp
                add     bp, 2                           ; переставим указатель на список блоков
@@blk_loop:     push    cx
		movzx   bx, ds:[bp]                     ; тип очередного блока
                cmp     bl, 255                         ; блок подключен ?
                je      @@blk_next                      ; НЕТ -
                mov     gs:[edi], bl                    ; запомним тип блока
                mov     al, TAB_TIP_BL[bx]              ; получим то типу блока обобщённый тип
                mov     gs:[edi + 5], al                ; запомним обощённый тип
                shl     bx, 1
                movzx   edx, TAB_DL_BL[bx]              ; получим по типу * 2 число линий в блоке
                mov     gs:[edi + 6], dx                ; и запомним его
		cmp	BYTE PTR gs:[edi], 4		; ЦК ?
                je      @@blk_next                      ; ДА - память под конфигурацию линий не выделяем
                shl     edx, 4                          ; столько байт под все линии текущего блока
                ; --- {Paul} - <15/12/12>
                cmp     DWORD PTR gs:[edi + 1], -1      ; конфигурация блока уже есть?
                jne     @@clear_conf                    ; возъмем готовый участок памяти сег.конфигурации
                mov     ebx, Cfg_Free_Ptr               ; ук. на свободное место в сегменте конф.
                mov     eax, edx                        ; не будем портить edx на всяк. случай)
                add     eax, ebx                        ; двигаем ук. на свободное место
                cmp     eax, DLIN_KONF                  ; выскочили за сегмент ?
                jbe      @@blk_OK                        ; НЕТ -
                ; --- переполнение сегмента конфигурации
                cmp     Cfg_Full, 0
                jne     @@blk_next
                mov     al, Cfg_Curr_CK                 ; номер текущего ЦК
                mov     Cfg_Full, al                    ; запомним для индикации этой БЕДЫ !
                jmp     @@blk_next                      ; выход БЕЗ ПРИЁМА !
@@blk_OK:       ; --- очистим сегмент конфигурации под линии
                add     Cfg_Free_Ptr, edx               ; двигаем ук. на свободное место
                mov     gs:[edi + 1], ebx               ; запомним только если всё ОК
                jmp     @@clear_good
@@clear_conf:   mov     ebx, gs:[edi + 1]
@@clear_good:   push    edi
                mov     edi, ebx                        ; !!!
                mov     ecx, edx
                ; --- {Paul} -- <15/06/12> --
                shr     ecx, 2       ;;;;;!!!! было - 4
                cmp     ecx, 0
                jz      @@not_clear_conf_ln
                mov     eax, 0ffffffffh
                @_RSTOSD
@@not_clear_conf_ln:
                ; --- {Paul} -- <END>
                pop     edi
                ; --- на обработку следующего блока ЦК
@@blk_next:     add	edi, 8
		inc     bp
		pop	cx
                dec     cx
                jnz     @@blk_loop
                pop     bp
		; --- работа с ЦК0
		cmp	Cfg_Curr_CK, 0
		je	@@ok
		; --- прием префикса
                mov     eax, ds:[bp + 18]
                movzx   si, Cfg_Curr_CK
                shl     si, 2
                mov     Cfg_CK_Prefix[si], eax
                ; --- получаем полный префикс для № А
                xchg    al, ah
                rol     al, 4
                rol     ah, 4
                ror     eax, 16
                shl     ax, 8
                shr     eax, 8
                rol     al, 4
                mov     PREF_A_FULL, eax        ; для изменения типов соединений в тар. пакетах
                ; вычисляем короткий префикс для № А
                and     ax, PREF_A_ZN_MASK
                mov     PREF_A_ZN, ax
                ; --- вычисляем префиксы для №Б из МГ префикса и префикса № А
                shl     ax, 4
                or      ax, PREF_B_MG
                mov     PREF_B_ZN, ax
                mov     eax, PREF_A_FULL
                shl     eax, 4
                or      ax, PREF_B_MG
                mov     PREF_B_FULL, eax

                ; --- посылка в ПУМу префикса для обратной совместимости ---
                call    PUMA_Start_Tar                  ;
                mov     al, 0f0h
                @_STOSB                                 ; код команды
                mov     al, BYTE PTR ds:[bp + 1];;;Cfg_Curr_CK
                @_STOSB                                 ; № ЦК
                movzx   esi, al
                shl     si, 2
                mov     eax, Cfg_CK_Prefix[si]
                @_STOSD                                 ; префикс этого ЦК
                call    PUMA_Send_Tar
                ; --- посылка в ПУМу общей конфигурации ЦК ---
                call    PUMA_Start_Tar                  ;
                mov     al, 0f8h
                @_STOSB                                 ; код команды
                mov     al, BYTE PTR ds:[bp + 1]
                @_STOSB                                 ; № ЦК
                movzx   esi, al
                shl     si, 2
                mov     eax, Cfg_CK_Date[si]
                @_STOSD                                 ; дата конфигурации этого ЦК
                shr     si, 1
                mov     ax, Cfg_CK_Time[si]
                @_STOSW                                 ; время конфигурации этого ЦК
                ; --- блоки этого ЦК
                mov     eax, ds:[bp + 2]
                @_STOSD
                mov     eax, ds:[bp + 6]
                @_STOSD
                mov     eax, ds:[bp + 10]
                @_STOSD
                mov     eax, ds:[bp + 14]
                @_STOSD
                ;--- префикс этого ЦК
                shl     si, 1
                mov     eax, Cfg_CK_Prefix[si]
                @_STOSD
                call    PUMA_Send_Tar
                ; ---
@@ok:           movzx   bx, Cfg_Curr_CK                 ; номер ЦК
		mov	Cfg_Show_CK, bl
		mov	Cfg_Show_Blk, 0
                or      Cfg_CK_Flags[bx], cf_RECV_CK    ; отметим приём общей конфигурации
		shl	bx, 1
		mov	Cfg_Curr_Mod, 0
		mov	Cfg_Curr_Line, 0
		mov 	Cfg_CK_Mod_OK[bx], 0
		mov 	Cfg_CK_Mod_Err[bx], 0
                stc
                ret
                ; ---
@@bad:          clc
                ret
MO_125GO        ENDP

MO_125:         ;ОБЩАЯ КОНФИГУРАЦИЯ ЦК
                call    MO_125GO
                jnc     BAD_MO
                ADD     BP,22
                JMP     SLED_MO


                ; --- номер телефона
MO_126GO        PROC
                movzx   ebx, BYTE PTR ds:[bp + 1]       ; номер ЦК
                cmp     bl, Cfg_Curr_CK                 ; тот что ждём ?
;;;!!!                jne     @@bad                           ; НЕТ -
		; --- по параметрам команды получим адрес в SEG_KONF
MO_126GO_2:     movzx   edi, BYTE PTR ds:[bp + 1]       ; № ЦК
                shl     edi, 4                          ; 16 блоков в ЦК
                mov     dx, ds:[bp + 2]                 ; \ номер
                shr     dx, 9                           ; / блока
                or      di, dx                          ; № ЦК и № блока вместе
                shl     edi, 3                          ; 8 байт на конфигурацию блока ЦК
                mov     gs, ss:SEG_KONF
                ; --- защита от ЦК ---
                cmp     BYTE PTR gs:[edi], 4
                je      @@ok
                cmp     BYTE PTR gs:[edi], 0ffh
                je      @@ok
                mov     dx, ds:[bp + 2]
                and     dx, 01ffh
                cmp     dx, gs:[edi + 6]
                ja      @@ok
                ; --- защита от ЦК ---
                mov     edi, gs:[edi + 1]               ; адрес линий блока
                cmp     edi, Cfg_Free_Ptr
                jae     @@ok_bad
                movzx   edx, WORD PTR ds:[bp + 2]       ; \ номер
                and     dx, 1ffh                        ; / линии
                shl     edx, 4                          ; 16 байт на конфигурацию линии
                add     edi, edx                        ; адрес линии в SEG_KONF
                cmp     edi, Cfg_Free_Ptr               ;
                jae     @@ok_bad                        ; выйдем с ОК, но запишем
                ; --- приём категории
                mov     dl, ds:[bp + 6]                 ; \ кате-
                mov     gs:[edi + 8], dl                ; / гория
                ; --- приём номера
                ; выделяем из префикса нужное количество последних цифр для расширения номера
                movzx   si, ds:[bp + 1]
                shl     si, 2
                mov     eax, Cfg_CK_Prefix[si]
                movzx   si, LENGTH_NUM
                and     si, 7
                shl     si, 2
                mov     edx, M_OR_PREF_TIS[si]
                or      eax, edx
                mov     ebx, eax                        ; в ebx нужное кол-во цифр из префикса
                ; цифры тысяч
                mov     ax, WORD PTR ds:[bp + 4]        ; логический номер абонента 0 ... 9999
                cmp     ax, 9999                        ; номера НЕТ ?
                ja      @@ok_bad                        ; ДА - т.е. НЕТ
                xor     dx, dx                          ; \
                mov     cx, 1000                        ; |  добавляем к префиксу слева
                div     cx                              ;  > старшую цифру лог. №
                shl     ebx, 4                          ; |  в dx оставшиеся 3 цифры
                or      bx, ax                          ; /
                ; цифры сотен
                mov     ax, dx                          ; \
                xor     dx, dx                          ; |
                mov     cx, 100                         ;  \ добавляем слева 2-ую цифру лог. №
                div     cx                              ;  / в dx оставшиеся 2 цифры
                shl     ebx, 4                          ; |
                or      bx, ax                          ; /
                ; цифры десятков
                mov     ax, dx                          ; \
                xor     dx, dx                          ; |
                mov     cx, 10                          ;  \ добавляем слева 3-ью цифру лог. №
                div     cx                              ;  / в dx последняя цифра
                shl     ebx, 4                          ; |
                or      bx, ax                          ; /
                ; цифры единиц
                shl     ebx, 4                          ; \ добавляем слева
                or      bx, dx                          ; / последнюю 4-ую цифру
                ; приводим номер к формату СИДС
                movzx   si, LENGTH_NUM
                and     si, 7
                mov     cl, ROL_PREF_TIS[si]
                rol     ebx, cl
                xchg    bh, bl
                ror     ebx, 16
                xchg    bh, bl
                mov     gs:[edi + 10], ebx
                ; заносим № в NLOG
                movzx   edx, BYTE PTR ds:[bp + 1]       ; \
                shl     edx, 13                         ; | абс. физ. № линии в формате СИДС
                or      dx, ds:[bp + 2]                 ; /
                mov     eax, ebx                        ; лог. № абонента в формате СИДС
                call    Add_Tel_Num

                ; --- посылка в ПУМу
                mov     cl, gs:[edi]                    ; тип линии
                call    PUMA_Start_Tar
                mov     al, 0f4h                        ; код команды
                @_STOSB
                mov     eax, edx                        ; \
                @_STOSW                                 ;  - №СЛ! (20 бит)
                shr     eax, 16                         ; /
                @_STOSB                                 ;
                mov     eax, ebx                        ; №тел(формат СИДС)
                @_STOSD
                mov     al, ds:[bp + 6]                 ; категория
                @_STOSB
                mov     al, cl                          ; тип линии
                @_STOSB
                call    PUMA_Send_Tar                   ; завершим запись
                ; ---
@@ok:           stc
                ret
                ; ---
@@ok_bad:       stc
                ret
                ; ---
@@bad:          clc
                ret
MO_126GO        ENDP

MO_126:         ;НОМЕР ТЕЛЕФОНА
                call    MO_126GO_2
                jnc     BAD_MO
                ADD     BP,7
                JMP     SLED_MO


                ; --- конфигурация линии
MO_127GO        PROC
                movzx   ebx, BYTE PTR ds:[bp + 1]       ; номер ЦК
                cmp     bl, Cfg_Curr_CK                 ; тот что ждём ?
;;;!!!                jne     @@bad                           ; НЕТ -
		; --- по параметрам команды получим адрес в SEG_KONF
MO_127GO_2:     movzx   edi, BYTE PTR ds:[bp + 1]       ; № ЦК
                shl     edi, 4                          ; 16 блоков в ЦК
                mov     dx, ds:[bp + 2]                 ; \ номер
                shr     dx, 9                           ; / блока
                and     dx, 0fh
                or      di, dx                          ; № ЦК и № блока вместе
                shl     edi, 3                          ; 8 байт на конфигурацию блока ЦК
                mov     gs, ss:SEG_KONF
                ; --- защита от ЦК ---
                cmp     BYTE PTR gs:[edi], 4
                je      @@ok
                cmp     BYTE PTR gs:[edi], 0ffh
                je      @@ok
                mov     dx, ds:[bp + 2]
                and     dx, 01ffh
                cmp     dx, gs:[edi + 6]
                ja      @@ok
                ; --- защита от ЦК ---
                mov     edi, gs:[edi + 1]               ; адрес линий блока
                cmp     edi, Cfg_Free_Ptr
                jae     @@ok_bad
                movzx   edx, WORD PTR ds:[bp + 2]       ; \ номер
                and     dx, 1ffh                        ; / линии
                shl     edx, 4                          ; 16 байт на конфигурацию линии
                add     edi, edx                        ; адрес линии в SEG_KONF
                cmp     edi, Cfg_Free_Ptr               ;
                jae     @@ok_bad                        ; выйдем с ОК, но запишем
		; --- приём данных
                mov     dl, ds:[bp + 4]                 ; \ тип
                mov     gs:[edi], dl                    ; / линии
                movzx   edx, WORD PTR ds:[bp + 5]       ; \ СПУС
                mov     gs:[edi + 1], edx               ; /
                mov     eax, 0ffffffffh                 ; если нет номера
                mov     gs:[edi + 10], eax              ; будут ff-ы

                ; ---- посылка в ПУМу ---
                push    eax edx
                call    PUMA_Start_Tar
                xor     edx, edx
                xor     eax, eax
                mov     al, 0f3h                        ; код команды
                @_STOSB
                movzx   edx, WORD PTR ds:[bp + 2]       ; \№линии +
                and     edx, 1fffh                      ; /№блока
                movzx   eax, BYTE PTR ds:[bp + 1]       ; №ЦК
                and     eax, 3fh
                shl     eax, 13
                or      eax, edx                        ; \
                @_STOSW                                 ;  - №СЛ! (20 бит)
                shr     eax, 16                         ; /
                @_STOSB                                 ;
                mov     al, BYTE PTR ds:[bp +4]         ; тип линии
                @_STOSB
                call    PUMA_Send_Tar                   ; завершим запись
                pop     edx eax
                ; ---
                ;mov     edx, gs:[edi + 10]              ; № телефона в SEG_KONF
                ;movzx   eax, BYTE PTR ds:[bp + 1]       ; \
                ;shl     eax, 13                         ; |  абс. физ. № линии
                ;or      ax, ds:[bp + 2]                 ;  > в формате СИДС
                ;shl     eax, 8                          ; |  с префиксом 254 (АОН)
                ;mov     al, 0feh                        ; /
                ;mov     gs:[edi + 10], eax              ; запомним это вместо № телефона
                ;mov     eax, edx                        ; ДА - удалим его из SEG_NLOG
;;;		call	Del_Tel_Num
                ; ---
@@ok:           stc
                ret
                ; ---
@@ok_bad:       stc
                ret
                ; ---
@@bad:          clc
                ret
MO_127GO        ENDP

MO_127:         ;КОНФИГУРАЦИЯ ЛИНИИ
                call    MO_127GO_2
                jnc     BAD_MO
                ADD     BP,9
                JMP     SLED_MO


                ; --- конец конфигурации ЦК
MO_128GO        PROC
                movzx   ebx, BYTE PTR ds:[bp + 1]       ; номер ЦК
		; --- работа с ЦК0
		test	FG_PROGRAM, _fg_prg_ck0
		jz	MO_128_Standart
                or      Cfg_CK_Flags[bx], cf_DONE       ; отметим приём полной конфигурации ЦК
		or	bx, bx
		jnz	@@ok_not_0
		mov	Cfg_Run, 0
@@ok_not_0:	stc
		ret
		; ---
MO_128_Standart:cmp     bl, Cfg_Curr_CK                 ; тот что ждём ?
                jne     @@bad                           ; НЕТ -
		; ---
@@ok:           call    PUMA_Start_Tar                  ; в ПУМу
                mov     al, 0f2h
                @_STOSB                                 ; код команды
                mov     al, Cfg_Curr_CK
                @_STOSB                                 ; № ЦК
                call    PUMA_Send_Tar
                ; ---
                movzx   si, Cfg_Curr_CK                 ; номер ЦК
                or      Cfg_CK_Flags[si], cf_DONE       ; отметим приём полной конфигурации ЦК
                stc
                ret
                ; ---
@@bad:          clc
                ret
MO_128GO        ENDP

MO_128:         ;КОНЕЦ КОНФИГУРАЦИИ
                call    MO_128GO
                jnc     BAD_MO
                ADD     BP,2
                JMP     SLED_MO


                ; --- линия УПАТС
MO_129GO        PROC
                movzx   ebx, BYTE PTR ds:[bp + 1]       ; номер ЦК
                cmp     bl, Cfg_Curr_CK                 ; тот что ждём ?
;;;!!!                jne     @@bad                           ; НЕТ -
		; --- по параметрам команды получим адрес в SEG_KONF
                movzx   edi, BYTE PTR ds:[bp + 1]       ; № ЦК
                shl     edi, 4                          ; 16 блоков в ЦК
                mov     dx, ds:[bp + 2]                 ; \ номер
                shr     dx, 9                           ; / блока
                and     dx, 0fh
                or      di, dx                          ; № ЦК и № блока вместе
                shl     edi, 3                          ; 8 байт на конфигурацию блока ЦК
                mov     gs, ss:SEG_KONF
                ; --- защита от ЦК ---
                cmp     BYTE PTR gs:[edi], 4
                je      @@ok
                cmp     BYTE PTR gs:[edi], 0ffh
                je      @@ok
                mov     dx, ds:[bp + 2]
                and     dx, 01ffh
                cmp     dx, gs:[edi + 6]
                ja      @@ok
                ; --- защита от ЦК ---
                mov     edi, gs:[edi + 1]               ; адрес линий блока
                cmp     edi, Cfg_Free_Ptr
                jae     @@bad
                movzx   edx, WORD PTR ds:[bp + 2]       ; \ номер
                and     dx, 1ffh                        ; / линии
                shl     edx, 4                          ; 16 байт на конфигурацию линии
                add     edi, edx                        ; адрес линии в SEG_KONF
                cmp     edi, Cfg_Free_Ptr               ;
                jae     @@ok_bad                        ; выйдем с ОК, но запишем
		; --- приём данных
                ; --- {Paul} --- <12/10/2012>
                cmp     Byte PTR ds:[bp + 6], 99
                ja      @@ok_bad
                cmp     Word PTR ds:[bp + 4], 9999
                ja      @@ok_bad
                ; --- {Paul} --- <END>
                mov     dx, ds:[bp + 4]                 ; \ № логический
                mov     gs:[edi + 5], dx                ; / в УПАТС
                mov     dl, ds:[bp + 6]                 ; \ №
                mov     gs:[edi + 7], dl                ; / УПАТС

                ; заносим № в ULOG
                movzx   edx, BYTE PTR ds:[bp + 1]       ; \
                shl     edx, 13                         ; | абс. физ. № линии в формате СИДС
                or      dx, ds:[bp + 2]                 ; /
                mov     eax, ds:[bp + 4]                ; № телефона в УПАТС (и лишний байт)
                call    Add_U_Num
                ; ---
@@ok:           stc
                ret
                ; ---
@@ok_bad:       stc
                ret
                ; ---
@@bad:          clc
                ret
MO_129GO        ENDP

MO_129:         ;ЛИНИЯ УПАТС
                test    FG_PROGRAM, _fg_prg_multy_pref
                jnz     @@not_129
                call    MO_129GO
                jnc     BAD_MO
  @@not_129:    ADD     BP,7
                JMP     SLED_MO

;+----+----------------------------+-------+------------------------------+
;| 133| Версия конфигурации ЦК     | Т->ТТ | "133"                        |
;|    |                            | Т->Т  | "Nт"   -1б                   |
;|    |                            |       | "Дата" -4б(BCD)              |
;|    |                            |       | "Время"-2б(BCD)              |
;+----+----------------------------+-------+------------------------------+
MO_133:
                movzx   bx, ds:[bp + 1]
                ; --- <Paul> --- {15/12/12}
                cmp     Cfg_Run, 0
                jz      MO_133_Restart               ; restart block!!!
                ; идет сбор конфигурации!
                cmp     Cfg_Curr_CK, bl
                jne     MO_133_fur                   ; restart block!!
                ; --- good for get data
                or      Cfg_CK_Flags[bx], cf_RECV_CD ; отметим приём даты конфигурации
                shl     bx, 2
                mov     eax, ds:[bp + 2]
                mov     Cfg_CK_Date[bx], eax
                shr     bx, 1
                mov     ax, ds:[bp + 6]
                mov     Cfg_CK_Time[bx], ax
                shr     bx, 1
                ; ---
MO_133_PUMA:    call    PUMA_Start_Tar
                mov     al, 0feh
                @_STOSB
                mov     al, bl
                @_STOSB
                shl     bx, 2
                mov     eax, Cfg_CK_Date[bx]
                @_STOSD
                shr     bx, 1
                mov     ax, Cfg_CK_Time[bx]
                @_STOSW
                call    PUMA_Send_Tar
                ; ---
                JMP     END_MO_133
MO_133_Restart: ; --- сбора конфигурации нет - блок перегрузился
                ; -- init all parametrs
                mov     Cfg_Run, 255
                mov     Cfg_Curr_CK,        bl
                mov     Cfg_from_One_CK,    bl
                mov     Cfg_Check_Level,    0

MO_133_fur:     ; -- reset flags of block conf.
                mov     Cfg_CK_Flags[bx],   0
                mov     Cfg_CK_Errs[bx],    0
		shl	bx, 1
                mov     Cfg_CK_Lines[bx],   0
                mov     Cfg_CK_Mod_OK[bx],  0
		mov	Cfg_CK_Mod_Err[bx], 0
                shr     bx, 1
                ; ---<Paul>---(End)---
                ; -----------
END_MO_133:     ADD     BP,8
                JMP     SLED_MO


MO_141:         ;ПАКЕТ С КОНФИГУРАЦИЕЙ
IF _MSP EQ TRUE
                mov     eax, Sys_Time           ; \ фиксируем время
ELSE
                mov     eax, Current_Time           ; \ фиксируем время
ENDIF
                mov     Cfg_TOut, eax           ; / приёма
                ; ---
                mov     PEREM_BP, bp
                inc     bp
                cmp     BYTE PTR ds:[bp], 128   ; конец конфигурация ЦК
                je      MO_141_128
                cmp     BYTE PTR ds:[bp], 127   ; конфигурация линии
                je      MO_141_127
                cmp     BYTE PTR ds:[bp], 125   ; общая конфигурация ЦК
                je      MO_141_125
END_MO141:
                mov     bp, PEREM_BP
                @POLUCH_ADR_MO
                or      dl, dl
                jne     EXIT_MO141
                mov     al, 142
                stosb
                mov     al, ds:[bp + 2]         ; НОМЕР ТЫСЯЧНИКА
                stosb
                @ZAPIS_OK_MO
EXIT_MO141:
                add     bp, 26
                jmp     SLED_MO
BAD_MO141:
                ;;;mov     bp, PEREM_BP         ; чтоб ошибка определялась внутри МО141
                jmp     BAD_MO

MO_141_125:     ; --- общая конфигурация ЦК
                call    MO_125GO
                jnc     BAD_MO141
                jmp     END_MO141

MO_141_126:     ; --- номер телефона
                call    MO_126GO
                jnc     BAD_MO141
                add     bp, 7
                jmp     MO141_Get_Next

MO_141_127:     ; --- конфигурация линии
                ; --- определяем момент перехода к линиям нового блока
                mov     bx, ds:[bp + 2]
                shr     bx, 9
		mov	Cfg_Show_Blk, bl
                cmp     bl, Cfg_Curr_Mod
                je      MO_141_127_Count_Line
		cmp	Cfg_Curr_Line, 1
		jbe	MO_141_127_New_Mod
                ; --- определяем сколько линий должно быть у предыдущего блока
                mov     gs, ss:SEG_KONF
                movzx   edi, Cfg_Curr_CK
                shl     edi, 4
                movzx   ax, Cfg_Curr_Mod
		and	ax, 0fh
                or      di, ax
                shl     edi, 3
		cmp	BYTE PTR gs:[edi], 4
		je	MO_141_127_New_Mod
		cmp	BYTE PTR gs:[edi], 255
		je	MO_141_127_New_Mod
                mov     dx, gs:[edi + 6]
                movzx   di, Cfg_Curr_CK
                ; --- определяем результат обработки блока по числу принятых линий
                shl     di, 1
                lea     si, Cfg_CK_Mod_OK[di]
                or      dx, dx
                je      MO_141_127_New_Mod
                cmp     Cfg_Curr_Line, 1
                je      MO_141_127_New_Mod
                cmp     Cfg_Curr_Line, dx
                jae     MO_141_127_Mod_Result
                lea     si, Cfg_CK_Mod_Err[di]
MO_141_127_Mod_Result:
                bts     [si], ax		; ax - текущий блок
                ; --- запоминаем склько линий приняли у предыдущего блока
                movzx   si, Cfg_Curr_CK
                shl     si, 4                   ; 16 блоков в ЦК
                add     si, ax                  ; ax - текущий блок
                shl     si, 1                   ; слово
                mov     ax, Cfg_Curr_Line
                mov     Cfg_CK_Lines[si], ax
                ; --- готовимся к обработке нового блока
MO_141_127_New_Mod:
                mov     Cfg_Curr_Line, 0
                mov     Cfg_Curr_Mod,  bl
                ; --- подсчитываем принятые линии
MO_141_127_Count_Line:
                inc     Cfg_Curr_Line
                ; ---
                call    MO_127GO
                jnc     BAD_MO141
                add     bp, 9
MO141_Get_Next:
                cmp     ds:[bp], BYTE PTR 129  ; конфигурация линии УПАТС
                je      MO_141_129
                cmp     ds:[bp], BYTE PTR 126  ; номер телефона
                je      MO_141_126
                cmp     ds:[bp], BYTE PTR 128  ; конец конфигурации
                je      MO_141_128
                jmp     END_MO141

MO_141_129:     ; --- линия УПАТС
                call    MO_129GO
                add     bp, 7
                jmp     MO141_Get_Next

MO_141_128:     ; --- конец конфигурации
		cmp	Cfg_Curr_Line, 0
		je	MO_141_128GO
                ; --- определяем сколько линий должно быть у предыдущего блока
                mov     gs, ss:SEG_KONF
                movzx   edi, Cfg_Curr_CK
                shl     edi, 4
                movzx   ax, Cfg_Curr_Mod
		and	ax, 0fh
                or      di, ax
                shl     edi, 3
                mov     dx, gs:[edi + 6]
                movzx   di, Cfg_Curr_CK
                ; --- определяем результат обработки блока по числу принятых линий
                shl     di, 1
                lea     si, Cfg_CK_Mod_OK[di]
                cmp     Cfg_Curr_Line, dx
                jae     MO_141_128_Mod_Result
                lea     si, Cfg_CK_Mod_Err[di]
MO_141_128_Mod_Result:
                bts     [si], ax		; ax - текущий блок
                ; --- запоминаем сколько линий приняли у предыдущего блока
                movzx   si, Cfg_Curr_CK
                shl     si, 4                   ; 16 блоков в ЦК
                add     si, ax                  ; ax - текущий блок
                shl     si, 1                   ; слово
                mov     ax, Cfg_Curr_Line
                mov     Cfg_CK_Lines[si], ax
		; ---
MO_141_128GO:   call    MO_128GO
                jnc     BAD_MO141
                mov     Cfg_Show_CK, -1
		mov	Cfg_Show_Blk, -1
                add     bp, 2
                jmp     END_MO141


MO_152:         ;ВРЕМЯ И ДАТА ОТ ЦК
                ADD     BP, 8
                JMP     SLED_MO

;;; ---------------------------------------------------------------------------
;;; --- старая конфигурация - КОНЕЦ                                         ---
;;; ---------------------------------------------------------------------------

;  ----------------------------------------------------------------------------
;  --- СОРМ - НАЧАЛО                                                        ---
;  ----------------------------------------------------------------------------

; ----= m =-----------------------------------------------------------------
                        ;0    1     2-3    4-5
                        ;COD/ N_TS/ N_RAB/ N_FIZ
MO_100:                 ;ЗАНЯТИЕ ОТ БЛОКА
		mov	al, ds:[bp + 1]
		mov	MO_CK, al
		and	BYTE PTR ds:[bp + 1], 7fh
		and     al, 7fh
                cmp     al, _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 4], _PHYS_NUM_MAX
                jae     BAD_MO
                ; --- создание и инициализация SOED
                call    Seizure
@_check_debug_black
                ; --- запрос АОН (если получится)
                call    ANI_Request
END_MO_100:     add     bp, 6
                jmp     SLED_MO

; ----= m101 =-----------------------------------------------------------------
                        ;0    1     2-3    4
                        ;COD/ N_TS/ N_RAB/ CIF
MO_101:                 ;ЦИФРА НАБОРА
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                call    Check_Work_Num_A
                jnc     END_MO_101
                ; --- приём (одной) цифры
                lea     si, ds:[bp + 4]
                mov     cx, 1
                call    Put_Digits
                ; --- чёрный список
		cmp     BYTE PTR fs:[edi].len_b, 4
                jne     END_MO_101
                test    FG_PROGRAM, _fg_prg_bl_new ; "новый черный список" - ?
                jnz     END_MO_101                 ;  да
                call    Black_List
END_MO_101:
                add     bp, 5
                jmp     SLED_MO

; ----= m102 =-----------------------------------------------------------------
                        ;0    1     2-3    4       5-...
                        ;COD/ N_TS/ N_RAB/ KL_CIF/ CIF
MO_102:                 ;ЦИФРЫ НАБОРА
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                call    Check_Work_Num_A
                jnc     END_MO_102
                ; --- приём (нескольких) цифр
                lea     si, ds:[bp + 5]
                movzx   cx, ds:[bp + 4]
                call    Put_Digits
                ; --- чёрный список
                cmp     BYTE PTR fs:[edi].len_b, 4
                jne     END_MO_102
                test    FG_PROGRAM, _fg_prg_bl_new ; "новый черный список" - ?
                jnz     END_MO_102                 ;  да
                call    Black_List
                ; ---
END_MO_102:     movzx	ax, ds:[bp + 4]
		add     bp, ax
		add     bp, 5
                jmp     SLED_MO


; ----= m103 =----------------------------------------------------------------
                        ;0    1     2-3    4       5-...
                        ;COD/ N_TS/ N_RAB/ KL_CIF/ CIF
MO_103:                 ;ПОСЛЕДНЯЯ ЦИФРА НАБОРА
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                call    Check_Work_Num_A
                jnc     END_MO_103
                ; --- приём (одной) цифры, отмечаем приём полного номера
                lea     si, ds:[bp + 4]
                mov     cx, 1
                call    Put_Digits
                or      fs:[edi].soed_flags, sf_NUM_FULL
                test    fs:[edi].soed_flags, sf_BUSY
                jnz     @@not_SORM
                ; --- запрос АОН (если получится)
                call    ANI_Request
                ; --- получение атрибутов номера Б для СОРМ
                call    Num_B_Attr
                ; --- СОРМ -----------------------------------------------------
                call    Call_SORM
                call    Ua_Received
                ; --- чёрный список --------------------------------------------
@@not_SORM:     test    FG_PROGRAM, _fg_prg_bl_new ; "новый черный список" - ?
                jnz     END_MO_103                 ;  да
                call    Black_List_In
                call    Black_List
                ; ---
END_MO_103:     add     bp, 5
                jmp     SLED_MO

; ----= m104 =----------------------------------------------------------------
                        ;0    1     2-3
                        ;COD/ N_TS/ N_RAB
MO_104:                 ;НАБОР ЗАКОНЧЕН
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                call    Check_Work_Num_A
                jnc     END_MO_104
                ; --- отмечаем приём полного номера
                or      fs:[edi].soed_flags, sf_NUM_FULL
                test    fs:[edi].soed_flags, sf_BUSY
                jnz     @@not_SORM
                ; --- запрос АОН (если получится)
                call    ANI_Request
                ; --- получение атрибутов номера Б для СОРМ
                call    Num_B_Attr
                ; --- СОРМ -----------------------------------------------------
                call    Call_SORM
                call    Ua_Received
                ; --- чёрный список --------------------------------------------
@@not_SORM:     test    FG_PROGRAM, _fg_prg_bl_new ; "новый черный список" - ?
                jnz     END_MO_104                 ;  да
                call    Black_List_In
                call    Black_List
                ; ---
END_MO_104:     add     bp, 4
                jmp     SLED_MO

; ----= m106 =----------------------------------------------------------------
                        ;0    1     2-3
                        ;COD/ N_TS/ N_RAB
MO_106:                 ;РАЗЪЕДИНЕНИЕ
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                call    Check_Work_Num_AB
                jnc     END_MO_106
                ; --- отмечаем завершение соединения
                or      fs:[edi].soed_flags, sf_REL
                ; --- разъединение для СОРМа
                call    Disconnect
                ; ---
                ;;;@_null_refr_black
END_MO_106:     add     bp, 4
                jmp     SLED_MO

; ----= m107 =----------------------------------------------------------------
                        ;0    1     3
                        ;COD/ N_TS/ N_RAB
MO_107:                 ;ОТВЕТ
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                call    Check_Work_Num_AB
                jnc     END_MO_107
                ; --- запрос АОН (если получится)
                call    ANI_Request
                ; --- СОРМ для ответа
                ;;;call    Call_SORM_Ans
                ; --- ответ для СОРМа
                call    Answer
                ; --- отмечаем ответ
                or      fs:[edi].soed_flags, sf_ANS
                ; ---
END_MO_107:     add     bp, 4
                jmp     SLED_MO

; ----= m109 =----------------------------------------------------------------
                        ;0    1     2-3
                        ;COD/ N_TS/ N_RAB
MO_109:                 ;ОТБОЙ
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                call    Check_Work_Num_AB
                jnc     END_MO_109
                ; --- отмечаем завершение соединения
                or      fs:[edi].soed_flags, sf_REL
                ; --- разъединение для СОРМа
                call    Disconnect
                ; ---
                ;;;@_null_refr_black
END_MO_109:     add     bp, 4
                jmp     SLED_MO

; ----= m110 =----------------------------------------------------------------
                        ;0    1     2-3    4-5
                        ;COD/ N_TS/ N_RAB/ N_FIZ
MO_110:                 ;ЗАНЯТИЕ ОТ МГ
		mov	al, ds:[bp + 1]
		mov	MO_CK, al
		and	BYTE PTR ds:[bp + 1], 7fh
		and     al, 7fh
                cmp     al, _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 4], _PHYS_NUM_MAX
                jae     BAD_MO
                ; --- создание и инициализация SOED, отмечаем МГ
                call    Seizure
                or      fs:[esi].soed_flags, sf_MG
                ; --- запрос АОН (если получится)
                call    ANI_Request
                ; ---
END_MO_110:     add     bp, 6
                jmp     SLED_MO

; ----= m112 =----------------------------------------------------------------
                        ;0     1       2-3      4       5-6      7-8
                        ;COD / N_TS_A/ N_RAB_A/ N_TS_B/ N_RAB_B/ N_FIZ_B
MO_112:                 ;ЗАНЯТИЕ НА БЛОК
cmp WORD PTR ds:[bp+2],0ffffh
je END_MO_112
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                mov     al, BYTE PTR ds:[bp + 4]
		mov     MO_CK, al
		and	BYTE PTR ds:[bp + 4], 7fh
		and     al, 7fh
                cmp     al, _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 5], _WORK_NUM_MAX
                jae     BAD_MO
                mov     ax, WORD PTR ds:[bp + 7]
		and	ax, 07fffh
                cmp     ax, _PHYS_NUM_MAX
                jae     BAD_MO
                ; ---
                call    Check_Work_Num_A
                jnc     END_MO_112
                ; --- обработка злобного вызова
                test    WORD PTR ds:[bp + 7], M_OR_15
                jz      MO_112_SOED
                ; ---
                push    edi
                mov     esi, edi
                call    PUMA_Start_Tar
                mov     al, 91h
                @_STOSB
                call    TimeStampToPUMA
                cmp     fs:[esi].len_a, 0
                je      MO_112_Evil_no_A
                mov     eax, DWORD PTR fs:[esi].num_a
                rol     al, 4
                rol     ah, 4
                rol     eax, 16
                rol     al, 4
                rol     ah, 4
                rol     eax, 16
                @_STOSD
                mov     ax, WORD PTR fs:[esi + 4].num_a
                rol     al, 4
                rol     ah, 4
                @_STOSW
                mov     ax, 0ffffh
                @_STOSW
                @_STOSB
                jmp     MO_112_Evil_B
MO_112_Evil_no_A:; ---
                mov     al, 0feh                        ; признак физического номера
                @_STOSB
                movzx   ax, fs:[esi].ck_a               ; № ЦК А
                shl     eax, 16
                mov     ax, fs:[esi].phys_a             ; полный физический № линии на ЦК А
                shl     ax, 3                           ; \ собрали абсолютный №
                shr     eax, 3                          ; / линии на ЦК А
                @_STOSW                                 ; \
                shr     eax, 16                         ;  > записали в ПУМу
                @_STOSB                                 ; /
                mov     eax, 0ffffffffh                 ; \
                @_STOSD                                 ;  > записали 5 байт добивки
                @_STOSB                                 ; /
MO_112_Evil_B:  ; ---
                mov     eax, DWORD PTR fs:[esi].num_b   ;
                rol     al, 4
                rol     ah, 4
                rol     eax, 16
                rol     al, 4
                rol     ah, 4
                rol     eax, 16
                @_STOSD                                 ; номер телефона Б
                call    PUMA_Send_Tar
                pop     edi
MO_112_SOED:    ; --- проверим получали ли мы уже МО_112
                test    fs:[edi].soed_flags, sf_BLK_B
                jz      MO_112_New_B
		; --- удаляем ссылку по прежнему Nб
                movzx   ebx, fs:[edi].ck_b
		cmp	bl, _CK_MAX
		jae	MO_112_New_B
                cmp     fs:[edi].work_b, _WORK_NUM_MAX ; MVT исправил PAUL нашёл, было _PHYS_NUM_MAX
		jae	MO_112_New_B
                shl     ebx, _WORK_NUM_SHL
                or      bx, fs:[edi].work_b
                shl     ebx, 1
                mov     es, ss:SEG_ADR_SOED
                mov     WORD PTR es:[ebx], 0ffffh
                ; --- приём рабочего номера Б в SOED
MO_112_New_B:   mov     al, ds:[bp + 4]
		and     al, 07fh
                mov     fs:[edi].ck_b, al
                mov     ax, ds:[bp + 5]
                mov     fs:[edi].work_b, ax
                mov     ax, ds:[bp + 7]
		and	ax, 7fffh
                mov     fs:[edi].phys_b, ax
                test    fs:[edi].soed_flags, sf_RA
                jz      MO_112_Adr_Soed
                mov     al, fs:[edi].ck_b
                mov     fs:[edi].ck_ra, al
                mov     ax, fs:[edi].work_b
                mov     fs:[edi].work_ra, ax
                mov     ax, fs:[edi].phys_b
                mov     fs:[edi].phys_ra, ax
                ; --- на соединение, созданное по раб. № А устанавливаем ссылку по раб. № Б
MO_112_Adr_Soed:mov     es, ss:SEG_ADR_SOED
                movzx   ebx, BYTE PTR ds:[bp + 4]
                shl     ebx, _WORK_NUM_SHL
                or      bx, ds:[bp + 5]
                shl     ebx, 1
                xor     edx, edx
                mov     eax, edi
                mov     ecx, _SOED_SIZE
                div     ecx
                mov     es:[ebx], ax
; --- for blacklist_in debuger
@_check_debug_black_in
		; --- проверяем выход на IP
		test    MO_CK, 80h
		jz      MO_112_Sorm
                or      fs:[edi].soed_flags, sf_IP_B
                ; --- СОРМ --------------------------------------------------------------
MO_112_SORM:	test    fs:[edi].soed_flags, sf_BLK_B
                jz      MO_112_SORM_1st
		; --- работа СОРМа по второй 112-ой
MO_112_SORM_2nd:push    edi
		mov	bl, fs:[edi].ck_a               ;
		mov	dx, fs:[edi].work_a             ;
                mov     es, ss:SEG_SORM                 ;
                ; --- проверяем все сесcии, которые могут быть связаны с соединением
                mov     cx, _SOED_SESSIONs              ;
                add     edi, bunch_a_ptr                ;
MO_112_Ses_Find:mov     si, fs:[edi]                    ;
                cmp     si, 0ffffh                      ;
                je      MO_112_Ses_Next                 ;
		test    es:[si].sorm_flags, ssf_REL     ;
		jnz	MO_112_Ses_Next                 ;
		cmp	es:[si].sorm_ck_a, bl           ;
		jne	MO_112_Ses_Next                 ;
		cmp	es:[si].sorm_work_a, dx         ;
		jne	MO_112_Ses_Next                 ;
		call	ReComm                          ;
MO_112_Ses_Next:add     edi, 2                          ;
                loop    MO_112_Ses_Find                 ;
		pop	edi                             ;
                or      fs:[edi].soed_flags, sf_BLK_B   ;
                 ; -- {Paul.N} --- [07/06/13]
                ; -- попробуем сформировать сообщения даже, если нет АОНа
                ; -- может быть №УПАТС
                ;test    fs:[edi].soed_flags, sf_ANI     ; есть №а
                ;jz      END_MO_112                      ;
                test    fs:[edi].soed_flags, sf_NUM_FULL; есть №б
                jz      END_MO_112                      ;
                test    fs:[edi].soed_flags, sf_RA
                jz      END_MO_112
		call	Num_C_Received                  ;
		jmp     END_MO_112
		; --- работа СОРМа по первой 112-ой
MO_112_Sorm_1st:or      fs:[edi].soed_flags, sf_BLK_B
                ; --- UPATS ----------------------------;
                call    Ua_Received                     ;
                call    Ub_Received                     ;
                ; --- UPATS ----------------------------;
                call    Call_SORM
		; ---
END_MO_112:     add     bp, 9
                jmp     SLED_MO

; ----= m113 =----------------------------------------------------------------
                        ;0    1     2-3    4
                        ;COD/ N_TS/ N_RAB/ PRITCHINA
MO_113:                 ;ВЫЗЫВАЕМЫЙ ЗАНЯТ
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                call    Check_Work_Num_AB
                jnc     END_MO_113
                ; --- пока Б не отвечает
                xor     dh, dh
                ; -- Rostov --->>>
                test    BYTE PTR ds:[bp + 4], 80h
                jnz     @@new_release
                ; -- OLD Release
                or      fs:[edi].soed_flags, sf_REL
		cmp	BYTE PTR ds:[bp + 4], 38	; Б не отвечает ?
                je      MO_113_Disc
                ;
;;;		cmp	BYTE PTR ds:[bp + 4], 33	; Б не отвечает для IP?
;;;             je      MO_113_Disc
                ; --- а здесь Б занят
                or      fs:[edi].soed_flags, sf_BUSY
                jmp     MO_113_Disc

                ; --- {Paul} -- <03/10/2012> ---
@@new_release:  ; -----------------
                ; -- Б не отвечает
                or      fs:[edi].soed_flags, sf_REL
                cmp     BYTE PTR ds:[bp + 4], 90h
                je      MO_113_DISC
                cmp     BYTE PTR ds:[bp + 4], 92h
                je      MO_113_Disc
                cmp     BYTE PTR ds:[bp + 4], 93h
                je      MO_113_Disc
                ; -- Б занят
                or      fs:[edi].soed_flags, sf_BUSY
                cmp     BYTE PTR ds:[bp + 4], 91h
                je      MO_113_Disc
                cmp     BYTE PTR ds:[bp + 4], 95h
                je      MO_113_Disc
                ; -- Все отсальные коды - техническая неисправность!
                or      fs:[edi].soed_flags, sf_MN_rostov
                ; --- {End} -- <Paul>
MO_113_Disc:    call    Disconnect
                ; ---
                ;;;@_null_refr_black
END_MO_113:     add     bp, 5
                jmp     SLED_MO

; ----= m118 =----------------------------------------------------------------
                        ;0    1     3     4
                        ;COD/N_RAB/N_TIS/8-CIF
MO_118:                 ;ИНФОРМАЦИЯ АОН
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                call    Check_Work_Num_A
                jnc     END_MO_118
                ; --- проверка получения АОН
                test    fs:[edi].soed_flags, sf_ANI
                jnz     END_MO_118
                ; --- отмечаем получение АОН и принимаем номер --------------------------
                or      fs:[edi].soed_flags, sf_ANI
                ; --- проверка длины АОН
                movzx   cx, ds:[bp + 4]
                cmp     cx, 1
                jbe     END_MO_118
                cmp     cx, 0ffh
                je      END_MO_118
                dec     cx
                cmp     cx, 18
                jbe     SHORT MO_118_Num_Len
                mov     cx, 18
MO_118_Num_Len: mov	fs:[edi].len_a, cl
                xor     ebx, ebx
                lea     si, ds:[bp + 5]
MO_118_Num:	; --- сам приём АОН (без категории)
                lodsb
                or      al, 0f0h
                mov     fs:[edi + ebx].num_a, al
                dec     cx
                jz      MO_118_SORM
                lodsb
                and     fs:[edi + ebx].num_a, 0fh
                shl     al, 4
                or      fs:[edi + ebx].num_a, al
                inc     ebx
                loop    MO_118_Num
                ; --- запишем категорию в соединение ---
MO_118_SORM:    lodsb
                and     al, 0Fh
                mov     fs:[edi].cat_dig, al
                ; ------      -------
                ; --- СОРМ --------------------------------------------------------------
                call    Num_A_Attr
                ; -- {Paul.N - add - 17/07/17}
                test    fs:[edi].soed_flags, sf_2VSK_Chelnok      ; был ли "челнок"
                jz      MO_118_Black_Check
                ; -- здесь "челнок" - пробуем отработать по №А(АОН)
                call    Ub_Received
                call    Call_SORM
                ; - хоть это и маловероятно, т.к АОН запрашивается
                ; - в команде "ответ" при "челноке"
                ; - и не только, но все же ->
                test    fs:[edi].soed_flags, sf_ANS             ; <- проверим а был ли ответ?
                jz      MO_118_Black_Check                      ;
                and     fs:[edi].soed_flags, not sf_ANS         ; <- временно снимем "ответ"
                ; --- ответ для СОРМа
                call    Answer                                  ; <- "Ответим"
                or      fs:[edi].soed_flags, sf_ANS             ; <- вернем "ответ"
                ; --- всякие проверки с АОН ---------------------------------------------
MO_118_Black_Check:;- проверка чёрного списка
                test    fs:[edi].soed_flags, sf_NUM_FULL
                jz      MO_118_ANI_Check
                test    FG_PROGRAM, _fg_prg_bl_new ; "новый черный список" - ?
                jnz     MO_118_ANI_Check           ;  да
                call    Black_List
MO_118_ANI_Check:;--- проверка АОН по длине
                test    FG_PROGRAM, M_OR_10     ; АОН по длине проверяем ?
                jz      MO_118_ANI_List         ; НЕТ -
                cmp     fs:[edi].len_a, 7       ; АОН ОК ? или коротковат ?
                jae     MO_118_ANI_List         ; АОН ОК
                @Send_MO169                     ; АОН не ОК
                jmp     END_MO_118
MO_118_ANI_List:; --- проверяем АОН по списку
;;;                test    FG_PROGRAM, M_OR_11     ; АОН по списку проверяем ?
;;;                jz      END_MO_118              ; НЕТ -
;;;                call    Find_ANI                ; АОН ОК ? или эта линия не провeряется ?
;;;                jc      END_MO_118              ; ДА - или ДА -
;;;                @Send_MO169                     ; НЕТ - и НЕТ - отбой для МО
                ; ---
END_MO_118:     movzx	ax, ds:[bp + 4]
		add     bp, ax
		add     bp, 5
                jmp     SLED_MO

; ----= m119 =----------------------------------------------------------------
                        ;0    1     2-3
                        ;COD/ N_TS/ N_RAB
MO_119:                 ;НЕТ ИНФОРМАЦИИ АОН
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                call    Check_Work_Num_A
                jnc     END_MO_119
                ; ----
                test    FG_PROGRAM, M_OR_10     ; наличие АОН проверяем ?
                jz      END_MO_119              ; НЕТ -
                @Send_MO169                     ; ДА -
                ; ---
END_MO_119:     add     bp, 4
                jmp     SLED_MO

;;; ---------------------------------------------------------------------------
;;; --- ТАРИФИКАЦИОННЫЕ КОМАНДЫ - КОНЕЦ                                     ---
;;; ---------------------------------------------------------------------------


;;; ---------------------------------------------------------------------------
;;; --- ДВО - НАЧАЛО                                                        ---
;;; ---------------------------------------------------------------------------

DVO_ConvertTbl  LABEL BYTE
                DB      53, 26, 55, 56, 29, ?

DVO_ConvertTbl_Use      LABEL BYTE
                DB      93, 66, 95, 96, 94, ?

; макрос преобразования
@TranslateToUseDVO      MACRO
                LOCAL   @@L1, @@L2, @@L3
                push    si
                xor     si, si
                mov     cx, 5         ; длина массива
@@L1:           cmp     al, cs:DVO_ConvertTbl[si]
                je      @@L2
                inc     si
                loop    @@L1
                jmp     @@L3
@@L2:           mov     al, cs:DVO_ConvertTbl_Use[si]
@@L3:           pop     si
                ENDM

Tarif_DVO       PROC
                push    es
                pushad
                mov     esi, edi
                call    PUMA_Start_Tar
                mov     al, 090h                ; код сообщения о ДВО
                @_STOSB
                call    TimestampToPUMA         ; короткая временная метка события
                mov     al, ds:[bp + 4]         ; код самого ДВО
                cmp     ds:[bp], BYTE PTR 123   ; использование?
                jne     TD_Code
		@TranslateToUseDVO
TD_Code:        @_STOSB
                ; для будильников заготовим физ. № из команды
                movzx   ebx, BYTE PTR ds:[bp]   ; \
                shl     ebx, _PHYS_NUM_SHL      ;  > получаем из команды физ. № линии
                or      bx, ds:[bp - 1]         ; /
                ; особые случаи ДВО
                cmp     al, 95                  ; однократный будильник после перекодировки (55)
                je      TD_Get_Cfg
                cmp     al, 96                  ; многократный будильник после перекодировки (56)
                je      TD_Get_Cfg
                cmp     al, 98                  ; п/а при занятости
                je      TD_no_Send
                cmp     al, 99                  ; п/а при неответе
                je      TD_no_Send
                ; стандартные ДВО
                movzx   ebx, fs:[esi].ck_a      ; \
                shl     ebx, _PHYS_NUM_SHL      ;  > получаем из SOED физ. № линии А
                or      bx, fs:[esi].phys_a     ; /
                mov     ax, ds:[bp + 2]         ; возьмем рабочий номер из команды
                cmp     ax, fs:[esi].work_a     ; это № а ?
                je      TD_Get_Cfg              ; ДА -
                movzx   ebx, fs:[esi].ck_b      ; \
                shl     ebx, _PHYS_NUM_SHL      ;  > получаем из SOED физ. № линии Б
                or      bx, fs:[esi].phys_b     ; /
TD_Get_Cfg:     ;
                mov     eax, ebx                ; запомним
                shr     ebx, 9                  ;
                shl     ebx, 3                  ; ук. на общую конфигурацию блока
                mov     ebx, gs:[ebx + 1]       ; ук. на конфигурацию линий блока
                cmp     ebx, Cfg_Free_Ptr       ; ук. хорош ?
                jae     TD_no_Na                ; НЕТ -
                and     eax, 1ffh               ;
                shl     eax, 4                  ;
                add     ebx, eax                ; ук. на конфигурацию конкретной линии
                cmp     ebx, Cfg_Free_Ptr       ; ук. хорош ?
                jae     TD_no_Na                ; НЕТ -
                mov     eax, gs:[ebx + 10]      ; № телефона из конфигурации линии
                ;
TD_Send:        @_STOSD
                mov     al, 255                 ; \ пока балласт ! но можно расширить из
                @_STOSB                         ; / префикса до федерального номера !
                call    PUMA_Send_Tar
TD_no_Send:     popad
                pop     es
                ret
                ; --- если не удаётся определить №а - пошлём отладку
TD_no_Na:       mov     eax, ds:[bp]
                jmp     TD_Send
Tarif_DVO       ENDP


; ----= m122 =----------------------------------------------------------------
MO_122          PROC            ;АКТИВИЗАЦИЯ ДВО
                ;1     3     4
                ;COD/N_TIS/N_RAB/DVO
                ; проверка номера ЦК
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                ; проверка рабочего номера
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                call    Check_Work_Num_AB
                jnc     END_MO_122
                ; --- ДВО в ПУМу
                call    Tarif_DVO
                ;
END_MO_122:     add     bp, 5
                jmp     SLED_MO
MO_122          ENDP

; ----= m123 =----------------------------------------------------------------
; ИСПОЛЬЗОВАНИЕ ДВО
;      1       2       4
;COD / N_TIS / N_RAB / COD_DVO /
MO_123          PROC
                ; проверка номера ЦК
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                ; проверка на будильник, т.к. у него физический, а не рабочий номер
                cmp     ds:[bp + 4], BYTE PTR 55        ; одноразовый будильник
                je      MO_123_Ring
                cmp     ds:[bp + 4], BYTE PTR 56        ; многоразовый будильник
                jne     MO_123_no_Ring
MO_123_Ring:    ; проверка физического номера
                cmp     WORD PTR ds:[bp + 2], _PHYS_NUM_MAX
                jae     BAD_MO
                call    DVO_Alarm                       ; будильники для СОРМа
                jmp     MO_123_Tar_DVO                  ; будильники для СИДСа
                ; проверка рабочего номера
MO_123_no_Ring: cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                call    Check_Work_Num_AB
                jnc     END_MO_123
                ; --- ДВО в ПУМу
MO_123_Tar_DVO:	call    Tarif_DVO
                ; --- HT - hot telephon ---------------------------------------
MO_123_HT:      cmp     BYTE PTR ds:[bp + 4], MO_HT     ;
                jne     MO_123_HOLE                     ;
                mov     fs:[edi].dvo_code, HT           ;
                mov     fs:[edi].dvo_phase_code, PHASE_USE_DVO
                ; ---{Paul} --- {04/12/11}---
                test    fs:[edi].soed_flags, sf_UPATS_A
                jz      END_MO_123
                ; --- Creating 44 mess(using DVO) ----
                push    es
                push    eax bx cx edx si
                push    ds                                      ;
                pop     es                                      ;
                lea     si, UPATS_Buff                          ;
                mov     eax, DWORD PTR fs:[edi].upats_a_num     ;
                call    Ux_UnPack                               ;
                mov     eax, es:[si]
                ; --- найдем все сессии с № А ---
                mov     cx, Obj_Count
                mov     es, ss:SEG_SORM
                xor     si, si
@@find_obj:     mov     edx, DWORD PTR es:[si].sorm_num_a
                cmp     eax, edx
                je      @@find_ok
                add     si, _SORM_SIZE
                loop    @@find_obj
                jmp     @@exit_HT                               ; сессии закончились!
@@find_ok:      test    es:[si].sorm_flags, ssf_REL
                jnz     @@no_sess
                mov     bx, WORD PTR fs:[edi].dvo_code
                mov     WORD PTR es:[si].sorm_dvo_code, bx
                mov     dl, SO_DVO                              ;
                call    Message_SORM                            ;
@@no_sess:      jcxz    @@exit_HT
                add     si, _SORM_SIZE
                loop    @@find_obj
@@exit_HT:      pop     si edx cx bx eax
                pop     es
                ; --- {END} ----
                jmp     END_MO_123                      ;
                ; --- HOLE ----------------------------------------------------
MO_123_HOLE:    cmp     BYTE PTR ds:[bp + 4], MO_HOLE   ;
                jne     MO_123_HOLD                     ;
                mov     fs:[edi].dvo_code, HOLE         ;
		mov	SORM_DVO_Flag, ssf_HOLE		;
                jmp     MO_123_DVO                      ;
                ; --- HOLD ----------------------------------------------------
MO_123_HOLD:    cmp     BYTE PTR ds:[bp + 4], MO_HOLD   ;
                jne     MO_123_CC                       ;
                mov     fs:[edi].dvo_code, HOLD         ;
		mov	SORM_DVO_Flag, ssf_HOLD		;
                jmp     MO_123_DVO                      ;
                ; --- CC ------------------------------------------------------
MO_123_CC:      cmp     BYTE PTR ds:[bp + 4], MO_CC     ;
                jne     MO_123_CW                       ;
                mov     fs:[edi].dvo_code, CC           ;
		mov	SORM_DVO_Flag, ssf_CC  		;
                jmp     MO_123_DVO                      ;
                ; --- CW ------------------------------------------------------
MO_123_CW:      cmp     BYTE PTR ds:[bp + 4], MO_CW     ;
                jne     END_MO_123                      ;
                mov     fs:[edi].dvo_code, CW           ;
		mov	SORM_DVO_Flag, ssf_CW  		;
MO_123_DVO:     ; --- собственно отправка -------------------------------------
                mov     gs, ss:SEG_OBJ                  ;
                mov     fs:[edi].dvo_phase_code, PHASE_USE_DVO
                ; А или Б выполнил ДВО ?
                mov     al, ds:[bp + 1]                 ;
                cmp     al, fs:[edi].ck_a               ;
                jne     MO_123_B_side                   ;
                mov     ax, ds:[bp + 2]                 ;
                cmp     ax, fs:[edi].work_a             ;
                je      MO_123_A_side                   ;
                ; ---
MO_123_B_side:  call    Session_DVO_B                   ; ДВО от абонента Б
                jmp     END_MO_123
                ; ---
MO_123_A_side:	call    Session_DVO_A                   ; ДВО от абонента А
                ; ---
END_MO_123:     ADD     BP,5
                JMP     SLED_MO
MO_123          ENDP

; ----= MO_155 = - вызов абонента УПАТС --------------------------------------
                        ;0    1     2-3    4               6
                        ;COD/ N_TS/ N_RAB/ N log in UPATS/ N UPATS
MO_155:                 ;
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                call    Check_Work_Num_A
                jnc     END_MO_155
                ; --- приём номера Б - УПАТС
                or      fs:[edi].soed_flags, sf_UPATS_B + sf_NUM_FULL ;
                mov     ax, ds:[bp + 4]                  ;
                mov     fs:[edi].upats_b_num, ax         ;
                mov     al, ds:[bp + 6]                  ;
                mov     fs:[edi].upats_b, al             ;
                ; -- {Paul} -- <7/11/2012>
                ; --- отметим вызов абонента УПАТС
                or      fs:[edi].soed_flags, sf_CALL_UPATS
                ; -- {Paul} -- <END>
END_MO_155:     add     bp, 7
                jmp     SLED_MO


; ----= MO_156 = - передача вызова -------------------------------------------
MO_156:
                ; --- проверка корректности рабочих номеров
                cmp     BYTE PTR ds:[bp + 1], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 2], _WORK_NUM_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 4], _WORK_NUM_MAX
                jae     BAD_MO
                ; --- привязка ко 2-му соединению (1-ый рабочий номер)
                call    Check_Work_Num_AB
                jnc     END_MO_156
                mov     ebx, edi
                ; --- привязка к 1-му соединению (2-ой рабочий номер)
                mov     ax, ds:[bp + 2]
                mov     PEREM_AX, ax
                mov     ax, ds:[bp + 4]
                mov     ds:[bp + 2], ax
                call    Check_Work_Num_AB
                jnc     END_MO_156
                mov     ax, PEREM_AX
                mov     ds:[bp + 2], ax
                ; ---
                mov     gs, ss:SEG_OBJ
		mov	SORM_DVO_Flag, ssf_CT   	;
                ; --- фиксируем по обоим соединениям ДВО
                mov     fs:[edi].dvo_phase_code, PHASE_USE_DVO
                mov     fs:[edi].dvo_code, CT
                mov     fs:[ebx].dvo_phase_code, PHASE_USE_DVO
                mov     fs:[ebx].dvo_code, CT
                ; --- формируем сообщения для СОРМа по первому соединению
                mov     PEREM_EBX, ebx	; указатель на второе соединение
                mov     PEREM_EDI, edi  ; указатель на первое соединение
                ; ---
                ; А или Б выполнил ДВО ?
                mov     al, ds:[bp + 1]                 ;
                cmp     al, fs:[edi].ck_a               ;
                jne     MO_156_B                        ;
                mov     ax, ds:[bp + 4]                 ;
                cmp     ax, fs:[edi].work_a             ;
                je      MO_156_A                        ;
                ; ---
MO_156_B:       call    Session_3PTY_CT_B               ; ДВО от абонента Б
                jmp     END_MO_156
                ; ---
MO_156_A:	call    Session_3PTY_CT_A               ; ДВО от абонента А
                ; ---
END_MO_156:     ADD     BP,6
                JMP     SLED_MO


; ----= MO_182 = - безусловная переадресация (must die !) --------------------
MO_182          PROC
                add     bp, 5                   ; на следующую команду
                jmp     sled_mo
MO_182          ENDP

;;; ---------------------------------------------------------------------------
;;; --- ДВО - НАЧАЛО                                                        ---
;;; ---------------------------------------------------------------------------


;;; ---------------------------------------------------------------------------
;;; --- 235-АЯ КОМАНДА - НАЧАЛО                                             ---
;;; ---------------------------------------------------------------------------

; ----= 235 =------------------------------------------------------------------
MO_235          proc

                mov     al, ds:[bp + 4]         ; возьмем что за тип подпакета
IF _MSP NE TRUE
if _DebugKonfig
	@dword_to_file eax,2
	call write_cr_lf_to_debug_file
endif
endif

; ----= 235/12 = - тарификация ЦК ---------------------------------------------
                cmp     al, 12                          ; тарификация от ЦК ?
                jne     @@no235_12                      ; нет, уйдем
                call    CallToPUMA                      ; обработка тарификации от ЦК
                jmp     @@exit_235
@@no235_12:


; ----= 235/16 = - отказ от ЦК включить прослушку -----------------------------
; --- <Yu> [14.04.2010] - контроль СОРМовской коммутации
                cmp     al, 16                          ; отказ ?
                jne     @@no235_16                      ; нет, уйдем
                ; ---
		mov     al, ds:[bp + 5]                 ; канал А
		mov     ah, ds:[bp + 6]                 ; канал Б
                ; ---
                mov     es, ss:SEG_SORM
                mov     cx, _SESSION_MAX
                xor     si, si
@@235_16_find:
		test    es:[si].sorm_flags, ssf_REL + ssf_CMND_8
                jnz     @@235_16_next
                cmp     es:[si].sorm_chnl_a, al
                jne     @@235_16_next
                cmp     es:[si].sorm_chnl_b, ah
                jne     @@235_16_next
                ;
@@235_16_free:  mov     dh, OSV_BAD                     ;
                call    KSL_Free                        ;
                jmp     @@exit_235
                ;
@@235_16_next:  add     si, _SORM_SIZE
                loop    @@235_16_find
                ;
                jmp     @@exit_235
@@no235_16:


; ----= 235/18 = - информация о переадресации ---------------------------------
                cmp     al, 18                  ; информация о переадресации ?
                jne     @@no235_18              ; НЕТ - дальше
                ; --- <Yu> [08.05.2010] - защита от двойной переадресации
                call    Is_Dbl_RA               ;
                jc      @@exit_235              ;
                ; --- <Yu> [08.05.2010] - защита от двойной переадресации
                movzx   si, ds:[bp + 3]         ;
                cmp     BYTE PTR ds:[si + bp + 1], _CK_MAX
                jae     @@exit_235;;;BAD_MO
                cmp     WORD PTR ds:[si + bp + 2], _WORK_NUM_MAX
                jae     @@exit_235;;;BAD_MO                  ;
                add     bp, si                  ;
                call    Check_Work_Num_A        ;
                jnc     @@no_235_work_num       ;
                sub     bp, si                  ;
                ; --- переадресацию в ПУМу
                pushad
                call    PUMA_Start_Tar          ;
                mov     al, 090h                ; код сообщения о ДВО
                @_STOSB                         ;
                call    TimestampToPUMA         ; короткая временная метка события
                mov     ah, ds:[bp + 8]         ; код самого ДВО
		mov	al, 97			;
		or	ah, ah			;
		jz	@@235_18_ra		;
		inc	al			;
		cmp	ah, 2                   ;
		je	@@235_18_ra		;
		inc	al			;
@@235_18_ra:    @_STOSB                         ;
                ; формируем в bx ук. на 1-ую цифру короткого №
                mov     bx, bp                  ;
                add     bx, 19                  ;
                movzx   cx, LENGTH_NUM          ;
                sub     bx, cx                  ;
                ; буфер под формируемый №
                mov     PEREM_EAX, 0ffffffffh   ;
                lea     si, PEREM_EAX           ;
                ; формируем сам №
@@235_18_N_get: mov     ax, [bx]                ; 2 очередные цифры
                shl     ah, 4                   ;
                rol     ax, 4                   ;
                mov     [si], al                ; упакуем эти 2 цифры и запишем
                add     bx, 2                   ;
                inc     si                      ;
                sub     cx, 2                   ;
                jz      @@235_18_N_is           ; в № чётное число цифр и все они обработаны
                jns     @@235_18_N_get          ; не все цифры № обработаны
                or      BYTE PTR [si - 1], 0fh  ; в № нечётное число цифр -> чистим последнюю тетраду
@@235_18_N_is:  mov     eax, PEREM_EAX          ;
                @_STOSD                         ;
                mov     al, 255                 ; \ пока балласт ! но можно расширить из
                @_STOSB                         ; / префикса до федерального номера !
                call    PUMA_Send_Tar           ;
                popad
                ; --- приём длины номера переадресации
                movzx   cx, ds:[bp + 3]         ;
                sub     cx, 1 + 3 + 1 + 10 + 3  ;
                cmp     cx, 18                  ;
                jbe     @@num_c_len             ;
                mov     cx, 18                  ;
@@num_c_len:    mov     fs:[edi].len_c, cl      ;
                ; --- приём самого номера переадресации
                cld                             ;
                xor     ebx, ebx                ;
                shr     cx, 1                   ;
                adc     cx, 0                   ;
                lea     si, ds:[bp + 19]        ;
@@num_c:        lodsw                           ; загрузим 2 цифры для записи
                shl     al, 4                   ;
                shr     ax, 4                   ;
                cmp     cx, 1                   ;
                jne     @@num_c_wr              ;
                test    fs:[edi].len_c, 1       ;
                jz      @@num_c_wr              ;
                or      al, 0f0h                ;
@@num_c_wr:     mov     fs:[edi + ebx].num_c, al;
                inc     ebx                     ;
                loop    @@num_c                 ;
                ; --- отметим переадресацию
		or	fs:[edi].soed_flags, sf_RA
                ; --- получение признака номера переадресации
                call    Num_C_Attr              ;
                ; --- формируем признак и код ДВО
                mov     fs:[edi].dvo_phase_code, PHASE_USE_DVO
                movzx   bx, ds:[bp + 8]         ;
                and     bx, RA_MASK             ;
                mov     al, RA_Codes[bx]        ;
                mov     fs:[edi].dvo_code, al   ;
                ; --- <Yu> [08.05.2010] - защита от двойной переадресации
                call    Set_RA                  ;
                jmp     @@exit_235
@@no235_18:

; ----= 235/22 = - тарификация МАТС -------------------------------------------
                cmp     al, 22          ; тарификация от МАТС
                jne     @@no235_22      ; нет, пропустим
                call    CallToPUMA      ; обработка тарификации от Матс
                jmp     @@exit_235
@@no235_22:

; ----= 235/61 = - отказ от транзитного ЦК  -----------------------------------
                cmp     al, 61
                jne     @@no235_61
;;;                call    ModErrToPUMA    ; сбои модемов в ПУМУ
                jmp     @@exit_235
@@no235_61:

; ----= 235/62 = - окончательный отказ от ЦК  ---------------------------------
                cmp     al, 62
                jne     @@no235_62
                call    CKFailToSOED    ; отказ ЦК в SOED
                jmp     @@exit_235
@@no235_62:

; ----= 235/101 = - потеря прослушиваемого соединения --------------------------
                cmp     al, 101
                jne     @@no235_101
                ; ---
                mov     al, ds:[bp + 8]                 ; канал А
                mov     ah, ds:[bp + 9]                 ; канал Б
                ; ---
                mov     es, ss:SEG_SORM
                mov     cx, _SESSION_MAX
                xor     si, si
@@235_101_find: test    es:[si].sorm_flags, ssf_REL + ssf_CMND_8
                jnz     @@235_101_next
                cmp     es:[si].sorm_chnl_a, al
                jne     @@235_101_next
                cmp     es:[si].sorm_chnl_b, ah
                jne     @@235_101_next
                ;
@@235_101_free: or      es:[si].sorm_flags, ssf_REL     ;
                mov     es:[si].sorm_opcode, PRI_TEX
                xor     dh, dh                          ;
                call    KSL_Free                        ;
                mov     dl, SO_RAZ
                call    Message_SORM
                jmp     @@exit_235
                ;
@@235_101_next: add     si, _SORM_SIZE
                loop    @@235_101_find
                ;
                jmp     @@log_235
@@no235_101:
; ----= 235/102 = - цифра подключения к ТСОП абонентом УПАТС --------
                cmp     al, 102
                jne     @@no235_102
                ; --- проверка корректности рабочих номеров ---
                cmp     BYTE PTR ds:[bp + 5], _CK_MAX
                jae     @@exit_235
                cmp     WORD PTR ds:[bp + 6], _WORK_NUM_MAX
                jae     @@exit_235
                mov     si, 4
                add     bp, si
                call    Check_Work_Num_A        ;
                jnc     @@no_235_work_num       ;
                sub     bp, si
                mov     al, ds:[bp + 8]         ; цифра выхода в ТСОП
                cmp     al, 0FFh
                je      @@exit_235
                ; --- занесем цифру подкл. ТСОП для УПАТС в соед.
                mov     fs:[edi].tcop_dig, al
                jmp     @@exit_235
@@no235_102:
; ----= 235/103 = - неполный набор номера абонента УПАТС --------
                cmp     al, 103
                jne     @@no235_103
                ; --- проверка корректности рабочих номеров ---
                cmp     BYTE PTR ds:[bp + 5], _CK_MAX
                jae     @@exit_235
                cmp     WORD PTR ds:[bp + 6], _WORK_NUM_MAX
                jae     @@exit_235
                mov     si, 4
                add     bp, si
                call    Check_Work_Num_A        ;
                jnc     @@no_235_work_num       ;
                sub     bp, si
                ; ---
                mov     ax, ds:[bp + 8]         ; цифры №Б УПАТС
                cmp     ax, 9999
                ja      @@exit_235
                push    bx
                mov     fs:[edi].upats_b_num, ax ;\
                mov     bl, fs:[edi].upats_a       ;№УПАТС и № группы для абонента Б
                mov     fs:[edi].upats_b, bl     ;/
                pop     bx
                or      fs:[edi].soed_flags, sf_UPATS_B ; для записи в сессию №Б УПАТС
                ; -- {Paul} -- <7/11/2012>
                ; --- отметим вызов абонента УПАТС
                or      fs:[edi].soed_flags, sf_CALL_UPATS
                ; -- {Paul} -- <END>
                jmp     @@exit_235
@@no235_103:
; ----= 235/111 = - трёхсторонняя конференц-связь -----------------------------
                cmp     al, 111                 ; ДВО - трёхсторонняя конференц-связь ?
                jne     @@no235_111             ; НЕТ - дальше
                ; --- проверка корректности рабочих номеров
                cmp     BYTE PTR ds:[bp + 5], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 6], _WORK_NUM_MAX
                jae     BAD_MO
                cmp     BYTE PTR ds:[bp + 8], _CK_MAX
                jae     BAD_MO
                cmp     WORD PTR ds:[bp + 9], _WORK_NUM_MAX
                jae     BAD_MO
                ; --- привязка ко второму соединению
                mov     si, 4
                add     bp, si
                call    Check_Work_Num_A
                jnc     @@no_235_work_num
                sub     bp, si
                mov     ebx, edi
                ; --- привязка к первому соединению
                mov     si, 7
                add     bp, si
                call    Check_Work_Num_AB
                jnc     @@no_235_work_num
                sub     bp, si
                ; ---
                mov     gs, ss:SEG_OBJ
		mov	SORM_DVO_Flag, ssf_3PTY 	;
                ; --- фиксируем по обоим соединениям ДВО
                mov     fs:[edi].dvo_phase_code, PHASE_USE_DVO
                mov     fs:[edi].dvo_code, _3PTY
                mov     fs:[ebx].dvo_phase_code, PHASE_USE_DVO
                mov     fs:[ebx].dvo_code, _3PTY
		; ---
                mov     PEREM_EBX, ebx	; указатель на второе соединение
                mov     PEREM_EDI, edi  ; указатель на первое соединение
                ; А или Б выполнил ДВО ?
                mov     al, ds:[bp + 8]                 ;
                cmp     al, fs:[edi].ck_a               ;
                jne     @@235_111_B                     ;
                mov     ax, ds:[bp + 9]                 ;
                cmp     ax, fs:[edi].work_a             ;
                je      @@235_111_A                     ;
                ; ---
@@235_111_B:    call    Session_3PTY_CT_B               ; ДВО от абонента Б
                jmp     @@exit_235
                ; ---
@@235_111_A:	call    Session_3PTY_CT_A               ; ДВО от абонента А
                jmp     @@exit_235
                ; ---

@@no235_111:
; ----= 235/132 -  Протоколирование работы оператора =----
                cmp     al, 132
                jne     @@no235_132
                call    PUMA_Start_Tar
                mov     al, 93h
                @_STOSB
                call    TimestampToPUMA
                ; --- {Paul.N} --- <20/05/13>
                xor     eax, eax
                movzx   ecx, BYTE PTR ds:[bp + 3]
                dec     ecx
                cmp     ecx, 22
                jb      @@go_add
                ; -- здесь ecx >= 22
                mov     ecx, 22                       ; обрежем инфу до 22 байт
                jmp     @@go_rec
   @@go_add:    ; -- здесь ecx < 22
                mov     eax, 22
                sub     eax, ecx                      ; столько не хватает до 22 -> eax
   @@go_rec:    lea     esi, ds:[bp + 5]
                DB      67h
                rep     movsb
                cmp     eax, 0
                je      @@go_send
                ; -- необходимо добавить недостающие байты
                mov     ecx, eax
                mov     al, 0FFh
                DB      67h
                rep     stosb                         ; добъем значением -> 0FFh
                ; --- {Paul.N} --- <END>
  @@go_send:    call    PUMA_Send_Tar
                jmp     @@exit_235
@@no235_132:
; ----= 235/133 = - логирование МО в ПУМу --------------------------------------
; --- <Yu> [14.04.2010] - контроль СОРМовской коммутации
                cmp     al, 133
                jne     @@no235_133
@@log_235:      test    FG_PROGRAM, M_OR_14
                jz      @@exit_235
                call    PUMA_Start_Tar
                mov     al, 08fh
                @_STOSB
                mov     al, 0fdh
                @_STOSB
                mov     si, bp
                movzx   cx, [si + 3]
                add     cl, 4
                mov     al, cl                  ; длина
                @_STOSB
@@log_235_Loop: mov     al, [si]
                @_STOSB
                inc     si
                loop    @@log_235_Loop
                call    PUMA_Send_Tar
                jmp     @@exit_235
@@no235_133:

; ----= 235/150 = - длительность занятия (сбой) вызывающей линии --------------
                cmp     al, 150
                jne     @@no235_150
IF _MSP NE TRUE
                if _NO_AL_JITTER
                        cmp     BYTE PTR ds:[bp + 3], 9
                        jne     @@235_150
                        cmp     BYTE PTR ds:[bp + 12], 0
                        jne     @@235_150
                        cmp     BYTE PTR ds:[bp + 11], 93h
                        je      @@exit_235
                        @@235_150:
                endif
ENDIF
                ; ---
                call    GetCallPtr      	; получим указатель на соединение
		; ---
                cmp     Is_Call_Num, 0          ; есть ук-ль на соединение ?
                je      @@exit_235              ; НЕТ - ловить нечего !!!
                cmp     BYTE PTR ds:[bp + 3], 6    ; есть дополнительные поля ?
                jbe     @@235_150_Std              ; НЕТ - на стандартную обработку
                call    DVO_SR                  ; назначение или отмена ДВО с клавиатуры

@@235_150_Std:  call    LnLoadToPUMA    ; запись длительности занятия (и сбоя) вызывающей линии
                call    FailedCallsToPUMA;запись несостоявшихся соединений в ПУМУ (если надо)
                jmp     @@exit_235
@@no235_150:

; ----= 235/151 = - длительность занятия (сбой) вызываемоей линии -------------
                cmp     al, 151
                jne     @@no235_151
                call    GetCallPtr      ; получим указатель на соединение
                call    LnLoadToPUMA    ; запись длительности занятия (и сбоя) вызываемой линии
                call    FailedCallsToPUMA;запись несостоявшихся соединений в ПУМУ (если надо)
                jmp     @@exit_235
@@no235_151:

; ----= 235/152 = - поминутный срез модемной нагрузки ЦК ----------------------
                cmp     al, 152
                jne     @@no235_152
                call    ModLoadToPUMA   ; модемную нагрузку в ПУМУ
                jmp     @@exit_235
@@no235_152:

;-----= 235/171 =---- запрос разрешения на установление соединения от АЛ или СЛ к тарификатору
                cmp     al, 171
                jne     @@no235_171
                PUSH    BP
                add     bp, 4
                ; проверим корректности данных в команде
                movzx   cx, ds:[bp + 4]
                jcxz    BAD_MO_235_171
                push    bp
                movzx   ax, ds:[bp + 4]
                add     bp, 5
                add     bp, ax
                movzx   cx, ds:[bp]
                POP     BP
                OR      CX, CX
                JZ      BAD_MO_235_171
                ; =--- здесь порядок, будем работать по-новому!
                call    Black_List_NEW
                cmp     BL_Return_Code, 1
                je      BAD_MO_235_171
                call    Black_List_In_NEW
BAD_MO_235_171:
                pop     bp
                jmp     @@exit_235
@@no235_171:


; ----= 235/209 = - подтверждение от ЦК на получение тарификационной сети -----
                cmp     al, 209
                jne     @@no235_209
        ; - copy cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 0f6h
                @_STOSB
                mov     al, ds:[bp + 5]
                @_STOSB
                call    PUMA_Send_Tar
        ; - copy cmd to PUMA

                ; --- проверка
                movzx   bx, ds:[bp + 5]                 ; номер ЦК
                cmp     bl, Cfg_Curr_CK                 ; тот что ждём ?
                jne     BAD_MO                          ; НЕТ -
                ; --- приём флага
                or      Cfg_CK_Flags[bx], cf_RECV_TN    ; отметим приём подтверждения
                jmp     @@exit_235
@@no235_209:
; ----= 235/211 = - общая конфигурация ЦК -------------------------------------
                cmp     al, 211
                jne     @@no235_211
                ; - copy cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 0f8h
                @_STOSB
                movzx   ecx, BYTE PTR ds:[bp + 3]
                dec     cx
                movzx   esi, bp
                add     si, 5
                @_RMOVSB
                call    PUMA_Send_Tar
                ; - copy cmd to PUMA
                ; --- проверка
                movzx   ebx, BYTE PTR ds:[bp + 5]       ; номер ЦК
                cmp     bl, Cfg_Curr_CK                 ; тот что ждём ?
                jne     BAD_MO                          ; НЕТ -
                mov     gs, SS:SEG_KONF
                push    gs
                pop     es
                ; --- формируем конфигурацию для каждого блока ЦК
                shl     ebx, 7                          ; 128 байт конфигурации на ЦК
                mov     edi, ebx
                mov     cx, 16                          ; 16 блоков на ЦК
                push    bp
                add     bp, 12                          ; переставим указатель на список блоков
@@blk_loop:     push    cx
		movzx   bx, ds:[bp]                     ; тип очередного блока
                cmp     bl, 255                         ; блок подключен ?
                je      @@blk_next                      ; НЕТ -
                mov     gs:[edi], bl                    ; запомним тип блока
                mov     al, TAB_TIP_BL[bx]              ; получим то типу блока обобщённый тип
                mov     cl, TAB_TIP_LN[bx]              ; и размер конф. линии в сдвигах
                mov     gs:[edi + 5], al                ; запомним обощённый тип
                shl     bx, 1
                movzx   edx, TAB_DL_BL[bx]              ; получим по типу * 2 число линий в блоке
                mov     gs:[edi + 6], dx                ; и запомним его
                ; --- {Paul} -- <15/06/12> --
		cmp	BYTE PTR gs:[edi], 4		; ЦК ?
                je      @@blk_next  ;;!!было - @@blk_OK ; ДА - память под конфигурацию линий не выделяем
                ; --- {Paul} -- <END>
                shl     edx, cl                         ; столько байт под все линии текущего блока
                ; --- {Paul} - <15/12/12>
                cmp     DWORD PTR gs:[edi + 1], -1      ; конфигурация блока уже есть?
                jne     @@clear_conf                    ; возъмем готовый участок памяти сег.конфигурации
                mov     ebx, Cfg_Free_Ptr               ; ук. на свободное место в сегменте конф.
                mov     eax, edx
                add     eax, ebx                        ; двигаем ук. на свободное место
                cmp     eax, DLIN_KONF                  ; выскочили за сегмент ?
                jbe     @@blk_OK                        ; НЕТ -
                ; --- переполнение сегмента конфигурации
		pop	cx
                pop     bp                              ; ДА - не будем отмечать приём общей конф.
                mov     al, ds:[bp + 5]                 ; номер текущего ЦК
                mov     Cfg_Full, al                    ; запомним для индикации этой БЕДЫ !
                jmp     @@exit_235                      ; выход БЕЗ ПРИЁМА !
@@blk_OK:       ; --- очистим сегмент конфигурации под линии
                add     Cfg_Free_Ptr, edx
                mov     gs:[edi + 1], ebx               ; запомним только если всё ОК
                jmp     @@clear_good
@@clear_conf:   mov     ebx, gs:[edi + 1]
@@clear_good:   push    edi
                ; --- {Paul} -- <15/06/12> --
                mov     edi, ebx ;;;;;;;;;;;;;!!!! было - Cfg_Free_Ptr
                mov     ecx, edx
                shr     ecx, 2          ;;;;;;!!!! было - 4
                cmp     ecx, 0
                jz      @@not_clear_conf_ln
                mov     eax, 0ffffffffh
                @_RSTOSD
@@not_clear_conf_ln:
                ; --- {Paul} -- <END>
                pop     edi
                ; --- на обработку следующего блока ЦК
@@blk_next:     add	edi, 8
		inc     bp
		pop	cx
                dec     cx
                jnz     @@blk_loop
                pop     bp
		; ---
		mov	eax, ds:[bp + 28]
                cmp     eax, 0ffffffffh
		je	@@no_prefix
                ; --- получаем полный префикс для № А
                xchg    al, ah
                rol     al, 4
                rol     ah, 4
                ror     eax, 16
                shl     ax, 8
                shr     eax, 8
                rol     al, 4
                mov     PREF_A_FULL, eax        ; для изменения типов соединений в тар. пакетах
                ; вычисляем короткий префикс для № А
                and     ax, PREF_A_ZN_MASK
                mov     PREF_A_ZN, ax
                ; --- вычисляем префиксы для №Б из МГ префикса и префикса № А
                shl     ax, 4
                or      ax, PREF_B_MG
                mov     PREF_B_ZN, ax
                mov     eax, PREF_A_FULL
                shl     eax, 4
                or      ax, PREF_B_MG
                mov     PREF_B_FULL, eax
		; ---
@@no_prefix:    movzx   si, ds:[bp + 5]                 ; номер ЦК
                or      Cfg_CK_Flags[si], cf_RECV_CK    ; отметим приём общей конфигурации
                mov     Cfg_Curr_Mod, 0                 ; принимать конф. начнём с 1-го модема
                jmp     @@exit_235
@@no235_211:

; ----= 235/214 = - получение от ЦК пакета не требующего подтверждения -----------
                cmp     al, 214
                jne     @@no235_214
                ; - copy cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 0fah
                @_STOSB
                movzx   ecx, BYTE PTR ds:[bp + 3]
                dec     cx
                cmp     cx, 59
                jb      @@235_214_si
                mov     cx, 59
@@235_214_si:   movzx   esi, bp
                add     si, 5
                @_RMOVSB
                call    PUMA_Send_Tar
                ; - copy cmd to PUMA
                ; --- проверим ждём ли мы конфигурацию от этой линии
                mov     al, ds:[bp + 5]
                cmp     al, Cfg_Curr_CK
                jne     @@exit_235
                mov     ax, ds:[bp + 7]
                mov     dx, ax
                shr     ax, 9
                cmp     al, Cfg_Curr_Mod
                jne     @@exit_235
                and     dx, 1ffh
                cmp     dx, Cfg_Lines_Total
                jae     @@exit_235
                cmp     dx, Cfg_Curr_Line
                jne     @@request
                ; --- приём конфигурации линии
                movzx   eax, BYTE PTR ds:[bp + 5]
                movzx   cx, BYTE PTR ds:[bp + 3]
                add     bp, 6
                call    Get_Line_Cfg
                sub     bp, 6
                ; --- зафиксируем приём конфигурации линии
                inc     Cfg_Curr_Line           ; на одну линию принято больше
IF _MSP EQ TRUE
                mov     eax, Sys_Time           ; \ фиксируем время
ELSE
                mov     eax, Current_Time           ; \ фиксируем время
ENDIF
                mov     Cfg_TOut, eax           ; / приёма
                mov     Cfg_Attempt_Count, 1    ; счётчик попыток на случай возможного переспроса
		mov	Cfg_Line_Request, 0	;
                jmp     @@exit_235
                ; --- порядок следования линий нарушен, перезапрос
@@request:      cmp     Cfg_Line_Request, 0
		jne 	@@exit_235
                @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
                or      dl, dl                  ; есть возможность послать?
                jnz     @@exit_235              ; НЕТ -
                mov     eax, 40404ebh           ; 235-я длиной 4
                stosd
                mov     al, 213                 ; тип 235-той
                stosb
                mov     al, Cfg_Curr_CK         ; номер ЦК
                stosb
                movzx   ax, Cfg_Curr_Mod
                shl     ax, 9
                or      ax, Cfg_Curr_Line       ; физ. номер линии на ЦК
                stosw
                @ZAPIS_OK_MO                    ; пошлем
		mov	Cfg_Line_Request, 255	;
                jmp     @@exit_235
@@no235_214:

; ----= 235/215 = - получение от ЦК пакета требующего подтверждения -----------
                cmp     al, 215
                jne     @@no235_215
                ; - copy cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 0fch
                @_STOSB
                movzx   ecx, BYTE PTR ds:[bp + 3]
                dec     cx
                cmp     cx, 59
                jbe     @@235_215_si
                mov     cx, 59
@@235_215_si:   movzx   esi, bp
                add     si, 5
                @_RMOVSB
                call    PUMA_Send_Tar
                ; - copy cmd to PUMA
                ; --- приём конфигурации линии
                movzx   eax, BYTE PTR ds:[bp + 5]
                movzx   cx, BYTE PTR ds:[bp + 3]
		dec	cx
		add     bp, 7
                call    Get_Line_Cfg
                sub     bp, 7
                ; --- подтверждение приёма
                @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
                or      dl, dl                  ; есть возможность послать?
                jnz     @@exit_235              ; НЕТ -
                mov     eax, 30404ebh           ; 235-я длиной 3
                stosd
                mov     al, 216                 ; тип 235-той
                stosb
                mov     ax, ds:[bp + 5]         ; № ЦК и № пакета
                stosw
                @ZAPIS_OK_MO                    ; пошлем
                ; - copy cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 0fdh
                @_STOSB
                mov     ax, ds:[bp + 5]
                @_STOSW
                call    PUMA_Send_Tar
                ; - copy cmd to PUMA
                jmp     @@exit_235
@@no235_215:

@@exit_235:
                MOVZX   AX,DS:[BP+3]    ; узнаем длину
                ADD     BP,4            ; прибавим базу команды
                ADD     BP,AX           ; прибавим длину
                JMP     SLED_MO         ; на следующую команду
@@no_235_work_num:
                sub     bp, si
                jmp     @@exit_235
MO_235  ENDP

;;; ---------------------------------------------------------------------------
;;; --- 235-АЯ КОМАНДА - КОНЕЦ                                              ---
;;; ---------------------------------------------------------------------------


