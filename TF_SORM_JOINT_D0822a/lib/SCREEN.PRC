;**********************************
;*  �㭪樨 ��� ࠡ��� � ��࠭��  *
;**********************************

; ����ன��
local   @@
p386
_SEG_SCREEN     equ     0B800h
_ScreenSize     equ     8000

@to_seg_scr     macro   _PAR_
        push    _SEG_SCREEN
ifdef   _PAR_
        pop     _PAR_
else
        pop     es
endif
                endm

; --- -=* �뢮� ��⭠����筮�� �᫠ �� ��࠭ � �������� ����樨 *=- ------
; ebx - �᫮
; ah - ��ਡ��
; cl - ᪮�쪮 �뢮���� ��ࠤ (1 - 8)
; di - �㤠 �뢮���� (ᬥ饭�� ��࠭�)
PrintH          proc
        or      cl, cl          ; �஢�ਬ �� ����
        jz      @@exitH         ; �᫨ ���� �멤��

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
PrintH          endp

; --- �뢮� 3-� ���筮�� �����筮�� �᫠ ----
; -�室: ebx - �᫮
;         di - �㤠 �뢮����
;         ah - ��ਡ��
; ---                                      ----
printDigs       proc
                cmp     di, _ScreenSize - 2
                ja      @@exit_bad
        push    es
                mov     es, ss:SEG_FS
        pushad
                xor     ecx, ecx
                xor     edx, edx
                xchg    ebx, eax                ; -> bh - ��ਡ��, -> eax - �᫮
                cmp     eax, 999
                ja      @@exit
                mov     ecx, 100
                div     ecx
                or      eax, eax
                jz      @@draw_tens
                mov     ah, bh
                add     al, '0'
                mov     word ptr es:[di], ax
                add     di, 2
                mov     ecx, 10
                mov     eax, edx
                xor     edx, edx
                div     ecx
                jmp     @@draw_tens_1

@@draw_tens:    add     di, 2
                mov     ecx, 10
                mov     eax, edx
                xor     edx, edx
                div     ecx
                or      eax, eax
                jz      @@draw_ones
@@draw_tens_1:  mov     ah, bh
                add     al, '0'
                cmp     di, _ScreenSize - 2
                ja      @@exit
                mov     word ptr es:[di], ax
@@draw_ones:    add     di, 2
                mov     eax, edx
                mov     ah, bh
                add     al, '0'
                cmp     di, _ScreenSize - 2
                ja      @@exit
                mov     word ptr es:[di], ax
        @@exit:
                popad
                pop     es
@@exit_bad:
                ret
printDigs       endp

; --- -=* �뢮� ��⭠����筮�� �᫠ �� ��࠭ � �������� ����樨 *=- ------
; ds:[si] - ��㤠 �뢮����
; ah      - ��ਡ��
; cl      - ᪮�쪮 �뢮���� ����
; di      - �㤠 �뢮���� (ᬥ饭�� ��࠭�)
printHex        proc
                or      cl, cl          ; �஢�ਬ �� ����
                jz      @@exitH         ; �᫨ ���� �멤��
                push    ax cx di si es
                mov     es, ss:SEG_FS
                xor     ch, ch
                cmp     di, _ScreenSize
                ja      @@exit
                cld
@@loop:         lodsb
                and     al, 0Fh
                cmp     al, 9
                jbe     @@no_hex
                add     al, 'A' - '9' - 1
@@no_hex:       add     al, '0'
                stosw
                loop    @@loop
@@exit:         pop     es si di cx ax
@@exitH:        ret
printHex        endp

; --- -=* �뢮� ��ப� �� ��࠭ (��ப� ������ ����稢����� �㫥�) *=- --------
; SI - ��㤠
; DI - �㤠 (ᬥ饭�� ��࠭�)
; AH - ��ਡ��
printS          PROC
                push    si di ax es
                mov     es, ss:SEG_FS
@@loop:         cmp     di, _ScreenSize - 2
                ja      @@exit
                lodsb
                or      al, al
                jz      @@exit
                stosw
                jmp     @@loop
@@exit:         pop     es ax di si
                ret
printS          ENDP

; --- �뢮� ��ப� �� ᥣ���� ��࠭�
printSscr       PROC
                push    ds
                mov     ds, ss:SEG_PCM
                call    printS
                pop     ds
                ret
printSscr       ENDP

