                ; === ���樠������ ������ ���� =========================================
@SORM_INIT_DATA MACRO
                LOCAL   @@init_soed, @@init_comm
                ; --- ���樠������ ���� ᮮ�饭�� 1-�� ������
                mov     PER_SORM, 0
                mov     POS_SORM, 0
                ; --- ���樠������ ���� ᮮ�饭�� 2-�� ������
                mov     SLED_SOOB, 0
                mov     POS_SOOB,  0
                ; --- ����⠭������� ��஫� �� 㬮�砭��
                mov     eax, OLD_PAROL_0_3
                mov     PAROL_0_3, eax
                mov     ax, OLD_PAROL_4_5
                mov     PAROL_4_5, ax
                ; --- �㬥��� �맮��� ����
                mov     SORM_Call_Num, 0
                mov     SORM_Curr_Call, 0ffffh
                ; --- ���⪠ ⠡��� ���
                push    ds
                pop     es
                mov     eax, 0ffffffffh
                mov     cx, _KSL_DATA_SIZE
                lea     di, KSL_Group
                rep     stosb
                ; --- ���⪠ ⠡��� ��ꥪ⮢
                mov     Obj_Count, 0            ; ��� ⥫�䮭�� ⮫쪮
                ;
                mov     es, ss:SEG_OBJ
                mov     cx, _OBJ_SIZE * (_BUNCH_MAX + _OBJ_MAX) - 1
                xor     di, di
                rep     stosb
                ; --- ���⪠ ��ᨩ
                mov     es, ss:SEG_SORM
                mov     cx, _SESSION_MAX * _SORM_SIZE
                shr     cx , 2
                xor     di, di
                rep     stosd
                ; --- ���⪠ ᥣ���� ᮥ������� �� ��뫮� �� ��ꥪ�� ����
                mov     es, ss:SEG_SOED
                mov     cx, _SOED_MAX
                xor     edi, edi
@@init_soed:    mov     es:[edi].bunch_a_ptr,       ax
                mov     es:[edi].bunch_b_ptr,       ax
                mov     es:[edi].calling_ptr,       ax
                mov     es:[edi].calling_short_ptr, ax
                mov     es:[edi].called_ptr,        ax
                mov     es:[edi].called_short_ptr,  ax
                mov     es:[edi].called_mg_ptr,     ax
                mov     es:[edi].ra_ptr,            ax
                mov     es:[edi].ra_mg_ptr,         ax
                mov     es:[edi].bunch_ra_ptr,      ax
                mov     es:[edi].upats_ab_ptr,      ax
                mov     es:[edi].upats_ba_ptr,      ax
                mov     es:[edi].Ub_calling_ptr,    ax
                mov     es:[edi].Ua_called_ptr,     ax
                mov     es:[edi].called_Ua_ptr,     ax
                mov     es:[edi].called_mg_Ua_ptr,  ax
                mov     es:[edi].calling_Ub_ptr,    ax
                mov     es:[edi].called_Ua_Nc_ra_ptr, ax
                mov     es:[edi].Uc_calling_ra_ptr, ax
                mov     es:[edi].upats_ca_ra_ptr,   ax
                add     edi, _SOED_SIZE
                loop    @@init_soed
                ; --- ���⪠ ������樨
                mov     es, ss:SEG_ST
IF _MSP EQ TRUE
                mov     cx, 55;;;!!!28;;;_MAX_E1 * 30
ELSE
                mov     cx, _MAX_E1 * 30
ENDIF
IF _MSP EQ TRUE
                mov     si, ADR_OZU_K1 + ADD_4_KAN
@@init_comm:    mov     BYTE PTR es:[si], 0c0h
                add     si, 4
                loop    @@init_comm
ELSE
                mov     si, ADR_OZU_K1 + 256 + 1   ; � ���0
@@init_comm:    mov     BYTE PTR es:[si], 80h
                add     si, 2
                loop    @@init_comm
ENDIF
                ; --- {Paul.N} --- <01/04/2013>
                ; -- ���⨬ ���ᨢ ������� ��� ��ᨨ ����(3���)
                push    es
                mov     es, ss:SEG_AX
                lea     di, Sess_3PTY
                mov     cx, _sorm_3pty_last
                rep     stosb
                pop     es
                ; --- {Paul.N} --- <END>
                ; ---
                ENDM


; === ࠡ�� � �������஬ =============================================================

