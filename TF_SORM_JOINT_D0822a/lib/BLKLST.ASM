;********************* Поиск телефона в черном спике ************
; Функция должна искать в доп. инфе номер телефона ebx - 4 байта
; если нашла - возвр-ет ffff иначе номер и правило
;
Find_Black_Tel  PROC

                push    eax
                push    ecx
                push    edx
                push    esi
                push    edi
                push    ds
                mov     ds, ss:SEG_BLK_LST      ; сегмент черного списка
                ; ---
                mov     esi, 0ffffh             ; середина (если * 4)
                mov     cx, 16                  ; максимальное количество итераций поиска
_find_blck_t_loop:
                xor     eax, eax                ; 0
                mov     edi, [esi * 4]          ; Save to result
                mov     edx, edi
                and     edi, 0fffffff0h         ; телефон без группы
                cmp     ebx, edi                ;
                je      _find_t_it              ; равны => нашли, выходим
                jb      _find_blck_t_b          ; меньше => на уменьшение esi
                mov     eax, 10b                ; увеличить ESI xor 1100..00b
_find_blck_t_b: or      al, 01b                 ; уменьшить ESI xor 0100..00b
                dec     cx                      ; превращаем № итерации в степень двойки
                shl     eax, cl                 ; задаём изменяемые разряды esi
                xor     esi, eax                ; следующее деление отрезка пополам
                inc     cx                      ; восстановим № итерации
                loop    _find_blck_t_loop

                ; Проверим последний раз
                mov     edi, [esi * 4]          ; Save to result
                mov     edx, edi
                and     edi, 0fffffff0h         ; телефон без группы
                cmp     ebx, edi                ;
                je      _find_t_it              ; равны => нашли, выходим

_Notfind_t_it:  pop     ds
                mov     ebx, 0ffffffffh         ; не нашел
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     eax
                clc
                ret

_find_t_it:     ; проверка на место для групп
                cmp     esi, 080000h-16*16*4-4
                ja      _Notfind_t_it
                ; все как обычно
                pop     ds
                mov     ebx, edx                ; вернём номер целиком (с правилом)
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     eax
                stc
                ret
Find_Black_Tel  ENDP


Find_Black_Tel_Scr      PROC
                call    Find_Black_Tel
                retf
Find_Black_Tel_Scr      ENDP


;*****************************************************************************
; Поиск вхождения № телефона в "белый" список
; Сравнение по наименьшему из количества набранных цифр и длины № в белом списке
; bl - номер правила, fs:edi - указатель на SOED
;*****************************************************************************
Find_Tel_In_Rules       PROC
                push    esi
                push    edx
                push    ecx
                push    eax
                push    ds
                mov     ds, ss:SEG_BLK_LST      ; сегмент черного списка
                ; указатель на правило 0
                mov     esi, 80000h             ; Начнем с конца чёрного списка (512kb)
                sub     esi, 4 + 16 * 16 * 4    ; минус этикетка минус 16 правил по 16 DD
                and     ebx, 0fh                ; на всякий случай
                shl     ebx, 6                  ; умножаем на размер правила (16 * 4)
                add     esi, ebx                ; теперь esi указывает на правило
                ; в правиле не более 16 номеров
                mov     ecx, 16
                ; приводим № из формата SOED к формату чёрного списка
                mov     ebx, DWORD PTR fs:[edi].num_b
                xchg    bh, bl                  ; приводим номер из старого формата SOED
                rol     bl, 4                   ; из формата СОРМ приводим к старому формату SOED
                rol     bh, 4
                ror     ebx, 16                 ; к формату чёрного списка:
                xchg    bh, bl                  ; 1-ая в ст. тетраде ст. байта
                rol     bl, 4
                rol     bh, 4
                ; --- собственно поиск
