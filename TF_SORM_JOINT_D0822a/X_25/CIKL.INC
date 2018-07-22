

MTP_GO:
                ; --- {Paul.N} --- [27/06/2013]
                ; -- при выдаче тестовых собщений был EXC(Кировская обл.)
                ; -- на всяк случай обнулим ECX !
                XOR     ECX, ECX
                ; --- {Paul.N} --- [END]
                MOV     CX,KL
                XOR     BX,BX
CN_PO_ISDN:
                PUSH    CX
                CALL    TIME_L3
                CALL    TIME_L2
                CALL    L4_L3_GO
                CALL    IN_ISDN
                CALL    OUT_ISDN
                ADD     BX,2
                POP     CX
                LOOP    CN_PO_ISDN

                TEST    FG_INIC,M_OR_0
                JZ      SHORT NET_INIC_OSTANOV
                MOV     AX,TIME_2MC
                SUB     AX,TIME_PEREZ
                CMP     AX,1000
                JB      SHORT NET_INIC_OSTANOV
                AND     FG_INIC,M_AND_0
                OR      FG_INIC,M_OR_1
                CALL    INIC_LEV2

NET_INIC_OSTANOV:
                RET

OUT_ISDN:
               ; TEST    FG_LAPB[BX],M_OR_7
               ; JZ      RET_LEV3
                CMP     SOST_SDL[BX],6
;!!!!!
                JNE     RET_LEV3
                MOV     AL,BYTE PTR FSNL[BX]
                MOV     DL,AL
                MOV     AH,BYTE PTR FSNF[BX]
                SUB     AL,AH
                AND     AX,K
                CMP     AX,OKNO_L2[BX]
                JNB     RET_LEV3
                XOR     AX,AX
                MOV     AL,DL
                AND     AX,K
                MOV     DI,BX
                SHL     DI,SHL_K
                OR      DI,AX
                SHL     DI,SHL_BUF
                LEA     DI,OFFSET BUF_LEV1[DI]
                MOV     ES,SS:SEG_AX
                XOR     CX,CX
                PUSH    BX
                PUSH    DI
                PUSH    ES
                ADD     DI,4
                CALL    ISDN_OUT
                MOV     AX,DI
                POP     ES
                POP     DI
                POP     BX
                SUB     AX,DI
                SUB     AX,4
                OR      AX,AX
                JZ      RET_LEV3
                CMP     AX,100
                JA      RET_LEV3
                ADD     AX,2
                CLD
                STOSW
                MOV     AX,FSNL[BX]
                INC     AX
                AND     AX,MOD_FSN
                MOV     FSNL[BX],AX
                RET

        ;-----------------------------------
RET_LEV3:
                RET
IN_ISDN:
;                TEST    FG_LAPB[BX],M_OR_7
;                JZ      SHORT RET_LEV3
                CMP     SOST_SDL[BX],6
                JNE     RET_LEV3
                MOV     AX,POS_POK[BX]
                CMP     AX,SLED_POK[BX]
                JE      SHORT RET_LEV3
                MOV     SI,BX
                SHL     SI,SHL_K                ; 2  - 8 пакетов
                OR      SI,AX
                SHL     SI,SHL_BUF              ; 7   - по 128 байт
                LEA     SI,BUF_LEV2[SI]         ; 2 * 8 * 256
                CLD
                LODSW
                MOV     CX,AX
                MOVZX   AX,DS:[SI+1+MODUL]
                AND     AX,0FH                  ; N(s) LAPB
                ADD     SI,2                    ; на ИОФ Х.25
                PUSH    BX
                PUSH    AX
                CALL    ISDN_IN
                POP     AX
                POP     BX
                SHR     AX,1
                INC     AX
                AND     AX,MOD_FSN
                CMP     BSNT[BX],AX
                JE      SHORT DL_IN_ISDN_1
                MOV     BSNT[BX],AX
                @COM_TX_O       RR
DL_IN_ISDN_1:
                INC     POS_POK[BX]
                AND     POS_POK[BX],K
                RET

TIME_L2:
                TEST    FG_LAPB[BX],M_OR_10
                JZ      SHORT DL_TIMI_1
                MOV     AX,TIME_2MC
                SUB     AX,OLD_TIMET200[BX]
                CMP     AX,POROG_T200
                JB      SHORT DL_TIMI_1
                @STOP_T200
                MOV     AH,15
                CALL    SDL_GO
DL_TIMI_1:
                TEST    FG_LAPB[BX],M_OR_11
                JZ      SHORT DL_TIMI_2
                MOV     AX,TIME_2MC
                SUB     AX,OLD_TIMET203[BX]
                CMP     AX,POROG_T203
                JB      SHORT DL_TIMI_2
                @STOP_T203
                MOV     AH,16
                CALL    SDL_GO
DL_TIMI_2:
                RET
MDL_GO:
                RET

DL_GO:
                MOVZX   DI,AH
                SHL     DI,1
                JMP     ADR_DL[DI]
RELEASE2:
                AND     FG_LAPB[BX],M_AND_7
                MOV     AH,0
                JMP     SDL_GO
ESTABL:
                OR      FG_LAPB[BX],M_OR_7
                RET
RET_DL:
                RET


