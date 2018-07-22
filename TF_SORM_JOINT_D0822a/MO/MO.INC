; --- перевод двоичногов в al в BCD
;     ВНИМАНИЕ - обнуляет ah !!!
Hex_To_BCD      PROC
                xor     ah, ah
                aam
                shl     al, 4
                shr     ax, 4
                ret
Hex_To_BCD      ENDP
IF _MSP NE TRUE
PrintLogErr     PROC
                pusha
                mov     al, _color_err_log
                mov     color_log, al
                jmp     PrintLog_2
                ENDP

PrintLog        PROC
                pusha
                mov     al, _color_log
                mov     color_log, al
PrintLog_2:
                push    si
                mov     si, offset LOG_CLEAR
                mov     di, _pos_log
                mov     ah, _color_cl_log
                call    print_S
                pop     si

                mov     di, _pos_log
                mov     ah, color_log
                call    print_S

                mov     LOG_CHANGE, 1
                mov     ax, TIME_2MC
                mov     LOG_TIMEOUT, ax
                popa
                ret
PrintLog        ENDP
ENDIF
; --- -=* Вывод шестнадцатиричного числа на экран в заданной позиции *=- ------
; ah - атрибут
; cl - сколько выводить
; di - куда выводить (смещение экрана)
Print_A          proc
        or      cl, cl          ; проверим на ноль
        jz      @@exitA         ; если ноль выйдем

        push    cx di ax es
        mov     es, ss:SEG_FS

        mov     al, ah
        inc     di
        cld
@@loop: cmp     di, _ScreenSize - 2
        ja      @@exit
        stosb
        inc     di
        loop    @@loop
@@exit:
        pop     es ax di cx
@@exitA:
        ret
Print_A          endp

; --- -=* Вывод шестнадцатиричного числа на экран в заданной позиции *=- ------
; ebx - число
; ah - атрибут
; cl - сколько выводить тетрад (1 - 8)
; di - куда выводить (смещение экрана)
Print_H          proc
        or      cl, cl          ; проверим на ноль
        jz      @@exitH         ; если ноль выйдем

        push    cx di edx ax es
        mov     es, ss:SEG_FS

        dec     cl
        and     cl, 7
@@loop:
        cmp     di, _ScreenSize - 2
        ja      @@exit
        push    cx

        shl     cl, 2
        mov     edx, ebx
        shr     edx, cl
        mov     al, dl
        and     al, 0Fh

        cmp     al, 9
        jbe     @@no_hex
        add     al, 'A' - '9' - 1
@@no_hex:
        add     al, '0'
        stosw

        pop     cx
        dec     cl
        jns     @@loop
@@exit:
        pop     es ax edx di cx
@@exitH:
        ret
Print_H          endp

Print_Buff      proc

cld
        or      cx, cx
        jz      @@quit_print

        push    ax es di
        mov     es, ss:SEG_FS
@@next_byte:
        push    cx
        mov     cx, 2
        mov     dl, byte ptr [si]

@@loop_digit:
        cmp     di, _ScreenSize - 2
        ja      @@exit
        mov     al, dl
        and     al, 0Fh
        cmp     al, 9
        jbe     @@no_hex
        add     al, 'A' - '9' - 1
@@no_hex:
        add     al, '0'
        stosw
        shr     dl, 4
        loop    @@loop_digit
        inc     si
        pop     cx
        loop    @@next_byte
        jmp     short @@end_print
@@exit: pop     cx
@@end_print:
        pop     di es ax
@@quit_print:
                ret
Print_Buff      endp


; --- -=* вывод строки на экран (строка должна оканчиваться нулем) *=- --------
; SI - откуда
; DI - куда (смещение экрана)
; AH - атрибут
print_S          PROC
        push    si di ax es
        mov     es, ss:SEG_FS
@@loop:
        cmp     di, _ScreenSize - 2
        ja      @@exit
        lodsb
        cmp     al, 0
        je      @@exit
        stosw
        jmp     @@loop
@@exit:
        pop     es ax di si
        ret
print_S          ENDP

; --- -=* Вывод десятиччного числа на экран в заданной позиции *=- ------
; ebx - число
; ah - атрибут
; cl - сколько выводить тетрад (1 - 8)
; di - куда выводить (смещение экрана)
print_D         proc
        or      cl, cl          ; проверим на ноль
        jz      @@exitD         ; если ноль выйдем

        push    si di edx cx ebx eax es
        mov     es, ss:SEG_FS

        xor     ch, ch
        add     di, cx
        add     di, cx
        sub     di, 2
        xor     si, si
        cmp     di, _ScreenSize - 2
        ja      @@exit
        mov     ch, ah
        mov     eax, ebx
        mov     ebx, 10
@@loopD:
        xor     edx, edx
        div     ebx
        push    ax
        or      edx, edx
        jnz     @@drawDigit
        or      eax, eax
        jnz     @@drawDigit
        or      si, si
        jz      @@drawDigit
        mov     al, ' ';;;Empty_Char
        jmp     @@drawD
@@drawDigit:
        mov     al, dl
        add     al, '0'
@@drawD:
        mov     ah, ch
        mov     es:[di], ax
        pop     ax
        sub     di, 2
        inc     si
        dec     cl
        jnz     @@loopD
@@exit:
        pop     es eax ebx cx edx di si
@@exitD:
        ret
print_D         endp

;******************************************************************************
;* обработка параметров тарификационной сети
;******************************************************************************
MOIN_1          PROC
		test	FG_PROGRAM, _fg_prg_ck0
		jnz	END_MOIN_1
        ; --- гасим флаги получения и/или сбора конфигурации (весь сбор конфигурации сначала)
        MOV     CX, KL_TIS
        LEA     DI, FG_TIS
        MOV     ES, SS:SEG_AX
        MOV     AL, 0
  REP   STOSB

        MOV     ES, SS:SEG_AXD          ; сегмент сети ЦК
        ; очистим уже сформированный список сети для ЦК
        LEA     DI, TIS_TIS
        MOV     CX, KL_TIS
        SHL     CX, 7
        MOV     AL, 0FFH
  REP   STOSB
        ; чистим что-то нигде неиспользуемое !!!!
        LEA     DI, BUF_A_TIS
        MOV     CX, KL_TIS * 64         ; на каждый ЦК 32 слова
        MOV     AL, 0FFH
  REP   STOSB

        ; --- ниже БАЗОВЫМ называется ЦК № которого равен номеру шага основного цикла  ---
        ; ---      РАБОЧИМИ называется ЦК № котрого равен номеру шага вложенного цикла ---                   ---
        ; ---      ТРЕТИЙ ЦК это ЦК имеющий прямые связи с РАБОЧИМ, но не БАЗОВЫМ ЦК   ---                   ---

        ; --- преобразуем TIS_ROAD в TIS_TIS -------------------------
        ; === строим прямые пути между ЦК ============================
        MOV     CX, KL_TIS              ; количество обрабатываемых ЦК
        XOR     DI, DI                  ; номер базового ЦК
        XOR     AX, AX                  ;
CN_SLED_TIS:            ; на каждый шаг цикла DI увеличивается на 16 (так устроен массив TIS_ROAD)
        MOV     BX, 0FH                 ; номер модема: начнем с последнего
CN_SLED_MOD:
        MOV     AL, TIS_ROAD[DI+BX]     ; возьмем значение по тысячнику/модему
        CMP     AL, KL_TIS              ; значение больше максимального?
        JAE     SHORT NET_TIS_IN_MOD    ; да - пропустим
        MOV     SI, DI                  ; \ № базового ЦК
        SHL     SI, 3                   ; / * 128
        ADD     SI, AX                  ; \ базовый ЦК № DI связан с рабочим ЦК № AX
        MOV     ES:TIS_TIS[SI], BL      ; / через модем BX
NET_TIS_IN_MOD:
        DEC     BL                      ; следующий модем
        JNS     SHORT CN_SLED_MOD       ; если не достигли ZERO модема, то на следующий
        ADD     DI, 10H                 ; на следующий ЦК
        LOOP    CN_SLED_TIS             ; следующая итерация

        ; === строим прямые пути между ЦК, которые не имеют прямых связей ====
        MOV     CX, KL_LEV              ; количество транзитных ЦК между ЦК А и ЦК Б - 5
CN_SLED_LEV:            ; цикл по уровням вложения
        PUSH    CX                      ;
        XOR     DI, DI                  ; № ЦК - базовый [0...KL_TIS]
CN_SLED_BAZ_TIS:        ; DI + 128 - цикл по базовым ЦК
        XOR     BX, BX                  ; № ЦК - рабочий [0...KL_TIS]
CN_SLED_RAB_TIS:        ; BX++     - цикл по рабочим ЦК (всем остальным)
        MOV     AL, ES:TIS_TIS[DI+BX]   ; AL - модем
        CMP     AL, 10H                 ; базовый ЦК № DI имеет прямые связи с рабочим ЦК № BX ?
        JAE     SHORT NET_TIS_IN_TIS_TIS; НЕТ -
        MOV     SI, BX                  ; ИМЕЕТ -
        ; --- строим пути с БАЗОВОГО ЦК ко всем ЦК, имеющим связи с рабочим ЦК ------
        SHL     SI, 4                   ; преобразуем № рабочего ЦК BX в индекс для TIS_ROAD
        MOV     CX, 16                  ; просмотрим всё окружение рабочего ЦК (№ BX)
