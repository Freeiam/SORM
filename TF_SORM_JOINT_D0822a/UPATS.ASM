Clear_ULOG      PROC
                ; --- ���⪠ SEG_ULOG
                mov     Cfg_ULOG_Count, 0       ; !!!
                mov     es, ss:SEG_ULOG
                xor     edi, edi
                mov     al, 0ffh
                xor     ecx, ecx
                mov     cl, DESCR_SEG_ULOG.ATTR_2
                mov     ah, cl
                and     cl, 0fh
                shl     ecx, 10h
                mov     cx, DESCR_SEG_ULOG.LIM
                test    ah, 80h
                jz      SHORT @@clear_ULOG
                shl     ecx, 12
@@clear_ULOG:   @_RSTOSB
                ret
Clear_ULOG      ENDP

; === ࠡ�� � ����ࠬ� ����� ===========================================================
;
; --- �८�ࠧ������ ����� �� �ଠ� ���� � ����筮� DD -------------------------------
; �室:         eax - ����� � �ଠ� ����: ax - � ⥫�䮭� � �����, ᫥���騩 ���� - � �����,
; ��室:        eax - ����� � �ଠ� ULOG
;
@Format_U_Num   MACRO
                and     eax, 0ffffffh
                ENDM

; --- ���� ����� � SEG_ULOG ------------------------------------------------------------
; �室:         eax - ����� ����� � �ଠ� ULOG
; ��室:        CF  -  ��,  �� ������
;               edx - ���. 䨧. � ����� � �ଠ� ����
;               esi - 㪠��⥫� �� ����, ��� �४��� ����
;
Find_U_Num      PROC
                ; --- ��砫�� ��⠭����
                push    ds
                push    ebx
                push    ecx
                mov     ds, ss:SEG_ULOG         ; ᥣ���� ����஢ ⥫�䮭��
                mov     esi, 03ffh              ; �।��� (�᫨ * 8 � �� ���� ��᫥���� 64 �)
                mov     cx, 10                  ; ���ᨬ��쭮� ������⢮ ���権 ���᪠
                ; --- 横� ���᪠
@@loop:         xor     ebx, ebx                ; 0
		cmp     eax, [esi * 8]          ;
                je      @@found                 ; ࠢ�� => ��諨, ��室��
                jb      @@below                 ; ����� => �� 㬥��襭�� esi
@@above:        mov     bl, 10b                 ; 㢥����� ESI xor 1100..00b
@@below:        or      bl, 01b                 ; 㬥����� ESI xor 0100..00b
                dec     cx                      ; �ॢ�頥� � ���樨 � �⥯��� ������
                shl     ebx, cl                 ; ����� �����塞� ࠧ��� esi
                xor     esi, ebx                ; ᫥���饥 ������� ��१�� �������
                inc     cx                      ; ����⠭���� � ���樨
                loop    @@loop
                ; --- ��᫥���� �஢�ઠ
                cmp     eax, [esi * 8]          ;
                je      @@found                 ; ࠢ�� => ��諨, ��室��
		jb	@@not_found
		inc	esi
                ; --- �� ��諨
@@not_found:    shl     esi, 3
                pop     ecx
                pop     ebx
                pop     ds
                clc
                ret
                ; --- ��諨
@@found:        shl     esi, 3
                mov     edx, [esi + 4]
                pop     ecx
                pop     ebx
                pop     ds
                stc
                ret
Find_U_Num      ENDP

Find_U_Num_Far  PROC FAR
                call    Find_U_Num
                retf
Find_U_Num_Far  ENDP

