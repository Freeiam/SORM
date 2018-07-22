;***********************************************************************
; 29/09/2004    (C)Odinokov Alex
;************************НАБОР ДРАЙВЕРОВ ДЛЯ ФЛЕШ***********************
; содержит библиотеку для видов флеш:
; 1) ELAN400 - Драйвер Elan410 flash 29DL32(LV16)
; 2) INT1024 - Драйвер Interface1024 flash 29DL32
; 3) INT1000 - Драйвер Interface1000 flash ????
; Каждый драйвер включает в себя следующий набор функций:
; 1) READ_FLC - чтение в буфер
; 2) WRITE_FLC - Запись с автозатиранием очередного сектора
; 3) WRTNE_FLC - Запись без авто затирания (нельзя выходить за пределы 1 сектора)
; 4) ERASE_FLC - Затирание сектора
; 5) ERSCM_FLC - команда на затирание
; 6) ERSCHK_FLC - проверка на выполнение команды на затирание
;***********************************************************************

; <<<<<<<<<<<<<<<<<<<<<<<<< Драйвер Elan410 flash 29DL32(LV16) >>>>>>>>>>>>>>>>>>>>>>>>>>>>
;***********************************************************************
; Чтение флеш Elan'a; esi - адрес от начала флеш
; ecx - количество слов к чтению; es:edi - адрес dst
;***********************************************************************
ELAN400_READ_FLC        PROC
                        cld
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
                        movsw
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
                        cld
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

                        pop     edi
                        pop     eax
                        pop     es                          ; Error exit
                        stc
                        ret
            _Saved_OK:
                        pop     edi
            _Nt_Save:   ; не надо записывать
                        add     edi, 2                      ; сдвинем

                        DB      67h
                        loop    ELAN400_WRITE_FLC_LOOP      ; Следующий байт
                        ; Нормальный выход
ELAN400_WRITE_EX:
                        pop     eax
                        pop     es
                        clc
                        ret
;***********************************************************************
; ФУНКЦИЯ записи без стирания (не дальше сектора)
;***********************************************************************
ELAN400_WRTNE_FLC:
                        cld
                        push    es
                        push    eax
                        mov     es, _FLASH1_SEG     ; Сегмент флеш
                        jmp     _Nt_Erse
;***********************************************************************
;подфункция стирания сектора (64кб) (может работать как фунцкия)
;edi- адрес от начала флеш
;***********************************************************************
ELAN400_ERASE_FLC:
                        call    ELAN400_ERSCM_FLC       ; Команда
    ELAN400_ERASE_FLC_L:
                        call    ELAN400_ERSCHK_FLC      ; проверим
                        jc      ELAN400_ERASE_FLC_L
                        ret
;***********************************************************************
;подфункция команды стирания сектора (64кб)
;edi- адрес от начала флеш
;***********************************************************************
ELAN400_ERSCM_FLC:
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
                        pop     es
                        pop     eax
                        ret
;***********************************************************************
; подфункция проверки стирания сектора
;***********************************************************************
ELAN400_ERSCHK_FLC:
                        push    eax
                        push    es
                        mov     es, _FLASH1_SEG      ; Сегмент флеш

                        MOV     AL,ES:[0]
                        TEST    AL,80h
                        JNZ     CLR_SE1
                        TEST    AL,20h
                        JZ      ELAN400_ERSCHK_FLC_NGOOD
                        MOV     AL,ES:[0]
                        TEST    AL,80h
                        JNZ     CLR_SE1
                CLR_SE1:
                        MOV     BYTE PTR ES:[0],0F0h
                        pop     es
                        pop     eax
                        clc
                        ret
ELAN400_ERSCHK_FLC_NGOOD:
                        pop     es
                        pop     eax
                        stc
                        ret
ELAN400_WRITE_FLC       ENDP

