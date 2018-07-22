@START_TL31     MACRO
                MOV     AX,TIME_2MC
                MOV     OLD_TIME_L3[BX],AX
                OR      FG_TL3[BX],M_OR_0
                ENDM

@STOP_TL31      MACRO
                AND     FG_TL3[BX],M_AND_0
                ENDM

@START_TL32     MACRO
                MOV     AX,TIME_2MC
                MOV     OLD_TIME_L3_2[BX],AX
                OR      FG_TL3[BX],M_OR_1
                ENDM

@STOP_TL32      MACRO
                AND     FG_TL3[BX],M_AND_1
                ENDM

@START_TL33     MACRO
                MOV     AX,TIME_2MC
                MOV     OLD_TIME_L3_3[BX],AX
                OR      FG_TL3[BX],M_OR_2
                ENDM

@STOP_TL33      MACRO
                AND     FG_TL3[BX],M_AND_2
                ENDM

TIME_L3:
                TEST    FG_TL3[BX],M_OR_0
                JZ      SHORT DL_TL3
                MOV     AX,TIME_2MC
                SUB     AX,OLD_TIME_L3[BX]
                CMP     AX,POROG_TL3
                JB      SHORT DL_TL3
                @STOP_TL31
                MOV     AH,4
                CALL    L3_GO
DL_TL3:
                TEST    FG_TL3[BX],M_OR_1
                JZ      SHORT DL_TL3_2
                MOV     AX,TIME_2MC
                SUB     AX,OLD_TIME_L3_2[BX]
                CMP     AX,POROG_TL3_2
                JB      SHORT DL_TL3_2
                @STOP_TL32
                MOV     AH,7
                CALL    L3_GO
DL_TL3_2:
                TEST    FG_TL3[BX],M_OR_2
                JZ      SHORT DL_TL3_3
                MOV     AX,TIME_2MC
                SUB     AX,OLD_TIME_L3_3[BX]
                CMP     AX,POROG_TL3_3
                JB      SHORT DL_TL3_3
                @STOP_TL33
                MOV     AH,11
                CALL    L3_GO
DL_TL3_3:
                RET

@OBNUL_L3       MACRO
                PUSH    AX
;                MOV     AX,FSNF_3[BX]
;                ADD     ADD_BUF_L3[BX],AX
;                AND     ADD_BUF_L3[BX],7
;                MOV     AX,FSNL_3[BX]
;                SUB     AX,FSNF_3[BX]
;                AND     AX,7
                MOV     FSNL_3[BX],0;AX
                MOV     FSNT_3[BX],0
                MOV     FSNF_3[BX],0
                MOV     P_R_LEV3[BX],0
                mov     OLD_P_R[bx], 0          ;;; Yu
                MOV     ADD_BUF_L3[BX],0
                POP     AX
                ENDM

@OBNUL_L3_0     MACRO
                @OBNUL_L3
;                PUSH    BX
;                XOR     BX,2
;                @OBNUL_L3
;                POP     BX
                ENDM





                ; 0  1       2    3    4        5       6           7
                ;CL/CH/IND_COM/FSNT/BSNR/OBSH_IND/KANAL_1/FSL_2.BSR_3/
ISDN_OUT:
                MOV     AH,1
                JMP     SHORT L3_GO

ISDN_IN:
                MOVZX   BP,DS:[SI+2]            ;P(S)+P(R)
                MOV     AH,8
                SHR     BP,1
                JNC     L3_GO
                MOV     AH,BUF_OKT3[BP]
                CMP     AH,0
                JNE     L3_GO
                AND     BP,01111B
                MOV     AH,BUF_OKT3[BP]
                CMP     AH,0
                JNE     L3_GO
                JMP     RET_L3

ADR_L3          DW      OFFSET RESTART
                DW      OFFSET OSH_POD_REST
                DW      OFFSET PODT_RESTARTA
                DW      OFFSET SBROS
                DW      OFFSET OSH_POD_SBROSA
                DW      OFFSET POD_SBROSA
                DW      OFFSET RABOTA

L3_GO:
                MOV     BP,SOST_L3[BX]
                SHL     BP,1
                JMP     ADR_L3[BP]
RET_L3:
                XOR     CX,CX
                RET