@@find:         lods    DWORD PTR ds:[esi]      ; грузим очередной № из правила
                test    eax, 80000000h          ; длина № >= 8 цифр ?
                jnz     @@not_found; ДА - кончилось правило
                ; хитрая операция сравнения номеров
                push    ecx
                push    ebx
                mov     edx, eax
                ; полностью синхронизируем № из SOED и № из чёрного списка
                shl     eax, 4                  ; убиваем длину черного № в ст. тетраде
                or      eax, 0fh                ; а мл. тетраду заполняем единицами
                ;
                shr     edx, 28                 ; получаем в dl длину очередного № из правила
                mov     cl, 8                   ; max длина № (8 цифр в DD)
                sub     cl, dl                  ; столько цифр справа надо откинуть
                and     cl, 07h                 ; исключительно на всякий случай
                shl     cl, 2                   ; по 4 бита на цифру (*4)
                shr     eax, cl                 ; откидываем лишние цифры из № правила
                shr     ebx, cl                 ; откидываем лишние цифры из набранного №
                xor     eax, ebx                ; собственно сравнение
                pop     ebx
                pop     ecx
                jz      @@found                 ; нашел
                loop    @@find                  ; ищем дальше
                ; ---
@@not_found:    pop     ds
                pop     eax
                pop     ecx
                pop     edx
                pop     esi
                clc
                ret
                ; ---
@@found:        pop     ds
                pop     eax
                pop     ecx
                pop     edx
                pop     esi
                stc
                ret
Find_Tel_In_Rules       ENDP

;*****************************************************************************
; Поиск вхождения № телефона в "белый" список
; Сравнение по наименьшему из количества набранных цифр и длины № в белом списке
; вход: ds:[bp+1] - указатель на №B в 235/171
;*****************************************************************************
Find_Tel_In_White_NEW   PROC
                        push    esi
                        push    edx
                        push    ecx
                        push    eax

                        PUSH    BP
                        MOVZX   CX, DS:[BP + 4]
                        ADD     BP, 5
                        ADD     BP, CX
                        MOV     EAX, DS:[BP + 1]
                        xchg    ah, al                  ; приводим номер из 235/171
                        ror     eax, 16                 ; к формату чёрного списка:
                        xchg    ah, al                  ; 1-ая в ст. тетраде ст. байта

                        push    DS
                        mov     DS, SS:SEG_BLK_LST      ; сегмент черного списка

                        mov     esi, 80000h             ; Начнем с конца чёрного списка (512kb)
                        sub     esi, 4+16*16*4          ; минус этикетка минус 16 правил по 16 DD

                        mov     ebx, eax
                        mov     ecx, 16                 ; в правиле не более 16 номеров

Find_Tel_In_White_Loop_NEW:
                        lods    DWORD PTR DS:[ESI]      ; грузим очередной № из правила

                        test    eax, 80000000h          ; длина № >= 8 цифр ?
                        jnz     Find_Tel_In_White_not_Found_NEW; ДА - кончилось правило

                        ; Хитрая операция сравнения номеров
                        push    ecx
                        push    ebx
                        mov     edx, eax
                        ; --- полностью синхронизируем № из 235/171 и № из чёрного списка
                        shl     eax, 4                  ; убиваем длину черного № в ст. тетраде
                        or      eax, 0fh                ; а мл. тетраду заполняем единицами

                        shr     edx, 28                 ; в dl - длина № из чёрного списка
                        CMP     DL, DS:[BP]
                        jbe     Find_Tel_In_White_Length_NEW
                        MOV     DL, DS:[BP]

Find_Tel_In_White_Length_NEW:
                        mov     cl, 8                   ; max длина № (8 цифр в DD)
                        sub     cl, dl                  ; столько цифр справа надо откинуть
                        and     cl, 07h                 ; исключительно на всякий случай
                        shl     cl, 2                   ; по 4 бита на цифру (*4)

                        shr     eax, cl                 ; откидыавем лишние цифры из набранного №
                        shr     ebx, cl                 ; откидываем лишние цифры из № чёрного списка
                        XOR     eax, ebx                ; собственно сравнение
                        pop     ebx
                        pop     ecx
                        jz      Find_Tel_In_White_Found_NEW     ; нашел

                        loop    Find_Tel_In_White_Loop_NEW

Find_Tel_In_White_not_Found_NEW:
                        pop     DS
                        POP     BP
                        pop     eax
                        pop     ecx
                        pop     edx
                        pop     esi
                        clc
                        ret
Find_Tel_In_White_Found_NEW:
                        pop     DS
                        POP     BP
                        pop     eax
                        pop     ecx
                        pop     edx
                        pop     esi
                        stc
                        ret
