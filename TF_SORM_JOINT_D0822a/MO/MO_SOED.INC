;;; ---------------------------------------------------------------------------
;;; --- ࠡ�� � ᮥ������ﬨ - ������                                      ---
;;; ---------------------------------------------------------------------------
;
; --- ����祭�� �� ࠡ�祬� ������ ��뫪� �� ᫮� SOED
;     ����������� ��ࠬ��஢ ������
;     ��⠭������� ��뫪� �� ࠡ���� ������ �� ᫮� SOED
;
Seizure         PROC
                ; --- ���᫥��� ����⭮�� ࠡ�祣� �����
                mov     es, ss:SEG_ADR_SOED
                movzx   ebx, BYTE PTR ds:[bp + 1]
		and	bl, 07fh
                shl     ebx, _WORK_NUM_SHL
                or      bx, ds:[bp + 2]
                shl     ebx, 1
                ; --- ����砥� �� ࠡ�祬� ������ ��뫪� �� SOED
;                movzx   eax, WORD PTR es:[ebx]
;                cmp     eax, _SOED_MAX
;                jae      @@find
;                mov     ecx, _SOED_SIZE
;                mul     ecx
;                mov     esi, eax
;                test    fs:[esi].soed_flags, sf_REL
;                jnz     @@found_2
; ------------- !!! �������� ����� �����-�� �������� �� ������� !!! ----------------------
;                inc     SOED_Busy_Err
;                jmp     @@found_2
                ; --- ���� ᫥���饣� ᢮������� SOED
@@find:         mov     esi, SOED_Next
                mov     cx,  _SOED_MAX
@@find_loop:    add     esi, _SOED_SIZE
                cmp     esi, _SOED_LAST
                jb      @@tst_rel_flag
                xor     esi, esi
@@tst_rel_flag: test    fs:[esi].soed_flags, sf_REL  ; �஢�ਬ ��� �⡮�
                jnz     @@found
                loop    @@find_loop
; ------------- !!! �������� ����� �����-�� �������� �� ������� !!! ----------------------
                inc     SOED_Full_Err
                ; �⮡� ��⮫����� SOED_Next �� ᫥���騩 蠣
                add     esi, _SOED_SIZE
                cmp     esi, _SOED_LAST
                jb      @@found
                xor     esi, esi
                ; --- ���� SOED ��� ࠡ���
@@found:        mov     SOED_Next, esi
                ; --- ��⪠ ���������� ᫮� SOED"�
@@found_2:      mov     al, 0ffh
                mov     ecx, _SOED_SIZE
                push    fs
                pop     es
                mov     edi, esi
                @_RSTOSB
		xor	eax, eax
                mov     fs:[esi].len_a, al
                mov     fs:[esi].len_b, al
                mov     fs:[esi].len_c, al
                mov     DWORD PTR fs:[esi].soed_flags, eax
                ; --- ���������� ��ࠬ���� ������
                mov     al, ds:[bp + 1]
		and	al, 07fh
                mov     fs:[esi].ck_a, al
                mov     eax, ds:[bp + 2]
                mov     DWORD PTR fs:[esi].work_a, eax	; work & phys
                or      fs:[esi].soed_flags, sf_SZR
		test    MO_CK, 80h
                jz      @@upats
                or      fs:[esi].soed_flags, sf_IP_A
                ; --- UPATS
