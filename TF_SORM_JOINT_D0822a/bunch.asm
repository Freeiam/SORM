Bunch_Define    PROC
                mov     al, 0ffh                        ;
                lea     si, Tranks                      ;
                mov     cx, _TRANK_SIZE * _MAX_TRANK    ;
BD_Clear:       mov     [si], al
                inc     si
                loop    BD_Clear
                ; ---
                ; --- EWSD
                lea     si, Tranks                      ;
                mov     WORD PTR [si].tr_number,    1   ;
                mov     DWORD PTR [si].tr_name,     044535745h;
                mov     BYTE PTR [si].tr_descr,     128 ;
                mov     BYTE PTR [si + 1].tr_descr, 1   ;
                mov     BYTE PTR [si + 2].tr_descr, 1   ;
                mov     BYTE PTR [si + 3].tr_descr, 13  ;
                mov     WORD PTR [si + 4].tr_descr, 0   ;
                ; --- �����
                add     si, _TRANK_SIZE
                mov     WORD PTR [si].tr_number,    2   ;
                mov     DWORD PTR [si].tr_name,     0cdc0c2cah ;
                mov     DWORD PTR [si + 4].tr_name, 0ffffffd2h ;
                mov     BYTE PTR [si].tr_descr,     64  ;
                mov     BYTE PTR [si + 1].tr_descr, 1   ;
                mov     BYTE PTR [si + 2].tr_descr, 1   ;
                mov     BYTE PTR [si + 3].tr_descr, 8   ;
                mov     WORD PTR [si + 4].tr_descr, 0   ;
                mov     BYTE PTR [si + 6].tr_descr, 32  ;
                mov     BYTE PTR [si + 7].tr_descr, 1   ;
                mov     BYTE PTR [si + 8].tr_descr, 1   ;
                mov     BYTE PTR [si + 9].tr_descr, 8   ;
                mov     WORD PTR [si + 10].tr_descr, 96 ;
                mov     BYTE PTR [si + 12].tr_descr, 32 ;
                mov     BYTE PTR [si + 13].tr_descr, 1  ;
                mov     BYTE PTR [si + 14].tr_descr, 1  ;
                mov     BYTE PTR [si + 15].tr_descr, 10 ;
                mov     WORD PTR [si + 16].tr_descr, 96 ;
                ; --- �.���襢�
                add     si, _TRANK_SIZE
                mov     WORD PTR [si].tr_number,    3   ;
                mov     DWORD PTR [si].tr_name,     0e0d82ec8h ;
                mov     DWORD PTR [si + 4].tr_name, 0e2e5f8ech ;
                mov     DWORD PTR [si + 8].tr_name, 0ffffffeeh ;
                mov     BYTE PTR [si].tr_descr,     32  ;
                mov     BYTE PTR [si + 1].tr_descr, 1   ;
                mov     BYTE PTR [si + 2].tr_descr, 1   ;
                mov     BYTE PTR [si + 3].tr_descr, 7   ;
                mov     WORD PTR [si + 4].tr_descr, 32  ;
                ; --- �.��⠩
                add     si, _TRANK_SIZE
                mov     WORD PTR [si].tr_number,    4   ;
                mov     DWORD PTR [si].tr_name,     0e0c12ecch ;
                mov     DWORD PTR [si + 4].tr_name, 0ffe9e0f2h ;
                mov     BYTE PTR [si].tr_descr,     32  ;
                mov     BYTE PTR [si + 1].tr_descr, 1   ;
                mov     BYTE PTR [si + 2].tr_descr, 1   ;
                mov     BYTE PTR [si + 3].tr_descr, 7   ;
                mov     WORD PTR [si + 4].tr_descr, 64  ;
                ; --- ��୮�ࠤ
                add     si, _TRANK_SIZE
                mov     WORD PTR [si].tr_number,    5   ;
                mov     DWORD PTR [si].tr_name,     0edf0e5c7h ;
                mov     DWORD PTR [si + 4].tr_name, 0e0f0e3eeh ;
                mov     DWORD PTR [si + 8].tr_name, 0ffffffe4h ;
                mov     BYTE PTR [si].tr_descr,     32  ;
                mov     BYTE PTR [si + 1].tr_descr, 1   ;
                mov     BYTE PTR [si + 2].tr_descr, 1   ;
                mov     BYTE PTR [si + 3].tr_descr, 8   ;
                mov     WORD PTR [si + 4].tr_descr, 64  ;
                ; --- �.�����᪨�
                add     si, _TRANK_SIZE
                mov     WORD PTR [si].tr_number,    6   ;
                mov     DWORD PTR [si].tr_name,     0e0d22ed0h ;
                mov     DWORD PTR [si + 4].tr_name, 0f7e8f0e2h ;
                mov     DWORD PTR [si + 8].tr_name, 0e8eaf1e5h ;
                mov     DWORD PTR [si + 12].tr_name,0ffffffe9h ;
                mov     BYTE PTR [si].tr_descr,     32  ;
                mov     BYTE PTR [si + 1].tr_descr, 1   ;
                mov     BYTE PTR [si + 2].tr_descr, 1   ;
                mov     BYTE PTR [si + 3].tr_descr, 9   ;
                mov     WORD PTR [si + 4].tr_descr, 0   ;
                ; --- ��஭殢��
                add     si, _TRANK_SIZE
                mov     WORD PTR [si].tr_number,    7   ;
                mov     DWORD PTR [si].tr_name,     0eef0eec2h ;
                mov     DWORD PTR [si + 4].tr_name, 0e2eef6edh ;
                mov     DWORD PTR [si + 8].tr_name, 0ffffe0eah ;
                mov     BYTE PTR [si].tr_descr,     32  ;
                mov     BYTE PTR [si + 1].tr_descr, 1   ;
                mov     BYTE PTR [si + 2].tr_descr, 1   ;
                mov     BYTE PTR [si + 3].tr_descr, 9   ;
                mov     WORD PTR [si + 4].tr_descr, 32  ;
                ; --- ������
                add     si, _TRANK_SIZE
                mov     WORD PTR [si].tr_number,    8   ;
                mov     DWORD PTR [si].tr_name,     0fcebe8c2h ;
                mov     DWORD PTR [si + 4].tr_name, 0fff1ecdfh ;
                mov     BYTE PTR [si].tr_descr,     32  ;
                mov     BYTE PTR [si + 1].tr_descr, 1   ;
                mov     BYTE PTR [si + 2].tr_descr, 1   ;
                mov     BYTE PTR [si + 3].tr_descr, 9   ;
                mov     WORD PTR [si + 4].tr_descr, 96  ;
                ; --- ��஢᪠�
                add     si, _TRANK_SIZE
                mov     WORD PTR [si].tr_number,    9   ;
                mov     DWORD PTR [si].tr_name,     0eef0e8cah ;
                mov     DWORD PTR [si + 4].tr_name, 0e0eaf1e2h ;
                mov     DWORD PTR [si + 8].tr_name, 0ffffffdfh ;
                mov     BYTE PTR [si].tr_descr,     64  ;
                mov     BYTE PTR [si + 1].tr_descr, 1   ;
                mov     BYTE PTR [si + 2].tr_descr, 1   ;
                mov     BYTE PTR [si + 3].tr_descr, 10  ;
                mov     WORD PTR [si + 4].tr_descr, 0   ;
                ; --- ����⮢᪠�
                add     si, _TRANK_SIZE
                mov     WORD PTR [si].tr_number,    10  ;
                mov     DWORD PTR [si].tr_name,     0f3eceed5h ;
                mov     DWORD PTR [si + 4].tr_name, 0f1e2eef2h ;
                mov     DWORD PTR [si + 8].tr_name, 0ffdfe0eah ;
                mov     BYTE PTR [si].tr_descr,     32  ;
                mov     BYTE PTR [si + 1].tr_descr, 1   ;
                mov     BYTE PTR [si + 2].tr_descr, 1   ;
                mov     BYTE PTR [si + 3].tr_descr, 10  ;
                mov     WORD PTR [si + 4].tr_descr, 64  ;
                ; ---
                ; ---
                ret