Find_Tel_In_White_NEW   ENDP
;*****************************************************************************
; Функция Должна находить входит ли телефон в правила
; bl - номер правила, eax - номер  B c мусором
;*****************************************************************************
Find_Tel_Rules_In_NEW   PROC
                        push    esi
                        push    edx
                        push    ecx
                        push    eax

                        PUSH    BP
                        MOVZX   CX, DS:[BP + 4]
                        ADD     BP, 5
                        ADD     BP, CX
                        MOV     EAX, DS:[BP + 1]        ; № B -  (обратный BCD)
                        xchg    ah, al                  ; приводим номер из 235/171
                        ror     eax, 16                 ; к формату чёрного списка:
                        xchg    ah, al                  ; 1-ая в ст. тетраде ст. байта

                        push    DS
                        mov     DS, SS:SEG_BLK_LST      ; сегмент черного списка

                        mov     esi, 80000h             ; Начнем с конца чёрного списка (512kb)
                        sub     esi, 4+16*16*4          ; минус этикетка минус 16 правил по 16 DD

                        and     ebx, 0fh                ; на всякий случай
                        shl     ebx, 6                  ; умножаем на размер правила (*16*4)
                        add     esi, ebx                ; теперь esi указывает на правило
                        mov     ebx, eax
                        mov     ecx, 16                 ; в правиле не более 16 номеров
Find_Tel_RULEs_Loop_NEW:
                        lods    DWORD PTR DS:[ESI]      ; грузим очередной № из правила

                        test    eax, 80000000h          ; длина № >= 8 цифр ?
                        jnz     Find_Tel_RULEs_IN_NotFNd_NEW; ДА - кончилось правило

                        ; Хитрая операция сравнения номеров
                        push    ecx
                        push    ebx
                        mov     edx, eax
                        ; --- полностью синхронизируем № из 235/171 и № из чёрного списка
                        shl     eax, 4                  ; убиваем длину черного № в ст. тетраде
                        or      eax, 0fh                ; а мл. тетраду заполняем единицами

                        shr     edx, 28                 ; в dl - длина № из чёрного списка

                        mov     cl, 8                   ; max длина № (8 цифр в DD)
                        sub     cl, dl                  ; столько цифр справа надо откинуть
                        and     cl, 07h                 ; исключительно на всякий случай
                        shl     cl, 2                   ; по 4 бита на цифру (*4)

                        shr     eax, cl                 ; откидываем лишние цифры из набранного №
                        shr     ebx, cl                 ; откидываем лишние цифры из № чёрного списка
                        XOR     eax, ebx                ; собственно сравнение
                        pop     ebx
                        pop     ecx
                        jz      Find_Tel_RLs_IN_FNd_NEW     ; нашел

                        dec     ecx
                        jnz    Find_Tel_RULEs_Loop_NEW
;;;                        loop    Find_Tel_RULEs_Loop

Find_Tel_RULEs_IN_NotFNd_NEW:
                        pop     DS
                        POP     BP
                        pop     eax
                        pop     ecx
                        pop     edx
                        pop     esi
                        clc
                        ret
Find_Tel_RLs_IN_FNd_NEW:
                        pop     DS
                        POP     BP
                        pop     eax
                        pop     ecx
                        pop     edx
                        pop     esi
                        stc
                        ret
Find_Tel_Rules_In_NEW   ENDP
;*****************************************************************************
; вход: fs:edi    -  указатель на SOED
;       PEREM_EDI == edi
;       ds:bp     -  указатель на 118-ую команду
;*****************************************************************************
Black_List      PROC
; ---------------- =========== BLACK LIST ============ ----------------
                ; Добавлено Mr_Alex
                ; Возможность включена?

                pushad
@_test_debug_black 1
                test    WORD PTR fs:[edi].soed_flags, sf_ANI
                jz      BL_Exit
@_test_debug_black 2
                test    FG_PROGRAM, _fg_prg_bl_out
                JZ      BL_Exit
@_test_debug_black 3
                ; надо проверить длину
                CMP     fs:[edi].len_a, 7       ; min 7 цифр
                JB      BL_Exit
@_test_debug_black 4
                ; проверяем АОН - вызов пришёл из той же зоны где стоит АТС или другой ?
                mov     ax, WORD PTR fs:[edi].num_a   ; первые 4 цифры АОН"а (например 52.84)
                and     ax, PREF_A_ZN_MASK
                cmp     ax, PREF_A_ZN           ; код зоны для АТС равен коду зоны из АОН ?
                jne     BL_Exit                 ; НЕТ -