; --- �ப��祭�� ��� -------------------------------------------------------------------
;
@COMM_CTO       MACRO
IF _MSP EQ TRUE
                mov     es, ss:SEG_ST
                mov     di, ADR_OZU_K1 + 12
                mov     ax, CHAN_FROM_CTO               ; AL - ����� ���� �ਥ�� �� ���
                or      al, 0e0h                        ; �������� "��.ॣ-� � ����"
                stosb
                xor     al, al
                stosb
                mov     ax, CHAN_TO_CTO                 ; AL - ����� ���� �뤠� � ���
                or      al, 080h                        ; �������� "���� � ���.ॣ-�"
                stosb
ELSE
                ; � ���
                mov     fs, ss:SEG_ST
                mov     al, _BABA_TO_CTO                ; al - ����� ���� �뤠� � ���
                mov     ah, 7                           ; ����� ���� �������� - ����
                mov     fs:[ADR_OZU_K1 + 6], al         ; \ � ��⨩ ᫮� ������
                mov     fs:[ADR_OZU_K1 + 7], ah         ; / �� 㪠������� ������ ����
                ; �� ���
                mov     di, _BABA_FROM_CTO              ; ����� ���� ��� ��� ���
                shl     di, 1                           ; ��� ������樨 ᫮����
                add     di, ADR_OZU_K1 + 256 * 7        ; ᤢ������� �� ��砫� ���� �� ����
                mov     al, 3                           ; ����� ᫮� � ������
                xor     ah, ah                          ; ����� ���� �������� - �����
                mov     fs:[di],     al                 ; \ � 㪠����� ����� ����
                mov     fs:[di + 1], ah                 ; / � ���쥣� ᫮� ������
ENDIF
                ENDM

; --- ��砫쭠� ��⠭���� ������樨 ��� ����� ------------------------------------------
;
@COMM_INIT      MACRO
                LOCAL   @@loop_baba, @@loop_pcm
IF _MSP EQ TRUE
                mov     es, ss:SEG_ST
                mov     di, ADR_OZU_K1 + _MODEM_1ST_TS * 4
                lea     si, BUF_R_BABA
                mov     cx, 28
@@loop_baba:    movsb
                movsb
                add     di, 2
                loop    @@loop_baba
ELSE
                ; --- �������� �� ������ � ����
                mov     es, ss:SEG_ST
                mov     di, ADR_OZU_K1 + 256 * 7        ; ���� !
		add     di, _COMM_1ST_BABA * 2          ;;;<mmm>
                mov     ax, _MODEM_1ST_TS               ; ��砫�� ����� ������   (al=60,ah=0)
                mov     cx, _COMM_CHNLS                 ; �᫮ �������            (14)
@@loop_baba:    mov     es:[di],     al                 ; \ � ����� ����
                mov     es:[di + 1], ah                 ; / � ������ ������ (�)
                inc     al                              ;
                mov     es:[di + _COMM_AB_SHIFT * 2],     al ; \ � ����� ���� ���� ᤢ��
                mov     es:[di + _COMM_AB_SHIFT * 2 + 1], ah ; / � ᫥���饣� ������ ������ (�)
                inc     al                              ;
                add     di, 2                           ; �� ��ࠡ��� ᫥�. ���� �������� �������
                loop    @@loop_baba
                ; --- ������樨 � ��� ���� ���
                mov     ax, 8000h                       ; ��� ��� ������樨
                mov     cx, _MAX_E1 * 30                ; �� �ᥬ ᫮⠬ ��� ���-30
                mov     di, ADR_OZU_K1 + 256 * 1        ;
@@loop_pcm:     mov     es:[di],     al
		mov	es:[di + 1], ah                 ; ����頥� ��������
                add     di, 2                           ; �� ᫥���騩 �����
                loop    @@loop_pcm
ENDIF
                ENDM

; --- �ணࠬ���� �㬬�஢���� �� ��� ᮢ������ ������� -------------------------------
;
@COMM_SUM_AB    MACRO
		LOCAL   @@end, @@next_chnl, @@sum_chnl, @@skip

                push    fs
                mov     es, ss:SEG_BABA
                mov     fs, ss:SEG_AX
                mov     ds, ss:SEG_BUS
                ASSUME  DS:DATA_BUS
                mov     dx, _MAX_MODEM_TS - 1
                mov     bp, POINT_R_W
                add     bp, (_COMM_1ST_BABA + _COMM_SUM_SHIFT) * 512
		; ---
