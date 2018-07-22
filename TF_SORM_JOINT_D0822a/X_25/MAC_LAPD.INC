

@START_T200     MACRO
                MOV     AX,TIME_2MC
                MOV     OLD_TIMET200[BX],AX
                OR      FG_LAPB[BX],M_OR_10
                ENDM

@STOP_T200      MACRO
                AND     FG_LAPB[BX],M_AND_10
                ENDM

@T200_RUNNING   MACRO
                TEST    FG_LAPB[BX],M_OR_10
                ENDM

@START_T203     MACRO
                MOV     AX,TIME_2MC
                MOV     OLD_TIMET203[BX],AX
                OR      FG_LAPB[BX],M_OR_11
                ENDM

@STOP_T203      MACRO
                AND     FG_LAPB[BX],M_AND_11
                ENDM

@COM_L3         MACRO
LOCAL           DL_1
;                MOV     FSNT_3[BX],0
;                MOV     FSNF_3[BX],0
;                MOV     P_R_LEV3[BX],0
;                MOV     OLD_P_R[BX],0
                MOV     AX,0
                CMP     SOST_L3[BX],6
                JNE     SHORT DL_1
                MOV     AX,3
DL_1:
;               MOV     SOST_L3[BX],AX
                ENDM

@LSSU           MACRO
LOCAL           DL_1,END_M
                SHL     DL,4
                OR      AL,DL
                MOV     DI,POS_LSSU[BX]
                CMP     DI,SLED_LSSU[BX]
                JE      SHORT DL_1
                MOV     BP,SLED_LSSU[BX]
                ADD     BP,BX
                ADD     BP,BX
                CMP     DS:BUF_LSSU[BP],AL
                JE      END_M
DL_1:
                SHL     BX,1
                MOV     BUF_LSSU[DI+BX],AL
                SHR     BX,1
                INC     POS_LSSU[BX]
                AND     POS_LSSU[BX],M_4
END_M:
                ENDM

@LSSU_TX_O      MACRO   LS
                MOV     AL,LS
                MOV     DL,BYTE PTR F[BX]
                MOV     F[BX],0
                @LSSU
                ENDM

@LSSU_TX_C      MACRO   LS
                MOV     AL,LS
                MOV     DL,BYTE PTR P[BX]
                MOV     P[BX],0
                @LSSU
                ENDM

@COM_TX         MACRO
LOCAL           DL_1,END_M
                OR      AX,DX
                MOV     DI,POS_COM[BX]
                MOV     BP,SLED_COM[BX]
                CMP     DI,BP
                JE      SHORT DL_1
                ADD     BP,BX
                ADD     BP,BX
                CMP     DS:BUF_COML2[BP],AL
                JE      SHORT END_M
DL_1:
                SHL     BX,1
                MOV     BUF_COML2[DI+BX],AL
                SHR     BX,1
                INC     POS_COM[BX]
                AND     POS_COM[BX],M_4
END_M:
                ENDM

@COM_TX_C       MACRO  TXX
                MOV     AL,TXX
                OR      AL,10B
                MOV     DL,BYTE PTR P[BX]
                MOV     P[BX],0
                @COM_TX
                ENDM

@COM_TX_O       MACRO  TXX
                MOV     AL,TXX
                AND     AL,11111101B
                MOV     DL,BYTE PTR F[BX]
                MOV     F[BX],0
                @COM_TX
                ENDM


@OWN            MACRO
                TEST    FG_LAPB[BX],M_OR_2
                ENDM
@UNIT_DATA      MACRO
                ENDM
@UI_FRAME       MACRO
                ENDM
@I_FRAME        MACRO
                ENDM
@PUT_IN         MACRO
                ENDM
@GET_NEXT       MACRO
                ENDM
@GET_LAST       MACRO
                ENDM
@VS_VA_K        MACRO
                ENDM
@SAVE_TEI       MACRO
                MOV     AX,TEI[BX]
                MOV     RAB_TEI[BX],AX
                ENDM
@DISCARD_INFORMATION    MACRO
                OR      FG_LAPB[BX],M_OR_6
                        ENDM
@VS_VA          MACRO
                MOV     AX,FSNF[BX]
                CMP     AX,FSNT[BX]
                ENDM
@RC_N200        MACRO
                CMP     RC[BX],N200
                ENDM


@CLEAR_EXCEPTION        MACRO
                        ENDM
@CLEAR_ACKNOWLEDGE      MACRO
                AND     FG_LAPB[BX],M_AND_4
                        ENDM
@SET_ACKNOWLEDGE        MACRO
                OR      FG_LAPB[BX],M_OR_4
                        ENDM
@ACKNOWLEDGE    MACRO
                TEST    FG_LAPB[BX],M_OR_4
                ENDM

@ESTABLISH      MACRO
                OR      FG_LAPB[BX],M_OR_7
                @CLEAR_EXCEPTION
                MOV     RC[BX],0
                MOV     P[BX],1
                @LSSU_TX_C SABME
                @COM_L3
                @START_T200
                @STOP_T203
                ENDM