; <<<<<<<<<<<<<<<<<<<<<< Драйвер Interface1024 flash 29DL32 >>>>>>>>>>>>>>>>>>>>>>>>>>
;***********************************************************************
; Чтение флеш интерфейса; esi - адрес от начала флеш
; ecx - количество слов к чтению; es:edi - адрес dst
;***********************************************************************
INT1024_READ_FLC        PROC
                        cld
                        push    ds
                        push    eax
                        shl     ecx, 1
                        mov     ds, _FLASH2_SEG         ; Сегмент флеш
                        ; выберем окно
                        mov     eax, esi             	;
                        shr     eax, 14                 ; al - номер окна
                        MOV     DS,_FLASH2HELP_SEG
                        MOV     DS:[ADR_RU],AL

                        MOV     DS,_FLASH2_SEG
    INT1024_READ_FLC_MOV:
                        push    esi
                        and     esi, 03fffh             ;
                        DB      67h
                        movsb
                        pop     esi
                        add     esi, 1

                        test    esi, 03fffh             ;
                        jnz     INT1024_READ_FLC_NZ
                        ; изменим окно
                        mov     eax, esi             	;
                        shr     eax, 14                 ; al - номер окна
                        MOV     DS,_FLASH2HELP_SEG
                        MOV     DS:[ADR_RU],AL
                        MOV     DS,_FLASH2_SEG
     INT1024_READ_FLC_NZ:
                        DB      67h
                        loop    INT1024_READ_FLC_MOV

                        pop     eax
			pop	ds
                        ret
INT1024_READ_FLC        ENDP
;***********************************************************************
; Запись на флеш интерфейса; edi- адрес от начала флеш
; ecx - количество слов к записи; ds:esi - адрес src
;***********************************************************************
INT1024_WRITE_FLC       PROC
                        cld
                        push    es
                        push    eax
                        shl     ecx, 1
                        mov     es, _FLASH2_SEG     ; Сегмент флеш
INT1024_WRITE_FLC_LOOP:
                        ; проверим, надо ли стирать
                        test    edi, 0ffffh          ; адрес кратен 64кб
                        jnz     _Nt_Erse1            ; не надо
                        call    INT1024_ERASE_FLC    ; стирать
            _Nt_Erse1:
		        DB    	67h
                        lodsb
                        cmp     al, 0ffh
                        je      _Nt_Save1
                        push    ax
                        ; выберем окно
                        mov     eax, edi             ;
                        shr     eax, 14              ; al - номер окна
                                                     ; для перестановки окон
                        MOV     ES,_FLASH2HELP_SEG
                        MOV     BYTE PTR ES:[ADR_RU],0;AL
                        MOV     ES,_FLASH2_SEG
                        ; команда "program word"
                        MOV     BYTE PTR ES:[0AAAh],0AAh
                        MOV     BYTE PTR ES:[555h],55h
                        MOV     BYTE PTR ES:[0AAAh],0A0h

                        MOV     ES,_FLASH2HELP_SEG
                        MOV     ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG
                        ; можно писать и проверять
                        pop     ax
                        push    edi
                        and     edi, 03fffh
                        MOV     ES:[di], AL
                        ; Проверка записи
            _Test_loop1:
                        mov     dl, ES:[di]
                        cmp     al, dl
                        je      _Saved_OK1                  ; Записалось
                        test    dl, 20h
                        jz      _Test_loop1                 ; Не проходил стадию
                        mov     dl, ES:[di]
                        cmp     al, dl
                        jne     INT1024_WRITE_ErrEX
            _Saved_OK1:
                        pop     edi
            _Nt_Save1:  ; не надо записывать
                        inc     edi                         ; сдвинем

                        DB      67h
                        loop    INT1024_WRITE_FLC_LOOP      ; Следующий байт
                        ; Нормальный выход
INT1024_WRITE_EX:
                        pop     eax
                        pop     es
                        clc
                        ret
INT1024_WRITE_ErrEX:
                        pop     edi
                        pop     eax
                        pop     es                          ; Error exit
                        stc
                        ret
;***********************************************************************
; ФУНКЦИЯ записи без стирания (не дальше сектора)
;***********************************************************************
INT1024_WRTNE_FLC:
                        cld
                        push    es
                        push    eax
                        shl     ecx, 1
                        mov     es, _FLASH2_SEG     ; Сегмент флеш
                        jmp     _Nt_Erse1
