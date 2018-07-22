; ----------------------------------------------------------------------------

LEV_L4:                                       ;ПРИЕМ С 3 УРОВНЯ
;;;!!!
     ;;;           call    SORM_Emul_cmnd_debug
                ; ---
;;;!!!
                MOV     GS,SS:SEG_OBJ
                ; --- команда получена по статистическому каналу
                mov     Oper_Do_flag, 2 ; команда получена по статистическому каналу
                cmp     bx, 2           ;;;!!!
                JE      BAD_POKET
                ; --- проверка допустимости кода команды
                mov     Oper_Do_Flag, 3 ; неизвестная команда
                MOV     AL,DS:[SI+2]
                OR      AL,AL
                JZ      BAD_POKET
                CMP     AL,11H
                JA      BAD_POKET
                ; --- проверка допустимости длины команды (и заодно пакета)
                mov     Oper_Do_Flag, 4 ; неверная длина команды
                movzx   di, al
                mov     al, [si + 3]
                cmp     al, COM_LEN[di]
                jne     BAD_POKET
		; --- проверка преамбулы
                mov     Oper_Do_Flag, 5 ; неверная преамбула команды
		cmp	BYTE PTR [si], 0cch
		jne	BAD_POKET
                ; --- для тестирования ничего не проверяем
                CMP     DI,COM_TES_
                JE      TESTIROVANIE
                ; --- проверка пароля для всех команд
                mov     Oper_Do_Flag, 6 ; неверный пароль
                MOV     EDX,DS:[SI+4]
                CMP     EDX,PAROL_0_3
                JNE     NETOT_PAROL
                MOV     DX,DS:[SI+8]
                CMP     DX,PAROL_4_5
                JNE     NETOT_PAROL
                ; --- для 1-ой команды № СОРМ и запуск не проверяем
                MOVZX   DI,DS:[SI+2]
                CMP     DI,COM_ZAP
                JE      ZAPUSK_SIST
                ; --- проверка № СОРМ для всех команд кроме запуска
                mov     Oper_Do_Flag, 7 ; неверный номер СОРМ
                MOV     AX,DS:[SI]
                CMP     AX,PREAMB_NOM_SORM
                JNE     BAD_POKET
                ; --- проверка запущен ли СОРМ для всех команд кроме запуска
                mov     Oper_Do_Flag, 8 ; СОРМ не запущен
                CMP     FG_SOST,1
                JNE     BAD_POKET
                ; --- проверка выдачи таблиц - при выдаче принимаем только 2, 7, 8, 13 и 14
                cmp     Bunch_Data_Run, 0
		jne	ne_OK_POKET
		cmp	Obj_Data_Run, 0
		je	OK_POKET
ne_OK_POKET:    cmp     BYTE PTR [si + 2], COM_OST      ; 2 - стоп СОРМ
		je	OK_POKET
                cmp     BYTE PTR [si + 2], COM_PDK      ; 7 - подключение к разговорному тракту
		je	OK_POKET
                cmp     BYTE PTR [si + 2], COM_OSV      ; 8 - освобождение КСЛ
		je	OK_POKET
                cmp     BYTE PTR [si + 2], COM_PRE      ; 13 - прерывание выдачи таблиц
		je	OK_POKET
                mov     Oper_Do_Flag, 9 ; идёт выдача таблиц
		jmp	BAD_POKET
                ; --- подтверждаем приём команды
OK_POKET:       PUSH    DI
                PUSH    SI
                MOV     AL,COM_POD
                MOV     BL,DS:[SI+2]
                MOV     BH,IND_GUD
                CALL    SORM_C
                POP     SI
                POP     DI
                ; --- фиксируем время приёма хоть чего-нибудь по командному каналу
IF _MSP EQ TRUE
                mov     eax, Sys_Time       ;;;
ELSE
                mov     eax, Current_Time   ;;;
ENDIF
                mov     SORM_Test_Time, eax ;;;
		mov 	SORM_Stoped, 0      ;;;
                ; --- и на исполнение команды
                ADD     SI,DLIN_ZAGOLOVKA
                SUB     DI,2
                SHL     DI,1
                JMP     DS:KOMANDI[DI]

BAD_POKET:      ; --- не подтверждаем приём команды
IF _MSP NE TRUE
                ; если режим эмуляуии - просто игнорируем
                cmp     Pass_OK, 0
                je      Send_Bad
                ret
ENDIF
Send_Bad:
                MOV     AL,COM_POD
                MOV     BL,DS:[SI+2]
                MOV     BH,Oper_Do_Flag
                JMP     SORM_C

NETOT_PAROL:    ; --- команда не с тем паролем
                MOV     AL,COM_POD
                MOV     BL,DS:[SI+2]
                MOV     BH,Oper_Do_Flag
                CALL    SORM_C
		cmp	FG_SOST, 0
		je	SORM_Stopped
Send_Cmd6:      MOV     AL,COM_NES
                MOV     BL,2
                JMP     SORM_C
SORM_Stopped:   cmp     BYTE PTR [si + 2], COM_ZAP      ; запуск
		je      Send_Cmd6
                ret


;
ZAPUSK_SIST:    ; *** 1 - ЗАПУСК СИСТЕМЫ ************************************************
                CMP     FG_SOST,0
                JNE     SISTEMA_NE_GOTOVA
IF _MSP EQ TRUE
        	mov 	eax, Sys_Time       ;;;
ELSE
                mov     eax, Current_Time       ;;;
ENDIF
        	mov 	SORM_Test_Time, eax ;;;
		mov 	SORM_Stoped, 0      ;;;

                MOV     AX,DS:[SI]
                MOV     PREAMB_NOM_SORM,AX
                ;
                @SORM_INIT_DATA
                ;
                MOV     FG_SOST,1
                MOV     FG_SOST_STOP, 0
                ;
                MOV     AL,COM_POD
                MOV     BL,COM_ZAP
                MOV     BH,IND_GUD
                CALL    SORM_C
                MOV     AL,COM_VIP
                MOV     BL,COM_ZAP
                MOV     BH,IND_GUD
                JMP     SORM_C

                ; --- попытка повторного запуска
SISTEMA_NE_GOTOVA:
                MOV     AL,COM_POD
                MOV     BL,DS:[SI+2]
                MOV     BH,IND_GUD
                PUSH    SI
                CALL    SORM_C
                POP     SI
                MOV     AL,COM_VIP
                MOV     BL,DS:[SI+2]
                MOV     BH,10           ; попытка повторного запуска СОРМ
                JMP     SORM_C

;
OSTANOV:        ; *** 2 - ОСТАНОВ СОРМ **************************************************
                @SORM_INIT_DATA
		; ---
		mov	Obj_Data_Run, 0
		mov	Obj_Data_Out_Len, 0
		; ---
		mov	Bunch_Data_Run, 0
		mov	Bunch_Data_Out_Len, 0
		; ---
                MOV     FG_SOST,0
                MOV     FG_SOST_STOP, 1
                ; ---
                MOV     AL,COM_POD
                MOV     BL,COM_OST
                MOV     BH,IND_GUD
                CALL    SORM_C          ; т.к. после INIT_DATA теряем 7-ую команду !
                MOV     AL,COM_VIP
                MOV     BL,COM_OST
                MOV     BH,IND_GUD
                JMP     SORM_C

PAROL:          ; *** 3 -
                MOV     EDX,DS:[SI]
                MOV     PAROL_0_3,EDX
                MOV     DX,DS:[SI+4]
                MOV     PAROL_4_5,DX
                MOV     AL,COM_VIP
                MOV     BL,COM_PAR
                MOV     BH,IND_GUD
                JMP     SORM_C

ZAKREPLENIE:    ; *** 4 - закрепление КСЛ за группой ************************************
                ; --- проверка правильности номера группы
                mov     Oper_Do_Flag, 40h       ; неверный номер группы
                cmp     BYTE PTR [si], 0        ;
                je      BAD_ZAKR
                cmp     BYTE PTR [si], _MAX_GROUP
                ja      BAD_ZAKR
                ; --- проверка правильности типа группы
                mov     Oper_Do_Flag, 41h       ; неверный тип группы
                movzx   bx, [si]                ; № группы из команды
                mov     dl, [si + 1]            ; тип группы из команды
                mov     ax, [si + 2]            ; №№ КСЛ из команды
		cmp	dl, RAZDEL              ; просим раздельное наблюдение ?
		je	ZAKR_Grp_Exist          ; ДА - можно
		cmp	dl, SOVMEST             ; просим совмещенное ?
		jne	BAD_ZAKR                ; НЕТ - но третьего не дано
ZAKR_Grp_Exist:
                mov     Oper_Do_Flag, 42h       ; тип КСЛ не соответствует типу группы
                cmp     KSL_Group_Type[bx], 0ffh; группа существует ?
                je      ZAKR_KSL
                cmp     KSL_Group_Type[bx], dl  ; тип существующей группы = типу в команде ?
                jne     BAD_ZAKR                ;
                ; --- проверка правильности №№ КСЛ
ZAKR_KSL:       mov     Oper_Do_Flag, 43h       ; неверный № КСЛ
                cmp     al, 0ffh                ; КСЛ-А не может быть ff
                je      BAD_ZAKR
                movzx   di, al
                cmp     KSL_Enabled[di], 1     ; в этой проверке № КСЛ может быть ff
                jne     BAD_ZAKR
                movzx   di, ah
                cmp     KSL_Enabled[di], 1     ; КСЛ-Б может быть ff
                jne     BAD_ZAKR
                ; --- проверка назначенности КСЛ
                mov     Oper_Do_Flag, 44h       ; КСЛ уже назначена
                movzx   di, al
                cmp     KSLs[di], 0ffh
                jne     BAD_ZAKR
                movzx   di, ah
                cmp     KSLs[di], 0ffh
                jne     BAD_ZAKR
                ; --- проверка соответствия типа группы и №№ КСЛ
                mov     Oper_Do_Flag, 45h       ; комбинация №№ КСЛ-А и КСЛ-Б не соответствует типу группы
                cmp     dl, SOVMEST
                jne     ZAKR_RAZDEL
                ; совмещённое наблюдение
ZAKR_SOVMEST:   cmp     ah, 0ffh
                je      ZAKR_SAVE
                cmp     al, ah
                jne     BAD_ZAKR
                mov     ah, 0ffh
                jmp     ZAKR_SAVE
                ; раздельное наблюдение
ZAKR_RAZDEL:    cmp     ah, 0ffh
                je      BAD_ZAKR
                cmp     al, ah
                je      BAD_ZAKR
                mov     Oper_Do_Flag, 46h       ; КСЛ-А и КСЛ-Б в разных потоках
                mov     cx, ax
                and     cx, 0e0e0h
                cmp     cl, ch
                jne     BAD_ZAKR
                ; --- заполнение таблиц СОРМ
ZAKR_SAVE:      cmp     KSL_Group[bx], 0ffh
                jne     ZAKR_OLD
                ; создаётся новая группа
ZAKR_NEW:       mov     KSL_Group[bx], bl
                mov     KSL_Group_Type[bx], dl
                mov     KSL_Count[bx], 1
                mov     KSL_Last[bx], 255
                jmp     ZAKR_BOTH
                ; добавляем КСЛ в существующую группу
ZAKR_OLD:       inc     KSL_Count[bx]
                ; одинаково для новой и старой группы
ZAKR_BOTH:      movzx   di, al
                mov     KSLs[di], bl
                mov     KSL_A[di], 0
                mov     KSL_Pair[di], ah
                mov     KSL_Busy[di], 255
		cmp	ah, 255
		je	GUT_ZAKR
                movzx   di, ah
                mov     KSLs[di], bl
                mov     KSL_A[di], 255
                mov     KSL_Pair[di], al
                mov     KSL_Busy[di], 255

                ; --- удачно
GUT_ZAKR:       MOV     AL,COM_VIP        ;
                MOV     BL,COM_ZAK        ;
                MOV     BH,IND_GUD        ;
                JMP     SORM_C            ;
                ; --- неудачно
BAD_ZAKR:       MOV     AL,COM_VIP        ;
                MOV     BL,COM_ZAK        ;
                MOV     BH,Oper_Do_Flag   ;
                JMP     SORM_C            ;

; ---------------------------------------------------------------------------------------

POSTANOVKA_OBJECT:; *** 5 - ПОСТАНОВКА ОБЪЕКТА НА НАБЛЮДЕНИЕ
                ; --- проверка занятости условного номера -------------------------------
POST_USL_NUM_TEL: ; для номера телефона
                mov     Oper_Do_Flag, 50h       ; условный номер уже используется
                cmp     Obj_Count, 0
                je      POST_TYPE
                mov     ax, [si]
                mov     cx, _BUNCH_MAX
                add     cx, Obj_Count
                lea     di, Bunches
POST_USL_NUM_Loop:
                cmp     WORD PTR gs:[di].obj_type, 0ffffh
                je      POST_USL_NUM_Next
                cmp     ax, gs:[di].obj_num
                je      BAD_POST_OBJ
POST_USL_NUM_Next:
                add     di, _OBJ_SIZE
                loop    POST_USL_NUM_Loop
                ; --- проверка типа объекта ---------------------------------------------
POST_TYPE:      mov     Oper_Do_Flag, 51h       ; неопределённый в ТТ тип объекта
                mov     al, [si + 2]
                ; --- UPATS ---------------------
                cmp     al, 0ffh                ;
                je      POST_ATTR_NUM           ;
                ; --- UPATS ---------------------
                or      al, al
                jz      BAD_POST_OBJ
                test    FG_PROGRAM, _fg_prg_cats; комбинированная !!!
                jnz     POST_TYPE_CATS
                cmp     al, BUNCH
                je      BAD_POST_OBJ
POST_TYPE_CATS: cmp     al, BUNCH
IF _MSP NE TRUE
                jbe     POST_ATTR_NUM
else
                jb      POST_ATTR_NUM
ENDIF
                cmp     al, NEPOLN_NOM
                jne     BAD_POST_OBJ
                ; --- проверка признака номера телефона ---------------------------------
POST_ATTR_NUM:  mov     Oper_Do_Flag, 52h       ; неопределённый в ТТ признак номера телефона
                mov     al, [si + 3]
                or      al, al
                jz      BAD_POST_OBJ
                cmp     al, PRIZN_SLUSHB
                jbe     POST_TYPE_vs_ATTR
                cmp     al, PRIZN_UNDEF
                jne     BAD_POST_OBJ
		; --- проверка соответствия типа объекта и признака номера телефона
POST_TYPE_vs_ATTR:
                mov     Oper_Do_Flag, 53h       ;тип объекта не соответствует признаку номера телефона
                ; --- UPATS ---------------------
                cmp     BYTE PTR [si + 2], 0ffh ;
                jne     POST_TYPE_STAN          ;
                cmp     BYTE PTR [si + 3], PRIZN_STAN
                je      POST_CAT_CTRL           ;
                jmp     BAD_POST_OBJ            ;
                ; --- UPATS ---------------------
POST_TYPE_STAN:	cmp	BYTE PTR [si + 2], AB_STANC
		jne	POST_TYPE_SETI
                cmp     BYTE PTR [si + 3], PRIZN_STAN        ; здесь только сама станция!
		je	POST_CAT_CTRL
                cmp     BYTE PTR [si + 3], PRIZN_SLUSHB      ; а вот и не только !
		je	POST_CAT_CTRL
                jmp     BAD_POST_OBJ
