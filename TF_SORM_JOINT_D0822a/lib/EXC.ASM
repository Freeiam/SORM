EXC_0:
        mov     SS:ex.ereg_number, 0h
        jmp     EXEPTION_WORK
EXC_1:
        mov     SS:ex.ereg_number, 1h
        jmp     EXEPTION_WORK
EXC_2:
        mov     SS:ex.ereg_number, 2h
        jmp     EXEPTION_WORK
EXC_3:
        mov     SS:ex.ereg_number, 3h
        jmp     EXEPTION_WORK
EXC_4:
        mov     SS:ex.ereg_number, 4h
        jmp     EXEPTION_WORK
EXC_5:
        mov     SS:ex.ereg_number, 5h
        jmp     EXEPTION_WORK
EXC_6:
        mov     SS:ex.ereg_number, 6h
        jmp     EXEPTION_WORK
EXC_7:
        mov     SS:ex.ereg_number, 7h
        jmp     EXEPTION_WORK
EXC_8:
        mov     SS:ex.ereg_number, 8h
        jmp     EXEPTION_WORK
EXC_9:
        mov     SS:ex.ereg_number, 9h
        jmp     EXEPTION_WORK
EXC_A:
        mov     SS:ex.ereg_number, 0Ah
        jmp     EXEPTION_WORK
EXC_B:
        mov     SS:ex.ereg_number, 0Bh
        jmp     EXEPTION_WORK
EXC_C:
        mov     SS:ex.ereg_number, 0Ch
        jmp     EXEPTION_WORK
EXC_D:
        mov     SS:ex.ereg_number, 0Dh
        jmp     EXEPTION_WORK
EXC_E:
        mov     SS:ex.ereg_number, 0Eh
        jmp     EXEPTION_WORK
EXC_F:
        mov     SS:ex.ereg_number, 0Fh
        jmp     EXEPTION_WORK
EXC_10:
        mov     SS:ex.ereg_number, 10h
        jmp     EXEPTION_WORK
EXC_11:
        mov     SS:ex.ereg_number, 11h
        jmp     EXEPTION_WORK
EXC_12:
        mov     SS:ex.ereg_number, 12h
        jmp     EXEPTION_WORK
EXC_13:
        mov     SS:ex.ereg_number, 13h
        jmp     EXEPTION_WORK
EXC_14:
        mov     SS:ex.ereg_number, 14h
        jmp     EXEPTION_WORK
EXC_15:
        mov     SS:ex.ereg_number, 15h
        jmp     EXEPTION_WORK
EXC_16:
        mov     SS:ex.ereg_number, 16h
        jmp     EXEPTION_WORK
EXC_17:
        mov     SS:ex.ereg_number, 17h
        jmp     EXEPTION_WORK
EXC_18:
        mov     SS:ex.ereg_number, 18h
        jmp     EXEPTION_WORK
EXC_19:
        mov     SS:ex.ereg_number, 19h
        jmp     EXEPTION_WORK
EXC_1A:
        mov     SS:ex.ereg_number, 1Ah
        jmp     EXEPTION_WORK
EXC_1B:
        mov     SS:ex.ereg_number, 1Bh
        jmp     EXEPTION_WORK
EXC_1C:
        mov     SS:ex.ereg_number, 1Ch
        jmp     EXEPTION_WORK
EXC_1D:
        mov     SS:ex.ereg_number, 1Dh
        jmp     EXEPTION_WORK
EXC_1E:
        mov     SS:ex.ereg_number, 1Eh
        jmp     EXEPTION_WORK
EXC_1F:
        mov     SS:ex.ereg_number, 1Fh
