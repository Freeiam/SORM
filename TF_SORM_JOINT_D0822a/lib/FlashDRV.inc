;***********************************************************************
; 29/09/2004    (C)Odinokov Alex
;************************����� ��������� ��� ����***********************
; ᮤ�ন� ������⥪� ��� ����� 䫥�:
; 1) ELAN400 - �ࠩ��� Elan410 flash 29DL32(LV16)
; 2) INT1024 - �ࠩ��� Interface1024 flash 29DL32
; 3) INT1000 - �ࠩ��� Interface1000 flash ????
; ����� �ࠩ��� ����砥� � ᥡ� ᫥���騩 ����� �㭪権:
; 1) READ_FLC - �⥭�� � ����
; 2) WRITE_FLC - ������ � ��⮧��࠭��� ��।���� ᥪ��
; 3) WRTNE_FLC - ������ ��� ��� ���࠭�� (����� ��室��� �� �।��� 1 ᥪ��)
; 4) ERASE_FLC - ���࠭�� ᥪ��
; 5) ERSCM_FLC - ������� �� ���࠭��
; 6) ERSCHK_FLC - �஢�ઠ �� �믮������ ������� �� ���࠭��
;***********************************************************************

; <<<<<<<<<<<<<<<<<<<<<<<<< �ࠩ��� Elan410 flash 29DL32(LV16) >>>>>>>>>>>>>>>>>>>>>>>>>>>>
;***********************************************************************
; �⥭�� 䫥� Elan'a; esi - ���� �� ��砫� 䫥�
; ecx - ������⢮ ᫮� � �⥭��; es:edi - ���� dst
;***********************************************************************
ELAN400_READ_FLC        PROC
                        cld
                        push    ds
                        push    eax
                        mov     ds, _FLASH1_SEG     	; ������� 䫥�
                        ; �롥६ ����
                        mov     eax, esi             	;
                        shr     eax, 7               	; ah - ����� ����
                        mov     al, 32h              	; al - ����� INDEX'a ���� �����
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
                        ; ������� ����
                        mov     eax, esi             	;
                        shr     eax, 7               	; ah - ����� ����
                        mov     al, 32h              	; al - ����� INDEX'a ���� �����
			out     22h, ax
     ELAN400_READ_FLC_NZ:
                        DB      67h
                        loop    ELAN400_READ_FLC_MOV

                        pop     eax
			pop	ds
                        ret
ELAN400_READ_FLC        ENDP
;***********************************************************************
; ������ �� 䫥� Elan'a; edi- ���� �� ��砫� 䫥�
; ecx - ������⢮ ᫮� � �����; ds:esi - ���� src
;***********************************************************************
ELAN400_WRITE_FLC      	PROC
                        cld
                        push    es
                        push    eax
                        mov     es, _FLASH1_SEG     ; ������� 䫥�
ELAN400_WRITE_FLC_LOOP:
                        ; �஢�ਬ, ���� �� �����
                        test    edi, 0ffffh          ; ���� ��⥭ 64��
                        jnz     _Nt_Erse             ; �� ����
                        call    ELAN400_ERASE_FLC    ; �����
            _Nt_Erse:
		        DB    	67h
                        lodsw
                        cmp     ax, 0ffffh
                        je      _Nt_Save
                        push    ax
                        ; �롥६ ����
                        mov     eax, edi             ;
                        shr     eax, 7               ; ah - ����� ����
                        mov     al, 32h              ; al - ����� INDEX'a ���� �����
                                                     ; ��� ����⠭���� ����
                        push    ax
                        and     ah, 0C0h             ; ���� ��� �ࠢ�����
                        out     22h, ax
                        ; ������� "program word"
                        MOV BYTE PTR ES:[0AAAh],0AAh
                        MOV BYTE PTR ES:[554h],55h
                        MOV BYTE PTR ES:[0AAAh],0A0h
                        pop     ax
                        out     22h, ax              ; �� ࠧ �롥६ ����
                        ; ����� ����� � �஢�����
                        pop     ax
                        push    edi
                        and     edi, 07fffh
                        MOV     ES:[di], AX
                        ; �஢�ઠ �����
            _Test_loop:
                        mov     dx, ES:[di]
                        cmp     al, dl
                        je      _Saved_OK                   ; ����ᠫ���
                        test    dl, 20h
                        jz      _Test_loop                  ; �� ��室�� �⠤��
                        mov     dx, ES:[di]
                        cmp     al, dl
                        je      _Saved_OK                   ; ����ᠫ���

                        pop     edi
                        pop     eax
                        pop     es                          ; Error exit
                        stc
                        ret
            _Saved_OK:
                        pop     edi
            _Nt_Save:   ; �� ���� �����뢠��
                        add     edi, 2                      ; ᤢ����

                        DB      67h
                        loop    ELAN400_WRITE_FLC_LOOP      ; ������騩 ����
                        ; ��ଠ��� ��室