CN_SLED_MOD_IN_RAB:     ; SI++     - цикл по всем модемам рабочего ЦК (начиная с нулевого)
        MOV     AL, TIS_ROAD[SI]        ; № ЦК связанного с рабочим ЦК № BX через текущий модем
        CMP     AL, KL_TIS              ; имеет смысл ?
        JAE     SHORT NET_TIS_IN_RAB    ; НЕТ - через текущий модем связей нет
        PUSH    SI                      ;
        XOR     AH, AH                  ;
        MOV     SI, AX                  ; № ТРЕТЬЕГО ЦК имеющего связь с РАБОЧИМ ЦК
        ADD     SI, DI                  ; № БАЗОВОГО ЦК * 128
        CMP     ES:TIS_TIS[SI], 10H     ; БАЗОВЫЙ ЦК уже знает про ТРЕТИЙ ЦК ?
        JB      SHORT THIS_TIS_EST      ; ДА - есть прямой путь или путь с меньшим числом транзитов
        MOV     AL, ES:TIS_TIS[DI+BX]   ; НЕТ - на ТРЕТИЙ ЦК будем выходить
        MOV     ES:TIS_TIS[SI], AL      ;       с БАЗОВОГО через транзитные ЦК
THIS_TIS_EST:
        POP     SI                      ;
NET_TIS_IN_RAB:
        INC     SI                      ;
        LOOP    CN_SLED_MOD_IN_RAB      ; конец цикла по модемам одного рабочего ЦК
NET_TIS_IN_TIS_TIS:
        INC     BX                      ;
        CMP     BX, KL_TIS              ;
        JB      SHORT CN_SLED_RAB_TIS   ; конец цикла по всем рабочим ЦК
        ADD     DI, 128                 ;
        CMP     DI, KL_TIS * 128        ;
        JB      CN_SLED_BAZ_TIS         ; конец цикла по всем базовым ЦК
        POP     CX                      ;
        DEC     CX                      ;
        JNZ     CN_SLED_LEV             ; конец цикла по количеству транзитных ЦК
END_MOIN_1:
        RET
MOIN_1          ENDP

;******************************************************************************
;* обработка параметров тарификационной сети ?????
;******************************************************************************
MOIN_2          PROC
                XOR     BX,BX
                CALL    MO_PROC
                RET
MOIN_2          ENDP



MO_PROC         PROC
IF _MSP EQ TRUE
                MOV     ES,SS:SEG_ST
                MOV     DI,MO_SMESH[BX]
                MOV     MO_SLOV,BX
                MOV     SI, MO_IDX[BX]
                mov     al, ES:[DI+MO_STAT]; чтение регистра состояния модема
ELSE
                mov     es, ss:SEG_GS
                mov     fs, ss:SEG_GS
                mov     gs, ss:SEG_GS
                MOV     SI, MO_IDX
                ; --- выбор кодировки --- MVT перенёс
                cmp     MO_IDX, 4
                jbe     @@change_not
                mov     eax, ss:Curr_T
                sub     eax, HDB3_TIME
                cmp     eax, 5000
                jb      @@change_not
                xor     MO_RM_FLAG, 10000000b
                and     MO_RM_FLAG, 80h         ; на всяк.случай
                mov     MO_Idx, 6               ; начнем все с начала
                mov     eax, ss:Curr_T
                mov     HDB3_TIME, eax          ; запомним время смены кодировки
                JMP     MO_Init_1
		;
 @@change_not:
                mov     al, fs:[MO_STAT]
ENDIF
		; /MVT 
                and     SI, 7
                shl     SI, 1
                jmp     MO_Link[SI]     ; по индексу на этап обработки МО
MO_END:
                RET
                ENDP

; *** MO_Idx = 6 - НЕТ МОДЕМА **************************************
IF _MSP EQ TRUE
MO6:
                MOV     AL,3Ch                  ;\
                MOV     ES:[DI+MO_OUT],AL       ;|
                MOV     AL,ES:[DI+MO_OUT]       ;|Проверка наличия модема
                CMP     AL,3Ch                  ;|
                JE      SHORT   MO_INIT         ;/
To_MO6:
                MOV     AX,MO_RM[BX]
                mov     ES:[DI+MO_MODE],AL      ; да - сброс гот.
                mov     ax, TIME_2MC            ; нет
                mov     MO_Time[BX], ax         ; зафиксировать время
                mov     MO_Idx[BX], 6
                jmp     MO_End                  ; и остаться на 6 этапе
MO_Init:
                mov     ax, TIME_2MC
                sub     ax, MO_Time[BX]
                cmp     ax, 2000                ; прошла 1сек от обнар-я модема?
                jb      MO_End                  ; нет
                MOV     AX,MO_RM[BX]
                mov     ES:[DI+MO_MODE],AL      ; да - сброс гот.
                mov     AL,BYTE PTR MO_RM[BX]
                mov     BYTE PTR ES:[DI+MO_MODE],AL; сброс гот., отдали ОЗУ
To_MO5:
		mov     MO_Idx[BX], 5
                jmp     MO_End                  ; переходим на 5 этап
To_MO5_From_MO4:
                inc     OSH_VX_P[bx]
		mov     MO_Idx[BX], 5
                jmp     MO_End                  ; переходим на 5 этап

; *** MO_Idx = 5 - ОЖИДАНИЕ НЕ ГОТОВ *******************************
MO5:
                test    al, 8                   ; есть модем ?
                jnz     To_MO6                  ; нет - на 6 этап
                test    al, 042h                ; есть входной поток ?
                jnz     MO_End                  ; нет - ждем
                mov     ax, TIME_2MC            ; да  -
                mov     MO_TIME[BX], ax         ; фиксируем время
To_MO4:         mov     MO_Idx[BX], 4
                mov     MO_Ptr[BX], 2           ; указатель на команду min дл-ы
                jmp     MO_End                  ; и переходим на 4 этап

; *** MO_Idx = 4 - ТАЙМ - АУТ **************************************
MO4:
                test    al, 8                   ; есть модем ?
                jnz     To_MO6                  ; нет - уйти на 6 этап
                test    al, 040h                ; есть входной поток ?
                jnz     To_MO5_From_MO4         ; нет - уйти на 5 этап
                test    al, 2                   ; есть готовность ?
                jnz     SHORT To_MO3            ; да  - переходим в состояние 3
                mov     ax, TIME_2MC            ; нет -
                sub     ax, MO_TIME[BX]
                cmp     ax, MO_TOUT[BX]         ; тайм-аут истек ?
                jb      MO_End                  ; нет - остались на 4 этапе
TO_MO2:
                mov     ax, TIME_2MC            ; да  -
                mov     MO_TIME[BX], ax         ; фиксируем время
                mov     al,ES:[DI+MO_IN]        ; модем в режим передачи
                or      MO_RM[BX], 8
                mov     al, BYTE PTR MO_RM[BX]
                mov     MO_Idx[BX], 2
                mov     ES:[DI+MO_MODE], al     ; взводим готовность
                jmp     MO_End                  ; уйти на 2 этап

To_MO3:         mov     ax, TIME_2MC
                mov     MO_TIME[BX], ax         ; фиксируем время
                mov     al, ES:[DI+MO_IN]       ; модем в режим передачи
        TEST    FG_PROG,2                       ; моноблок?
        jnz     @@mo3_no_ff                     ; да пропустим
                mov     BYTE PTR ES:[DI+MO_LEN], 0ffh ; модем в режим приема
@@mo3_no_ff:
                or      MO_RM[BX], 8
                mov     al, BYTE PTR MO_RM[BX]
                mov     MO_Idx[BX], 3
                mov     ES:[DI+MO_MODE], al     ; взводим готовность
                jmp     MO_End                  ; уйти на 3 этап

; *** MO_Idx = 3 - ОЖИДАНИЕ "ЕЕ" ************************************
MO3:
                test    al, 8                   ; есть модем ?
                jnz     To_MO6                  ; нет -
                test    al, 040h                ; есть входной поток ?
                jnz     To_MO5_NVP              ; нет - уйти на 5 этап
                test    al, 2                   ; есть готовность ?
                jz      To_MO4NR                ; нет - уйти на 4 этап
                cmp     Mo_Flg, 0
                jz      Time_Out                ; нет - на проверку тайм-аута
                MOV     MO_FLG, 0
                cmp     BYTE PTR ES:[DI+MO_IN], 2 ; длина пакета = 2 ?
                jnz     To_MO5NR                ; нет - уйти на 5 этап
                cmp     BYTE PTR ES:[DI+MO_IN + 1], 0eeh  ; код команды "EE" ?
                jnz     To_MO5NR                ; нет - уйти на 5 этап
                cmp     BYTE PTR ES:[DI+MO_IN + 2], 0f0h  ; второй байт = "F0" ?
                jnz     To_MO5NR                ; нет - уйти на 5 этап
                mov     ax, TIME_2MC            ; принят правильный пакет "ЕЕ"
                mov     MO_Time[BX], ax         ; фиксируем время
                mov     MO_Err[BX], 0           ; счетчик ошибок приема МО = 0
                mov     MO_Num[BX], 0;FFH       ; номер пакета = 1
                mov     MO_NumInc[BX], 0        ; при переспросе - тек. номер
if 1 ;_SORM_MONO NE TRUE 
                LEA     BP,ES:[DI+MO_OUT + 0f0h]
                mov     BYTE PTR ES:[BP], 2     ; Запись в ОЗУ пакета иниц-ии
                mov     BYTE PTR ES:[BP + 1], 0eeh
                mov     BYTE PTR ES:[BP + 2], 0f0h
                mov     BYTE PTR ES:[DI+MO_LEN], 0f0h ; отправить пакет иниц-ии
else
		LEA     BP,ES:[DI+MO_OUT]
		mov     BYTE PTR ES:[BP], 2     ; Запись в ОЗУ пакета иниц-ии
		mov     BYTE PTR ES:[BP + 1], 0eeh
		mov     BYTE PTR ES:[BP + 2], 0f0h
		mov     BYTE PTR ES:[DI+MO_LEN], 3      ; отправить пакет иниц-ии