; --- -=* �뢮� ��ப� �� ��࠭ � ᬥ饭��� 㪠��⥫� (��ப� ������ ����稢����� �㫥�) *=- ---
; SI - ��㤠
; DI - �㤠 (ᬥ饭�� ��࠭�)
; AH - ��ਡ��
printSdi        PROC
        push    si ax es
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
        pop     es ax si
        ret
printSdi        ENDP



; --- -=* �뢮� ��ப� ��।��񭭮� ����� �� ��࠭ (��ப� ������ ����稢����� �㫥�) *=- --------
; CX - �����
; SI - ��㤠
; DI - �㤠 (ᬥ饭�� ��࠭�)
; AH - ��ਡ��
printSLen       PROC
        push    si di ax es
        mov     es, ss:SEG_FS
@@loop:
        cmp     di, _ScreenSize - 2
        ja      @@exit
        lodsb
        stosw
        loop    @@loop
@@exit:
        pop     es ax di si
        ret
printSLen       ENDP


; --- ��楤�� �뢮�� ���� ---------------------------------------------------
; � SI - 㪠��⥫� �� ��ப�
Print_log_err   PROC
        pusha
				mov     al, _color_err_log
				mov     color_log, al
        jmp     Print_log_2
				ENDP

Print_log       PROC
        pusha
				mov     al, _color_log
				mov     color_log, al
Print_log_2:
        push    si
        mov     si, offset LOG_CLEAR
        mov     di, _pos_log
        mov     ah, _color_cl_log
        call    printS
        pop     si

        mov     di, _pos_log
        mov     ah, color_log
        call    printS

        mov     LOG_CHANGE, 1
        mov     ax, TIME_2MC
        mov     LOG_TIMEOUT, ax
        popa
        ret
Print_log       ENDP


; --- ��楤�� �뢮�� ����� �୮�� ᯨ᪠ ----------------------------------
; ebx - �᫮
; ah - ��ਡ��
; di - �㤠 �뢮����
PrintBL          proc
        push    ecx di edx ax es
        mov     es, ss:SEG_FS
        cmp     di, _ScreenSize - 8*_x
        ja      @@exit

        mov     edx, ebx
        rol     edx, 4
        and     dl, 0Fh
        test    dl, 1000b               ;\ ����� 7 㩤��
        jnz     @@print_error           ;/
        and     ah, not 80h
        or      ah, 8h
        mov     al, dl
        push    ax
        add     al, '0'
        stosw
        pop     ax

        xor     ah, 80h
        mov     cx, 7
        sub     cl, dl
        shl     cl, 2
        push    ebx
        shr     ebx, cl
        mov     cl, dl
        call    printH
        pop     ebx

        shl     cl, 1
        add     di, cx
        xor     ah, 80h
        mov     cl, 7
        sub     cl, dl
        call    printH
@@exit:
        pop     es ax edx di ecx
        ret
@@print_error:
        mov     ah, cBLUE*10h + clRED
        mov     cx, 8
        call    PrintH
        jmp     @@exit
PrintBL          endp

; --- -=* �뢮� ������筮�� �᫠ �� ��࠭ � �������� ����樨 *=- ------
; ebx - �᫮
; ah - ��ਡ��
; cl - ᪮�쪮 �뢮���� ��ࠤ (1 - 8)
; di - �㤠 �뢮���� (ᬥ饭�� ��࠭�)
printD         proc
        or      cl, cl          ; �஢�ਬ �� ����
        jz      @@exitD         ; �᫨ ���� �멤��

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
printD         endp

; --- -=* �뢮� ��⭠����筮�� �᫠ �� ��࠭ � �������� ����樨 *=- ------
; ebx - �᫮
; ah - ��ਡ��
; cl - ᪮�쪮 �뢮���� ��ࠤ (1 - 8)
; di - �㤠 �뢮���� (ᬥ饭�� ��࠭�)
prnH            proc
                or      cl, cl          ; �஢�ਬ �� ����
                jz      @@exitH         ; �᫨ ���� �멤��
                push    ax ebx cx di es
                mov     es, ss:SEG_FS
                xor     ch, ch
                add     di, cx
                add     di, cx
                sub     di, 2
                cmp     di, _ScreenSize
                ja      @@exit
@@loop:         mov     al, bl
                and     al, 0Fh
                cmp     al, 9
                jbe     @@no_hex
                add     al, 'A' - '9' - 1
@@no_hex:       add     al, '0'
                mov     es:[di], ax
                shr     ebx, 4
                sub     di, 2
                loop    @@loop