RESTART:                                ;0
;                CMP     BX,0
;                JNE     RET_L3
                CMP     AH,2
                JE      PRINYT_RESTART
;                CMP     AH,1
;                JNE     RET_L3
                MOV     AX,OBSH_IND
                STOSW
                MOV     AX,00FBH
                STOSW
                xor     al, al                  ;;; Yu
                stosb                           ;;; Yu
                @START_TL31
                MOV     SOST_L3[BX],1
                RET

PRINYT_RESTART:
                @OBNUL_L3_0
                @STOP_TL31
                MOV     SOST_L3[BX],2
                RET

OSH_POD_REST:                           ;1
;                CMP     BX,0
;                JNE     RET_L3
                CMP     AH,2
                JE      PRINYT_RESTART
                CMP     AH,3
                JE      PRINYT_PODTVER_REST
                CMP     AH,4
                JNE     RET_L3
                        ;ëêÄÅéíÄã íÄâåÖê
                MOV     SOST_L3[BX],0
                RET

PRINYT_PODTVER_REST:
                @STOP_TL31
                MOV     SOST_L3[BX],3
;                MOV     SOST_L3[0],3
;                MOV     SOST_L3[2],3
                @OBNUL_L3_0
                RET

PODT_RESTARTA:                                  ;2
                CMP     AH,1
                JNE     RET_L3
                        ;ÇõÑÄóÄ èéÑíÇÖêÜÑÖçàü êÖëíÄêíÄ
                @OBNUL_L3_0
                MOV     AX,OBSH_IND
                STOSW
                MOV     AX,00FFH
                STOSB                            ;<- {Paul.N - 17/10/17 - change} °Î´Æ STOSW - Ø‡Æ°´•¨† · X.25 ¢ ä‡†·≠Æ§†‡• ≠† ·‚†‡Æ¨ ¶•´•ß•!
                MOV     SOST_L3[BX],3
                RET
                      ; 0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5
TAB_SBROSA      DB      0,1,2,0,0,3,0,0,0,0,0,0,0,0,0,0
ADR_SBROSA      DW      OFFSET RET_L3
                DW      OFFSET PERED_SBROS
                DW      OFFSET REST_PRI_SBROSE
                DW      OFFSET PRINYT_SBROSA

SBROS:                                  ;3
                MOVZX   BP,AH
                MOVZX   BP,TAB_SBROSA[BP]
                SHL     BP,1
                JMP     ADR_SBROSA[BP]

PERED_SBROS:
                @OBNUL_L3
                MOV     AL,OBSH_IND
                MOV     AH,BYTE PTR KANAL_1
                STOSW
                MOV     AL,1BH
                STOSB
                MOV     AX,0
                STOSW
                @START_TL32
                MOV     SOST_L3[BX],4           ;éÜàÑÄçàÖ èéÑíÇÖêÜÑÖçàü ëÅêéëÄ
                RET
REST_PRI_SBROSE:
                MOV     SOST_L3[BX],2
;                MOV     SOST_L3[0],2
;                MOV     SOST_L3[2],2
                @STOP_TL32
                RET

PRINYT_SBROSA:
                ;;; --- Yu!!!
                mov     ax, ds:[si + 3]
                mov     X25_Reset[bx], ax
                ;;; --- Yu!!!
                @OBNUL_L3
                @STOP_TL32
                MOV     SOST_L3[BX],5
                RET

                      ; 0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5
TAB_OSH_SBROSA  DB      0,0,2,0,0,3,4,5,0,0,0,0,0,0,0,0
ADR_OSH_SBROSA  DW      OFFSET RET_L3
                DW      OFFSET PERED_SBROS
                DW      OFFSET REST_PRI_SBROSE
                DW      OFFSET PRINYT_SBROSA
                DW      OFFSET PRINYT_PODT_SBROSA
                DW      OFFSET TIMER_PO_SBROSU

OSH_POD_SBROSA:                         ;4
                MOVZX   BP,AH
                MOVZX   BP,TAB_OSH_SBROSA[BP]
                SHL     BP,1
                JMP     ADR_OSH_SBROSA[BP]

