

SDL_GO:
                MOV     DI,SOST_SDL[BX]
                SHL     DI,1
                JMP     ADR_SDL[DI]
RET_SDL:
                RET
;
TEI_UNASSIGNED:                                 ;0
                MOVZX   DI,AH
                MOVZX   DI,TAB_TEI[DI]
                JMP     ADR_TEI[DI]
SDL_0_GO:                       ;DL-ESTABLISH request
                MOV     SOST_SDL[BX],2
                MOV     AH,0
                CALL    MDL_GO
                RET
SDL_1_GO:                       ;DL-UNIT DATA request
                MOV     SOST_SDL[BX],1
                MOV     AH,0
                CALL    MDL_GO
                @UNIT_DATA
                @UI_FRAME
                RET
SDL_2_GO:                       ;MDL-ASSIGN request
                MOV     SOST_SDL[BX],3
                @SAVE_TEI
                MOV     AH,1
                JMP     UI_GO
;
ASSIGN_AWAITING:                                        ;1-
                MOVZX   DI,AH
                MOVZX   DI,TAB_ASS[DI]
                JMP     ADR_ASS[DI]
SDL_0_ASS:                       ;DL-ESTABLISH request
                MOV     SOST_SDL[BX],2
                RET
SDL_1_ASS:                       ;DL-UNIT DATA request
                MOV     SOST_SDL[BX],1
                @UNIT_DATA
                @UI_FRAME
                RET
SDL_3_GO:                       ;MDL-ERROR response
SDL_4_GO:                       ;PERSISTENT DEACTIVATION
                MOV     SOST_SDL[BX],0
                @DISCARD
                RET
;
ESTABLISH_AWAITING:                                      ;2-
                MOVZX   DI,AH
                MOVZX   DI,TAB_SDL_EST[DI]
                JMP     ADR_SDL_EST[DI]
SDL_2_EST:                      ;MDL-ASSIGN request
                MOV     SOST_SDL[BX],4
                @SAVE_TEI
                @ESTABLISH
                @SET_LAYER_3
                RET
SDL_3_EST:                      ;MDL-ERROR response
SDL_4_EST:                      ;PERSISTENT DEACTIVATION
                MOV     SOST_SDL[BX],0
                @DISCARD
                MOV     AH,0
                CALL    DL_GO
                RET
;
TEI_ASSIGNED:                                   ;3-
                MOVZX   DI,AH
                MOVZX   DI,TAB_SDL_TEI[DI]
                JMP     ADR_SDL_TEI[DI]
SDL_0_TEI2:                     ;DL-ESTABLISH request
                MOV     SOST_SDL[BX],4
                @ESTABLISH
                @SET_LAYER_3
                RET
SDL_5_GO:                       ;DL-RELEASE request
                MOV     AH,1
                CALL    DL_GO
                RET
SDL_6_GO:                       ;MDL-REMOVE request
SDL_4_TEI2:                     ;PERSISTENT DEACTIVATION
                MOV     SOST_SDL[BX],0
                @DISCARD
                RET
SDL_7_GO:                       ;SABME
                @F_P
                @ABLE_TO
                JZ      SHORT   DL_SDL_1
                MOV     SOST_SDL[BX],6
                @LSSU_TX_O UA
                @CLEAR_EXCEPTION
                MOV     FSNT[BX],0
                MOV     BSNT[BX],0
                MOV     FSNF[BX],0
                MOV     FSNX[BX],0
                MOV     FSNL[BX],0
                MOV     AH,2
                CALL    DL_GO
                @START_T203
                RET
DL_SDL_1:
                @LSSU_TX_O DM
                RET
SDL_8_GO:                       ;DISC
                @F_P
                @LSSU_TX_O DM
                RET
SDL_9_GO:               ;UA
                MOV     AH,1
                CALL    MDL_GO
                RET