POST_TYPE_SETI:	cmp	BYTE PTR [si + 2], AB_SET
		je	POST_is_TYPE_SETI
		cmp	BYTE PTR [si + 2], NEPOLN_NOM
		jne	POST_TYPE_BUNCH
POST_is_TYPE_SETI:
           	cmp	BYTE PTR [si + 3], PRIZN_SETI
		jb	BAD_POST_OBJ
           	cmp	BYTE PTR [si + 3], PRIZN_SLUSHB
		ja	BAD_POST_OBJ
                jmp     POST_CAT_CTRL
POST_TYPE_BUNCH:cmp	BYTE PTR [si + 3], PRIZN_UNDEF
		jne	BAD_POST_OBJ
                ; --- проверка категории контроля ---------------------------------------
POST_CAT_CTRL:  mov     Oper_Do_Flag, 54h       ; неопределённая в ТТ категория контроля
                mov     al, [si + 16]
                or      al, al
                jz      BAD_POST_OBJ
                cmp     al, STATIC
                je      POST_PRIOR_STAT
                cmp     al, SOVMEST
                je      POST_PRIOR
                cmp     al, RAZDEL
                jne     BAD_POST_OBJ
                ; --- проверка метки приоритета -----------------------------------------
POST_PRIOR:     mov     Oper_Do_Flag, 55h       ; неопределённая в ТТ метка приоритета
                mov     al, [si + 18]
                or      al, al
                jz      BAD_POST_OBJ
                cmp     al, PRIOR_LO
                ja      BAD_POST_OBJ
                ; --- проверка номера группы --------------------------------------------
POST_GROUP:     mov     Oper_Do_Flag, 56h       ; Группа КСЛ с указанным номером не распределена
                movzx   bx, [si + 17]
                cmp     KSL_Group[bx], 0ffh
                je      BAD_POST_OBJ
                ; --- соответствие номера группы категории контроля ----------------------
POST_GROUP_vs_CAT_CTRL:
                mov     Oper_Do_Flag, 57h       ; Тип группы КСЛ не соответствует категории контроля
                mov     al, KSL_Group_Type[bx]
                cmp     al, [si + 16]
                jne     BAD_POST_OBJ
                jmp     POST_TEL_or_BUNCH
                ; --- проверка метки приоритета -----------------------------------------
POST_PRIOR_STAT:mov     Oper_Do_Flag, 58h       ; Неверные параметры при статистической категории контроля
                cmp     WORD PTR [si + 17], 0ffffh
                jne     BAD_POST_OBJ
                ; --- пучок или телефон ? -----------------------------------------------
POST_TEL_or_BUNCH:
                mov     Oper_Do_Flag, 59h       ; одновременно заданы номер телефона и номер пучка
                cmp     BYTE PTR [si + 2], BUNCH
                jne     POST_TEL_NUM
                ; --- дополнительная проверка для пучка ---------------------------------
POST_BUNCH:     cmp     DWORD PTR [si + 3], 0ffffffffh
                jne     BAD_POST_OBJ
IF _MSP NE TRUE
                ; проверка наличия такого пучка в АТС
POST_BUNCH_is:  mov     Oper_Do_Flag, 5fh       ; пучок с указанным номером не существует
                mov     es, ss:SEG_BUNCH
                mov     ax, [si + 14]
                mov     cx, _BUNCH_MAX
                mov     edi, _BUNCH_PRM_OFFS
POST_BUNCH_is_Loop:
                cmp     BYTE PTR es:[edi], 0ffh
                je      POST_BUNCH_is_Next
                cmp     ax, WORD PTR es:[edi + 1]
                je      POST_BUNCH_DOUBLE
POST_BUNCH_is_Next:
                add     edi, 4
                loop    POST_BUNCH_is_Loop
                jmp     BAD_POST_OBJ
                ; проверка повторной постановки пучка
POST_BUNCH_DOUBLE:
                mov     Oper_Do_Flag, 5ch       ; объект уже стоит на контроле
                mov     ax, [si + 14]
                mov     cx, _BUNCH_MAX
                lea     di, Bunches
POST_BUNCH_DOUBLE_Loop:
                cmp     WORD PTR gs:[di].obj_type, 0ffffh
                je      POST_BUNCH_DOUBLE_Next
                cmp     ax, gs:[di].obj_bunch
                je      BAD_POST_OBJ
POST_BUNCH_DOUBLE_Next:
                loop    POST_BUNCH_DOUBLE_Loop
                ; --- постановка пучка на контроль
                mov     cx, _BUNCH_MAX
                lea     di, Bunches
POST_BUNCH_Loop:cmp     WORD PTR gs:[di].obj_type, 0ffffh
                je      POST_OBJ
                add     di, _BUNCH_SIZE
                loop    POST_BUNCH_Loop
                mov     Oper_Do_Flag, 5dh       ; на контроле стоит max допустимое число объектов
                jmp     BAD_POST_OBJ
ENDIF
                ; --- дополнительная проверка для номера телефона -----------------------
POST_TEL_NUM:   cmp     WORD PTR [si + 14], 0ffffh
                jne     BAD_POST_OBJ
                ; --- проверка на допустимые символы
                mov     Oper_Do_Flag, 5ah       ; неверный формат № телефона - недопустимые символы или длина не соответсвует N
                mov     bx, 5			; смещение N от начала команды
                mov     cx, 9                   ; байт в номере
		movzx	dx, [si + 4]		; dl - длина, dh - 0
POST_TEL_Bad_Loop:
		inc	dh
                mov     al, [si + bx]
                and     al, 0fh
		cmp	al, 0ah			; \
		jb      POST_TEL_Bad_Next_10    ;  \ цифра от A до Е -
		cmp	al, 0fh                 ;  / это всегда ошибка
		jne	BAD_POST_OBJ            ; /
		cmp	dh, dl                  ; код F внутри N ?
		jbe	BAD_POST_OBJ            ; ДА - ошибка
		jmp     POST_TEL_Bad_Next_11    ; НЕТ - после последней цифры
POST_TEL_Bad_Next_10:
		cmp	dh, dl                  ; код 0..9 после N ?
		ja	BAD_POST_OBJ            ; ДА - ошибка
POST_TEL_Bad_Next_11:
		inc	dh
                mov     al, [si + bx]
                shr     al, 4
		cmp	al, 0ah			; \
		jb      POST_TEL_Bad_Next_20    ;  \ цифра от A до Е -
		cmp	al, 0fh                 ;  / это всегда ошибка
		jne	BAD_POST_OBJ            ; /
		cmp	dh, dl                  ; код F внутри N ?
		jbe	BAD_POST_OBJ            ; ДА - ошибка
		jmp     POST_TEL_Bad_Next_21    ; НЕТ - после последней цифры
POST_TEL_Bad_Next_20:
		cmp	dh, dl                  ; код 0..9 после N ?
		ja	BAD_POST_OBJ            ; ДА - ошибка
POST_TEL_Bad_Next_21:
                inc     bx
                loop    POST_TEL_Bad_Loop
                ; --- проверка соответствия признака номера и самого номера -------------
POST_TEL_ATTR_vs_NUM:
                mov     Oper_Do_Flag, 5bh       ; признак номера не соответствует номеру
                ; --- UPATS ---------------------
                cmp     BYTE PTR [si + 2], 0ffh ;
                jne     POST_NUM_TEL            ;
POST_U_NUM:     cmp     BYTE PTR [si + 3], PRIZN_STAN ;
                jne     BAD_POST_OBJ            ;
                cmp     BYTE PTR [si + 4], 5    ;
                jb      BAD_POST_OBJ            ;
                cmp     BYTE PTR [si + 4], 6    ;
                ja      BAD_POST_OBJ            ;
                mov     al, [si + 5]            ;
                and     al, 0fh                 ;
                mul     BYTE PTR __VAL_10       ;
                mov     dl, [si + 5]            ;
                shr     dl, 4                   ;
                add     al, dl                  ;
                movzx   ebx, al                 ;
                shl     ebx, 16                 ;
                movzx   eax, BYTE PTR [si + 6]  ;
                and     al, 0fh                 ;
                mul     BYTE PTR __VAL_10       ;
                mov     dl, [si + 6]            ;
                shr     dl, 4                   ;
                add     al, dl                  ;
                mul     WORD PTR __VAL_10       ;
                movzx   dx, [si + 7]            ;
                and     dx, 0fh                 ;
                add     ax, dx                  ;
                cmp     BYTE PTR [si + 4], 5    ;
                je      POST_U_FIND             ;
                mul     WORD PTR __VAL_10       ;
                movzx   dx, [si + 7]            ;
                shr     dx, 4                   ;
                add     ax, dx                  ;
POST_U_FIND:    add     eax, ebx                ;
                push    esi                     ; портим esi !
                DB      9AH
                DW      OFFSET Find_U_Num_Far   ; ищем в SEG_ULOG
                DW      120
                pop     esi                     ;
                jnc     BAD_POST_OBJ            ; не НАШЛИ
                jmp     POST_TEL_DOUBLE         ;
                ; --- UPATS ---------------------
                ; преобразуем № наблюдаемого в формат годный для поиска SEG_NLOG
POST_NUM_TEL:   mov     eax, [si + 5]           ; первые 8 цифр номера
                rol     al, 4
                rol     ah, 4
                xchg    al, ah
                rol     eax, 16
                rol     al, 4
                rol     ah, 4
                xchg    al, ah
                movzx   bx, LENGTH_NUM
                mov     cl, Cfg_Num_Shift[bx]
                shr     eax, cl
                xor     dh, dh                  ; dh == НАШЛИ
		push	edx                     ; портим edx !
	        push    esi                     ; портим esi !
                DB      9AH
                DW      OFFSET Find_Tel_Num_Far ; ищем в SEG_NLOG
                DW      120
                pop     esi                     ;
		pop	edx			;
                jc      K_POST_TEL_ATTR_STAN           ; НАШЛИ
                not     dh                      ; dh != НЕ НАШЛИ
K_POST_TEL_ATTR_STAN:     ; ---------------------------------
                test    FG_PROGRAM, _fg_prg_cats; комбинированная !!!
                jz      POST_TEL_ATTR_not_CATS
                ; === КАТС - начало ======================================================
                cmp     BYTE PTR [si + 3], PRIZN_STAN
                jne     K_POST_TEL_ATTR_SETI
                or      dh, dh
                jnz     BAD_POST_OBJ            ; НЕ НАШЛИ
		jmp	POST_TEL_DOUBLE

K_POST_TEL_ATTR_SETI:     ; ---------------------------------
                cmp     BYTE PTR [si + 3], PRIZN_SETI
                jne     K_POST_TEL_ATTR_ZONI
                or      dh, dh
                jz      BAD_POST_OBJ            ; НАШЛИ
		mov	ax, [si + 5]
		and	ax, PREF_B_MG_MASK
		cmp	ax, PREF_B_MG
		je	BAD_POST_OBJ
		mov	ax, [si + 5]
		and	ax, PREF_B_MN_MASK
		cmp	ax, PREF_B_MN
		je	BAD_POST_OBJ
                mov     al, LENGTH_NUM
                cmp     BYTE PTR [si + 2], NEPOLN_NOM
                jne     POST_TEL_ATTR_SETI_full
                cmp     al, [si + 4]		; абонент местной сети с неполным номером
                jbe     BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE
K_POST_TEL_ATTR_SETI_full:
                cmp     al, [si + 4]            ; абонент местной сети с полным номером
                jne     BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE

K_POST_TEL_ATTR_ZONI:     ; ---------------------------------
                cmp     BYTE PTR [si + 3], PRIZN_ZONI
                jne     K_POST_TEL_ATTR_MESHGOR
                or      dh, dh
                jz      BAD_POST_OBJ            ; НАШЛИ
                cmp     BYTE PTR [si + 4], 10
                jb      K_POST_TEL_ATTR_ZONI_NEPOLN
                je      K_POST_TEL_ATTR_ZONI_10
                cmp     BYTE PTR [si + 4], 11
                ja      BAD_POST_OBJ
		mov	al, [si + 5]
		and	al, 0fh
		cmp	al, 8
                jne     BAD_POST_OBJ
		mov	ax, [si + 5]
		and	ax, PREF_B_MN_MASK
		cmp	ax, PREF_B_MN
		je	BAD_POST_OBJ
		mov	ax, [si + 5]
		and	ax, PREF_B_ZN_MASK
		cmp	ax, PREF_B_ZN
		jne	BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE
K_POST_TEL_ATTR_ZONI_10:
		mov	ax, [si + 5]
		and	ax, PREF_A_ZN_MASK
		cmp	ax, PREF_A_ZN
		jne	BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE
K_POST_TEL_ATTR_ZONI_NEPOLN:
                cmp     BYTE PTR [si + 2], NEPOLN_NOM
                jne     BAD_POST_OBJ
		cmp	BYTE PTR [si + 4], 3
		jb	POST_TEL_DOUBLE
		mov	ax, [si + 5]
		and	ax, PREF_B_MN_MASK
		cmp	ax, PREF_B_MN
		je	BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE

K_POST_TEL_ATTR_MESHGOR:  ; ---------------------------------
                cmp     BYTE PTR [si + 3], PRIZN_MESHGOR
                jne     K_POST_TEL_ATTR_MESHDUN
                or      dh, dh
                jz      BAD_POST_OBJ            ; НАШЛИ
                cmp     BYTE PTR [si + 4], 10
                jb      K_POST_TEL_ATTR_MESHGOR_NEPOLN
                je      K_POST_TEL_ATTR_MESHGOR_10
                cmp     BYTE PTR [si + 4], 11
                ja      BAD_POST_OBJ
		mov	al, [si + 5]
		and	al, 0fh
		cmp	al, 8
                jne     BAD_POST_OBJ
		mov	ax, [si + 5]
		and	ax, PREF_B_MN_MASK
		cmp	ax, PREF_B_MN
		je	BAD_POST_OBJ
		mov	ax, [si + 5]
		and	ax, PREF_B_ZN_MASK
		cmp	ax, PREF_B_ZN
		je	BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE
K_POST_TEL_ATTR_MESHGOR_10:
		mov	ax, [si + 5]
		and	ax, PREF_A_ZN_MASK
		cmp	ax, PREF_A_ZN
		je	BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE
K_POST_TEL_ATTR_MESHGOR_NEPOLN:
                cmp     BYTE PTR [si + 2], NEPOLN_NOM
                jne     BAD_POST_OBJ
		cmp	BYTE PTR [si + 4], 3
		jb	POST_TEL_DOUBLE
		mov	ax, [si + 5]
		and	ax, PREF_B_MN_MASK
		cmp	ax, PREF_B_MN
		je	BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE

K_POST_TEL_ATTR_MESHDUN:  ; ---------------------------------
                cmp     BYTE PTR [si + 3], PRIZN_MESHDUN
                jne     K_POST_TEL_ATTR_SLUSHB
                or      dh, dh
                jz      BAD_POST_OBJ            ; НАШЛИ
                cmp     BYTE PTR [si + 2], NEPOLN_NOM
                je      K_POST_TEL_ATTR_MESHDUN_NEPOLN
		mov	ax, [si + 5]
		and	ax, PREF_B_MN_MASK
		cmp	ax, PREF_B_MN
                je      K_POST_TEL_ATTR_MESHDUN_PREF
