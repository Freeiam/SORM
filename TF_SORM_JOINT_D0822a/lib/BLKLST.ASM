;********************* ���� ⥫�䮭� � �୮� ᯨ�� ************
; �㭪�� ������ �᪠�� � ���. ��� ����� ⥫�䮭� ebx - 4 ����
; �᫨ ��諠 - �����-�� ffff ���� ����� � �ࠢ���
;
Find_Black_Tel  PROC

                push    eax
                push    ecx
                push    edx
                push    esi
                push    edi
                push    ds
                mov     ds, ss:SEG_BLK_LST      ; ᥣ���� �୮�� ᯨ᪠
                ; ---
                mov     esi, 0ffffh             ; �।��� (�᫨ * 4)
                mov     cx, 16                  ; ���ᨬ��쭮� ������⢮ ���権 ���᪠
_find_blck_t_loop:
                xor     eax, eax                ; 0
                mov     edi, [esi * 4]          ; Save to result
                mov     edx, edi
                and     edi, 0fffffff0h         ; ⥫�䮭 ��� ��㯯�
                cmp     ebx, edi                ;
                je      _find_t_it              ; ࠢ�� => ��諨, ��室��
                jb      _find_blck_t_b          ; ����� => �� 㬥��襭�� esi
                mov     eax, 10b                ; 㢥����� ESI xor 1100..00b
_find_blck_t_b: or      al, 01b                 ; 㬥����� ESI xor 0100..00b
                dec     cx                      ; �ॢ�頥� � ���樨 � �⥯��� ������
                shl     eax, cl                 ; ����� �����塞� ࠧ��� esi
                xor     esi, eax                ; ᫥���饥 ������� ��१�� �������
                inc     cx                      ; ����⠭���� � ���樨
                loop    _find_blck_t_loop

                ; �஢�ਬ ��᫥���� ࠧ
                mov     edi, [esi * 4]          ; Save to result
                mov     edx, edi
                and     edi, 0fffffff0h         ; ⥫�䮭 ��� ��㯯�
                cmp     ebx, edi                ;
                je      _find_t_it              ; ࠢ�� => ��諨, ��室��

_Notfind_t_it:  pop     ds
                mov     ebx, 0ffffffffh         ; �� ��襫
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     eax
                clc
                ret

_find_t_it:     ; �஢�ઠ �� ���� ��� ��㯯
                cmp     esi, 080000h-16*16*4-4
                ja      _Notfind_t_it
                ; �� ��� ���筮
                pop     ds
                mov     ebx, edx                ; ���� ����� 楫���� (� �ࠢ����)
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     eax
                stc
                ret
Find_Black_Tel  ENDP


Find_Black_Tel_Scr      PROC
                call    Find_Black_Tel
                retf
Find_Black_Tel_Scr      ENDP


;*****************************************************************************
; ���� �宦����� � ⥫�䮭� � "����" ᯨ᮪
; �ࠢ����� �� �������襬� �� ������⢠ ���࠭��� ��� � ����� � � ����� ᯨ᪥
; bl - ����� �ࠢ���, fs:edi - 㪠��⥫� �� SOED
;*****************************************************************************
Find_Tel_In_Rules       PROC
                push    esi
                push    edx
                push    ecx
                push    eax
                push    ds
                mov     ds, ss:SEG_BLK_LST      ; ᥣ���� �୮�� ᯨ᪠
                ; 㪠��⥫� �� �ࠢ��� 0
                mov     esi, 80000h             ; ��筥� � ���� ��୮�� ᯨ᪠ (512kb)
                sub     esi, 4 + 16 * 16 * 4    ; ����� �⨪�⪠ ����� 16 �ࠢ�� �� 16 DD
                and     ebx, 0fh                ; �� ��直� ��砩
                shl     ebx, 6                  ; 㬭����� �� ࠧ��� �ࠢ��� (16 * 4)
                add     esi, ebx                ; ⥯��� esi 㪠�뢠�� �� �ࠢ���
                ; � �ࠢ��� �� ����� 16 ����஢
                mov     ecx, 16
                ; �ਢ���� � �� �ଠ� SOED � �ଠ�� ��୮�� ᯨ᪠
                mov     ebx, DWORD PTR fs:[edi].num_b
                xchg    bh, bl                  ; �ਢ���� ����� �� ��ண� �ଠ� SOED
                rol     bl, 4                   ; �� �ଠ� ���� �ਢ���� � ��஬� �ଠ�� SOED
                rol     bh, 4
                ror     ebx, 16                 ; � �ଠ�� ��୮�� ᯨ᪠:
                xchg    bh, bl                  ; 1-�� � ��. ��ࠤ� ��. ����
                rol     bl, 4
                rol     bh, 4
                ; --- ᮡ�⢥��� ����
@@find:         lods    DWORD PTR ds:[esi]      ; ��㧨� ��।��� � �� �ࠢ���
                test    eax, 80000000h          ; ����� � >= 8 ��� ?
                jnz     @@not_found; �� - ���稫��� �ࠢ���
                ; ���� ������ �ࠢ����� ����஢
                push    ecx
                push    ebx
                mov     edx, eax
                ; ��������� ᨭ�஭����㥬 � �� SOED � � �� ��୮�� ᯨ᪠
                shl     eax, 4                  ; 㡨���� ����� �୮�� � � ��. ��ࠤ�
                or      eax, 0fh                ; � ��. ��ࠤ� ������塞 �����栬�
                ;
                shr     edx, 28                 ; ����砥� � dl ����� ��।���� � �� �ࠢ���
                mov     cl, 8                   ; max ����� � (8 ��� � DD)
                sub     cl, dl                  ; �⮫쪮 ��� �ࠢ� ���� �⪨����
                and     cl, 07h                 ; �᪫��⥫쭮 �� ��直� ��砩
                shl     cl, 2                   ; �� 4 ��� �� ���� (*4)
                shr     eax, cl                 ; �⪨�뢠�� ��譨� ���� �� � �ࠢ���
                shr     ebx, cl                 ; �⪨�뢠�� ��譨� ���� �� ���࠭���� �
                xor     eax, ebx                ; ᮡ�⢥��� �ࠢ�����
                pop     ebx
                pop     ecx
                jz      @@found                 ; ��襫
                loop    @@find                  ; �饬 �����
                ; ---
@@not_found:    pop     ds
                pop     eax
                pop     ecx
                pop     edx
                pop     esi
                clc
                ret
                ; ---
@@found:        pop     ds
                pop     eax
                pop     ecx
                pop     edx
                pop     esi
                stc
                ret
Find_Tel_In_Rules       ENDP

;*****************************************************************************
; ���� �宦����� � ⥫�䮭� � "����" ᯨ᮪
; �ࠢ����� �� �������襬� �� ������⢠ ���࠭��� ��� � ����� � � ����� ᯨ᪥
; �室: ds:[bp+1] - 㪠��⥫� �� �B � 235/171
;*****************************************************************************
Find_Tel_In_White_NEW   PROC
                        push    esi
                        push    edx
                        push    ecx
                        push    eax

                        PUSH    BP
                        MOVZX   CX, DS:[BP + 4]
                        ADD     BP, 5
                        ADD     BP, CX
                        MOV     EAX, DS:[BP + 1]
                        xchg    ah, al                  ; �ਢ���� ����� �� 235/171
                        ror     eax, 16                 ; � �ଠ�� ��୮�� ᯨ᪠:
                        xchg    ah, al                  ; 1-�� � ��. ��ࠤ� ��. ����

                        push    DS
                        mov     DS, SS:SEG_BLK_LST      ; ᥣ���� �୮�� ᯨ᪠

                        mov     esi, 80000h             ; ��筥� � ���� ��୮�� ᯨ᪠ (512kb)
                        sub     esi, 4+16*16*4          ; ����� �⨪�⪠ ����� 16 �ࠢ�� �� 16 DD

                        mov     ebx, eax
                        mov     ecx, 16                 ; � �ࠢ��� �� ����� 16 ����஢

Find_Tel_In_White_Loop_NEW:
                        lods    DWORD PTR DS:[ESI]      ; ��㧨� ��।��� � �� �ࠢ���

                        test    eax, 80000000h          ; ����� � >= 8 ��� ?
                        jnz     Find_Tel_In_White_not_Found_NEW; �� - ���稫��� �ࠢ���

                        ; ����� ������ �ࠢ����� ����஢
                        push    ecx
                        push    ebx
                        mov     edx, eax
                        ; --- ��������� ᨭ�஭����㥬 � �� 235/171 � � �� ��୮�� ᯨ᪠
                        shl     eax, 4                  ; 㡨���� ����� �୮�� � � ��. ��ࠤ�
                        or      eax, 0fh                ; � ��. ��ࠤ� ������塞 �����栬�

                        shr     edx, 28                 ; � dl - ����� � �� ��୮�� ᯨ᪠
                        CMP     DL, DS:[BP]
                        jbe     Find_Tel_In_White_Length_NEW
                        MOV     DL, DS:[BP]