ELAN400_WRITE_EX:
                        pop     eax
                        pop     es
                        clc
                        ret
;***********************************************************************
; ������� ����� ��� ��࠭�� (�� ����� ᥪ��)
;***********************************************************************
ELAN400_WRTNE_FLC:
                        cld
                        push    es
                        push    eax
                        mov     es, _FLASH1_SEG     ; ������� 䫥�
                        jmp     _Nt_Erse
;***********************************************************************
;����㭪�� ��࠭�� ᥪ�� (64��) (����� ࠡ���� ��� ��檨�)
;edi- ���� �� ��砫� 䫥�
;***********************************************************************
ELAN400_ERASE_FLC:
                        call    ELAN400_ERSCM_FLC       ; �������
    ELAN400_ERASE_FLC_L:
                        call    ELAN400_ERSCHK_FLC      ; �஢�ਬ
                        jc      ELAN400_ERASE_FLC_L
                        ret
;***********************************************************************
;����㭪�� ������� ��࠭�� ᥪ�� (64��)
;edi- ���� �� ��砫� 䫥�
;***********************************************************************
ELAN400_ERSCM_FLC:
                        push    eax
                        push    es
                        mov     es, _FLASH1_SEG      ; ������� 䫥�

                        mov     eax, edi             ;
                        shr     eax, 7               ; ah - ����� ����
                        mov     al, 32h              ; al - ����� INDEX'a ���� �����
                                                     ; ��� ����⠭���� ����
                        push    ax
                        and     ah, 0C0h             ; ���� ��� �ࠢ�����
                        out     22h, ax
                        ; ������� "erase sector"
                        MOV     BYTE PTR ES:[0AAAh],0AAh
                        MOV     BYTE PTR ES:[554h],55h
                        MOV     BYTE PTR ES:[0AAAh],80h
                        MOV     BYTE PTR ES:[0AAAh],0AAh
                        MOV     BYTE PTR ES:[554h],55h
                        pop     ax
                        out     22h, ax              ; �� ࠧ �롥६ ����
                        MOV     BYTE PTR ES:[0],30h
                        pop     es
                        pop     eax
                        ret
;***********************************************************************
; ����㭪�� �஢�ન ��࠭�� ᥪ��
;***********************************************************************
ELAN400_ERSCHK_FLC:
                        push    eax
                        push    es
                        mov     es, _FLASH1_SEG      ; ������� 䫥�

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

; <<<<<<<<<<<<<<<<<<<<<< �ࠩ��� Interface1024 flash 29DL32 >>>>>>>>>>>>>>>>>>>>>>>>>>
;***********************************************************************
; �⥭�� 䫥� ����䥩�; esi - ���� �� ��砫� 䫥�
; ecx - ������⢮ ᫮� � �⥭��; es:edi - ���� dst
;***********************************************************************
INT1024_READ_FLC        PROC
                        cld
                        push    ds
                        push    eax
                        shl     ecx, 1
                        mov     ds, _FLASH2_SEG         ; ������� 䫥�
                        ; �롥६ ����
                        mov     eax, esi             	;
                        shr     eax, 14                 ; al - ����� ����
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
                        ; ������� ����
                        mov     eax, esi             	;
                        shr     eax, 14                 ; al - ����� ����
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
; ������ �� 䫥� ����䥩�; edi- ���� �� ��砫� 䫥�
; ecx - ������⢮ ᫮� � �����; ds:esi - ���� src
;***********************************************************************
INT1024_WRITE_FLC       PROC
                        cld
                        push    es
                        push    eax
                        shl     ecx, 1
                        mov     es, _FLASH2_SEG     ; ������� 䫥�
INT1024_WRITE_FLC_LOOP:
                        ; �஢�ਬ, ���� �� �����
                        test    edi, 0ffffh          ; ���� ��⥭ 64��
                        jnz     _Nt_Erse1            ; �� ����
                        call    INT1024_ERASE_FLC    ; �����
            _Nt_Erse1:
		        DB    	67h
                        lodsb
                        cmp     al, 0ffh
                        je      _Nt_Save1
                        push    ax
                        ; �롥६ ����
                        mov     eax, edi             ;
                        shr     eax, 14              ; al - ����� ����
                                                     ; ��� ����⠭���� ����
                        MOV     ES,_FLASH2HELP_SEG
                        MOV     BYTE PTR ES:[ADR_RU],0;AL
                        MOV     ES,_FLASH2_SEG
                        ; ������� "program word"
                        MOV     BYTE PTR ES:[0AAAh],0AAh
                        MOV     BYTE PTR ES:[555h],55h
                        MOV     BYTE PTR ES:[0AAAh],0A0h

                        MOV     ES,_FLASH2HELP_SEG
                        MOV     ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG
                        ; ����� ����� � �஢�����
                        pop     ax
                        push    edi
                        and     edi, 03fffh
                        MOV     ES:[di], AL
                        ; �஢�ઠ �����
            _Test_loop1:
                        mov     dl, ES:[di]
                        cmp     al, dl
                        je      _Saved_OK1                  ; ����ᠫ���
                        test    dl, 20h
                        jz      _Test_loop1                 ; �� ��室�� �⠤��
                        mov     dl, ES:[di]
                        cmp     al, dl
                        jne     INT1024_WRITE_ErrEX
            _Saved_OK1:
                        pop     edi
            _Nt_Save1:  ; �� ���� �����뢠��
                        inc     edi                         ; ᤢ����

                        DB      67h
                        loop    INT1024_WRITE_FLC_LOOP      ; ������騩 ����
                        ; ��ଠ��� ��室
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
; ������� ����� ��� ��࠭�� (�� ����� ᥪ��)
;***********************************************************************
INT1024_WRTNE_FLC:
                        cld
                        push    es
                        push    eax
                        shl     ecx, 1
                        mov     es, _FLASH2_SEG     ; ������� 䫥�
                        jmp     _Nt_Erse1
;***********************************************************************
;����㭪�� ��࠭�� ᥪ�� (64��) (����� ࠡ���� ��� ��檨�)
;edi- ���� �� ��砫� 䫥�
;***********************************************************************
INT1024_ERASE_FLC:
                        call    INT1024_ERSCM_FLC       ; �������
    INT1024_ERASE_FLC_L:
                        call    INT1024_ERSCHK_FLC      ; �஢�ਬ
                        jc      INT1024_ERASE_FLC_L
                        ret
;***********************************************************************
;����㭪�� ������� ��࠭�� ᥪ�� (64��)
;edi- ���� �� ��砫� 䫥�
;***********************************************************************
INT1024_ERSCM_FLC:
                        push    eax
                        push    es
                        mov     es, _FLASH2_SEG      ; ������� 䫥�

                        mov     eax, edi             ;
                        shr     eax, 14              ; aL - ����� ����
                                                     ; ��� ����⠭���� ����
                        MOV     ES,_FLASH2HELP_SEG
                        MOV     BYTE PTR ES:[ADR_RU],0;AL
                        MOV     ES,_FLASH2_SEG

                        ; ������� "erase sector"
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
; ����㭪�� �஢�ન ��࠭�� ᥪ��
;***********************************************************************
INT1024_ERSCHK_FLC:
                        push    eax
                        push    es
                        mov     es, _FLASH2_SEG      ; ������� 䫥�
                        ; �஢��塞 ����� �����
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
; <<<<<<<<<<<<<<<<<<<<<< �ࠩ��� Interface1000 flash ???? >>>>>>>>>>>>>>>>>>>>>>>>>>
;***********************************************************************
; �⥭�� 䫥� ����䥩�; esi - ���� �� ��砫� 䫥�
; ecx - ������⢮ ᫮� � �⥭��; es:edi - ���� dst
;***********************************************************************
INT1000_READ_FLC        PROC
                        call    INT1024_READ_FLC        ; �⥭�� �� �⫨砥���
                                                        ; �� ������
                        ret
