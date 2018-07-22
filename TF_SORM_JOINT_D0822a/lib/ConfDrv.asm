; �������
;***********************************************************************
; FLInicialize - �㭪�� ॠ����� ���樠������ ������ ��� ࠡ���
; ��⮮�।������ ⮦� ���� ��⠢���� �
;***********************************************************************
FLInicialize		PROC
			; ����� ���� ��।����� ࠧ��� 䫥� � �.�.
                        push    es
                        mov     es, SS:SEG_AX

			; ��।���� ���� �����
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
; ����⢠��� ���䨣��樨 ��� ��� ���ଠ樨 � ������
;****************************************************************
FLReInicialize:
                        cmp     FLCurTaskID,_CONFG_ID
                        jne     FLReInicialNOT_CFG
                        ; ���� ������� ���䨣����
                        push    es
                        mov     es, SS:SEG_AX
                        xor     edi, edi                        ; ��⠭�������� ��
                        mov     eax, _CONFG_ID                  ;
                        @GetOfCurConf   0, esi
                        mov     ecx, (OFFSET END_KONF_FLASH)    ; ᫮�
                        shr     ecx, 1

                        CALL    FL_READ_FUNC
                        pop     es
                        jmp     FLReInicial_EX
     FLReInicialNOT_CFG:
                        cmp     FLCurTaskID,_DOPIN_ID
                        jne     FLReInicial_EX
                        ; ���� ������� ���.����
                        push    es
                        mov     es, SS:SEG_BLK_LST
                        xor     edi, edi                        ; ��⠭�������� ��
                        mov     eax, _DOPIN_ID                  ;
                        @GetOfCurConf   0, esi
                        mov     ecx, 40000h                     ; ᫮�
                        CALL    FL_READ_FUNC
                        pop     es
FLReInicial_EX:
                        mov     FLCurTaskID,_NOP_ID             ; ���㫨�
                        ret
;****************************************************************
; ��१����� ���䨣��樨 ��� ��� ���ଠ樨 �� ����� � 䫥�
;****************************************************************
FLReWrite:
                        cmp     FLCurTaskID,_CONFG_ID
                        jne     FLReWriteNOT_CFG
                        ; ���� ��े������ ���䨣����
        SAVE_CONF:
                        push    ds
                        mov     ds, SS:SEG_AX

                        xor     esi, esi                        ; ��⠭�������� ��
                        mov     FLCurTaskID,_CONFG_ID
                        mov     eax, _CONFG_ID                  ;
                        @GetOfCurConf   1, edi
                        mov     ecx, (OFFSET END_KONF_FLASH)    ; ᫮�
                        shr     ecx, 1

                        CALL    FL_WRITE_FUNC


                        jmp     FLReWrite_EX
     FLReWriteNOT_CFG:
                        cmp     FLCurTaskID,_DOPIN_ID
                        jne     FLReWrite_EX
                        ; ���� ��े������ ���.����
                        push    ds
                        mov     ds, SS:SEG_BLK_LST

                        xor     esi, esi                        ; ��⠭�������� ��
                        mov     FLCurTaskID,_CONFG_ID
                        mov     eax, _DOPIN_ID                  ;
                        @GetOfCurConf   1, edi
                        mov     ecx, 40000h                     ; ᫮�

                        CALL    FL_WRITE_FUNC
       FLReWrite_EX:
                        call    FLChangeActCopy                 ; �ਭ﫨
                        call    FLIndicate
                        mov     FLCurTaskID,_NOP_ID             ; ���㫨�
                        pop     ds
                        ret
FLInicialize            ENDP
;***********************************************************************
;�ᯮ����⥫쭠� �-�, ��।������ ��⨢��� �����
;FLCurTaskID - ID, ����� �⨬ ���������
;***********************************************************************
FLCalcCurActCopyB	PROC
                        push    eax ebx

			movzx	eax, BYTE PTR FLCurTaskID	; ����騩 ID
			; �஢�ਬ 1-� �����
                        mov     esi, FLFirstCopyAddrD[eax*4]    ; ��砫�
			mov	ebx, FLLablOffsetD[eax*4]	; ᬥ饭��
			add	esi, ebx			; 楫���� ���� �� ��砫�
			mov	edi, OFFSET FLTmpLabel		; ᬥ饭�� dst
			mov	ecx, 1				; 1 ᫮��
                        CALL    FL_READ_FUNC                    ; ��⠥�
                        mov     bx, FLActvCopyLabelW[eax*2]     ;
			cmp	WORD PTR ES:FLTmpLabel, bx	; �ࠢ���
			jne	FLCalcCurActCopyB_2
                        mov     FLCurActCopyB[eax],0
			jmp	FLCalcCurActCopyB_EX		;
     FLCalcCurActCopyB_2:
			mov	esi, FLSecndCopyAddrD[eax*4]	; ��砫�
			mov	ebx, FLLablOffsetD[eax*4]	; ᬥ饭��
			add	esi, ebx			; 楫���� ���� �� ��砫�
			mov	edi, OFFSET FLTmpLabel		; ᬥ饭�� dst
			mov	ecx, 1				; 1 ᫮��
			CALL	FL_READ_FUNC			; ��⠥�
                        mov     bx, FLActvCopyLabelW[eax*2]     ;
			cmp	WORD PTR ES:FLTmpLabel, bx	; �ࠢ���
			jne	FLCalcCurActCopyB_3
                        mov     FLCurActCopyB[eax],1
			jmp	FLCalcCurActCopyB_EX		;
     FLCalcCurActCopyB_3:
     FLCalcCurActCopyB_EX:
                        pop     ebx eax
                        ret