K_POST_TEL_ATTR_MESHDUN_NO_PREF:
                jmp     BAD_POST_OBJ	;;;!!!
                cmp     BYTE PTR [si + 4], 10
                jb      BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE
K_POST_TEL_ATTR_MESHDUN_PREF:
                cmp     BYTE PTR [si + 4], 13
                jb      BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE
K_POST_TEL_ATTR_MESHDUN_NEPOLN:
		cmp	BYTE PTR [si + 4], 3
		jb	POST_TEL_DOUBLE
		mov	ax, [si + 5]	;;;!!!
		and	ax, PREF_B_MN_MASK
		cmp	ax, PREF_B_MN
		jne	BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE

K_POST_TEL_ATTR_SLUSHB:   ; ---------------------------------
		;;;<Yu> [05.10.2010] - пока считаем, что спецслужб на нас нет !!!
                cmp     BYTE PTR [si + 2], AB_STANC
                je      BAD_POST_OBJ
		; --- спецслужба начинается с определённой цифры
		mov	al, [si + 5]
		and	al, PREF_B_SS_MASK
		cmp	al, PREF_B_SS
		jne	BAD_POST_OBJ
		; --- полный номер спецслужбы
                cmp     BYTE PTR [si + 2], NEPOLN_NOM
                je      K_POST_TEL_ATTR_SLUSHB_NEPOLN
                cmp     BYTE PTR [si + 4], 2
		jb	BAD_POST_OBJ
                cmp     BYTE PTR [si + 4], 3
		ja	BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE
		; --- неполный номер спецслужбы
K_POST_TEL_ATTR_SLUSHB_NEPOLN:
                cmp     BYTE PTR [si + 4], 1
		jb	BAD_POST_OBJ
                cmp     BYTE PTR [si + 4], 2
		ja	BAD_POST_OBJ
                jmp     POST_TEL_DOUBLE
                ; === КАТС - конец =======================================================

POST_TEL_ATTR_not_CATS:

POST_OBJ_TYPE:  ; === проверяем тип объекта на полный - неполный номер
                cmp     BYTE PTR [si + 2], NEPOLN_NOM
                jne     POST_OBJ_FULL
POST_OBJ_NOT_FULL:      ; --- объект сети с неполным номером
                ; Rostov!
                cmp     BYTE PTR [si + 3], PRIZN_SETI
                jb      BAD_POST_OBJ
                ja      POST_OBJ_ZONI
                ; --- проверим неполный местный -----
POST_OBJ_SETI:  mov     al, [si + 5]             ;\
                and     al, 0fh                  ; \
                cmp     al, 8                    ;  \
                je      BAD_POST_OBJ             ;   = подстрахуемся от 8, 0 и 1
                cmp     al, 0                    ;   = это же местный номер!
                je      BAD_POST_OBJ             ;  /
                cmp     al, 1                    ; /
                je      BAD_POST_OBJ             ;/
                or      dh, dh
                jz      BAD_POST_OBJ             ; НАШЛИ!
                mov     ah, LENGTH_NUM
                cmp     [si + 4], ah
                jae     BAD_POST_OBJ
                jmp     POST_TEL_DOUBLE

                ; --- проверим неполный зоновый -----
POST_OBJ_ZONI:  cmp     BYTE PTR [si + 3], PRIZN_ZONI
                jne     POST_OBJ_MESHGOR
                cmp     BYTE PTR [si + 4], 8
                jae     BAD_POST_OBJ
                jmp     POST_TEL_DOUBLE

                ; --- проверим неполный МГ -----
POST_OBJ_MESHGOR:
                cmp     BYTE PTR [si + 3], PRIZN_MESHGOR
                jne     POST_OBJ_MESHDUN
                cmp     BYTE PTR [si + 4], 11
                jae     BAD_POST_OBJ
                jmp     POST_TEL_DOUBLE

                ; --- проверим неполный МН -----
POST_OBJ_MESHDUN:
                cmp     BYTE PTR [si + 3], PRIZN_MESHDUN
                jne     POST_OBJ_SS
                ; --- {Paul} --- <12/10/2012> --
                cmp     BYTE PTR [si + 4], 18           ; максимальный международный
                jae     BAD_POST_OBJ
                ; --- {Paul} --- <END>
                jmp     POST_TEL_DOUBLE

                ; --- проверим неполный СС -----
POST_OBJ_SS:    cmp     BYTE PTR [si + 3], PRIZN_SLUSHB
                jne     BAD_POST_OBJ
                mov     al, [si + 5]
                and     al, 0fh
                cmp     al, 0
                jne     BAD_POST_OBJ
                cmp     BYTE PTR [si + 4], 3
                jae     BAD_POST_OBJ
                jmp     POST_TEL_DOUBLE

POST_OBJ_FULL:  ; === объект с полным номером ===========================================
                cmp     BYTE PTR [si + 2], AB_STANC
                jne     POST_TEL_ATTR_STAN
                ; --- абонент АТС с полным номером
                cmp     BYTE PTR [si + 3], PRIZN_STAN
                jne     AB_SS
                ;
AB_STAN:        or      dh, dh
                jnz     BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE
                ;
AB_SS:          cmp     BYTE PTR [si + 3], PRIZN_SLUSHB
                jne     BAD_POST_OBJ
                cmp     BYTE PTR [si + 4], 2
		jb	BAD_POST_OBJ
                cmp     BYTE PTR [si + 4], 3
		ja	BAD_POST_OBJ
                mov     al, [si + 5]
		and	al, PREF_B_SS_MASK
		cmp	al, PREF_B_SS
		jne	BAD_POST_OBJ
                mov     ax, [si + 5]
                mov     cx, 1;;;_SS_MAX
                lea     di, SS_Nums
AB_SS_Loop:     mov     dx, [di]
                mov     bl, dl
                shr     dx, 4
                and     bl, 0fh
                jz      AB_SS_Next
                mov     bp, 0f000h
                cmp     bl, 3
                ja      AB_SS_Next
                je      AB_SS_Or
                mov     bp, 0ff00h
AB_SS_Or:       or      dx, bp
                cmp     dx, ax
                jne     AB_SS_Next
                jmp     POST_TEL_DOUBLE
AB_SS_Next:     add     di, 2
                loop    AB_SS_Loop
                jmp     BAD_POST_OBJ
                ; --- абонент сети с полным номером
POST_TEL_ATTR_STAN:
                cmp     BYTE PTR [si + 3], PRIZN_STAN
                jne     POST_TEL_ATTR_SETI
                or      dh, dh
                jnz     BAD_POST_OBJ            ; НЕ НАШЛИ
; ----- <Paul> ---------- {01/11/2010} --------------
                xor     cl, cl                  ;
                mov     cl, BYTE PTR [si + 4]   ;\
                cmp     cl, LENGTH_NUM          ; * если длина > LENGTH_NUM, все равно
                ja      BAD_POST_OBJ            ;/  могли поставить номер!
; ----- <Paul> ---------- {END} ---------------------
		jmp	POST_TEL_DOUBLE
POST_TEL_ATTR_SETI:     ; не начинается с 810, не начинается 8 и имеет длину LENGTH_NUM
                cmp     BYTE PTR [si + 3], PRIZN_SETI
                jne     POST_TEL_ATTR_ZONI
                or      dh, dh
                jz      BAD_POST_OBJ            ; НАШЛИ
		mov	ax, [si + 5]
		and	ax, PREF_B_MG_MASK
		cmp	ax, PREF_B_MG
		je	BAD_POST_OBJ
		mov	ax, [si + 5]
		and	ax, PREF_B_MN_MASK
		cmp	ax, PREF_B_MN
		je	BAD_POST_OBJ
                mov     al, BYTE PTR [si + 5]         ;\
                and     al, 0Fh                       ; \
                cmp     al, 0                         ;  \ подстрахуемся от 0 и 1!
                je      BAD_POST_OBJ                  ;  /
                cmp     al, 1                         ; /
                je      BAD_POST_OBJ                  ;/
                mov     al, LENGTH_NUM
                cmp     al, [si + 4]		; абонент местной сети с неполным номером
                jne     BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE
POST_TEL_ATTR_SETI_full:
                cmp     al, [si + 4]            ; абонент местной сети с полным номером
                jne     BAD_POST_OBJ
		jmp	POST_TEL_DOUBLE
POST_TEL_ATTR_ZONI:     ; не начинается с 810, начинается с 8 и имеет полную длину 9
                cmp     BYTE PTR [si + 3], PRIZN_ZONI
                jne     POST_TEL_ATTR_MESHGOR
                or      dh, dh
                jz      BAD_POST_OBJ            ; НАШЛИ
		mov	ax, [si + 5]
                and     ax, PREF_B_MN_MASK
		cmp	ax, PREF_B_MN
		je	BAD_POST_OBJ
                ; Rostov!
                mov     al, [si + 5]
                and     al, 0fh
                cmp     al, 8
                jne     BAD_POST_OBJ
                cmp     BYTE PTR [si + 4], 9
                jne     BAD_POST_OBJ
                jmp     POST_TEL_DOUBLE
                ; Rostov!
if 0
                mov     ax, [si + 5]
		and	ax, PREF_B_ZN_MASK
		cmp	ax, PREF_B_ZN
		je	POST_TEL_DOUBLE
		mov	ax, [si + 5]
		and	ax, PREF_A_ZN_MASK
		cmp	ax, PREF_A_ZN
		je	POST_TEL_DOUBLE
		jmp	BAD_POST_OBJ
endif
POST_TEL_ATTR_MESHGOR:  ; не начинается с 810, начинается с 8 и имеет полную длину 11 ИЛИ
                        ; не начинается с 810, не начинается с 8 и имеет полную длину 10
                cmp     BYTE PTR [si + 3], PRIZN_MESHGOR
                jne     POST_TEL_ATTR_MESHDUN
                or      dh, dh
                jz      BAD_POST_OBJ            ; НАШЛИ
                mov     ax, [si + 5]
		and	ax, PREF_B_MN_MASK
		cmp	ax, PREF_B_MN
		je	BAD_POST_OBJ
                ; Rostov!
                mov     ax, [si + 5]
                and     ax, PREF_B_MG_MASK
                cmp     ax, PREF_B_MG
                je      POST_MESHGOR_8
POST_MESHGOR_not_8:
                cmp     BYTE PTR [si + 4], 10
                jne     BAD_POST_OBJ
                jmp     POST_TEL_DOUBLE
POST_MESHGOR_8: cmp     BYTE PTR [si + 4], 10
                jb      BAD_POST_OBJ
                cmp     BYTE PTR [si + 4], 11
                ja      BAD_POST_OBJ
                jmp     POST_TEL_DOUBLE
                ; Rostov!
POST_TEL_ATTR_MESHDUN:  ; начинается с 810, длина не важна!!!
                cmp     BYTE PTR [si + 3], PRIZN_MESHDUN
                jne     POST_TEL_ATTR_SLUSHB
                or      dh, dh
                jz      BAD_POST_OBJ            ; НАШЛИ
                ; --- {Paul} --- <20/11/2012>
                mov     ax, [si + 5]
                and     ax, PREF_B_MN_MASK
                cmp     ax, PREF_B_MN
                jne     BAD_POST_OBJ
                ; --- {Paul} --- <END>
                ;;;cmp     BYTE PTR [si + 4], 11
                ;;;jbe     BAD_POST_OBJ
                jmp     POST_TEL_DOUBLE
                ; Rostov!
POST_TEL_ATTR_SLUSHB:
                cmp     BYTE PTR [si + 4], 2
		jb	BAD_POST_OBJ
                cmp     BYTE PTR [si + 4], 3
		ja	BAD_POST_OBJ
		mov	al, [si + 5]
		and	al, PREF_B_SS_MASK
		cmp	al, PREF_B_SS
		jne	BAD_POST_OBJ
                mov     ax, [si + 5]
                mov     cx, 1;;;_SS_MAX
                lea     di, SS_Nums
POST_TEL_ATTR_SLUSHB_Loop:
                mov     dx, [di]
                mov     bl, dl
                shr     dx, 4
                and     bl, 0fh
                jz      POST_TEL_ATTR_SLUSHB_Next
                mov     bp, 0f000h
                cmp     bl, 3
                ja      POST_TEL_ATTR_SLUSHB_Next
                je      POST_TEL_ATTR_SLUSHB_Or
                mov     bp, 0ff00h
POST_TEL_ATTR_SLUSHB_Or:
                or      dx, bp
                cmp     dx, ax
                jne     POST_TEL_ATTR_SLUSHB_Next
                jmp     BAD_POST_OBJ
POST_TEL_ATTR_SLUSHB_Next:
                add     di, 2
                loop    POST_TEL_ATTR_SLUSHB_Loop

                ; --- проверка повторной постановки номера -------------------------------
POST_TEL_DOUBLE:
                cmp     Obj_Count, 0
                je      POST_TEL
                xor     cx, cx
                lea     di, Objects
                mov     Oper_Do_Flag, 5ch       ; объект уже стоит на контроле
POST_TEL_DOUBLE_Loop:
                cmp     WORD PTR gs:[di].obj_type, 0ffffh
		je	POST_TEL_DOUBLE_Next
                push    cx
                push    si
                push    di
                ; выбор количества цифр для сравнения
                movzx   cx, [si + 4]            ; длина № объекта, что хочем наблюдать
                cmp     cl, gs:[di + 4]         ; \  номера
                jbe     POST_TEL_DOUBLE_Cmp     ;  > сравниваем
                movzx   cx, gs:[di + 4]         ; /  по наименьшей длине
POST_TEL_DOUBLE_Cmp:
                ; из чётного / нечётного количества цифр - количество сравниваемых байт
                mov     dh, cl
                shr     cx, 1
                adc     cx, 0
                ; собственно сравнение
                xor     dl, dl
                add     di, 5
                add     si, 5
POST_TEL_DOUBLE_Cmp_Loop:
                mov     al, [si]
                mov     ah, gs:[di]
                cmp     cx, 1
                jne     POST_TEL_DOUBLE_Cmp_Bytes
                test    dh, 1
                jz      POST_TEL_DOUBLE_Cmp_Bytes
                or      ax, 0f0f0h
POST_TEL_DOUBLE_Cmp_Bytes:
                cmp     al, ah
                je      POST_TEL_DOUBLE_Bytes_are_Equal
                mov     dl, 255
POST_TEL_DOUBLE_Bytes_are_Equal:
                inc     si
                inc     di
                loop    POST_TEL_DOUBLE_Cmp_Loop
                pop     di
                pop     si
                pop     cx
                or      dl, dl
                jz      BAD_POST_OBJ
POST_TEL_DOUBLE_Next:
                inc     cx
                add     di, _OBJ_SIZE
                cmp     cx, Obj_Count
                jb      POST_TEL_DOUBLE_Loop
                ; --- ищем куда воткнуть новый объект-телефон