endif
		
                mov     MO_Idx[BX], 0
                jmp     MO_End                  ; уходим на 0 этап: РАБОТА
                ; --------------------------------------
To_MO4NR:
                mov     ax, TIME_2MC            ; уйти на 4 этап с не готов
                mov     MO_Time[BX], ax         ; фиксируем время
                and     MO_RM[BX], 0f7h
                mov     al,BYTE PTR MO_RM[BX]
                mov     ES:[DI+MO_MODE], al     ; выдать не готов
                mov     MO_Idx[BX], 4           ; уйти на 4 этап
                inc     OSH_GOT[BX]             ; увеличим счетчик ошибок готовности
                jmp     MO_End
                ; --------------------------------------
Time_Out:
                mov     ax, TIME_2MC
                sub     ax, MO_Time[BX]
                cmp     ax, MO_TOUT[BX]         ; тайм-аут модема истек ?
                jb      MO_End                  ; нет - продолжаем работу
                INC     OSH_T_O[BX]
To_MO5_NVP:
                inc     OSH_VX_P[bx]
To_MO5NR:
                and     MO_RM[BX], 0f7h         ; уйти на 5 этап с не готов
                mov     al, BYTE PTR MO_RM[BX]
                mov     ES:[DI+MO_MODE], al     ; выдать не готов
                mov     MO_Idx[BX], 5
                jmp     MO_End                  ; уйти на 4 этап

; *** MO_Idx = 2 - ОЖИДАНИЕ "ГОТОВ" *********************************
MO2:
                test    al, 8                   ; есть модем ?
                jnz     To_MO6                  ; нет - уйти на 6 этап
                test    al, 040h                ; есть входной поток ?
                jnz     To_MO5_NVP              ; нет - уйти на 5 этап
                test    al, 2                   ; есть готовность ?
                jz      Time_Out                ; нет - на проверку тайм-аута
                mov     ax, TIME_2MC            ; да  -
                mov     MO_Time[BX], ax         ; фиксируем время
if 1 ;_SORM_MONO NE TRUE 
                LEA     BP,ES:[DI+MO_OUT+0f0h]
                mov     BYTE PTR ES:[BP], 2     ; Запись в ОЗУ пакета иниц-ии
                mov     BYTE PTR ES:[BP + 1], 0EEh
                mov     BYTE PTR ES:[BP + 2], 0f0h
                mov     BYTE PTR ES:[DI+MO_LEN], 0f0h ; отправить пакет иниц-ии
else
                LEA     BP,ES:[DI+MO_OUT]
		mov     BYTE PTR ES:[BP], 2    ; Запись в ОЗУ пакета иниц-ии
		mov     BYTE PTR ES:[BP + 1], 0eeh
		mov     BYTE PTR ES:[BP + 2], 0f0h
		mov     BYTE PTR ES:[DI+MO_LEN], 3    ; отправить пакет иниц-ии
endif
		
                mov     MO_Idx[BX], 1
                jmp     MO_End                  ; уйти на 1 этап

; *** MO_Idx = 1 - ОЖИДАНИЕ "ЕЕ" ************************************
MO1:
                test    al, 8                   ; есть модем ?
                jnz     To_MO6                  ; нет - уйти на 6 этап
                test    al, 040h                ; есть входной поток ?
                jnz     To_MO5_NVP               ; нет - уйти на 5 этап
                test    al, 2                   ; есть готовность ?
                jz      To_MO4NR                ; нет - уйти на 4 этап
                cmp     mo_flg,0                ; пришел пакет?
                JZ      TIME_OUT                ; нет, ждем
                MOV     MO_FLG,0
                LEA     BP,ES:[DI+MO_IN]        ; проверка принятого пакета
                cmp     BYTE PTR ES:[BP], 2     ; длина пакета = 2 ?
                jnz     To_MO5NR                ; нет - уйти на 5 этап
                cmp     BYTE PTR ES:[BP + 1], 0eeh ; код команды "ЕЕ" ?
                jnz     To_MO5NR                ; нет - уйти на 5 этап
                cmp     BYTE PTR ES:[BP + 2], 0f0h ; второй байт = F0 ?
                jnz     To_MO5NR                ; нет - уйти на 5 этап
                mov     MO_Err[BX], 0           ; счетчик ошибок приема МО = 0
                mov     MO_Num[BX], 0;FFH       ; номер пакета = 0
                mov     MO_NumInc[BX], 1        ; при переспросе - тек. номер+1
if 1 ;_SORM_MONO NE TRUE 
                LEA     BP,ES:[DI+MO_OUT + 0f0h]
                mov     BYTE PTR ES:[BP], 2     ; Запись в ОЗУ пакета иниц-ии
                mov     BYTE PTR ES:[BP + 1], 0ddh
                mov     BYTE PTR ES:[BP + 2], 0dfh
                mov     BYTE PTR ES:[DI+MO_LEN], 0f0h ; отправить пакет иниц-ии
else
                LEA     BP,ES:[DI+MO_OUT]
		mov     BYTE PTR ES:[BP], 2     ; Запись в ОЗУ пакета иниц-ии
		mov     BYTE PTR ES:[BP + 1], 0ddh
		mov     BYTE PTR ES:[BP + 2], 0dfh
		mov     BYTE PTR ES:[DI+MO_LEN], 3 ; отправить пакет иниц-ии
endif
                mov     MO_Idx[BX],0
ELSE
; в AL - состояние модема
MO6:    mov     BYTE PTR gs:[MO_OUT], 3ch
        mov     ah, gs:[MO_OUT]
        cmp     ah, 3ch                 ; есть модем ?
        je      SHORT MO_Init           ; ДА - инициализировать его
        jne     M6                      ; НЕТ
To_MO6:
        mov     MO_Idx, 6
M6:
        mov     eax, SS:Curr_T
        mov     MO_Time, eax            ; зафиксировать время
        jmp     MO_End                  ; и остаться на 6 этапе
MO_Init:
        mov     eax, SS:Curr_T
        sub     eax, MO_Time
        cmp     eax, 300                ; прошла 4 сек от обнар-я модема?
        jb      MO_End                  ; НЕТ -
	; MVT перенёс
MO_Init_1:
        mov     BYTE PTR gs:[MO_MODE], 1
        lea     si, KOD
        mov     di, ADR_OZU_COD
	push    es
        mov     es, SS:SEG_GS
        mov     cx, 256
        rep     MOVSB

        lea     si, KOD
        mov     di, ADR_OZU_DEC
        mov     cx, 256
        rep     movsb
	pop     es
	; /MVT перенёс
	and     MO_RM, 077h             ; уйти на 5 этап с не готов MVT: надо сбрасывать и старший бит тоже
                                        ; иначе не будет переключаться HDB3-MII
        mov     al, MO_RM_FLAG  	; MVT перенёс
        or      MO_RM, al
        mov     al, MO_RM
        mov     fs:[MO_MODE], al
        ;mov     al, gs:[MO_IN]
        mov     MO_Idx, 5
        jmp     MO_End                  ; переходим на 5 этап

; *** MO_Idx = 5 - ОЖИДАНИЕ НЕ ГОТОВ *******************************
MO5:
        mov     BYTE PTR gs:[MO_OUT], 3ch
        mov     ah, gs:[MO_OUT]
        cmp     ah, 3ch                 ; есть модем ?
        jne     To_MO6                  ; НЕТ - на 6 этап

        test    al, 040h                ; есть входной поток ?
        jnz     MO_End                  ; НЕТ - ждем
        test    al, 2                   ; есть НЕ ГОТОВ ?
        jnz     MO_End                  ; НЕТ - ждем
        mov     eax, SS:Curr_T          ; ДА  -
        mov     MO_Time, eax            ; фиксируем время

To_MO4: mov     MO_Idx, 4
        jmp     MO_End                  ; и переходим на 4 этап

; *** MO_Idx = 4 - ТАЙМ - АУТ **************************************
MO4:
        test    al, 040h                ; есть входной поток ?
        jnz     To_MO5                  ; НЕТ - уйти на 5 этап
        test    al, 2                   ; есть готовность ?
        jnz     SHORT To_MO3            ; ДА  - переходим в состояние 3
        mov     eax, SS:Curr_T          ; НЕТ -
        sub     eax, MO_Time
        cmp     eax, MO_TOUT            ; тайм-аут истек ?
        jb      MO_End                  ; НЕТ - остались на 4 этапе
        mov     eax, SS:Curr_T          ; ДА  -
        mov     MO_Time, eax            ; фиксируем время
        ;mov     al, gs:[_MO_IN]         ; модем в режим передачи
        or      MO_RM, 8
        mov     al, MO_RM
        mov     MO_Flg, 0
        mov     MO_Idx, 2
        mov     fs:[MO_MODE], al       ; взводим готовность
        jmp     MO_End                  ; уйти на 2 этап

To_MO3: mov     eax, SS:Curr_T
        mov     MO_Time, eax            ; фиксируем время
        ;mov     al, gs:[_MO_IN]         ; модем в режим передачи
        or      MO_RM, 8
        mov     al, MO_RM
        mov     MO_Flg, 0
        mov     MO_Idx, 3
        mov     fs:[MO_MODE], al       ; взводим готовность
        jmp     MO_End                  ; уйти на 3 этап

To_MO5: ;;;!!!inc     MO_NVP
        inc     OSH_VX_P;;;!!!
        mov     MO_Idx, 5
        jmp     MO_End                  ; переходим на 5 этап