@@next_chnl:    mov     di, dx
                cmp     fs:Modem_KSL[di], 0ffh
                je      @@skip
		; ---
                shld    di, dx, 25
                add     di, bp
                mov     cx, 16
@@sum_chnl:     movzx   bx, es:[di - _COMM_SUM_SHIFT * 512]
                shl     bx, 1
                mov     si, TAB_AL[bx]
                movzx   bx, es:[di - _COMM_AB_SHIFT * 512]
                shl     bx, 1
                add     si, TAB_AL[bx]
                mov     al, TAB_L[si];
                stosb
                loop    @@sum_chnl
		; ---
@@skip:         dec     dx
                jns     @@next_chnl
                add     POINT_R_W, 16
                cmp     POINT_R_W, 320
                jb      @@end
                mov     POINT_R_W, 0
@@end:          ; ---
                pop     fs
                ENDM


; --- �८�ࠧ������ ����� ��� 1..28 � ����� ⠩�-᫮� 0,,15 � 17,,29
; �室:         al - ����� ���
; ��室:        dh - ����� ⠩�-᫮� � ���
@KSL_TO_TS      MACRO
                mov     dh, al                  ;
                shr     dh, 4                   ;
                add     dh, al                  ;
                ENDM

; --- ��⠭���� ������樨 � ��஭� ��� ------------------------------------------------
; �室:         ax - ����� ���-� � ���-�
;               dl - ����� ������ ���� �������㥬��� � ���
;
@COMM_SET       MACRO
                LOCAL   @@combined, @@separate, @@exit
                push    es
                push    ax
                push    dx
                push    di
                ; ---
                mov     es, ss:SEG_ST
                ; --- �� ?
IF _MSP EQ TRUE
                add     dh, _nom_ikm                    ; !!!! ��� ࠡ��� � ��㣨� ��⮪�� !!!
ENDIF
                cmp     ah, 0ffh
                je      @@combined
IF _MSP EQ TRUE
                ; --- ࠧ���쭮� �������
@@separate:     ; �
                movzx   di, al                          ; ����� ���-�
                dec     di
                shl     di, 3                           ; * 8 �.�. �������� ��� ������� ᫮��
                add     di, ADR_OZU_K1 + ADD_4_KAN      ; + ����
                or      dl, 80h                         ; ��⪠ ������ ���� (������ 0...15)
                mov     es:[di],     dl                 ; �⠥� �� �⮣� ������ ����
                @KSL_TO_TS                              ; �८�ࠧ㥬 � ���-� � � ��
                mov     es:[di + 1], dh                 ; � ��襬 � ����祭�� �� ����
                ; �
                add     di, 4                           ; ᫥���饥 DD ������樨
                or      dl, 90h                         ; ��⪠ ������ ���� (������ 16...31)
                mov     es:[di],     dl                 ; �⠥� �� �⮣� ������ ����
                mov     al, ah                          ; \ �८�ࠧ㥬 � ���-�
                @KSL_TO_TS                              ; / � � ��
                mov     es:[di + 1], dh                 ; � ��襬 � ����祭�� �� ����
                jmp     @@exit
                ; --- ᮢ���񭭮� �������
@@combined:     movzx   di, al                          ;
                dec     di
                shl     di, 3                           ; * 8 �.�. �������� ��� ������� ᫮��
                add     di, ADR_OZU_K1 + ADD_4_KAN      ; + ����
                or      dl, 0a0h                        ; ��⪠ ������ ���� (������ 32...47)
                mov     es:[di],     dl                 ; �⠥� �� �⮣� ������ ����
                @KSL_TO_TS                              ; �८�ࠧ㥬 � ���-� � � ��
                mov     es:[di + 1], dh                 ; � ��襬 � ����祭�� �� ����
ELSE
                ; --- ࠧ���쭮� �������
@@separate:     ; �
                cmp	al, 16   			;
		jb	@@sepr_lo_a                     ;
		inc     al				;
@@sepr_lo_a:    movzx   di, al                          ; ����� ���-�
                shl     di, 1                           ; *2 �.�. �������� ᫮����
                add     di, ADR_OZU_K1 + 256            ; � ���-30 ��
		add     dl, _COMM_1ST_BABA              ;
                mov     es:[di],              dl        ;   �⮣� ������
                mov     BYTE PTR es:[di + 1], 7         ;   ����
                ; �
                cmp	ah, 16   			;
		jb	@@sepr_lo_b                     ;
		inc     ah				;