Bunch_Define    ENDP

; --- ���樠������ ��� ��� ��窨
;
Bunch_Init      PROC
                mov     al, 0ffh                        ;
                movzx   ecx, DESCR_SEG_BUNCH.lim        ;
                shl     ecx, 12                         ;
                dec     ecx                             ;
                mov     es, ss:SEG_BUNCH                ;
                xor     edi, edi                        ;
                @_RSTOSB                                ;
                ret
Bunch_Init      ENDP


Bunch_Calc      PROC
                mov     dl, 1                           ;
                lea     si, Tranks                      ;
                mov     es, ss:SEG_BUNCH                ;
                ; --- 横� �� �࠭��� �� 1 �� _TRANK_MAX
BC_Loop:        cmp     WORD PTR [si], 0ffffh           ;
                je      BC_Exit                         ;
                ; ��襬 ��ࠬ���� �࠭��
                mov     edi, _TRANK_PRMS_PTR            ;
                movzx   ebx, dl                         ;
                dec     ebx                             ;
                shl     ebx, 2                          ;
                mov     es:[edi + ebx], dl              ; ���浪��� ����� �࠭��
                mov     ax, [si].tr_number              ; \ �᫮��� �����
                mov     es:[edi + ebx + 1], ax          ; / ��窠
                ; ��襬 �������� �࠭��
                mov     edi, _TRANK_NAMES_PTR           ;
                push    edx                             ;
                movzx   ebx, dl                         ;
                dec     ebx                             ;
                mov     eax, _MAX_TRANK_NAME            ;
                mul     ebx                             ;
                pop     edx                             ;
                add     edi, eax                        ;
                xor     ebx, ebx                        ;
                mov     cx, _MAX_TRANK_NAME             ;