Find_Tel_In_White_Length_NEW:
                        mov     cl, 8                   ; max ����� � (8 ��� � DD)
                        sub     cl, dl                  ; �⮫쪮 ��� �ࠢ� ���� �⪨����
                        and     cl, 07h                 ; �᪫��⥫쭮 �� ��直� ��砩
                        shl     cl, 2                   ; �� 4 ��� �� ���� (*4)

                        shr     eax, cl                 ; �⪨�렢�� ��譨� ���� �� ���࠭���� �
                        shr     ebx, cl                 ; �⪨�뢠�� ��譨� ���� �� � ��୮�� ᯨ᪠
                        XOR     eax, ebx                ; ᮡ�⢥��� �ࠢ�����
                        pop     ebx
                        pop     ecx
                        jz      Find_Tel_In_White_Found_NEW     ; ��襫

                        loop    Find_Tel_In_White_Loop_NEW

Find_Tel_In_White_not_Found_NEW:
                        pop     DS
                        POP     BP
                        pop     eax
                        pop     ecx
                        pop     edx
                        pop     esi
                        clc
                        ret
Find_Tel_In_White_Found_NEW:
                        pop     DS
                        POP     BP
                        pop     eax
                        pop     ecx
                        pop     edx
                        pop     esi
                        stc
                        ret
Find_Tel_In_White_NEW   ENDP
;*****************************************************************************
; �㭪�� ������ ��室��� �室�� �� ⥫�䮭 � �ࠢ���
; bl - ����� �ࠢ���, eax - �����  B c ���஬
;*****************************************************************************
Find_Tel_Rules_In_NEW   PROC
                        push    esi
                        push    edx
                        push    ecx
                        push    eax

                        PUSH    BP
                        MOVZX   CX, DS:[BP + 4]
                        ADD     BP, 5
                        ADD     BP, CX
                        MOV     EAX, DS:[BP + 1]        ; � B -  (����� BCD)
                        xchg    ah, al                  ; �ਢ���� ����� �� 235/171
                        ror     eax, 16                 ; � �ଠ�� ��୮�� ᯨ᪠:
                        xchg    ah, al                  ; 1-�� � ��. ��ࠤ� ��. ����

                        push    DS
                        mov     DS, SS:SEG_BLK_LST      ; ᥣ���� �୮�� ᯨ᪠

                        mov     esi, 80000h             ; ��筥� � ���� ��୮�� ᯨ᪠ (512kb)
                        sub     esi, 4+16*16*4          ; ����� �⨪�⪠ ����� 16 �ࠢ�� �� 16 DD

                        and     ebx, 0fh                ; �� ��直� ��砩
                        shl     ebx, 6                  ; 㬭����� �� ࠧ��� �ࠢ��� (*16*4)
                        add     esi, ebx                ; ⥯��� esi 㪠�뢠�� �� �ࠢ���
                        mov     ebx, eax
                        mov     ecx, 16                 ; � �ࠢ��� �� ����� 16 ����஢
Find_Tel_RULEs_Loop_NEW:
                        lods    DWORD PTR DS:[ESI]      ; ��㧨� ��।��� � �� �ࠢ���

                        test    eax, 80000000h          ; ����� � >= 8 ��� ?
                        jnz     Find_Tel_RULEs_IN_NotFNd_NEW; �� - ���稫��� �ࠢ���

                        ; ����� ������ �ࠢ����� ����஢
                        push    ecx
                        push    ebx
                        mov     edx, eax
                        ; --- ��������� ᨭ�஭����㥬 � �� 235/171 � � �� ��୮�� ᯨ᪠
                        shl     eax, 4                  ; 㡨���� ����� �୮�� � � ��. ��ࠤ�
                        or      eax, 0fh                ; � ��. ��ࠤ� ������塞 �����栬�

                        shr     edx, 28                 ; � dl - ����� � �� ��୮�� ᯨ᪠

                        mov     cl, 8                   ; max ����� � (8 ��� � DD)
                        sub     cl, dl                  ; �⮫쪮 ��� �ࠢ� ���� �⪨����
                        and     cl, 07h                 ; �᪫��⥫쭮 �� ��直� ��砩
                        shl     cl, 2                   ; �� 4 ��� �� ���� (*4)

                        shr     eax, cl                 ; �⪨�뢠�� ��譨� ���� �� ���࠭���� �
                        shr     ebx, cl                 ; �⪨�뢠�� ��譨� ���� �� � ��୮�� ᯨ᪠
                        XOR     eax, ebx                ; ᮡ�⢥��� �ࠢ�����
                        pop     ebx
                        pop     ecx
                        jz      Find_Tel_RLs_IN_FNd_NEW     ; ��襫

                        dec     ecx
                        jnz    Find_Tel_RULEs_Loop_NEW