@_test_debug_black 5
                ; преобразуем № А из формата АОН в формат чёрного списка
                mov     ebx, DWORD PTR fs:[edi + 1].num_a
                xchg    bh, bl                  ; приводим номер из SOED
                rol     bl, 4
                rol     bh, 4
                ror     ebx, 16                 ; к формату чёрного списка:
                xchg    bh, bl                  ; 1-ая в ст. тетраде ст. байта
                rol     bl, 4
                rol     bh, 4
                mov     PEREM_EBX, ebx          ; запишем для вывода на экран
                shl     ebx, 4                  ; ещё сдвиг, чтоб формат стал как во FLASH !
                ; проверяем есть ли № А в чёрном списке
                call    Find_Black_Tel
                jnc     BL_Exit                 ; абонента нет в чёрном списке
@_test_debug_black 6
                ; Проверим белый список
                push    ebx
                xor     bl, bl
                call    Find_Tel_In_Rules
                pop     ebx
                jc      BL_Exit                 ; № Б в белом списке - выходим без отбоя
@_test_debug_black 7
                ; абонент в нулевом правиле ?
                and     ebx, 0fh                ; в bl - правило
                cmp     bl, 0
                je      BL_Done                 ; 0 - закрывает все
@_test_debug_black 8
                ; проверим другие правила
                call    Find_Tel_In_Rules
                jnc     BL_Exit                 ; не в списке
                ; да этот телефон - запишем это в тысячник
BL_Done:        @_test_debug_black 9
                MOV     ESI, EDI                ; Теперь edi в esi
if 01
                @POLUCH_ADR_MO                  ;
                OR      DL,DL                   ; проверим есть место ?
                JNZ     BL_Exit                 ; нет - уйдем
@_test_debug_black 10
                ; есть место - все же запишем
                mov     eax, 070404ebh          ; Команда 169 - абонент в черном списке
                stosd
                MOV     AL, 169                 ; Команда 169 - абонент в черном списке
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
                mov     ax, fs:[esi].phys_a     ; абсолютный физический номер А
                STOSW
                mov     al, 2
                stosb
                @ZAPIS_OK_MO
endif
                ; --- для совместимости со старой версией ------------------------------------------------------
                @POLUCH_ADR_MO                  ;
                OR      DL,DL                   ; проверим есть место ?
                JNZ     BL_Exit                 ; нет - уйдем
@_test_debug_black 11
                ; есть место - все же запишем
                MOV     AL, 169                 ; Команда 169 - абонент в черном списке
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
                mov     ax, fs:[esi].phys_a     ; абсолютный физический номер А
                STOSW
                @ZAPIS_OK_MO
                ; ---
                or      fs:[esi].soed_flags, sf_BLACK_DENY
                ; --- собственно индикация
                mov     ebx, PEREM_EBX
                mov     BL_CurrNum_Out, ebx     ; запишем для вывода на экран
                inc     BL_Count_Out
BL_Exit:        popad
                ret
Black_List      ENDP
;===========================================================================
;---------------------------------------------------------------------------
;===========================================================================

Black_List_In   PROC
@_test_debug_black_In  1
                test    FG_PROGRAM, _fg_prg_bl_in      ; входящую связь закрываем ?
                jz      BLI_Exit                       ; НЕТ -
@_test_debug_black_In  2
                pushad
                ; --- преобразуем № Б из формата SOED в формат чёрного списка
                mov     al, fs:[edi].len_b
                cmp     al, LENGTH_NUM
                jne     BLI_End
@_test_debug_black_In  3
                ; приводим № Б из формата  SOED  в формат чёрного списка
                mov     ebx, DWORD PTR fs:[edi].num_b
                xchg    bl, bh                  ;
                rol     bl, 4                   ;
                rol     bh, 4
                ror     ebx, 16                 ;
                xchg    bh, bl                  ;
                rol     bl, 4
                rol     bh, 4
                ; в зависимости от длины № Б добиваем старшими цифрами до 7 знаков
                cmp     al, 7
                je      BLI_Num_7
                mov     edx, PREF_A_FULL
                cmp     al, 6
                jne     BLI_Num_5
BLI_Num_6:      shr     ebx, 4
                shl     edx, 16
                and     edx, 0f0000000h
                or      ebx, edx
                jmp     BLI_Num_7