SDL_10_GO:                      ;DM
                @DM_F_1
                JNZ     RET_SDL
                @ABLE_TO
                JZ      RET_SDL
                MOV     SOST_SDL[BX],4
                @ESTABLISH
                @SET_LAYER_3
                RET
;
AWAITING_ASTABLISHMENT:                                          ;4-
                MOVZX   DI,AH
                MOVZX   DI,TAB_SDL_AWA[DI]
                JMP     ADR_SDL_AWA[DI]
SDL_0_AWA:                      ;DL-ESTABLISH request
                @DISCARD
                @SET_LAYER_3
                RET
SDL_6_AWA:                      ;MDL-REMOVE request
                MOV     SOST_SDL[BX],0
                @DISCARD2
                MOV     AH,3
                CALL    DL_GO
                @STOP_T200
                RET
SDL_7_AWA:                      ;SABME
                @F_P
                @LSSU_TX_O UA
                MOV     SOST_SDL[BX],6
                @STOP_T200
                @START_T203
                @COM_L3
                MOV     FSNT[BX],0
                MOV     BSNT[BX],0
                MOV     FSNF[BX],0
                MOV     FSNX[BX],0
                MOV     FSNL[BX],0
;                MOV     FSNT[BX],0
;                MOV     FSNF[BX],0
                RET

SDL_8_AWA:                      ;DISK

                @F_P
                @LSSU_TX_O DM
                RET
SDL_9_AWA:                      ;UA
                @DM_F_1
                JZ      SHORT DL_SDL_2
                @LAYER_3
                JZ      SHORT DL_SDL_3
                @CLEAR_LAYER_3
                MOV     AH,4
                CALL    DL_GO
CN_SDL_4:
                MOV     SOST_SDL[BX],6
                @STOP_T200
                @START_T203
;                MOV     FSNT[BX],0
;                MOV     FSNF[BX],0
                MOV     FSNT[BX],0
                MOV     BSNT[BX],0
                MOV     FSNF[BX],0
                MOV     FSNX[BX],0
                MOV     FSNL[BX],0
                RET
DL_SDL_2:
                MOV     AH,1
                JMP     MDL_GO
DL_SDL_3:
                @VS_VA
                JE      SHORT CN_SDL_4
                @DISCARD
                MOV     AH,2
                CALL    DL_GO
                JMP     SHORT CN_SDL_4
SDL_10_AWA:                     ;DM
                @DM_F_1
                JZ      RET_SDL
                MOV     SOST_SDL[BX],3
                @DISCARD
                MOV     AH,3
                CALL    DL_GO
                @STOP_T200
                RET
SDL_15_AWA:                     ;TIMER T200 EXPIRY
                @RC_N200
                JE      DL_SDL_5
                INC     RC[BX]
                MOV     P[BX],1
                @LSSU_TX_C SABME
                @COM_L3
                @START_T200
                RET
DL_SDL_5:
                MOV     SOST_SDL[BX],3
                @DISCARD
                MOV     AH,1
                CALL    MDL_GO
                MOV     AH,3
                JMP     DL_GO
SDL_11_AWA:                     ;I FRAME QUEUED UP
                @LAYER_3
                JNZ     RET_SDL
                @I_FRAME
                RET
SDL_12_AWA:                     ;DL-DATA request
                @LAYER_3
                JNZ     RET_SDL
                @PUT_IN
                @I_FRAME
                RET
;
AWATING_RELEASE:                                        ;5-
                MOVZX   DI,AH
                MOVZX   DI,TAB_SDL_AW2[DI]
                JMP     ADR_SDL_AW2[DI]
SDL_6_AW2:                      ;MDL-REMOVE request,PER...
                MOV     SOST_SDL[BX],0
                @DISCARD
                MOV     AH,1
                CALL    DL_GO
                @STOP_T200
                RET
SDL_9_AW2:                                      ;UA
                @DM_F_1
                JZ      SHORT DL_SDL_7
                MOV     SOST_SDL[BX],3
                MOV     AH,1
                CALL    DL_GO
                @STOP_T200
                RET