; *** MO_Idx = 3 - ОЖИДАНИЕ "ЕЕ" ************************************
MO3:    test    al, 040h                ; есть входной поток ?
        jnz     To_MO5NR                ; НЕТ - уйти на 5 этап
        test    al, 2                   ; есть готовность ?
        jz      To_MO4NR                ; НЕТ - уйти на 4 этап
        cmp     MO_Flg, 0               ; пакет принят ?
        je      Time_Out                ; НЕТ - на проверку тайм-аута
        mov     di, MO_IN              ; ДА  - проверить пакет
        ;mov     al,gs:[di]
        cmp     BYTE PTR gs:[di], 2    ; длина пакета = 2 ?
        jnz     To_MO5NR2               ; НЕТ - уйти на 5 этап
        cmp     BYTE PTR gs:[di + 1], 0eeh  ; код команды "EE" ?
        jnz     To_MO5NR2               ; НЕТ - уйти на 5 этап
        cmp     BYTE PTR gs:[di + 2], 0f0h  ; второй байт = "F0" ?
        jnz     To_MO5NR2               ; НЕТ - уйти на 5 этап
        mov     eax, SS:Curr_T          ; принят правильный пакет "ЕЕ"
        mov     MO_Time, eax            ; фиксируем время
        mov     MO_Err, 0               ; счетчик ошибок приема МО = 0
        mov     MO_Num, 0               ; номер пакета = 0
        mov     MO_NumInc, 0            ; при переспросе - тек. номер
        mov     MO_Flg, 0               ; флаг приема пакета сброшен
;;;!!!        mov     al, No_CFG              ; наличие конфигурации
;;;!!!        mov     Ask_CFG, al             ;  или  флаг запроса конфиг-ии
if 0;1;;;ck_0 MVT
        mov     di, MO_OUT + 0f0h
        mov     BYTE PTR gs:[di], 2     ; Запись в ОЗУ пакета иниц-ии
        mov     BYTE PTR gs:[di + 1], 0eeh
        mov     BYTE PTR gs:[di + 2], 0f0h
        mov     BYTE PTR fs:[MO_LEN], 0f0h   ; отправить пакет иниц-ии
else
        mov     di, MO_OUT
        mov     BYTE PTR gs:[di], 2     ; Запись в ОЗУ пакета иниц-ии
        mov     BYTE PTR gs:[di + 1], 0eeh
        mov     BYTE PTR gs:[di + 2], 0f0h
        mov     BYTE PTR fs:[MO_LEN], 3      ; отправить пакет иниц-ии
endif
        mov     MO_Idx, 0
        mov     eax, SS:Curr_T
        mov     Cfg_Time, eax           ; фиксируем время перехода в работу
        jmp     MO_End                  ; уходим на 0 этап: РАБОТА

To_MO4NR:
;;;!!!        inc     MO_NR                   ; счетчик "не готово"
        inc     OSH_GOT;;;!!!
        mov     eax, SS:Curr_T             ; уйти на 4 этап с не готов
        mov     MO_Time, eax            ; фиксируем время
        and     MO_RM, 0f7h
        mov     al, MO_RM
        mov     fs:[MO_MODE], al        ; выдать не готов
        mov     MO_Idx, 4
        jmp     MO_End                  ; уйти на 4 этап

Time_Out:
        mov     eax, SS:Curr_T
        sub     eax, MO_Time
        cmp     eax, MO_TOUT            ; тайм-аут модема истек ?
        jb      MO_End                  ; НЕТ - продолжаем работу
;;;!!!        inc     MO_TO                   ; счетчик "тайм-аутов"
        inc     OSH_T_O;;;!!!
        cmp     MO_Idx, 0
        jne     To_MO5NR2
        and     MO_RM, 0f7h
        mov     al, MO_RM
        mov     fs:[MO_MODE], al
        mov     MO_Idx, 5
        jmp     MO_End

To_MO5NR:
;;;!!!        inc     MO_NVP                  ; счетчик нет "вх. потока"
        inc     OSH_VX_P;;;!!!
To_MO5NR2:
        and     MO_RM, 0f7h             ; уйти на 5 этап с не готов
        mov     al, MO_RM
        mov     fs:[MO_MODE], al        ; выдать не готов
        mov     MO_Idx, 5
        jmp     MO_End                  ; уйти на 5 этап

; *** MO_Idx = 2 - ОЖИДАНИЕ "ГОТОВ" *********************************
MO2:
        test    al, 040h                ; есть входной поток ?
        jnz     To_MO5NR                ; НЕТ - уйти на 5 этап
        test    al, 2                   ; есть готовность ?
        jz      Time_Out                ; НЕТ - на проверку тайм-аута

        mov     eax, SS:Curr_T          ; ДА  -
        mov     MO_Time, eax            ; фиксируем время
        mov     MO_Flg, 0               ; сброс флага приема пакета
if 0;1;;;ck_0 MVT
        mov     di, MO_OUT + 0f0h
        mov     BYTE PTR gs:[di], 2    ; Запись в ОЗУ пакета иниц-ии
        mov     BYTE PTR gs:[di + 1], 0eeh
        mov     BYTE PTR gs:[di + 2], 0f0h
        mov     BYTE PTR fs:[MO_LEN], 0f0h ; отправить пакет иниц-ии
else
        mov     di, MO_OUT
        mov     BYTE PTR gs:[di], 2    ; Запись в ОЗУ пакета иниц-ии
        mov     BYTE PTR gs:[di + 1], 0eeh
        mov     BYTE PTR gs:[di + 2], 0f0h
        mov     BYTE PTR fs:[MO_LEN], 3    ; отправить пакет иниц-ии
endif
        mov     MO_Idx, 1
        jmp     MO_End                  ; уйти на 1 этап

; *** MO_Idx = 1 - ОЖИДАНИЕ "ЕЕ" ************************************
MO1:    test    al, 040h                ; есть входной поток ?
        jnz     To_MO5NR                ; НЕТ - уйти на 5 этап
        test    al, 2                   ; есть готовность ?
        jz      To_MO4NR                ; НЕТ - уйти на 4 этап
        cmp     MO_Flg, 0               ; есть пакет ?
        je      Time_Out                ; НЕТ - на проверку тайм-аута
        mov     di, MO_IN               ; проверка принятого пакета
        cmp     BYTE PTR gs:[di], 2     ; длина пакета = 2 ?
        jnz     To_MO5NR2               ; НЕТ - уйти на 5 этап
        cmp     BYTE PTR gs:[di + 1], 0eeh ; код команды "ЕЕ" ?
        jnz     To_MO5NR2               ; НЕТ - уйти на 5 этап
        cmp     BYTE PTR gs:[di + 2], 0f0h ; второй байт = F0 ?
        jnz     To_MO5NR2               ; НЕТ - уйти на 5 этап
        mov     MO_Err, 0               ; счетчик ошибок приема МО = 0
        mov     MO_Num, 0               ; номер пакета = 0
        mov     MO_NumInc, 1            ; при переспросе - тек. номер+1
        mov     MO_Flg, 0               ; сбросить флаг приема пакета
;;;!!!        mov     al, No_CFG              ; наличие конфигурации
;;;!!!        mov     Ask_CFG, al             ;  или  флаг запроса конфиг-ии
if 0;1;;;ck_0 MVT
        mov     di, MO_OUT + 0f0h
        mov     BYTE PTR gs:[di], 2     ; Запись в ОЗУ пакета иниц-ии
        mov     BYTE PTR gs:[di + 1], 0ddh
        mov     BYTE PTR gs:[di + 2], 0dfh
        mov     BYTE PTR fs:[MO_LEN], 0f0h ; отправить пакет иниц-ии
else
        mov     di, MO_OUT
        mov     BYTE PTR gs:[di], 2     ; Запись в ОЗУ пакета иниц-ии
        mov     BYTE PTR gs:[di + 1], 0ddh
        mov     BYTE PTR gs:[di + 2], 0dfh
        mov     BYTE PTR fs:[MO_LEN], 3 ; отправить пакет иниц-ии
endif
        mov     MO_Idx, 0
        mov     eax, SS:Curr_T
        mov     Cfg_Time, eax           ; фиксируем время перехода в работу
ENDIF
                jmp     MO_End                  ; уходим на 0 этап: РАБОТА
; *** MO_Idx = 0 - РАБОТА *******************************************
MO0:
                test    al, 8                   ; есть модем ?
                jnz     To_MO6                  ; нет - уйти на 6 этап

                test    al, 040h                ; есть входной поток ?
IF _MSP EQ TRUE
                jnz     To_MO5_NVP              ; нет - уйти на 5 этап
ELSE
                jnz     To_MO5;;;!!!_NRVP              ; нет - уйти на 5 этап
ENDIF
                test    al, 2                   ; есть готовность ?
                jz      To_MO4NR                ; нет - уйти на 4 этап
                cmp     Mo_Flg, 0
		JZ	TIME_OUT
                MOV     MO_FLG, 0
IF _MSP EQ TRUE
                LEA     SI,ES:[DI+MO_IN]
		MOVZX	CX,ES:[SI]
                mov     MO_Len2[BX], cx         ; запомним длину еще раз в Len2
ELSE
                mov     si, MO_IN
                movzx   cx, es:[si]
                mov     MO_Len2, cx             ; запомним длину еще раз в Len2
ENDIF
                cmp     cx, 2                   ; длина принятого пакета < 2 ?
                jb      Pack_Err                ; ДА - на ошибку
		CMP	CX,2
		JE	PACK_MAKE
                mov     al,ES:[SI + 1]
                cmp     al, 0cch                ; принят запрос на повтор ?
                je      Pack_CC                 ; ДА -
                cmp     al, 0ddh                ; отправленый ранее принят ?
                jne     Pack_Err                ; НЕТ - на ошибку
                CMP     CX,2
                JE      PACK_MAKE
                PUSH    DI
                mov     dx, 0                   ; dx - для расчета контр.суммы
IF _MSP EQ TRUE
                MOV     DI,MO_SM_BUF[BX]
                lea     di, MO_BufIn[DI]
