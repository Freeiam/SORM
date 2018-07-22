; ����������������������������������������������������������������������������ͻ
; �  ����:         PUMA_Scr.ASM                                                �
; �  �ணࠬ����:  ������檨� �.�.                                             �
; ����������������������������������������������������������������������������Ķ
; �  �ᯮ����:   PUMA_Scr.DAT                                                �
; ����������������������������������������������������������������������������Ķ
; �  �����祭��:   �����, ��।��騩 ��������� �࠭� �� ����                 �
; ����������������������������������������������������������������������������Ķ
; �  ����ন�:     Scr_Mark_Diffs - ��楤�� �ࠢ����� ࠡ�祣� �࠭� � ���  �
; �                                 ������ ��� �����㦥��� ��� ���������       �
; �                Scr_Send_Diffs - ��।�� ��������� ��������� � ����        �
; �                @Clear_All_Diffs - ����� ���⪨ ���� ��������           �
; �                @Close_Curr_Diff - ����� ᮧ���騩 ����� ������ � ����   �
; �                                   ���������                                �
; �                @Scr_Enabled     -                                          �
; �                @Scr_Disabled    -                                          �
; ����������������������������������������������������������������������������Ķ
; �  �����:                                                                   �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; ����������������������������������������������������������������������������ͼ


; --- ���� ��������� �࠭�, �������� ��������� 䨪������� � ���� ���������
;     ��� ��᫥���饩 ���뫪� � ����
; �室:         fs:[esi] - 㪠��⥫� �� ��砫� �᭮����� �࠭�
;               ࠡ�稥 ��६���� �����
; ��室:        ࠡ�稥 ��६���� ����� [���� ��������� �०�� �ᥣ�]
;
Scr_Mark_Diffs  PROC
                ASSUME  DS:SCREEN_SEG_PUMA
                ; ---
                cmp     bScrEnabled, 0
                je      SMD_Exit
                ; --- ���� �� ࠭�� �������� ��������� �� 㩤�� � ����, ����� �� �饬
SMD_Count:      cmp     dDiffsCount, 0
                jne     SMD_Exit
                ; --- ��砫�� ��⠭����
                @Clear_All_Diffs        ; ��頥� ���� �⫨稩
                cld
                push    ds              ; \ ᥣ����
                pop     es              ; / ����� �࠭�
                xor     ebx, ebx        ; 㪠��⥫� �� ��砫� �����񭭮� ��� �࠭�
                mov     ecx, 80 * 50    ; � �࠭� 50 ��ப �� 80 ᨬ�����
                xor     edx, edx        ; ����� �����񭭮� ��� �࠭� (� ���� ᨬ��� + ���ਡ��)
                lea     edi, Screen_Copy; 㪠��⥫� �� ��砫� ����� �࠭�
                ; --- ���� ��ࢮ�� ��ᮢ�������
                repe    cmps    WORD PTR fs:[esi], WORD PTR es:[edi]
                jne     SMD_1st_Diff    ; ���� ��ᮢ�������
                ; --- ��������� �࠭� �� �����㦥��
                ret                     ; ��室�� ��祣� �� ��ࠢ���
                ; --- �⬥砥� ��ࢮ� �����㦥���� ��������� �࠭�
SMD_1st_Diff:   sub     edi, 2          ; \ ����頥� 㪠��⥫�
                sub     esi, 2          ; / �� ��ᮢ�������
                mov     ebx, esi        ; ���������� 㪠��⥫� �� ��砫� ��ᮢ�������
                mov     edx, 2          ; ��� ������ ���������� ���� ������ �࠭� ��� �� ���ਡ��
                movs    WORD PTR es:[edi], WORD PTR fs:[esi] ; �ࠢ�塞 ����� �࠭��
                ; --- 横� ���᪠ ��� ��᫥����� ��ᮢ�������
SMD_Loop:       repe    cmps    WORD PTR fs:[esi], WORD PTR es:[edi]
                jecxz   SMD_End
                jne     SMD_Next_Diff   ; ��।��� ��ᮢ�������
                ; --- ��諨 ���� �࠭
SMD_End:        or      edx, edx        ; ���� ���������, �� �� �⬥祭��� � ���� ����������� ������� �࠭�
                jz      SMD_Exit        ; ��� - ���� ��室��
                @Close_Curr_Diff        ; ����뢠�� ��᫥���� ���������
SMD_Exit:       ret
                ; --- ��ࠡ��뢠�� ��᫥���騥 ��������� �࠭�
SMD_Next_Diff:  sub     edi, 2          ; \ ����頥� 㪠��⥫�
                sub     esi, 2          ; / �� ��ᮢ�������
                movs    WORD PTR es:[edi], WORD PTR fs:[esi] ; �ࠢ�塞 ����� �࠭��
                ; --- �஢��塞 ���� �� �� ������� �����⥩ ��ᮢ�������
                or      edx, edx        ; ���� ��������� ������� ��������� �࠭�
                jz      SMD_New_Diff    ; ��� - �� ��� �� ����� ���������
                ; --- �஢��塞 ࠧ��� ⥪�饣� ���������
                cmp     edx, 80 * 2     ; ����� �� �����⮩ ������ ��������� >= ����� ��ப� �࠭� ?
                jae     SMD_Close_Diff  ; �� - ���� �������
                ; --- �஢��塞 ���� �� ��� �⭥�� ����� ��������� � ⥪�饬�
                mov     eax, esi        ;
                sub     eax, ebx        ; �� �窨 �।��饣� ��ᮢ������� �� �窨 ⮫쪮 �� �����㦥���� ��ᮢ�������
                sub     eax, edx        ; � ⥯��� �� ���� �।��饣� ��ᮢ������� �� ��砫� ������
                cmp     eax, 4 * 2      ; ����� ������ ����権 �࠭� ?
                ja      SMD_Close_Diff  ; �� - �룮���� ������� �।��饥 ��������� � ����� �����
                ; --- ���� 㢥��稬 ࠧ��� �����񭭮� ������
                add     eax, 2          ;
                add     edx, eax        ;
                jmp     SMD_Loop        ; � �த�����
                ; --- ��⠥� �� ��諨 ����� �������襩�� ������ �࠭�
SMD_Close_Diff: @Close_Curr_Diff        ;
                ; --- ��⠥� �� ��諨 ��।��� ������������ ������� �࠭�
SMD_New_Diff:   mov     ebx, esi        ; \ ��
                sub     ebx, 2          ; / ��砫�
                mov     edx, 2          ; ����� ���� �������쭮 ���������
                jmp     SMD_Loop        ;
Scr_Mark_Diffs  ENDP

; --- ��।�� ���������, ��䨪�஢����� � ����, � ���� --------------------
;     �室/��室 - ࠡ�稥 ��६���� �����
;
Scr_Send_Diffs  PROC
                ASSUME  DS:SCREEN_SEG_PUMA
                ; ---
                cmp     bScrEnabled, 0
                je      SSD_Exit
                ; --- ���� ��ࠢ���� ��祣� - ��祣� �� ������
                cmp     dDiffsCount, 0          ;
                je      SSD_Exit                ;
                ; --- ����砥� ������ �� ��।��� ���������
SSD_Loop:       mov     ebp, dCurrDiffNum       ;
                cmp     ebp, dDiffsCount        ;
                jae     SSD_Stop                ;
                inc     dCurrDiffNum            ;
                shl     ebp, _DIF_REC_SHIFT     ;
                mov     esi, bufDiffs[ebp].diff_ptr
                mov     ecx, bufDiffs[ebp].diff_len
                ; --- �஢�ਬ ����� �� ������� ����� � ����
;                mov     eax, ecx
;                add     eax, 4 + 2 + 2 + 1      ; ����, ��㯯�+�������, ᬥ饭��, �����
;                mov     edx, 0                  ; �� �室��� ���� ���1
;                mov     es, ss:SEG_PUMA         ; �� ���� �����
;                call    RB_IsFull               ; ���堫� !
;                or      edx, edx                ;
;                jnz     SSD_Exit                ;
                ; --- �ନ஢���� �����᪮�� ᮮ�饭�� �� ��������� �࠭�
                lea     edi, messDiff           ; � �஬������ ����:
                mov     eax, dCtrlAddr          ; \ 1. ����: ��
IF _MSP EQ TRUE
                and     ah, 0f0h
                or      ah, 2
ENDIF
                mov     [edi], eax              ; /    �ணࠬ�� � ����
                mov     BYTE PTR [edi + 4], 80h ; 2. ��㯯�: �᥮�騥 ������� ����
                mov     BYTE PTR [edi + 5], 83h ; 3. �������: ᨬ���� + ���ਡ���
                mov     [edi + 6], si           ; 4. ��ࠬ���: 㪠��⥫�
                ; ---
                shr     ecx, 1
                mov     [edi + 8], cl           ; 5. ��ࠬ���: �᫮ ��� ᨬ��� + ���ਡ��
                ; ---
                cld
                push    ds
                pop     es
                add     edi, 9
                rep     movsw
                ; ---
                mov     ecx, bufDiffs[ebp].diff_len
                add     ecx, 4 + 2 + 2 + 1      ; ����, ��㯯�+�������, ᬥ饭��, �����
                movzx   edx, bCtrlChnl          ; \ �� �室��� ���� ������
                shl     edx, 1                  ; / � ���ண� �ࠢ����� ����
                lea     esi, messDiff           ; c�ନ஢���� �����
                mov     es, ss:SEG_PUMA         ; �� ���� �����
                call    RB_MPut                 ; ���堫� !
                jmp     SSD_Loop
                ; --- �⬥砥�, �� ��।��� �� ���������
SSD_Stop:       @Clear_All_Diffs

SSD_Exit:       ret
Scr_Send_Diffs  ENDP
