; ФУНКЦИИ
;***********************************************************************
; FLInicialize - функция реализует инициализацию данных для работы
; автоопределение тоже надо вставлять сюда
;***********************************************************************
FLInicialize		PROC
			; Здесь надо определять размер флеша и т.д.
                        push    es
                        mov     es, SS:SEG_AX

			; Определим места копий
			mov	FLCurTaskID,_PROGR_ID
			call	FLCalcCurActCopyB
			mov	FLCurTaskID,_CONFG_ID
			call	FLCalcCurActCopyB
                        call    FLReInicialize
			mov	FLCurTaskID,_DOPIN_ID
			call	FLCalcCurActCopyB
                        call    FLReInicialize
                        mov     FLCurTaskID,_NOP_ID

                        call    FLIndicate
                        pop     es
                        ret
;****************************************************************
; Перечитвание конфигурации или доп информации в память
;****************************************************************
FLReInicialize:
                        cmp     FLCurTaskID,_CONFG_ID
                        jne     FLReInicialNOT_CFG
                        ; Надо перечитать конфигурацию
                        push    es
                        mov     es, SS:SEG_AX
                        xor     edi, edi                        ; Устанавливаем все
                        mov     eax, _CONFG_ID                  ;
                        @GetOfCurConf   0, esi
                        mov     ecx, (OFFSET END_KONF_FLASH)    ; слов
                        shr     ecx, 1

                        CALL    FL_READ_FUNC
                        pop     es
                        jmp     FLReInicial_EX
     FLReInicialNOT_CFG:
                        cmp     FLCurTaskID,_DOPIN_ID
                        jne     FLReInicial_EX
                        ; Надо перечитать доп.инфу
                        push    es
                        mov     es, SS:SEG_BLK_LST
                        xor     edi, edi                        ; Устанавливаем все
                        mov     eax, _DOPIN_ID                  ;
                        @GetOfCurConf   0, esi
                        mov     ecx, 40000h                     ; слов
                        CALL    FL_READ_FUNC
                        pop     es
FLReInicial_EX:
                        mov     FLCurTaskID,_NOP_ID             ; обнулим
                        ret
;****************************************************************
; Перезапись конфигурации или доп информации из памяти в флеш
;****************************************************************
FLReWrite:
                        cmp     FLCurTaskID,_CONFG_ID
                        jne     FLReWriteNOT_CFG
                        ; Надо переЗаписать конфигурацию
        SAVE_CONF:
                        push    ds
                        mov     ds, SS:SEG_AX

                        xor     esi, esi                        ; Устанавливаем все
                        mov     FLCurTaskID,_CONFG_ID
                        mov     eax, _CONFG_ID                  ;
                        @GetOfCurConf   1, edi
                        mov     ecx, (OFFSET END_KONF_FLASH)    ; слов
                        shr     ecx, 1

                        CALL    FL_WRITE_FUNC


                        jmp     FLReWrite_EX
     FLReWriteNOT_CFG:
                        cmp     FLCurTaskID,_DOPIN_ID
                        jne     FLReWrite_EX
                        ; Надо переЗаписать доп.инфу
                        push    ds
                        mov     ds, SS:SEG_BLK_LST

                        xor     esi, esi                        ; Устанавливаем все
                        mov     FLCurTaskID,_CONFG_ID
                        mov     eax, _DOPIN_ID                  ;
                        @GetOfCurConf   1, edi
                        mov     ecx, 40000h                     ; слов

                        CALL    FL_WRITE_FUNC
       FLReWrite_EX:
                        call    FLChangeActCopy                 ; Приняли
                        call    FLIndicate
                        mov     FLCurTaskID,_NOP_ID             ; обнулим
                        pop     ds
                        ret