;***********************************************************************
;подфункция стирания сектора (64кб) (может работать как фунцкия)
;edi- адрес от начала флеш
;***********************************************************************
INT1024_ERASE_FLC:
                        call    INT1024_ERSCM_FLC       ; Команда
    INT1024_ERASE_FLC_L:
                        call    INT1024_ERSCHK_FLC      ; проверим
                        jc      INT1024_ERASE_FLC_L
                        ret
;***********************************************************************
;подфункция команды стирания сектора (64кб)
;edi- адрес от начала флеш
;***********************************************************************
INT1024_ERSCM_FLC:
                        push    eax
                        push    es
                        mov     es, _FLASH2_SEG      ; Сегмент флеш

                        mov     eax, edi             ;
                        shr     eax, 14              ; aL - номер окна
                                                     ; для перестановки окон
                        MOV     ES,_FLASH2HELP_SEG
                        MOV     BYTE PTR ES:[ADR_RU],0;AL
                        MOV     ES,_FLASH2_SEG

                        ; команда "erase sector"
                        mov     Byte PTR ES:[0AAAh], 0AAh
                        mov     Byte PTR ES:[0555h], 055h
                        mov     Byte PTR ES:[0AAAh], 080h
                        mov     Byte PTR ES:[0AAAh], 0AAh
                        mov     Byte PTR ES:[0555h], 055h

                        MOV     ES,_FLASH2HELP_SEG
                        MOV     ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG

                        MOV     BYTE PTR ES:[0],30h
                        pop     es
                        pop     eax
                        ret
;***********************************************************************
; подфункция проверки стирания сектора
;***********************************************************************
INT1024_ERSCHK_FLC:
                        push    eax
                        push    es
                        mov     es, _FLASH2_SEG      ; Сегмент флеш
                        ; Проверяем конец записи
                        MOV     AL,ES:[0]
                        TEST    AL,80h
                        JNZ     CLR1_SE1
                        TEST    AL,20h
                        JZ      INT1024_ERSCHK_FLC_NGOOD
                        MOV     AL,ES:[0]
                        TEST    AL,80h
                        JNZ     CLR1_SE1
               CLR1_SE1:
                        MOV     BYTE PTR ES:[0],0F0h
                        pop     es
                        pop     eax
                        clc
                        ret
INT1024_ERSCHK_FLC_NGOOD:
                        pop     es
                        pop     eax
                        stc
                        ret
INT1024_WRITE_FLC       ENDP
; <<<<<<<<<<<<<<<<<<<<<< Драйвер Interface1000 flash ???? >>>>>>>>>>>>>>>>>>>>>>>>>>
;***********************************************************************
; Чтение флеш интерфейса; esi - адрес от начала флеш
; ecx - количество слов к чтению; es:edi - адрес dst
;***********************************************************************
INT1000_READ_FLC        PROC
                        call    INT1024_READ_FLC        ; чтение не отличается
                                                        ; от нового
                        ret
INT1000_READ_FLC        ENDP
;***********************************************************************
; Запись на флеш интерфейса; edi- адрес от начала флеш
; ecx - количество слов к записи; ds:esi - адрес src
;***********************************************************************
INT1000_WRITE_FLC       PROC
                        cld
                        push    es
                        push    eax
                        shl     ecx, 1
                        mov     es, _FLASH2_SEG     ; Сегмент флеш