POST_TEL:       mov     Oper_Do_Flag, 5dh       ; на контроле стоит max допустимое число объектов
                cmp     Obj_Count, _OBJ_MAX
                ja      BAD_POST_OBJ
                mov     cx, _OBJ_MAX
                lea     di, Objects
		xor	dx, dx
POST_TEL_Loop:  cmp     WORD PTR gs:[di].obj_type, 0ffffh
                je      POST_OBJ_TEL
		inc	dx
                add     di, _OBJ_SIZE
                loop    POST_TEL_Loop
                jcxz    BAD_POST_OBJ
POST_OBJ_TEL:	cmp	dx, Obj_Count
		jb	POST_Obj
                inc     Obj_Count
                ; --- собственно постановка --------------------------------------------
POST_OBJ:       cld
                mov     cx, 19
		mov	dl, [si + 16]
                mov     es, ss:SEG_OBJ
		push	di
		rep	movsb
		pop	di
                mov     es:[di].obj_flags, 0
		;
                cmp     dl, RAZDEL
		jne	POST_GET_NUM
		mov	dl, 3
POST_GET_NUM:   mov	es:[di].obj_get_num, dl
                ; ---
GUT_POST_OBJ:   MOV     AL,COM_VIP
                MOV     BL,COM_POS
                MOV     BH,IND_GUD
                JMP     SORM_C
                ; ---
BAD_POST_OBJ:   MOV     AL,COM_VIP
                MOV     BL,COM_POS
                MOV     BH, Oper_Do_Flag
                JMP     SORM_C
;
;
SNYTIE_OBJECT:  ; *** 6 - снятие объекта с контроля ************************************
                ; --- проверка наличия объекта с указанным условным номером
                mov     Oper_Do_Flag, 60h       ; объект с указанным номером не стоит на контроле
                mov     bp, [si]                ;
                mov     cx, _BUNCH_MAX          ;
                add     cx, Obj_Count           ;
                lea     bx, Bunches             ;
SNYT_USL_NUM_Loop:
                cmp     WORD PTR gs:[bx].obj_type, 0ffffh  ;
                je      SNYT_USL_NUM_Next       ;
                cmp     bp, gs:[bx].obj_num     ;
                je      SNYT_USL_NUM_Found      ;
SNYT_USL_NUM_Next:
                add     bx, _OBJ_SIZE           ;
                loop    SNYT_USL_NUM_Loop       ;
                jmp     BAD_SNYT_OBJ            ;
                ; --- проверка соответствия параметров при снятии и постановке
SNYT_USL_NUM_Found:
                mov     Oper_Do_Flag, 61h       ; не соответствие параметров при снятии и постановке объекта
                mov     eax, [si + 2]                           ;
                cmp     eax, DWORD PTR gs:[bx].obj_type         ;
                jne     BAD_SNYT_OBJ                            ;
                mov     eax, [si + 6]                           ;
                cmp     eax, DWORD PTR gs:[bx + 4].obj_type     ;
                jne     BAD_SNYT_OBJ                            ;
                mov     eax, [si + 10]                          ;
                cmp     eax, DWORD PTR gs:[bx + 8].obj_type     ;
                jne     BAD_SNYT_OBJ                            ;
                mov     ax, [si + 14]                           ;
                cmp     ax, WORD PTR gs:[bx + 12].obj_type      ;
                jne     BAD_SNYT_OBJ                            ;
                ; --- проверяем, наблюдается ли объект
                mov     fs, ss:SEG_SOED         ;
                mov     es, ss:SEG_SORM         ;
                mov     cx, _SESSION_MAX        ;
		mov     dh, OSV_KOM6            ;
                xor     si, si                  ;
SNYT_OBJ_Find:  test    es:[si].sorm_flags, ssf_REL
                jnz     SNYT_OBJ_Next           ;
		mov	di, es:[si].sorm_obj    ;
                cmp     bp, gs:[di].obj_num     ;
		jne     SNYT_OBJ_Next           ;
                or      es:[si].sorm_flags, ssf_REL
                DB      9AH                     ;
                DW      OFFSET KSL_Free_Far     ;
                DW      120                     ;
                mov     edi, es:[si].sorm_soed  ;
                cmp     fs:[edi], si            ;
                jne     SNYT_OBJ_next           ;
                mov     WORD PTR fs:[edi], 0ffffh
SNYT_OBJ_Next:  add     si, _SORM_SIZE          ;
                loop    SNYT_OBJ_Find           ;
                ; --- реально снимаем объект
SNYT_OBJ:       mov     WORD PTR gs:[bx].obj_type, 0ffffh  ;
                ; --- при необходимости корректировка счётчика объектов
                mov     ax, bx                  ;
                shr     ax, _OBJ_SHL            ;
                sub     ax, _BUNCH_MAX          ;
                js      GUT_SNYT_OBJ            ;
                inc     ax                      ;
                cmp     ax, Obj_Count           ;
                jb      GUT_SNYT_OBJ            ;
                ja      GUT_SNYT_OBJ            ;
                cmp     Obj_Count, 0            ;
                je      GUT_SNYT_OBJ            ;
SNYT_OBJ_Count: dec     Obj_Count               ;
                jz      GUT_SNYT_OBJ            ;
                sub     bx, _OBJ_SIZE           ;
                cmp     WORD PTR gs:[bx].obj_type, 0ffffh  ;
                je      SNYT_OBJ_Count          ;

                ; ---
GUT_SNYT_OBJ:   MOV     AL,COM_VIP
                MOV     BL,COM_SNY
                MOV     BH,IND_GUD
                JMP     SORM_C
                ; ---
BAD_SNYT_OBJ:   MOV     AL,COM_VIP
                MOV     BL,COM_SNY
                MOV     BH, Oper_Do_Flag
                JMP     SORM_C

PODKLUCHENIE:   ; *** 7 - подключение к разговорному тракту
                ; --- проверка номера вызова
                mov     Oper_Do_Flag, 70h       ; неверный номер вызова
                mov     ax, [si]                ;
                mov     es, ss:SEG_SORM         ;
                mov     cx, _SESSION_MAX        ;
                xor     di, di                  ;
PODKL_Find:     test    es:[di].sorm_flags, ssf_REL
                jnz     PODKL_Next              ;
                cmp     es:[di].sorm_call, ax   ;
                je      PODKL_Found             ;
PODKL_Next:     add     di, _SORM_SIZE          ;
                loop    PODKL_Find              ;
                jmp     BAD_PODKL               ;

                ; --- неверная  ссылкка на объект наблюдения
PODKL_Found:    mov     Oper_Do_Flag, 71h       ; неверная  ссылка на объект наблюдения
                mov     bx, es:[di].sorm_obj    ; ссылка на наблюдаемый объект из сессии
                cmp     WORD PTR gs:[bx].obj_type, 0ffffh  ; найденный объект наблюдается ?
                je      BAD_PODKL               ; НЕТ -

                ; --- проверка условного номера объекта
                mov     Oper_Do_Flag, 72h       ; № объекта не соответствует № вызова
                mov     ax, [si + 3]            ;
                cmp     ax, gs:[bx].obj_num     ;
                jne     BAD_PODKL               ;

                ; --- проверка типа объекта
                mov     Oper_Do_Flag, 73h       ; тип объекта не соответствует № вызова
                mov     al, [si + 2]            ;
                cmp     al, gs:[bx].obj_type    ;
                jne     BAD_PODKL               ;

                ; --- проверка возможности подключения
                mov     Oper_Do_Flag, 74h       ; разговорный тракт уже подключен
                cmp     es:[di].sorm_ksl_a, 0ffh;
                jne     BAD_PODKL               ;

                ; --- проверка существования номера группы
		cmp	gs:[bx].obj_cat_ctrl, STATIC ;
		je	PODKL_STATIC            ;
                movzx   si, gs:[bx].obj_gr_ksl  ; для не статистики используем собственную группу объекта
		jmp     PODKL                   ;
PODKL_STATIC:   movzx   si, [si + 5]            ; для статистики используем группу указанную в команде
                mov     Oper_Do_Flag, 75h       ; неверный № группы для подключения к статистике
                cmp     KSL_Group[si], 0ffh     ;
                je      BAD_PODKL               ;

                ; --- собственно подключение
PODKL:          mov     ax, si                  ;
		mov	ah, gs:[bx].obj_type	;
		mov	dx, gs:[bx].obj_num	;
		mov	cx, gs:[bx].obj_bunch   ;
                ; создаем временный объект на время исполнения команды 7
                lea     bx, Obj_for_Connect     ;
                mov     gs:[bx].obj_type,     ah;
                mov     gs:[bx].obj_num,      dx;
		mov	gs:[bx].obj_bunch,    cx;
                mov     gs:[bx].obj_gr_ksl,   al;
                mov     al, KSL_Group_Type[si]  ;
                mov     gs:[bx].obj_cat_ctrl, al;
                cmp     al, RAZDEL              ;
                jne     PODKL_get_num           ;
                mov     al, 3                   ;
PODKL_get_num:  mov     gs:[bx].obj_get_num, al ;
                mov     gs:[bx].obj_prior, PRIOR_HI ;
		; ---
                mov     dh, OSV_KOM7            ;
                mov     si, di                  ;
                DB      9AH                     ;
                DW      OFFSET KSL_Get_Far      ;
                DW      120                     ;

                ; --- проверка результата подключения
                mov     Oper_Do_Flag, 76h       ; в указанной группе все КСЛ заняты высоким приоритетом
                cmp     es:[si].sorm_opcode, NEPODKL_ZAN ;
                je      BAD_PODKL               ;
                and     es:[si].sorm_flags, not ssf_CMND_8

GUT_PODKL:      MOV     AL,COM_VIP
                MOV     BL,COM_PDK
                MOV     BH,IND_GUD
                JMP     SORM_C

BAD_PODKL:      MOV     AL,COM_VIP
                MOV     BL,COM_PDK
                MOV     BH,Oper_Do_Flag
                JMP     SORM_C
;

OSVOBOSHDENIE:  ; *** 8 - освобождение КСЛ **********************************************
                ; --- проверяем задан ли идентификатор вызова
                cmp     DWORD PTR [si], 0ffffffffh ;
                jne     OSVOB_by_ID             ;
                cmp     BYTE PTR [si + 4], 0FFh
                jne     OSVOB_by_ID
                ; --- освобождение по номерам КСЛ
OSVOB_by_KSL:   mov     Oper_Do_Flag, 80h       ; не заданы ни идентификатор вызова, ни номера КСЛ
                movzx   bx, [si + 5]            ;
                cmp     bl, 0ffh                ;
                je      BAD_OSVOB               ;
		;
                mov     Oper_Do_Flag, 81h       ; недопустимые по ТТ номера КСЛ
                mov     ax, [si + 5]            ; номера КСЛ из команды
                movzx   bx, al                  ; номер КСЛ А
                cmp     bl, _MAX_KSL            ; не может быть ff
                je      BAD_OSVOB               ;
                cmp     KSL_Enabled[bx], 1      ; в этой проверке № КСЛ может быть ff
                jne     BAD_OSVOB               ;
                mov     Oper_Do_Flag, 82h       ; КСЛ-А не закреплена
                cmp     KSLs[bx], 0ffh          ;
                je      BAD_OSVOB               ;
                mov     Oper_Do_Flag, 83h       ; номер КСЛ-А указан неверно или №№ КСЛ несоответсвуют друг другу
                cmp     KSL_A[bx], 0            ;
                jne     BAD_OSVOB               ;
                cmp     al, ah                  ; номера КСЛ равны ?
                jne     OSVOB_KSL_Pair          ; НЕТ -
                mov     ah, 255                 ; ДА - сделаем КСЛ-Б равным ff
OSVOB_KSL_Pair: cmp     KSL_Pair[bx], ah        ; КСЛ-Б реальная и КСЛ-Б из команды совпадают ?
                jne     BAD_OSVOB               ; НЕТ -
                ; ---
                mov     Oper_Do_Flag, 84h       ; КСЛ не связана с разговорным трактом
		cmp	KSL_Busy[bx], 0ffh	;
		je	BAD_OSVOB		;
                shl     bx, 1                   ;
                mov     si, KSL_Session[bx]     ;
                cmp     si, 0ffffh              ;
		je	BAD_OSVOB		;
                jmp     OSVOB                   ;

                ; --- освобождение по идентификатору вызова
OSVOB_by_ID:    ; --- проверка номера вызова
                mov     Oper_Do_Flag, 85h       ; неверный номер вызова
                mov     ax, [si]                ;
                mov     es, ss:SEG_SORM         ;
                mov     cx, _SESSION_MAX        ;
                xor     di, di                  ;
OSVOB_ID_Find:  test    es:[di].sorm_flags, ssf_REL
                jnz     OSVOB_ID_Next           ;
                cmp     ax, es:[di].sorm_call   ;
                je      OSVOB_ID_Found          ;
OSVOB_ID_Next:  add     di, _SORM_SIZE          ;
                loop    OSVOB_ID_Find           ;
                jmp     BAD_OSVOB               ;

                ; --- неверная  ссылкка на объект наблюдения
OSVOB_ID_Found: mov     Oper_Do_Flag, 86h       ; неверная  ссылка на объект наблюдения
                mov     bx, es:[di].sorm_obj    ; ссылка на наблюдаемый объект из соединения
                cmp     WORD PTR gs:[bx].obj_type, 0ffffh  ; найденный объект наблюдается ?
                je      BAD_OSVOB               ; НЕТ -

                ; --- проверка условного номера объекта
                mov     Oper_Do_Flag, 87h       ; № объекта не соответствует № вызова
                mov     ax, [si + 3]            ;
                cmp     ax, gs:[bx].obj_num     ;
                jne     BAD_OSVOB               ;

                ; --- проверка типа объекта
                mov     Oper_Do_Flag, 88h       ; тип объекта не соответствует № вызова
                mov     al, [si + 2]            ;
                cmp     al, gs:[bx].obj_type    ;
                jne     BAD_OSVOB               ;

                ; --- проверка возможности подключения
                mov     Oper_Do_Flag, 89h       ; разговорный тракт не подключен
                cmp     es:[di].sorm_ksl_a, 0ffh;
                je      BAD_OSVOB       ;;;;BAD_PODKL!               ;
                mov     si, di			;

                ; --- собственно освобождение
OSVOB:          mov     es, ss:SEG_SORM         ;
                or      es:[si].sorm_flags, ssf_CMND_8
                mov     dh, OSV_KOM8            ;
                DB      9AH                     ;
                DW      OFFSET KSL_Free_Far     ;
                DW      120                     ;

GUT_OSVOB:      MOV     AL,COM_VIP
                MOV     BL,COM_OSV
                MOV     BH,IND_GUD
                JMP     SORM_C

BAD_OSVOB:      MOV     AL,COM_VIP
                MOV     BL,COM_OSV
                MOV     BH,Oper_Do_Flag
                JMP     SORM_C

ISKLUCHENIE:    ; *** 9 - исключение КСЛ из группы **************************************
                ; --- проверка правильности номера группы
                mov     Oper_Do_Flag, 90h       ; ндопустимый номер группы
                cmp     BYTE PTR [si], 0        ;
                je      BAD_ISKL
                cmp     BYTE PTR [si], _MAX_GROUP
                ja      BAD_ISKL
                ; --- проверка существования группы
                mov     Oper_Do_Flag, 91h       ; группа не существует (не создана)
                movzx   bx, [si]                ; № группы из команды
                cmp     KSL_Group[bx], 255
                je      BAD_ISKL
                ; --- проверка правильности №№ КСЛ
                mov     Oper_Do_Flag, 92h       ; недопустимый № КСЛ
                mov     ax, [si + 1]            ; №№ КСЛ из команды
                cmp     al, ah
                jne     ISKL_
                mov     ah, 255
