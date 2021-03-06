; �������
;*************************************************
; �����頥� � reg ��砫쭮� ᬥ饭��, � eax - ID
;*************************************************
@GetOfCurConf           MACRO   @Change, @reg
                        LOCAL   @M_Exit, @@secnd_cpy
                        push    dx
                        mov     dl, FLCurActCopyB[eax]          ; ⥪��� ���䨣����
                        and     dl, 1                           ; ���� 0, ���� 1
             if  @Change
                        xor     dl, 1                           ; ���塞
             endif
                        mov     @reg, FLFirstCopyAddrD[eax*4]
                        cmp     dl, 1
                        jne     @M_Exit                         ; 0 - ��室��
                        mov     @reg, FLSecndCopyAddrD[eax*4]   ; ���塞
@M_Exit: ;|
                        pop     dx
                        ENDM

;*************************************************************************
; "�����" ���� - �����頥� � reg ��砫쭮� ᬥ饭�� + add_val, � eax - ID
;*************************************************************************
@GetOfCurConf_@         MACRO   @Change, @reg
                        LOCAL   @M_Exit, @M_Start
                        push    dx
                        mov     dl, FLCurActCopyB[eax]          ; ⥪��� ���䨣����
                        and     dl, 1                           ; ���� 0, ���� 1
                        ;jnz     @@secnd_cpy
             if  @Change
                        xor     dl, 1                           ; ���塞
             endif
                        cmp     Flash_ptr, 0
                        jz      @M_Start
                        mov     @reg, Flash_ptr
                        jmp     @M_Exit
                        ; ----------------------------------
@M_Start:               mov     @reg, FLFirstCopyAddrD[eax*4]
                        cmp     dl, 1
                        jne     @M_Exit                         ; 0 - ��室��
                        mov     @reg, FLSecndCopyAddrD[eax*4]   ; ���塞
@M_Exit: ;|
                        pop     dx
                        ENDM

;--- ����������� ����㤮����� --------------------------------------
; ࠧ��頥��� � ���樠����樨
INI_COM_CTO     PROC

        PUSH    ES
	MOV	ES,SS:SEG_ZAG
	MOV	AL,ES:[0FFF7h]			;\
	CMP	AL,'�'				;|
	JNE	BEG_5	 			;|
	MOV	AL,ES:[0FFF8h]			;|
	CMP	AL,'�'				;|
	JNE	BEG_5				;|
	MOV	AL,ES:[0FFF9h]			;|
	CMP	AL,'�'				;|
	JNE	BEG_5  				; \ ��।��塞 �������
	MOV	AL,ES:[0FFFBh]			; /     �����稪�
        CMP     AL,'�'
	JNE	BEG_5  				;|
	MOV	AL,ES:[0FFFCh]			;|
	CMP	AL,'�'				;|
	JNE	BEG_5  				;|
	MOV	AL,ES:[0FFFDh]			;|
	CMP	AL,'�'				;|
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
        ; --- � �������� ��������� ---------------------------------
        MOV     FLASH_OK,0                      ;����뢠�� 䫠� �訡�� FLASH
        MOV     PLACE,0                         ;����㧪� � FLASH-���
        ; --- ����� FLASH � �������� D000 --------------------------
        MOV     DX,SS:SEG_PCM                   ; FS - ᥣ���� ���������
        MOV     SS:SEG_FS,DX
        MOV     SCREEN_SEG,DX                   ; ���������� ᥣ���� ���������
        MOV     FS,DX

        MOV     ES,SS:SEG_GS8                   ; ES = D000h
        CMP     BYTE PTR ES:[0],55h             ; ���� ������� ?
        JNE     @@ELAN_2MB                        ; ��� - ��� FLASH
        CMP     BYTE PTR ES:[1],0AAh            ; ���� ������� ?
        JNE     @@ELAN_2MB                        ; ��� - ��� FLASH
        CMP     BYTE PTR ES:[6],'�'             ;\
        JNE     @@ELAN_2MB                      ;|
        CMP     BYTE PTR ES:[7],'�'             ;|
        JNE     @@ELAN_2MB                      ;|   ��।��塞
        CMP     BYTE PTR ES:[8],'�'             ;|   ���� �� � D000
        JNE     @@ELAN_2MB                      ; \  �������
        CMP     BYTE PTR ES:[9],'�'             ; /  ��襣� �����稪�
        JNE     @@ELAN_2MB                      ;|
        CMP     BYTE PTR ES:[10],'�'            ;|
        JNE     @@ELAN_2MB                      ;|
        CMP     BYTE PTR ES:[11],'�'            ;|
        JNE     @@ELAN_2MB                      ;/
        MOV     PLACE,1                         ;����㧪� � FLASH
        mov     SS:SEG_FLASH, ES
        ; --- ���� ����஫��� ELAN (2MB) -------------------------------------