DL_SDL_7:
                MOV     AH,1
                JMP     MDL_GO
SDL_10_AW2:                                     ;DM
                @DM_F_1
                JZ      RET_SDL
                MOV     SOST_SDL[BX],3
                MOV     AH,1
                CALL    DL_GO
                @STOP_T200
                RET
SDL_15_AW2:                                     ;TIMER T200 EXPIRY
                @RC_N200
                JNZ     SHORT DL_SDL_8
                INC     RC[BX]
                MOV     P[BX],1
                @LSSU_TX_C DISC
                @START_T200
                RET
DL_SDL_8:
                MOV     SOST_SDL[BX],3
                MOV     AH,1
                CALL    MDL_GO
                MOV     AH,1
                CALL    DL_GO
                RET
;
MULTIPLE_FRAME:                                                 ;6-
                MOVZX   DI,AH
                MOVZX   DI,TAB_SDL_MUL[DI]
                JMP     ADR_SDL_MUL[DI]
SDL_0_MUL:                      ;DL-ESTABLISH
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],4
                @DISCARD
                @ESTABLISH
                @SET_LAYER_3
                RET
SDL_5_MUL:                      ;DL-RELEASE request
                MOV     SOST_SDL[BX],5
                @DISCARD
                MOV     RC[BX],0
                MOV     P[BX],1
                @LSSU_TX_C DISC
                @STOP_T203
                @START_T200
                RET
SDL_12_MUL:                     ;DL-DATA request
                @PUT_IN
                @I_FRAME
                RET
SDL_11_MUL:                     ;I FRAME QUEUED UP
                @PEER
                JNZ     SHORT DL_SDL_9
;                @VS_VA_K
;                JNZ     SHORT DL_SDL_9
                @GET_NEXT
                MOV     P[BX],0
                MOV     AH,0
;                CALL    TXI_GO
                INC     FSNT[BX]
                @CLEAR_ACKNOWLEDGE
                @T200_RUNNING
                JNZ     RET_SDL
                @STOP_T203
                @START_T200
                RET
DL_SDL_9:
                @I_FRAME
                RET
SDL_15_MUL:                     ;TIMER_T200
                ;MOV     AX,FSNF[BX]
                ;MOV     FSNT[BX],AX
                MOV     SOST_SDL[BX],7
                MOV     RC[BX],0
                @TRANSMIT
                INC     RC[BX]
                RET
SDL_16_MUL:                     ;TIMER T203
                MOV     SOST_SDL[BX],7
                @TRANSMIT
                MOV     RC[BX],0
                RET
SDL_6_MUL:                      ;MDL-REMOVE request
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],0
                @DISCARD2
                MOV     AH,3
                CALL    DL_GO
                @STOP_T200
                @STOP_T203
                RET
SDL_4_MUL:                      ;PERSISTENT DEACTIVATION
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],3
                @DISCARD2
                MOV     AH,3
                CALL    DL_GO
                @STOP_T200
                @STOP_T203
                RET

SDL_7_MUL:                      ;SABME
                @F_P                            ;
                @LSSU_TX_O UA                   ;
                @STOP_T200                      ;
                @START_T203                     ;
                MOV     FSNL[BX],0              ;
                MOV     BSNT[BX],0              ;
                MOV     FSNT[BX],0              ;
                MOV     FSNF[BX],0              ;
                MOV     FSNX[BX],0              ;
                RET

SDL_8_MUL:                      ;DISC
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],3
                @DISCARD
                @F_P
                @LSSU_TX_O UA
                MOV     AH,3
                CALL    DL_GO
                @STOP_T200
                @STOP_T203
                RET
SDL_9_MUL:                      ;UA
                MOV     AH,1
                JMP     MDL_GO
SDL_10_MUL:                     ;DM
                @DM_F_1
                JNZ     DL_SDL_12
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],4
                MOV     AH,1
                CALL    MDL_GO
                @ESTABLISH
                @CLEAR_LAYER_3
                RET