ISKL_:          cmp     al, 0ffh                ; КСЛ-А не может быть ff
                je      BAD_ISKL
                movzx   di, al
                cmp     KSL_Enabled[di], 1      ; в этой проверке № КСЛ может быть ff
                jne     BAD_ISKL
                movzx   di, ah
                cmp     KSL_Enabled[di], 1      ; КСЛ-Б может быть ff
                jne     BAD_ISKL
                ; --- проверка назначенности КСЛ
                mov     Oper_Do_Flag, 93h       ; КСЛ не назначена
                movzx   di, al                  ;
                cmp     KSLs[di], 0ffh          ; КСЛ-А назначена ?
                je      BAD_ISKL                ; НЕТ -
                cmp     ah, 255                 ; КСЛ-Б надо проверять ?
                je      ISKL_KSL_A              ; НЕТ -
                movzx   di, ah                  ;
                cmp     KSLs[di], 0ffh          ; КСЛ-Б назанчена ?
                je      BAD_ISKL                ; НЕТ -
		; --- проверка правильности КСЛ-А и КСЛ-Б
ISKL_KSL_A:     mov	Oper_Do_Flag, 94h	; неверно задана КСЛ-А или КСЛ-Б
                movzx   di, al                  ;
                cmp     KSL_A[di], 0            ; КСЛ-А правильная ?
                jnz      BAD_ISKL               ; НЕТ -
                movzx   di, ah                  ;
                cmp     KSL_A[di], 0            ; КСЛ-Б правильная ?
                jz      BAD_ISKL                ; НЕТ -
                ; --- проверка соответствия типа группы и №№ КСЛ
ISKL_Group_Type:mov     Oper_Do_Flag, 95h               ; комбинация №№ КСЛ не соответствует типу группы
                cmp     KSL_Group_Type[bx], SOVMEST     ; тип группы - совмещённое наблюдение ?
                jne     ISKL_RAZDEL                     ; НЕТ - раздельное
                ; совмещённое наблюдение
ISKL_SOVMEST:   cmp     ah, 0ffh                ; КСЛ-Б не указана ?
                je      ISKL_Soot               ; ДА - ОК !
                cmp     al, ah                  ; КСЛ-Б = КСЛ-А ?
                jne     BAD_ISKL                ; НЕТ - беда !
                mov     ah, 0ffh                ; для совмещённого наблюдения КСЛ-Б будет ff
                jmp     ISKL_Soot               ;
                ; раздельное наблюдение
ISKL_RAZDEL:    cmp     ah, 0ffh                ; КСЛ-Б не указана ?
                je      BAD_ISKL                ; ДА - плохо
                cmp     al, ah                  ; КСЛ-Б = КСЛ-А ?
                je      BAD_ISKL                ; ДА - плохо
                ; --- проверка соответствия № КСЛ номеру группы или номеру второй КСЛ
ISKL_Soot:      mov     Oper_Do_Flag, 96h       ; не соответствие № КСЛ
		movzx	di, al                  ;
                cmp     KSLs[di], bl            ; № КСЛ-А соответствует № группы ?
                jne     BAD_ISKL                ; НЕТ -
		cmp	ah, 255			;
		je	ISKL_Busy		;
                cmp     KSL_Pair[di], ah        ; № КСЛ-А соответствует № КСЛ-Б ?
                jne     BAD_ISKL                ; НЕТ -
		movzx	di, ah                  ;
                cmp     KSLs[di], bl            ; № КСЛ-Б соответсвует № группы ?
                jne     BAD_ISKL                ; НЕТ -
                ; --- проверка занятости КСЛ
ISKL_Busy:      mov     Oper_Do_Flag, 97h       ; КСЛ занята
                movzx   di, al                  ; по номеру КСЛ-А
                cmp     KSL_Busy[di], 0ffh      ; КСЛ занята ?
                jne     BAD_ISKL                ; ДА - исключать нельзя
                ; --- проверка последней КСЛ в группе
                mov     Oper_Do_Flag, 98h       ; последняя КСЛ в группе
                cmp     KSL_Count[bx], 1        ; последняя КСЛ в группе ?
                ja      ISKL_Save               ; НЕТ -
                mov     cx, _BUNCH_MAX          ; по всем наблюдаемым объектам
                add     cx, Obj_Count
                lea     di, Bunches             ;
ISKL_Loop:      cmp     gs:[di].obj_gr_ksl, bl  ; есть объекты использующие эту группу ?
                je      BAD_ISKL                ; ДА - последнюю КСЛ исключать нельзя
                add     di, _OBJ_SIZE           ;
                loop    ISKL_Loop               ;
                ; --- заполнение таблиц СОРМ
ISKL_Save:      ; исключаем КСЛ-А
                movzx   di, al                  ;
                mov     KSLs[di], 0ffh          ;
                mov     KSL_A[di], 0ffh         ;
                mov     KSL_Pair[di], 0ffh      ;
                mov     KSL_Busy[di], 0ffh      ;
                ; исключаем КСЛ-Б
                movzx   di, ah                  ;
                mov     KSLs[di], 0ffh          ;
                mov     KSL_A[di], 0ffh         ;
                mov     KSL_Pair[di], 0ffh      ;
                mov     KSL_Busy[di], 0ffh      ;
                ; уменьшаем счётчики
ISKL_Dec_Counters:
                dec     KSL_Count[bx]           ; общее число КСЛ обнулилось ?
                jnz     GUT_ISKL                ; НЕТ -
                ; удаляем группу                  ДА - удаляем группу
                mov     KSL_Group[bx], 255      ;
                mov     KSL_Group_Type[bx], 255 ;
                mov     KSL_Last[bx], 254       ;

GUT_ISKL:       MOV     AL,COM_VIP
                MOV     BL,COM_ISK
                MOV     BH,IND_GUD
                JMP     SORM_C
BAD_ISKL:
                MOV     AL,COM_VIP
                MOV     BL,COM_ISK
                MOV     BH,Oper_Do_Flag
                JMP     SORM_C


DANNIE_OBJECT:  ; *** 10 - запрос на передачу данных об объектах наблюдения ***
                mov     ax, [si + 2]
                cmp     al, 0ffh
                je      DANN_OBJ_Get
                ; --- проверка - не запрошены ли одновременно телефон и пучок -----------
                mov     Oper_Do_Flag, 0a0h      ; информация запрошена по пучку и № телефона одновременно
                cmp     al, BUNCH
                je      DANN_OBJ_BUNCH
DANN_OBJ_TEL:   cmp     WORD PTR [si + 14], 0ffffh
                jne     BAD_DANN_OBJ
                jmp     DANN_OBJ_Get
DANN_OBJ_BUNCH: cmp     DWORD PTR [si + 3], 0ffffffffh
                jne     BAD_DANN_OBJ
                ; --- проверка запроса по пучку для АТС
DANN_BUNCH:     mov     Oper_Do_Flag, 0a1h      ; информация запрошена по пучку для АТС
IF _MSP EQ TRUE
                jmp     BAD_DANN_OBJ
ENDIF
                ; --- собственно поиск соответствий -------------------------------------
DANN_OBJ_Get:   mov     Obj_Data_Run, 0
                mov     Obj_Data_Send, 0
                mov     Obj_Data_Count, 0
                ; --- подготовка цикла
                mov     cx, _BUNCH_MAX
                add     cx, Obj_Count
                lea     di, Bunches
DANN_OBJ_Loop:  ; стоит ли вообще объект на контроле
                cmp     WORD PTR gs:[di].obj_type, 0ffffh
		je	DANN_OBJ_Next
		; сравнение условных номеров объектов
                mov     ax, [si]
                cmp     ax, 0ffffh
                je      DANN_Obj_Type
                cmp     ax, gs:[di].obj_num
                jne     DANN_OBJ_Next
                ; сравнение типов объектов
DANN_OBJ_Type:  mov     al, [si + 2]
                cmp     al, 0ffh
                je      DANN_OBJ_Attr
                cmp     al, gs:[di].obj_type
                jne     DANN_OBJ_Next
                ; сравнение признаков номера телефона
DANN_OBJ_Attr:  mov     al, [si + 3]
                cmp     al, 0ffh
                je      DANN_OBJ_Num_Len
                cmp     al, gs:[di].obj_attr
                jne     DANN_OBJ_Next
                ; сравнение длины номеров
DANN_OBJ_Num_Len:
                mov     al, [si + 4]
                or      al, al
                jz      DANN_OBJ_Next
                cmp     al, 0ffh
                je      DANN_OBJ_Num;;;DANN_OBJ_Num_B            ; --- {Paul} -- <20/11/2012>
                cmp     al, gs:[di].obj_tel_len
                jne     DANN_OBJ_Next
                ; сравнение номеров телефонов
DANN_OBJ_Num:   mov     eax, [si + 5]
                cmp     eax, 0ffffffffh
                je      DANN_OBJ_Num_B
                cmp     eax, DWORD PTR gs:[di].obj_tel_num
                jne     DANN_OBJ_Next
                mov     eax, [si + 9]
                cmp     eax, 0ffffffffh
                je      DANN_OBJ_Num_B
                cmp     eax, DWORD PTR gs:[di + 4].obj_tel_num
                jne     DANN_OBJ_Next
                mov     al, [si + 13]
                cmp     al, 0ffh
                je      DANN_OBJ_Num_B
                cmp     al, gs:[di + 8].obj_tel_num
                jne     DANN_OBJ_Next
                ; сравнение по условному номеру пучка
DANN_OBJ_Num_B: mov     ax, [si + 14]
                cmp     ax, 0ffffh
                je      DANN_OBJ_Found
                cmp     ax, gs:[di].obj_bunch
                jne     DANN_OBJ_Next
                ; объект обнаружен, запоминаем ссылку на него
DANN_OBJ_Found: mov     bx, Obj_Data_Count
                shl     bx, 1
                mov     Obj_Data_Buf[bx], di
		inc	Obj_Data_Count
                ; объект не найден, дальше по циклу
DANN_OBJ_Next:  add     di, _OBJ_SIZE
		dec	cx
                jnz     DANN_OBJ_Loop
                ; --- проверяем результаты поиска
                mov     Oper_Do_Flag, 0         ; ОК на случай если ничего не нашли и запрос пустой
		cmp     DWORD PTR [si], 0ffffffffh
		jne	DANN_OBJ_No
		cmp     DWORD PTR [si + 4], 0ffffffffh
		jne	DANN_OBJ_No
		cmp     WORD PTR [si + 14], 0ffffh
		je	DANN_OBJ_Is
DANN_OBJ_No:    mov     Oper_Do_Flag, 0a2h      ; не найдено ни одного объекта
DANN_OBJ_Is:    cmp     Obj_Data_Count, 0
                jz      BAD_DANN_OBJ

                ; --- запрос нашёл данные - отмечаем начало передачи --------------------
GUT_DANN_OBJ:   mov     Obj_Data_Run, 255
		mov	Obj_Data_Out_Len, 0
IF _MSP EQ TRUE
		mov	eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
		mov	Obj_Data_Time, eax
		ret

                ; --- данных не искали или искали но не нашли ---------------------------
BAD_DANN_OBJ:
                ; --- @ADR_COMM
                XOR     EDI,EDI
                MOV     DI,POS_SORM
                SHL     EDI,6
                ADD     EDI,2
                MOV     ES,SS:SEG_COM
                mov     ax,1423H
                @_STOSW
                @SOOB_ODNO
                ; --- @ADR_COMM
                ; переносим запрошенные параметры в ответ
                mov     eax, [si]
                @_STOSD
                mov     eax, [si + 4]
                @_STOSD
                mov     eax, [si + 8]
                @_STOSD
                mov     eax, [si + 12]
                @_STOSD
                ; заполняем хвост - категория, группа, приоритет, состояние АК
                mov     eax, 0ffffffffh
                @_STOSD
                ; --- @COM_OKK
                MOV     EAX,EDI
                AND     EDI,0FFFFFFC0H
                SUB     EAX,EDI
                SUB     EAX,2
                JZ      END_DANN_OBJ
                MOV     DX,POS_SORM
                INC     DX
                AND     DX,M_COM
                CMP     DX,PER_SORM
                JE      END_DANN_OBJ
                MOV     POS_SORM,DX
                @_STOSW
END_DANN_OBJ:   ; --- @COM_OKK
        IF _MSP EQ TRUE
                call    SORM_Emul_answ_debug
        ENDIF
                MOV     AL,COM_VIP
                MOV     BL,COM_ZOB
                MOV     BH, Oper_Do_Flag
                JMP     SORM_C

SOOTVETSTVIE:   ; *** 11 - информация о соответсвии между КСЛ и группами ****
                mov     ax, [si]
                mov     cl, ah
                mov     dx, [si + 2]
		cmp	dl, dh
		jne	SOOT_Begin
		mov	dh, 0ffh
                ; --- проверка № группы КСЛ на ТТ
SOOT_Begin:     mov     Oper_Do_Flag, 0b0h      ; недопустимый по ТТ № группы
                cmp     al, 255
                je      SOOT_Type
                or      al, al
                jz      BAD_SOOT
                cmp     al, _MAX_GROUP
                ja      BAD_SOOT
                ; --- проверка № группы на заданность
SOOT_Group:     mov     Oper_Do_Flag, 0b1h      ; незаданный № группы
                movzx   bx, al
                cmp     KSL_Group[bx], 0ffh
                je      BAD_SOOT
                ; --- проверка типа группы КСЛ на ТТ
SOOT_Type:      mov     Oper_Do_Flag, 0b2h      ; недопустимый по ТТ тип группы
                cmp     ah, 255
                je      SOOT_KSL_Enabled
                cmp     ah, SOVMEST
                je      SOOT_Group_and_Type
                cmp     ah, RAZDEL
                je      SOOT_Group_and_Type
                jmp     BAD_SOOT
                ; --- проверка соответствия № группы и её типа
SOOT_Group_and_Type:
                mov     Oper_Do_Flag, 0b3h      ; № и тип группы не соответствуют друг другу
                cmp     al, 0ffh
                je      SOOT_KSL_Enabled
SOOT_is_Group:  movzx   bx, al
                mov     cl, KSL_Group_Type[bx]
                cmp     ah, 0ffh
                je      SOOT_KSL_Enabled
                cmp     cl, ah
                jne     BAD_SOOT
                ; --- проверка номеров КСЛ на допустимость
SOOT_KSL_Enabled:
                mov     Oper_Do_Flag, 0b4h      ; недопустимый по ТТ № КСЛ
                movzx   bx, dl
                cmp     KSL_Enabled[bx], 0
                je      BAD_SOOT
                movzx   bx, dh
                cmp     KSL_Enabled[bx], 0
                je      BAD_SOOT
                ; --- проверка правильности КСЛ-А или КСЛ-Б
                mov     Oper_Do_Flag, 0b5h      ; неверная КСЛ-А или КСЛ-Б
		cmp	dl, 0ffh
		je	SOOT_KSL_
                movzx   bx, dl
                cmp     KSL_A[bx], 0
                jnz     BAD_SOOT