@@ELAN_2MB:
        mov     ax, _ADR_SEG_FLASH_0            ;\
        mov     es, ax                          ;|
        cmp     BYTE PTR es:[0fff8h],'�'        ;|
        jne     @@ELAN_4MB                      ;|
        cmp     BYTE PTR es:[0fff9h],'�'        ;|
        jne     @@ELAN_4MB                      ;|
        cmp     BYTE PTR es:[0fffah],'�'        ; \ ��।��塞 �������
        jne     @@ELAN_4MB                      ; / �����稪� �� FLASH
        mov     BYTE PTR es:[0fffbh],'�'        ;|  ELAN SC-410
        jne     @@ELAN_4MB                      ;|
        cmp     BYTE PTR es:[0fffch],'�'        ;|
        jne     @@ELAN_4MB                      ;|
        cmp     BYTE PTR es:[0fffdh],'�'        ;|
        jne     @@ELAN_4MB                      ;/
        ; ! ����� 䫥�� 2 ��
        mov     PLACE, 3
        mov     ax, _ADR_SEG_FLASH_1            ; ��⠭���� ᥣ���� "seg_flash"
        mov     ss:SEG_FLASH, ax                ; �� FLASH ELAN SC-410
        mov     WORD PTR WIN_ETIC[2],  3
        mov     WORD PTR WIN_ETIC[4],  5
        mov     WORD PTR WIN_ETICP[0], 47
        mov     WORD PTR WIN_ETICP[2], 63
        mov     WORD PTR WIN_CONF[2],  2
        mov     WORD PTR WIN_CONF[4],  4
        mov     WORD PTR WIN_PROC[0],  32
        mov     WORD PTR WIN_PROC[2],  48
        mov     FLFirstCopyAddrD[0],    32 * _fls_win_size ;\
        mov     FLSecndCopyAddrD[0],    48 * _fls_win_size ;| �ணࠬ��
        mov     FLSizeInByteD[0],       80000h             ;/
;;; ���� ��� - ������ ���� 2 * _fls_win_size � 4 * _fls_win_size
        mov     FLFirstCopyAddrD[4],    32 * _fls_win_size ;\
        mov     FLSecndCopyAddrD[4],    48 * _fls_win_size ;| ���䨣����
        mov     FLSizeInByteD[4],       10000h             ;/

        mov     Flash_size, 2                   ; ࠧ��� 䫥� 2 ��
        jmp     @@Count_Memory
