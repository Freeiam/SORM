; 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様融
; �  ����:         PUMA_Scr.ASM                                                �
; �  蹍������痰:  ���ア�罟┤ �.�.                                             �
; 把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超
; �  �甎��讌礇�:   PUMA_Scr.DAT                                                �
; 把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超
; �  ��Л�腑�┘:   ��ゃ��, �ムイ�鉗┤ ├�キキ�� 蹣���� �� ����                 �
; 把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超
; �  ��ぅ爨��:     Scr_Mark_Diffs - �牀罐ゃ�� 痼�↓キ�� ��｀腑�� 蹣���� � ィ�  �
; �                                 ���┘� か� �´�珮Ε��� ィ� ├�キキ┤       �
; �                Scr_Send_Diffs - �ムイ��� ���ぅ��諷 ├�キキ┤ � ����        �
; �                @Clear_All_Diffs - ���牀� �腮痰�� ＜筌�� ├キキ┤           �
; �                @Close_Curr_Diff - ���牀� 甌Г�鉗┤ ��≪� ����瘡 � ＜筌爛   �
; �                                   ├�キキ┤                                �
; �                @Scr_Enabled     -                                          �
; �                @Scr_Disabled    -                                          �
; 把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超
; �  ��｀��:                                                                   �
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
; 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様夕


; --- ���瓷 ├�キキ┤ 蹣����, ���ぅ��襯 ├�キキ�� 筥�瓱珮鈞瘴 � ＜筌爛 ├�キキ┤
;     か� ��甄イ竡薀� �矚覊�� � ����
; √��:         fs:[esi] - 礫���皀�� �� ������ �甅�↓��� 蹣����
;               ��｀腮� �ムガキ�襯 ��ゃ��
; �諷��:        ��｀腮� �ムガキ�襯 ��ゃ�� [＜筌� ├�キキ┤ �爛Δ� ≡ィ�]
;
Scr_Mark_Diffs  PROC
                ASSUME  DS:SCREEN_SEG_PUMA
                ; ---
                cmp     bScrEnabled, 0
                je      SMD_Exit
                ; --- ���� ≡� ���ゥ ���ぅ��襯 ├�キキ�� �� 礬ゃ� � ����, ���諷 �� �薀�
SMD_Count:      cmp     dDiffsCount, 0
                jne     SMD_Exit
                ; --- �����讚襯 竅����→�
                @Clear_All_Diffs        ; �腮��ガ ＜筌� �皓�腮�
                cld
                push    ds              ; \ 瓮��キ�
                pop     es              ; / ���┬ 蹣����
                xor     ebx, ebx        ; 礫���皀�� �� ������ ├�キ餃��� ��痰� 蹣����
                mov     ecx, 80 * 50    ; � 蹣���� 50 痰牀� �� 80 瓱�〓���
                xor     edx, edx        ; か┃� ├�キ餃��� ��痰� 蹣���� (� ����� 瓱�〓� + �矜爬＜�)
                lea     edi, Screen_Copy; 礫���皀�� �� ������ ���┬ 蹣����
                ; --- ���瓷 �ム〓�� �メ����ぅ���
                repe    cmps    WORD PTR fs:[esi], WORD PTR es:[edi]
                jne     SMD_1st_Diff    ; メ碎 �メ����ぅ�┘
                ; --- ├�キキ┤ 蹣���� �� �´�珮Ε��
                ret                     ; �諷�え� ��腑�� �� �皴��←闖
                ; --- �皙ョ�ガ �ム〓� �´�珮Ε���� ├�キキ┘ 蹣����
SMD_1st_Diff:   sub     edi, 2          ; \ 〓о���ガ 礫���皀��
                sub     esi, 2          ; / �� �メ����ぅ�┘
                mov     ebx, esi        ; �����┃�ガ 礫���皀�� �� ������ �メ����ぅ���
                mov     edx, 2          ; ��� �┃━祠 ├�キ┼�瘡 �き� ��Ж罔� 蹣���� ┼� ヱ �矜爬＜�
                movs    WORD PTR es:[edi], WORD PTR fs:[esi] ; 痼�↓錺� ���┬ 蹣�����
                ; --- 罔�� ���瓷� ≡ュ ��甄イ竡薑� �メ����ぅ�┤
SMD_Loop:       repe    cmps    WORD PTR fs:[esi], WORD PTR es:[edi]
                jecxz   SMD_End
                jne     SMD_Next_Diff   ; �腑爛き�� �メ����ぅ�┘
                ; --- �牀茫� ▲瘡 蹣���
SMD_End:        or      edx, edx        ; メ碎 ���ぅ����, �� �� �皙ョキ��� � ＜筌爛 ├�キ│��閨� �゛�痰� 蹣����
                jz      SMD_Exit        ; ��� - �牀痰� �諷�え�
                @Close_Curr_Diff        ; ���琺��ガ ��甄イ�ゥ ├�キキ┘
SMD_Exit:       ret
                ; --- �÷���硅��ガ ��甄イ竡薑� ├�キキ�� 蹣����
SMD_Next_Diff:  sub     edi, 2          ; \ 〓о���ガ 礫���皀��
                sub     esi, 2          ; / �� �メ����ぅ�┘
                movs    WORD PTR es:[edi], WORD PTR fs:[esi] ; 痼�↓錺� ���┬ 蹣�����
                ; --- �牀▲瑙ガ メ碎 �� �� ���琺��� �゛�痰ォ �メ����ぅ���
                or      edx, edx        ; メ碎 �ェ��琺��� �゛�痰� ├�キキ�� 蹣����
                jz      SMD_New_Diff    ; ��� - �� �遏��� 轤� ��〓� ├�キキ┘
                ; --- �牀▲瑙ガ ��Кム 皀�竕ィ� ├�キキ��
                cmp     edx, 80 * 2     ; か┃� �� ���琺皰� �゛�痰� ├�キキ�� >= か┃� 痰牀�� 蹣���� ?
                jae     SMD_Close_Diff  ; �� - ��ぎ ���琺碎
                ; --- �牀▲瑙ガ メ碎 �� 甃諱� �皚メ皋 ��〓� ├�キキ┘ � 皀�竕ガ�
                mov     eax, esi        ;
                sub     eax, ebx        ; �� 皰腦� �爛るゃ薀�� �メ����ぅ��� ぎ 皰腦� 皰�讓� 艪� �´�珮Ε���� �メ����ぅ���
                sub     eax, edx        ; � 皀�ム� �� ����� �爛るゃ薀�� �メ����ぅ��� ぎ ������ ��〓��
                cmp     eax, 4 * 2      ; ｀�跏� 腑硅瑜� ��Ж罔� 蹣���� ?
                ja      SMD_Close_Diff  ; �� - �襭�きゥ ���琺碎 �爛るゃ薀� ├�キキ┘ � ����碎 ��〓�
                ; --- �牀痰� 磚カ�腮� ��Кム ├�キ餃��� �゛�痰�
                add     eax, 2          ;
                add     edx, eax        ;
                jmp     SMD_Loop        ; � �牀ぎ�Θ�
                ; --- 瘍���ガ 艪� ��茫� ���ユ ├�キ│茱�瘴 �゛�痰� 蹣����
SMD_Close_Diff: @Close_Curr_Diff        ;
                ; --- 瘍���ガ 艪� ��茫� �腑爛き竡 ├�キ│蓊釶� �゛�痰� 蹣����
SMD_New_Diff:   mov     ebx, esi        ; \ ヱ
                sub     ebx, 2          ; / ������
                mov     edx, 2          ; か┃� ���� �┃━��讚� 〓К�Ν��
                jmp     SMD_Loop        ;
Scr_Mark_Diffs  ENDP

; --- �ムイ��� ├�キキ┤, ��筥�瓱牀����諷 � ＜筌爛, � ���� --------------------
;     √��/�諷�� - ��｀腮� �ムガキ�襯 ��ゃ��
;
Scr_Send_Diffs  PROC
                ASSUME  DS:SCREEN_SEG_PUMA
                ; ---
                cmp     bScrEnabled, 0
                je      SSD_Exit
                ; --- ���� �皴��←閧� �ョィ� - ��腑�� �� ぅ��ガ
                cmp     dDiffsCount, 0          ;
                je      SSD_Exit                ;
                ; --- ���竍�ガ ����瘡 �� �腑爛き�� ├�キキ┬
SSD_Loop:       mov     ebp, dCurrDiffNum       ;
                cmp     ebp, dDiffsCount        ;
                jae     SSD_Stop                ;
                inc     dCurrDiffNum            ;
                shl     ebp, _DIF_REC_SHIFT     ;
                mov     esi, bufDiffs[ebp].diff_ptr
                mov     ecx, bufDiffs[ebp].diff_len
                ; --- �牀▲爬� ��Ν� �� ������碎 ���モ � ����
;                mov     eax, ecx
;                add     eax, 4 + 2 + 2 + 1      ; �むメ, �珮���+�������, 甃ラキ┘, か┃�
;                mov     edx, 0                  ; 〓 √�き�� ＜筌� ���1
;                mov     es, ss:SEG_PUMA         ; ≡� ＜筌�� Гメ�
;                call    RB_IsFull               ; ��ュ��� !
;                or      edx, edx                ;
;                jnz     SSD_Exit                ;
                ; --- 筮爼�牀���┘ ����≡���� 甌�♂キ�� �� ├�キキ┬ 蹣����
                lea     edi, messDiff           ; � �牀�ウ竄�膈覃 ＜筌�:
                mov     eax, dCtrlAddr          ; \ 1. �むメ: ��
IF _MSP EQ TRUE
                and     ah, 0f0h
                or      ah, 2
ENDIF
                mov     [edi], eax              ; /    �牀������ � ����
                mov     BYTE PTR [edi + 4], 80h ; 2. �珮���: ≡ギ♂┘ �����る ����
                mov     BYTE PTR [edi + 5], 83h ; 3. �������: 瓱�〓�� + �矜爬＜硅
                mov     [edi + 6], si           ; 4. �����モ�: 礫���皀��
                ; ---
                shr     ecx, 1
                mov     [edi + 8], cl           ; 5. �����モ�: 腮甄� ��� 瓱�〓� + �矜爬＜�
                ; ---
                cld
                push    ds
                pop     es
                add     edi, 9
                rep     movsw
                ; ---
                mov     ecx, bufDiffs[ebp].diff_len
                add     ecx, 4 + 2 + 2 + 1      ; �むメ, �珮���+�������, 甃ラキ┘, か┃�
                movzx   edx, bCtrlChnl          ; \ 〓 √�き�� ＜筌� ������
                shl     edx, 1                  ; / � ��皰牀�� 祚��←錺矚� ゛��
                lea     esi, messDiff           ; c筮爼�牀����覃 ���モ
                mov     es, ss:SEG_PUMA         ; ≡� ＜筌�� Гメ�
                call    RB_MPut                 ; ��ュ��� !
                jmp     SSD_Loop
                ; --- �皙ョ�ガ, 艪� �ムイ��� ≡� ├�キキ��
SSD_Stop:       @Clear_All_Diffs

SSD_Exit:       ret
Scr_Send_Diffs  ENDP