FLInicialize            ENDP
;***********************************************************************
;Вспомогательная ф-я, определяющая активную копию
;FLCurTaskID - ID, который хотим заполнить
;***********************************************************************
FLCalcCurActCopyB	PROC
                        push    eax ebx

			movzx	eax, BYTE PTR FLCurTaskID	; Текущий ID
			; Проверим 1-ю копию
                        mov     esi, FLFirstCopyAddrD[eax*4]    ; Начало
			mov	ebx, FLLablOffsetD[eax*4]	; смещение
			add	esi, ebx			; целиком адрес от начала
			mov	edi, OFFSET FLTmpLabel		; смещение dst
			mov	ecx, 1				; 1 слово
                        CALL    FL_READ_FUNC                    ; считаем
                        mov     bx, FLActvCopyLabelW[eax*2]     ;
			cmp	WORD PTR ES:FLTmpLabel, bx	; Сравним
			jne	FLCalcCurActCopyB_2
                        mov     FLCurActCopyB[eax],0
			jmp	FLCalcCurActCopyB_EX		;
     FLCalcCurActCopyB_2:
			mov	esi, FLSecndCopyAddrD[eax*4]	; Начало
			mov	ebx, FLLablOffsetD[eax*4]	; смещение
			add	esi, ebx			; целиком адрес от начала
			mov	edi, OFFSET FLTmpLabel		; смещение dst
			mov	ecx, 1				; 1 слово
			CALL	FL_READ_FUNC			; считаем
                        mov     bx, FLActvCopyLabelW[eax*2]     ;
			cmp	WORD PTR ES:FLTmpLabel, bx	; Сравним
			jne	FLCalcCurActCopyB_3
                        mov     FLCurActCopyB[eax],1
			jmp	FLCalcCurActCopyB_EX		;
     FLCalcCurActCopyB_3:
     FLCalcCurActCopyB_EX:
                        pop     ebx eax
                        ret
FLCalcCurActCopyB	ENDP
;***********************************************************************
; Индикация номеров копий на экране - пропустим
;***********************************************************************
FLIndicate              PROC
                        push    es
                        MOV     ES,SS:SEG_PCM
                        MOVZX   AX, BYTE PTR DS:FLCurActCopyB[0]
                        ADD     AL,31h
                        MOV     ES:[02],AL
                        MOVZX   AX, BYTE PTR DS:FLCurActCopyB[1]
                        ADD     AL,31h
                        MOV     ES:[04],AL
                        MOVZX   AX, BYTE PTR DS:FLCurActCopyB[2]
                        ADD     AL,31h
                        MOV     ES:[06],AL
                        pop     es
                        ret
FLIndicate              ENDP
;***********************************************************************
; Замена (постановка, удаление этикеток), смена активного номера
; FLCurTaskID - ID, с которым работаем
;***********************************************************************
FLChangeActCopy         PROC
                        pushad
                        push    ds
                        push    es
                        mov     ds, SS:SEG_AX                   ; Сегмент данных
                        mov     es, SS:SEG_AX                   ; Сегмент данных

                        movzx   eax, BYTE PTR FLCurTaskID
                        ; Проверим нужно ли ставить этикетку
                        @GetOfCurConf   1, esi                  ; 1! начало резервной
                        mov     ebx, FLLablOffsetD[eax*4]       ; смещение
                        add     esi, ebx                        ; целиком адрес от начала
                        mov     edi, OFFSET FLTmpLabel          ; смещение dst
                        mov     ecx, 1                          ; 1 слово
                        CALL    FL_READ_FUNC                    ; считаем

                        cmp     FLTmpLabel, 0ffffh              ; Там уже написано
                        jne     FLChangeAct_NWR_LBL
                        ; Поставим этикетку
                        movzx   eax, BYTE PTR FLCurTaskID
                        mov     dx, FLActvCopyLabelW[eax*2]
                        mov     FLTmpLabel, dx                  ; Этикетка

                        @GetOfCurConf   1, edi                  ; 1!начало резервной
			mov	ebx, FLLablOffsetD[eax*4]	; смещение
                        add     edi, ebx                        ; целиком адрес от начала
                        mov     esi, OFFSET FLTmpLabel          ; смещение dst
			mov	ecx, 1				; 1 слово
                        CALL    FL_WRITE_FUNC                   ; записываем
                        ; сотрем старую этикетку
     FLChangeAct_NWR_LBL:
                        movzx   eax, BYTE PTR FLCurTaskID
                        mov     FLTmpLabel, 0000                ; Этикетку затереть 0

                        @GetOfCurConf   0, edi                  ; Начало текущей
			mov	ebx, FLLablOffsetD[eax*4]	; смещение
                        add     edi, ebx                        ; целиком адрес от начала

                        mov     esi, OFFSET FLTmpLabel          ; смещение dst
			mov	ecx, 1				; 1 слово
                        CALL    FL_WRITE_FUNC                   ; записываем
                        ; сохраним информацию о новой копии
                        mov     dl, FLCurActCopyB[eax]          ; текущая конфигурация
                        and     dl, 1                           ; либо 0, либо 1
                        xor     dl, 1                           ; меняем
                        mov     FLCurActCopyB[eax], dl          ; текущая конфигурация
                        ; отобразим все
                        call    FLIndicate
                        pop     es
                        pop     ds
                        popad
                        ret
FLChangeActCopy         ENDP