@BAD_VSTAVKA    MACRO
                MOV     AX,FSNF[BX]
                MOV     FSNT[BX],AX
                ENDM

@TRANSMIT       MACRO
LOCAL           DL_,DL_2
                MOV     P[BX],1
                @OWN
                JNZ     SHORT DL_
                @COM_TX_C RR
                JMP     SHORT DL_2
DL_:
                @COM_TX_C RNR
DL_2:
                @CLEAR_ACKNOWLEDGE
                @START_T200
                ENDM

@ENQUIRY        MACRO
LOCAL           DL_1,DL_2
                MOV     F[BX],1
                @OWN
                JZ      SHORT DL_1
                @COM_TX_O RNR
                JMP     SHORT DL_2
DL_1:
                @COM_TX_O RR
DL_2:
                @CLEAR_ACKNOWLEDGE
                ENDM

@INVOKE         MACRO
                MOV     AX,FSNF[BX]
                MOV     FSNT[BX],AX
                ENDM
@NR_ERROR       MACRO
                MOV     AH,1
                CALL    MDL_GO
                @ESTABLISH
                @CLEAR_LAYER_3
                ENDM
@VA_NR_VS       MACRO
LOCAL           DL_1
                MOV     AH,DS:[SI+1+LEV2+MODUL]
                SHR     AH,1
                TEST    FG_LAPB[BX],M_OR_14
                JNZ     SHORT DL_1
                SHR     AH,4
DL_1:
                MOV     DL,BYTE PTR FSNT[BX]         ;BSNR- ÇõïéÑàí áÄ êÄåäà
                MOV     DH,DL               ;
                SUB     DL,BYTE PTR FSNF[BX]         ;  FSNT< ! <=FSNX
                AND     DL,MOD_FSN      ;
                SUB     DH,AH           ;
                AND     DH,MOD_FSN      ;
                CMP     DH,DL           ;
                ENDM

@NS_VR          MACRO
LOCAL           DL_1
                MOV     AX,FSNX[BX]
                MOV     AL,DS:[SI+1+LEV2]
                SHR     AL,1
                TEST    FG_LAPB[BX],M_OR_14
                JNZ     SHORT DL_1
                AND     AL,111B
DL_1:
                CMP     AL,BYTE PTR FSNX[BX]
                ENDM

@NR_VS          MACRO
LOCAL           DL_1
                MOVZX   AX,DS:[SI+1+LEV2+MODUL]
                SHR     AL,1
                TEST    FG_LAPB[BX],M_OR_14
                JNZ     SHORT DL_1
                SHR     AL,4
DL_1:
                CMP     AX,FSNT[BX]
                ENDM

@NR_VA          MACRO
LOCAL           DL_1
                MOVZX   AX,DS:[SI+1+LEV2+MODUL]
                SHR     AL,1
                TEST    FG_LAPB[BX],M_OR_14
                JNZ     SHORT DL_1
                SHR     AL,4
DL_1:
                CMP     AX,FSNF[BX]
                ENDM

@VA__NR         MACRO
LOCAL           DL_1
                MOVZX   AX,DS:[SI+1+LEV2+MODUL]
                SHR     AL,1
                TEST    FG_LAPB[BX],M_OR_14
                JNZ     SHORT DL_1
                SHR     AL,4
DL_1:
                MOV     FSNF[BX],AX
                ENDM

@SET_OWN        MACRO
                OR      FG_LAPB[BX],M_OR_2
                ENDM
@CLEAR_OWN      MACRO
                AND     FG_LAPB[BX],M_AND_2
                ENDM
@SET_PEER       MACRO
                OR      FG_LAPB[BX],M_OR_3
                ENDM
@CLEAR_PEER     MACRO
                AND     FG_LAPB[BX],M_AND_3
                ENDM
@PEER           MACRO
                TEST    FG_LAPB[BX],M_OR_3
                ENDM
@CLEAR_REJECT   MACRO
                AND     FG_LAPB[BX],M_AND_5
                ENDM
@SET_REJECT     MACRO
                OR      FG_LAPB[BX],M_OR_5
                ENDM
@REJECT_EXCEPTION       MACRO
                TEST    FG_LAPB[BX],M_OR_5
                        ENDM
@SET_LAYER_3    MACRO
                OR      FG_LAPB[BX],M_OR_0
                ENDM
@LAYER_3        MACRO
                TEST    FG_LAPB[BX],M_OR_0
                ENDM
@CLEAR_LAYER_3  MACRO
                AND     FG_LAPB[BX],M_AND_0
                ENDM

@ABLE_TO        MACRO
                TEST    FG_LAPB[BX],M_OR_1
                ENDM
@F_P            MACRO
                MOV     AL,DS:[SI+1+LEV2]
                AND     AL,M_U_F
                SHR     AL,4
                MOV     BYTE PTR F[BX],AL
                MOV     P[BX],2
                ENDM
@I_F_P          MACRO
LOCAL           DL_1,DL_2
                MOV     AL,DS:[SI+1+LEV2+MODUL]
                TEST    FG_LAPB[BX],M_OR_14
                JZ      SHORT DL_1
                AND     AL,M_S_F
                JMP     SHORT DL_2