BLI_Num_5:      shr     ebx, 8
                shr     edx, 4
                rol     dh, 4
                shl     edx, 16
                and     edx, 0ff000000h
                or      ebx, edx
BLI_Num_7:      and     ebx, 0fffffff0h         ; убьём мл. тетраду, чтоб формат стал как во FLASH!
                ; --- проверяем есть ли № Б в чёрном списке
                call    Find_Black_Tel
                jnc     BLI_End                 ; №Б НЕ в чёрном списке
@_test_debug_black_In  4
                mov     PEREM_EBX, ebx          ; для индикации
                shr     PEREM_EBX, 4
                and     ebx, 0fh                ; в bl - правило
                cmp     bl, 0
                jne     BLI_End                 ; вход закрыт только для 0 правила
@_test_debug_black_In  5
                ; --- собственно отбой
                mov     esi, edi
if 01
                @POLUCH_ADR_MO                  ;
                OR      DL,DL                   ; проверим есть место ?
                JNZ     BLI_End                 ; нет - уйдем
@_test_debug_black_In  6
                ; есть место - все же запишем
                mov     eax, 070404ebh          ; Команда 169 - абонент в черном списке
                stosd
                mov     al, 169
                stosb
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
                mov     ax, fs:[esi].phys_a     ; абсолютный физический номер А
                STOSW
                mov     al, 1
                stosb
                @ZAPIS_OK_MO
endif
                ; --- для совместимости со старой версией ------------------------------------------------------
                @POLUCH_ADR_MO                  ;
                OR      DL,DL                   ; проверим есть место ?
                JNZ     BLI_End                 ; нет - уйдем
@_test_debug_black_In  7
                ; есть место - все же запишем
                mov     al, 169
                stosb
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
                mov     ax, fs:[esi].phys_a     ; абсолютный физический номер А
                STOSW
                @ZAPIS_OK_MO
                ; ---
                or      fs:[esi].soed_flags, sf_BLACK_DENY
                ; --- собственно индикация
                mov     ebx, PEREM_EBX
                mov     BL_CurrNum_In, ebx      ; запишем для вывода на экран
                inc     BL_Count_In
BLI_End:        popad
BLI_Exit:       ret
                ENDP
;==========================================================================
;--------- "новый" черный список исходящий --------------------------------
;==========================================================================
Black_List_NEW      PROC

                 ;Возможность включена?
                pushad
@_test_debug_black_q 1
                test    FG_PROGRAM, _fg_prg_bl_out     ; если отключено делаем как обычно
                JZ      _BL_Enable
@_test_debug_black_q 2
                ; надо проверить длину
                MOVZX   CX, DS:[BP + 4]
                CMP     CX, 4
                JE      @@no_prefix
                CMP     CX, 5
                JE      @@prefix_ok
                JMP     _BL_Exit

@@no_prefix:
                MOV     EBX, DS:[BP + 5]
                JMP     @@find_black


@@prefix_ok:    @_test_debug_black_q 3
                MOV     BX, DS:[BP + 5]         ; первые 4 цифры АОН"а (например 52.84)
                ; проверяем АОН - вызов пришёл из той же зоны где стоит АТС или другой ?
                mov     eax, PREF_A_FULL        ; возьмем префикс (например 00.84.52.79)
                xchg    al, ah
                rol     al, 4
                rol     ah, 4
                ror     eax, 16
                shl     ax, 8
                shr     eax, 8
                rol     al, 4
                shr     eax, 12                 ; оставим первые три цифры (только код зоны) 08.45
                xchg    bl, bh                  ;
                shr     bx, 4                   ;
                cmp     ax, bx                  ; код зоны для АТС равен коду зоны из АОН ?
                jne     _BL_Enable              ; НЕТ -
               ; преобразуем № А из формата АОН в формат чёрного списка
                MOV     EBX, DS:[BP + 6]        ; № А -  (обратный BCD)
@@find_black:   @_test_debug_black_q 4
                xchg    bh, bl                  ; приводим номер из 235/171
                ror     ebx, 16                 ; к формату чёрного списка:
                xchg    bh, bl                  ; 1-ая в ст. тетраде ст. байта
                mov     PEREM_EBX, ebx          ; запишем для вывода на экран
                shl     ebx, 4                  ; ещё сдвиг, чтоб формат стал как во FLASH !
                ; проверяем есть ли № А в чёрном списке
                call    Find_Black_Tel
                mov     eax, ebx
                cmp     eax, 0ffffffffh
                jnc     _BL_Enable                 ; абонента нет в чёрном списке
