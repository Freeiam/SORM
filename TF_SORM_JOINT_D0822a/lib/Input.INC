;
; === автоматическое поле ввода - код ===============================
; --- отображение окна и заполнение поля ввода
; вход;         si - указатель на описатель поля ввода: тип, длина, метка
; выход:        нет
Input_init_win  PROC
                mov     ah, _color_INPUT
                ; настраиваем поле под текущий ввод
Input_init_win_color:
                push    ds
                pop     es
                mov     al, [si]
                mov     Input_Type, al
                mov     al, [si + 1]
                mov     Input_Len, al
                mov     Input_Data, si
                add     Input_Data, 3
                mov     Input_Count, 0       ; пароль всегда вводим по новой
                cmp     al, _PASS
                je      @@draw
                mov     al, [si + 2]
                mov     Input_Count, al
                movzx   cx, al
                jcxz    @@draw
                push    di
                push    si
                lea     di, Input_Buff
                add     si, 3
                cld
                rep     movsb
                pop     si
                pop     di
                ; измеряем длину подписи
@@draw:         xor     al, al
                mov     cx, 100
                lea     di, Input_Footer
                cmp     Input_Type, _RD_ONLY
                jne     @@scan
                lea     di, Output_Footer
@@scan:         repne   scasb

                mov     ch, 100
                sub     ch, cl
                dec     ch
                mov     Input_Foot_Len, ch
                ; измеряем длину надписи
                mov     cx, 100
                mov     di, si
                movzx   bx, [si + 1]
                add     di, bx
                add     di, 3
                repne   scasb
                mov     ch, 100
                sub     ch, cl
                dec     ch
                mov     Input_Label_Len, ch
                ; фиксируем длину данных
                mov     ch, [si + 1]
                mov     Input_Data_Len, ch
                ; выбираем наидлинейшего
                movzx   dx, Input_Foot_Len
                cmp     dl, Input_Data_Len
                jae     @@cmp_2
                mov     dl, Input_Data_Len
@@cmp_2:        cmp     dl, Input_Label_Len
                jae     @@set_win_size
                mov     dl, Input_Label_Len
                ; установим размер окна
@@set_win_size: add     dl, 4
                ; вычисляем начало окна
                mov     di, 80
                sub     di, dx
                shr     di, 1
                shl     di, 1
                add     di, _pos_Y
                ; --- рисуем верхнююю рамку с заголовком
                ; левый верхний угол
                push    di
                mov     al, Input_LU
                mov     cx, 1
                call    printCdi
                ; полоска до надписи
                movzx   cx, dl
                sub     cl, 2
                sub     cl, Input_Label_Len
                shr     cx, 1
                push    cx
                mov     al, Input_H
                call    printCdi
                ; надпись
                push    si
                movzx   cx, [si + 1]
                add     cx, 3
                add     si, cx
                call    printSdi
                pop     si
                ; полоска после надписи
                movzx   cx, dl
                sub     cl, 2
                sub     cl, Input_Label_Len
                pop     bx
                sub     cx, bx
                mov     al, Input_H
                call    printCdi
                ; правый верхний угол
                mov     al, Input_RU
                mov     cx, 1
                call    printCdi
                pop     di
                ; --- рисуем среднюю строчку с полем ввода
                add     di, 160
                push    di
                ; левая граница
                mov     al, Input_V
                mov     cx, 1
                call    printCdi
                ; пробелы до поля ввода
                movzx   cx, dl
                sub     cl, 2
                sub     cl, Input_Data_Len
                shr     cx, 1
                push    cx
                mov     al, ' '
                call    printCdi
                ; поле ввода
                mov     Input_Ptr, di
                call    Input_input
                ; пробелы после поля ввода
                movzx   cx, dl
                sub     cl, 2
                sub     cl, Input_Data_Len
                pop     bx
                sub     cx, bx
                mov     al, ' '
                call    printCdi
                ; правая граница
                mov     al, Input_V
                mov     cx, 1
                call    printCdi
                pop     di
                ; --- рисуем нижнюю строчку с подписью
                add     di, 160
                ; левый нижний угол
                mov     al, Input_LD
                mov     cx, 1
                call    printCdi
                ; полоска до подписи
                movzx   cx, dl
                sub     cl, 2
                sub     cl, Input_Foot_Len
                shr     cx, 1
                push    cx
                mov     al, Input_H
                call    printCdi
                ; подпись
                push    si
                lea     si, Input_Footer
                cmp     Input_Type, _RD_ONLY
                jne     @@print
                lea     si, Output_Footer
@@print:        call    printSdi
                pop     si
                ; полоска после подписи
                movzx   cx, dl
                sub     cl, 2
                sub     cl, Input_Foot_Len
                pop     bx
                sub     cx, bx
                mov     al, Input_H
                call    printCdi
                ; правый нижний угол
                mov     al, Input_RD
                mov     cx, 1
                call    printCdi
                ; ---
                ret
Input_init_win  ENDP

; --- заполнение поле ввода данными из памяти (результатами предыдущего ввода)
; вход:         ah - цвет ввода
Input_input     PROC
                ; формируем пустое поле ввода
                mov     al, '░'
                movzx   cx, Input_Data_Len
                mov     di, Input_Ptr
                call    printCdi
                mov     ebp, edi
                ; определяем тип вводимой информации
                cmp     Input_Count, 0
                je      @@exit
                cmp     Input_Type, _PASS
                jne     @@not_pass
                ; пароль - не показываем
@@pass:         mov     al, Input_PS            ; '*'
                movzx   cx, Input_Count
                mov     di, Input_Ptr
                call    printCdi
                ret
@@not_pass:     ; во всех остальных случаях заполняем поле результатами предыдущего ввода
                movzx   cx, Input_Count
                mov     di, Input_Ptr
                lea     si, Input_Buff
                call    printHex
;                add     di, cx
;                add     di, cx
                mov     edi, ebp
@@exit:         ret
Input_input     ENDP

; --- редактирование поля ввода
Input_kla       PROC
                cmp     Input_Type, _RD_ONLY
                je      @@exit
                ; ---
                cmp     ah, 0aH         ; [ 0..9 или BACKSPACE ]
                jb      @@Digits        ; -> [ 0..9 ]
                je      @@BackSpace     ; -> [ BACKSPACE ]
                cmp     ah, 13H         ; [ ENTER ]
                je      @@Enter
                cmp     ah, 41h
                jb      @@exit
                cmp     ah, 46h
                ja      @@exit
                ; --- буковки
@@Letters:      sub     ah, 55
                ; --- цифирьки
@@Digits:       mov     al, Input_Count
                cmp     al, Input_Len
                jae     @@exit
                movzx   bx, Input_Count
                mov     Input_Buff[bx], ah
                inc     Input_Count
                jmp     @@redraw
                ; --- забой
@@BackSpace:    cmp     Input_Count, 0
                je      @@exit
                dec     Input_Count
@@redraw:       mov     ah, _color_INPUT
                call    Input_input
@@exit:         clc
                ret
                ; --- ввод
@@Enter:        push    ds
                pop     es
                movzx   cx, Input_Count
                mov     di, Input_Data
                mov     [di - 1], cl
                jcxz    @@no_data
                lea     si, Input_Buff
                cld
                rep     movsb
@@no_data:      stc
                ret
Input_kla       ENDP