;;;                        loop    Find_Tel_RULEs_Loop

Find_Tel_RULEs_IN_NotFNd_NEW:
                        pop     DS
                        POP     BP
                        pop     eax
                        pop     ecx
                        pop     edx
                        pop     esi
                        clc
                        ret
Find_Tel_RLs_IN_FNd_NEW:
                        pop     DS
                        POP     BP
                        pop     eax
                        pop     ecx
                        pop     edx
                        pop     esi
                        stc
                        ret
Find_Tel_Rules_In_NEW   ENDP
;*****************************************************************************
; �室: fs:edi    -  㪠��⥫� �� SOED
;       PEREM_EDI == edi
;       ds:bp     -  㪠��⥫� �� 118-�� �������
;*****************************************************************************
Black_List      PROC
; ---------------- =========== BLACK LIST ============ ----------------
                ; ��������� Mr_Alex
                ; ����������� ����祭�?

                pushad
@_test_debug_black 1
                test    WORD PTR fs:[edi].soed_flags, sf_ANI
                jz      BL_Exit
@_test_debug_black 2
                test    FG_PROGRAM, _fg_prg_bl_out
                JZ      BL_Exit
@_test_debug_black 3
                ; ���� �஢���� �����
                CMP     fs:[edi].len_a, 7       ; min 7 ���
                JB      BL_Exit
@_test_debug_black 4
                ; �஢��塞 ��� - �맮� ���� �� ⮩ �� ���� ��� �⮨� ��� ��� ��㣮� ?
                mov     ax, WORD PTR fs:[edi].num_a   ; ���� 4 ���� ���"� (���ਬ�� 52.84)
                and     ax, PREF_A_ZN_MASK
                cmp     ax, PREF_A_ZN           ; ��� ���� ��� ��� ࠢ�� ���� ���� �� ��� ?
                jne     BL_Exit                 ; ��� -
@_test_debug_black 5
                ; �८�ࠧ㥬 � � �� �ଠ� ��� � �ଠ� ��୮�� ᯨ᪠
                mov     ebx, DWORD PTR fs:[edi + 1].num_a
                xchg    bh, bl                  ; �ਢ���� ����� �� SOED
                rol     bl, 4
                rol     bh, 4
                ror     ebx, 16                 ; � �ଠ�� ��୮�� ᯨ᪠:
                xchg    bh, bl                  ; 1-�� � ��. ��ࠤ� ��. ����
                rol     bl, 4
                rol     bh, 4
                mov     PEREM_EBX, ebx          ; ����襬 ��� �뢮�� �� ��࠭
                shl     ebx, 4                  ; ��� ᤢ��, �⮡ �ଠ� �⠫ ��� �� FLASH !
                ; �஢��塞 ���� �� � � � ��୮� ᯨ᪥
                call    Find_Black_Tel
                jnc     BL_Exit                 ; ������� ��� � ��୮� ᯨ᪥
@_test_debug_black 6
                ; �஢�ਬ ���� ᯨ᮪
                push    ebx
                xor     bl, bl
                call    Find_Tel_In_Rules
                pop     ebx
                jc      BL_Exit                 ; � � � ����� ᯨ᪥ - ��室�� ��� �⡮�
@_test_debug_black 7
                ; ������� � �㫥��� �ࠢ��� ?
                and     ebx, 0fh                ; � bl - �ࠢ���
                cmp     bl, 0
                je      BL_Done                 ; 0 - ����뢠�� ��
@_test_debug_black 8
                ; �஢�ਬ ��㣨� �ࠢ���
                call    Find_Tel_In_Rules
                jnc     BL_Exit                 ; �� � ᯨ᪥
                ; �� ��� ⥫�䮭 - ����襬 �� � ����筨�
BL_Done:        @_test_debug_black 9
                MOV     ESI, EDI                ; ������ edi � esi
if 01
                @POLUCH_ADR_MO                  ;
                OR      DL,DL                   ; �஢�ਬ ���� ���� ?
                JNZ     BL_Exit                 ; ��� - 㩤��
@_test_debug_black 10
                ; ���� ���� - �� �� ����襬
                mov     eax, 070404ebh          ; ������� 169 - ������� � �୮� ᯨ᪥
                stosd
                MOV     AL, 169                 ; ������� 169 - ������� � �୮� ᯨ᪥
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; ����� ����筨��
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; ��᮫��� ࠡ�稩 �����
                STOSW
                mov     ax, fs:[esi].phys_a     ; ��᮫��� 䨧��᪨� ����� �
                STOSW
                mov     al, 2
                stosb
                @ZAPIS_OK_MO