SOOT_KSL_:
;;;                mov     dx, ds:[si + 2]         ; подстрахуемся(если КСЛА=КСЛВ, то уже испортили dh!)
                cmp     dh, 0ffh
		je	SOOT_KSL_AB
                movzx   bx, dh
                cmp     KSL_A[bx], 0
                jz      BAD_SOOT
;;;                cmp     dl, dh                  ;\
;;;                jne     SOOT_KSL_AB             ; - и здесь не мешает подстраховаться
;;;                mov     dh, 0FFh                ;/
                ; --- проверка номеров КСЛ на закреплённость
SOOT_KSL_AB:    mov     Oper_Do_Flag, 0b6h      ; КСЛ не закреплена
SOOT_KSL_A:     cmp     dl, 0ffh
                je      SOOT_KSL_B
                movzx   bx, dl
                cmp     KSLs[bx], 0ffh
                je      BAD_SOOT
SOOT_KSL_B:     cmp     dh, 0ffh
                je      SOOT_KSL_A_or_B
                movzx   bx, dh
                cmp     KSLs[bx], 0ffh
                je      BAD_SOOT
                ; --- проверка номеров КСЛ на парность (соответствие типу группы)
SOOT_KSL_A_or_B:mov     Oper_Do_Flag, 0b7h      ; №№ КСЛ-А и КСЛ-Б не соответствуют типу группы
                cmp     cl, 0ffh
                je      SOOT_KSL_A_and_B
                cmp     dl, 0ffh
                je      SOOT_KSL_A_and_B
                cmp     dh, 0ffh
                je      SOOT_KSL_A_and_B
                cmp     cl, 1
                jne     SOOT_Group_Separate
SOOT_Group_Joint:
                cmp     dl, dh
                jne     BAD_SOOT
                jmp     SOOT_KSL_A_and_B
SOOT_Group_Separate:
                cmp     dl, dh
                je      BAD_SOOT
                ; --- проверка номеров КСЛ на групность (вхождение в одну группу)
SOOT_KSL_A_and_B:
		mov    Oper_Do_Flag, 0b8h      ; №№ КСЛ-А и КСЛ-Б не входят в одну группу
                cmp     dl, 0ffh
                je      SOOT_KSL_A_and_Group
                cmp     dh, 0ffh
                je      SOOT_KSL_A_and_Group
                movzx   bx, dl
                mov     cl, KSLs[bx]
                movzx   bx, dh
                mov     ch, KSLs[bx]
                cmp     cl, ch
                jne     BAD_SOOT
		; --- проверка КСЛ-А на вхождение в указанную группу
SOOT_KSL_A_and_Group:
		cmp	al, 0ffh               ;
		je	SOOT_Ready             ;
		cmp	dl, 0ffh               ;
		je	SOOT_Ready             ;
		mov     Oper_Do_Flag, 0b9h     ; КСЛ-А не входит в указанную группу
		movzx	bx, dl                 ;
		cmp	KSLs[bx], al           ;
		jne	BAD_SOOT
                ; --- конец входных проверок, подготовка к поиску соответсвия
SOOT_Ready:     xor     bx, bx
                mov     KSL_Conform_Count, 0
                ; --- собственно поиск
SOOT_Loop:      cmp     KSL_A[bx], 0ffh
                je      SOOT_Loop_Next
                mov     cl, bl
                mov     ch, KSL_Pair[bx]
                cmp     dl, 0ffh
                je      SOOT_Loop_B
                cmp     cl, dl
                jne     SOOT_Loop_Next
SOOT_Loop_B:    cmp     dh, 0ffh
                je      SOOT_Loop_Group
                cmp     ch, dh
                jne     SOOT_Loop_Next
SOOT_Loop_Group:shl     ecx, 16
                mov     cl, KSLs[bx]
                movzx   si, cl
                mov     ch, KSL_Group_Type[si]
                cmp     al, 0ffh
                je      SOOT_Loop_Type
                cmp     al, cl
                jne     SOOT_Loop_Next
SOOT_Loop_Type:	cmp     ah, 0ffh
                je      SOOT_Loop_Found
                cmp     ch, ah
                jne     SOOT_Loop_Next
SOOT_Loop_Found:mov     di, KSL_Conform_Count
                shl     di, 2
                mov     KSL_Conform_Buff[di], ecx
                inc     KSL_Conform_Count
SOOT_Loop_Next: inc     bx
                cmp     bx, 0ffh
                jb      SOOT_Loop

                ; --- подготовка к выдаче найденнго
SOOT_Send:      mov     KSL_Conform_Send, 0
                mov     ax, KSL_Conform_Count
                mov     bx, 11
		xor	dx, dx
                div     bx
                or      dx, dx
                jz      SOOT_Count
                inc     ax
SOOT_Count:     mov     dx, ax

                xor     bx, bx
SOOT_Send_Loop: ; --- собственно выдача
                ; --- @ADR_COMM
                XOR     EDI,EDI
                MOV     DI,POS_SORM
                SHL     EDI,6
                ADD     EDI,2
                MOV     ES,SS:SEG_COM
                mov     ax,2d24H
                @_STOSW
                mov     ax, dx
                @_STOSW
                inc     bx
                mov     ax, bx
                @_STOSW
                mov     ax, 0ffffh
                @_STOSW
                ; --- @ADR_COMM
                mov     ecx, 45
                mov     si, KSL_Conform_Send
                shl     si, 2
SOOT_Send_Body: mov     ax, KSL_Conform_Count
                cmp     ax, KSL_Conform_Send
                jbe     SOOT_Send_Body_Tail
                mov     eax, KSL_Conform_Buff[si]
                @_STOSD
                inc     KSL_Conform_Send
                add     si, 4
                sub     ecx, 4
                cmp     ecx, 1
                jne     SOOT_Send_Body
SOOT_Send_Body_Tail:
                mov     al, 0ffh
                @_RSTOSB
                ; --- @COM_OKK
                MOV     EAX,EDI
                AND     EDI,0FFFFFFC0H
                SUB     EAX,EDI
                SUB     EAX,2
                JZ      END_SOOT
                MOV     DX,POS_SORM
                INC     DX
                AND     DX,M_COM
                CMP     DX,PER_SORM
                JE      END_SOOT
                MOV     POS_SORM,DX
                @_STOSW
END_SOOT:       ; --- @COM_OKK
        IF _MSP EQ TRUE
                call    SORM_Emul_answ_debug
        ENDIF
                mov     ax, KSL_Conform_Count
                cmp     ax, KSL_Conform_Send
                ja      SOOT_Send_Loop

GUT_SOOT:       ; ---
                MOV     AL,COM_VIP
                MOV     BL,COM_ZKS
                MOV     BH, IND_GUD
                JMP     SORM_C

BAD_SOOT:       ; ---
                MOV     AL,COM_VIP
                MOV     BL,COM_ZKS
                MOV     BH,Oper_Do_Flag
                JMP     SORM_C

SPISOK_DVO:     ; *** 12 - запрос на передачу списка ДВО
                ; --- проверка признака номера телефона
                mov     Oper_Do_Flag, 0c0h      ; неверный признак номера
                CMP     DS:[SI],BYTE PTR 01     ;
                JNE     BAD_SPIS_DVO            ;
                ; --- проверка длины номера
                mov     Oper_Do_Flag, 0c1h      ; длина номера не соответсвует длине номера в АТС
                MOV     AL,LENGTH_NUM           ;
                CMP     DS:[SI+1],AL            ;
                JNE     BAD_SPIS_DVO            ;
		; --- проверяем соответствие длины и количества знаков
                mov     Oper_Do_Flag, 0c2h      ; длина номера не соответствует
		xor	cx, cx                  ;
		mov	bx, si                  ;
		inc     bx			;
DVO_Num:        inc	bx                      ;
		mov	al, [bx]                ;
		mov	ah, al                  ;
		and	al, 0fh                 ;
		cmp	al, 9                   ;
		ja	DVO_Num_End             ;
		inc	cl                      ;
		shr	ah, 4                   ;
		cmp	ah, 9                   ;
		ja	DVO_Num_End             ;
		inc	cl                      ;
		cmp	cl, 18                  ;
		jb	DVO_Num                 ;
DVO_Num_End:	cmp	cl, [si + 1]		;
                jne     BAD_SPIS_DVO            ;
                ; --- проверка номера на свойскость - сначала как УПАТСа, а потом как нормального
                mov     Oper_Do_Flag, 0c3h      ; указанный номер не является абонентом АТС
                ; --- UPATS ---------------------
                mov     al, [si + 2]            ;
                and     al, 0fh                 ;
                mul     BYTE PTR __VAL_10       ;
                mov     dl, [si + 2]            ;
                shr     dl, 4                   ;
                add     al, dl                  ;
                movzx   ebx, al                 ;
                shl     ebx, 16                 ;
                movzx   eax, BYTE PTR [si + 3]  ;
                and     al, 0fh                 ;
                mul     BYTE PTR __VAL_10       ;
                mov     dl, [si + 3]            ;
                shr     dl, 4                   ;
                add     al, dl                  ;
                mul     WORD PTR __VAL_10       ;
                movzx   dx, [si + 4]            ;
                and     dx, 0fh                 ;
                add     ax, dx                  ;
                cmp     BYTE PTR [si + 1], 5    ;
                je      DVO_U_FIND              ;
                mul     WORD PTR __VAL_10       ;
                movzx   dx, [si + 4]            ;
                shr     dx, 4                   ;
                add     ax, dx                  ;
DVO_U_FIND:     add     eax, ebx                ;
                push    esi                     ; портим esi !
                DB      9AH
                DW      OFFSET Find_U_Num_Far   ; ищем в SEG_ULOG
                DW      120
                pop     esi                     ;
                jc      DVO_Found               ; НАШЛИ как абонента УПАТС !
                ; --- UPATS ---------------------
                ; преобразуем № наблюдаемого в формат годный для поиска SEG_NLOG
                mov     eax, [si + 2]           ; первые 8 цифр номера
                rol     al, 4                   ;
                rol     ah, 4                   ;
                xchg    al, ah                  ;
                rol     eax, 16                 ;
                rol     al, 4                   ;
                rol     ah, 4                   ;
                xchg    al, ah                  ;
                movzx   bx, LENGTH_NUM          ;
                mov     cl, Cfg_Num_Shift[bx]   ;
                shr     eax, cl                 ;
                ; --- смотрим наш ли абонент
                push    si                      ;
                DB      9AH                     ;
                DW      OFFSET Find_Tel_Num_Far ; ищем в SEG_NLOG
                DW      120                     ;
                pop     si                      ;
                jnc     BAD_SPIS_DVO            ; не НАШЛИ - команда не выполнена
                ; --- наш - берём его конфигуацию
DVO_Found:      mov     Oper_Do_Flag, 0c4h      ; нет информации о ДВО
                mov     edi, edx                ;
                shr     edi, 13                 ;
                cmp     edi, KL_TIS             ;
                jae     BAD_SPIS_DVO            ;
                shl     edi, 7                  ;
                mov     eax, edx                ;
                shr     edx, 9                  ;
                and     edx, 0fh                ;
                shl     edx, 3                  ;
                add     edi, edx                ;
                mov     gs, ss:SEG_KONF         ;
                mov     edi, gs:[edi + 1]       ;
                cmp     edi, Cfg_Free_Ptr       ;
                jae     BAD_SPIS_DVO            ;
                and     eax, 1ffh               ;
                shl     eax, 4                  ;
                add     edi, eax                ;
                mov     eax, gs:[edi + 1]       ; СПУС
                mov     dl, gs:[edi + 9]        ; переадресация, горячий телефон будильники
                ; --- чистим буфер ДВО абонента
                push    ax                      ;
                mov     al, 255                 ;
                mov     cx, _DVO_BUFF_SIZE      ;
                lea     di, DVO_Buff            ;
                push    ds                      ;
                pop     es                      ;
                rep     stosb                   ;
                pop     ax                      ;
                ;
                mov     DVO_buff, 0             ;
                lea     di, DVO_Buff[3]         ;
DVO_FLASH:      test    ax, M_OR_13             ; бит D - FLASH-услуги
                jz      DVO_in_Cfg              ;
                test    ax, M_OR_3              ; бит 3 - запрет уведомления о входящей
                jnz     DVO_no_CW               ;
                inc     DVO_Buff                ;
                mov     BYTE PTR [di], CW       ; CW
                add     di, 3                   ;
DVO_no_CW:
                inc     DVO_Buff                ;
                mov     BYTE PTR [di], CT       ; CT
                add     di, 3                   ;
                inc     DVO_Buff                ;
                mov     BYTE PTR [di], CC       ; CC
                add     di, 3                   ;
                inc     DVO_Buff                ;
                mov     BYTE PTR [di], HOLE     ; HOLE
                add     di, 3                   ;
                inc     DVO_Buff                ;
                mov     BYTE PTR [di], HOLD     ; HOLD
                add     di, 3                   ;
DVO_3PTY:       test    eax, M_OR_14            ;
                jz      DVO_in_Cfg              ;
                inc     DVO_Buff                ;
                mov     BYTE PTR [di], _3PTY    ; 3PTY
                add     di, 3                   ;
DVO_in_Cfg:     ; ---------
DVO_RA_CFU:     bt      dx, 0                   ;
                jc      DVO_RA_CFB              ;
                inc     DVO_Buff                ;
                mov     BYTE PTR [di], CFU      ; CFU
                add     di, 3                   ;
                jmp     DVO_HT                  ;
DVO_RA_CFB:     bt      dx, 1                   ;
                jc      DVO_RA_CFNRY            ;
                inc     DVO_Buff                ;
                mov     BYTE PTR [di], CFB      ; CFB
                add     di, 3                   ;
                jmp     DVO_HT                  ;
DVO_RA_CFNRY:   bt      dx, 2                   ;
                jc      DVO_HT                  ;
                inc     DVO_Buff                ;
                mov     BYTE PTR [di], CFNRY    ; CFNRY
                add     di, 3                   ;
DVO_HT:         bt      dx, 3                   ;
                jc      DVO_OA                  ;
                inc     DVO_Buff                ;
                mov     BYTE PTR [di], HT       ; Hot Telephon
                add     di, 3                   ;
DVO_OA:         bt      dx, 4                   ;
                jc      DVO_MA                  ;
                inc     DVO_Buff                ;
                mov     BYTE PTR [di], OA       ; Once Alarm
                add     di, 3                   ;
DVO_MA:         bt      dx, 5                   ;
                jc      DVO_End                 ;
                inc     DVO_Buff                ;
                mov     BYTE PTR [di], MA       ; Multy Alarm