@@exit:         pop     es di cx ebx ax
@@exitH:        ret
prnH            endp

; --- -=* �뢮� ���ਡ�⮢ �� ��࠭ � �������� ����樨 *=- ------
; ah - ��ਡ��
; cl - ᪮�쪮 �뢮����
; di - �㤠 �뢮���� (ᬥ饭�� ��࠭�)
PrintA          proc
                or      cl, cl          ; �஢�ਬ �� ����
                jz      @@exitA         ; �᫨ ���� �멤��

                push    cx di ax es
                mov     es, ss:SEG_FS
                mov     al, ah
                inc     di
                cld
@@loop:         cmp     di, _ScreenSize - 2
                ja      @@exit
                stosb
                inc     di
                loop    @@loop
@@exit:
                pop     es ax di cx
@@exitA:
                ret
PrintA          endp


; --- -=* �뢮� ᨬ����� �� ��࠭ � �������� ����樨 *=- ------
; al - ᨬ���
; ah - ��ਡ��
; cl - ᪮�쪮 �뢮����
; di - �㤠 �뢮���� (ᬥ饭�� ��࠭�)
printC          proc
                or      cl, cl          ; �஢�ਬ �� ����
                jz      @@exitC         ; �᫨ ���� �멤��
                push    cx di es
                mov     es, ss:SEG_FS
                cld
@@loop:         cmp     di, _ScreenSize - 2
                ja      @@exit
                stosw
                loop    @@loop
@@exit:         pop     es di cx
@@exitC:        ret
printC          endp

; --- -=* �뢮� ᨬ����� �� ��࠭ � �������� ����樨 � ᬥ饭��� 㪠��⥫� *=- ------
; al - ᨬ���
; ah - ��ਡ��
; cx - ᪮�쪮 �뢮����
; di - �㤠 �뢮���� (ᬥ饭�� ��࠭�)
printCdi        proc
                or      cl, cl          ; �஢�ਬ �� ����
                jz      @@exitC         ; �᫨ ���� �멤��
                push    cx es
                mov     es, ss:SEG_FS
                cld
@@loop:         cmp     di, _ScreenSize - 2
                ja      @@exit
                stosw
                loop    @@loop
@@exit:         pop     es cx
@@exitC:        ret
printCdi        endp


; --- -=* �뢮� ����筮�� �᫠ �� ��࠭ � �������� ����樨 � �⠡��쭮� �ଥ *=- ------
; ah - ���ਡ��
; ebx - �᫮
; cl - ᪮�쪮 �뢮����
; di - �㤠 �뢮���� (ᬥ饭�� ��࠭�)
printBx         proc
                or      cl, cl          ; �஢�ਬ �� ����
                jz      @@exitBx        ; �᫨ ���� �멤��
                push    ax ebx cx di es
                cmp     cl, 32
                jbe     @@continue
                mov     cl, 32
@@continue:     xor     ch, ch
                add     di, cx
                add     di, cx
                test    cl, 3
                jz      @@continue_1
                add     di, 2
@@continue_1:   push    cx
                shr     cx, 2
                jcxz    @@continue_2
                dec     cx
@@continue_2:   add     di, cx
                add     di, cx
                pop     cx
                mov     es, ss:SEG_FS
                std
@@loop:         cmp     di, _ScreenSize - 2
                ja      @@exit
                shr     ebx, 1
                mov     al, 'X'
                jc      @@draw
                mov     al, '-'
@@draw:         stosw
                inc     ch
                cmp     ch, cl
                jae     @@exit
                test    ch, 3
                jnz     @@next
                mov     al, '.'
                stosw
@@next:         jmp      @@loop
@@exit:         cld
                pop     es di cx ebx ax
@@exitBx:       ret
printBx         endp

; --- -=* �뢮� ����� ⥫�䮭� � �ଠ� ���� �� ��࠭ � �������� ����樨 *=- ------
; ah - ���ਡ��
; ebx - ����� ⥫�䮭�
; di - �㤠 �뢮���� (ᬥ饭�� ��࠭�)
printNsids      proc
                push    ax ebx cx di es
                mov     cx, 8
                rol     bl, 4
                rol     bh, 4
                rol     ebx, 16
                rol     bl, 4
                rol     bh, 4
                rol     ebx, 16
                mov     es, ss:SEG_FS
@@loop:         cmp     di, _ScreenSize - 2
                ja      @@exit
                mov     al, bl
                and     al, 0fh
                cmp     al, 0fh
                je      @@num_end
                add     al, '0'
                jmp     @@draw
