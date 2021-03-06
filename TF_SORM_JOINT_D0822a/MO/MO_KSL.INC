;;; ---------------------------------------------------------------------------
;;; --- ࠡ�� � ��� - ������                                               ---
;;; ---------------------------------------------------------------------------
;
; --- �᢮�������� ��� --------------------------------------------------------
; �室:         es:[si]         - ��뫪� �� ����
;               dh		- ��� �᢮��������, �᫨ = 0 �᢮������� ��� �� ����
;
KSL_Free        PROC
		push	es
		pushad
                ; --- � ���� �� ��� ?
                mov     ax, WORD PTR es:[si].sorm_ksl_a ;
                cmp     al, 0ffh                        ;
                je      @@exit                          ;
                ; --- ���� �����蠥��� ?
                or      dh, dh                          ;
                jz      @@clear_ksl                     ;
                ; --- ����� �� �����襭�� ��ᨨ, � �᢮�������� ��� �� ⮩ ��� ���� ��稭�
                ; ᮮ�饭�� "�᢮�������� ���"
                mov     es:[si].sorm_opcode, dh         ;
		mov	dl, SO_OSV                      ;
		call	Message_SORM                    ;
                ; �����塞 ��ࠬ���� ��ᨨ
                mov     WORD PTR es:[si].sorm_ksl_a, 0ffffh ; ��� ���
                mov     es:[si].sorm_prior, 0ffh        ; ��� �ਮ���
                mov     dl, es:[si].sorm_get_num        ;
                mov     es:[si].sorm_get_num, 2         ; ��⨪� ��� �
                test    dl, 3                           ;
                jnz     @@clear_ksl                     ;
                mov     es:[si].sorm_get_num, 8         ; ��⨪� ��� �
                test    dl, 0ch                         ;
                jnz     @@clear_ksl                     ;
                mov     es:[si].sorm_get_num, 20h       ; ��⨪� ��� ��窠
                ; --- ����� ���� �� �᢮������� - � al � ���-�
@@clear_ksl:    @COMM_RESET                             ; �﫨 ��������
                ; �᢮������� ��� � ࠧ�뢠�� ��� ��� � ��ᨥ�
                movzx   bx, al                          ; � ���-�
		mov	KSL_Busy[bx], 255		;
                shl     bx, 1                           ;
                mov     KSL_Session[bx], 0ffffh         ;
                ; �饬 �易��� ⠩�-᫮� ������
@@find_tms_mod: mov     cx, _MAX_MODEM_TS               ;
                xor     bx, bx                          ;
@@ksl_mod_find: cmp     al, Modem_KSL[bx]               ;
                je      @@ksl_mod_found                 ;
                inc     bx                              ;
                loop    @@ksl_mod_find                  ;
                jmp     @@exit                          ;
@@ksl_mod_found:
                ; --- {Paul} --- <12/02/2013>
                test    FG_SORM4, _3_pty_sorm           ; ����� 3PTY: B(*) -> C ?
                jz      @@go_121                        ; ���, ᭨��� �� �� ������
                and     FG_SORM4, not _3_pty_sorm
                pushad
                mov     eax, PEREM_EBX
                xor     bp, bp
                mov     cx, _sorm_3pty_max
                ; --- ��� ������� �� � ����� 2-�� ᮥ�������
                mov     edx, DWORD PTR es:[si].sorm_num_a
  @@find_slot:  cmp     Sess_3PTY[bp].perem_ebx_3pty, eax ; �� ��� ᮥ�������, � ���஬ 3PTY?
                jne     @@next_slot                       ; ���
                cmp     Sess_3PTY[bp].sorm_numb_3pty, edx ; ����� � ����� ᮮ⢥������ ?
                jne     @@next_slot                       ; ��
 @@whats_slot:  cmp     Sess_3PTY[bp].perem_tms, 0FFh     ; ⠩�᫮� �ய�ᠭ?
                jne     @@next_slot                       ; ��
                mov     Sess_3PTY[bp].perem_tms, bl       ; ->�������� ����� ⠩�᫮� ��� ������ ��ᨨ
                mov     eax, es:[si].sorm_soed            ;\
                mov     Sess_3PTY[bp].soed_ptr, eax       ; ->�������� �-�� � ᮥ�.
                popad
                jmp     @@exit
  @@next_slot:  add     bp, _sorm_3pty_size
                loop    @@find_slot
                popad
                ; --- {Paul} --- <END>
                ; -- �᫨, ����, ��祣� �� ������, ⮣��
                ; �᢮������� �易��� ⠩�-᫮� ������
@@go_121:       mov     Modem_KSL[bx], 0ffh             ;
                ; ���������� ���. ࠡ�稩 �
                mov     al, es:[si].sorm_ck_a           ;
                mov     SORM_CK, al                     ;
                mov     ax, es:[si].sorm_work_a         ;
                mov     SORM_Work_Num, ax               ;
                ; ��ࠢ�塞 ��_121
                @POLUCH_ADR_MO                          ;
                or      dl, dl                          ; �஢�ਬ ���� ���� ?
                jnz     @@exit                          ; ��� - �ய��⨬
                mov     al, 121                         ; ����� �����
                stosb                                   ;
                mov     al, SORM_CK                     ;
                stosb                                   ;
                mov     ax, SORM_Work_Num               ;
                stosw                                   ;
                mov     ax, bx                          ; bx - ���浪��� � ���������� ⠩�-᫮�
                add     ax, ax                          ;
                add     al, _MODEM_1ST_TS               ; ������ � ⠩�-᫮�
                mov     ah, al                          ;
                inc     ah                              ;
                stosw                                   ;
                @ZAPIS_OK_MO                            ;
@@exit:         popad
		pop	es
		ret
KSL_Free        ENDP

KSL_Free_Far    PROC    FAR
                call    KSL_Free
                retf
                ENDP


; --- ����⨥ ��� � ���⮬ �ਮ��� � �ନ஢���� �� ����室����� ᮮ�饭�� "�᢮�������� ���"
; �室:         gs:[bx]         - ��뫪� �� ��ꥪ� ���� ��� 䨪⨢�� ��ꥪ� ������祭��
;               es:[si]         - ��뫪� �� ���� ����
;               dh              - �������� ��稭� �᢮�������� ���:
;                                 OSV_PRI  - �� ������� � ��᮪�� �ਮ��⮬ (��⮬���᪮�)
;                                 OSV_KOM7 - �� ������祭�� � ��� (��筮�)
;
KSL_Get         PROC
		push	es
		pushad
                ; --- �஢�ઠ �� ����⨪� ⮫쪮 ��� ��⮬���᪮�� �뤥����� ���
		cmp	dh, OSV_PRI			;
                jne     @@ksl_prms                      ;
                ; --- ��⮬���᪨ ����������� � ����⨪� ����� !
                mov     BYTE PTR es:[si].sorm_opcode, NEPODKL_STAT ;
                cmp     gs:[bx].obj_cat_ctrl, STATIC    ;
                je      @@exit                          ;
                ; --- ����砥� ��ࠬ���� ��� ��� ��ꥪ�
@@ksl_prms:     mov     al, gs:[bx].obj_gr_ksl          ;
                mov     dl, gs:[bx].obj_prior           ;
                mov     ah, gs:[bx].obj_get_num         ;
		mov	PEREM_AH, ah                    ;
                ; --- ���� ᢮������ ��� � ��㯯� � ���⮬ �ਮ���
                mov     BYTE PTR es:[si].sorm_opcode, NEPODKL_ZAN  ;
                mov     SORM_KSL_Clear, 0ffffh          ; ���� ��祣� �� ��諨
                mov     cx, 256                         ; �� �ᥬ �������� ���
		movzx	bp, al                          ;
                inc     KSL_Last[bp]                    ; � ᫥���饩 ��� �⮩ ��㯯�
                movzx   bx, KSL_Last[bp]                ; �� ��� ��᫥ ��� �������襩�� ��᫥����
@@ksl_find:     cmp     KSLs[bx], al                    ; ��� �室�� � �㦭�� ��㯯� ?
                jne     @@ksl_next                      ; ��� - �����
                cmp     KSL_A[bx], 0ffh                 ; �� ���-� ?
                je      @@ksl_next                      ; ��� - �����
                cmp     KSL_Busy[bx], 0ffh              ; ��� ᢮����� ?
                jne     @@ksl_prior                     ; ��� - �஢�ਬ �ਮ���
                mov     KSL_Last[bp], bl                ; �� - �������� ������ ��������� ���
                jmp     @@ksl_found                     ; ����� !
@@ksl_prior:    cmp     dl, PRIOR_LO                    ; �⨬ ������ ������ �ਮ��⮬ ?
                je      @@ksl_next                      ; �� - ����� ������ ��祣�, �����
                cmp     KSL_Busy[bx], PRIOR_HI          ; ��� ����� ��᮪�� �ਮ��⮬ ?
                je      @@ksl_next                      ; �� - �����
                cmp     SORM_KSL_Clear, 0ffffh          ; 㦥 ��室��� ��� ������� ���. �ਮ�-⮬ ?
                jne     @@ksl_next                      ; �� - �����
                mov     SORM_KSL_Clear, bx              ; �������� ��� ������� ������ �ਮ��⮬
@@ksl_next:     inc     bx                              ; ����� �� �ᥬ ���
                and     bx, 0ffh                        ; ������ ����஢ �� mod 256
                loop    @@ksl_find                      ;
		; ---
                cmp     SORM_KSL_Clear, 0ffffh          ; ��諨 � ��㯯� ��� ������� ���. �ਮ�-� ?
                je      @@exit                          ; ��� - �����, �.�. �� ��� ��� ���. ��-��
                ; --- �᢮�������� ��� ����⮩ ������ �ਮ��⮬ -------------