DVO_End:
                ; --- определяем размер списка ДВО в байтах
                movzx   cx, DVO_Buff            ; \
                add     cx, cx                  ;  > количество ДВО в списке * 3
                add     cl, DVO_Buff            ; /
                inc     cl                      ; и ещё один байт с общим количеством услуг
                ; --- @ADR_COMM
                XOR     EDI,EDI
                MOV     DI,POS_SORM
                SHL     EDI,6
                ADD     EDI,2
                MOV     ES,SS:SEG_COM
                mov     ah,11
                add     ah, cl
                mov     al,25H
                @_STOSW
                @SOOB_ODNO
                ; --- @ADR_COMM
                ; переносим из запроса тип, длину № и сам № абонента
                mov     eax, [si]
                @_STOSD
                mov     eax, [si + 4]
                @_STOSD
                mov     ax, [si + 8]
                @_STOSW
                mov     al, [si + 10]
                @_STOSB
                ; --- копируем заполненную часть буфера ДВО в сообщение
                lea     si, DVO_Buff

SPIS_DVO_LOOP:  lodsb
                @_STOSB
                loop    SPIS_DVO_LOOP
                ; --- @COM_OKK
                MOV     EAX,EDI
                AND     EDI,0FFFFFFC0H
                SUB     EAX,EDI
                SUB     EAX,2
                JZ      END_SPIS_DVO
                MOV     DX,POS_SORM
                INC     DX
                AND     DX,M_COM
                CMP     DX,PER_SORM
                JE      SHORT END_SPIS_DVO
                MOV     POS_SORM,DX
                @_STOSW
END_SPIS_DVO:   ; --- @COM_OKK
        IF _MSP EQ TRUE
                call    SORM_Emul_answ_debug
        ENDIF
GUD_SPIS_DVO:   MOV     AL,COM_VIP
                MOV     BL,COM_ZDV
                MOV     BH,IND_GUD
                JMP     SORM_C

BAD_SPIS_DVO:   MOV     AL,COM_VIP
                MOV     BL,COM_ZDV
                MOV     BH,Oper_Do_Flag
                JMP     SORM_C

PRERIVANIE:     ; *** 13 - прерывание выдачи таблиц
PRER_BUNCH:     cmp     Bunch_Data_Run, 0; выдача соответствия для пучков
                je      PRER_OBJ
		mov	Bunch_Data_Run, 0
		mov	Bunch_Data_Out_Len, 0
                MOV     AL,COM_VIP
                MOV     BL,COM_ZUS
                MOV     BH,0fah          ; выдача данных прервана по команде 13
                CALL    SORM_C
		jmp	GUT_PRER
		; ---
PRER_OBJ:       cmp     Obj_Data_Run, 0  ; выдача данных об объектах
		je	BAD_PRER
		mov	Obj_Data_Run, 0
		mov	Obj_Data_Out_Len, 0
                MOV     AL,COM_VIP
                MOV     BL,COM_ZOB
                MOV     BH,0a3h          ; выдача данных прервана по команде 13
                CALL    SORM_C

GUT_PRER:       MOV     AL,COM_VIP
                MOV     BL,COM_PRE
                MOV     BH,IND_GUD
                JMP     SORM_C

BAD_PRER:       MOV     AL,COM_VIP
                MOV     BL,COM_PRE
                MOV     BH, 0d0h        ; нет выдачи данных - прерывать нечего
                JMP     SORM_C

TESTIROVANIE:   ; *** 14 - тестирование каналов передачи данных *************************
                ; --- <Yu> - для игнорирования тестовых сообщений до официального запуска СОРМа
                cmp     FG_SOST, 0
                jne     Test_Continue
                ret
Test_Continue:
                ; --- <Yu> - для останова СОРМа при долгом неприходе тестов
IF _MSP EQ TRUE
                mov     eax, Sys_Time       ;;;
ELSE
                mov     eax, Current_Time       ;;;
ENDIF
                mov     SORM_Test_Time, eax ;;;
		mov 	SORM_Stoped, 0      ;;;

                MOV     AX,DS:[SI]
                XOR     AX,PREAMB_NOM_SORM
                JNZ     SHORT NET_TESTA
                OR      FG_TEST,1
                OR      FG_TEST[1],1
                MOV     BL,DS:[SI+10]
                MOV     BUF_TEST1[10],BL
                MOV     AX,KPD1_2
                MOV     WORD PTR BUF_TEST1[11],AX
                MOV     AX,PREAMB_NOM_SORM
                MOV     WORD PTR BUF_TEST2[2],AX
                MOV     BL,DS:[SI+10]
                MOV     BUF_TEST2[14],BL
                MOV     AX,KPD1_2
                MOV     WORD PTR BUF_TEST2[15],AX
;                @SOOB_TES
;                MOV     AL,COM_TES
;                JMP     SORM_C
NET_TESTA:
                RET

                ; *** 15 - изменение параметров объекта контроля *********************
IZMENENIE_KATEGORII:
                ; --- проверка категории контроля
IZMEN_CAT_CTRL: mov     Oper_Do_Flag, 0f0h      ; неопределённая в ТТ категория контроля
                mov     al, [si + 2]
                or      al, al
                jz      BAD_IZMEN
                cmp     al, STATIC
                je      IZMEN_STAT
                cmp     al, SOVMEST
                je      IZMEN_PRIOR
                cmp     al, RAZDEL
                je      IZMEN_PRIOR
                jmp     BAD_IZMEN
                ; --- проверка метки приоритета
IZMEN_PRIOR:    mov     Oper_Do_Flag, 0f1h      ; неопределённая в ТТ метка приоритета
                mov     al, [si + 4]
                or      al, al
                jz      BAD_IZMEN
                cmp     al, PRIOR_LO
                ja      BAD_IZMEN
		jmp	IZMEN_GROUP
                ; --- надо ли проверять номер группы
IZMEN_STAT:     mov     Oper_Do_Flag, 0f2h      ; неверные параметры для статистической категории контроля
                cmp	BYTE PTR [si + 2], STATIC
		jne	IZMEN_GROUP
		cmp	WORD PTR [si + 3], 0ffffh
		jne	BAD_IZMEN
		jmp	IZMEN
                ; --- проверка номера группы
IZMEN_GROUP:	mov     Oper_Do_Flag, 0f3h      ; нераспределённый номер группы
                movzx   bx, [si + 3]
                cmp     KSL_Group[bx], 0ffh
                je      BAD_IZMEN
                ; --- проверка соответсвия номера группы и типа контроля
IZMEN_GROUP_vs_CAT_CTRL:
                mov     Oper_Do_Flag, 0f4h      ; номер группы не соответствует категории контроля
                mov     al, KSL_Group_Type[bx]
                cmp     al, [si + 2]
                jne     BAD_IZMEN
                ; --- проверка условного номера объекта
IZMEN:          mov     Oper_Do_Flag, 0f5h      ; несуществующий условный номер
                mov     ax, [si]
                mov     cx, _BUNCH_MAX
                add     cx, Obj_Count
                lea     di, Bunches
IZMEN_Obj_Loop: cmp     ax, WORD PTR gs:[di].obj_num
                je      IZMEN_Obj_Found
                add     di, _OBJ_SIZE
                loop    IZMEN_Obj_Loop
                jmp     BAD_IZMEN
                ; --- приём изменений
IZMEN_Obj_Found:mov     al, [si + 2]
                mov     gs:[di].obj_cat_ctrl, al
                cmp     al, RAZDEL
		jne	IZMEN_get_num
		mov	al, 3
IZMEN_get_num:  mov     gs:[di].obj_get_num, al
                mov     al, [si + 3]
                mov     gs:[di].obj_gr_ksl, al
                mov     al, [si + 4]
                mov     gs:[di].obj_prior, al

GUT_IZMEN:      MOV     AL,COM_VIP
                MOV     BL,COM_IZM
                MOV     BH,IND_GUD
                JMP     SORM_C

BAD_IZMEN:      MOV     AL,COM_VIP
                MOV     BL,COM_IZM
                MOV     BH,Oper_Do_Flag
                JMP     SORM_C


;
BXOD_PUCHEK:    ; *** 16 - соответствие имени входящего пучка и его условного №
                ; --- определение числа пучков к передаче
                mov     Bunch_Data_Run, 0       ;
                mov     Bunch_Data_Send, 0      ;
                mov     Bunch_Data_Count, 0     ;
		; ---
		mov	ax, [si]		;
		mov	cx, _BUNCH_MAX          ;
		mov	fs, ss:SEG_BUNCH        ;
                mov     ebp, _BUNCH_PRM_OFFS    ;
		mov	Bunch_Data_Count, 0     ;
BXOD_Count:     cmp	BYTE PTR fs:[ebp], 0ffh ;
		je	BXOD_Count_Next         ;
		cmp	ax, 0ffffh		;
		je	BXOD_Count_Found	;
		cmp	ax, fs:[ebp + 1]	;
		jne	BXOD_Count_Next		;
BXOD_Count_Found:
		movzx	ebx, Bunch_Data_Count   ;
		shl	ebx, 2			;
		mov	Bunch_Data_Buf[ebx], ebp;
		inc	Bunch_Data_Count        ;
BXOD_Count_Next:add     ebp, 4                  ;
		loop	BXOD_Count              ;
                ; --- если облом - сообщим
		cmp	Bunch_Data_Count, 0 	;
		jne	BXOD_Send_Prepare	;
                mov     Oper_Do_Flag, 0f8h      ; нет определенных пучков
		cmp	ax, 0ffffh		;
		je	BAD_BXOD                ;
                mov     Oper_Do_Flag, 0f9h      ; пучка с указанным условным номером не существует
		jmp	BAD_BXOD                ;

                ; --- готовимся к передаче названий пучков
BXOD_Send_Prepare:
                mov     Bunch_Data_Run, 255     ;
                mov     Bunch_Data_Out_Len, 0   ;
IF _MSP EQ TRUE
                mov     eax, Sys_Time           ;
ELSE
                mov     eax, Current_Time       ;;;
ENDIF
                mov     Bunch_Data_Time, eax    ;
		ret

BAD_BXOD:       ; ---
                MOV     AL,COM_VIP
                MOV     BL,COM_ZUS
                MOV     BH,Oper_Do_Flag
                JMP     SORM_C

VERSIY_PO_C:    ; *** 17 - запрос версии ПО станции
                MOV     AL,COM_VPO
                JMP     SORM_C


;-----------------------------------------------------------
;процедура выдачи пакета
;-----------------------------------------------------------
L4_LEV4:
                OR      DI,DI
                JZ      KOMANDI_VIV2
;                MOV     AL,BYTE PTR KANAL_1[DI]
;                CMP     AL,BYTE PTR KANAL_1
;                JE      KOMANDI_VIV2
;                CMP     AL,BYTE PTR KANAL_2                  ;COM_SOOB
;                JNE     RET_KANAL                      ;0-КОД СООБЩЕНИЯ
                XOR     ESI,ESI
                MOV     ECX,ESI
                TEST    FG_TEST[1],1
                JNZ     VIDAT_TEST_2
                MOV     SI,SLED_SOOB                   ;1-2-КОД АБОНЕНТА*16
                CMP     SI,POS_SOOB                    ;3-/0-НАБЛ.-ВЫЗЫВАЮЩИЙ
                JE      RET_KANAL                      ;   1-ВЫЗЫВАЕМЫЙ
                INC     SLED_SOOB
                AND     SLED_SOOB,M_SOOB
                SHL     ESI,6
                ADD     ESI,10000H
                CMP     FG_SOST,0
                JNE     SHORT NORM_VIDACHA
                MOV     DS,SS:SEG_COM
                CMP     DS:[ESI+4],BYTE PTR 53H
                JNE     SHORT RET_KANAL
NORM_VIDACHA:
                MOV     DS,SS:SEG_COM
                @_LODSW
                MOV     DS,SS:SEG_AX
                XOR     ECX,ECX
                OR      AX,AX
                JZ      SHORT RET_KANAL
                CMP     AX,100
                JA      SHORT RET_KANAL
                MOV     DS,SS:SEG_COM
                MOV     CX,AX
                PUSH    CX

CN_VIDAT_TEST_2:
                MOV     ES,SS:SEG_AX
                XOR     EDI,EDI
                MOV     DI,BX
                @_RMOVSB
                POP     CX
                SUB     CX,2
                MOV     DS,SS:SEG_AX
                RET
RET_KANAL:
                MOV     DS,SS:SEG_AX
                XOR     ECX,ECX
                RET

VIDAT_TEST_2:
                AND     FG_TEST[1],0
                LEA     ESI,BUF_TEST2
                LODSW
                MOV     CX,AX
                MOV     DS,SS:SEG_AX
                PUSH    CX
                JMP     SHORT CN_VIDAT_TEST_2



;-----------------------------------------------
;---ВЫДАЧА КОМАНД КОМАНДНОГО КАНАЛА
;-----------------------------------------------

KOMANDI_VIV2:
;;;ret;;;
                XOR     EDI,EDI
                MOV     DI,BX
                MOV     ES,SS:SEG_AX
                MOV     AX,PREAMB_NOM_SORM
                @_STOSW
                XOR     ESI,ESI
                TEST    FG_TEST,1
                JNZ     VIDAT_TEST_1
                MOV     SI,PER_SORM
                CMP     SI,POS_SORM
                JE      SHORT RET_KOM_VIV
                INC     PER_SORM
                AND     PER_SORM,M_COM
                SHL     ESI,6
                MOV     DS,SS:SEG_COM
                @_LODSW
                MOV     DS,SS:SEG_AX
                XOR     ECX,ECX
                OR      AX,AX
                JZ      SHORT RET_KOM_VIV
                CMP     AX,100
                JA      SHORT RET_KOM_VIV
                MOV     CX,AX
        	PUSH    CX
                MOV     DS,SS:SEG_COM
CN_VIDAT_TEST_1:
                @_RMOVSB
                MOV     DS,SS:SEG_AX
        	POP     CX
                RET

RET_KOM_VIV:
		; --- проверяем нет ли передачи данных пучков
		cmp	Bunch_Data_Run, 0
		je	is_Obj_VIV
		cmp	Bunch_Data_Out_Len, 0
		je	RET_KOM_VIV_YES
		movzx	ecx, Bunch_Data_Out_Len
		mov	Bunch_Data_Out_Len, 0
		lea	esi, Bunch_Data_Out_Buf
		cmp	Bunch_Data_Out_Buf, 28h
		jne	RET_KOM_VIV_2
		mov	Bunch_Data_Run, 0
		jmp	RET_KOM_VIV_2
		; --- проверяем нет ли передачи данных объектов
is_Obj_VIV:	cmp	Obj_Data_Run, 0
		je	RET_KOM_VIV_YES
		cmp	Obj_Data_Out_Len, 0
		je	RET_KOM_VIV_YES
		movzx	ecx, Obj_Data_Out_Len
		mov	Obj_Data_Out_Len, 0
		lea	esi, Obj_Data_Out_Buf
		cmp	Obj_Data_Out_Buf, 28h
		jne	RET_KOM_VIV_2
		mov	Obj_Data_Run, 0
RET_KOM_VIV_2:
		push	cx
                @_RMOVSB
		pop	cx
		ret
		; ---
RET_KOM_VIV_YES:
                XOR     CX,CX
                RET

VIDAT_TEST_1:
                AND     FG_TEST,0
                LEA     ESI,BUF_TEST1
                LODSW
                ; --- {Paul} --- {18/05.2012} ---
                XOR     ECX, ECX        ; ECX не равнялся 0!
                ; --- {Paul} --- {End}
                MOV     CX,AX
                PUSH    CX
                MOV     DS,SS:SEG_AX
                JMP     CN_VIDAT_TEST_1

