; 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様融
; �  ����:         CTO_Drv.ASM                                                 �
; �  蹍������痰:                                                              �
; �  �ム瓱�:                                                                   �
; 把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超
; �  �爛＜モ:      CTO_Drv.DAT                                                 �
; 把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超
; �  ��Л�腑�┘:   ����▲� ������ ���                                          �
; 把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超
; �  ��ぅ爨��:                                                                 �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様夕

;浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様融
;�     ==    �襷�� 瓱�縲��├�珮鉗┤ 礫���皀�� �� ����             ==    � �
;藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様夕 �
;  烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝
; --- か� 40 �� �爛琺�����
PUMA_CTO_40ms   PROC
                push    es
                mov     es, ss:SEG_PUMA_SCR
IF _MSP EQ TRUE
                mov     es:cto_curr_pos, 0
ELSE
                mov     es:cto_curr_pos, 16
ENDIF
                pop     es
                ret
PUMA_CTO_40ms   ENDP


;浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様融
;�     ==    甌＝癶キ�� む��▲�                                   ==    � �
;藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様夕 �
;  烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝烝
; --- か� 2 �� �爛琺�����
PUMA_CTO_2ms    PROC
                push    ds
                push    es
                push    fs
                pushad
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  ds:SCREEN_SEG_PUMA
                mov     fs, ss:SEG_PUMA
                ; --- ���瓷 ��｀��鉗ィ� ������ ���
                xor     edx, edx
                mov     edi, _PUMA_TRAN_OFFS
@@find:         cmp     fs:[edi].L3_state, _PL3_OFF ; �皖�鈑キ� !
                je      @@next
                cmp     fs:[edi].pt_type, _PT_CTO   ;
                je      @@found
@@next:         add     edi, PUMA_Trans_Size
                add     edx, 2
                cmp     edx, _PUMA_CHNLS_CNT * 2 ; ┃筮爼�罔���諷 ������� � ゛��� * 2
                jb      @@find
                jmp     @@exit

@@found:        ; --- ��瘍驍 礫���皀��
                add     cto_curr_pos, 16
                cmp     cto_curr_pos, 320
                jb      @@RxD
                mov     cto_curr_pos, 0

@@RxD:          ; --- �爬飫 ����諷
                mov     ecx, 16
                push    ds
                push    edx
                movzx   esi, fs:[edi].pt_cto_in         ; \
                shl     esi, 9                          ;  \ ds:esi - ��
                add     esi, cto_curr_pos               ;  / ������矣珮
                mov     ds, ss:SEG_BABA                 ; /
                mov     es, ss:SEG_PUMA_DRV             ; \ es � edx - 瓮��キ�
                inc     edx                             ; / � � ＜筌�� � es
                call    RB_BPut
                pop     edx
                pop     ds

@@TxD:          ; ---  �ムイ���
                mov     es, ss:SEG_BABA                 ; \
                movzx   edi, fs:[edi].pt_cto_out        ;  \ es:edi - ��
                shl     edi, 9                          ;  / ������矣珮
                add     edi, cto_curr_pos               ; /
                mov     fs, ss:SEG_PUMA_DRV             ; fs � edx - 瓮��キ� � � �礒���� ＜筌�� � fs
                mov     ecx, 16                         ;
                call    RB_BGet                         ;
                sub     ecx, edx                        ; �ムイ��� ≡� 16 ���� ?
                jecxz   @@exit                          ; �� -
                mov     al, 80h                         ; \ ぎ′飫
                rep     stosb; BYTE PTR [edi]             ; / ��ぎ� 皋荐��

@@exit:         ; ---
                popad
                pop     fs
                pop     es
                pop     ds
                ASSUME  ds:DATA_AXS
                ret
PUMA_CTO_2ms    ENDP

