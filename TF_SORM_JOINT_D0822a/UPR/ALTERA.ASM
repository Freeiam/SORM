ALTERA_IN1      PROC
                MOV     GS,SS:SEG_GS
                MOV     LENTH_FILE,_RBF_LEN
; ***********************************  ������������� ALTERA-�
                MOV     ES,SS:SEG_TF
		XOR	EDI,EDI
                MOV     DL,BYTE PTR GS:[ADR_REG_PLIS]
                AND     DL,11111011b            ;�������� ��� D2:nCONFIG
                MOV     BYTE PTR GS:[ADR_REG_PLIS],DL                       ;��� 0
;************************************************************************
                MOV     DL,BYTE PTR GS:[ADR_REG_PLIS]     ;��������� ��� D3(NSTATUS)
                TEST    DL,00001000b            ;NSTATUS=1?       ;��� 2
                JNZ     CF3_21                  ;�������,���� �� �� ��� 21
;************************************************************************
                OR      DL,00000100b            ;���������� D2:nCONFIG
                MOV     BYTE PTR GS:[ADR_REG_PLIS],DL                       ;��� 3
;************************************************************************
                XOR     AX,AX                  ;���������  00    ;��� 4
                JMP     SHORT   CF3_7_1
;************************************************************************
CF3_7:
                MOVZX   AX,ES:[EDI]              ;������ ���������� �����
		INC	EDI
;************************************************************************
CF3_7_1:        MOV     CX,8                                      ;��� 5 � 6
;************************************************************************
NEXT_BIT:
                AND     DL,11111101b            ;�������� ��� D1:DCLK
                MOV     BYTE PTR GS:[ADR_REG_PLIS],DL
                SHR     DL,1
                RCR     AL,1
                RCL     DL,1
                MOV     BYTE PTR GS:[ADR_REG_PLIS], DL    ;�������-����
;************************************************************************
                OR      DL,00000010b            ;���������� D1    ;��� 8
                MOV     BYTE PTR GS:[ADR_REG_PLIS], DL
;************************************************************************
CF3_1:          MOV     DL,BYTE PTR GS:[ADR_REG_PLIS]     ;��������� ��� D3(NSTATUS)
                TEST    DL,00001000b            ;NSTATUS=0?        ��� 11.1
                JZ      SHORT   CF3_12          ;�����
                LOOP    NEXT_BIT
                CMP     EDI,LENTH_FILE          ;��� ����� �������� ?
                JAe     SHORT   CF3_15          ;�� -
                JMP     CF3_7
;************************************************************************
CF3_12:
                MOV     C_ERR,96
                JMP     SHORT   CF3_16          ;�� ����� ��������
;************************************************************************
CF3_15:
                MOV     DL,BYTE PTR GS:[ADR_REG_PLIS]     ;���������         ��� 15
                TEST    DL,00010000b
                JZ      SHORT   CF3_22          ;���� D4=0, ������� �� ��� 22
;************************************************************************
                MOV     C_ERR,32                ;���������������� ���������
                MOV     CONF_ALTERA,1
                JMP     SHORT   CF3_16          ;�����
;************************************************************************
CF3_21:
                MOV     C_ERR,64                ;��� ������� ������������� nSTATUS
                JMP     SHORT   CF3_16          ;�� ����� ��������
;************************************************************************
CF3_22:
                MOV     C_ERR,0                  ;��� ������� ���������� CONF_DONE
CF3_16:
                MOV     DL,07h                   ;�������� ��� ����
                MOV     BYTE PTR GS:[ADR_REG_PLIS],DL
                JMP     SHORT   $+2
                RET
                ENDP


ALTERA_IN1_1	PROC
                MOV     GS,SS:SEG_GS
		MOV	DL,CUR_ALTERA_CHIP
                mov     Alt_Cfg_Errors, 0

PLA20_Start:    mov     al, 10000100b
                shl     dl, 3
                or      al, dl
                mov     gs:[ALT_CONFIG_CTRL], al
                mov     ah, gs:[5000h]
                mov     ah, gs:[5000h]
                mov     ah, gs:[5000h]

                and     al, 11111011b
                mov     gs:[ALT_CONFIG_CTRL], al
                mov     ah, gs:[5000h]
                mov     ah, gs:[5000h]
                mov     ah, gs:[5000h]
                test    byte ptr gs:[ALT_CONFIG_CTRL], 00001000b
                jnz     PLA20_Err

                or      al, 00000100b
                mov     gs:[ALT_CONFIG_CTRL], al
                mov     ah, gs:[5000h]
                mov     ah, gs:[5000h]
                mov     ah, gs:[5000h]
                test    byte ptr gs:[ALT_CONFIG_CTRL], 00001000b
                jz      SHORT PLA20_Err

                MOV     ES,SS:SEG_TF
		XOR	EDI,EDI
		MOV	ECX,LENGTH_DOP_SN
PLA20_Next_Byte:
		MOV	AL,ES:[EDI]
		INC	EDI
                mov     gs:[ALT_CONFIG_DATA], al
                mov     ah, gs:[ALT_CONFIG_CTRL]
                test    ah, 00001000b
                jz      SHORT PLA20_Err
                DB      67h
                loop    PLA20_Next_Byte

                test    byte ptr gs:[ALT_CONFIG_CTRL], 00010000b
                jnz     SHORT PLA20_OK

PLA20_Err:
		inc     Alt_Cfg_Errors
                cmp     Alt_Cfg_Errors, 3
                jbe     PLA20_Start

                mov     cl, 0
                ret

PLA20_OK:       mov     cl, 255
                ret

		RET
		ENDP