; --- ���� ����஫��� ELAN (4MB) --------------------------------------------
@@ELAN_4MB:
;;;jmp @@Count_Memory
        mov     ax, _ADR_SEG_FLASH_0            ;\
        mov     es, ax                          ;|
        cmp     BYTE PTR es:[0fff7h],'�'        ;|
        jne     @@Count_Memory                  ;|
        cmp     BYTE PTR es:[0fff8h],'�'        ;|
        jne     @@Count_Memory                  ;|
        cmp     BYTE PTR es:[0fff9h],'�'        ; \ ��।��塞 �������
        jne     @@Count_Memory                  ; / �����稪� �� FLASH
        mov     BYTE PTR es:[0fffbh],'�'        ;|  ELAN SC-410
        jne     @@Count_Memory                  ;|
        cmp     BYTE PTR es:[0fffch],'�'        ;|
        jne     @@Count_Memory                  ;|
        cmp     BYTE PTR es:[0fffdh],'�'        ;|
        jne     @@Count_Memory                  ;/
        ; ! ����� 䫥�� 4 ��
        mov     PLACE, 3
        mov     ax, _ADR_SEG_FLASH_1            ; ��⠭���� ᥣ���� "seg_flash"
        mov     ss:SEG_FLASH, ax                ; �� FLASH ELAN SC-410
        mov     WORD PTR WIN_ETIC[2],  65
        mov     WORD PTR WIN_ETIC[4],  67
        mov     WORD PTR WIN_ETICP[0], 111
        mov     WORD PTR WIN_ETICP[2], 127
        mov     WORD PTR WIN_CONF[2],  64
        mov     WORD PTR WIN_CONF[4],  66
        mov     WORD PTR WIN_PROC[0],  96
        mov     WORD PTR WIN_PROC[2],  112
        mov     Flash_size, 4           ; ࠧ��� 䫥� 4 ��
        ; --- 㧭��� ᪮�쪮 ����⨢��� ����� �� ELAN`e
@@Count_Memory:
        ;call    CountMemory
        push    es
        mov     ax, _SEG_ALL            ;\ ����ந��� �� ᥣ���� ��४�뢠�騩
        mov     es, ax                  ;/ ��� ������
        MOV     EBP, 400000h            ; 4 MB
        XOR     ECX, ECX                ; ���� �����㦥�� 0 �������� ����⨢��� �����
        MOV     EBX, EBP                ; ��७�ᥬ 4 ����
        DEC     EBX                     ; ����ந��� �� ��᫥���� ���� ����㥬�� ������
;_dymmy_memory_test equ 400000h - 200h
;        mov     ES:[_dymmy_memory_test], 5Ah
@@LOOP_MEM:
        MOV     ES:[EBX], BYTE PTR 0AAh ; ����襬
;        mov     al, ES:[_dymmy_memory_test]
        CMP     ES:[EBX], BYTE PTR 0AAh ; �஢�ਬ
        JNE     @@EXIT_count            ; �� ࠢ��, - �멤��
        MOV     ES:[EBX], BYTE PTR 055h ; ����襬
;        mov     al, ES:[_dymmy_memory_test]
        CMP     ES:[EBX], BYTE PTR 055h ; �஢�ਬ
        JNE     @@EXIT_count            ; �� ࠢ��, - �멤��
        ADD     EBX, EBP                ; ����ந��� �� ᫥���騩 ���� ���஢����
        ADD     ECX, 4                  ; �ਡ���� �� 4 �� � �������
        CMP     ECX, 32                 ; ��諨 �� 32 ��?
        JBE     @@LOOP_MEM              ; ���, ⮣�� �஢�ਬ ᫥���騥 4 ����
@@EXIT_count:
; - �.�. ࠡ�⠥� �� ᮢᥬ ��୮ �� �������� ����� �����誠 ��� ����� 祬 8 ��
cmp cl, 8
jb @@no_memory_change
mov cl, 8
@@no_memory_change:
        mov     Memory_size, CL         ; ����襬 ��� �뢮��
        pop     es

        ; --- ������ ���䨣���� --------------
@@Find_conf:
        PUSH    FS
        CALL    FIND_CONF_ELAN                  ;�饬 ࠡ���� ����� ���䨣��-��
        CMP     CURR_CONF,0                     ;���� ���䨣���� �� FLASH ?
        JE      @@test_4MB_pereh                ; ��� -
        CALL    READ_CONF                       ;���뢠�� ���䨣����
        JMP     @@NO_COPY
        ; --- ������������ ��� �� FLASH ( ������ ������ ��������� ����� ���������� )
@@test_4MB_pereh:
        cmp     flash_size, 4                   ; �஢�ਬ ࠧ��� ����� 4?
        jb      @@NO_FIND                       ; ��, ����� ����⢨⥫쭮
                                                ; ��� ���䨣��樨
                                                ; ���� �஢�ਬ �� ��஬ ����
;;;jmp @@no_find, ��� �⫠���
        mov     ax, WORD PTR WIN_ETIC[2]        ;\
        mov     temp_var[0], ax                 ;|
        mov     ax, WORD PTR WIN_ETIC[4]        ;|   ��࠭�� �� �६����� ����
        mov     temp_var[2], ax                 ; \  ⥪�饥 ���祭�� ����
        mov     ax, WORD PTR WIN_CONF[2]        ; /
        mov     temp_var[4], ax                 ;|
        mov     ax, WORD PTR WIN_CONF[4]        ;|
        mov     temp_var[6], ax                 ;/
        mov     WORD PTR WIN_ETIC[2],  3        ;\
        mov     WORD PTR WIN_ETIC[4],  5        ;| ��᢮�� ���祭�� ��ன
        mov     WORD PTR WIN_CONF[2],  2        ;| ���䨣��樨 ��� 2�4
        mov     WORD PTR WIN_CONF[4],  4        ;/
        call    Find_conf_elan                  ; ���饬 � ��஬ ����
        cmp     Curr_Conf, 0                    ; ��諨...?
        je      @@no_find_norm2mb               ; ���, ����� �஢�ਬ ��� 1.5
        ; !!! ����� ��諨 ���䨣���� �� ��஬ ����
        call    Read_Conf                       ; ���⠥� ���䨣����
        jmp     @@no_find_4MB                   ;
@@no_find_norm2mb:
        MOV     ES:WIN_ETIC[2], 41H             ;\
        MOV     ES:WIN_ETIC[4], 51H             ;| ����� ��室���� ���䨣����
        MOV     ES:WIN_CONF[2], 40H             ;| �᫨ ��� 1.5
        MOV     ES:WIN_CONF[4], 50H             ;/
        call    Find_conf_elan                  ; ���饬 � ��஬ ����
        cmp     Curr_Conf, 0                    ; ��諨...?
        je      @@no_find_4MB                   ; ���, ����� ����⢨⥫쭮 ���
        ; !!! ����� ��諨 ���䨣���� �� ��஬ ���� v1.5
        call    Read_Conf                       ; ���⠥� ���䨣����
@@no_find_4MB:
        mov     ax, temp_var[0]                 ;\
        mov     WORD PTR WIN_ETIC[2], ax        ;|
        mov     ax, temp_var[2]                 ;|  ���⠭���� �� �६������
        mov     WORD PTR WIN_ETIC[4], ax        ; \ ���� �����饥 ���祭�� ����
        mov     ax, temp_var[4]                 ; /
        mov     WORD PTR WIN_CONF[2], ax        ;|
        mov     ax, temp_var[6]                 ;|
        mov     WORD PTR WIN_CONF[4], ax        ;/
@@NO_FIND:
        MOV     CURR_CONF,4                     ;�⠢�� �ਧ��� 2-� �����
        CALL    SAVE_CONF                       ;�����뢠�� ����-� (1-� �����)

@@NO_COPY:
	MOV     AX,CURR_CONF                    ;���������� ����� �����
        SHR     AL,1                            ;\
        ADD     AL,30h                          ; > �뢮��� ����� �����
        MOV     ES,SS:SEG_PCM
        MOV     ES:MENU[00],AL                  ;/  ���䨣��樨 �� ��࠭
@@NO_FLSH:
        MOV     NOM_CONF,1
        POP     FS

        CLI                                     ; ���஥� ���뢠���
        ; --- ���樠������ ��� ����
        MOV     BX,N_COM                        ; BX - ����� ���-����(1 ��� 2)
        DEC     BX
        AND     BX,1
        SHL     BX,1
        MOV     DX,ADR_PORT[BX]                 ; DX - ���� ���-����
        MOV     PORT_,DX                         ;���������� ���� ���-����
        MOV     AX,VECT_PORT[BX]                ; AX - ���� ����� ����-�
        MOV     COM_VECT,AX                     ;���������� ���� �����
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
        MOV     OLD_MASK0,AL                    ;��������� 1-� ���� ���뢠���
        MOV     AL,11100001b
        MOV     AH,MASK_PORT[BX]                ;����᪨�㥬 IRQ4 ��� IRQ3
        MOV     COM_MASK,AH                     ;���������� ���� ���뢠���
        OR      AL,AH                           ;����᪨�㥬 IRQ4 ��� IRQ3
;        OUT     021h,AL
        JMP     SHORT   $+2

;; *****************************************    ����������� ������� ������������
        MOV     AX,OFFSET LCO                   ; AX - ���� ��ࢮ� ��६�����
                                                ; ��᫥ ������ ���䨣��樨
                                                ;������ ���� ������ 512 !!!!!
        DEC     AX
        SHR     AX,9
        MOV     MAX_BL_CONF,AX                  ;���-�� ᥪ�஢ ���䨣��樨

; ******************************************    ��᫥ ����᪠ ��������
        MOV     AL,11100001b                    ; ����襭�� IRQ1, 2
        MOV     AH,COM_MASK
        NOT     AH
        AND     AL,AH                           ; ����襭�� IRQ4 ��� IRQ3
;        OUT     021h,AL
        JMP     SHORT   $+2
        MOV     DX,PORT_
        IN      AL,DX                           ; kod = inportb( port );
        MOV     AL,55h
        OUT     DX,AL                           ; outportb( port, kod );
        MOV     DX,PORT_2
        IN      AL,DX                           ; �⥭�� ॣ-� ������-�� ���.
        POP     FS

        call    FLInicialize                    ; ���樠�����㥬 ࠡ��� � �⠥� 䫥�

        RET
INI_COM_CTO     ENDP