@_test_debug_black_q 5
                ; Проверим белый список (bl = 0)
                xor     ebx, ebx                 ; нулевое правило
                call    Find_Tel_In_White_NEW
                jc      _BL_Enable_White         ; № Б в белом списке - выходим на 235_172
@_test_debug_black_q 6
                mov     ebx, eax
                and     ebx, 0fh                 ; в bl - правило для № А
                cmp     bl, 0
                jz      _BL_Disable              ; 0 - закрывает все
@_test_debug_black_q 7
                call    Find_Tel_Rules_In_NEW
                jnc     _BL_Enable               ; нет в списке

                 ; да этот телефон - запишем это в тысячник
_BL_Disable:   @_test_debug_black_q 8
                xor     bx, bx
               @POLUCH_ADR_MO  TOTGE,1         ;
                OR      DL,DL                   ; проверим есть место ?
                JNZ     _BL_Exit                 ; нет - уйдем
@_test_debug_black_q 9
                ; есть место - все же запишем
                mov     eax, 070404ebh          ; Команда 169 - абонент в черном списке
                stosd
                MOV     AL, 169                 ; Команда 169 - абонент в черном списке
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
                mov     AX, 0FFFFH; абсолютный физический номер А
                STOSW
                mov     al, 2
                stosb
               @ZAPIS_OK_MO
                ; --- для совместимости со старой версией ------------------------------------------------------
                xor     bx, bx
               @POLUCH_ADR_MO  TOTGE,1         ;
                OR      DL,DL                   ; проверим есть место ?
                JNZ     _BL_Exit                 ; нет - уйдем
                mov     BL_Return_Code, 0
                ; есть место - все же запишем
                MOV     AL, 169                 ; Команда 169 - абонент в черном списке
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
                mov     AX, 0FFFFH
                STOSW
               @ZAPIS_OK_MO
                ; --- для совместимости со старой версией ------------------------------------------------------
                or      fs:[esi].soed_flags, sf_BLACK_DENY
                ; --- собственно индикация
                mov     ebx, PEREM_EBX
                mov     BL_CurrNum_Out, ebx     ; запишем для вывода на экран
                inc     BL_Count_Out
@_test_debug_black_q 10
_BL_Exit:                ;;;@BL_Log_Return
                mov     BL_Return_Code, 1       ; Входящий черный список не обрабатываем
                popad
                ret
                ; -----------------

_BL_Enable:     mov     BL_Return_Code, 2
                popad
                ret
_BL_Enable_White:
                mov     BL_Return_Code, 1
                xor     bx, bx
               @POLUCH_ADR_MO  TOTGE,1         ;
                mov     eax, 040404ebh          ; Команда 235 - работа с конфигурацией
                stosd
                MOV     AL, 172                 ; Подкоманда 172 - разрешение на соединение
                                                ; от тарификатора к АЛ и СЛ
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
               @ZAPIS_OK_MO
                popad
                ret

Black_List_NEW      ENDP
;==============================================================================
;--------------- "новый" черный список входящий -------------------------------
;==============================================================================
Black_List_In_NEW   PROC
@_test_debug_black_in_q 1
                test    FG_PROGRAM, _fg_prg_bl_in      ; входящую связь закрываем ?
                jz      _BLI_Exit                      ; НЕТ -
@_test_debug_black_in_q 2
                pushad
                ; --- преобразуем № Б из формата SOED в формат чёрного списка
                PUSH     BP
                MOVZX   CX, DS:[BP + 4]
                ADD     BP, 5
                ADD     BP, CX
                MOV     AL, DS:[BP]             ; запомним длину
                MOV     EBX, DS:[BP + 1]
                xchg    bh, bl                  ; приводим номер из 235/171
                ror     ebx, 16                 ; к формату чёрного списка:
                xchg    bh, bl                  ; 1-ая в ст. тетраде ст. байта
                ; ---- узнаем сколько цифр набрано - ? -----------
                CMP     AL, 3
                JE      @BYTE_3
                CMP     AL, 4
                JE      @BYTE_4
                JMP     _BLI_Enable_BP