@@upats:        movzx   edi, BYTE PTR ds:[bp + 1]       ; � ��
                shl     edi, 4                          ; 16 ������ � ��
                mov     dx, ds:[bp + 4]                 ; \ �����
                shr     dx, 9                           ; / �����
                and     dx, 0fh
                or      di, dx                          ; � �� � � ����� �����
                shl     edi, 3                          ; 8 ���� �� ���䨣���� ����� ��
                mov     gs, ss:SEG_KONF
                ; --- {Paul} -- <15/10/2012>
                push    bx
                movzx   bx, BYTE PTR gs:[edi]           ; -- �ᯮ���� ⨯ �����
                mov     cl, TAB_TIP_LN[bx]
                pop     bx
                cmp     BYTE PTR gs:[edi], 0            ; ��뢠�騩 �� ���� ?
                jne     @@seg_adr_soed                  ; ��� - � �����-�� ����� ?
                mov     edi, gs:[edi + 1]               ; ���� ����� �����
                cmp     edi, Cfg_Free_Ptr
                jae     @@seg_adr_soed
                movzx   edx, WORD PTR ds:[bp + 4]       ; \ �����
                and     dx, 1ffh                        ; / �����
                test    FG_PROGRAM, _fg_prg_multy_pref
                jnz     @@New_conf
                mov     cl, 4                           ; ࠧ��� ����(old). ����� � ᤢ����
   @@New_conf:  shl     edx, cl                         ; ? ���� �� ���䨣���� �����
                ; --- {Paul} -- <END>
                add     edi, edx                        ; ���� ����� � SEG_KONF
                cmp     edi, Cfg_Free_Ptr               ;
                jae     @@seg_adr_soed                  ; �멤�� � ��, �� ����襬
		; --- ��� ������
                cmp     WORD PTR gs:[edi + 5], 0ffffh   ;
                je      @@seg_adr_soed                  ;
                or      fs:[esi].soed_flags, sf_UPATS_A ;
                mov     ax, gs:[edi + 5]                ; \ � �����᪨�
                mov     fs:[esi].upats_a_num, ax        ; / � �����
                mov     al, gs:[edi + 7]                ; \ �
                mov     fs:[esi].upats_a, al            ; / � �����
                ; --- UPATS
@@seg_adr_soed: ; --- �⬥砥� � SEG_ADR_SOED
                mov     es, ss:SEG_ADR_SOED
                xor     edx, edx
                mov     eax, esi
                mov     ecx, _SOED_SIZE
                div     ecx
                mov     es:[ebx], ax
                ; ---
		mov	edi, esi
                ret
Seizure         ENDP

; --- ����祭�� �� ࠡ�祬� � � ������� ���४⭮� ��뫪� �� SOED
; �室:         ds:[bp+1] - ��᮫��� ࠡ�稩 ����� �
; ��室:        fs:[edi] � CF  - ��
;               CF  - ��뫪� ��� ��� ��� �� ���४⭠
;
Check_Work_Num_A PROC
                ; --- ����砥� 㪠��⥫� � SEG_ADR_SOED
                mov     es, ss:SEG_ADR_SOED
                movzx   ebx, BYTE PTR ds:[bp + 1]
                shl     ebx, _WORK_NUM_SHL
                or      bx, ds:[bp + 2]
                shl     ebx, 1
                ; --- �஢�ઠ ᠬ�� ��뫪�
                movzx   edi, WORD PTR es:[ebx]
                cmp     edi, _SOED_MAX
                jae     @@no_ref
                ; --- �஢�ઠ ���४⭮�� ��뫪�
                mov     eax, edi
                mov     ecx, _SOED_SIZE
                mul     ecx
                mov     edi, eax
                mov     al, ds:[bp + 1]
                cmp     al, fs:[edi].ck_a
                jne     @@bad_ref_a
                mov     ax, ds:[bp + 2]
                cmp     ax, fs:[edi].work_a
                jne     @@bad_ref_a
                ; ---
@@OK:
		stc
                ret
                ; ---
@@no_ref:       inc     SOED_no_Ref_Err
                jmp     @@not_OK
@@bad_ref_a:    inc     SOED_Ref_A_Err
                ; ---
@@not_OK:       clc
                ret
Check_Work_Num_A ENDP

; --- ����祭�� �� ࠡ�祬� � ������� (� ��� �) ���४⭮� ��뫪� �� SOED
; �室:         ds:[bp+1] - ��᮫��� ࠡ�稩 ����� �
; ��室:        fs:[edi] � CF  - ��
;               CF  - ��뫪� ��� ��� ��� �� ���४⭠
;
Check_Work_Num_AB PROC
                ; --- ����砥� 㪠��⥫� � SEG_ADR_SOED
		push	ebx
                mov     es, ss:SEG_ADR_SOED
                movzx   ebx, BYTE PTR ds:[bp + 1]
                shl     ebx, _WORK_NUM_SHL
                or      bx, ds:[bp + 2]
                shl     ebx, 1
                ; --- �஢�ઠ ᠬ�� ��뫪�
                movzx   edi, WORD PTR es:[ebx]
                cmp     edi, _SOED_MAX
                jae     @@no_ref
                ; --- �஢�ઠ ���४⭮�� ��뫪� �� ࠡ�祬� ������ �
                mov     eax, edi
                mov     ecx, _SOED_SIZE
                mul     ecx
                mov     edi, eax
                mov     al, ds:[bp + 1]
                cmp     al, fs:[edi].ck_a
                jne     @@check_work_b
                mov     ax, ds:[bp + 2]
                cmp     ax, fs:[edi].work_a
                je      @@OK
                ; --- �஢�ઠ ���४⭮�� ��뫪� �� ࠡ�祬� ������ �
@@check_work_b: mov     al, ds:[bp + 1]
                cmp     al, fs:[edi].ck_b
                jne     @@bad_ref_ab
                mov     ax, ds:[bp + 2]
                cmp     ax, fs:[edi].work_b
                jne     @@bad_ref_ab
                ; ---
@@OK:           pop	ebx
		stc
                ret
                ; ---
@@no_ref:       inc     SOED_no_Ref_Err
                jmp     @@not_OK
@@bad_ref_ab:   inc     SOED_Ref_AB_Err
@@not_OK:       pop	ebx
		clc
                ret
Check_Work_Num_AB ENDP

; --- ����� ���
; �室:         fs:[edi] - 㪠��⥫� �� SOED
; ��室:        ���
; �����:       edi      - !!!!
;
ANI_Request     PROC
                mov     esi, edi
                test    fs:[esi].soed_flags, sf_ANI
                jnz     @@exit
                ; ---
                @POLUCH_ADR_MO
                or      dl, dl
                jnz     @@exit
                mov     al, 117                 ; ����� ���
                stosb
                mov     al, fs:[esi].ck_a       ; � ��
                cmp al, 127
                ja      @@exit
                stosb
                mov     ax, fs:[esi].work_a     ; �� ⠪��� ࠡ�祬� � �
                cmp ax, 8191
                ja      @@exit
                stosw
                @ZAPIS_OK_MO
@@exit:         mov	edi, esi
		ret
ANI_Request     ENDP


; --- ��࠭���� ��।��� ��� ����� � � SOED
; �室:         fs:[edi] - 㪠��⥫� �� SOED
;               ds:[si]  - 㪠��⥫� �� (�����) ����
;               cx       - ������⢮ ���
; ��室:        ���
; �����:       edi      - !!!!
;
Put_Digits      PROC
                jcxz    @@exit                  ; ��� �।���饭�� CX = 0
                ; ---
@@loop:         cmp     fs:[edi].len_b, 18      ; ���-�� ��� >= 18
                jae     @@exit                  ; �� - �멤��   MVT ��ࠢ�� ja � jae ���� num_b �������� � ck_b
                lodsb                           ; ����㧨� ���� ��� �����
                movzx   ebx, fs:[edi].len_b     ; ���쬥� ���-�� ��� �� ����� ������
                shr     ebx, 1                  ; /2
                jc      @@even                  ;
                ; --- ��襬 ���� � ������ ���浪��� ����஬ - �����, ����� � �.�.
@@odd:  	or      al, 0f0h                ; �ਧ��� ���� ����� (fh � ���襩 ��ࠤ�)
                mov     fs:[edi + ebx].num_b, al; ����訬 ����� ����
                inc     fs:[edi].len_b          ; 㢥��稬 ������⢮ ����ᠭ��� ���
                loop    @@loop                  ; �த�����, �᫨ �� ��-����� ����
                jmp     @@exit
                ; --- ��襬 ���� � ���� ���浪��� ����஬ - �����, ������� � �.�.
@@even:         and     fs:[edi + ebx].num_b, 0fh ; ���⨬ ������ ��ࠤ�
	 	shl	al, 4                     ; ���� � ������ ��ࠤ�
                or      fs:[edi + ebx].num_b, al  ; ����訬 ����� ����
                inc     fs:[edi].len_b          ; 㢥��稬 ������⢮ ����ᠭ��� ���
                loop    @@loop                  ; �த�����, �᫨ �� ��-����� ����
                ; ---
@@exit:         ret
Put_Digits      ENDP

;;; ---------------------------------------------------------------------------
;;; --- ࠡ�� � ᮥ������ﬨ - �����                                       ---
;;; ---------------------------------------------------------------------------
