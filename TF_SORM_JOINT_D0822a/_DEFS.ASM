;  䠩� ��� ��।������ ��������� �������樨, � ⠪�� ����⠭��
;*****************************************************************


P486
LOCALS  @@

; ��।������ ���
NULL                    equ     0
TRUE                    =       1
true                    =       TRUE
FALSE                   equ     NULL
false                   equ     FALSE

                        ; ���譠� ��������
_BABA_TO_CTO            EQU     15              ;
_BABA_FROM_CTO          EQU     14              ;
IF _MSP NE TRUE
_BABA_FOR_EXC           EQU     0               ; ����� ���� �ᯮ��㥬� ��� ��࠭���� exception
ELSE
_BABA_FOR_EXC           EQU     31               ; ����� ���� �ᯮ��㥬� ��� ��࠭���� exception
ENDIF
_BABA_FOR_CP            EQU     30               ; ����� ���� �ᯮ��㥬� ��� ��࠭���� ����஫쭮� �窨
IF _MSP EQ TRUE
; === ⨯� ��������� � ����묨 ࠡ�⠥� �ணࠬ�� ===========================
;
_TYPE_BUN               = 4
_TYPE_MONO              = 3
_TYPE_STANDART          = 1
_TYPE_OLD               = 0
ENDIF
; === �᭮��� ��ࠬ���� �������樨 ===========================================
;
;_SORM_NO_MODEM          equ     TRUE;;;FALSE;;;
_C_NO_MODEM_MONO        equ     TRUE;;;FALSE
;IF _MSP EQ TRUE
;_SORM_MONO              equ     TRUE ;FALSE;;;
;ENDIF
; --- �롮� ࠡ�祣� E1 - 0, 32, 64, 96
_nom_ikm                equ     0 ;32;;;           <- {Paul.N -- 08/12/14} - ���ﭮ�� 2-�� ���.
IF _MSP EQ TRUE
; --- ��� ��� �� ���
_BUN                    equ     FALSE
ENDIF
; --- ᯮᮡ ࠧꥤ������ ��䨪��஬ ᮥ�������: 235/169 ��� ���� 169
;
_NEW_REL_FROM_TS        EQU     TRUE;FALSE;

 ; --- ��।������ ��஭� LAPB
_DCE                    EQU     0
_DTE                    EQU     1
_USER                   EQU     _DCE;;;_DTE;

; --- �������⥫쭠� �⫠��� ��
;
_AUX_MO_DEBUG           EQU     false;;;TRUE;

; --- ���� �� �ॡ���� �������᪨� �����
;
_NO_AL_JITTER           EQU     TRUE

; --- ��⮪���஢���� ⮫쪮 ������ �������
;
_ONLY_ONE_PROTOCOL      EQU     TRUE

IF _MSP NE TRUE
; --- ��।������ ०���� �⫠��� - ⮫쪮 ���� ����⠭� ����� ���� TRUE
;
_DEBUG_NO		EQU	TRUE;FALSE;
_DEBUG_KSL              EQU     FALSE;TRUE;
_DEBUG_OBJ              EQU     FALSE;TRUE;
; ---
_DEBUG_DD               EQU     FALSE;TRUE;
_DEBUG_COMM		EQU	FALSE;TRUE;
ENDIF


IF _MSP NE TRUE
; MVT �����஢���� � 䠩�
_debugFile		=	false
_isDebugBuffered 	=	true
_DebugBufferSize	=	0FFFh
_DebugWriteSize		=	100
_DebugFileMaxSize	equ	1000000
; ������騥 ��६���� ����� ������ true, ⮫쪮 �᫨ _debugFile ���� true
_DebugKonfig		=	false
; MVT �����஢���� ���
_debug_Puma		=	false
ENDIF


; === �⮡ࠦ���� �᭮���� ��ࠬ��஢ �������樨 �� ��࠭� ====================
;
@MODEM_STR      MACRO
        if _SORM_NO_MODEM
                DB      '- no modem -'
        else
                DB      '- m o d e m-'
        endif
                ENDM