@@sepr_lo_b:    movzx   di, ah                          ; ����� ���-�
                shl     di, 1                           ;
                add     di, ADR_OZU_K1 + 256            ;
                add     dl, _COMM_AB_SHIFT              ;
                mov     es:[di],              dl        ;
                mov     BYTE PTR es:[di + 1], 7         ;
                jmp     @@exit
                ; --- ᮢ���񭭮� �������
@@combined:     cmp	al, 16   			;
		jb	@@comb_lo                       ;
		inc     al				;
@@comb_lo:      movzx   di, al                          ;
                shl     di, 1                           ;
                add     di, ADR_OZU_K1 + 256            ;
                add     dl, _COMM_1ST_BABA + _COMM_SUM_SHIFT ;
                mov     es:[di],              dl        ;
                mov     BYTE PTR es:[di + 1], 7         ;
ENDIF
                ; ---
@@exit:         pop     di
                pop     dx
                pop     ax
                pop     es
                ENDM


; --- ��⨥ ������樨 � ��஭� ��� ---------------------------------------------------
; �室:         ax - ����� ���-� � ���-�
;
@COMM_RESET     MACRO
                LOCAL   @@combined, @@separate, @@exit
                ; ---
                push    es
                push    ax
                push    di
                mov     es, ss:SEG_ST
                ; --- �� ?
                cmp     ah, 0ffh
                je      @@combined
IF _MSP EQ TRUE
                ; --- ࠧ���쭮� �������
@@separate:     ; �
                movzx   di, al                          ; ����� ���-�
                dec     di
                shl     di, 3                           ; * 8 �.�. �������� ��� ������� ᫮��
                add     di, ADR_OZU_K1 + ADD_4_KAN      ; + ����
                mov     BYTE PTR es:[di], 0c0h          ; ����頥� �뤠�� � ���
                ; �
                add     di, 4                           ; ᫥���饥 DD ������樨
                mov     BYTE PTR es:[di], 0c0h          ; ����頥� �뤠�� � ���
                jmp     @@exit
                ; --- ᮢ���񭭮� �������
@@combined:     movzx   di, al                          ;
                dec     di
                shl     di, 3                           ; * 8 �.�. �������� ��� ������� ᫮��
                add     di, ADR_OZU_K1 + ADD_4_KAN      ; + ����
                mov     BYTE PTR es:[di], 0c0h          ; ����頥� �뤠�� � ���
ELSE
                ; --- ࠧ���쭮� �������
@@separate:     ; �
                cmp	al, 16   			;
		jb	@@sepr_lo_a                     ;
		inc     al				;
@@sepr_lo_a:    movzx   di, al                          ; ����� ���-�
                shl     di, 1                           ; *2 �.�. �������� ᫮����
                add     di, ADR_OZU_K1 + 256 + 1        ; � ��� ⠩�-᫮� ���-30
                mov     BYTE PTR es:[di], 80h           ;   �뤠� ���
                ; �
                cmp	ah, 16   			;
		jb	@@sepr_lo_b                     ;
		inc     ah				;
@@sepr_lo_b:    movzx   di, ah                          ; ����� ���-�
                shl     di, 1                           ;
                add     di, ADR_OZU_K1 + 256 + 1        ;
                mov     BYTE PTR es:[di], 80h           ;
                jmp     @@exit
                ; --- ᮢ���񭭮� �������
@@combined:     cmp	al, 16   			;
		jb	@@comb_lo                       ;
		inc     al				;
@@comb_lo:      movzx   di, al                          ;
                shl     di, 1                           ;
                add     di, ADR_OZU_K1 + 256 + 1        ;
                mov     BYTE PTR es:[di], 80h           ;
ENDIF
                ; ---
@@exit:         pop     di
                pop     ax
                pop     es
                ENDM