FLCalcCurActCopyB	ENDP
;***********************************************************************
; �������� ����஢ ����� �� �࠭� - �ய��⨬
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
; ������ (���⠭����, 㤠����� �⨪�⮪), ᬥ�� ��⨢���� �����
; FLCurTaskID - ID, � ����� ࠡ�⠥�
;***********************************************************************
FLChangeActCopy         PROC
                        pushad
                        push    ds
                        push    es
                        mov     ds, SS:SEG_AX                   ; ������� ������
                        mov     es, SS:SEG_AX                   ; ������� ������

                        movzx   eax, BYTE PTR FLCurTaskID
                        ; �஢�ਬ �㦭� �� �⠢��� �⨪���
                        @GetOfCurConf   1, esi                  ; 1! ��砫� १�ࢭ��
                        mov     ebx, FLLablOffsetD[eax*4]       ; ᬥ饭��
                        add     esi, ebx                        ; 楫���� ���� �� ��砫�
                        mov     edi, OFFSET FLTmpLabel          ; ᬥ饭�� dst
                        mov     ecx, 1                          ; 1 ᫮��
                        CALL    FL_READ_FUNC                    ; ��⠥�

                        cmp     FLTmpLabel, 0ffffh              ; ��� 㦥 ����ᠭ�
                        jne     FLChangeAct_NWR_LBL
                        ; ���⠢�� �⨪���
                        movzx   eax, BYTE PTR FLCurTaskID
                        mov     dx, FLActvCopyLabelW[eax*2]
                        mov     FLTmpLabel, dx                  ; �⨪�⪠

                        @GetOfCurConf   1, edi                  ; 1!��砫� १�ࢭ��
			mov	ebx, FLLablOffsetD[eax*4]	; ᬥ饭��
                        add     edi, ebx                        ; 楫���� ���� �� ��砫�
                        mov     esi, OFFSET FLTmpLabel          ; ᬥ饭�� dst
			mov	ecx, 1				; 1 ᫮��
                        CALL    FL_WRITE_FUNC                   ; �����뢠��
                        ; ��६ ����� �⨪���
     FLChangeAct_NWR_LBL:
                        movzx   eax, BYTE PTR FLCurTaskID
                        mov     FLTmpLabel, 0000                ; �⨪��� ������ 0

                        @GetOfCurConf   0, edi                  ; ��砫� ⥪�饩
			mov	ebx, FLLablOffsetD[eax*4]	; ᬥ饭��
                        add     edi, ebx                        ; 楫���� ���� �� ��砫�

                        mov     esi, OFFSET FLTmpLabel          ; ᬥ饭�� dst
			mov	ecx, 1				; 1 ᫮��
                        CALL    FL_WRITE_FUNC                   ; �����뢠��
                        ; ��࠭�� ���ଠ�� � ����� �����
                        mov     dl, FLCurActCopyB[eax]          ; ⥪��� ���䨣����
                        and     dl, 1                           ; ���� 0, ���� 1
                        xor     dl, 1                           ; ���塞
                        mov     FLCurActCopyB[eax], dl          ; ⥪��� ���䨣����
                        ; �⮡ࠧ�� ��
                        call    FLIndicate
                        pop     es
                        pop     ds
                        popad
                        ret
FLChangeActCopy         ENDP

; <<<<<<<<<<<<<<<<<<<<<<<<< �ࠩ��� Elan410 flash 29DL32(LV16) >>>>>>>>>>>>>>>>>>>>>>>>>>>>
;***********************************************************************
; �⥭�� 䫥� Elan'a; esi - ���� �� ��砫� 䫥�
; ecx - ������⢮ ᫮� � �⥭��; es:edi - ���� dst
;***********************************************************************
ELAN400_READ_FLC        PROC
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
                        movs    word ptr es:[edi], word ptr ds:[esi]
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
                        ;mov     ax, di
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
                        pop     eax
                        pop     edi
                        pop     es                          ; Error exit
                        stc
                        ret
            _Saved_OK:
                        pop     edi
            _Nt_Save:   ; �� ���� �����뢠��
                        add     edi, 2                      ; ᤢ����
		        DB      66h
                        loop    ELAN400_WRITE_FLC_LOOP      ; ������騩 ����
                        ; ��ଠ��� ��室