; <<<<<<<<<<<<<<<<<<<<<<<<< Драйвер Elan410 flash 29DL32(LV16) >>>>>>>>>>>>>>>>>>>>>>>>>>>>
;***********************************************************************
; Чтение флеш Elan'a; esi - адрес от начала флеш
; ecx - количество слов к чтению; es:edi - адрес dst
;***********************************************************************
ELAN400_READ_FLC        PROC
                        push    ds
                        push    eax
                        mov     ds, _FLASH1_SEG     	; Сегмент флеш
                        ; выберем окно
                        mov     eax, esi             	;
                        shr     eax, 7               	; ah - номер окна
                        mov     al, 32h              	; al - номер INDEX'a порта елана
			out     22h, ax
    ELAN400_READ_FLC_MOV:
                        push    esi
                        and     esi, 07fffh             ;
                        DB      67h
                        movs    word ptr es:[edi], word ptr ds:[esi]
                        pop     esi
                        add     esi, 2

                        test    esi, 07fffh             ;
                        jnz     ELAN400_READ_FLC_NZ
                        ; изменим окно
                        mov     eax, esi             	;
                        shr     eax, 7               	; ah - номер окна
                        mov     al, 32h              	; al - номер INDEX'a порта елана
			out     22h, ax
     ELAN400_READ_FLC_NZ:
                        DB      67h
                        loop    ELAN400_READ_FLC_MOV

                        pop     eax
			pop	ds
                        ret
ELAN400_READ_FLC        ENDP
;***********************************************************************
; Запись на флеш Elan'a; edi- адрес от начала флеш
; ecx - количество слов к записи; ds:esi - адрес src
;***********************************************************************
ELAN400_WRITE_FLC      	PROC
                        push    es
                        push    eax
                        mov     es, _FLASH1_SEG     ; Сегмент флеш
ELAN400_WRITE_FLC_LOOP:
                        ; проверим, надо ли стирать
                        test    edi, 0ffffh          ; адрес кратен 64кб
                        jnz     _Nt_Erse             ; не надо
                        call    ELAN400_ERASE_FLC    ; стирать
            _Nt_Erse:
		        DB    	67h
                        lodsw
                        cmp     ax, 0ffffh
                        je      _Nt_Save
                        push    ax
                        ; выберем окно
                        mov     eax, edi             ;
                        shr     eax, 7               ; ah - номер окна
                        mov     al, 32h              ; al - номер INDEX'a порта елана
                                                     ; для перестановки окон
                        push    ax
                        and     ah, 0C0h             ; окно для управления
                        out     22h, ax
                        ; команда "program word"
                        MOV BYTE PTR ES:[0AAAh],0AAh
                        MOV BYTE PTR ES:[554h],55h
                        MOV BYTE PTR ES:[0AAAh],0A0h
                        pop     ax
                        out     22h, ax              ; еще раз выберем окно
                        ; можно писать и проверять
                        pop     ax
                        push    edi
                        ;mov     ax, di
                        and     edi, 07fffh
                        MOV     ES:[di], AX
                        ; Проверка записи
            _Test_loop:
                        mov     dx, ES:[di]
                        cmp     al, dl
                        je      _Saved_OK                   ; Записалось
                        test    dl, 20h
                        jz      _Test_loop                  ; Не проходил стадию
                        mov     dx, ES:[di]
                        cmp     al, dl
                        je      _Saved_OK                   ; Записалось
                        pop     eax
                        pop     edi
                        pop     es                          ; Error exit
                        stc
                        ret
            _Saved_OK:
                        pop     edi
            _Nt_Save:   ; не надо записывать
                        add     edi, 2                      ; сдвинем
		        DB      66h
                        loop    ELAN400_WRITE_FLC_LOOP      ; Следующий байт
                        ; Нормальный выход
ELAN400_WRITE_EX:
                        pop     eax
                        pop     es
                        clc
                        ret