; --- �������� ����� � SEG_ULOG ---------------------------------------------------------
; �室:         eax - ����� ����� � �ଠ� ����
;               edx - ���. 䨧. � ����� � �ଠ� ����
; ��室:        CF  -  ��,  �� �믮�����
;               eax - ����� � �ଠ� ULOG
;               esi - 㪠��⥫� �� ��⠢����� �����
;               ����� � SEG_KONF
;
Add_U_Num       PROC
                push    ecx
                @Format_U_Num                   ; ����� � ��� �ਣ����� ��� ����筮�� ���᪠
                mov     es, ss:SEG_ULOG
                xor     esi, esi                ; �᫨ �㤥� ����, ��⠢�� � ��砫� ULOG
                mov     ecx, Cfg_ULOG_Count     ; ���� ����� � ULOG ?
                jecxz   @@insert                ; ��� -
                cmp     ecx, _CFG_ULOG_MAX - 1  ; �� - �� �� ����� �� �� ?
                jae     @@not_add               ; ����� -
                push    edx
                call    Find_U_Num              ; �饬
                pop     edx
                jnc     @@not_found             ; �� ��諨 -
                ; --- �祭� ��࠭�� - ��諨 !!! => ���� ������� ���
@@found:        mov     es:[esi + 4], edx       ; �� ��直� ��砩 ��१���襬 ��� �⮣� ���. �����
                pop     ecx
                stc                             ; ��� ࠢ�� �� ?
                ret
                ; --- �祭� ��ଠ�쭮 - �� ��諨 => ࠧ������ ULOG, ��⠢�� ����� � Count++
@@not_found:    ; ࠧ�������
                push    edi
                push    esi
                shl     ecx, 3                  ; 8 ���� �� �����
                mov     edi, ecx                ; 㪠��⥫� �� ��砫� ᢮������� ����� � ULOG
		add	edi, 4
                sub     ecx, esi                ; �⮫쪮 ���� ���� ���������
                shr     ecx, 2                  ; � �⮫쪮 DD
                mov     esi, edi                ; \ ��� ����
                sub     esi, 8                  ; / �������
                push    ds
                mov     ds, ss:SEG_ULOG
                std                             ; ����� ᤢ��
                @_RMOVSD                        ; �������
                cld                             ; ����⠭���� 䫠� ���ࠢ�����
                pop     ds
                pop     esi
                pop     edi
@@insert:       ; ��⠢�塞
		mov     es:[esi], eax           ; ���������� ��
                mov     es:[esi + 4], edx       ; ���������� ���
                ; �⬥砥�
                inc     Cfg_ULOG_Count          ; �⬥砥� �� �⠫� �� ����� �����
                pop     ecx
                stc                             ; ��
                ret
                ; --- �� ᬮ��� ��⠢���
@@not_add:      pop     ecx
                clc                             ; �� ��
                ret
Add_U_Num       ENDP

; --- 㤠���� ����� �� SEG_ULOG ---------------------------------------------------------
; �室:         eax - ����� ����� � �ଠ� ����
; ��室:        CF  -  ��,  �� �믮�����
;               eax - ����� � �ଠ� ULOG
;               edx - ��� 㤠�񭭮�� �����
;               esi - 㪠��⥫� �� 㤠��� �����
;               ����� � SEG_KONF
;
Del_U_Num       PROC
                cmp     Cfg_ULOG_Count, 0       ; ���� �� 㤠���� ?
                je      @@not_del               ; ������ -
                @Format_U_Num                   ; �८�ࠧ㥬 �����
                call    Find_U_Num              ; �饬
                jnc     @@not_del               ; �� ��諨 -
                ; --- ��諨, �� 㤠���
                push    ecx
                push    edi
                push    esi
                mov     ecx, Cfg_ULOG_Count     ; �⮫쪮 ����஢ � SEG_ULOG
                shl     ecx, 3                  ; 8 ���� �� �����
                sub     ecx, esi                ; �⮫쪮 ���� ���� ���������
                shr     ecx, 2                  ; � �⮫쪮 DD
                mov     edi, esi                ; � �㤥� ���������
                add     esi, 8                  ; ��� �㤥� �������
                push    ds
                mov     ds, ss:SEG_ULOG
                mov     es, ss:SEG_ULOG
                cld                             ; ���� ��᫮� �� �ᯮ���� !
                @_RMOVSD                        ; �������
                pop     ds
                pop     esi
                pop     edi
                pop     ecx
                dec     Cfg_ULOG_Count          ; �⬥⨬ �� ���� ����� 㤠����
                stc                             ; ��
                ret
                ; --- �� 㤠����
@@not_del:      clc                             ; �� ��
                ret
Del_U_Num       ENDP

; === ��ᨨ � ���⨥� ������⮢ ����� =========================
;
; --- �ᯠ����� ����筮�� � ����� � 㪠����� ����
; �室:         eax     - ������ � ����� � �ଠ� ����
;               es:[si] - 㪠��⥫� �� ����: �ਧ���, ����� � ᠬ �����
;
Ux_UnPack       PROC
                push    ebx
                mov     BYTE PTR es:[si], PRIZN_STAN ; �ਧ��� ����� - �ᥣ�� �⠭�� !
                mov     BYTE PTR es:[si + 1], 3      ; ����� ����� - 2 ���� �� � ����� � 1 �� � � �����
                cmp     ax, 10                       ;
                jb      @@clear_num                  ; � � ����� 1 - ���
                mov     BYTE PTR es:[si + 1], 4      ; ����� ����� - 2 ���� �� � ����� � 2 �� � � �����
                cmp     ax, 100
                jb      @@clear_num                  ; � � ����� 2 - ����
                mov     BYTE PTR es:[si + 1], 5      ; ����� ����� - ���� 2 ���� �� � ����� � 3 �� � � �����
                cmp     ax, 1000                     ; ����� ����- ��� �����姭��� ?
                jb      @@clear_num                  ; ���-
                mov     BYTE PTR es:[si + 1], 6      ; ������- ���� ��� ���� ���
@@clear_num:    ; --- ��⨬ ᠬ �����
                add     si, 2
                mov     DWORD PTR es:[si],     0ffffffffh
                mov     DWORD PTR es:[si + 4], 0ffffffffh
                mov     BYTE  PTR es:[si + 8], 0ffh
                ; --- �ᯠ�㥬 � �����
                and     eax, 00ffffffh          ; ����� ����� - �� �ᥣ�� 3 ���� !
                mov     ecx, eax                ; ��࠭��
                shr     eax, 16                 ; ���� � � �����
                aam
                shl     al, 4
                shr     ax, 4
                rol     al, 4
                mov     es:[si], al             ; ����� ��稭����� � ����� �����
                ; --- �ᯠ㪥� � ⥫�䮭� � �����
                mov     ax, cx                  ; ᫮�� � � ⥫�䮭� � �����
                xor     dx, dx
                mov     bx, 100
                div     bx
                ; ���� ��� ����
                aam
                shl     al, 4
                shr     ax, 4
                rol     al, 4
                ; ���� ��� ����
                xchg    dx, ax
                aam
                shl     al, 4
                shr     ax, 4
                rol     al, 4
                ; ᪫������
                shl     ax, 8
                mov     al, dl
                ; �஢��塞 �����
                cmp     cx, 1000
                jae     @@put_num
                cmp     cx, 100
                jae     @@3_digs
                cmp     cx, 10
                jae     @@2_digs
                ; --- 1 ���
                shr     ax, 12
                or      ax, 0FFF0h
                jmp     @@put_num
                ; --- 2 ����
   @@2_digs:    shr     ax, 8
                or      ax, 0FF00h
                jmp     @@put_num
                ; --- 3-� ����� �����
   @@3_digs:    shr     ax, 4
                or      ax, 0f000h
@@put_num:      mov     es:[si + 1], ax         ; ⥯��� � ⥫�䮭� � �����
                pop     ebx
                ret
Ux_UnPack       ENDP

; --- ��७�� ����筮�� � ����� �� ᮥ������� � ����
; �室:         dl              - 0 - upats_a_num, 1 - upats_b_num
;               fs:[edi]        - 㪠��⥫� �� ᮥ�������
;               es:[si]         - 㪠��⥫� �� ����
;
Ux_Session      PROC
                pushad
                mov     eax, DWORD PTR fs:[edi].upats_a_num
                mov     bx, sorm_attr_a
                or      dl, dl
                jz      @@begin
                mov     eax, DWORD PTR fs:[edi].upats_b_num
                mov     bx, sorm_attr_b