ELAN400_WRITE_EX:
                        pop     eax
                        pop     es
                        clc
                        ret
;***********************************************************************
;����㭪�� ��࠭�� ᥪ�� (64��) (����� ࠡ���� ��� ��檨�)
;edi- ���� �� ��砫� 䫥�
;***********************************************************************
ELAN400_ERASE_FLC:
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
                        ; �஢��塞 ����� �����
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
                        mov     es, _FLASH1_SEG     ; ������� 䫥�

ELAN400_WRITE_FLC_LOOP_X:
                        ; �஢�ਬ, ���� �� �����
                        test    edi, 0ffffh          ; ���� ��⥭ 64��
                        jnz     _Nt_Erse_X             ; �� ����
                        call    ELAN400_ERASE_FLC_X    ; �����
            _Nt_Erse_X:
		        DB    	67h
                        lodsw
                        cmp     ax, 0ffffh
                        je      _Nt_Save_X
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
                        ;mov     ax, di
                        and     edi, 07fffh
                        MOV     ES:[di], AX
                        ; �஢�ઠ �����
            _Test_loop_X:
                        mov     dx, ES:[di]
                        cmp     al, dl
                        je      _Saved_OK_X                   ; ����ᠫ���
                        test    dl, 20h
                        jz      _Test_loop_X                  ; �� ��室�� �⠤��
                        mov     dx, ES:[di]
                        cmp     al, dl
                        je      _Saved_OK_X                   ; ����ᠫ���
                        ;pop     eax
                        pop     edi
                        pop     eax
                        pop     es                          ; Error exit
                        pop     ds
                        stc
                        ret
            _Saved_OK_X:
                        pop     edi
            _Nt_Save_X:   ; �� ���� �����뢠��
                        add     edi, 2                      ; ᤢ����

		        DB      66h
                        loop    ELAN400_WRITE_FLC_LOOP_X      ; ������騩 ����
                        ; ��ଠ��� ��室
ELAN400_WRITE_EX_X:
                        pop     eax
                        pop     es
                        pop     ds
                        clc
                        ret
;***********************************************************************
;����㭪�� ��࠭�� ᥪ�� (64��) (����� ࠡ���� ��� ��檨�)
;edi- ���� �� ��砫� 䫥�
;***********************************************************************
ELAN400_ERASE_FLC_X:
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
                        ; �஢��塞 ����� �����
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
        MOV     AH,BL                           ;� AH - ����
	OUT	22h,AX 				;���� 22h/23h ������ 32h
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

; *********************************     ����� ������������ �� FLASH
FIND_CONF_ELAN  PROC   NEAR
        PUSHAD
        MOV     AX,WIN_ETIC[2]                  ;� AH - ���� �⨪�⪨ 1-� �����
        SHL     AX,8
        MOV     AL,INDEX_FLASH
	OUT	22h,AX                          ;�롮� ���� �����
        PUSH    DS
        MOV     DS,SS:SEG_FLASH			;\
        MOV     SI,ADR_ID                       ; |�⥭�� ��䨪�
        LODSW                                   ;/
        POP     DS
        CMP     AX,0BDEFh
        JNE     SHORT   NO_F_CE
        MOV     CURR_CONF,2
        POPAD
        RETN
NO_F_CE:
        MOV     AX,WIN_ETIC[4]                  ;� AH - ���� �⨪�⪨ 2-� �����
        SHL     AX,8
        MOV     AL,INDEX_FLASH
	OUT	22h,AX                          ;�롮� ���� �����
        PUSH    DS
        MOV     DS,SS:SEG_FLASH			;\
        MOV     SI,ADR_ID                       ; |�⥭�� ��䨪�
        LODSW                                   ;/
        POP     DS
        CMP     AX,0BDEFh
        JNE     SHORT   NO_S_CE
        MOV     CURR_CONF,4
NO_S_CE:POPAD
        RETN
FIND_CONF_ELAN  ENDP

; **********************************    ������ ������������ � FLASH
; � EDX - ����� ᥣ���� ��� �⥭��
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
        SHR     CX,1       ;/  /2 ??? �⮣� 42d
        MOV     DS,SS:SEG_FLASH
        MOV     ES,SS:SEG_AX
        MOV     AL,INDEX_FLASH
	MOV	AH,BL				;� AH - ����
	OUT	22h,AX 				;���� 22h/23h ������ 32h
        MOV     SI,0
RD_CF5: LODSW        ;\ 5t ����� �ᯮ�짮���� MOVSW 7t, ������ ���� ����॥ ��� MOVSD
        STOSW        ;/ 4t                      �먣��� 2t                       many t
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