DL_1:
                AND     AL,M_S_F8
                SHR     AL,4
DL_2:
                MOV     BYTE PTR F[BX],AL
                MOV     P[BX],2
                ENDM
@DM_F_1         MACRO
                TEST    BYTE PTR DS:[SI+1+LEV2],M_U_F
                ENDM
@RR_P_1         MACRO
LOCAL           DL_1,END_M
                TEST    FG_LAPB[BX],M_OR_14
                JZ      SHORT DL_1
                TEST    BYTE PTR DS:[SI+1+LEV2+MODUL],M_S_F8
                JMP     SHORT END_M
DL_1:
                TEST    BYTE PTR DS:[SI+1+LEV2],M_S_F8
END_M:
                ENDM
@I_P_1          MACRO
LOCAL           DL_1,END_M
                TEST    FG_LAPB[BX],M_OR_14
                JZ      SHORT DL_1
                TEST    BYTE PTR DS:[SI+2+LEV2],M_S_F
                JMP     SHORT END_M
DL_1:
                TEST    BYTE PTR DS:[SI+1+LEV2],M_S_F8
END_M:
                ENDM
@COMMAND        MACRO
LOCAL           DL_1,END_M
                PUSH    DX
                MOV     AL,DS:[SI]
                MOV     DL,BYTE PTR USER[BX]    ;M_OR_COM[BX]
                TEST    FG_LAPB[BX],M_OR_14
                JZ      DL_1
                AND     AL,M_C_R
                JMP     SHORT END_M
DL_1:
                AND     AL,M_C_RX25
                SHL     DL,1
END_M:
                XOR     AL,DL
                POP     DX
                ENDM

@RR_            MACRO
LOCAL           DL_SDL_13,DL_SDL__1,END,DL_SDL_14
                @CLEAR_PEER
                @RR_P_1
                JZ      DL_SDL__1
                @COMMAND
                JNZ     DL_SDL_13
                @ENQUIRY
DL_SDL__1:
                @VA_NR_VS
                JNA     END
                @NR_ERROR
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],4
                RET
DL_SDL_13:
                MOV     AH,1
                CALL    MDL_GO
                JMP     DL_SDL__1
END:
                ENDM
@I_COM          MACRO
LOCAL           DL_SDL_20,DL_SDL_23,DL_SDL_24_DL_SDL_25
LOCAL           DL_SDL_21,DL_SDL_27,DL_SDL_22,DL_SDL__3
                AND     FG_LAPB[BX],M_AND_6
                @OWN
                JNZ     SHORT DL_SDL_20
                @NS_VR
                JNE     DL_SDL_21
                INC     FSNX[BX]
                AND     FSNX[BX],MOD_FSN
                @CLEAR_REJECT
                @I_F_P
                MOV     AH,5
                CALL    DL_GO
                @ACKNOWLEDGE
                JNZ     DL_SDL__3
                @SET_ACKNOWLEDGE
                MOV     AH,22
 ;;;;               CALL    SDL_GO              ;èéëãÖ èêàçüíàü 3 ìêéÇçÖå -RR-
                JMP     DL_SDL__3
DL_SDL_20:
                @DISCARD_INFORMATION
                @I_P_1
                JZ      DL_SDL__3
                MOV     F[BX],1
                @COM_TX_O RNR
                @CLEAR_ACKNOWLEDGE
                JMP     DL_SDL__3
DL_SDL_21:
                @DISCARD_INFORMATION
                @REJECT_EXCEPTION
                @SET_REJECT
                @I_F_P
                @COM_TX_O REJ
                @CLEAR_ACKNOWLEDGE
                JMP     DL_SDL__3
DL_SDL_23:
                @NR_ERROR
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],4
                RET
DL_SDL__3:
                @VA_NR_VS
                JA      DL_SDL_23
                ENDM

@SAVE_POKET     MACRO
LOCAL           KONEC
                TEST    FG_LAPB[BX],M_OR_6
                JNZ      KONEC
                MOV     AX,SLED_POK[BX]
                MOV     DX,AX
                INC     AX
                AND     AX,K
                CMP     AX,POS_POK[BX]
                JE       KONEC
                MOV     SLED_POK[BX],AX
                PUSH    DS
                POP     ES
                MOV     DI,BX
                SHL     DI,SHL_K
                OR      DI,DX
                SHL     DI,SHL_BUF
                LEA     DI,BUF_LEV2[DI]
                MOV     AX,CX
                CLD
                STOSW
                PUSH    SI
                REP     MOVSB
                POP     SI
KONEC:
                AND     FG_LAPB[BX],M_AND_6
                ENDM




@DISCARD        MACRO
;                MOV     SOST_L3[BX],0
                @COM_L3
                MOV     AX,SLED_LSSU[BX]
                MOV     POS_LSSU[BX],AX
                ENDM
@DISCARD2       MACRO
                @DISCARD
                MOV     AX,FSNT[BX]
                MOV     FSNX[BX],AX
                ENDM