@BLOCK_STR      MACRO
        if _SORM_NO_MODEM
IF _MSP EQ TRUE
                if _SORM_MONO
                        DB      '- �������� -'
                else
                        DB      '- �⠭���� -'
                endif
ELSE
                if _C_NO_MODEM_MONO
                        DB      '- w h i t e-'
                else
                        DB      '- b l a c k-'
                endif
ENDIF
        else
                DB      '            '
        endif
                ENDM

@PRMS_STR       MACRO
        if _NEW_REL_FROM_TS
                DB      '-235;'
        else
                DB      '-169;'
        endif
        if _USER EQ _DCE
                DB      'DCE;'
        else
                DB      'DTE;'
        endif
        if _AUX_MO_DEBUG
                DB      'MO-'
        else
                DB      '  -'
        endif
                ENDM

IF _MSP NE TRUE
; - - - - - - - - - - - - - - -  H D D - - - - - - - - - - - - - - - - - - - -
;
CALL_HDD        MACRO   PROCNAME, params
                local   param_num
                param_num = 0
                irp param, <&params&>
                push    param
                param_num = param_num + 2
                endm
                DB      9Ah
                DW      OFFSET PROCNAME
                DW      336
                if param_num ne 0
                add     sp,param_num
                endif
                ENDM

; - - - - - - - - - - - - - - -  � � � � - - - - - - - - - - - - - - - - - - -
;
CALL_PLL	MACRO	PROCNAME
                DB      9Ah
                DW      OFFSET PROCNAME
                DW      352
ENDM
ENDIF
; 梥�
cBLACK  = 00h ; ���
cBLUE   = 01h ; ᨭ��
cGREEN  = 02h ; ������
cCYAN   = 03h ; ���㡮�
cRED    = 04h ; ����
cMAGNETA= 05h ; ...
cBROWN  = 06h ; ���筥��
cWHITE  = 07h ; ����
cGRAY   = 08h ; ���
clBLUE  = 09h ; �ન� ᨭ��
clGREEN = 0Ah ; �ન� ������
clCYAN  = 0Bh ; �ન� ���㡮�
clRED   = 0Ch ; �ન� ����
clMAGENTA=0Dh ; ...
clYELLOW= 0Eh ; �����
clWHITE = 0Fh ; ��⥭ᨢ�� ����

_B_00   EQU     00000000000000000000000000000001b
_B_01   EQU     00000000000000000000000000000010b
_B_02   EQU     00000000000000000000000000000100b
_B_03   EQU     00000000000000000000000000001000b
_B_04   EQU     00000000000000000000000000010000b
_B_05   EQU     00000000000000000000000000100000b
_B_06   EQU     00000000000000000000000001000000b
_B_07   EQU     00000000000000000000000010000000b
_B_08   EQU     00000000000000000000000100000000b
_B_09   EQU     00000000000000000000001000000000b
_B_10   EQU     00000000000000000000010000000000b
_B_11   EQU     00000000000000000000100000000000b
_B_12   EQU     00000000000000000001000000000000b
_B_13   EQU     00000000000000000010000000000000b
_B_14   EQU     00000000000000000100000000000000b
_B_15   EQU     00000000000000001000000000000000b
_B_16   EQU     00000000000000010000000000000000b
_B_17   EQU     00000000000000100000000000000000b
_B_18   EQU     00000000000001000000000000000000b
_B_19   EQU     00000000000010000000000000000000b
_B_20   EQU     00000000000100000000000000000000b
_B_21   EQU     00000000001000000000000000000000b
_B_22   EQU     00000000010000000000000000000000b
_B_23   EQU     00000000100000000000000000000000b
_B_24   EQU     00000001000000000000000000000000b
_B_25   EQU     00000010000000000000000000000000b
_B_26   EQU     00000100000000000000000000000000b
_B_27   EQU     00001000000000000000000000000000b
_B_28   EQU     00010000000000000000000000000000b
_B_29   EQU     00100000000000000000000000000000b
_B_30   EQU     01000000000000000000000000000000b
_B_31   EQU     10000000000000000000000000000000b
; ����� �ᯮ�짮���� NOT _B_28 ��� ������஢����
M_OR_0          EQU     0000000000000001B
M_OR_1          EQU     0000000000000010B
M_OR_2          EQU     0000000000000100B
M_OR_3          EQU     0000000000001000B
M_OR_4          EQU     0000000000010000B
M_OR_5          EQU     0000000000100000B
M_OR_6          EQU     0000000001000000B
M_OR_7          EQU     0000000010000000B
M_OR_8          EQU     0000000100000000B
M_OR_9          EQU     0000001000000000B
M_OR_10         EQU     0000010000000000B
M_OR_11         EQU     0000100000000000B
M_OR_12         EQU     0001000000000000B
M_OR_13         EQU     0010000000000000B
M_OR_14         EQU     0100000000000000B
M_OR_15         EQU     1000000000000000B
M_AND_0         EQU     1111111111111110B
M_AND_1         EQU     1111111111111101B
M_AND_2         EQU     1111111111111011B
M_AND_3         EQU     1111111111110111B
M_AND_4         EQU     1111111111101111B
M_AND_5         EQU     1111111111011111B
M_AND_6         EQU     1111111110111111B
M_AND_7         EQU     1111111101111111B
M_AND_8         EQU     1111111011111111B
M_AND_9         EQU     1111110111111111B
M_AND_10        EQU     1111101111111111B
M_AND_11        EQU     1111011111111111B
M_AND_12        EQU     1110111111111111B
M_AND_13        EQU     1101111111111111B
M_AND_14        EQU     1011111111111111B
M_AND_15        EQU     0111111111111111B

