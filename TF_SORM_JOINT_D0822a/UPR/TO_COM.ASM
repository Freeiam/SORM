; МАКРОСЫ
;*************************************************
; Возвращает в reg начальное смещение, в eax - ID
;*************************************************
@GetOfCurConf           MACRO   @Change, @reg
                        LOCAL   @M_Exit, @@secnd_cpy
                        push    dx
                        mov     dl, FLCurActCopyB[eax]          ; текущая конфигурация
                        and     dl, 1                           ; либо 0, либо 1
             if  @Change
                        xor     dl, 1                           ; меняем
             endif
                        mov     @reg, FLFirstCopyAddrD[eax*4]
                        cmp     dl, 1
                        jne     @M_Exit                         ; 0 - выходим
                        mov     @reg, FLSecndCopyAddrD[eax*4]   ; меняем
@M_Exit: ;|
                        pop     dx
                        ENDM

;*************************************************************************
; "Новая" ПУМА - возвращает в reg начальное смещение + add_val, в eax - ID
;*************************************************************************
@GetOfCurConf_@         MACRO   @Change, @reg
                        LOCAL   @M_Exit, @M_Start
                        push    dx
                        mov     dl, FLCurActCopyB[eax]          ; текущая конфигурация
                        and     dl, 1                           ; либо 0, либо 1
                        ;jnz     @@secnd_cpy
             if  @Change
                        xor     dl, 1                           ; меняем
             endif
                        cmp     Flash_ptr, 0
                        jz      @M_Start
                        mov     @reg, Flash_ptr
                        jmp     @M_Exit
                        ; ----------------------------------
@M_Start:               mov     @reg, FLFirstCopyAddrD[eax*4]
                        cmp     dl, 1
                        jne     @M_Exit                         ; 0 - выходим
                        mov     @reg, FLSecndCopyAddrD[eax*4]   ; меняем
@M_Exit: ;|
                        pop     dx
                        ENDM

;--- опознование оборудования --------------------------------------
; размещается в инициализации
INI_COM_CTO     PROC

        PUSH    ES
	MOV	ES,SS:SEG_ZAG
	MOV	AL,ES:[0FFF7h]			;\
	CMP	AL,'Н'				;|
	JNE	BEG_5	 			;|
	MOV	AL,ES:[0FFF8h]			;|
	CMP	AL,'П'				;|
	JNE	BEG_5				;|
	MOV	AL,ES:[0FFF9h]			;|
	CMP	AL,'О'				;|
	JNE	BEG_5  				; \ Определяем подпись
	MOV	AL,ES:[0FFFBh]			; /     загрузчика
        CMP     AL,'А'
	JNE	BEG_5  				;|
	MOV	AL,ES:[0FFFCh]			;|
	CMP	AL,'Т'				;|
	JNE	BEG_5  				;|
	MOV	AL,ES:[0FFFDh]			;|
	CMP	AL,'С'				;|
	JNE	BEG_5  				;/

	MOV	ES,SS:SEG_AX
	MOV	ES:WIN_DOP[2],4
	MOV	ES:WIN_DOP[4],6
	MOV	ES:WIN_DOP[6],12
	MOV	ES:WIN_DOP[8],18
	MOV	ES:WIN_DOP[10],21
	MOV	ES:WIN_DOP[12],0
	OR	FG_FLASH,M_OR_0

BEG_5:
	POP	ES

        PUSH    FS
        ; --- В ОСНОВНОЙ ПРОГРАММЕ ---------------------------------
        MOV     FLASH_OK,0                      ;Сбрасываем флаг ошибки FLASH
        MOV     PLACE,0                         ;Загрузка с FLASH-НЕТ
        ; --- ПОИСК FLASH В СЕГМЕНТЕ D000 --------------------------
        MOV     DX,SS:SEG_PCM                   ; FS - сегмент видеобуфера
        MOV     SS:SEG_FS,DX
        MOV     SCREEN_SEG,DX                   ; Запоминаем сегмент видеобуфера
        MOV     FS,DX

        MOV     ES,SS:SEG_GS8                   ; ES = D000h
        CMP     BYTE PTR ES:[0],55h             ; Есть подпись ?
        JNE     @@ELAN_2MB                        ; НЕТ - нет FLASH
        CMP     BYTE PTR ES:[1],0AAh            ; Есть подпись ?
        JNE     @@ELAN_2MB                        ; НЕТ - нет FLASH
        CMP     BYTE PTR ES:[6],'Н'             ;\
        JNE     @@ELAN_2MB                      ;|
        CMP     BYTE PTR ES:[7],'П'             ;|
        JNE     @@ELAN_2MB                      ;|   Определяем
        CMP     BYTE PTR ES:[8],'О'             ;|   есть ли с D000
        JNE     @@ELAN_2MB                      ; \  подпись
        CMP     BYTE PTR ES:[9],'А'             ; /  нашего загрузчика
        JNE     @@ELAN_2MB                      ;|
        CMP     BYTE PTR ES:[10],'Т'            ;|
        JNE     @@ELAN_2MB                      ;|
        CMP     BYTE PTR ES:[11],'С'            ;|
        JNE     @@ELAN_2MB                      ;/
        MOV     PLACE,1                         ;Загрузка с FLASH
        mov     SS:SEG_FLASH, ES
        ; --- поиск контроллера ELAN (2MB) -------------------------------------
