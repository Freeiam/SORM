
L1_EDINICAD     PROC    FAR
                CALL    L1_EDINICA
                RETF
                ENDP
;                           èÖêÖÑÄóÄ çÄ 1 ìêéÇÖçú
L1_EDINICA:
                MOV     DI,SLED_LSSU[BX]
                CMP     DI,POS_LSSU[BX]
                JNE     PER_LSSU
                MOV     DI,SLED_COM[BX]
                CMP     DI,POS_COM[BX]
                JNE     PER_COM
                TEST    FG_LAPB[BX],M_OR_7
           ;     JNZ     NET_POKETA
                CMP     SOST_SDL[BX],6
                JB      NET_POKETA
                @PEER
                JNZ     SHORT NET_POKETA
                MOV     AL,BYTE PTR FSNT[BX]
                CMP     AL,BYTE PTR FSNL[BX]             ;èÄäÖí Öëíú?
                JE      SHORT NET_POKETA
                AND     AX,K
                MOV     DI,BX
                SHL     DI,SHL_K
                OR      DI,AX
                SHL     DI,SHL_BUF
                LEA     SI,OFFSET BUF_LEV1[DI]
                MOV     AL,BYTE PTR FSNT[BX]
                SHL     AL,1
                MOV     DS:[SI+3+LEV2],AL
                MOV     AH,BYTE PTR BSNT[BX]
                SHL     AH,1
;                TEST    FG_LAPB[BX],M_OR_14
;                JNZ     SHORT DL_1_1_2
                SHL     AH,4
                OR      DS:[SI+3+LEV2],AH
                MOV     AL,BYTE PTR USER[BX]
                SHL     AL,1
;                XOR     AL,COM_X_25
                XOR     AL,11B
                MOV     DS:[SI+2+LEV2],AL
;                JMP     SHORT   DL_1_1_3
;DL_1_1_2:
;                MOV     DS:[SI+4+LEV2],AH
DL_1_1_3:
                MOV     CX,DS:[SI]
                AND     CX,7FH
                ADD     SI,2
                INC     FSNT[BX]
                AND     FSNT[BX],MOD_FSN
                RET
NET_POKETA:
                MOV     CX,OSHID
                RET
RET_L1_ED:
                RET
PER_LSSU:
                MOV     SI,BX
                SHL     SI,2
                LEA     SI,BUF_POK[SI]
                MOV     AL,BYTE PTR RAB_TEI[BX]
                MOV     DS:[SI+LEV2],BYTE PTR AL        ;Ñãü ISDN - TEI
                MOV     AL,BYTE PTR SAPI_L2[BX]
                MOV     DS:[SI],AL                      ;SAPI
                INC     SLED_LSSU[BX]
                AND     SLED_LSSU[BX],M_4
                SHL     BX,1
                MOV     AL,BUF_LSSU[DI+BX]
                MOV     DS:[SI+1+LEV2],AL
                SHR     BX,1
                MOVZX   DI,AL
                AND     DI,M_COM_U
                SHR     DI,2
                MOVZX   DI,TAB_U_C_O[DI]
                MOV     CX,2+LEV2
                SHL     DI,1
                AND     DI,3
                JMP     ADR_LSSU[DI]
COMANDA:
        ;
                MOV     AL,BYTE PTR USER[BX]
                SHL     AL,1
                XOR     AL,11B
                MOV     DS:[SI],AL
                RET
OTVET:
        ;
                MOV     AL,BYTE PTR USER[BX]
                SHL     AL,1
                XOR     AL,01B
                MOV     DS:[SI],AL
                RET

PER_COM:
                MOV     SI,BX
                SHL     SI,2
                LEA     SI,BUF_POK[SI]
                INC     SLED_COM[BX]
                AND     SLED_COM[BX],M_4
                MOV     AL,BYTE PTR USER[BX]
                SHL     AL,1
                XOR     AL,COM_X_25
                MOV     DS:[SI],AL
                SHL     BX,1
                MOV     AL,BUF_COML2[DI+BX]
                SHR     BX,1
                MOV     DS:[SI+1+LEV2],AL
                OR      DS:[SI+1+LEV2],BYTE PTR 1
                AND     DS:[SI+1+LEV2],BYTE PTR 11111101B
                MOV     AH,AL
                AND     AH,10B
                XOR     DS:[SI],AH
                MOV     AH,BYTE PTR BSNT[BX]
                SHL     AH,1
                AND     AL,1
                OR      AH,AL
                SHL     AH,4
                OR      DS:[SI+1+LEV2],AH
                MOV     CX,2
                RET



;
;----------------------éëçéÇçéÖ ìèêÄÇãÖçàÖ èêàÖåéå
;
SIG_ED_LEV1D    PROC    FAR
                CALL    SIG_ED_LEV1
                RETF
                ENDP


;                       èêàÖå ë ìêéÇÖçü 1
SIG_ED_LEV1:
                                                ;çÄ ÇïéÑÖ:-DS:SI-èéáàñàü ëàÉ.
                CMP     CX,2+LEV2
                JB      RET_SIG_ED
                MOV     AH,21                   ;I-COMMAND
                TEST    DS:[SI+1+LEV2],BYTE PTR 1
                JZ      SDL_GO
                TEST    DS:[SI+1+LEV2],BYTE PTR 2
                JNZ     NE_NUM_POKET
                MOVZX   DI,DS:[SI+1+LEV2]
                AND     DI,M_COM_S
                SHR     DI,2
                MOV     AH,TAB_S[DI]
                JMP     SDL_GO
RET_SDL2:
                RET

NE_NUM_POKET:                           ;éÅêÄÅéíäÄ çÖçìåÖêéÇÄççõï äéåÄçÑ
                MOVZX   DI,DS:[SI+1+LEV2]
                AND     DI,M_COM_U
                SHR     DI,2
                MOV     AH,TAB_U[DI]
                CMP     AH,0
                JE      UI_GO
                JMP     SDL_GO
RET_SIG_ED:
                RET

UI_GO:
                TEST    FG_LAPB[BX],M_OR_15
                JNZ     RET_SIG_ED
                MOVZX   DI,TAB_UI
                JMP     ADR_UI[DI]

UI_REQ:                         ;DL-DATA REQUEST-0
                OR      FG_LAPB[BX],M_OR_8
                @LSSU_TX_O      UI
                RET
UI_FRAME:                       ;       -1
                OR      FG_LAPB[BX],M_OR_9
                MOV     P[BX],0
                @LSSU_TX_C      UI
                RET
UI_COMMAND:
                OR      FG_LAPB[BX],M_OR_9
                @LSSU_TX_O      UI
                RET