DL_SDL_12:
                MOV     AH,1
                JMP     MDL_GO
SDL_13_MUL:                     ;SET OWN RECEIVER BUSY
                @OWN
                JNZ     RET_SDL
                @SET_OWN
                MOV     F[BX],0
                @COM_TX_O RNR
                @CLEAR_ACKNOWLEDGE
                RET
SDL_14_MUL:                     ;CLEAR_OWN_RECEIVER
                @OWN
                JZ      RET_SDL
                @CLEAR_OWN
                MOV     F[BX],0
                @COM_TX_O RR
                @CLEAR_ACKNOWLEDGE
                RET

SDL_17_MUL:                     ;RR
                @RR_
                @NR_VS
                JNE    SHORT DL_SDL_15
                @VA__NR
                @STOP_T200
                @START_T203
                MOV     SOST_SDL[BX],6
                RET
DL_SDL_15:
                @NR_VA
                JE      RET_SDL
                @VA__NR
                @START_T200
                RET
SDL_18_MUL:                     ;REJ
                @RR_
                @VA__NR
                @STOP_T200
                @START_T203
                @INVOKE
                RET
SDL_19_MUL:                     ;RNR
                @SET_PEER
                @RR_
                @VA__NR
                @STOP_T203
                @START_T200
                RET
SDL_20_MUL:                     ;FRMR
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],4
                MOV     AH,1
                CALL    MDL_GO
                @ESTABLISH
                @CLEAR_LAYER_3
                RET
SDL_21_MUL:                     ;I COMMAND
;!!!!!!
                @I_COM
                @SAVE_POKET
                @PEER
                JMP     SHORT DL_SDL_24
                @VA__NR
                RET
DL_SDL_24:
                @NR_VS
                JNE     SHORT DL_SDL_25
                @VA__NR
                @STOP_T200
                @START_T203
                RET
DL_SDL_25:
                @NR_VA
                JNZ     RET_SDL
                @VA__NR
                @START_T200
                RET

SDL_22_MUL:                             ;ACKNOWLEDGE PENDING
                @ACKNOWLEDGE
                JZ      RET_SDL
                @CLEAR_ACKNOWLEDGE
                MOV     F[BX],0
                @COM_TX_C RR
                RET

;
TIMER_RECOVERY:                                         ;7-
                MOVZX   DI,AH
                MOVZX   DI,TAB_SDL_TIM[DI]
                JMP     ADR_SDL_TIM[DI]
SDL_0_TIM:                      ;DL-ESTABLISH
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],4
                @DISCARD
                @ESTABLISH
                @SET_LAYER_3
                RET
SDL_5_TIM:                      ;DL-RELEASE request
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],5
                @DISCARD
                MOV     RC[BX],0
                MOV     P[BX],1
                @LSSU_TX_C DISC
                @START_T200
                RET
SDL_12_TIM:                     ;DL-DATA request
                @PUT_IN
SDL_11_TIM:                     ;I FRAME QUEUED UP
                @I_FRAME
                RET
SDL_15_TIM:                     ;TIMER T200
                @RC_N200
                JE      DL_SDL_30
                MOV     AX,FSNF[BX]
                MOV     FSNT[BX],AX
                @VS_VA
                JMP     SHORT DL_SDL_31
                ;;;;;
                ;!!!;
                ;;;;;
                @PEER
                JNZ     SHORT DL_SDL_31
                @GET_LAST
                DEC     FSNT[BX]
                AND     FSNT[BX],MOD_FSN
                MOV     P[BX],1
                MOV     AH,0
;                CALL    TXI_GO
                INC     FSNT[BX]
                AND     FSNT[BX],MOD_FSN
                @CLEAR_ACKNOWLEDGE
                @START_T200
CN_SDL_32:
                INC     RC[BX]
                RET
DL_SDL_31:
                @TRANSMIT
                JMP     CN_SDL_32