BM_OR_0          EQU     00000001B
BM_OR_1          EQU     00000010B
BM_OR_2          EQU     00000100B
BM_OR_3          EQU     00001000B
BM_OR_4          EQU     00010000B
BM_OR_5          EQU     00100000B
BM_OR_6          EQU     01000000B
BM_OR_7          EQU     10000000B
BM_AND_0         EQU     11111110B
BM_AND_1         EQU     11111101B
BM_AND_2         EQU     11111011B
BM_AND_3         EQU     11110111B
BM_AND_4         EQU     11101111B
BM_AND_5         EQU     11011111B
BM_AND_6         EQU     10111111B
BM_AND_7         EQU     01111111B

IF _MSP NE TRUE
; --- ����� �⫠��筮� ����
;
@DEBUG_PRINT	MACRO
                LOCAL   @@dd1, @@dd2, @@Group, @@KSL
		LOCAL	@@objs, @@onext, @@segbunch
        if _DEBUG_DD EQ TRUE
		mov	ah, cBLUE * 16 + clMAGENTA
		; ---
		mov	cx, 15
		lea	si, _1st_dd
		mov	di, 61 * 2
	@@dd1:	push	cx
	        mov	ebx, [si]
		mov	cx, 8
		call	print_H
		pop	cx
		add	di, 160
		add	si, 4
		loop	@@dd1
		; ---
		mov	cx, 15
		lea	si, _16n_dd
		mov	di, 71 * 2
	@@dd2:	push	cx
	        mov	ebx, [si]
		mov	cx, 8
		call	print_H
		pop	cx
		add	di, 160
		add	si, 4
		loop	@@dd2
	endif
        if _DEBUG_COMM EQ TRUE
		mov	ah, cBLUE * 16 + clMAGENTA
		mov     di, 160 * 17 + 2 * 61
		mov	PEREM_DI, di
		mov	cx, 40
                mov     es, ss:SEG_ST
                mov     si, ADR_OZU_K1 + 256            ; � ���-30 ��
	@@comm:	push	cx
		mov	bl, es:[si]
		mov	bh, es:[si + 1]
		mov	cx, 4
		call	print_H
		pop	cx
		add	si, 2
		add	di, 160
		push	ax
		mov	ax, cx
		xor	dx, dx
		div	WORD PTR __VAL_10
		cmp	dx, 1
		jne	@@comm2
		add	PEREM_DI, 2 * 5
		mov	di, PEREM_DI
	@@comm2:pop	ax
		loop	@@comm
	endif
	if _DEBUG_KSL EQ TRUE
		mov	ah, cBLUE * 16 + clYELLOW
		xor	si, si
		mov	di, 330
	@@Group:push	di
		mov	bl, KSL_Group[si]
		mov	cx, 2
		call	print_H
		mov	bl, KSL_Group_Type[si]
		add	di, 8
		call	print_H
		mov	bl, KSL_Count[si]
		add	di, 8
		call	print_H
		mov	bl, KSL_Last[si]
		add	di, 8
		call	print_H
		pop	di
		add	di, 160
		inc	si
		cmp	si, _MAX_GROUP
		jb	@@Group
		; ---
		xor	si, si
		mov	di, 2 * 160 + 29 * 2
	@@KSL:  push	di
		mov	bl, KSLs[si]
		mov	cx, 2
		call	print_H
		mov	bl, KSL_A[si]
		add	di, 8
		call	print_H
		mov	bl, KSL_Pair[si]
		add	di, 8
		call	print_H
		mov	bl, KSL_Busy[si]
		add	di, 8
		call	print_H
		pop	di
		add	di, 160
		inc	si
		cmp	si, 32
		jb	@@KSL
		; ---
		xor	si, si
		mov	di, 2 * 160 + 50 * 2
	@@Mod:  push	di
                mov     bl, Modem_KSL[si]
		mov	cx, 2
		call	print_H
		pop	di
		add	di, 160
		inc	si
		cmp	si, _MAX_MODEM_TS
		jb	@@Mod
	endif
        if _DEBUG_OBJ EQ TRUE
		mov	ah, cBLUE * 16 + clYELLOW
                mov     cx, 20
                lea     si, Bunches
		mov	es, ss:SEG_OBJ
                mov     di, 160 * 1 + 2 * 8
        @@objs: push    cx
                push    di
                ; �᫮��� �����
                mov     bx, es:[si].obj_num
                mov     cx, 4
                call    print_H
                ; ⨯ ��ꥪ�
                mov     bl, es:[si].obj_type
                mov     cx, 2
                add     di, 160
                call    print_H
                ; �ਧ��� �����
                mov     bl, es:[si].obj_attr
                add     di, 160
                call    print_H
                ; ����� �����
                mov     bl, es:[si].obj_tel_len
                add     di, 160
                call    print_H
                ; ����� ⥫�䮭�
                mov     ebx, DWORD PTR es:[si].obj_tel_num
                mov     cx, 6
                add     di, 160
                call    print_H
                mov     ebx, DWORD PTR es:[si + 3].obj_tel_num
                add     di, 160
                call    print_H
                mov     ebx, DWORD PTR es:[si + 6].obj_tel_num
                add     di, 160
                call    print_H
                ; ����� ��窠
                mov     bx, es:[si].obj_bunch
                mov     cx, 4
                add     di, 160
                call    print_H
                ; ��⥣��� ����஫�
                mov     bl, es:[si].obj_cat_ctrl
                mov     cx, 2
                add     di, 160
                call    print_H
                ; ��㯯� ���
                mov     bl, es:[si].obj_gr_ksl
                add     di, 160
                call    print_H
                ; �ਮ���
                mov     bl, es:[si].obj_prior
                add     di, 160
                call    print_H
                ; 䫠��
                mov     bl, es:[si].obj_flags
                add     di, 160
                call    print_H
                ;
                pop     di
                pop     cx
                add     si, _OBJ_SIZE
                add     di, 7 * 2
                cmp     cx, 11
                jne     @@onext
                mov     di, 160 * 15 + 2 * 8
        @@onext:dec     cx
                jnz     @@objs
		; ---
                mov     cx, 40
                mov     esi, _BUNCH_PRM_OFFS
		mov	di, 160 * 28
                mov     es, ss:SEG_BUNCH
		cld
	@@segbunch:
		push	cx
		mov	bl, es:[esi]
		mov	cx, 2
		call	print_H
		pop	cx
		inc 	esi
		add	di, 4
		loop	@@segbunch