PRINYT_PODT_SBROSA:
                @OBNUL_L3
                @STOP_TL32
                MOV     SOST_L3[BX],6           ;êÄÅéíÄ

        ;;;;;20.11.02
                CMP     BX,0
                JNE     SHORT NE_PERV_AVA
                OR      FG_INIC,M_OR_4
NE_PERV_AVA:
        ;;;;;
                RET

TIMER_PO_SBROSU:
;;;;;                JMP     PERED_SBROS
                MOV     SOST_L3[BX],3
                RET

                      ; 0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5
TAB_POD_SBROSA  DB      0,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0
ADR_POD_SBROSA  DW      OFFSET RET_L3
                DW      OFFSET PERED_POD_SBROSA
                DW      OFFSET REST_PRI_SBROSE

POD_SBROSA:                             ;5
                MOVZX   BP,AH
                MOVZX   BP,TAB_POD_SBROSA[BP]
                SHL     BP,1
                JMP     ADR_POD_SBROSA[BP]

PERED_POD_SBROSA:
                @OBNUL_L3
                MOV     AL,OBSH_IND
                MOV     Ah,BYTE PTR KANAL_1
                STOSW
                MOV     AL,1FH
                STOSB
                MOV     SOST_L3[BX],6
        ;;;;;20.11.02
                CMP     BX,0
                JNE     SHORT NE_PERV_AVA2
                OR      FG_INIC,M_OR_4
NE_PERV_AVA2:
        ;;;;;
                RET

                      ; 0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5
TAB_RABOTA      DB      0,1,2,0,0,3,0,0,4,5,6,7,0,0,0,0
ADR_RABOTA      DW      OFFSET RET_L3           ; 0
                DW      OFFSET PERED_PAKET      ; 1
                DW      OFFSET REST_PRI_SBROSE  ; 2
                DW      OFFSET PRINYT_SBROSA    ; 3
                DW      OFFSET PRINYT_PAKET     ; 4
                DW      OFFSET PRINYT_RR        ; 5
                DW      OFFSET PRINYT_RNR       ; 6
                DW      OFFSET TIME_PAKETA      ; 7

RABOTA:                                         ;6
                AND     AH,0FH
                MOVZX   BP,AH
                MOVZX   BP,TAB_RABOTA[BP]
                SHL     BP,1
                JMP     ADR_RABOTA[BP]

PERED_PAKET:
                TEST    FG_L3[BX],M_OR_0
                JNZ     NET_PAKETA_L3
;JMP     NET_PAKETA_L3
                MOV     PEREM_DI,DI
                MOV     AL,BYTE PTR FSNT_3[BX]
                SUB     AL,BYTE PTR FSNF_3[BX]
                AND     AL,MOD_7
                CMP     AL,BYTE PTR OKNO_L2[BX]
                JNB     NET_INF_PAKETA
                XOR     AX,AX
                MOV     AL,BYTE PTR FSNL_3[BX]
                CMP     AL,BYTE PTR FSNT_3[BX]
                JE      NET_INF_PAKETA
                MOV     AL,OBSH_IND
                MOV     AH,BYTE PTR KANAL_1
                STOSW
                MOV     AL,BYTE PTR P_R_LEV3[BX]
                MOV     BYTE PTR OLD_P_R[BX],AL
                SHL     AL,4
                AND     FSNT_3[BX],07H
                OR      AL,BYTE PTR FSNT_3[BX]
                SHL     AL,1
                STOSB
                MOV     SI,BX
                SHL     SI,3
                MOV     AX,FSNT_3[BX]
                ADD     AX,ADD_BUF_L3[BX]
                AND     AX,7
                OR      SI,AX
                SHL     SI,7
                LEA     SI,BUF_L3_L2[SI]
                INC     FSNT_3[BX]
                AND     FSNT_3[BX],7
                MOV     CX,DS:[SI]
                ADD     SI,2
                OR      CX,CX
                JZ      NET_PAKETA_L31
                CMP     CX,100
                JA      NET_PAKETA_L31
                ADD     CX,2
                REP     MOVSB
                @START_TL33
                JMP     RET_L3

NET_PAKETA_L31:
NET_PAKETA_L3:
                MOV     DI,PEREM_DI
                JMP     RET_L3