EXEPTION_WORK:
        ; --- ���������� ॣ�����
        mov     SS:ex.ereg_eax, eax
        mov     SS:ex.ereg_ebx, ebx
        mov     SS:ex.ereg_ecx, ecx
        mov     SS:ex.ereg_edx, edx
        mov     SS:ex.ereg_esi, esi
        mov     SS:ex.ereg_edi, edi
        mov     SS:ex.ereg_ebp, ebp
        mov     SS:ex.ereg_esp, esp

        mov     SS:ex.ereg_ss, ss
        mov     SS:ex.ereg_ds, ds
        mov     SS:ex.ereg_es, es
        mov     SS:ex.ereg_fs, fs
        mov     SS:ex.ereg_gs, gs
        ; --- ��।��塞 ����稥 ���� �訡��
        cmp     ss:ex.ereg_number, 8
        je      SHORT Excp_Get_ECode
        cmp     ss:ex.ereg_number, 17
        je      SHORT Excp_Get_ECode
        cmp     ss:ex.ereg_number, 10
        jb      SHORT Excp_Get_IP
        cmp     ss:ex.ereg_number, 14
        ja      SHORT Excp_Get_IP
        ; --- ᭨���� � �⥪� ��� �訡��
Excp_Get_ECode:
        pop     eax
        mov     ss:ex.ereg_kod, eax
        ; --- ᭨���� � �⥪� IP, CS � EFLAGS
Excp_Get_IP:
        ; --- ����襬 TRACE_CODE
        mov     eax, _ss_code
        mov     ss:ex.trace_code, eax
        ; ------
        pop     eax
        mov     ss:ex.ereg_eip, eax
        pop     eax
        mov     ss:ex.ereg_cs, ax
        pop     eax
        mov     ss:ex.ereg_flg, eax
        ; --- ��࠭�� �⥪ � ��������
        push    ss
        pop     es
        mov     cx, _exc_max_stack
        xor     si, si
        mov     di, offset ex.ereg_stack
exc_stack_save:
        cmp     sp, SS_SIZE - 2
        ja      exc_stack_save_exit
        inc     si
        pop     ax
        stosw
        loop    exc_stack_save
exc_stack_save_exit:
        mov     ss:ex.ereg_count, si
        ; --- ��࠭�� �������� � ������
                mov     es, ss:SEG_BABA                 ;\
                mov     di, _BABA_FOR_EXC * 512         ;|
                mov     byte ptr es:[di],     'D'       ;|  ��࠭���� � ����
                mov     byte ptr es:[di + 1], 'E'       ;|  �������� �᪫�祭��
                mov     byte ptr es:[di + 2], 'B'       ;|  �� - �� �� ����㧪� ���
                mov     cx, _exception_data_l           ;|  ��� �� ��ᬮ���� �� �뫮
                add     di, 3                           ;|
                mov     si, offset ex                   ;|
                rep     movs byte ptr es:[di], ss:[si]  ;/

;if _PUMA eq TRUE
;        db 9ah
;        dw offset Initialize
;        dw _puma_cod
;endif
        jmp    reboot_exc

        ; --- �뢮� ����
        call    printEXC
        mov     ax, time_2mc
        mov     ss:time_reboot, ax
        sti

eternal_loop:

        mov     ax, time_2mc
        sub     ax, ss:time_reboot
        cmp     ax, _time_reboot
        jb      short   eternal_loop

        mov     ax, time_2mc
        mov     ss:time_reboot, ax

        inc     ss:ereg_reboot
        cmp     ss:ereg_reboot, _ereg_max_reeboot
        ja      reboot_exc

        mov     es, ss:seg_fs
        movzx   eax, ss:ereg_reboot

        mov     ebx, _ereg_reboot - 1
        shl     eax, 1
        add     ebx, eax
        mov     al, 07h
        mov     es:[ebx], al

        jmp     short   eternal_loop
; --- ��१���㧪� �����
reboot_exc:
;        MOV     AL,0FH
;        OUT     70H,AL
;        MOV     AL,0
;        OUT     71H,AL
;        MOV     AL,0FEH
;        OUT     64H,AL
;        HLT

         mov    DOS_Flag, 0
reboot:
IF _MSP EQ TRUE
                MOV     ES,SS:SEG_ST
                mov     BYTE PTR es:[COM_MODE], 0F0h    ; ����� ��������
                mov     al, 255         ;\ ���஥� ���뢠���, ⥬ ᠬ�
                out     0a1h, al        ;/ �맮��� ��१���㧪�
ELSE
                mov     es, ss:SEG_GS
                mov     es:[440ah], BYTE PTR 0
                mov     es:[5871h], BYTE PTR 0
        ; ===========================================================================
        CLD
        CLI                                     ;\
        MOV     AL,80h                          ;|����� �������� ���뢠���
	OUT	70h,AL				;/� NMI
;
	MOV	AL,11h				;\
	OUT	20h,AL				;|
	MOV	AL,8				;|
	OUT	21h,AL				;|
	MOV	AL,4				;|
	OUT	21h,AL				;|
	MOV	AL,1				;|
	OUT	21h,AL				;\�ணࠬ��஢���� ����஫-
	MOV	AL,11h				;/��஢ ���뢠���
	OUT	0A0h,AL				;|
	MOV	AL,70h				;|
	OUT	0A1h,AL				;|
	MOV	AL,2				;|
	OUT	0A1h,AL				;|
	MOV	AL,1				;|
	OUT	0A1h,AL				;/
;
        MOV     DS,SS:SEG_AX
        MOV     AL,DOS_mask_0
	OUT	21h,AL
        MOV     AL,DOS_mask_1
	OUT	0A1h,AL
	MOV	AL,20h
	OUT	20h,AL
	OUT	0A0h,AL
;
	MOV	WORD PTR PDESCR,3FFh		;\
	MOV	DWORD PTR PDESCR[2],0		;|�����⮢�� � ����㧪� ⠡-
	LIDT	PDESCR				;/���� ���ਯ�஢ ���뢠���
;
        MOV     DESCR_SORM.LIM,0FFFFh           ;��⠭���� �।��� ᥣ. START
        MOV     DESCR_SEG_AX.LIM,0FFFFh         ;��⠭���� �।��� ᥣ. START
        MOV     AX,8                            ;\
	MOV	DS,AX				;|
	MOV	SS,AX				;\����㧪� ᥫ���஢
	MOV	ES,AX				;/ᥣ���⮢
	MOV	FS,AX				;|
	MOV	GS,AX				;/
;
	DB	0EAh				;\
	DW	OFFSET SBROS12			;|JMP FAR CS:SBROS12
        DW      120                             ;/
SBROS12:
        MOV     EAX,CR0                         ;�⥭�� ����� ����ﭨ� ��設�
	AND	AL,0FEh				;<<< ������ ०�� >>>
	MOV	CR0,EAX				;������ ����� ����ﭨ� ��設�
;
	DB	0EAh				;\
	DW	OFFSET SBROS11			;|JMP FAR CS:SBROS11
        DW      CODE_AXS                        ;/
SBROS11:
        mov     ax, STACK_AXS
        mov     ss, ax
;
        MOV     AX,DATA_AXS             ; �᭮���� DATA ᥣ����
        MOV     DS,AX                   ; ����ந���
        mov     es, ax
        CMP     DOS_Flag, 1
        JE      GoTo_DOS
;
	DB	0EAh
	DW	0FFF0h
	DW	0F000h
;
GoTo_DOS:
        STI                                     ;\
	MOV	AL,0				;|����襭�� ��������
	OUT	70h,AL				;/���뢠��� � NMI
;
        MOV     AX,3
	INT	10h
	MOV	AX,4C00h
	INT	21h
        ; ===========================================================================
ENDIF

stop_block:   jmp     short     stop_block

; ----=> printEXC <=-----------------------
; Created by Smaller on 09/12/04 2:28
; �뢮��� ���� �� ������� �᪫��⥫쭮� ���樨
printEXC        PROC
        mov     cx, 11;;;10
        mov     ah, 70h                 ; 梥� ����
        mov     si, offset exc_window
        mov     di, _ereg_win
@@exc_win_loop:
        call    EprintS
        add     si, 38
        add     di, 80*_x
        loop    @@exc_win_loop

        mov     cx, _exc_max_line