@BYTE_3:
                TEST    BH, 0FH
                JNZ     @GO_FIVE
                MOV     AL, 6
                JMP     @GO_PROV
@BYTE_4:        TEST    BL, 0FH
                JNZ     @GO_SEVEN
                JMP     _BLI_Enable_BP
@GO_FIVE:       MOV     AL, 5
                JMP     @GO_PROV
@GO_SEVEN:      MOV     AL, 7

@GO_PROV:       @_test_debug_black_in_q 3
                CMP     AL, LENGTH_NUM
                JNE     _BLI_Enable_BP
                POP     BP
                cmp     al, 7
                je      _BLI_Num_And
                mov     edx, PREF_A_FULL
                xchg    dl, dh                  ;\
                rol     dl, 4                   ; \
                rol     dh, 4                   ;  \
                ror     edx, 16                 ;   подготовим переменную для нормальной работы
                shl     dx, 8                   ;  /
                shr     edx, 8                  ; /
                rol     dl, 4                   ;/
                shl     edx, 20
                and     edx, 0ff000000h
                shr     ebx, 8
                cmp     al, 5
                je      _BLI_Num_Or
                and     edx, 0f0000000h
                shl     ebx, 4
_BLI_Num_Or:    or      ebx, edx
_BLI_Num_And:   and     ebx, 0fffffff0h         ; убьём мл. тетраду, чтоб формат стал как во FLASH!
                ;------------------------------------------
@_test_debug_black_in_q 4
                ; --- проверяем есть ли № Б в чёрном списке
                call    Find_Black_Tel
                jnc     _BLI_Enable                 ; №Б НЕ в чёрном списке
@_test_debug_black_in_q 5
                mov     PEREM_EBX, ebx          ; для индикации
                shr     PEREM_EBX, 4
                and     ebx, 0fh                ; в bl - правило
                cmp     bl, 0
                jne     _BLI_Enable                 ; вход закрыт только для 0 правила
@_test_debug_black_in_q 6
                ; --- для индикации
                ; --- собственно отбой
                xor     bx, bx
               @POLUCH_ADR_MO  TOTGE,1         ;
                OR      DL,DL                   ; проверим есть место ?
                JNZ     _BLI_End                 ; нет - уйдем
@_test_debug_black_in_q 7
                ; есть место - все же запишем
                mov     eax, 070404ebh          ; Команда 169 - абонент в черном списке
                stosd
                mov     al, 169
                stosb
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
                mov     AX, 0FFFFH              ; абсолютный физический номер А
                STOSW
                mov     al, 1
                stosb
               @ZAPIS_OK_MO
                ; --- для совместимости со старой версией ------------------------------------------------------
                xor     bx, bx
               @POLUCH_ADR_MO  TOTGE,1         ;
                OR      DL,DL                   ; проверим есть место ?
                JNZ     _BLI_End                 ; нет - уйдем
@_test_debug_black_in_q 8
                ; есть место - все же запишем
                mov     al, 169
                stosb
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
                mov     AX, 0FFFFH              ; абсолютный физический номер А
                STOSW
               @ZAPIS_OK_MO
                ; --- для совместимости со старой версией ------------------------------------------------------
                or      fs:[esi].soed_flags, sf_BLACK_DENY
                ; --- собственно индикация
                mov     ebx, PEREM_EBX
                mov     BL_CurrNum_In, ebx      ; запишем для вывода на экран
                inc     BL_Count_In
_BLI_End:        ;;;@BL_Log_Return
                popad
_BLI_Exit:      ret

_BLI_Enable_BP: POP     BP
                ; да этот телефон - запишем это в тысячник
_BLI_Enable:    xor     bx, bx
               @POLUCH_ADR_MO  TOTGE,1         ;
                OR      DL,DL                   ; проверим есть место ?
                JNZ     _BLI_End                  ; нет - уйдем
                ; есть место - все же запишем
                mov     eax, 040404ebh          ; Команда 235 - работа с конфигурацией
                stosd
                MOV     AL, 172                 ; Подкоманда 172 - разрешение на соединение
                                                ; от тарификатора к АЛ и СЛ
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
               @ZAPIS_OK_MO
                popad
                ret


Black_List_In_NEW   ENDP
;==========================================================================