NET_INF_PAKETA:                         ;
                MOV     CL,BYTE PTR P_R_LEV3[BX]
                CMP     CL,BYTE PTR OLD_P_R[BX]
                JE      RET_L3
                MOV     AL,OBSH_IND
                MOV     AH,BYTE PTR KANAL_1
                STOSW
                MOV     BYTE PTR OLD_P_R[BX],CL
                MOV     AL,BYTE PTR P_R_LEV3[BX]
                SHL     AL,5
                OR      AL,1                    ;---RR---
                STOSB
                JMP     RET_L3

@PODTVER_PAK    MACRO
LOCAL           DL_P_S_IN,DL_P_S_IN1
                MOV     DL,DS:[SI+2]            ;P(S)+P(R)
                MOV     DH,DL
                SHR     DH,5
                MOV     DL,DH
                CMP     DH,BYTE PTR FSNT_3[BX]
                JE      SHORT DL_P_S_IN1
                CMP     DH,BYTE PTR FSNF_3[BX]
                JE      SHORT DL_P_S_IN
                @START_TL33
                MOV     AL,BYTE PTR FSNT_3[BX]
                SUB     AL,BYTE PTR FSNF_3[BX]
                AND     AL,7
                SUB     DL,BYTE PTR FSNF_3[BX]
                AND     DL,7
                MOV     AH,DL
                CMP     DL,AL
                JA      SHORT DL_P_S_IN
DL_P_S_IN1:
                MOV     BYTE PTR FSNF_3[BX],DH
                CMP     DH,BYTE PTR FSNT_3[BX]
                JNE     SHORT DL_P_S_IN
                @STOP_TL33
DL_P_S_IN:
                ENDM

PRINYT_PAKET:
                MOV     DL,DS:[SI+2]            ;P(S)+P(R)
                SHR     DL,1
                AND     DL,7
                CMP     BYTE PTR P_R_LEV3[BX],DL        ;çÖ ëéÇèÄÑÄÖí çéåÖê
                JNE     SHORT NETOT_PAKET               ;éÜàÑÄÖåéÉé èÄäÖíÄ
                INC     DL
                AND     DL,7
                MOV     BYTE PTR P_R_LEV3[BX],DL
                @PODTVER_PAK
                ADD     SI,3
                PUSH    BX
                CALL    LEV_L4
                POP     BX
NETOT_PAKET:
                RET

PRINYT_RR:
                AND     FG_L3[BX],M_AND_0
                JMP     SHORT DL_RR_RNR_L3
PRINYT_RNR:
                OR      FG_L3[BX],M_OR_0
DL_RR_RNR_L3:
                @PODTVER_PAK
                RET

TIME_PAKETA:
                MOV     AX,FSNT_3[BX]
                MOV     AX,FSNF_3[BX]
                inc     X25_Err[bx];;;!!!
                MOV     SOST_L3[BX],3
                RET



L4_L3_GO        PROC
public L4_L3_GO
                CMP     SOST_L3[BX],6
                JNE     NET_PAKETA_L4
                MOV     SI,BX
                SHL     SI,3
                MOV     AX,FSNL_3[BX]
                MOV     DX,AX
                ADD     AX,ADD_BUF_L3[BX]
                AND     AX,7
                ADD     DX,1
                AND     DX,7
                CMP     DX,FSNF_3[BX]
                JE      NET_PAKETA_L4
                OR      SI,AX
                SHL     SI,7
                LEA     SI,BUF_L3_L2[SI]
                MOV     WORD PTR DS:[SI],0
                PUSH    BX
                MOV     DI,BX
                MOV     BX,SI
                PUSH    SI
                ADD     BX,2
                CALL    L4_LEV4
                POP     SI
                POP     BX
                OR      CX,CX
                JZ      NET_PAKETA_L4
                ; ! ß§•·Ï  BX - ≠Æ¨•‡ ™†≠†´†, DS:[SI] - „™†ß†‚•´Ï ≠† Ø†™•‚
                ; (Ø•‡¢Î• §¢† °†©‚† - §´®≠≠† Ø†™•‚†)

                INC     FSNL_3[BX]
                AND     FSNL_3[BX],7
                MOV     DS:[SI],CX
NET_PAKETA_L4:
                RET
                ENDP