@@ELAN_2MB:
        mov     ax, _ADR_SEG_FLASH_0            ;\
        mov     es, ax                          ;|
        cmp     BYTE PTR es:[0fff8h],'Н'        ;|
        jne     @@ELAN_4MB                      ;|
        cmp     BYTE PTR es:[0fff9h],'П'        ;|
        jne     @@ELAN_4MB                      ;|
        cmp     BYTE PTR es:[0fffah],'О'        ; \ определяем подпись
        jne     @@ELAN_4MB                      ; / загрузчика на FLASH
        mov     BYTE PTR es:[0fffbh],'А'        ;|  ELAN SC-410
        jne     @@ELAN_4MB                      ;|
        cmp     BYTE PTR es:[0fffch],'Т'        ;|
        jne     @@ELAN_4MB                      ;|
        cmp     BYTE PTR es:[0fffdh],'С'        ;|
        jne     @@ELAN_4MB                      ;/
        ; ! здесь флешь 2 МБ
        mov     PLACE, 3
        mov     ax, _ADR_SEG_FLASH_1            ; установка сегмента "seg_flash"
        mov     ss:SEG_FLASH, ax                ; на FLASH ELAN SC-410
        mov     WORD PTR WIN_ETIC[2],  3
        mov     WORD PTR WIN_ETIC[4],  5
        mov     WORD PTR WIN_ETICP[0], 47
        mov     WORD PTR WIN_ETICP[2], 63
        mov     WORD PTR WIN_CONF[2],  2
        mov     WORD PTR WIN_CONF[4],  4
        mov     WORD PTR WIN_PROC[0],  32
        mov     WORD PTR WIN_PROC[2],  48
        mov     FLFirstCopyAddrD[0],    32 * _fls_win_size ;\
        mov     FLSecndCopyAddrD[0],    48 * _fls_win_size ;| программа
        mov     FLSizeInByteD[0],       80000h             ;/
;;; Ниже херня - должно быть 2 * _fls_win_size и 4 * _fls_win_size
        mov     FLFirstCopyAddrD[4],    32 * _fls_win_size ;\
        mov     FLSecndCopyAddrD[4],    48 * _fls_win_size ;| конфигурация
        mov     FLSizeInByteD[4],       10000h             ;/

        mov     Flash_size, 2                   ; размер флеша 2 Мб
        jmp     @@Count_Memory
