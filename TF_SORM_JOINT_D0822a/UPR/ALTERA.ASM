ALTERA_IN1      PROC
                MOV     GS,SS:SEG_GS
                MOV     LENTH_FILE,_RBF_LEN
; ***********************************  ˆˆ–ˆ€‹ˆ‡€–ˆŸ ALTERA-ë
                MOV     ES,SS:SEG_TF
		XOR	EDI,EDI
                MOV     DL,BYTE PTR GS:[ADR_REG_PLIS]
                AND     DL,11111011b            ;‘Ž‘ˆ’œ ˆ’ D2:nCONFIG
                MOV     BYTE PTR GS:[ADR_REG_PLIS],DL                       ;˜€ƒ 0
;************************************************************************
                MOV     DL,BYTE PTR GS:[ADR_REG_PLIS]     ;Ž—ˆ’€’œ ˆ’ D3(NSTATUS)
                TEST    DL,00001000b            ;NSTATUS=1?       ;˜€ƒ 2
                JNZ     CF3_21                  ;……•Ž„,…‘‹ˆ „€ € ˜€ƒ 21
;************************************************************************
                OR      DL,00000100b            ;“‘’€Ž‚ˆ’œ D2:nCONFIG
                MOV     BYTE PTR GS:[ADR_REG_PLIS],DL                       ;˜€ƒ 3
;************************************************************************
                XOR     AX,AX                  ;ŠŽ‘’€’€  00    ;˜€ƒ 4
                JMP     SHORT   CF3_7_1
;************************************************************************
CF3_7:
                MOVZX   AX,ES:[EDI]              ;‡€ˆ‘œ ‘‹…„“ž™…ƒŽ €‰’€
		INC	EDI
;************************************************************************
CF3_7_1:        MOV     CX,8                                      ;˜€ƒ 5 ˆ 6
;************************************************************************
NEXT_BIT:
                AND     DL,11111101b            ;‘Ž‘ˆ’œ ˆ’ D1:DCLK
                MOV     BYTE PTR GS:[ADR_REG_PLIS],DL
                SHR     DL,1
                RCR     AL,1
                RCL     DL,1
                MOV     BYTE PTR GS:[ADR_REG_PLIS], DL    ;…ƒˆ‘’-€‰’
;************************************************************************
                OR      DL,00000010b            ;“‘’€Ž‚ˆ’œ D1    ;˜€ƒ 8
                MOV     BYTE PTR GS:[ADR_REG_PLIS], DL
;************************************************************************
CF3_1:          MOV     DL,BYTE PTR GS:[ADR_REG_PLIS]     ;Ž—ˆ’€’œ ˆ’ D3(NSTATUS)
                TEST    DL,00001000b            ;NSTATUS=0?        ˜€ƒ 11.1
                JZ      SHORT   CF3_12          ;‚›•Ž„
                LOOP    NEXT_BIT
                CMP     EDI,LENTH_FILE          ;‚‘… €‰’› ‡€ˆ‘€› ?
                JAe     SHORT   CF3_15          ;„€ -
                JMP     CF3_7
;************************************************************************
CF3_12:
                MOV     C_ERR,96
                JMP     SHORT   CF3_16          ;€ ‘Ž‘ …ƒˆ‘’€
;************************************************************************
CF3_15:
                MOV     DL,BYTE PTR GS:[ADR_REG_PLIS]     ;Ž—ˆ’€’œ         ˜€ƒ 15
                TEST    DL,00010000b
                JZ      SHORT   CF3_22          ;…‘‹ˆ D4=0, ……‰’ˆ € ˜€ƒ 22
;************************************************************************
                MOV     C_ERR,32                ;ŠŽ”ˆƒ“ˆŽ‚€ˆ… ‡€‚…˜…Ž
                MOV     CONF_ALTERA,1
                JMP     SHORT   CF3_16          ;‚›•Ž„
;************************************************************************
CF3_21:
                MOV     C_ERR,64                ;…’ ‘ˆƒ€‹€ Ž„’‚…†‹…ˆŸ nSTATUS
                JMP     SHORT   CF3_16          ;€ ‘Ž‘ …ƒˆ‘’€
;************************************************************************
CF3_22:
                MOV     C_ERR,0                  ;…’ ‘ˆƒ€‹€ ‡€‚…˜…ˆŸ CONF_DONE
CF3_16:
                MOV     DL,07h                   ;‘Ž‘ˆ’œ ‚‘… ˆ’›
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