@@exc_win_loop_stk:
        call    EprintS
        add     di, 80*_x
        loop    @@exc_win_loop_stk

        mov     si, offset exc_window_stack_e
        call    EprintS

        ; --- �뢮��� ��६���� �� ��࠭
        mov     cx, 8

        mov     ebx, ss:ex.ereg_eax
        mov     di, _ereg_eax
        call    EprintH
        mov     ebx, ss:ex.ereg_ebx
        mov     di, _ereg_ebx
        call    EprintH
        mov     ebx, ss:ex.ereg_ecx
        mov     di, _ereg_ecx
        call    EprintH
        mov     ebx, ss:ex.ereg_edx
        mov     di, _ereg_edx
        call    EprintH
        mov     ebx, ss:ex.ereg_esi
        mov     di, _ereg_esi
        call    EprintH
        mov     ebx, ss:ex.ereg_edi
        mov     di, _ereg_edi
        call    EprintH
        mov     ebx, ss:ex.ereg_ebp
        mov     di, _ereg_ebp
        call    EprintH
        mov     ebx, ss:ex.ereg_esp
        mov     di, _ereg_esp
        call    EprintH

        mov     ebx, ss:ex.ereg_eip
        mov     di, _ereg_eip
        call    EprintH
        mov     ebx, ss:ex.ereg_kod
        mov     di, _ereg_kod
        call    EprintH
        mov     ebx, ss:ex.ereg_flg
        mov     di, _ereg_flg
        call    EprintH

        shr     cx, 1
        mov     bx, ss:ex.ereg_cs
        mov     di, _ereg_cs
        call    EprintH
        mov     bx, ss:ex.ereg_ds
        mov     di, _ereg_ds
        call    EprintH
        mov     bx, ss:ex.ereg_es
        mov     di, _ereg_es
        call    EprintH
        mov     bx, ss:ex.ereg_fs
        mov     di, _ereg_fs
        call    EprintH
        mov     bx, ss:ex.ereg_gs
        mov     di, _ereg_gs
        call    EprintH
        mov     bx, ss:ex.ereg_ss
        mov     di, _ereg_ss
        call    EprintH

        shr     cx, 1
        push    ax
        mov     ah, 0F0h
        mov     bl, ss:ex.ereg_number
        mov     di, _ereg_number
        call    EprintH
        pop     ax
        ; --- <Paul> -- {06/12/2012}
        ; ----- [output TRACE CODE]
        mov     cx, 8
        mov     ebx, ss:ex.trace_code
        mov     di, _ereg_trace
        call    EprintH
        ; --- <Paul> -- {END}
        ; --- �뢮� �⥪�
        mov     si, ss:ex.ereg_count
        mov     di, _ereg_stack
        mov     cx, 4
        mov     dx, _exc_max_line
        mov     bp, offset ex.ereg_stack
@@exc_stroka:
        push    dx
        mov     dx, _exc_max_stolb
@@exc_stolbec:
        dec     si
        js      @@exc_exit_stk
        mov     bx, ss:[bp]
        call    EprintH
        inc     bp
        inc     bp
        add     di, 5*_x
        dec     dx
        jns     @@exc_stolbec
        add     di, 45*_x;80*_x - (_exc_max_stolb + 1) * 5 * _x;45*_x
        pop     dx
        dec     dx
        jns     @@exc_stroka
        ret
@@exc_exit_stk:
        pop     dx
        ret
printEXC        ENDP



; ebx - �᫮
; ah - ��ਡ��
; cl - ᪮�쪮 �뢮���� ��ࠤ (1 - 8)
; (es:)di - �㤠 �뢮����
EprintH          proc
        jcxz    @@exit
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
        ret
EprintH          endp


; SI - ��㤠
; DI - �㤠         ��ப� ������ ����稢����� �㫥�
; AH - ��ਡ��
EprintS          PROC
        push    si di ax es
        mov     es, ss:SEG_FS

@@loop:
        cmp     di, _ScreenSize - 2
        ja      @@exit
        lods    byte ptr ss:[si]
        cmp     al, 0
        je      @@exit
        stosw
        jmp     @@loop
@@exit:
        pop     es ax di si
        ret
EprintS          ENDP

; ----=> printLastExcSeg <=-----------------------
; Created by Smaller on 09/12/04 2:33
; �����誠 ��� �맮�� �ᮢ���� �� ��㣮�� ᥣ����
printLastExcSeg PROC FAR
        pushad
        push    ds es fs gs
        call    printEXC
        pop     gs fs es ds
        popad
        ret
printLastExcSeg ENDP