@@ksl_free:     mov     bx, SORM_KSL_Clear              ;
                push    si                              ;
                shld    si, bx, 17                      ;
                mov     si, KSL_Session[si]             ;
                call    KSL_Free                        ; dh ��⠭����� �� �室� � KSL_Get
                pop     si                              ;
                ; --- �ப��祭�� ��� -----------------------------------------
@@ksl_found:    ; --- �饬 ᢮������ �� �� ������
                xor     bp, bp                          ;
                mov     cx, _MAX_MODEM_TS               ;
@@ksl_mod_find: cmp     Modem_KSL[bp], 0ffh             ;
                je      @@ksl_mod_found                 ;
		inc	bp				;
                loop    @@ksl_mod_find                  ;
                jmp     @@ksl_no_free_ts                ;
@@ksl_mod_found:; -- ᢮����� ⠩�-᫮� �� ������ ������
                mov     Modem_KSL[bp], bl               ;
                ; ���������� ���. ࠡ�稩 ����� � ⠩�-᫮�� ������
                mov     al, es:[si].sorm_ck_a           ;
                mov     SORM_CK, al                     ;
                mov     ax, es:[si].sorm_work_a         ;
                mov     SORM_Work_Num, ax               ;
                ; ����塞 ����訢���� ⠩�-᫮��
                mov     ax, bp                          ;
                add     ax, ax                          ;
                add     al, _MODEM_1ST_TS               ;
                mov     ah, al                          ;
                inc     ah                              ;
                mov     SORM_Mod_Nums, ax               ;
                ; ��ࠢ�塞 ��_120
                push    es                              ;
                push    dx                              ;
                @POLUCH_ADR_MO                          ;
                or      dl, dl                          ; �஢�ਬ ���� ���� ?
                jnz     @@ksl_no_mo                     ; ��� - �ய��⨬
                mov     al, 120                         ; �뤠�� ࠧ����� � �����
                stosb                                   ;
                mov     al, SORM_CK                     ;
                stosb                                   ;
                mov     ax, SORM_Work_Num               ;
                stosw                                   ;
                mov     ax, SORM_Mod_Nums               ;
                stosw                                   ;
                @ZAPIS_OK_MO                            ;
                pop     dx                              ;
                pop     es                              ;
                ; �⠢�� ��������
                mov     al, bl                          ;
                mov     ah, KSL_Pair[bx]                ;
		push	dx				;
                mov     dx, bp                          ;
                @COMM_SET                               ;
		pop	dx				;
                ; �⬥砥� ����⨥ ���
                mov     KSL_Busy[bx], dl                ; �ਮ��� �� ��ꥪ� (������祭�� !)
                shl     bx, 1                           ;
                mov     KSL_Session[bx], si             ;
                ; �����塞 ��ࠬ���� ��������� ᮥ�������
                mov     WORD PTR es:[si].sorm_ksl_a, ax ;
                mov     ax, SORM_Mod_Nums               ;
                mov     es:[si].sorm_chnl_a, al         ;
                mov     es:[si].sorm_chnl_b, ah         ;
                mov     es:[si].sorm_prior,  dl         ;
                mov     al, es:[si].sorm_get_num        ;
		mov	ah, PEREM_AH			; obj_get_num
                test    al, 3                           ; �⡮� �� �� ?
                jnz     @@new_get_num                   ; �� -
                shl     ah, 2                           ;
                test    al, 0ch                         ; �⡮� �� �� ?
                jnz     @@new_get_num                   ; �� -
                shl     ah, 2                           ; �⡮� �� ����
@@new_get_num:  mov     es:[si].sorm_get_num, ah        ;
                mov     es:[si].sorm_opcode, NEPODKL_OK ;
                ; ᮮ�饭�� "������祭�� ���"
                cmp     dh, OSV_PRI                     ;
                je      @@exit                          ; �� ��⮬���᪮� ������祭�� �� ��
                mov     dl, SO_POD                      ;
                call    Message_SORM                    ;
@@exit:		popad
		pop	es
		ret
                ; === ��室� �� ��㤠� �� �孨�᪨� ��稭�� ==============
@@ksl_no_mo:    ; --- �� ������祭�� ��� �� ��. ��稭��
                pop     dx                              ;
                pop     es                              ;
                mov     BYTE PTR es:[si].sorm_opcode, NEPODKL_MO    ;!!!
		popad
                pop     es
		ret
@@ksl_no_free_ts:;--- �� ������祭�� ��� �� ��. ��稭��
                mov     BYTE PTR es:[si].sorm_opcode, NEPODKL_MODEM ;!!!
		popad
                pop     es
		ret
KSL_Get		ENDP

KSL_Get_Far     PROC    FAR
                call    KSL_Get
                retf
                ENDP

;;; ---------------------------------------------------------------------------
;;; --- ࠡ�� � ��� - �����                                                ---
;;; ---------------------------------------------------------------------------
;