; --- �ᯮ����⥫�� ������, �ᯮ��㥬� �� �ନ஢���� ᮮ�饭�� -------------------
;
@MSG_TIME       MACRO
		PUSH	DX
		MOV	AL,DEN_MES
		MOV	DL,AL
		SHR	AL,4
		SHL	DL,4
		OR	AL,DL
		@_STOSB
		MOV	AL,DEN_MES[3]
		MOV	DL,AL
		SHR	AL,4
		SHL	DL,4
		OR	AL,DL
		@_STOSB
		MOV	AL,DEN_MES[2]
		MOV	DL,AL
		SHR	AL,4
		SHL	DL,4
		OR	AL,DL
		@_STOSB
		MOV	AL,DEN_MES[1]
		MOV	DL,AL
		SHR	AL,4
		SHL	DL,4
		OR	AL,DL
		@_STOSB
		POP	DX
		ENDM

@SOOB_ODNO      MACRO
                MOV     EAX,10001H
                @_STOSD
                MOV     AX,0FFFFH
                @_STOSW
                ENDM


@Value_to_ddebug macro PAR1 ; MVT
		push	eax
		mov	eax,PAR1
		DB	 9Ah
		DW	 OFFSET	value_to_ddebug_proc
		DW	 120
		pop	eax
		endm

; MVT ������ ����稭� PAR2 � ��६����� �⫠��� PAR1
@Value_to_dd	macro PAR1,PAR2
		push	ds
		mov	ds,SS:SEG_AX
		mov	PAR1,PAR2
		pop	ds
		endm

; MVT ���६��� ��६����� �⫠��� PAR
@inc_dd		macro PAR
		push	ds
		mov	ds,SS:SEG_AX
		inc	PAR
		pop	ds
		endm
if _MSP NE TRUE
; MVT ������ � �⫠���� 䠩� ��ப�
; ������ �ந�������� �� DebugFileOpened =/= 0
; ��ࠬ��ࠬ� ���� ���� �����뢠���� � 䠩� ��ப� � ������⢮ �����뢠���� ����
@String_to_file macro PAR_SEG, PAR_OFF, PAR_COUNT
		push	ds edx ecx
		push	PAR_SEG
		pop	ds
		mov	edx, PAR_OFF
		mov	ecx, PAR_COUNT
		call	write_to_debug_file
		pop	ecx edx ds
		endm

; MVT ������ � �⫠���� 䠩� ����஢��
; ������ �ந�������� �� DebugFileOpened =/= 0
; ��ࠬ��஬ ���� ����� �����㥬�� �窨 �� 0 �� 9
@ici_to_file	macro PAR
		push	edx
		mov	edx, PAR
		call	write_ici_to_debug_file
		pop	edx
		endm

; MVT ������ �������� ᫮�� � �⫠���� 䠩�
; ������ �ந�������� �� DebugFileOpened =/= 0
; ��ࠬ����: ������� ᫮�� � ������⢮ ��࠭塞�� ��ࠤ (1-8)
@dword_to_file	macro DD_VALUE,NUM_TETR
		push	ebx ecx
		mov	ebx, DD_VALUE
		mov	ecx, NUM_TETR
		call	write_ebx_to_debug_file
		pop	ecx ebx
		endm

; MVT ������ ���ᨢ� ���⮢ � �⫠���� 䠩� � ���� �⮡ࠦ���� 16-���� �����
; ������ �ந�������� �� DebugFileOpened =/= 0
; ��ࠬ��ࠬ� ���� ���� �����뢠���� � 䠩� ��ப� � ������⢮ �����뢠���� ����
@Array_to_file	macro PAR_SEG, PAR_OFF, PAR_COUNT
		push	ds esi ecx
		push	PAR_SEG
		pop	ds
		mov	esi, PAR_OFF
		mov	ecx, PAR_COUNT
		call	write_array_to_debug_file
		pop	ecx esi ds
		endm
endif

; MVT �ࠢ����� ��᫥����⥫쭮�⥩ ���⮢
; �室: SEG1,OFF1,SEG2,OFF2,COUNT - ॣ����� ��� �����।�⢥��� ���祭��
; ��室: zf=1 ����砥�, �� ��ப� ࠢ��, ���� �� ࠢ��
@compare_arrays	macro SEG1,OFF1,SEG2,OFF2,COUNT
		push	ds esi es edi ecx
		push	SEG1 ; ����ᨬ ��ࠬ���� ⠪, �⮡� ��� �� ��蠫� ��� ����
		push	OFF1
		push	SEG2
		push	OFF2
		mov	ecx,COUNT
		pop	edi
		pop	es
		pop	esi
		pop	ds
		cld
		repe	cmpsb
		pop	ecx edi es esi ds
		endm