; --- поиск контроллера ELAN (4MB) --------------------------------------------
@@ELAN_4MB:
;;;jmp @@Count_Memory
        mov     ax, _ADR_SEG_FLASH_0            ;\
        mov     es, ax                          ;|
        cmp     BYTE PTR es:[0fff7h],'Н'        ;|
        jne     @@Count_Memory                  ;|
        cmp     BYTE PTR es:[0fff8h],'П'        ;|
        jne     @@Count_Memory                  ;|
        cmp     BYTE PTR es:[0fff9h],'О'        ; \ определяем подпись
        jne     @@Count_Memory                  ; / загрузчика на FLASH
        mov     BYTE PTR es:[0fffbh],'А'        ;|  ELAN SC-410
        jne     @@Count_Memory                  ;|
        cmp     BYTE PTR es:[0fffch],'Т'        ;|
        jne     @@Count_Memory                  ;|
        cmp     BYTE PTR es:[0fffdh],'С'        ;|
        jne     @@Count_Memory                  ;/
        ; ! здесь флешь 4 МБ
        mov     PLACE, 3
        mov     ax, _ADR_SEG_FLASH_1            ; установка сегмента "seg_flash"
        mov     ss:SEG_FLASH, ax                ; на FLASH ELAN SC-410
        mov     WORD PTR WIN_ETIC[2],  65
        mov     WORD PTR WIN_ETIC[4],  67
        mov     WORD PTR WIN_ETICP[0], 111
        mov     WORD PTR WIN_ETICP[2], 127
        mov     WORD PTR WIN_CONF[2],  64
        mov     WORD PTR WIN_CONF[4],  66
        mov     WORD PTR WIN_PROC[0],  96
        mov     WORD PTR WIN_PROC[2],  112
        mov     Flash_size, 4           ; размер флеша 4 Мб
        ; --- узнаем сколько оперативной памяти на ELAN`e
@@Count_Memory:
        ;call    CountMemory
        push    es
        mov     ax, _SEG_ALL            ;\ настроимся на сегмент перекрывающий
        mov     es, ax                  ;/ всю память
        MOV     EBP, 400000h            ; 4 MB
        XOR     ECX, ECX                ; пока обнаружено 0 мегабайт оперативной памяти
        MOV     EBX, EBP                ; перенесем 4 метра
        DEC     EBX                     ; настроимся на последний байт тестируемой области
;_dymmy_memory_test equ 400000h - 200h
;        mov     ES:[_dymmy_memory_test], 5Ah
@@LOOP_MEM:
        MOV     ES:[EBX], BYTE PTR 0AAh ; запишем
;        mov     al, ES:[_dymmy_memory_test]
        CMP     ES:[EBX], BYTE PTR 0AAh ; проверим
        JNE     @@EXIT_count            ; не равно, - выйдем
        MOV     ES:[EBX], BYTE PTR 055h ; запишем
;        mov     al, ES:[_dymmy_memory_test]
        CMP     ES:[EBX], BYTE PTR 055h ; проверим
        JNE     @@EXIT_count            ; не равно, - выйдем
        ADD     EBX, EBP                ; настроимся на следующий адрес тестирования
        ADD     ECX, 4                  ; прибавим еще 4 мб к найденым
        CMP     ECX, 32                 ; дошли до 32 мб?
        JBE     @@LOOP_MEM              ; нет, тогда проверим следующие 4 метра
@@EXIT_count:
; - т.к. работает не совсем верно на некоторых блока заглушка для больше чем 8 мб
cmp cl, 8
jb @@no_memory_change
mov cl, 8
@@no_memory_change:
        mov     Memory_size, CL         ; запишем для вывода
        pop     es

        ; --- найдем конфигурацию --------------
@@Find_conf:
        PUSH    FS
        CALL    FIND_CONF_ELAN                  ;Ищем рабочую копию конфигур-ии
        CMP     CURR_CONF,0                     ;Есть конфигурация на FLASH ?
        JE      @@test_4MB_pereh                ; НЕТ -
        CALL    READ_CONF                       ;Считываем конфигурацию
        JMP     @@NO_COPY
        ; --- КОНФИГУРАЦИИ НЕТ НА FLASH ( ПЕРВЫЙ ЗАПУСК ПРОГРАММЫ ПОСЛЕ ИНСТАЛЯЦИИ )
@@test_4MB_pereh:
        cmp     flash_size, 4                   ; проверим размер меньше 4?
        jb      @@NO_FIND                       ; да, значит действительно
                                                ; нет конфигурации
                                                ; иначе проверим на старом месте
;;;jmp @@no_find, для отладки
        mov     ax, WORD PTR WIN_ETIC[2]        ;\
        mov     temp_var[0], ax                 ;|
        mov     ax, WORD PTR WIN_ETIC[4]        ;|   сохраним во временное поле
        mov     temp_var[2], ax                 ; \  текущее значение окон
        mov     ax, WORD PTR WIN_CONF[2]        ; /
        mov     temp_var[4], ax                 ;|
        mov     ax, WORD PTR WIN_CONF[4]        ;|
        mov     temp_var[6], ax                 ;/
        mov     WORD PTR WIN_ETIC[2],  3        ;\
        mov     WORD PTR WIN_ETIC[4],  5        ;| присвоим значения старой
        mov     WORD PTR WIN_CONF[2],  2        ;| конфигурации для 2х4
        mov     WORD PTR WIN_CONF[4],  4        ;/
        call    Find_conf_elan                  ; поищем в старом месте
        cmp     Curr_Conf, 0                    ; нашли...?
        je      @@no_find_norm2mb               ; нет, значит проверим ИКМ 1.5
        ; !!! здесь нашли конфигурацию на старом месте
        call    Read_Conf                       ; прочитаем конфигурацию
        jmp     @@no_find_4MB                   ;
@@no_find_norm2mb:
        MOV     ES:WIN_ETIC[2], 41H             ;\
        MOV     ES:WIN_ETIC[4], 51H             ;| Здесь находится конфигурация
        MOV     ES:WIN_CONF[2], 40H             ;| если ИКМ 1.5
        MOV     ES:WIN_CONF[4], 50H             ;/
        call    Find_conf_elan                  ; поищем в старом месте
        cmp     Curr_Conf, 0                    ; нашли...?
        je      @@no_find_4MB                   ; нет, значит действительно нет
        ; !!! здесь нашли конфигурацию на старом месте v1.5
        call    Read_Conf                       ; прочитаем конфигурацию
@@no_find_4MB:
        mov     ax, temp_var[0]                 ;\
        mov     WORD PTR WIN_ETIC[2], ax        ;|
        mov     ax, temp_var[2]                 ;|  востановим из временного
        mov     WORD PTR WIN_ETIC[4], ax        ; \ поля настоящее значение окон
        mov     ax, temp_var[4]                 ; /
        mov     WORD PTR WIN_CONF[2], ax        ;|
        mov     ax, temp_var[6]                 ;|
        mov     WORD PTR WIN_CONF[4], ax        ;/
@@NO_FIND:
        MOV     CURR_CONF,4                     ;Ставим признак 2-й копии
        CALL    SAVE_CONF                       ;Записываем конф-ю (1-ю копию)

@@NO_COPY:
	MOV     AX,CURR_CONF                    ;Запоминаем номер копии
        SHR     AL,1                            ;\
        ADD     AL,30h                          ; > Выводим номер копии
        MOV     ES,SS:SEG_PCM
        MOV     ES:MENU[00],AL                  ;/  конфигурации на экран
@@NO_FLSH:
        MOV     NOM_CONF,1
        POP     FS

        CLI                                     ; закроем прерывания
        ; --- инициализация СОМ порта
        MOV     BX,N_COM                        ; BX - номер СОМ-порта(1 или 2)
        DEC     BX
        AND     BX,1
        SHL     BX,1
        MOV     DX,ADR_PORT[BX]                 ; DX - адрес СОМ-порта
        MOV     PORT_,DX                         ;Запоминаем адрес СОМ-порта
        MOV     AX,VECT_PORT[BX]                ; AX - адрес вектора прерыв-й
        MOV     COM_VECT,AX                     ;Запоминаем адрес вектора
        SHR     BX,1
        ADD     DX,3                            ; kod = inportb( port + 3 );
        IN      AL,DX
        AND     AL,7Fh                          ; sb = kod & 0x7F;
        OUT     DX,AL                           ; outportb( port + 3, sb );
        JMP     SHORT   $+2
        MOV     AL,3

        SUB     DX,2
        OUT     DX,AL                           ; outportb( port + 1, 3 );
        JMP     SHORT   $+2
        MOV     AL,80h
        ADD     DX,2
        OUT     DX,AL                           ; outportb( port + 3, 0x80 );
        JMP     SHORT   $+2
        MOV     AL,SPEED
        MOV     DX,PORT_
        OUT     DX,AL                           ; outportb( port, speed );
        JMP     SHORT   $+2
        MOV     AL,0
        INC     DX
        OUT     DX,AL                           ; outportb( port + 1, 0 );
        JMP     SHORT   $+2
        MOV     AL,3
        ADD     DX,2
        OUT     DX,AL                           ; outportb( port + 3, 3 );
        JMP     SHORT   $+2
        MOV     AL,8
        INC     DX
        OUT     DX,AL                           ; outportb( port + 4, 8 );
        JMP     SHORT   $+2
        MOV     AL,0C7h;07h0C7h
        SUB     DX,2
        MOV     PORT_2,DX
        OUT     DX,AL                           ; outportb( port + 2, 0xC7 );
        JMP     SHORT   $+2
        ADD     DX,3
        MOV     PORT_5,DX
        INC     DX
        MOV     PORT_6,DX

        IN      AL,21h
        MOV     OLD_MASK0,AL                    ;Запомнить 1-ю маску прерываний
        MOV     AL,11100001b
        MOV     AH,MASK_PORT[BX]                ;Замаскируем IRQ4 или IRQ3
        MOV     COM_MASK,AH                     ;Запоминаем маску прерывания
        OR      AL,AH                           ;Замаскируем IRQ4 или IRQ3
;        OUT     021h,AL
        JMP     SHORT   $+2

;; *****************************************    ОПРЕДЕЛЕНИЕ РАЗМЕРА КОНФИГУРАЦИИ
        MOV     AX,OFFSET LCO                   ; AX - адрес первой переменной
                                                ; после области конфигурации
                                                ;ДОНЖЕН БЫТЬ КРАТЕН 512 !!!!!
        DEC     AX
        SHR     AX,9
        MOV     MAX_BL_CONF,AX                  ;Кол-во секторов конфигурации

; ******************************************    После запуска коммутатора
        MOV     AL,11100001b                    ; Разрешение IRQ1, 2
        MOV     AH,COM_MASK
        NOT     AH
        AND     AL,AH                           ; Разрешение IRQ4 или IRQ3
;        OUT     021h,AL
        JMP     SHORT   $+2
        MOV     DX,PORT_
        IN      AL,DX                           ; kod = inportb( port );
        MOV     AL,55h
        OUT     DX,AL                           ; outportb( port, kod );
        MOV     DX,PORT_2
        IN      AL,DX                           ; Чтение рег-ра идентиф-ии прер.
        POP     FS

        call    FLInicialize                    ; Инициализируем работу и читаем флеш

        RET
INI_COM_CTO     ENDP


