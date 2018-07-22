@INT_5          MACRO
LOCAL           DAL_KLV,END_M,ALT_ON,ALT_OFF,X_ON,Y_ON
                XOR     BX,BX
                MOV     BL,AL
;                CMP     AL,45H
;                JE      EXIT4
;                MOV     BH,RUS_LAT
                MOV     AL,TABL_KLA[BX]
                MOV     N_KLAV,AL
                MOV     FLG_KLA,1
                CMP     AL,23H
                JNE     SHORT DAL_KLV
                XOR     RUS_LAT,1
DAL_KLV:
                CMP     AL,254          ;ALT
                JE      ALT_ON
                CMP     AL,253
                JE      ALT_OFF
                CMP     AL,5AH
                JE      X_ON
                CMP     AL,59H
                JE      Y_ON
		JMP	END_M
ALT_ON:
                OR      FG_KLA,M_OR_0
		JMP	END_M
ALT_OFF:
                AND     FG_KLA,M_AND_0
                AND     FG_KLA,M_AND_1
		JMP	END_M
X_ON:
                TEST    FG_KLA,M_OR_0
		JZ 	END_M
                OR      FG_KLA,M_OR_1
		JMP	END_M
Y_ON:
                TEST    FG_KLA,M_OR_0
		JZ 	END_M
                TEST    FG_KLA,M_OR_1
		JZ 	END_M
                MOV     AL,0FH
                OUT     70H,AL
                MOV     AL,0
                OUT     71H,AL
                MOV     AL,0FEH
                OUT     64H,AL
                HLT
;                JMP     EXIT3
END_M:
                ENDM

CALL_INT5       PROC
                @INT_5
                RET
                ENDP

KLAVII:
                PUSH  AX
                PUSH  BX
                PUSH    DS
                MOV     DS,SS:SEG_AX
                IN    AL,60H
                PUSH  AX
                IN    AL,61H
                OR    AL,80h
                OUT   61H,AL
                AND   AL,7fh
                OUT   61H,AL
                mov    al,20h
                OUT   20H,AL
                POP   AX
                @INT_5
                POP     DS
                POP     BX
                POP     AX
                IRETD


@KLAIN2         MACRO
LOCAL           END_KLV
                MOV     AH,0FFH
                CMP     FLG_KLA,0
                JE      END_KLV
                MOV     AH,N_KLAV
END_KLV:
                MOV     FLG_KLA,0
                ENDM