if 01
		mov	ah, cBLUE * 16 + clMAGENTA
		; ---
		lea	si, _1st_dd
;;;		mov	cx, 15
;;;		mov	di, 61 * 2
		mov	cx, 4
		mov	di, 160 * 29 + 61 * 2
	dd1:	push	cx
	        mov	ebx, [si]
		mov	cx, 8
		call	print_H
		pop	cx
		add	di, 160
		add	si, 4
		loop	dd1
		; ---
		lea	si, _16n_dd
;;;		mov	cx, 15
;;;		mov	di, 71 * 2
		mov	cx, 4
		mov	di, 160 * 29 + 71 * 2
	dd2:	push	cx
	        mov	ebx, [si]
		mov	cx, 8
		call	print_H
		pop	cx
		add	di, 160
		add	si, 4
		loop	dd2
endif
	endif
		ENDM


; --- �⫠��� �� ------------------------------------------------------
@TC_Debug       MACRO
                cmp     SOST_VIU5, 12 ; _TC_Ctrl        ;
                jne     TCD_Exit                        ;
                mov     es, ss:SEG_FS                   ;
                mov     ah, cBLUE * 16 + clWHITE        ;
if 01
                mov     gs, ss:SEG_OBJ                  ;
                mov     cx, 10                          ; 10 ����� ��������
                mov     di, 160 * 1 + 2 * 3             ;
                lea     si, Objects                     ;