ELSE
                lea     di, MO_BufIn
ENDIF
                MOV     ES,SS:SEG_AX
IF _MSP EQ TRUE
                MOV     DS,SS:SEG_ST
ELSE
                MOV     DS,SS:SEG_GS
ENDIF
Pack_RD:
                lodsb                           ; чтение байта из ОЗУ приема МО
                ADD     DL, AL                  ; накопление контр.суммы
                stosb                           ; запись в буфер приема МО
                loop    Pack_RD

                lodsb                           ; чтение принятой контр.суммы
                stosb                           ; запись в буфер приема МО
IF _MSP EQ TRUE
		MOV	ES,SS:SEG_ST
ELSE
		MOV	ES,SS:SEG_GS
ENDIF
                MOV     DS,SS:SEG_AX
                POP     DI
                cmp     al, dl                  ; контрольные суммы совпали ?
                jne     Pack_Err                ; НЕТ - на обработку ошибки
IF _MSP EQ TRUE
                MOV     BP,MO_SM_BUF[BX]
                LEA     BP,MO_BUFIN[BP]
                MOV     AX,MO_LEN2[BX]

                ADD     MO_LEN2[BX],BP
                ADD     BP, MO_Ptr[BX]          ; bp - указатель в буф. приема

SLED_MO:        xor     bx, bx                  ; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                MOV     BX,MO_SLOV
                MOV     DI,MO_SMESH[BX]         ;ЗАДАН АПРИОРНО
                cmp     bp, MO_Len2[BX]         ; обработан весь пакет ?
                JNB     Pack_Make               ; ДА - на подготовку посылки

                test    FG_PROGRAM, _fg_prg_mi     ; если включена отладка входящего МО
ELSE
		lea	bp, MO_BufIn
		add	MO_Len2, bp
                add     bp, MO_Ptr              ; bp - указатель в буф. приема

if _AUX_MO_DEBUG
                jmp mo_sled
SLED_MO:
                pop ax
                pop ax
                pop ax
                pop eax
                pop ax
                pop ax
mo_sled:
else
SLED_MO:
endif
                cmp     bp, MO_Len2            ; обработан весь пакет ?
                jae     Pack_Make              ; ДА - на подготовку посылки

;;;<Yu> -----------------------------------------
                test    FG_PROGRAM, M_OR_5     ; если включена отладка входящего МО
ENDIF
                jz      COM_SORM_TIS
push    bx  si
                mov     bl, BYTE PTR ds:[bp]
                mov     si, offset MO_Numb
                cmp     bl, 235
                jne     @@no_235                        ; пришла 235-ая
                mov     si, offset MO_Numb_235
                mov     bl, BYTE PTR ds:[bp + 4]
  @@no_235:
                DB      9AH                     ;\
                DW      OFFSET Find_Numb_MO      ;|  выводим только выделенные команды
                DW      224                     ;/
pop     si bx
                test    FG_Curr_MO_Num, _find_cmd
                jnz     MO_In_Is
                jmp     COM_SORM_TIS

MO_In_Is:       pushad
                ; --- отладка одной линии - начало
                cmp     OneLn_active, 0                 ; режим одной линии активен ?
                je      MO_In_Draw                      ; НЕТ - выводим всё подряд
                cmp     BYTE PTR ds:[bp], 100           ; занятие от линии ?
                je      in_OneLn_starting               ; ДА - проверим: может от нашей
                cmp     BYTE PTR ds:[bp], 110           ; занятие МГ от линии ?
                je      in_OneLn_starting               ; ДА - проверим: может от нашей
                cmp     BYTE PTR ds:[bp], 112           ; выход на линию Б ?
                jne     in_OneLn_running                ; НЕТ - ни 100, ни 110, ни 112 - слежение не начать !
                ; --- попытка начать наблюдение за вызываемой линией
in_OneLn_B_starting:                                    ; ДА - проверим: может на нашу
                mov     al, ds:[bp + 4]                 ; № ЦКб
                cmp     al, OneLn_CK                    ; == наблюдаемому ?
                jne     in_OneLn_AB                     ; НЕТ -
                mov     ax, ds:[bp + 7]                 ; полный № линии Б
                cmp     ax, OneLn_PhysNum               ;  == наблюдаемому ?
                jne     in_OneLn_AB                     ; НЕТ -
                ; ---
                mov     al, ds:[bp + 1]                 ;
                mov     OneLn_CkA, al                   ;
                mov     ax, ds:[bp + 2]                 ; \ запоминаем рабочий № А
                mov     OneLn_WorkA, ax                 ; / связанный с наблюдаемой линией
                ; ---
                mov     al, OneLn_CK                    ;
                mov     OneLn_CkB, al                   ;
                mov     ax, OneLn_PhysNum               ;
                mov     OneLn_PhysB,ax                  ;
                mov     ax, ds:[bp + 5]                 ; \ запоминаем рабочий № Б
                mov     OneLn_WorkB, ax                 ; / связанный с наблюдаемой линией
                mov     OneLn_running, 255              ; стартуем наблюдение
                jmp     MO_In_Draw                      ; рисуем выход на линию Б
                ; ---                                   ; ещё НЕТ - попытка привязать вызываемую сторону
in_OneLn_AB:
                mov     al, ds:[bp + 1]                 ; № ЦКа
                cmp     al, OneLn_CkA                   ; == наблюдаемому ?
                jne     MO_In_End                       ; НЕТ -
                mov     ax, ds:[bp + 2]                 ; рабочий № линии А
                cmp     ax, OneLn_WorkA                 ;  == наблюдаемому ?
                jne     MO_In_End                       ; НЕТ -
                mov     al, ds:[bp + 4]                 ; \
                mov     OneLn_CkB, al                   ; /
                mov     ax, ds:[bp + 7]                 ; \
                mov     OneLn_PhysB, ax                 ; /
                mov     ax, ds:[bp + 5]                 ; \ запоминаем рабочий
                mov     OneLn_WorkB, ax                 ; / № Б
                jmp     MO_In_Draw                      ; рисуем выход на линию Б
                ; --- попытка начать наблюдение за вызывающей линией
in_OneLn_starting:
                mov     al, ds:[bp + 1]                 ; № ЦКа
                cmp     al, OneLn_CK                    ; == наблюдаемому ?
                jne     MO_In_End                       ; НЕТ -
                mov     ax, ds:[bp + 4]                 ; полный № линии А
                cmp     ax, OneLn_PhysNum               ;  == наблюдаемому ?
                jne     MO_In_End                       ; НЕТ -
                ; ---
                mov     al, OneLn_CK                    ;
                mov     OneLn_CkA, al                   ;
                mov     ax, OneLn_PhysNum               ;
                mov     OneLn_PhysA,ax                  ;
                mov     ax, ds:[bp + 2]                 ; \ запоминаем рабочий № А
                mov     OneLn_WorkA, ax                 ; / связанный с наблюдаемой линией
                ; ---
                mov     OneLn_CkB, 0                    ;
                mov     OneLn_PhysB, 0                  ;
                mov     OneLn_WorkB, 0                  ; рабочий номер Б пока неизвестен
                mov     OneLn_running, 255              ; стартуем наблюдение
                jmp     MO_In_Draw                      ; рисуем занятие
                ; --- идёт наблюдение за линией
in_OneLn_running:
                cmp     OneLn_running, 0                ; наблюдаемая линия в работе ?
                je      MO_In_End                       ; НЕТ - пропустим
                ; ----
                cmp     BYTE PTR ds:[bp], 235           ; 235-ая ?
                jne     in_OneLn_SORM                   ; НЕТ - проверим СОРМовские команды
                movzx   dx, ds:[bp + 6]                 ; номер рабочий (без модема)
                mov     ax, ds:[bp + 1]                 ; номер физический (с модемом)
                and     ax, 1e00h                       ; выделим № модема
                shr     ax, 2                           ; двигаем его с физической в рабочую позицию
                or      dx, ax                          ; объединяем № рабочий и № модема
                mov     al, ds:[bp + 5]
                jmp     in_OneLn_Tst_A
                ; ---
in_OneLn_SORM:  mov     al, ds:[bp + 1]
                mov     dx, ds:[bp + 2]
                cmp     BYTE PTR ds:[bp], 100           ; \
                jb      MO_In_End                       ;  \ интересуемся только
                cmp     BYTE PTR ds:[bp], 123           ;  / СОРМовскими командами
                ja      MO_IN_End                       ; /
in_OneLn_Tst_A: cmp     al, OneLn_CkA
                jne     in_OneLn_B_continue
                cmp     dx, OneLn_WorkA
                je      in_OneLn_stopping
in_OneLn_B_continue:
                cmp     al, OneLn_CkB
                jne     MO_In_End
                cmp     dx, OneLn_WorkB
                jne     MO_In_End
in_OneLn_stopping:
                cmp     BYTE PTR ds:[bp], 106
                jne     MO_In_Draw
                mov     OneLn_running, 0
                mov     OneLn_WorkA, 0
                mov     OneLn_WorkB, 0
                ; --- отладка одной линии - конец
MO_In_Draw:
                mov     ah, cBLUE * 16 + clYELLOW
                lea     si, __mo_in_str
                mov     di, __inf_y
                call    print_S
                movzx   bx, ds:[bp]
                movzx   cx, __mo_in_lens[bx]
                cmp     bl, 235
                jne     MO_In_Len
                movzx   cx, ds:[bp + 3]
                add     cx, 4
MO_In_Len:      mov     di, __inf_y
                add     di, 2
MO_In_Loop:     push    cx
                mov     bl, ds:[bp]
                mov     cx, 2
                call    print_H
                pop     cx
                inc     bp
                add     di, 4
                loop    MO_In_Loop
                add     __inf_y, 160
                cmp     __inf_y, __INF_STR_COUNT * 160
                jb      MO_In_Str
                mov     __inf_y, 0