;***********************************************************************
;подфункция стирания сектора (64кб) (может работать как фунцкия)
;edi- адрес от начала флеш
;***********************************************************************
ELAN400_ERASE_FLC:
                        push    eax
                        push    es
                        mov     es, _FLASH1_SEG      ; Сегмент флеш


                        mov     eax, edi             ;
                        shr     eax, 7               ; ah - номер окна
                        mov     al, 32h              ; al - номер INDEX'a порта елана
                                                     ; для перестановки окон
                        push    ax
                        and     ah, 0C0h             ; окно для управления
                        out     22h, ax
                        ; команда "erase sector"
                        MOV     BYTE PTR ES:[0AAAh],0AAh
                        MOV     BYTE PTR ES:[554h],55h
                        MOV     BYTE PTR ES:[0AAAh],80h
                        MOV     BYTE PTR ES:[0AAAh],0AAh
                        MOV     BYTE PTR ES:[554h],55h
                        pop     ax
                        out     22h, ax              ; еще раз выберем окно
                        MOV     BYTE PTR ES:[0],30h
                        ; Проверяем конец записи
                CLR_SE0:
                        MOV     AL,ES:[0]
                        TEST    AL,80h
                        JNZ     CLR_SE1
                        TEST    AL,20h
                        JZ      CLR_SE0
                        MOV     AL,ES:[0]
                        TEST    AL,80h
                        JNZ     CLR_SE1
                CLR_SE1:
                        MOV     BYTE PTR ES:[0],0F0h
                        pop     es
                        pop     eax
                        ret
ELAN400_WRITE_FLC       ENDP
; ******************************
; ------------------------------
ELAN400_WRITE_FLC_X       PROC
                        push    ds
                        push    es
                        pop     ds
                        push    es
                        push    eax
                        mov     es, _FLASH1_SEG     ; Сегмент флеш

ELAN400_WRITE_FLC_LOOP_X:
                        ; проверим, надо ли стирать
                        test    edi, 0ffffh          ; адрес кратен 64кб
                        jnz     _Nt_Erse_X             ; не надо
                        call    ELAN400_ERASE_FLC_X    ; стирать
            _Nt_Erse_X:
		        DB    	67h
                        lodsw
                        cmp     ax, 0ffffh
                        je      _Nt_Save_X
                        push    ax
                        ; выберем окно
                        mov     eax, edi             ;
                        shr     eax, 7               ; ah - номер окна
                        mov     al, 32h              ; al - номер INDEX'a порта елана
                                                     ; для перестановки окон
                        push    ax
                        and     ah, 0C0h             ; окно для управления
                        out     22h, ax
                        ; команда "program word"
                        MOV BYTE PTR ES:[0AAAh],0AAh
                        MOV BYTE PTR ES:[554h],55h
                        MOV BYTE PTR ES:[0AAAh],0A0h
                        pop     ax
                        out     22h, ax              ; еще раз выберем окно
                        ; можно писать и проверять
                        pop     ax
                        push    edi
                        ;mov     ax, di
                        and     edi, 07fffh
                        MOV     ES:[di], AX
                        ; Проверка записи
            _Test_loop_X:
                        mov     dx, ES:[di]
                        cmp     al, dl
                        je      _Saved_OK_X                   ; Записалось
                        test    dl, 20h
                        jz      _Test_loop_X                  ; Не проходил стадию
                        mov     dx, ES:[di]
                        cmp     al, dl
                        je      _Saved_OK_X                   ; Записалось
                        ;pop     eax
                        pop     edi
                        pop     eax
                        pop     es                          ; Error exit
                        pop     ds
                        stc
                        ret
            _Saved_OK_X:
                        pop     edi
            _Nt_Save_X:   ; не надо записывать
                        add     edi, 2                      ; сдвинем

		        DB      66h
                        loop    ELAN400_WRITE_FLC_LOOP_X      ; Следующий байт
                        ; Нормальный выход
ELAN400_WRITE_EX_X:
                        pop     eax
                        pop     es
                        pop     ds
                        clc
                        ret
;***********************************************************************
;подфункция стирания сектора (64кб) (может работать как фунцкия)
;edi- адрес от начала флеш
;***********************************************************************
ELAN400_ERASE_FLC_X:
                        push    eax
                        push    es
                        mov     es, _FLASH1_SEG      ; Сегмент флеш


                        mov     eax, edi             ;
                        shr     eax, 7               ; ah - номер окна
                        mov     al, 32h              ; al - номер INDEX'a порта елана
                                                     ; для перестановки окон
                        push    ax
                        and     ah, 0C0h             ; окно для управления
                        out     22h, ax
                        ; команда "erase sector"
                        MOV     BYTE PTR ES:[0AAAh],0AAh
                        MOV     BYTE PTR ES:[554h],55h
                        MOV     BYTE PTR ES:[0AAAh],80h
                        MOV     BYTE PTR ES:[0AAAh],0AAh
                        MOV     BYTE PTR ES:[554h],55h
                        pop     ax
                        out     22h, ax              ; еще раз выберем окно
                        MOV     BYTE PTR ES:[0],30h
                        ; Проверяем конец записи
                CLR_SE0_X:
                        MOV     AL,ES:[0]
                        TEST    AL,80h
                        JNZ     CLR_SE1
                        TEST    AL,20h
                        JZ      CLR_SE0
                        MOV     AL,ES:[0]
                        TEST    AL,80h
                        JNZ     CLR_SE1
                CLR_SE1_X:
                        MOV     BYTE PTR ES:[0],0F0h
                        pop     es
                        pop     eax
                        ret