@@begin:        and     eax, 0ffffffh
                add     si, bx
                call    Ux_UnPack
                popad
                ret
Ux_Session      ENDP

; ===================-- ������������� ������ --===============================
; --- ᮧ����� �ᨨ ��� ������ � �� ��������� ����� � ����� �� ������� N�
;
Session_Nb_Ua_Create    PROC
                test    fs:[edi].soed_flags, sf_UPATS_A         ;
                jz      @@exit                                  ;
                push    eax dx
                ; --- ��稭��� ᮧ����� ��ᨨ
                mov     dl, 2                                   ;
                mov     eax, called_Ua_ptr
                @Session_Create                                 ;
                jnc     @@quit                                  ;
                ; --- ����ᨬ ��뢠�饣� �� ����� � ���� ��� � �
                xor     dl, dl                                  ;
                call    Ux_Session                              ;
                ; --- ��뢠�� ᮥ������� � ����
                mov     fs:[edi].called_Ua_ptr, si              ;
                ; --- � ⥯��� ��뢠�� ����
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ---
                ; ----{Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {END} -------
   @@quit:      pop     dx eax
                ; ---
@@exit:         ret
Session_Nb_Ua_Create    ENDP

; --- ᮧ����� �ᨨ ��� ������ � �� ��������� ����� � ����� �� ������� N�
;
Session_Nb_mg_Ua_Create PROC
                test    fs:[edi].soed_flags, sf_UPATS_A         ;
                jz      @@exit                                  ;
                push    eax dx
                ; --- ��稭��� ᮧ����� ��ᨨ
                mov     dl, 2                                   ;
                mov     eax, called_mg_Ua_ptr                   ;
                @Session_Create                                 ;
                jnc     @@quit                                  ;
                ; --- ����ᨬ ��뢠�饣� �� ����� � ���� ��� � �
                xor     dl, dl                                  ;
                call    Ux_Session                              ;
                ; --- ��뢠�� ᮥ������� � ����
                mov     fs:[edi].called_mg_Ua_ptr, si           ;
                ; --- � ⥯��� ��뢠�� ����
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ---
                ; ----{Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {END} -------
     @@quit:    pop     dx eax
                ; ---
  @@exit:       ret
Session_Nb_mg_Ua_Create ENDP
; -------------------------------
; ---  �������� ��ᨨ ��� ������ � �� ��������� �A �� ������� ������
; --- in:       gs:[bx]   - �-�� �� ��ꥪ�
;               fs:[edi]  - 㪠��⥫� �� ᮥ�������
Session_Na_Ub_Create    PROC
                push    eax dx
                ; --- �஡㥬 ᮧ���� ���� ->
                mov     dl, 2
                mov     eax, Ub_calling_ptr
                @Session_Create
                jnc     @@quit
                ; --- ����ᨬ ��뢠�饣� �� ����� � ���� ��� � �
                mov     dl, 1                                  ;
                call    Ux_Session
                ; --- {Paul.N} --- <29/04/13>
                @_Clear_tcop_dig                 ; 㡥६ ���� � ��� ��室� � ����
                ; --- {Paul.N} --- <END>
                ; --- ��뢠�� ᮥ������� � ����
                mov     fs:[edi].Ub_calling_ptr, si
                ; --- � ⥯��� ��뢠�� ����
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ---
                ; ----{Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {END} -------
     @@quit:    pop     dx eax
                ; ---
                ret
Session_Na_Ub_Create    ENDP
; -------------------------------

; ---  �������� ��ᨨ ��� ������ � ��������� ������ A �� �� ������� ��
; --- in:       gs:[bx]   - �-�� �� ��ꥪ�
;               fs:[edi]  - 㪠��⥫� �� ᮥ�������
Session_Ua_Nb_Create    PROC
                push    eax dx
                ; --- ᮧ��� ����
                xor     dl, dl                                  ;
                xor     eax, eax                                ;
                @Session_Create                                 ;
                jnc     @@quit                                  ;
                ; --- ����ᨬ ��뢠�饣� �� ����� � ���� ��� � �
                xor     dl, dl                                  ;
                call    Ux_Session                              ;
               ; -- ��뢠�� ᮥ������� � ����
                mov     fs:[edi].Ua_called_ptr, si              ;
                add     es:[si].sorm_soed, Ua_called_ptr        ;
                ; --- � ⥯��� ��뢠�� ����
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ----{Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {END} -------
    @@quit:     pop     dx eax
                ; ---
                ret
Session_Ua_Nb_Create    ENDP
; ---  �������� ��ᨨ ��� ������ � ��������� �A �� ��������� ������
; --- in:       gs:[bx]   - �-�� �� ��ꥪ�
;               fs:[edi]  - 㪠��⥫� �� ᮥ�������
Session_calling_Ub      PROC

                test    fs:[edi].soed_flags, sf_UPATS_B
                jz      @@exit
                push    eax dx
                ; ---  �������� ���� ��� ����� �
                xor     dl, dl
                mov     eax, calling_Ub_ptr
                @Session_Create
                jnc     @@quit
                ; --- ����ᨬ ��뢠�饣� �� ����� � ���� ��� � �
                mov     dl, 1                    ;
                call    Ux_Session               ;
                ; --- {Paul.N} --- <29/04/13>
                @_Clear_tcop_dig                 ; 㡥६ ���� � ��� ��室� � ����
                ; --- {Paul.N} --- <END>
                ; --- ��뢠�� ᮥ������� � ����
                mov     fs:[edi].calling_Ub_ptr, si              ;
                ; --- � ⥯��� ��뢠�� ����
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ---
                ; ----{Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {END} -------
      @@quit:   pop     dx eax
                ; ---
    @@exit:     ret
Session_calling_Ub      ENDP

; --- �஢��塞 �������� �� ����� �����
; �室:         gs      - SEG_OBJ
;               eax     - ������ � � �����
; ��室:        gs:[bx] - 㪠��⥫� �� �������� ��ꥪ�
;
Ux_Find         PROC
                cmp     Obj_Count, 0            ;
                je      @@exit                  ; ⠡��� ���� !
                ; ---
                push    ds
                pop     es
                lea     si, UPATS_Buff
                call    Ux_UnPack
                ; ---
                lea     bx, Objects             ;
                mov     cx, Obj_Count           ;
@@find:         cmp     WORD PTR gs:[bx].obj_type, 0ffffh  ;
                je      @@next                  ;
                cmp     gs:[bx].obj_type, 0ffh  ;
                jne     @@next                  ;
                mov     edx, es:[si]            ;
                cmp     edx, DWORD PTR gs:[bx].obj_tel_num
                je      @@found                 ;
@@next:         add     bx, _OBJ_SIZE           ;
                loop    @@find                  ;
                clc                             ;
                ret
                ; ---
@@found:        stc                             ;
@@exit:         ret
Ux_Find         ENDP

; --- �஢�ઠ �������� �� ��뢠�騩 � �� �����
;
Ua_Received     PROC
                test    fs:[edi].soed_flags, sf_UPATS_CALLED    ;
                jnz     @@exit                                  ;
                ; ---
                test    fs:[edi].soed_flags, sf_UPATS_A         ;
                jz      @@exit                                  ;
                ; ---
                test    fs:[edi].soed_flags, sf_NUM_FULL        ;
                jz      @@exit;;;@@continue                              ;

                ;test    fs:[edi].soed_flags, sf_UPATS_B         ;
                ;jz      @@exit                                  ;
                ; ---
                ;;;@@continue:
                or      fs:[edi].soed_flags, sf_UPATS_CALLED    ;
                cmp     Obj_Count, 0                            ;
                je      @@exit                                  ; ⠡��� ���� !
                mov     gs, ss:SEG_OBJ                          ;
                ; --- �饬 ��뢠�饣� �� ����� � ᯨ᪥ ��ꥪ⮢
                mov     eax, DWORD PTR fs:[edi].upats_a_num     ;
                push    cx si edx
                push    es
                call    Ux_Find                                 ;
                pop     es
                pop     edx si cx
                jnc     @@exit                                  ; �� �����
                ; --- ��諨, �த�����!
                ; �� � - ����� ��� ����� ⥫�䮭 ?
                test    fs:[edi].soed_flags, sf_UPATS_B         ;
                jz      @@usual_Nb                              ; �������
                ; --- ᮧ��� ����
                xor     dl, dl                                  ;
                xor     eax, eax                                ;
                @Session_Create                                 ;
                jnc     @@exit                                  ;
                ; --- ����ᨬ ��뢠�饣� �� ����� � ���� ��� � �
                xor     dl, dl                                  ;
                call    Ux_Session                              ;
                ; �� � �����
                mov     dl, 1                                   ;
                call    Ux_Session                              ;
                ; --- {Paul.N} --- <29/04/13>
                @_Clear_tcop_dig                 ; 㡥६ ���� � ��� ��室� � ����
                ; --- {Paul.N} --- <END>
                ; ��뢠�� ᮥ������� � ����
                mov     fs:[edi].upats_ab_ptr, si               ;
                add     es:[si].sorm_soed, upats_ab_ptr         ;
                ; --- � ⥯��� ��뢠�� ����
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ---- {Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {Paul} --- <END>
                ; --- �������� ⠪�� ���� ->
                ; --- {Paul.N} --- [27/05/13]
                test    fs:[edi].soed_flags, sf_CALL_UPATS
                jz      @@usual_Nb
                ; --- ⮫쪮 �� ����� ����� 㯠��!
                ; -- � ⮫쪮 � �� 112
                cmp     BYTE PTR ds:[bp], 112
                jne     @@exit
                ; --- �஡㥬 ���� ��(��த᪮�)
                push    eax edx bx cx
                push    gs
                push    edi
                mov     gs, SS:SEG_KONF
                mov     ax, ds:[bp + 7]
                shr     ax, 9
                and     ax, 0fh
                movzx   edi, BYTE PTR ds:[bp + 4]
                shl     di, 4
                or      di, ax
                and     edi, 7FFh
                shl     edi, 3
                movzx   bx, BYTE PTR gs:[edi]           ; -- �ᯮ���� ⨯ �����
                mov     cl, TAB_TIP_LN[bx]
                cmp     BYTE PTR gs:[edi], 0            ; ��뢠�騩 �� ���� ?
                jne     @@exit_1                        ; ��� - � �����-�� ����� ?
                mov     edi, gs:[edi + 1]               ; ���� ����� �����
                cmp     edi, Cfg_Free_Ptr
                jae     @@exit_1
                movzx   edx, WORD PTR ds:[bp + 7]       ; \ �����
                and     dx, 1ffh                        ; / �����
                test    FG_PROGRAM, _fg_prg_multy_pref
                jnz     @@New_conf
                mov     cl, 4                           ; ࠧ��� ����(old). ����� � ᤢ����
   @@New_conf:  shl     edx, cl                         ; ? ���� �� ���䨣���� �����
                add     edi, edx                        ; ���� ����� � SEG_KONF
                cmp     edi, Cfg_Free_Ptr               ;
                jae     @@exit_1                        ; �멤��
                mov     eax, gs:[edi + 10]
                cmp     eax, 0FFFFFFFFh
                je      @@exit_1                        ; ����� ���
                ; --- �ਢ���� ����� � �ଠ�� soed(sess)
                ror     al, 4
                ror     ah, 4
                ror     eax, 16
                ror     al, 4
                ror     ah, 4
                rol     eax, 16
                pop     edi
                pop     gs
                mov     edx, eax                ; ��࠭�� �����
                xor     bx, bx                  ; ���稪 �����
                mov     cx, 7                   ; max length
     @@circle:  cmp     dl, 0FFh
                je      @@to_len
                shr     edx, 4
                inc     bl
                loop    @@circle
     @@to_len:  cmp     bl, 0
                jz      @@exit_2                        ; �����४⭠� �����, �멤��
                cmp     LENGTH_NUM, bl
                jne     @@exit_2                        ; �����४⭠� �����, �멤��
                mov     fs:[edi]. attr_b, PRIZN_STAN    ; ���ਡ��
                mov     fs:[edi].len_b, bl              ; ����襬 �����
                mov     DWORD PTR fs:[edi].num_b, eax   ; � ᠬ �����
                pop     cx bx edx eax
                ; --- � ⥯��� ����� ᮧ������ ����
   @@usual_Nb:  call    Session_Ua_Nb_Create
                ret
@@exit_1:       pop     edi
                pop     gs
@@exit_2:       pop     cx bx edx eax
                ; --- {Paul.N} -- [THE END]
@@exit:         ret
Ua_Received     ENDP

; --- �஢�ઠ �������� �� ��뢠��� � �� �����
;
Ub_Received     PROC
                test    fs:[edi].soed_flags, sf_UPATS_B         ;
                jz      @@exit                                  ;
                ; ---
                cmp     Obj_Count, 0                            ;
                je      @@exit                                  ; ⠡��� ���� !
                mov     gs, ss:SEG_OBJ                          ;
                ; --- �饬 ��뢠�饣� �� ����� � ᯨ᪥ ��ꥪ⮢
                mov     eax, DWORD PTR fs:[edi].upats_b_num     ;
                push    cx si edx
                push    es
                call    Ux_Find                                 ;
                pop     es
                pop     edx si cx
                jnc     @@exit                                  ; �� �����
                ; -- {Paul.N - add - 19/07/17}
 @@Go_Na_Ub:    test    fs:[edi].soed_flags, sf_ANI             ; ���� ��
                jnz     @@Call_Na_Ub;; @@exit                   ; ���� - �஡㥬 ��ࠡ���� � �� ���. � �� �����
                or      fs:[edi].soed_flags, sf_2VSK_Chelnok    ;<- �⬥⨬ "祫���", �.� ����� ���� ࠡ�⠥� ��� ����
                jmp     @@Go_Upats_A
                ; ---
@@Call_Na_Ub:   call    Session_Na_Ub_Create
                ; --- �������� ⠪�� ���� ->
                test    fs:[edi].soed_flags, sf_2VSK_Chelnok    ;<- �᫨ �� "祫���", � �� ����� ᭮�� �� �ନ�㥬
                jnz     @@exit
@@Go_Upats_A:   test    fs:[edi].soed_flags, sf_UPATS_A
                jz      @@exit;;;@@Go_Na_Ub                     ; ⮫쪮 ���� ��� � ��த᪮��
                ; --- � ��뢠�饣� ����� ������ ���� � � �����
                mov     dl, 2                                   ;
                xor     eax, eax                                ;
                @Session_Create                                 ;
                jnc     @@exit                                  ;
                ; ����ᨬ ��뢠�饣� �� ����� � ���� ��� � �
                xor     dl, dl                                  ;
                call    Ux_Session                              ;
                ; ����ᨬ ��뢠����� �� ����� � ���� ��� � �
                mov     dl, 1                                   ;
                call    Ux_Session                              ;
                ; --- {Paul.N} --- <29/04/13>
                @_Clear_tcop_dig                 ; 㡥६ ���� � ��� ��室� � ����
                ; --- {Paul.N} --- <END>
                ; ��뢠�� ᮥ������� � ����
                mov     fs:[edi].upats_ba_ptr, si               ;
                add     es:[si].sorm_soed, upats_ba_ptr         ;
                ; ��뢠�� ����
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ---- {Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {Paul} -- <END>
@@exit:         ret
Ub_Received     ENDP

