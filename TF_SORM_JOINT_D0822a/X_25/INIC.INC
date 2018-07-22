
INIC_LEV2:
                MOV     ADR_SDL[00],OFFSET TEI_UNASSIGNED
                MOV     ADR_SDL[02],OFFSET ASSIGN_AWAITING       ;çÄáçÄóàíú éÜàÑÄçàÖ TEI
                MOV     ADR_SDL[04],OFFSET ESTABLISH_AWAITING    ;éÜàÑÄçàÖ TEI
                MOV     ADR_SDL[06],OFFSET TEI_ASSIGNED          ;TEI çÄáçÄóÖçé
                MOV     ADR_SDL[08],OFFSET AWAITING_ASTABLISHMENT
                MOV     ADR_SDL[10],OFFSET AWATING_RELEASE
                MOV     ADR_SDL[12],OFFSET MULTIPLE_FRAME
                MOV     ADR_SDL[14],OFFSET TIMER_RECOVERY




                MOV     ADR_LSSU[00],OFFSET COMANDA
                MOV     ADR_LSSU[02],OFFSET OTVET

     MOV  ADR_DL[00],OFFSET RET_DL
     MOV  ADR_DL[02],OFFSET RELEASE2
     MOV  ADR_DL[04],OFFSET ESTABL
     MOV  ADR_DL[06],OFFSET RELEASE2
     MOV  ADR_DL[08],OFFSET ESTABL
     MOV  ADR_DL[10],OFFSET RET_DL

                MOV     CX,KL
                XOR     BX,BX
CN_INI_ISDN:
                AND     FG_LAPB[BX],M_AND_14
                MOV     AL,MODUL
                OR      AL,0
                JZ      SHORT DL_INIC_MOD
                OR      FG_LAPB[BX],M_OR_14
DL_INIC_MOD:
                MOV     AL,BYTE PTR USER[BX]
;                SHL     AL,1
                MOV     BYTE PTR M_OR_COM[BX],AL
;;;;;
                MOV     SOST_SDL[BX],0
                MOV     SOST_L3[BX],0
;;;;;
                MOV     AH,2
                CALL    SDL_GO
                MOV     AH,0
                CALL    SDL_GO
                XOR     AL,AL
                CMP     AL,LEV2
        ;LAPB
                OR      FG_LAPB[BX],M_OR_15
                ADD     BX,2
                LOOP    CN_INI_ISDN



                RET