MO_In_Str:      lea     si, __inf_clear
                mov     di, __inf_y
                call    print_S
MO_In_End:
                popad
;;;<Yu> -----------------------------------------

COM_SORM_TIS:
                mov     fs, ss:SEG_SOED
                mov     gs, ss:SEG_KONF
IF _MSP NE TRUE
if _AUX_MO_DEBUG
                push WORD  PTR 1234h
                push WORD  PTR 5678h
                push DWORD PTR ds:[bp]
                push WORD  PTR ds:[bp + 4]
                push WORD  PTR 5678h
                push WORD  PTR 1234h
endif
ENDIF
                movzx   SI, DS:[BP]
if 0
push    eax
mov     eax, DWORD PTR ds:[bp]
mov     _ss_code, eax
mov     eax, DWORD PTR ds:[bp + 4]
mov     _ss_1, eax
mov     eax, DWORD PTR ds:[bp + 8]
mov     _ss_2, eax
mov     eax, DWORD PTR ds:[bp + 12]
mov     _ss_3, eax
mov     eax, DWORD PTR ds:[bp + 16]
mov     _ss_4, eax
pop     eax
endif
		; MVT перенёс
IF _MSP NE TRUE
                PUSH    eax
                mov     eax, ss:Curr_T
                mov     HDB3_TIME, eax
                mov     MO_Time, eax
                POP     eax
ENDIF
		; /MVT перенёс
IF _MSP NE TRUE
if _DebugKonfig
	@dword_to_file esi,2
	call write_cr_lf_to_debug_file
endif
endif
                shl     SI, 1
                JMP     MO_COM[SI]              ; ПЕРЕХОД ПО КОДУ КОМАНДЫ

if _AUX_MO_DEBUG
BAD_MO:
                pop ax
                pop ax
                pop ax
                pop eax
                pop ax
                pop ax
else
BAD_MO:
endif
                xor     bx, bx                  ; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                push    ds
                pop     es
                ; указ. на очередной буфер
                movzx   di, bad_MO_wr_ptr
                shl     di, _BAD_MO_SHL
                add     di, OFFSET bad_MO_buff
                ; смещение ошибочной команды относительно начала пакета
                ;;;mov     ax, bp
                ; --- {Paul} --- <29/11/2012>
                mov     si, bp                          ; на начало ошибочной команды
                mov     ax, offset MO_BufIn + 256
                sub     ax, bp                          ; столько байт до конца пакета
                push    bx
                mov     bx, ax
IF _MSP EQ TRUE
                ;mov     si, MO_SM_BUF[bx]
                ;lea     si, MO_BUFIN[si]
ELSE
                ;lea     si, MO_BufIn
ENDIF
                ;;;sub     ax, si
                stosb
                ; время
                mov     al, CHAS
                stosb
                mov     al, MIN
                stosb
                mov     al, SECUNDA
                stosb
                ; сам пакет - всегда копируем столько, сколько влезет в буфер
                mov     cx, _BAD_MO_SIZE - _BAD_MO_PACKET - 1
                cmp     bx, cx
                jae     @@go_movsb
                mov     cx, bx
   @@go_movsb:  rep     movsb
                pop     bx
                ; --- {Paul} --- <END>
                cmp     bad_MO_count, _BAD_MO_MAX
                jae     bad_MO_buff_full
                inc     bad_MO_count
bad_MO_buff_full:
                inc     bad_MO_wr_ptr
                and     bad_MO_wr_ptr, _BAD_MO_MAX - 1

; *** обработка ошибок ***************************************************
IF _MSP EQ TRUE
                MOV     ES,SS:SEG_ST
                MOV     DI,MO_SMESH[BX]

                MOV     SI,MO_SM_BUF[BX]
                LEA     DX,MO_BUFIN[SI]
                SUB     BP,DX
;;;                dec     bp                   ; непонятная ком. в прав. пакете
                mov     ax, bp
                mov     BYTE PTR MO_Ptr[BX], al ; запоминаем место сбойной ком-ы
Pack_Err:
                INC     OSH_PAK[BX]
                inc     MO_Err[BX]              ; общая ошибка (формата) пакета
                cmp     MO_Err[BX], 3           ; подряд 3 ошибки ?
                JB      Pack_Ask                ; НЕТ - на запрос повтора
PEREGRUS_MO:
                ;;;INC     OSH_PAK[BX]
                mov     MO_Idx[BX], 5           ; ДА - на разрыв связи
                and     MO_RM[BX], 0f7h
                mov     al, BYTE PTR MO_RM[BX]
                mov     ES:[DI+MO_MODE], al     ; снимаем готовность
ELSE
Pack_CErr:
                lea     dx, MO_BUFIN
                sub     bp, dx
                mov     ax, bp
                mov     BYTE PTR MO_Ptr, al         ; запоминаем место сбойной ком-ы
Pack_Err:
                INC     OSH_PAK
                inc     MO_Err                      ; общая ошибка (формата) пакета
                cmp     MO_Err, 3                   ; подряд 3 ошибки ?
                JB      Pack_Ask                    ; НЕТ - на запрос повтора
PEREGRUS_MO:
;;;                INC     OSH_PAK
                mov     MO_Idx, 5                   ; ДА - на разрыв связи
                and     MO_RM, 0f7h
                mov     al, BYTE PTR MO_RM
                mov     es:[MO_MODE], al            ; снимаем готовность
ENDIF
                @MOIN_1
                jmp     MO_End

; *** проверка запроса повтора пакета ************************************
Pack_CC:
IF _MSP EQ TRUE
                cmp     MO_Len2[BX], 3          ; длина запроса повтора = 3 ?
ELSE
                cmp     MO_Len2, 3              ; длина запроса повтора = 3 ?
ENDIF
                jne     Pack_Err                ; НЕТ - ошибка
                mov     al, ES:[si + 2]
                movzx   dx, al
                add     dx, 0cfh                ; dl - контр. сумма запроса
                CMP     DL, ES:[si + 3]         ; ah - контр. сумма из запроса
                jne     Pack_Err                ; НЕТ - ошибка
IF _MSP EQ TRUE
                cmp     al, BYTE PTR MO_Num[BX]              ; запрос повтора инф-го пакета ?
ELSE
                cmp     al, BYTE PTR MO_Num     ; запрос повтора инф-го пакета ?
ENDIF
                je      Pack_Last               ; ДА - на повтор предыдущего
IF _MSP EQ TRUE
                inc     mo_err[BX]
                cmp     MO_ERR[BX],3
ELSE
                inc     mo_err
                cmp     MO_ERR, 3
ENDIF
                JAE     PEREGRUS_MO

; *** пеpеспpос пакета ***************************************************
IF _MSP EQ TRUE
Pack_Ask:       mov     al, BYTE PTR MO_Num[BX] ; номеp непонятого пакета
                add     al, BYTE PTR MO_NumInc[BX] ; учесть ветку уст-я связи
                MOV     ES,SS:SEG_ST
                MOV     DI,MO_SMESH[BX]
if 1 ;_SORM_MONO NE TRUE 
                LEA     BP,ES:[DI+MO_Out + 0f0h]
else
                LEA     BP,ES:[DI+MO_Out]
endif
ELSE
Pack_Ask:       mov     al, BYTE PTR MO_Num              ; номеp непонятого пакета
                add     al, BYTE PTR MO_NumInc           ; учесть ветку уст-я связи
                MOV     ES,SS:SEG_GS
if 0;1;;;ck_0 MVT
                lea     bp, es:[MO_Out + 0f0h]
else
                lea     bp, es:[MO_Out]
endif
ENDIF
                mov     BYTE PTR ES:[BP], 3
                mov     BYTE PTR ES:[BP + 1], 0cch
                mov     ES:[BP + 2], al
                add     al, 0cfh
                mov     ES:[BP + 3], al         ; сформировали пакет CC
IF _MSP EQ TRUE
if 1 ;_SORM_MONO NE TRUE 
                mov     BYTE PTR ES:[DI+MO_LEN],0f0h  ; отправили запрос повтора
else
                mov     BYTE PTR ES:[DI+MO_LEN],4  ; отправили запрос повтора
endif
                mov     ax, TIME_2MC
                mov     MO_Time[BX], ax         ; зафисировали время и
ELSE
if 0;1;;;ck_0 MVT
                mov     BYTE PTR es:[MO_LEN], 0f0h  ; отправили запрос повтора
else
                mov     BYTE PTR es:[MO_LEN], 4     ; отправили запрос повтора
endif
                mov     eax, ss:Curr_T
                mov     MO_Time, eax
ENDIF
                jmp     MO_End

; *** подготовка пакета к передаче ****************************************
Pack_Make:
IF _MSP EQ TRUE
		MOV	ES,SS:SEG_ST
                mov     MO_Err[BX], 0           ; обнулить счетчик ош. приема
                mov     MO_Ptr[BX], 2           ; указатель на команду min дл-ы
                MOV     DI,MO_SM_BUF[BX]
                mov     MO_BufOut[DI+1], 0ddh
                mov     BP, 2                   ; bx - указатель в буф.передачи
        	mov     dx, 0ddh                ; dx - контрольная сумма
                MOV     SI,MO_SM_ACC[BX]        ; si - ук-ль на нач. нак-ля
                ADD     ACC_PTR[BX],SI
Cmnd_Nxt:
                cmp     si, Acc_Ptr[BX]         ; еще команды в накопителе ?
ELSE
		MOV	ES,SS:SEG_GS
                mov     MO_Err, 0               ; обнулить счетчик ош. приема
                mov     MO_Ptr, 2               ; указатель на команду min дл-ы
                mov     MO_BufOut[1], 0ddh
                mov     bp, 2                   ; bx - указатель в буф.передачи
        	mov     dx, 0ddh                ; dx - контрольная сумма
                xor     si, si