INT1000_READ_FLC        ENDP
;***********************************************************************
; ������ �� 䫥� ����䥩�; edi- ���� �� ��砫� 䫥�
; ecx - ������⢮ ᫮� � �����; ds:esi - ���� src
;***********************************************************************
INT1000_WRITE_FLC       PROC
                        cld
                        push    es
                        push    eax
                        shl     ecx, 1
                        mov     es, _FLASH2_SEG     ; ������� 䫥�
INT1000_WRITE_FLC_LOOP:
                        ; �஢�ਬ, ���� �� �����
                        test    edi, 0ffffh          ; ���� ��⥭ 64��
                        jnz     _Nt_Erse2            ; �� ����
                        call    INT1000_ERASE_FLC    ; �����
            _Nt_Erse2:
		        DB    	67h
                        lodsb
                        cmp     al, 0ffh
                        je      _Nt_Save2
                        push    ax
                        ; �롥६ ����
                        mov     eax, edi             ;
                        shr     eax, 14              ; al - ����� ����
                                                     ; ��� ����⠭���� ����
                        ; ������� "program word"
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
                        shr     eax, 14              ; al - ����� ����

                        MOV     ES,_FLASH2HELP_SEG
                        MOV     ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG
                        ; ����� ����� � �஢�����
                        pop     ax
                        push    edi
                        and     edi, 03fffh
                        MOV     ES:[di], AL
                        ; �஢�ઠ �����
            _Test_loop2:
                        mov     dl, ES:[di]
                        cmp     al, dl
                        je      _Saved_OK2                  ; ����ᠫ���
                        test    dl, 20h
                        jz      _Test_loop2                 ; �� ��室�� �⠤��
                        mov     dl, ES:[di]
                        cmp     al, dl
                        jne     INT1000_WRITE_ErrEX
            _Saved_OK2:
                        pop     edi
            _Nt_Save2:  ; �� ���� �����뢠��
                        inc     edi                         ; ᤢ����

                        ;DB      67h
                        ;loop    INT1000_WRITE_FLC_LOOP      ; ������騩 ����
                        dec     ecx
                        jnz     INT1000_WRITE_FLC_LOOP      ; ������騩 ����
                        ; ��ଠ��� ��室
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
; ������� ����� ��� ��࠭�� (�� ����� ᥪ��)
;***********************************************************************
INT1000_WRTNE_FLC:
                        cld
                        push    es
                        push    eax
                        shl     ecx, 1
                        mov     es, _FLASH2_SEG     ; ������� 䫥�
                        jmp     _Nt_Erse2
;***********************************************************************
;����㭪�� ��࠭�� ᥪ�� (64��) (����� ࠡ���� ��� ��檨�)
;edi- ���� �� ��砫� 䫥�
;***********************************************************************
INT1000_ERASE_FLC:
                        call    INT1000_ERSCM_FLC       ; �������
    INT1000_ERASE_FLC_L:
                        call    INT1000_ERSCHK_FLC      ; �஢�ਬ
                        jc      INT1000_ERASE_FLC_L
                        ret
;***********************************************************************
;����㭪�� ������� ��࠭�� ᥪ�� (64��)
;edi- ���� �� ��砫� 䫥�
;***********************************************************************
INT1000_ERSCM_FLC:
                        push    eax
                        push    es
                        mov     es, _FLASH2_SEG      ; ������� 䫥�

                        mov     eax, edi             ;
                        shr     eax, 14              ; aL - ����� ����
                        and     al, 0E0h
                                                     ; ��� ����⠭���� ����
                        ; ������� "erase sector"
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
                        shr     eax, 14              ; aL - ����� ����
                        MOV     ES,_FLASH2HELP_SEG
                        MOV     ES:[ADR_RU],AL
                        MOV     ES,_FLASH2_SEG

                        MOV     BYTE PTR ES:[0],30h
                        pop     es
                        pop     eax
                        ret
;***********************************************************************
; ����㭪�� �஢�ન ��࠭�� ᥪ��
;***********************************************************************
INT1000_ERSCHK_FLC:
                        call    INT1024_ERSCHK_FLC
                        ret
INT1000_WRITE_FLC       ENDP

; <<<<<<<<<<<<<<<<<<<<<<<<<<<<<< END >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