DL_SDL_30:
                MOV     RC[BX],0
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],4
                MOV     AH,1
                CALL    MDL_GO
                @ESTABLISH
                @CLEAR_LAYER_3
                RET
SDL_6_TIM:                      ;MDL-REMOVE request
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],0
                @DISCARD2
                MOV     AH,3
                CALL    DL_GO
                @STOP_T200
                RET
SDL_4_TIM:                      ;PERSISTENT DEACIVATION
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],3
                @DISCARD2
                MOV     AH,3
                CALL    DL_GO
                @STOP_T200
                RET
SDL_7_TIM:                      ;SABME
                @F_P
                @LSSU_TX_O UA
                @CLEAR_EXCEPTION
                MOV     AH,1
                CALL    MDL_GO
                @VS_VA
                JNZ     SHORT DL_SDL_40
                @DISCARD
                MOV     AH,2
                CALL    DL_GO
DL_SDL_40:
                MOV     SOST_SDL[BX],6
                @STOP_T200
                @START_T203
                MOV     FSNL[BX],0
                MOV     BSNT[BX],0
                MOV     FSNT[BX],0
                MOV     FSNF[BX],0
                MOV     FSNX[BX],0
                RET
SDL_8_TIM:                      ;DISC
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],3
                @DISCARD
                @F_P
                @LSSU_TX_O UA
                MOV     AH,3
                CALL    DL_GO
                @STOP_T200
                RET
SDL_9_TIM:                      ;UA
                MOV     AH,1
                CALL    MDL_GO
                RET
SDL_10_TIM:                     ;DM
                @DM_F_1
                JNZ     DL_SDL_41
                MOV     AH,1
                CALL    MDL_GO
CN_SDL_42:
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],4
                @ESTABLISH
                @CLEAR_LAYER_3
                RET
DL_SDL_41:
                MOV     AH,1
                CALL    MDL_GO
                JMP     CN_SDL_42

SDL_13_TIM:                     ;SET OWN RECEIVER BUSY
                @OWN
                JNZ     RET_SDL
                @SET_OWN
                MOV     F[BX],0
                @COM_TX_O RNR
                @CLEAR_ACKNOWLEDGE
                RET

@RR_2           MACRO
LOCAL           DL_SDL_50,DL_SDL_51,DL_SDL_52
                @COMMAND
                JZ      DL_SDL_50
                @RR_P_1
                JZ      DL_SDL_51
                @ENQUIRY
DL_SDL_51:
                @VA_NR_VS
                JA      SHORT DL_SDL_52
                @VA__NR
                RET
DL_SDL_52:
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],4
                @NR_ERROR
                RET
DL_SDL_50:
                @RR_P_1
                JZ      DL_SDL_51
                @VA_NR_VS
                JA      DL_SDL_52
                @VA__NR
                ENDM

SDL_17_TIM:                     ;RR
SDl_18_TIM:                     ;REJ
                MOV     SOST_SDL[BX],6
                @CLEAR_PEER
                @RR_2
                @STOP_T200
                @START_T203
                @INVOKE
                RET
SDL_19_TIM:                     ;RNR
                MOV     SOST_SDL[BX],6
                @SET_PEER
                @RR_2
                @START_T200
                @INVOKE
                RET
SDL_20_TIM:                     ;FRMR
                inc     LAPB_Err[bx];;;!!!
                MOV     SOST_SDL[BX],4
                MOV     AH,1
                CALL    MDL_GO
                @ESTABLISH
                @CLEAR_LAYER_3
                RET
SDL_21_TIM:                     ;I COMMAND
                @I_COM
                @SAVE_POKET
                @VA__NR
                RET
SDL_22_TIM:                     ;ACKNOWLEDGE PENDING
                @ACKNOWLEDGE
                JZ      RET_SDL
                @CLEAR_ACKNOWLEDGE
                MOV     F[BX],0
                @COM_TX_C RR
                RET