Cmnd_Nxt:
                cmp     si, Acc_Ptr             ; еще команды в накопителе ?
ENDIF
                jae     Acc_Clear               ; НЕТ - сдвинуть инф. накоп-ля
                movzx   cx, ACC[SI]         	; cx - длина команды
                MOV     AL,16H
                CMP     CX,0
                JE      PEREZAGRUSKA1           ;КОМАНДА НЕПРАВЕЛЬНОЙ ДЛИННЫ
                INC     SI
Cmnd_Len:
                mov     AX, BP
                add     AX, cx
                cmp     AX, 255                 ; в буфере есть место под ком-у?
                JAE     Acc_Shift               ; НЕТ - сдвинуть инф. накоп-ля
IF _MSP EQ TRUE
                test    FG_PROGRAM, _fg_prg_mo      ; если включена отладка исходящего МО
ELSE
                test    FG_PROGRAM, M_OR_6      ; если включена отладка исходящего МО
ENDIF
                jz      Cmnd_Mov
push    bx
                mov     bl, BYTE PTR acc[si]
                cmp     bl, 235                         ; пришла 235-ая ?
                jne     @@no_235                        ; нет
                mov     bl, BYTE PTR acc[si + 4]
        push    si
                mov     si, offset MO_Numb_235
                jmp     @@find
  @@no_235:     push    si
                mov     si, offset MO_Numb
  @@find:       DB      9AH                     ;\
                DW      OFFSET Find_Numb_MO      ;|  выводим только выделенные команды
                DW      224                     ;/
pop     si bx
                test    FG_Curr_MO_Num, _find_cmd
                jnz     Cmnd_Out
                jmp     Cmnd_Mov

Cmnd_Out:
                pushad
                ; --- отладка одной линии - начало
                cmp     OneLn_active, 0                 ; режим одной линии активен ?
                je      MO_Out_Draw                     ; НЕТ - выводим всё подряд
                cmp     OneLn_running, 0                ; наблюдаемая линия в работе ?
                je      MO_Out_End                      ; НЕТ - выходим
                ; --- идёт наблюдение за линией
                mov     al, Acc[si + 1]
                mov     dx, WORD PTR Acc[si + 2]
		cmp	Acc[si], 235
		jne     out_not_235
                mov     al, Acc[si + 5]
                mov     dx, WORD PTR Acc[si + 6]
out_not_235:
                cmp     Acc[si], 117                    ; \
                jb      MO_Out_End                      ;  \ интересуемся только
                cmp     Acc[si], 169                    ;  / СОРМовскими командами
                jbe     out_OneLn_Work                  ; /
out_OneLn_Work: cmp     al, OneLn_CkA
                jne     out_OneLn_B
                cmp     dx, OneLn_WorkA
                je      MO_Out_Draw
out_OneLn_B:    cmp     al, OneLn_CkB
                jne     MO_Out_End
                cmp     dx, OneLn_WorkB
                jne     MO_Out_End
                ; --- отладка одной линии - конец

MO_Out_Draw:    mov     ah, cBLUE * 16 + clMAGENTA
                mov     bp, si
                lea     si, __mo_out_str
                mov     di, __inf_y
                call    print_S
                mov     di, __inf_y
                add     di, 2
                cmp     cx, 39
                jbe     MO_Out_Loop
                mov     cx, 39
MO_Out_Loop:    push    cx
                mov     bl, Acc[bp]
                mov     cx, 2
                call    print_H
                pop     cx
                inc     bp
                add     di, 4
                loop    MO_Out_Loop
                add     __inf_y, 160
                cmp     __inf_y, __INF_STR_COUNT * 160
                jb      MO_Out_Str
                mov     __inf_y, 0
MO_Out_Str:     lea     si, __inf_clear
                mov     di, __inf_y
                call    print_S
MO_Out_End:     popad
;;;<Yu> ----------------------------------------

Cmnd_Mov:
                XOR     AX,AX
                MOV     AL, ACC[SI]             ; ДА - записать команду в буф.
        	inc     si
IF _MSP EQ TRUE
                mov     MO_BufOut[DI+BP], al
ELSE
                mov     MO_BufOut[bp], al
ENDIF
                inc     BP
        	add     dx, ax                  ; накопление контрольной суммы
        	loop    Cmnd_Mov
                jmp     Cmnd_Nxt

; *** сдвиг оставшейся информации накопителя к началу **********************
Acc_Shift:
IF _MSP EQ TRUE
                MOV     DI,MO_SM_ACC[BX]
                mov     cx, Acc_Ptr[BX]
                DEC     SI                      ;УЧЕТ ДЛИННЫ ПАКЕТА
        	sub     cx, si
                mov     Acc_Ptr[BX], cx         ; смещение указ-ля конца нак.
                LEA     SI,ACC[SI]
                LEA     DI,ACC[DI]

                MOV     ES,SS:SEG_AX
                REP     MOVSB
                MOV     ES,SS:SEG_ST

                jmp     SHORT Acc_Out           ; на выдачу пакета
Acc_Clear:
                mov     Acc_Ptr[BX], 0              ; теперь накопитель пуст
Acc_Out:        ADD     DX, BP                  ; конец расчета контр. суммы
                MOV     AX,BP
                MOV     DI,MO_SM_BUF[BX]
                mov     MO_BufOut[DI], AL           ; запись длины пакета в буфер
                mov     MO_BufOut[DI+BP], dl       ; запись контр.суммы в буфер
                inc     MO_Num[BX]              ; увеличить N отправ. пакета

; *** повтор последнего пакета *********************************************
Pack_Last:
                MOV     DI,MO_SM_BUF[BX]
                movzx   cx, MO_BufOut[DI]       ; cx - длина последнего пакета
if 1 ;_SORM_MONO NE TRUE 
        	mov     dx, cx
        	not     dl                      ; dl = 255 - cl
        	inc     cx                      ; учесть сам байт длины
                MOV     DI,MO_SMESH[BX]
                LEA     di,ES:[DI+MO_OUT]       ; di - на ОЗУ передачи мод-а
        	cmp     dl, 0f0h                ; длина пакета < 16 ?
        	jbe     SHORT Len_OK            ; НЕТ
        	mov     dl, 0f0h                ; ДА - увеличить длину до 16
Len_OK: 	add     di, dx                  ; di - адрес в ОЗУ передатчика
else
        	inc     cx                      ; учесть сам байт длины
        	mov     dx, cx
                MOV     DI,MO_SMESH[BX]
                LEA     di,ES:[DI+MO_OUT]       ; di - на ОЗУ передачи мод-а
endif
                MOV     SI,MO_SM_BUF[BX]
                lea     si, MO_BufOut[SI]
        	rep     movsb                   ; переписать пакет в передат-к
                MOV     DI,MO_SMESH[BX]
                mov     ES:[DI+MO_LEN], dl         ; послать его
                mov     ax, TIME_2MC
                mov     MO_Time[BX], ax            ; и зафиксировать время
ELSE
                mov     cx, Acc_Ptr
                dec     si                      ;УЧЕТ ДЛИННЫ ПАКЕТА
        	sub     cx, si
                mov     Acc_Ptr, cx             ; смещение указ-ля конца нак.
                lea     si, Acc[si]
                lea     di, Acc

                MOV     ES,SS:SEG_AX
                REP     MOVSB
                MOV     ES,SS:SEG_GS

                jmp     SHORT Acc_Out           ; на выдачу пакета
Acc_Clear:
                mov     Acc_Ptr, 0              ; теперь накопитель пуст
Acc_Out:        add     dx, bp                  ; конец расчета контр. суммы
                mov     ax, bp
                mov     MO_BufOut, al           ; запись длины пакета в буфер
                mov     MO_BufOut[bp], dl       ; запись контр.суммы в буфер
                inc     MO_Num                  ; увеличить N отправ. пакета

; *** повтор последнего пакета *********************************************
Pack_Last:
                movzx   cx, MO_BufOut           ; cx - длина последнего пакета
		inc	cx
        	mov     dx, cx   		; MVT поправил работу с регистром dl
;                not     dl                      ; dl = 255 - cl
                mov     di, MO_OUT              ; di - на ОЗУ передачи мод-а
if 0;1;;;ck_0 MVT    
                cmp     dl, 0f0h                ; длина пакета < 16 ?
                jbe     SHORT Len_OK            ; НЕТ
                mov     dl, 0f0h                ; ДА - увеличить длину до 16
Len_OK:         add     di, dx                  ; di - адрес в ОЗУ передатчика
endif
                lea     si, MO_BufOut
        	rep     movsb                   ; переписать пакет в передат-к
                mov     es:[MO_LEN], dl         ; послать его
                mov     eax, ss:Curr_T
                mov     MO_Time, eax
ENDIF
                jmp     MO_End


                ; === начало и завершение записи команды МО =============================
VIBOR_MOD       PROC
                ; --- проверка наличия МО
                MOV     DL,3            ;ЛИНИЯ НЕДОСТУПНА
                CMP     MO_IDX,0        ; наличие МО с этим модемом
                JNE     END_NO_MOD
                ; --- проверка накопителя
                CMP     ACC_PTR,2000    ; в накопителе должно быть не более 2000 байт
                JAE     END_NO_MOD      ; если в накопителе места нет, то ничего не делаем
                MOV     AX,ACC_PTR      ; маслом кашу не испортишь !
                MOV     ES,SS:SEG_AX    ;
                LEA     DI,ACC          ; начало накопителя найденного модема
                ADD     DI,AX           ; и начало свободного места в этом накопителе
                MOV     NACH_ADR_MO,DI  ;
                INC     DI              ;
                MOV     DL,0            ;