endif
                ; --- ��� ᮢ���⨬��� � ��ன ���ᨥ� ------------------------------------------------------
                @POLUCH_ADR_MO                  ;
                OR      DL,DL                   ; �஢�ਬ ���� ���� ?
                JNZ     BL_Exit                 ; ��� - 㩤��
@_test_debug_black 11
                ; ���� ���� - �� �� ����襬
                MOV     AL, 169                 ; ������� 169 - ������� � �୮� ᯨ᪥
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; ����� ����筨��
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; ��᮫��� ࠡ�稩 �����
                STOSW
                mov     ax, fs:[esi].phys_a     ; ��᮫��� 䨧��᪨� ����� �
                STOSW
                @ZAPIS_OK_MO
                ; ---
                or      fs:[esi].soed_flags, sf_BLACK_DENY
                ; --- ᮡ�⢥��� ��������
                mov     ebx, PEREM_EBX
                mov     BL_CurrNum_Out, ebx     ; ����襬 ��� �뢮�� �� ��࠭
                inc     BL_Count_Out
BL_Exit:        popad
                ret
Black_List      ENDP
;===========================================================================
;---------------------------------------------------------------------------
;===========================================================================

Black_List_In   PROC
@_test_debug_black_In  1
                test    FG_PROGRAM, _fg_prg_bl_in      ; �室���� ��� ����뢠�� ?
                jz      BLI_Exit                       ; ��� -
@_test_debug_black_In  2
                pushad
                ; --- �८�ࠧ㥬 � � �� �ଠ� SOED � �ଠ� ��୮�� ᯨ᪠
                mov     al, fs:[edi].len_b
                cmp     al, LENGTH_NUM
                jne     BLI_End
@_test_debug_black_In  3
                ; �ਢ���� � � �� �ଠ�  SOED  � �ଠ� ��୮�� ᯨ᪠
                mov     ebx, DWORD PTR fs:[edi].num_b
                xchg    bl, bh                  ;
                rol     bl, 4                   ;
                rol     bh, 4
                ror     ebx, 16                 ;
                xchg    bh, bl                  ;
                rol     bl, 4
                rol     bh, 4
                ; � ����ᨬ��� �� ����� � � �������� ���訬� ��ࠬ� �� 7 ������
                cmp     al, 7
                je      BLI_Num_7
                mov     edx, PREF_A_FULL
                cmp     al, 6
                jne     BLI_Num_5
BLI_Num_6:      shr     ebx, 4
                shl     edx, 16
                and     edx, 0f0000000h
                or      ebx, edx
                jmp     BLI_Num_7
BLI_Num_5:      shr     ebx, 8
                shr     edx, 4
                rol     dh, 4
                shl     edx, 16
                and     edx, 0ff000000h
                or      ebx, edx
BLI_Num_7:      and     ebx, 0fffffff0h         ; ��� ��. ��ࠤ�, �⮡ �ଠ� �⠫ ��� �� FLASH!
                ; --- �஢��塞 ���� �� � � � ��୮� ᯨ᪥
                call    Find_Black_Tel
                jnc     BLI_End                 ; �� �� � ��୮� ᯨ᪥
@_test_debug_black_In  4
                mov     PEREM_EBX, ebx          ; ��� ������樨
                shr     PEREM_EBX, 4
                and     ebx, 0fh                ; � bl - �ࠢ���
                cmp     bl, 0
                jne     BLI_End                 ; �室 ������ ⮫쪮 ��� 0 �ࠢ���
@_test_debug_black_In  5
                ; --- ᮡ�⢥��� �⡮�
                mov     esi, edi
if 01
                @POLUCH_ADR_MO                  ;
                OR      DL,DL                   ; �஢�ਬ ���� ���� ?
                JNZ     BLI_End                 ; ��� - 㩤��
@_test_debug_black_In  6
                ; ���� ���� - �� �� ����襬
                mov     eax, 070404ebh          ; ������� 169 - ������� � �୮� ᯨ᪥
                stosd
                mov     al, 169
                stosb
                MOV     AL, BYTE PTR DS:[BP+1]  ; ����� ����筨��
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; ��᮫��� ࠡ�稩 �����
                STOSW
                mov     ax, fs:[esi].phys_a     ; ��᮫��� 䨧��᪨� ����� �
                STOSW
                mov     al, 1
                stosb
                @ZAPIS_OK_MO
endif
                ; --- ��� ᮢ���⨬��� � ��ன ���ᨥ� ------------------------------------------------------
                @POLUCH_ADR_MO                  ;
                OR      DL,DL                   ; �஢�ਬ ���� ���� ?
                JNZ     BLI_End                 ; ��� - 㩤��
@_test_debug_black_In  7
                ; ���� ���� - �� �� ����襬
                mov     al, 169
                stosb
                MOV     AL, BYTE PTR DS:[BP+1]  ; ����� ����筨��
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; ��᮫��� ࠡ�稩 �����
                STOSW
                mov     ax, fs:[esi].phys_a     ; ��᮫��� 䨧��᪨� ����� �
                STOSW
                @ZAPIS_OK_MO
                ; ---
                or      fs:[esi].soed_flags, sf_BLACK_DENY
                ; --- ᮡ�⢥��� ��������
                mov     ebx, PEREM_EBX
                mov     BL_CurrNum_In, ebx      ; ����襬 ��� �뢮�� �� ��࠭
                inc     BL_Count_In
BLI_End:        popad
BLI_Exit:       ret
                ENDP
;==========================================================================
;--------- "����" ��� ᯨ᮪ ��室�騩 --------------------------------
;==========================================================================
Black_List_NEW      PROC

                 ;����������� ����祭�?
                pushad
@_test_debug_black_q 1
                test    FG_PROGRAM, _fg_prg_bl_out     ; �᫨ �⪫�祭� ������ ��� ���筮
                JZ      _BL_Enable
@_test_debug_black_q 2
                ; ���� �஢���� �����
                MOVZX   CX, DS:[BP + 4]
                CMP     CX, 4
                JE      @@no_prefix
                CMP     CX, 5
                JE      @@prefix_ok
                JMP     _BL_Exit

@@no_prefix:
                MOV     EBX, DS:[BP + 5]
                JMP     @@find_black


@@prefix_ok:    @_test_debug_black_q 3
                MOV     BX, DS:[BP + 5]         ; ���� 4 ���� ���"� (���ਬ�� 52.84)
                ; �஢��塞 ��� - �맮� ���� �� ⮩ �� ���� ��� �⮨� ��� ��� ��㣮� ?
                mov     eax, PREF_A_FULL        ; ���쬥� ��䨪� (���ਬ�� 00.84.52.79)
                xchg    al, ah
                rol     al, 4
                rol     ah, 4
                ror     eax, 16
                shl     ax, 8
                shr     eax, 8
                rol     al, 4
                shr     eax, 12                 ; ��⠢�� ���� �� ���� (⮫쪮 ��� ����) 08.45
                xchg    bl, bh                  ;
                shr     bx, 4                   ;
                cmp     ax, bx                  ; ��� ���� ��� ��� ࠢ�� ���� ���� �� ��� ?
                jne     _BL_Enable              ; ��� -
               ; �८�ࠧ㥬 � � �� �ଠ� ��� � �ଠ� ��୮�� ᯨ᪠
                MOV     EBX, DS:[BP + 6]        ; � � -  (����� BCD)
@@find_black:   @_test_debug_black_q 4
                xchg    bh, bl                  ; �ਢ���� ����� �� 235/171
                ror     ebx, 16                 ; � �ଠ�� ��୮�� ᯨ᪠:
                xchg    bh, bl                  ; 1-�� � ��. ��ࠤ� ��. ����
                mov     PEREM_EBX, ebx          ; ����襬 ��� �뢮�� �� ��࠭
                shl     ebx, 4                  ; ��� ᤢ��, �⮡ �ଠ� �⠫ ��� �� FLASH !
                ; �஢��塞 ���� �� � � � ��୮� ᯨ᪥
                call    Find_Black_Tel
                mov     eax, ebx
                cmp     eax, 0ffffffffh
                jnc     _BL_Enable                 ; ������� ��� � ��୮� ᯨ᪥
@_test_debug_black_q 5
                ; �஢�ਬ ���� ᯨ᮪ (bl = 0)
                xor     ebx, ebx                 ; �㫥��� �ࠢ���
                call    Find_Tel_In_White_NEW
                jc      _BL_Enable_White         ; � � � ����� ᯨ᪥ - ��室�� �� 235_172
@_test_debug_black_q 6
                mov     ebx, eax
                and     ebx, 0fh                 ; � bl - �ࠢ��� ��� � �
                cmp     bl, 0
                jz      _BL_Disable              ; 0 - ����뢠�� ��
@_test_debug_black_q 7
                call    Find_Tel_Rules_In_NEW
                jnc     _BL_Enable               ; ��� � ᯨ᪥

                 ; �� ��� ⥫�䮭 - ����襬 �� � ����筨�
_BL_Disable:   @_test_debug_black_q 8
                xor     bx, bx
               @POLUCH_ADR_MO  TOTGE,1         ;
                OR      DL,DL                   ; �஢�ਬ ���� ���� ?
                JNZ     _BL_Exit                 ; ��� - 㩤��
@_test_debug_black_q 9
                ; ���� ���� - �� �� ����襬
                mov     eax, 070404ebh          ; ������� 169 - ������� � �୮� ᯨ᪥
                stosd
                MOV     AL, 169                 ; ������� 169 - ������� � �୮� ᯨ᪥
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; ����� ����筨��
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; ��᮫��� ࠡ�稩 �����
                STOSW
                mov     AX, 0FFFFH; ��᮫��� 䨧��᪨� ����� �
                STOSW
                mov     al, 2
                stosb
               @ZAPIS_OK_MO
                ; --- ��� ᮢ���⨬��� � ��ன ���ᨥ� ------------------------------------------------------
                xor     bx, bx
               @POLUCH_ADR_MO  TOTGE,1         ;
                OR      DL,DL                   ; �஢�ਬ ���� ���� ?
                JNZ     _BL_Exit                 ; ��� - 㩤��
                mov     BL_Return_Code, 0
                ; ���� ���� - �� �� ����襬
                MOV     AL, 169                 ; ������� 169 - ������� � �୮� ᯨ᪥
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; ����� ����筨��
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; ��᮫��� ࠡ�稩 �����
                STOSW
                mov     AX, 0FFFFH
                STOSW
               @ZAPIS_OK_MO
                ; --- ��� ᮢ���⨬��� � ��ன ���ᨥ� ------------------------------------------------------
                or      fs:[esi].soed_flags, sf_BLACK_DENY
                ; --- ᮡ�⢥��� ��������
                mov     ebx, PEREM_EBX
                mov     BL_CurrNum_Out, ebx     ; ����襬 ��� �뢮�� �� ��࠭
                inc     BL_Count_Out
@_test_debug_black_q 10
_BL_Exit:                ;;;@BL_Log_Return
                mov     BL_Return_Code, 1       ; �室�騩 ��� ᯨ᮪ �� ��ࠡ��뢠��
                popad
                ret
                ; -----------------

_BL_Enable:     mov     BL_Return_Code, 2
                popad
                ret
_BL_Enable_White:
                mov     BL_Return_Code, 1
                xor     bx, bx
               @POLUCH_ADR_MO  TOTGE,1         ;
                mov     eax, 040404ebh          ; ������� 235 - ࠡ�� � ���䨣��樥�
                stosd
                MOV     AL, 172                 ; ���������� 172 - ࠧ�襭�� �� ᮥ�������
                                                ; �� ��䨪��� � �� � ��
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; ����� ����筨��
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; ��᮫��� ࠡ�稩 �����
                STOSW
               @ZAPIS_OK_MO
                popad
                ret

Black_List_NEW      ENDP
;==============================================================================
;--------------- "����" ��� ᯨ᮪ �室�騩 -------------------------------
;==============================================================================
Black_List_In_NEW   PROC
@_test_debug_black_in_q 1
                test    FG_PROGRAM, _fg_prg_bl_in      ; �室���� ��� ����뢠�� ?
                jz      _BLI_Exit                      ; ��� -
@_test_debug_black_in_q 2
                pushad
                ; --- �८�ࠧ㥬 � � �� �ଠ� SOED � �ଠ� ��୮�� ᯨ᪠
                PUSH     BP
                MOVZX   CX, DS:[BP + 4]
                ADD     BP, 5
                ADD     BP, CX
                MOV     AL, DS:[BP]             ; �������� �����
                MOV     EBX, DS:[BP + 1]
                xchg    bh, bl                  ; �ਢ���� ����� �� 235/171
                ror     ebx, 16                 ; � �ଠ�� ��୮�� ᯨ᪠:
                xchg    bh, bl                  ; 1-�� � ��. ��ࠤ� ��. ����
                ; ---- 㧭��� ᪮�쪮 ��� ���࠭� - ? -----------
                CMP     AL, 3
                JE      @BYTE_3
                CMP     AL, 4
                JE      @BYTE_4
                JMP     _BLI_Enable_BP
@BYTE_3:
                TEST    BH, 0FH
                JNZ     @GO_FIVE
                MOV     AL, 6
                JMP     @GO_PROV
@BYTE_4:        TEST    BL, 0FH
                JNZ     @GO_SEVEN
                JMP     _BLI_Enable_BP
@GO_FIVE:       MOV     AL, 5
                JMP     @GO_PROV
@GO_SEVEN:      MOV     AL, 7