BC_Name:        mov     al, [si + bx].tr_name           ;
                mov     es:[edi + ebx], al              ;
                inc     ebx                             ;
                loop    BC_Name                         ;
                ; ��襬 ������� - ��뫪� �� ���. 䨧. ����ࠬ �� �࠭��
                push    si                              ;
                mov     cx, _MAX_TRANK_DESCR            ;
                add     si, tr_descr                    ;
BC_Refrs:       push    cx                              ;
                movzx   cx, [si]                        ;
                jcxz    BC_Refrs_Next                   ;
                cmp     cl, 0ffh                        ;
                je      BC_Refrs_Next                   ;
                movzx   eax, BYTE PTR [si + 1]          ;
                movzx   ebx, BYTE PTR [si + 2]          ;
                cmp     bl, KL_TIS                      ;
                jae     BC_Refrs_Next                   ;
                cmp     BYTE PTR [si + 3], 15           ;
                ja      BC_Refrs_Next                   ;
                shl     ebx, 4                          ;
                or      bl, [si + 3]                    ;
                cmp     WORD PTR [si + 4], 511          ;
                ja      BC_Refrs_Next                   ;
                shl     ebx, 9                          ;
                or      bx, [si + 4]                    ;
BC_Refrs_Loop:  mov     es:[ebx], dl                    ;
                add     ebx, eax                        ;
                loop    BC_Refrs_Loop                   ;
BC_Refrs_Next:  pop     cx                              ;
                add     si, _TRANK_DESCR_SIZE           ;
                loop    BC_Refrs                        ;
                pop     si                              ;
                ; ---
BC_Next:        add     si, _TRANK_SIZE                 ;
                inc     dl                              ;
                cmp     dl, _MAX_TRANK                  ;
                jbe     BC_Loop                         ;
BC_Exit:        ret
Bunch_Calc      ENDP