@@num_end:      mov     al, ' '
@@draw:         stosw
                shr     ebx, 4
                loop    @@loop
@@exit:         pop     es di cx ebx ax
                ret
printNsids      endp

; --- -=* �뢮� ����� ⥫�䮭� � �ଠ� SEG_NLOG �� ��࠭ � �������� ����樨 *=- ------
; ah - ���ਡ��
; ebx - ����� ⥫�䮭�
; di - �㤠 �뢮���� (ᬥ饭�� ��࠭�)
printNlog       proc
                push    ax ebx cx dx di es
                mov     cx, 8
                xor     dx, dx
                mov     es, ss:SEG_FS
@@loop:         cmp     di, _ScreenSize - 2
                ja      @@exit
                rol     ebx, 4
                mov     al, bl
                and     al, 0fh
                jz      @@zero_found
                inc     dx
@@zero_found:   or      dx, dx
                jz      @@skip
                add     al, '0'
                stosw
@@skip:         loop    @@loop
@@exit:         pop     es di dx cx ebx ax
                ret
printNlog       endp

; --- -=* �뢮� 䨧��᪮�� ����� ����� �� ��࠭ � �������� ����樨 *=- ------
; ah - ���ਡ��
; ebx - 䨧��᪨� ����� �����
; di - �㤠 �뢮���� (ᬥ饭�� ��࠭�)
printNphys      proc
                push    cx di es
                mov     es, ss:SEG_FS
                push    ebx
                shr     ebx, 13
                mov     cx, 2
                call    printD
                pop     ebx
                add     di, 4
                mov     al, '/'
                stosw
                push    ebx
                shr     ebx, 9
                and     ebx, 0fh
                mov     cx, 2
                call    printD
                pop     ebx
                add     di, 4
                mov     al, '/'
                stosw
                push    ebx
                and     ebx, 01ffh
                mov     cx, 3
                call    printD
                pop     ebx
                pop     es di cx
                ret
printNphys      endp


; �뢮� ���� �� ᥣ���� ��࠭�
printWin        PROC
                jcxz    @@quit
                push    ds es ax cx di si
                mov     ds, ss:SEG_PCM
                mov     es, ss:SEG_FS
@@loop:         push    di
@@loop_scr:     cmp     di, _ScreenSize - 2
                ja      @@next
                lodsb
                or      al, al
                jz      @@next
                stosw
                jmp     @@loop_scr
@@next:         pop     di
                add     di, 160
                loop    @@loop
                pop     si di cx ax es ds
@@quit:         ret
printWin        ENDP

; �뢮� ���� �� ᥣ���� ds
printWin_ds     PROC
                jcxz    @@quit
                push    es ax cx di si
                mov     es, ss:SEG_FS
@@loop:         push    di
@@loop_scr:     cmp     di, _ScreenSize - 2
                ja      @@next
                lodsb
                or      al, al
                jz      @@next
                stosw
                jmp     @@loop_scr
@@next:         pop     di
                add     di, 160
                loop    @@loop
                pop     si di cx ax es
@@quit:         ret
printWin_ds     ENDP

; --- �뢮� �����筮�� �᫠ ���� 梥⮬ �� �ॢ�襭�� ��ண�
; ebx - �᫮
; edx - ��ண
; cl - ᪮�쪮 �뢮���� ���
; di - �㤠 �뢮���� (ᬥ饭�� ��࠭�)
printDavar      PROC
                mov     ah, _color_work
                cmp     ebx, edx
                jbe     @@print
                mov     ah, _color_avar
@@print:        call    printD
                ret
printDavar      ENDP

; --- -=* �뢮� ����� ⥫�䮭� � �ଠ� SEG_ULOG �� ��࠭ � �������� ����樨 *=- ------
; ah - ���ਡ��
; ebx - ����� ⥫�䮭�
; di - �㤠 �뢮���� (ᬥ饭�� ��࠭�)
printUlog       proc
                push    ax ebx cx dx di es
                mov     cx, 8
                xor     dx, dx
                mov     es, ss:SEG_FS
                cmp     di, _ScreenSize - 2 - 7 * 2
                ja      @@exit
                push    ebx
                shr     ebx, 16
                mov     cx, 2
                call    printD
                pop     ebx
                add     di, 4
                mov     al, '-'
                stosw
                and     ebx, 0ffffh
                mov     cx, 4
                call    printD
@@exit:         pop     es di dx cx ebx ax
                ret
printUlog       endp