@COM_OKK        MACRO
LOCAL           END_M
                MOV     EAX,EDI
                AND     EDI,0FFFFFFC0H
                SUB     EAX,EDI
                SUB     EAX,2
                JZ      END_M
                MOV     DX,POS_SORM
                INC     DX
                AND     DX,M_COM
                CMP     DX,PER_SORM
                JE      SHORT END_M
                MOV     POS_SORM,DX
                @_STOSW
END_M:
                ENDM

@ADR_COMM       MACRO
                XOR     EDI,EDI
                MOV     DI,POS_SORM
                SHL     EDI,6
                ADD     EDI,2
                MOV     ES,SS:SEG_COM
                ENDM

SORM_C:
                ; --- для эмулятора СОРМ - запоминаем результат выполнения
                mov     SORM_Exec_Data,    al
                shr     SORM_Exec_Data,    4
                mov     SORM_Exec_Data[1], al
                and     SORM_Exec_Data[1], 0fh
                mov     SORM_Exec_Data[2], bh
                shr     SORM_Exec_Data[2], 4
                mov     SORM_Exec_Data[3], bh
                and     SORM_Exec_Data[3], 0fh
                ; ---
                PUSH    ES
                PUSHAD
                @ADR_COMM
                XOR     AH,AH
                MOV     SI,AX
                CMP     SI,21H
                JB      PEREHOD
                SUB     SI,21H
                SHL     SI,1
                JMP     ADR_SORM_C[SI]
COM_OK:
                @COM_OKK
        IF _MSP EQ TRUE
                call    SORM_Emul_answ_debug
        ENDIF
NET_MESTA:
                POPAD
                POP     ES
                RET
PEREHOD:
                JMP     NET_MESTA

AVARIY:
                MOV     AX,221H         ;ДЛИННА И КОД
                @_STOSW
                @SOOB_ODNO              ;ПАРАМЕТРЫ
                MOV     AX,BX
                @_STOSW
                JMP     COM_OK

PEREZAPUSK:
                MOV     AX,022H
                @_STOSW
                @SOOB_ODNO
                JMP     COM_OK


NESANKS_DOST:
                MOV     AX,2D26H
                @_STOSW
                @SOOB_ODNO
                MOV     AL,BL
                @_STOSB
		xor	al, al
		@_STOSB
                @MSG_TIME
                MOV     AL,0FFH
                MOV     ECX,39
                @_RSTOSB
                JMP     COM_OK

PODT_PRI:
                MOV     AX,227H
                @_STOSW
                @SOOB_ODNO
                MOV     AX,BX
                @_STOSW
                JMP     COM_OK

PODT_VIP:
                @ADR_COMM
                MOV     AX,228H
                @_STOSW
                @SOOB_ODNO
                MOV     AX,BX
                @_STOSW
                JMP     COM_OK

OTV_TEST:
                MOV     AX,329H
                @_STOSW
                @SOOB_ODNO
                MOV     AL,BL
                @_STOSB
                MOV     AX,KPD1_2
                @_STOSW
                JMP     COM_OK

DANN_SOOT:
                MOV     AX,2AH
                @_STOSW
                @SOOB_ODNO
                JMP     COM_OK

VERSIY_PO:
                MOV     AX,2D2BH
                @_STOSW
                @SOOB_ODNO
                LEA     ESI,BUF_PO
                MOV     ECX,45;;;46;;;
                AND     ESI,0FFFFH
                @_RMOVSB
                @COM_OKK
        IF _MSP EQ TRUE
                call    SORM_Emul_answ_debug
        ENDIF
                MOV     AL,COM_VIP
                MOV     BL,COM_ZPO
                MOV     BH,IND_GUD
                JMP     PODT_VIP


INIC_SORM:
                MOV     DI,              OFFSET KOMANDI
                ; 2 - остановка СОРМа
                MOV     WORD PTR[DI],    OFFSET OSTANOV
                ; 3 - смена пароля
                MOV     WORD PTR[DI+2],  OFFSET PAROL
                ; 4 - закрепление КСЛ за группой
                MOV     WORD PTR[DI+4],  OFFSET ZAKREPLENIE
                ; 5 - постановка объекта на контроль
                MOV     WORD PTR[DI+6],  OFFSET POSTANOVKA_OBJECT
                ; 6 - снятие объекта с контроля
                MOV     WORD PTR[DI+8],  OFFSET SNYTIE_OBJECT
                ; 7 - подключение к разговорному тракту
                MOV     WORD PTR[DI+10], OFFSET PODKLUCHENIE
                ; 8 - принудительное освобождение КСЛ
                MOV     WORD PTR[DI+12], OFFSET OSVOBOSHDENIE
                ; 9 - исключение КСЛ из группы
                MOV     WORD PTR[DI+14], OFFSET ISKLUCHENIE
                ;10 - запрос на передачу данных об объектах наблюдения
                MOV     WORD PTR[DI+16], OFFSET DANNIE_OBJECT
                MOV     WORD PTR[DI+18], OFFSET SOOTVETSTVIE     ;
                MOV     WORD PTR[DI+20], OFFSET SPISOK_DVO       ;
                MOV     WORD PTR[DI+22], OFFSET PRERIVANIE       ;
                MOV     WORD PTR[DI+24], OFFSET TESTIROVANIE     ;
                MOV     WORD PTR[DI+26], OFFSET IZMENENIE_KATEGORII;
                MOV     WORD PTR[DI+28], OFFSET BXOD_PUCHEK      ;
                MOV     WORD PTR[DI+30], OFFSET VERSIY_PO_C      ;

                MOV     DI,              OFFSET ADR_SORM_C
                MOV     WORD PTR[DI],    OFFSET AVARIY
                MOV     WORD PTR[DI+2],  OFFSET PEREZAPUSK
                MOV     WORD PTR[DI+4],  OFFSET NET_MESTA
                MOV     WORD PTR[DI+6],  OFFSET NET_MESTA
                MOV     WORD PTR[DI+8],  OFFSET NET_MESTA
                MOV     WORD PTR[DI+10], OFFSET NESANKS_DOST
                MOV     WORD PTR[DI+12], OFFSET PODT_PRI
                MOV     WORD PTR[DI+14], OFFSET PODT_VIP
                MOV     WORD PTR[DI+16], OFFSET OTV_TEST
                MOV     WORD PTR[DI+18], OFFSET DANN_SOOT
                MOV     WORD PTR[DI+20], OFFSET VERSIY_PO

                ; --- инициализация коммутации
                @COMM_INIT
                ; --- инициализация данных
                @SORM_INIT_DATA
                ; --- сообщение о перезапуске СОРМ для пульта ФСБ
                MOV     AL,COM_PER
                CALL    SORM_C
                RET

; ----------------------------------------------------------------------------
Obj_Data_Output	PROC
		; --- есть ли вообще передача
		cmp	Obj_Data_Run, 0
		je	@@exit
		; --- ушло ли предыдущее сообщение
		cmp	Obj_Data_Out_Len, 0
		jne	@@exit
		; --- истек ли тайм-аут
IF _MSP EQ TRUE
		mov	eax, Sys_Time
ELSE
                mov     eax, Current_Time       ;;;
ENDIF
		sub	eax, Obj_Data_Time
		cmp	eax, _OBJ_DATA_TOUT
		jb	@@exit
IF _MSP EQ TRUE
		mov	eax, Sys_Time
ELSE
                mov     eax, Current_Time       ;;;
ENDIF
		mov	Obj_Data_Time, eax
                ; --- надо ли завершить передачу
                mov     ax, Obj_Data_Count
                cmp     ax, Obj_Data_Send
		jbe	@@stop
                ; --- готовим заголовок для сообщения №3
		cld
		push	ds
		pop	es
                lea     di, Obj_Data_Out_Buf
		mov	ax, 1423h
		stosw
                mov     ax, Obj_Data_Count
		stosw
                mov     ax, Obj_Data_Send
                inc     ax
                stosw
		mov	ax, 0ffffh
		stosw
                ; --- получаем указатель на объект
                mov     si, Obj_Data_Send
		shl	si, 1
                mov     si, Obj_Data_Buf[si]
                inc     Obj_Data_Send
                ; --- определяем состояние АК
                mov     gs, ss:SEG_OBJ
                mov     gs:[si].obj_ak_state, 0FFh;;;3 ; -{Paul}-установим нач. значение-<20/11/2012>
                cmp     gs:[si].obj_type, 0FFh
                je      @@find_upats
                ; --- Tel Find -> ---
                mov     eax, DWORD PTR gs:[si].obj_tel_num
                rol     al, 4
                rol     ah, 4
                xchg    al, ah
                rol     eax, 16
                rol     al, 4
                rol     ah, 4
                xchg    al, ah
                movzx   bx, LENGTH_NUM
                mov     cl, Cfg_Num_Shift[bx]
                shr     eax, cl
	        push    esi                     ; портим esi !
                DB      9AH
                DW      OFFSET Find_Tel_Num_Far ; ищем в SEG_NLOG
                DW      120
                pop     esi                     ;
                jnc     @@send                  ; НЕ НАШЛИ
                jmp     @@find_type
                ; --- Upats Find -> ---
@@find_upats:   ; -- переведем № УПАТС в двоичный
                mov     al, gs:[si].obj_tel_num ;
                and     al, 0fh                 ;
                mul     BYTE PTR __VAL_10       ;
                mov     dl, gs:[si].obj_tel_num ;
                shr     dl, 4                   ;
                add     al, dl                  ;
                movzx   ebx, al                 ;
                shl     ebx, 16                 ;
                movzx   eax, BYTE PTR gs:[si + 1].obj_tel_num
                and     al, 0fh                 ;
                mul     BYTE PTR __VAL_10       ;
                mov     dl, gs:[si + 1].obj_tel_num
                shr     dl, 4                   ;
                add     al, dl                  ;
                mul     WORD PTR __VAL_10       ;
                movzx   dx, gs:[si + 2].obj_tel_num
                and     dx, 0fh                 ;
                add     ax, dx                  ;
                cmp     BYTE PTR gs:[si].obj_tel_len, 5
                je      @@_U_FIND             ;
                mul     WORD PTR __VAL_10       ;
                movzx   dx, gs:[si + 2].obj_tel_num
                shr     dx, 4                   ;
                add     ax, dx                  ;
@@_U_FIND:      add     eax, ebx                ;
                push    esi
                DB      9Ah
                DW      OFFSET Find_U_Num_Far
                DW      120
                pop     esi
                jnc     @@send
                ; нашли - забираем из Cfg тип линии
 @@find_type:                                   ;;;;;mov     gs:[si].obj_ak_state, 22
                mov     fs, ss:SEG_KONF
                mov     ebx, edx
                shr     ebx, 9
                shl     ebx, 3                                ; на конфигурацию блока
                push    bp
                mov     bp, fs:[ebx]                          ;  -- {Paul} - вспомним тип блока
                mov     ebx, fs:[ebx + 1]                     ; адрес линий блока
                cmp     ebx, Cfg_Free_Ptr
                jae     @@send_1
                and     edx, 1ffh
                ; --- {Paul} -- <15/06/12> --
                push    cx
                mov     cl, 4
                test    FG_PROGRAM, _fg_prg_multy_pref
                jz      @@Old_conf
                mov     cl, TAB_TIP_LN[bp]
  @@Old_conf:   shl     edx, cl
                pop     cx
                ; --- {Paul} -- <END>
                add     ebx, edx
                cmp     ebx, Cfg_Free_Ptr
                jae     @@send_1
                mov     gs:[si].obj_ak_state, 1
                cmp     BYTE PTR fs:[ebx], 2    ; абонент ?
                jb      @@send_1
                cmp     BYTE PTR fs:[ebx], 3    ; абонент УПАТС ?
                ja      @@send_1
                mov     gs:[si].obj_ak_state, 0
@@send_1:       pop     bp
                ; формируем тело
@@send:         mov     cx, 14h
                push    ds
                mov     ds, ss:SEG_OBJ
                rep     movsb
                pop     ds
                mov     Obj_Data_Out_Len, 30 - 2
                ; --- просто выход
@@exit:         ret
                ; --- остановка передачи - полная остановка см: RET_KOM_VIV_2
@@stop:         cld
		push	ds
		pop	es
		lea	di, Obj_Data_Out_Buf
		mov	ax, 0228h
		stosw
		@SOOB_ODNO
		mov	al, COM_ZOB
		mov	ah, IND_GUD
		stosw
		mov	Obj_Data_Out_Len, 12 - 2
		ret
Obj_Data_Output	ENDP

; ----------------------------------------------------------------------------
Bunch_Data_Output	PROC
                ; --- есть ли вообще передача
		cmp	Bunch_Data_Run, 0
		je	@@exit
		; --- ушло ли предыдущее сообщение
		cmp	Bunch_Data_Out_Len, 0
		jne	@@exit
		; --- истек ли тайм-аут
IF _MSP EQ TRUE
		mov	eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
		sub	eax, Bunch_Data_Time
		cmp	eax, _BUNCH_DATA_TOUT
		jb	@@exit
IF _MSP EQ TRUE
		mov	eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
		mov	Bunch_Data_Time, eax
                ; --- надо ли завершить передачу
                mov     ax, Bunch_Data_Count
                cmp     ax, Bunch_Data_Send
		jbe	@@stop
                ; --- готовим заголовок для сообщения №10
		cld
		push	ds
		pop	es
                lea     di, Bunch_Data_Out_Buf
		mov	ax, 2d2ah
		stosw
                mov     ax, Bunch_Data_Count
		stosw
                mov     ax, Bunch_Data_Send
                inc     ax
                stosw
		mov	ax, 0ffffh
		stosw
                ; --- получаем указатель на объект
                mov     si, Bunch_Data_Send
		shl	si, 2
		mov	esi, Bunch_Data_Buf[si]
		mov	fs, ss:SEG_BUNCH
		; формируем тело
		mov	eax, fs:[esi]
		movzx	esi, al
		or	al, al
		jz	@@exit
		dec	esi
		shr	eax, 8
		stosw			; номер пучка
		mov	ecx, _BUNCH_NAME_SIZE
		mov	eax, esi
		mul	ecx
		add	eax, _BUNCH_NAME_OFFS
		mov	esi, eax
@@loop:         mov	al, fs:[esi]
		stosb
		inc	esi
		loop	@@loop
		;
                inc     Bunch_Data_Send
		mov	Bunch_Data_Out_Len, 55 - 2
                ; --- просто выход
@@exit:         ret
                ; --- остановка передачи - полная остановка см: RET_KOM_VIV_2
@@stop:         cld
		push	ds
		pop	es
		lea	di, Bunch_Data_Out_Buf
		mov	ax, 0228h
		stosw
		@SOOB_ODNO
		mov	al, COM_ZUS
		mov	ah, IND_GUD
		stosw
		mov	Bunch_Data_Out_Len, 12 - 2
                ret
Bunch_Data_Output	ENDP

; --- отправка сообщения о перезапуске СОРМ
Restart_SORM_Mess  PROC    FAR
                mov     al, COM_PER
                call    SORM_C
                retf
Restart_SORM_Mess  ENDP