@GO_PROV:       @_test_debug_black_in_q 3
                CMP     AL, LENGTH_NUM
                JNE     _BLI_Enable_BP
                POP     BP
                cmp     al, 7
                je      _BLI_Num_And
                mov     edx, PREF_A_FULL
                xchg    dl, dh                  ;\
                rol     dl, 4                   ; \
                rol     dh, 4                   ;  \
                ror     edx, 16                 ;   �����⮢�� ��६����� ��� ��ଠ�쭮� ࠡ���
                shl     dx, 8                   ;  /
                shr     edx, 8                  ; /
                rol     dl, 4                   ;/
                shl     edx, 20
                and     edx, 0ff000000h
                shr     ebx, 8
                cmp     al, 5
                je      _BLI_Num_Or
                and     edx, 0f0000000h
                shl     ebx, 4
_BLI_Num_Or:    or      ebx, edx
_BLI_Num_And:   and     ebx, 0fffffff0h         ; ��� ��. ��ࠤ�, �⮡ �ଠ� �⠫ ��� �� FLASH!
                ;------------------------------------------
@_test_debug_black_in_q 4
                ; --- �஢��塞 ���� �� � � � ��୮� ᯨ᪥
                call    Find_Black_Tel
                jnc     _BLI_Enable                 ; �� �� � ��୮� ᯨ᪥
@_test_debug_black_in_q 5
                mov     PEREM_EBX, ebx          ; ��� ������樨
                shr     PEREM_EBX, 4
                and     ebx, 0fh                ; � bl - �ࠢ���
                cmp     bl, 0
                jne     _BLI_Enable                 ; �室 ������ ⮫쪮 ��� 0 �ࠢ���
@_test_debug_black_in_q 6
                ; --- ��� ������樨
                ; --- ᮡ�⢥��� �⡮�
                xor     bx, bx
               @POLUCH_ADR_MO  TOTGE,1         ;
                OR      DL,DL                   ; �஢�ਬ ���� ���� ?
                JNZ     _BLI_End                 ; ��� - 㩤��
@_test_debug_black_in_q 7
                ; ���� ���� - �� �� ����襬
                mov     eax, 070404ebh          ; ������� 169 - ������� � �୮� ᯨ᪥
                stosd
                mov     al, 169
                stosb
                MOV     AL, BYTE PTR DS:[BP+1]  ; ����� ����筨��
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; ��᮫��� ࠡ�稩 �����
                STOSW
                mov     AX, 0FFFFH              ; ��᮫��� 䨧��᪨� ����� �
                STOSW
                mov     al, 1
                stosb
               @ZAPIS_OK_MO
                ; --- ��� ᮢ���⨬��� � ��ன ���ᨥ� ------------------------------------------------------
                xor     bx, bx
               @POLUCH_ADR_MO  TOTGE,1         ;
                OR      DL,DL                   ; �஢�ਬ ���� ���� ?
                JNZ     _BLI_End                 ; ��� - 㩤��
@_test_debug_black_in_q 8
                ; ���� ���� - �� �� ����襬
                mov     al, 169
                stosb
                MOV     AL, BYTE PTR DS:[BP+1]  ; ����� ����筨��
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; ��᮫��� ࠡ�稩 �����
                STOSW
                mov     AX, 0FFFFH              ; ��᮫��� 䨧��᪨� ����� �
                STOSW
               @ZAPIS_OK_MO
                ; --- ��� ᮢ���⨬��� � ��ன ���ᨥ� ------------------------------------------------------
                or      fs:[esi].soed_flags, sf_BLACK_DENY
                ; --- ᮡ�⢥��� ��������
                mov     ebx, PEREM_EBX
                mov     BL_CurrNum_In, ebx      ; ����襬 ��� �뢮�� �� ��࠭
                inc     BL_Count_In
_BLI_End:        ;;;@BL_Log_Return
                popad
_BLI_Exit:      ret

_BLI_Enable_BP: POP     BP
                ; �� ��� ⥫�䮭 - ����襬 �� � ����筨�
_BLI_Enable:    xor     bx, bx
               @POLUCH_ADR_MO  TOTGE,1         ;
                OR      DL,DL                   ; �஢�ਬ ���� ���� ?
                JNZ     _BLI_End                  ; ��� - 㩤��
                ; ���� ���� - �� �� ����襬
                mov     eax, 040404ebh          ; ������� 235 - ࠡ�� � ���䨣��樥�
                stosd
                MOV     AL, 172                 ; ���������� 172 - ࠧ�襭�� �� ᮥ�������
                                                ; �� ��䨪��� � �� � ��
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; ����� ����筨��
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; ��᮫��� ࠡ�稩 �����
                STOSW
               @ZAPIS_OK_MO
                popad
                ret


Black_List_In_NEW   ENDP
;==========================================================================