END_NO_MOD:     RET
VIBOR_MOD       ENDP


ZAPIS_OK_MOO    PROC
                PUSH    SI
                MOV     AX,DI           ;
                SUB     AX,NACH_ADR_MO  ;
                DEC     AX              ;
                JZ      SHORT END_M_OK_MOO;
                MOV     SI,NACH_ADR_MO  ;
                MOV     DS:[SI],AL      ;
                INC     AX              ;
                ADD     ACC_PTR,AX      ;
END_M_OK_MOO:   POP     SI
                RET
ZAPIS_OK_MOO    ENDP
                ; === начало и завершение записи команды МО =============================

; ----=> MO_Send <=-----------------------
; Created by Smaller on 11/13/04 15:45
; производит отсылку данных по межпроцессорному обмену
;Входные данные: ds:[si] - указатель на начало отсылаемой команды
;                cx - длина передаваемой команды
;На выходе: CR  - все ОК
;           CR  - не удалось отправить
MO_Send PROC
        push    es
        pusha
        xor     bx, bx
        push	cx
        @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
        pop	cx
        OR      DL,DL                   ; есть возможность послать?
        JNZ     @@exit_Error            ; нет -> пропустим
   rep  movsb
        ; тело !!! пока не задействовано
        @ZAPIS_OK_MO                    ; пошлем
        popa
        pop     es
        stc
        ret
@@exit_Error:
        popa
        pop     es
        clc
        ret
MO_Send ENDP

; ----=> MO_Send_far <=-----------------------
; Created by Smaller on 11/13/04 15:46
; дальний вызов отсылки по МО
MO_Send_far     PROC    FAR
        call    MO_Send
        retf
MO_Send_far     ENDP

; ----=> MO_Send_195 <=-----------------------
; Created by Smaller on 11/13/04 17:01
; отсылка комманды о разрыве управления по пуме
MO_Send_195      PROC
        push    es ds
        pushad
        cld
        mov     ds, ss:seg_ax
        xor     bx, bx
        @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
        OR      DL,DL                   ; есть возможность послать?
        JNZ     @@exit_Error            ; нет -> пропустим
        mov     al, 195
        stosb
        ;@_stosb
        mov     ax, 0303h
        stosb
        ;@_stosw
        ; тело !!! пока не задействовано
        @ZAPIS_OK_MO                    ; пошлем
@@exit_Error:
        popad
        pop     ds es
        ret
MO_Send_195     ENDP


; --- <Yu> [16.07.2010] - контроль СОРМовской коммутации
Check_SORM      PROC
                test    FG_PROGRAM, _fg_prg_check_sorm
                jz      @@quit
                ; ---
                mov     fs, ss:SEG_SORM
                mov     cx, _SESSION_MAX
                xor     si, si
@@loop:         test    fs:[si].sorm_flags, ssf_REL + ssf_CMND_8
                jnz     @@next
                ; действующая сессия - проверим время
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, CURRENT_TIME
ENDIF
                sub     eax, fs:[si].sorm_time
                cmp     eax, 500 * 5
                jb      @@next
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, CURRENT_TIME
ENDIF
                mov     fs:[si].sorm_time, eax
                ; отправка пинга в МО
                @POLUCH_ADR_MO
                mov     PEREM_DL, dl
                cmp     dl, 0
                jne     @@log
                mov     eax, 060404ebh           ; 235.04.04.длина 235/100-й команды
                stosd
                mov     al, 100                  ; тип 235-ой
                stosb
                mov     al, fs:[si].sorm_ck_a
                stosb
                mov     ax, fs:[si].sorm_work_a
                stosw
                mov     al, fs:[si].sorm_chnl_a
		cmp	al, 0ffh
		je	@@quit
                mov     ah, al
                inc     ah
                stosw
                @ZAPIS_OK_MO
                ; логирование пинга в ПУМу
@@log:          test    FG_PROGRAM, _fg_prg_log_sorm
                jz      @@quit
                call    PUMA_Start_Tar
                mov     al, 08fh
                @_STOSB
                mov     al, 0fdh
                @_STOSB
                mov     al, 10 + 1 + 1           ; длина 235/100 + номер модема + признак отправки в МО
                @_STOSB
                mov     eax, 060404ebh           ; 235.04.04.длина 100-й команды
                @_STOSD
                mov     al, 100                  ; тип 235-ой
                @_STOSB
                mov     al, fs:[si].sorm_ck_a
                @_STOSB
                mov     ax, fs:[si].sorm_work_a
                @_STOSW
                mov     al, fs:[si].sorm_chnl_a
                mov     ah, al
                inc     ah
                @_STOSW
                xor     al, al
                @_STOSB
                mov     al, PEREM_DL
                @_STOSB
                call    PUMA_Send_Tar
@@next:         add     si, _SORM_SIZE
                dec     cx
                jnz     @@loop
@@quit:         ret
Check_SORM      ENDP
; --- <Yu> [16.07.2010] - контроль СОРМовской коммутации

; --- <Yu> [08.05.2010] - защита от двойной переадресации
; --- отмечает в глобальном буфере что была переадресация на № ХХХ (по 235 / 18)
; вход:         fs:edi - указатель на соединение
; выход:        нет
;
Set_RA          PROC
                mov     cx, _DBL_RA_MAX
                xor     si, si
@@find_free:    cmp     Dbl_RA_Buff[si], 0ffffffffh
                je      @@free_found
                add     si, 4
                loop    @@find_free
                ret
@@free_found:   mov     eax, DWORD PTR fs:[edi].num_c
                mov     Dbl_RA_Buff[si], eax
                mov     eax, DWORD PTR fs:[edi + 4].num_c
                mov     Dbl_RA_Buff_2[si], eax
		mov	eax, 0ffffffffh
                mov     Dbl_RA_Buff_A1[si], eax
                mov     Dbl_RA_Buff_B1[si], eax
		cmp	fs:[edi].soed_flags, sf_IP_A
		jz	@@exit
                mov     eax, DWORD PTR fs:[edi].num_a
                mov     Dbl_RA_Buff_A1[si], eax
                mov     eax, DWORD PTR fs:[edi + 4].num_a
                mov     Dbl_RA_Buff_A2[si], eax
                mov     eax, DWORD PTR fs:[edi].attr_b
                mov     Dbl_RA_Buff_B1[si], eax
                mov     eax, DWORD PTR fs:[edi + 4].attr_b
                mov     Dbl_RA_Buff_B2[si], eax
                mov     eax, DWORD PTR fs:[edi].dvo_code
                mov     Dbl_RA_Buff_DVO[si], eax
@@exit:         ret
Set_RA          ENDP

; --- удаляет в глобальном буфере отметку о переадресации на № ХХХ (по Disconnect)
; вход:         fs:edi - указатель на соединение
; выход:        нет
;
Reset_RA        PROC
                cmp     fs:[edi].len_c, 0
                je      @@exit
                mov     eax, DWORD PTR fs:[edi].num_c
                mov     cx, _DBL_RA_MAX
                xor     si, si
@@find_num_c:   cmp     Dbl_RA_Buff[si], eax
                je      @@num_c_found
                add     si, 4
                loop    @@find_num_c
                ret
@@num_c_found:  mov     Dbl_RA_Buff[si],    0ffffffffh
                mov     Dbl_RA_Buff_B1[si], 0ffffffffh
                mov     Dbl_RA_Buff_A1[si], 0ffffffffh
@@exit:         ret
Reset_RA        ENDP

; --- проверяет по глобальному буферу не было ли ранее п/а на переадресующего (по 235 / 18)
; вход:         fs:edi - указатель на соединение
; выход:        CF     -  не было,  - была
;
Is_Dbl_RA       PROC
                ; --- получаем и преобразуем последние 7 цифр № переадресующего
                mov     cx, 7
                xor     edx, edx
                mov     si, bp
                add     si, 12
@@get_num:      lodsb
                or      dl, al
                ror     edx, 4
                loop    @@get_num
                or      dl, 0fh
                ror     edx, 4
                ; --- полученный номер обрезаем до LENGTH_NUM
                cmp     LENGTH_NUM, 7
                je      @@begin_find
		shr	edx, 4
                or      edx, 0ff000000h
                cmp     LENGTH_NUM, 6
                je      @@begin_find
		shr	edx, 4
                or      edx, 0fff00000h
                ; --- начинаем поиск
@@begin_find:   mov     cx, _DBL_RA_MAX
                lea     si, Dbl_RA_Buff
@@find_dbl_ra:  cmp     DWORD PTR [si], edx
                je      @@dbl_ra_found
                add     si, 4
                loop    @@find_dbl_ra
@@dbl_ra_not_found:
                clc
                ret
@@dbl_ra_found: @POLUCH_ADR_MO
                or      dl, dl                  ; проверим есть место ?
                jnz     @@exit                  ; нет - уйдем
                ; есть место - все же запишем
                mov     eax, 0e0404ebh              ; Команда 235 (4.4.14) где 14 длина команды
                stosd
                mov     al, 19                      ; тип команды - 19
                stosb
                mov     al, BYTE PTR ds:[bp + 5]    ; номер тысячника
                stosb
                mov     ax, WORD PTR ds:[bp + 6]    ; абсолютный рабочий номер
                stosw
                mov     eax, DWORD PTR ds:[bp + 9]  ; 1-ые 4 цифры номера переадресующего
                stosd
                mov     eax, DWORD PTR ds:[bp + 13] ; ещё 4 цифры номера переадресующего
                stosd
                mov     ax, WORD PTR ds:[bp + 17]  ; последние 2 цифры номера переадресующего
                stosw
                @ZAPIS_OK_MO
                stc
@@exit:         ret
Is_Dbl_RA       ENDP

; --- <Yu> [08.05.2010] - защита от двойной переадресации