ELAN400_WRITE_FLC_X       ENDP
; **********************************
READ_DOP_INFO   PROC    NEAR
        PUSHAD
        PUSH    DS
        PUSH    ES
	MOVZX	BX,VERSIY
	SHL	BX,1
	MOV	CX,LENGTH_DOPM[BX]
	MOV	BX,WIN_DOP[BX]
	TEST	FG_PROG,M_OR_1
	JZ	SHORT NET_GLB_RBF
	MOVZX	BX,CUR_ALTERA_CHIP
	SHL	BX,1
	MOV	CX,LENGTH_DOP_SNM

	MOV	BX,WIN_DOP_SN[BX]
NET_GLB_RBF:

	MOV	SS:SEG_FLASH,240

        MOV     DS,SS:SEG_FLASH
        MOV     ES,SS:SEG_TF

        CLD
        XOR     EDI,EDI
_REPEAT:
        MOV     AL,32h
        MOV     AH,BL                           ;В AH - окно
	OUT	22h,AX 				;Порт 22h/23h индекс 32h
        PUSH    CX
        MOV     CX,16384
        XOR     SI,SI

SMALL_REPEAT:
        LODSW
        @_STOSW
        LOOP    SMALL_REPEAT
        POP     CX
        INC     BX

        LOOP    _REPEAT

        POP     ES
        POP     DS
        POPAD
        RETN
READ_DOP_INFO   ENDP

; *********************************     ПОИСК КОНФИГУРАЦИИ НА FLASH
FIND_CONF_ELAN  PROC   NEAR
        PUSHAD
        MOV     AX,WIN_ETIC[2]                  ;В AH - окно этикетки 1-й копии
        SHL     AX,8
        MOV     AL,INDEX_FLASH
	OUT	22h,AX                          ;Выбор окна ЭППЗУ
        PUSH    DS
        MOV     DS,SS:SEG_FLASH			;\
        MOV     SI,ADR_ID                       ; |Чтение префикса
        LODSW                                   ;/
        POP     DS
        CMP     AX,0BDEFh
        JNE     SHORT   NO_F_CE
        MOV     CURR_CONF,2
        POPAD
        RETN
NO_F_CE:
        MOV     AX,WIN_ETIC[4]                  ;В AH - окно этикетки 2-й копии
        SHL     AX,8
        MOV     AL,INDEX_FLASH
	OUT	22h,AX                          ;Выбор окна ЭППЗУ
        PUSH    DS
        MOV     DS,SS:SEG_FLASH			;\
        MOV     SI,ADR_ID                       ; |Чтение префикса
        LODSW                                   ;/
        POP     DS
        CMP     AX,0BDEFh
        JNE     SHORT   NO_S_CE
        MOV     CURR_CONF,4
NO_S_CE:POPAD
        RETN
FIND_CONF_ELAN  ENDP

; **********************************    ЧТЕНИЕ КОНФИГУРАЦИИ С FLASH
; в EDX - длина сегмента для чтения
READ_CONF       PROC    NEAR
        PUSHAD
	MOV	DX,LAST_ADR
        MOV     BX,CURR_CONF
        MOV     BX,WIN_CONF[BX]
        MOV     DI,0
        PUSH    DS
        PUSH    ES
RD_CF4: MOV     CX,16384   ;\
        LEA     CX,T_F_D   ; >    ??? 55h - 85d
        SHR     CX,1       ;/  /2 ??? итого 42d
        MOV     DS,SS:SEG_FLASH
        MOV     ES,SS:SEG_AX
        MOV     AL,INDEX_FLASH
	MOV	AH,BL				;В AH - окно
	OUT	22h,AX 				;Порт 22h/23h индекс 32h
        MOV     SI,0
RD_CF5: LODSW        ;\ 5t можно использовать MOVSW 7t, должно быть быстрее или MOVSD
        STOSW        ;/ 4t                      выигрыш 2t                       many t
        CMP     DI,DX
        JAE     SHORT   RD_CF6
        LOOP    RD_CF5
;        INC     BX
;        JMP     SHORT   RD_CF4
RD_CF6: POP     ES
        POP     DS
        POPAD
        RETN
READ_CONF       ENDP