TCD_Objects:    push    cx                              ;
                push    di                              ;
                mov     bx, gs:[si].obj_num             ;
                mov     cx, 4                           ;
                call    print_H                         ;
                mov     bl, gs:[si].obj_type            ;
                mov     cx, 2                           ;
                add     di, 10                          ;
                call    print_H                         ;
                mov     bl, gs:[si].obj_attr            ;
                add     di, 6                           ;
                call    print_H                         ;
                mov     bl, gs:[si].obj_tel_len         ;
                add     di, 6                           ;
                call    print_H                         ;
                mov     ebx, DWORD PTR gs:[si].obj_tel_num ;
                rol     bl, 4                           ;
                rol     bh, 4                           ;
		xchg	bl, bh                          ;
                rol     ebx, 16                         ;
                rol     bl, 4                           ;
                rol     bh, 4                           ;
		xchg	bl, bh				;
		mov	cx, 8				;
                add     di, 6                           ;
                call    print_H                         ;
                pop     di                              ;
                pop     cx                              ;
                add     di, 160                         ;
                add     si, _OBJ_SIZE                   ;
                loop    TCD_Objects                     ;
endif
                mov     cx, 32                          ; �뢮� ���ଠ樨 �� 32-� ���
                xor     si, si                          ;
                mov     di, 160 * 1 + 2 * 30            ;
TCD_KSL:        push    cx                              ;
                mov     bl, KSLs[si]                    ;
                mov     cl, 2                           ;
                call    print_H                         ;
                mov     bl, KSL_Pair[si]                ;
                add     di, 4                           ;
                call    print_H                         ;
                add     di, 160 - 4                     ;
                inc     si                              ;
                pop     cx                              ;
                loop    TCD_KSL                         ;

                mov     cx, _DEBUG_SOUND_KSL            ; �뢮� ���ଠ樨 �� ��
                lea     si, Modem_KSL                   ;
                mov     di, 160 * 1 + 2 * 35            ;
TCD_KSL_TH:     push    cx                              ;
                mov     cl, 2                           ;
                mov     bx, [si]                        ;
                call    print_H                         ;
                add     di, 160                         ;
                inc     si                              ;
                pop     cx                              ;
                loop    TCD_KSL_TH                      ;

                cmp     __sound_count, 28 * 2 + 28 * 2  ; 㦥 �뢥�� ���ଠ�� �� ������樨 ?
                jb      TCD_Comm_In                     ; ��� - �� �뢮� �室�饩 ������樨
                ; --- �뢮� ��।���� ���� �� "����"
                ;     �뢮� �㬨஢����� ������� ����
TCD_BABA:       mov     gs, ss:SEG_BABA                 ;
                mov     ax, __sound_count               ; �㫥��� ���� - �� 0-�� ���� ��室��� ������樨
                sub     ax, 28 * 2 + 28 * 2             ; ⥯��� �㬥��� ���⮢ ���� � ���
                shr     ax, 4                           ; �⠥� �� ���� ����ﬨ �� 16 ����
                push    ax
                xor     dx, dx                          ;
                div     WORD PTR __VAL_3                ;
                mov     si, ax                          ;
                shl     si, 9                           ; ����� � si ����砥� ᬥ饭�� ��砫� ������ ����
                or      dx, dx                          ; ���樨 0, 3, 6... �⠥� � 0..15 ������� ����
                jz      TCD_BABA_si                     ;
                add     si, 512 * 16                    ;
                cmp     dx, 1                           ; ���樨 1, 4, 7... �⠥� � 16..31 ������ ����
                je      TCD_BABA_si                     ;
                add     si, 512 * 16                    ; ���樨 2, 5, 8... �⠥� � 32..47 ������ ����
TCD_BABA_si:    pop     ax
                inc     ax                              ;
                mul     WORD PTR __VAL_160              ;
                mov     di, ax                          ; ����� 16-�� ���� ������� � ᢮�� ��ப�
                mov     ax, __sound_count               ;
                and     ax, 0fh                         ;
                add     si, ax                          ; ᬥ頥��� �� ��砫� ������ �� 0..15 ����
                shl     ax, 2                           ;
                add     ax, 48 * 2                      ;
                add     di, ax                          ; ����塞 ���� � ��ப� ��� �ᮢ�� ����
                jmp     TCD_Sound_Read                  ;

TCD_Comm_In:    cmp     __sound_count, 28 * 2
                jb      TCD_Comm_Out
                ; --- �室��� �������� (� ���� �� ������)
                ;     �� �������� �⮨� �� ������� � ������ ���樠����樨
                mov     gs, ss:SEG_ST                   ; C800h
                mov     si, ADR_OZU_K1 + 256 * 7        ; ���� !
                mov     ax, __sound_count               ;
                sub     ax, 28 * 2                      ; 0-��� ���� ��. ���-樨 - 56-�� � 横��
                add     si, ax                          ;
                shr     ax, 1                           ; �� 2 ���� �� ��������
                inc     ax                              ;
                mul     WORD PTR __VAL_160              ;
                mov     di, ax                          ; ����� ��ப� ��� ᫮�� ������樨
                mov     ax, __sound_count               ;
                and     ax, 01h                         ; 2 ���� �� �������� � ����
                shl     ax, 2                           ; 4 ���� �� ᫮�� ������樨 � ����� ���
                add     ax, 43 * 2                      ; 39-� �⮫��� ��࠭�
                add     di, ax                          ; ���� ������� ���� � ᮮ⢥�-�� ��ப�
                jmp     TCD_Sound_Read

                ; --- �室��� �������� - (� ��� �� ����)
                ;
                mov     es:[si].sorm_chnl_a, al         ;
TCD_Comm_Out:   mov     gs, ss:SEG_ST                   ; C800h
                mov     si, ADR_OZU_K1 + 256 * 1        ; ��� !
                mov     ax, __sound_count               ;
                add     si, ax                          ;
                shr     ax, 1                           ; �� 2 ���� �� ��������
                inc     ax                              ;
                mul     WORD PTR __VAL_160              ;
                mov     di, ax                          ; ����� ��ப� ��� ᫮�� ������樨
                mov     ax, __sound_count               ;
                and     ax, 01h                         ; 2 ���� �� �������� � ����
                shl     ax, 2                           ; 4 ���� �� ᫮�� ������樨 � ����� ���
                add     ax, 38 * 2                      ; 30-� �⮫��� ��࠭�
                add     di, ax                          ; ���� ������� ���� � ᮮ⢥�-�� ��ப�

TCD_Sound_Read: ; --- �⥭�� ���� �� ���������
                mov     ah, cBLUE * 16 + clWHITE        ;
                mov     bl, gs:[si]                     ;
                ; ---  ����� ��� ��室��� ������樨
                cmp     __sound_count, 28 * 2           ;
                jae     TCD_Print                       ;
                mov     si, __sound_count               ;
                mov     __sound_com_out[si], bl         ;
                ; --- ᮡ�⢥��� �����
TCD_Print:      mov     cx, 2                           ;
                call    print_H                         ;
                ; --- ����稪 ����, ���⠭��� �� ���������
                inc     __sound_count                   ;
                ; 28 ᫮� �室��� ���-樨, 28 ᫮� ��室��� ���-樨, �� 16 ���� �� 30 ������� ����
                cmp     __sound_count, 28 * 2 + 28 * 2 + 30 * 16
                jb      TCD_HiLight                     ;
                mov     __sound_count, 0                ; ᭠砫�

                ; --- ���ᢥ⪠
TCD_HiLight:    xor     bx, bx                          ;
                mov     cx, _MAX_MODEM_TS               ;
TCD_HL_Loop:    mov     ah, cBLUE * 16 + clWHITE        ;
                cmp     Modem_KSL[bx], 0ffh             ;
                je      TCD_HL_Next                     ;
                mov     ah, bl                          ;
                add     ah, 2                           ;
                shl     ah, 4                           ;
                add     ah, clWHITE                     ;
                jmp     TCD_HL_Draw                     ;
TCD_HL_Next:    inc     bx                              ;
                loop    TCD_HL_Loop                     ;
                jmp     TCD_Exit                        ;

                ; --- �ᮢ���� ���ᢥ⪨
TCD_HL_Draw:    push    cx                              ;
                ; --- ���ᢥ⪠ ��� � ᯨ᪥
                mov     cx, 4                           ;
                push    ax                              ;
                movzx   ax, Modem_KSL[bx]               ;
                mul     WORD PTR __VAL_160              ;
                mov     PEREM_AX, ax                    ;
                mov     di, 160 * 1 + 2 * 30            ;
                add     di, ax                          ;
                pop     ax                              ;
                call    print_A                         ;
                ; --- ���ᢥ⪠ ������⢮������� ⠩�-᫮� ������
                mov     cx, 2                           ;
		push	ax				;
		mov	ax, bx				;
                mul     WORD PTR __VAL_160              ;
                mov     di, 160 * 1 + 2 * 35            ;
                add     di, ax                          ;
		pop	ax				;
                call    print_A                         ;
                ; --- ���ᢥ⪠ ��室��� ������樨
                mov     cx, 4                           ;
                mov     di, 160 * 1 + 2 * 38            ;
                add     di, PEREM_AX                    ;
                call    print_A                         ;
                ; --- ���ᢥ⪠ �室��� ������樨
                mov     cx, 4                           ;
                movzx   si, Modem_KSL[bx]               ;
                shl     si, 1                           ;
                push    ax                              ;
                movzx   ax, __sound_com_out[si]         ;
		and	ax, 0fh				;
                mul     WORD PTR __VAL_160              ;
                mov     di, 160 * 1 + 2 * 43            ;
                add     di, ax                          ;
                pop     ax                              ;
                call    print_A                         ;
                ; --- ���ᢥ⪠ ������� ����
                mov     cx, 32                          ;
                movzx   si, Modem_KSL[bx]               ;
                shl     si, 1                           ;
                push    ax                              ;
                movzx   ax, __sound_com_out[si]         ;
		and	ax, 0fh				;
                mul     WORD PTR __VAL_160              ;
                mul     WORD PTR __VAL_3                ;
                mov     di, 160 * 1 + 2 * 48            ;
                add     di, ax                          ;
                pop     ax                              ;
                call    print_A                         ;
                add     di, 160                         ;
                call    print_A                         ;
                add     di, 160                         ;
                call    print_A                         ;
                ; ---
                pop     cx                              ;
                jmp     TCD_HL_Next                     ;
                ; ---
TCD_Exit:
                ; ---
                ENDM
ENDIF