INT1000_WRITE_FLC_LOOP:
                        ; проверим, надо ли стирать
                        test    edi, 0ffffh          ; адрес кратен 64кб
                        jnz     _Nt_Erse2            ; не надо
                        call    INT1000_ERASE_FLC    ; стирать
            _Nt_Erse2:
		        DB    	67h
                        lodsb
                        cmp     al, 0ffh
                        je      _Nt_Save2
                        push    ax
                        ; выберем окно
                        mov     eax, edi             ;
                        shr     eax, 14              ; al - номер окна
                                                     ; для перестановки окон
                        ; команда "program word"
                        inc     al
                        MOV     ES,_FLASH2HELP_SEG
                        MOV     BYTE PTR ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG
                        MOV     BYTE PTR ES:[1555h],0AAh

                        dec     al
                        MOV     ES,_FLASH2HELP_SEG
                        MOV     BYTE PTR ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG
                        MOV     BYTE PTR ES:[2AAAh],55h

                        inc     al
                        MOV     ES,_FLASH2HELP_SEG
                        MOV     BYTE PTR ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG
                        MOV     BYTE PTR ES:[1555h],0A0h

                        mov     eax, edi             ;
                        shr     eax, 14              ; al - номер окна

                        MOV     ES,_FLASH2HELP_SEG
                        MOV     ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG
                        ; можно писать и проверять
                        pop     ax
                        push    edi
                        and     edi, 03fffh
                        MOV     ES:[di], AL
                        ; Проверка записи
            _Test_loop2:
                        mov     dl, ES:[di]
                        cmp     al, dl
                        je      _Saved_OK2                  ; Записалось
                        test    dl, 20h
                        jz      _Test_loop2                 ; Не проходил стадию
                        mov     dl, ES:[di]
                        cmp     al, dl
                        jne     INT1000_WRITE_ErrEX
            _Saved_OK2:
                        pop     edi
            _Nt_Save2:  ; не надо записывать
                        inc     edi                         ; сдвинем

                        ;DB      67h
                        ;loop    INT1000_WRITE_FLC_LOOP      ; Следующий байт
                        dec     ecx
                        jnz     INT1000_WRITE_FLC_LOOP      ; Следующий байт
                        ; Нормальный выход
INT1000_WRITE_EX:
                        pop     eax
                        pop     es
                        clc
                        ret
INT1000_WRITE_ErrEX:
                        pop     edi
                        pop     eax
                        pop     es                          ; Error exit
                        stc
                        ret
;***********************************************************************
; ФУНКЦИЯ записи без стирания (не дальше сектора)
;***********************************************************************
INT1000_WRTNE_FLC:
                        cld
                        push    es
                        push    eax
                        shl     ecx, 1
                        mov     es, _FLASH2_SEG     ; Сегмент флеш
                        jmp     _Nt_Erse2
;***********************************************************************
;подфункция стирания сектора (64кб) (может работать как фунцкия)
;edi- адрес от начала флеш
;***********************************************************************
INT1000_ERASE_FLC:
                        call    INT1000_ERSCM_FLC       ; Команда
    INT1000_ERASE_FLC_L:
                        call    INT1000_ERSCHK_FLC      ; проверим
                        jc      INT1000_ERASE_FLC_L
                        ret
;***********************************************************************
;подфункция команды стирания сектора (64кб)
;edi- адрес от начала флеш
;***********************************************************************
INT1000_ERSCM_FLC:
                        push    eax
                        push    es
                        mov     es, _FLASH2_SEG      ; Сегмент флеш

                        mov     eax, edi             ;
                        shr     eax, 14              ; aL - номер окна
                        and     al, 0E0h
                                                     ; для перестановки окон
                        ; команда "erase sector"
                        inc     al
                        MOV     ES,_FLASH2HELP_SEG
                        MOV     BYTE PTR ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG
                        mov     Byte PTR ES:[1555h], 0AAh

                        dec     al
                        MOV     ES,_FLASH2HELP_SEG
                        MOV     BYTE PTR ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG
                        mov     Byte PTR ES:[2AAAh], 055h

                        inc     al
                        MOV     ES,_FLASH2HELP_SEG
                        MOV     BYTE PTR ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG
                        mov     Byte PTR ES:[1555h], 080h
                        mov     Byte PTR ES:[1555h], 0AAh

                        dec     al
                        MOV     ES,_FLASH2HELP_SEG
                        MOV     BYTE PTR ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG
                        mov     Byte PTR ES:[2AAAh], 055h

                        mov     eax, edi             ;
                        shr     eax, 14              ; aL - номер окна
                        MOV     ES,_FLASH2HELP_SEG
                        MOV     ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG

                        MOV     BYTE PTR ES:[0],30h
                        pop     es
                        pop     eax
                        ret
;***********************************************************************
; подфункция проверки стирания сектора
;***********************************************************************
INT1000_ERSCHK_FLC:
                        call    INT1024_ERSCHK_FLC
                        ret
INT1000_WRITE_FLC       ENDP

; <<<<<<<<<<<<<<<<<<<<<<<<<<<<<< END >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
