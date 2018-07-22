IF _MSP EQ TRUE
_test_SORM_wait equ     FALSE;TRUE

@ZADERSHKA      MACRO
LOCAL           CN_
                PUSH    CX
                PUSH    ES
                MOV     ES,SS:SEG_ST
                XOR     CX,CX
CN_:
                MOV     AL,ES:[00]
                LOOP    CN_
                POP     ES
                POP     CX
                ENDM
ENDIF
; ИНИЦИАЛИЗАЦИЯ АППАРАТУРЫ

; ----=> Fapch_Init <=-----------------------
; Created by Smaller on 08/29/05 9:33
; Инициализирует Фапч
Fapch_Init      proc
        public  Fapch_Init
IF _MSP EQ TRUE
        push    fs es ds
        push    eax

        mov     es, ss:seg_gs
        mov     ds, ss:seg_ax
        mov     fs, ss:seg_st

        mov     eax, _ET_DGP                    ; возьмем эталон

        mov     DGP, eax
        neg     eax
        mov     MING, eax                       ; Нижний допуск грубой подстр-и

        mov     ax, 00011000b                   ; выдача ФАПЧ из модема
        mov     MO_RM, ax
        mov     fs:[3424h], al                  ;

        mov     RR_PHA, 1
        mov     al, RR_PHA
        mov     es:[ADR_RR_PHA], al             ; Запись регистра режима ФАПЧ
        mov     ax, VCAPI
        mov     VCAP, ax
        mov     es:[ADR_RD0_PHA], al            ;\ Запись кода ЦАП
        mov     es:[ADR_RD1_PHA], ah            ;/

        pop     eax
        pop     ds es fs
ELSE
        MOV     ES,SS:SEG_PLL_DATA              ;
        MOV     ES:_LENGTH_SOURCES_CHAIN,0      ;Последний источник ФАПЧ: 0*2
        MOV     ES:_PRIORITY_SOURCE[0],4        ;ИСточник ФАПЧ - Модем
        MOV     ES:_PRIORITY_SOURCE[1],0        ;номер - 0
        MOV     ES:_MHZ_MBIT,0                  ;
        MOV     ES:_CURRENT_SOURCE,0            ;Номер текущего источника ФАПЧ
        MOV     ES:_DOUBLING,1                  ;
        MOV     ES:_OLDPATF_FLAG,1              ;Старый генератор
        CALL_PLL _INITIAL_PATF
ENDIF
        ret
Fapch_Init      endp

Init_Hard       PROC
        ; --- остановка коммутатора
        mov     es,SS:SEG_ST
        mov     BYTE PTR es:[COM_MODE], 0       ; остановка коммутатора

        ; --- Инициализация ФАПЧ
        call    Fapch_Init
IF _MSP EQ TRUE
        ; --- сброс ОЗУ коммутации
        MOV     ES,SS:SEG_ST
        mov     di, COM_1               ; 1 ОЗУ
        mov     cx, 128
COM_1_Loop1:
        mov     al, 0c0h
        stosb
        xor     al, al
        stosb
        stosb
        stosb
        loop    COM_1_Loop1

        mov     di, 3800h              ; дополнительное ОЗУ
        xor     ax, ax
EX_COM_Loop:
        stosb
        mov     al, ah
        stosb
        xor     al, al
        inc     ah
        jns     EX_COM_Loop

ELSE
                ; --- сброс ОЗУ коммутации
                MOV     ES,SS:SEG_ST
                mov     ax, 8000h
                mov     cx, 2048
                mov     di, COM_1               ; коммутатор
                rep     stosw
ENDIF

        ; --- Инициализация модема
        MOV     ES,SS:SEG_ST
        mov     BYTE PTR ES:[MO_MODE], 0; сброс регистра режима модема
        MOV     ES,SS:SEG_GS
        mov     AL, ES:[MO_IN]          ; сбросим IRQ10

IF _MSP EQ TRUE
        CMP     FG_PROG, 0              ;Если новое железо,
        JnE     C101_INI                ;переходим на C101_INI

        ; --- Инициализация ИКМ v1.0
        MOV     ES,SS:SEG_GS
        mov     al, PCM_CTRL
        ; 5300h - 53FFh - ОЗУ принятых служебный данных
        mov     BYTE PTR es:[5300h], al         ;
        mov     BYTE PTR es:[5340h], 0F0h       ; погасить светодиоды
;        mov     BYTE PTR es:[5380h], 0ffh       ; погасить светодиоды
        mov     BYTE PTR es:[53c0h], 0FFh       ; общий сброс
        xor     dx, dx                  ;\
@@D:                                    ;| задержка
        MOV     AL,ES:[5380H]           ;|
        jnz     @@D                     ;/

        MOV     BYTE PTR ES:[5340H],1
        mov     BYTE PTR es:[53c0h], 0  ; общий запуск
        mov     bx, AGFA
        mov     si, OFFSET IKM_DISPL
        mov     BYTE PTR es:[bx][1], 2  ; запись MODE
        lodsw
        mov     dx, ax
        and     ax, 7
        shr     dx, 3
        mov     es:[bx + 6], al         ; запись XC0
        mov     es:[bx + 7], dl         ; запись XC1
        lodsw
        mov     dx, ax
        and     ax, 7
        or      ax, 030h
        shr     dx, 3
        mov     es:[bx + 8], al         ; запись RC0
        mov     es:[bx + 9], dl         ; запись RC1
        mov     BYTE PTR es:[bx], 010h  ; "1" в CCR.CLR
        mov     BYTE PTR es:[bx], 0     ; Очистка FEC и CVC
        MOV     BYTE PTR ES:[BX+4],0DFH
        MOV     BYTE PTR ES:[BX+5],4

        sub     bx, 020h
        mov     BYTE PTR es:[bx], 18h  ; сброс IPAT
        xor     dx, dx                 ;\
@@D2:   dec     dx                     ;| задержка
        jnz     @@D2                   ;/
        mov     BYTE PTR es:[bx],0     ; запуск IPAT (8 - внутр. петля)
ENDIF
C101_INI:
                CALL    INIC_HARD_C101
IF _MSP EQ TRUE
                JMP     DALEE
ELSE
                MOV     ES,SS:SEG_ST
                mov     BYTE PTR es:[COM_MODE], 070h    ; запуск коммутатора
                ret
ENDIF


INIC_HARD_C101  PROC
;инициализация встроеных с101
INI_HSCX_GLB:
IF _MSP EQ TRUE
        mov     ES:[04410H],BYTE PTR 33H ;C101 ;управляющий регистр синхронных СОМ портов
        MOV     ES:[05040H],BYTE PTR 0
        MOV     ES:[05050H],BYTE PTR 010H
        MOV     ES:[05040H],BYTE PTR 1
        MOV     ES:[05050H],BYTE PTR 030H
        MOV     ES:[05040H],BYTE PTR 2
        MOV     ES:[05050H],BYTE PTR 050H
        MOV     ES:[05040H],BYTE PTR 3
        MOV     ES:[05050H],BYTE PTR 070H

        MOV     ES:[05052H],BYTE PTR 0F0H
        MOV     ES:[0505CH],BYTE PTR 0

        mov     es:[4408h],byte PTR 00000000B
        mov     es:[4406h],byte PTR 00000000B
        mov     es:[4407h],byte PTR 00000000B
        mov     es:[59FCh],byte PTR 1   ;Включить бит работы

if _SORM_NO_MODEM ;eq TRUE
    if _SORM_MONO ;eq TRUE
        ; дли моноблока
        push    es
        mov     es, ss:seg_gs
        ; назначим ТС
        mov     byte ptr es:[5040h], 0
        mov     byte ptr es:[5050h], 30 + _nom_ikm

        mov     byte ptr es:[5040h], 1
        mov     byte ptr es:[5050h], 31 + _nom_ikm

        ; запустим контроллеры
        mov     byte ptr es:[5051h] , 011b
        pop     es
    else
        ; инициализируем HDLC
        ; для старого железа
        push    es
        mov     es, ss:seg_gs
        ; назначим ТС
        mov     byte ptr es:[5050h], 30 + _nom_ikm
        mov     byte ptr es:[5051h], 31 + _nom_ikm

        ; запустим контроллеры
        mov     byte ptr es:[5027h], 33h;0FFh
        pop     es
    endif
endif



MOV     AX,136;144
MOV     SEG_C101[0],AX
MOV     AX,144;136
MOV     SEG_C101[2],AX

                MOV     CX,KL_C101
                XOR     BX,BX
SLED_C101:
                PUSH    CX
                MOV     ES,SEG_C101[BX]
                MOV     DI,SM_1D00
                MOV     ES:[DI],BYTE PTR 0
                MOV     DI,SM_1E00
                MOV     ES:[DI],BYTE PTR 0
                @ZADERSHKA
                MOV     ES:[DI],BYTE PTR 0FFH
                MOV     CX,1000
CN_PO_INI_C101:
                CALL    END_INI_HARD
                LOOP    CN_PO_INI_C101
                MOV     DI,SM_1F00
                MOV     CX,DL_INI_C101[BX]
                MOV     SI,CX
                OR      BX,BX
                JZ      SHORT DL_CN_PO_INI_C101
                ADD     SI,SM_INI_C101[BX-2]
DL_CN_PO_INI_C101:
                MOV     SM_INI_C101[BX],SI
                SUB     SI,CX
                SHL     SI,1
                LEA     SI,DATA_INI_C101[SI]
CN_PO_INI_C102:
                LODSB
                MOVZX   BP,AL
                LODSB
                MOV     ES:[BP+DI],AL
                LOOP    CN_PO_INI_C102

                AND     FG_C101[BX],M_AND_0

                ADD     BX,2
                POP     CX
                LOOP    SLED_C101
ELSE
if _SORM_NO_MODEM eq TRUE
                ; --- безмодемный вариант
                ; ставим коммутацию в ИКМ из HDLC
                mov     es, ss:SEG_ST
                mov     di, ADR_OZU_K1 + 256 + 30 * 2
                mov     BYTE PTR es:[di],     0
                mov     BYTE PTR es:[di + 1], 12
                mov     BYTE PTR es:[di + 2], 32
                mov     BYTE PTR es:[di + 3], 12
                ; ставим коммутацию в HDLC из ИКМ
                mov     di, ADR_OZU_K1 + 256 * 12
                mov     BYTE PTR es:[di],     30
                mov     BYTE PTR es:[di + 1], 1
                add     di, 32 * 2
                mov     BYTE PTR es:[di],     31
                mov     BYTE PTR es:[di + 1], 1

                ; настройка HDLC
                mov     es, ss:SEG_GS
                mov     BYTE PTR es:[5040h], 0          ; нулевой HDLC
                mov     BYTE PTR es:[5026h], 0          ; тайм-слотов
                mov     BYTE PTR es:[5055h], 0          ; прерывания разрешены ?
                mov     BYTE PTR es:[5040h], 1          ; то же для первого
                mov     BYTE PTR es:[5026h], 0          ;
                mov     BYTE PTR es:[5055h], 0          ;
                mov     BYTE PTR es:[5040h], 0          ;

                ; запустим контроллеры 0 и 1
                mov     BYTE PTR es:[5051h], 011b       ;
                mov     BYTE PTR es:[505fh], 8          ; HDLC в сегменте B000

                ; выполним начальную холостую передачу
                mov     BYTE PTR es:[5040h], 0          ; нулевой HDLC
                mov     BYTE PTR es:[5022h], 5          ; \ длина передаваемого
                mov     BYTE PTR es:[5023h], 0          ; / пакета
                mov     BYTE PTR es:[5020h], 4          ; обработал передачу
                mov     BYTE PTR es:[5020h], 1          ; начать передачу
                mov     BYTE PTR es:[5040h], 1          ; то же для первого
                mov     BYTE PTR es:[5022h], 5          ;
                mov     BYTE PTR es:[5023h], 0          ;
                mov     BYTE PTR es:[5020h], 4          ;
                mov     BYTE PTR es:[5020h], 1          ;
                mov     BYTE PTR es:[5040h], 0          ;
else
                ; --- модемный вариант
                ; ставим коммутацию в ИКМ из АК
                mov     es, ss:SEG_ST
                mov     di, ADR_OZU_K1 + 256 + 30 * 2
                mov     BYTE PTR es:[di],     0
                mov     BYTE PTR es:[di + 1], 6
                mov     BYTE PTR es:[di + 2], 1
                mov     BYTE PTR es:[di + 3], 6
                ; ставим коммутацию в АК из ИКМ
                mov     di, ADR_OZU_K1 + 256 * 6
                mov     BYTE PTR es:[di],     30
                mov     BYTE PTR es:[di + 1], 1
                mov     BYTE PTR es:[di + 2], 31
                mov     BYTE PTR es:[di + 3], 1
                ; активируем СОМ порты и АК
                mov     es, ss:SEG_GS
                mov     BYTE PTR es:[4410h], 33h        ; управляющий регистр СОМ портов для HDLC
                mov     BYTE PTR es:[5860h], 0bbh       ; включаем АК1 и АК2

                ; настройка HDLC
                mov     es, ss:SEG_GS
                mov     BYTE PTR es:[5040h], 10h        ; 10h-й HDLC
                mov     BYTE PTR es:[5055h], 0          ; прерывания разрешены ?
                mov     BYTE PTR es:[5040h], 11h        ; то же для 11h-го
                mov     BYTE PTR es:[5055h], 0          ;
                mov     BYTE PTR es:[5040h], 0          ;

                ; запустим контроллеры 0 и 1
                mov     BYTE PTR es:[5053h], 011b       ;
                mov     BYTE PTR es:[505fh], 8          ; HDLC в сегменте B000

                ; выполним начальную холостую передачу
                mov     BYTE PTR es:[5040h], 10h        ; 10h-й HDLC
                mov     BYTE PTR es:[5022h], 5          ; \ длина передаваемого
                mov     BYTE PTR es:[5023h], 0          ; / пакета
                mov     BYTE PTR es:[5020h], 4          ; обработал передачу
                mov     BYTE PTR es:[5020h], 1          ; начать передачу
                mov     BYTE PTR es:[5040h], 11h        ; то же для 11h-го
                mov     BYTE PTR es:[5022h], 5          ;
                mov     BYTE PTR es:[5023h], 0          ;
                mov     BYTE PTR es:[5020h], 4          ;
                mov     BYTE PTR es:[5020h], 1          ;
                mov     BYTE PTR es:[5040h], 0          ;
endif
ENDIF
        RET
INIC_HARD_C101  ENDP


IF _MSP EQ TRUE
DALEE:
                MOV     CX,KL_C101
                XOR     BX,BX
CN_ZAD_PAR:
                MOV     AX,OFFSET BUF_START
                MOV     TEKUSHEE_NACHALO[BX],AX
                MOV     OSTAVSHAYCY_DLINNA[BX],1
                SHL     BX,8
                LEA     SI,BUF_LEV[BX]
                SHR     BX,8
                MOV     TEKUSHEE_KUDA[BX],SI
                MOV     GDE_BUF[BX],SI
                MOV     DLINNA_PRINYTOGO[BX],0

                ADD     BX,2
                LOOP    CN_ZAD_PAR

                MOV     ES,SS:SEG_ST
                mov     BYTE PTR es:[COM_MODE], 070h    ; запуск коммутатора
                MOV     AX,OFFSET BUF_PRI
                MOV     NACH_VVOD,AX
                MOV     MAX_VVOD,AX
                ADD     MAX_VVOD,MAX_POKET
                ADD     AX,256
                MOV     NACH_VVOD[2],AX
                MOV     MAX_VVOD[2],AX
                ADD     MAX_VVOD[2],MAX_POKET

                LEA     AX,BUF_PRI
                MOV     SMESH_PRIEMA,AX
                MOV     NACH_BUF,AX
                ADD     AX,256
                MOV     SMESH_PRIEMA[2],AX
                MOV     NACH_BUF[2],AX

                MOV     SCH_SLED,8
                MOV     SCH_SLED[2],8
                MOV     BYTE_BYTE,EQU_FLAG
                MOV     BYTE_BYTE[2],EQU_FLAG
END_INI_HARD:
        ret
ENDIF
Init_Hard       ENDP

IF _MSP EQ TRUE
SAVE_TIME       PROC
	MOV	AL,0Bh			;\
	OUT	70h,AL			;|
	MOV	AL,10100011b		;|
	OUT	71h,AL			;|
	MOV	AL,0			;|
	OUT	70h,AL			;|
	OUT	71h,AL			;|
	MOV	AL,2			;|
        OUT     70h,AL                  ; \Установка времени и
        MOV     AL,MIN                  ; /инициализация системных часов
	OUT	71h,AL			;|
	MOV	AL,4			;|
	OUT	70h,AL			;|
        MOV     AL,CHAS                 ;|
	OUT	71h,AL			;|
	MOV	AL,0Bh			;|
	OUT	70h,AL			;|
	MOV	AL,00100011b		;|
        OUT     71h,AL                  ;/
        RET
SAVE_TIME       ENDP

SAVE_DATA       PROC
	MOV	AL,0Bh			;\
	OUT	70h,AL			;|
	MOV	AL,10100011b		;|
	OUT	71h,AL			;|
	MOV	AL,7			;|
	OUT	70h,AL			;|
        MOV     AL,DEN_MES              ;|
	OUT	71h,AL			;|
	MOV	AL,8			;|
        OUT     70h,AL                  ; \Установка даты и
        MOV     AL,MES                  ; /инициализация системной даты
	OUT	71h,AL			;|
        MOV     AL,10                   ;|
	OUT	70h,AL			;|
        MOV     AL,20                   ;|
	OUT	71h,AL			;|
	MOV	AL,9			;|
	OUT	70h,AL			;|
        MOV     AL,GOD_                 ;|
	OUT	71h,AL			;|
	MOV	AL,0Bh			;|
	OUT	70h,AL			;|
	MOV	AL,00100011b		;|
        OUT     71h,AL                  ;/
        RET
SAVE_DATA       ENDP





C101_GO         PROC
        XOR     BX,BX                   ; обнули индекс с101 (железо)
        MOV     CX,KL_C101              ; количество с101 (=2)
SLED_C101_2:
        PUSH    CX                      ; сохраним текущее значение с101
        MOV     ES,SEG_C101[BX]         ; настроимся на сегмент текущего с101

        TEST    ES:[ST1],BYTE PTR 80H   ; проверяем есть XXX    ??Х
        JZ      SHORT NET_UNDERRUN      ; нет, пропустим        ?Х?
        MOV     ES:[ST1],BYTE PTR 80H   ; иначе ставим          Х??
NET_UNDERRUN:
        TEST    FG_C101[BX],M_OR_0      ; есть пакет для обработки?
        JNZ     SHORT EST_PAKET         ; да -> перейдем на обработку
ZAPROS_PAKETA:
                XOR     CX,CX
if _test_SORM_wait eq TRUE   ; принудительная задержка на отправку пакета
        mov     ax, TIME_2mc
        sub     ax, WaitSorm[bx]
        cmp     ax, _WaitSorm
        jb      NET_PAKETA
endif
                PUSH    ES
                PUSH    BX
                DB      9AH
                DW      OFFSET  L1_EDINICAD
                DW      224
                POP     BX
                POP     ES
                OR      CX,CX
                JZ      NET_PAKETA
                ; --- {Paul.N} --- [01/07/2013]
                test    FG_PROGRAM, _fg_prg_log_sorm
                jz      No_SORM_LOG_Send_m
                push    es
                pushad
                mov     PEREM_DI, di
                call    PUMA_Start_Tar
                mov     al, 8fh
                @_STOSB
                mov     al, 1
                @_STOSB
IF _MSP EQ TRUE
                mov     al, cl
                @_STOSB
                shl     bx, 1
                mov     al, bl
                @_STOSB
                cmp     cx, 56
                jbe     SORM_LOG_Send_Loop_m
                mov     cx, 56
ELSE
                mov     al, bl          ; номер порта - 0..2
                @_STOSB
                mov     ax, SOST_SDL[bx]; состояние LAPB
                @_STOSB
                mov     ax, SOST_L3[bx] ; состояние Х25
                @_STOSB
                mov     al, cl          ; длина принятого пакета
                @_STOSB
ENDIF
SORM_LOG_Send_Loop_m:
                lodsb
                @_STOSB
                loop    SORM_LOG_Send_Loop_m
                call    PUMA_Send_Tar
                popad
                pop     es
No_SORM_LOG_Send_m:
                ; --- {Paul.N} --- [END]
if _test_SORM_wait eq TRUE  ; установка таймаута следующего пакета
        mov     ax, TIME_2mc
        mov     WaitSorm[bx], ax
endif
                OR      FG_C101[BX],M_OR_0
                MOV     L1_PAKET[BX],SI
                MOV     DLIN_L1_PAKET[BX],CX
        ; --- есть пакет для обработки
EST_PAKET:
;                TEST    ES:[ST0],BYTE PTR 0010B
;                JZ      BUF_POLON
                MOV     SI,L1_PAKET[BX]
                MOV     CX,DLIN_L1_PAKET[BX]
                CMP     CX,127
                JB      CN_ZAPIS
                MOV     CX,0
                MOV     FG_C101[BX],M_AND_0
                JMP     PAKET_ZAKONCHEN
CN_ZAPIS2:
                MOV     ES:[TRB],AL
CN_ZAPIS:
                TEST    ES:[ST0],BYTE PTR 0010B
                JZ      SHORT BUF_ZAPOLNEN
                LODSB
                LOOP    CN_ZAPIS2
                MOV     ES:[CMD],BYTE PTR 0110B          ;КОНЕЦ ПАКЕТА
                MOV     ES:[TRB],AL
                AND     FG_C101[BX],M_AND_0
                JMP     ZAPROS_PAKETA
BUF_ZAPOLNEN:
                OR      CX,CX
                JZ      SHORT PAKET_ZAKONCHEN
                OR      FG_C101[BX],M_OR_0
PAKET_ZAKONCHEN:
                MOV     L1_PAKET[BX],SI
                MOV     DLIN_L1_PAKET[BX],CX
BUF_POLON:
NET_PAKETA:
        ;
                MOV     AL,ES:[ST0]
                MOV     AL,ES:[ST1]
                TEST    ES:[ST0],BYTE PTR 0001B
                JZ      NET_PRIEMA
                MOV     CX,KOL_PRIEMA[BX]
                MOV     SI,SMESH_PRIEMA[BX]
CN_PRIEM:
                TEST    ES:[ST0],BYTE PTR 0001B
                JZ      BUF_PUST
                MOV     AL,ES:[TRB]
                MOV     DS:[SI],AL
                INC     CX
                INC     SI
                TEST    ES:[FSTT],BYTE PTR 80H
                JNZ     SHORT PAKET_POLUCHEN
NET_POLUCHENIY:
                CMP     CX,MAX_PAKET
                JB      SHORT CN_PRIEM
                        ;ПЛОХОЙ ПАКЕТ - ПРОПУЩЕН ФЛАГ
                XOR     CX,CX
                MOV     SI,NACH_BUF[BX]
                JMP     SHORT CN_PRIEM
PAKET_POLUCHEN:
                MOV     ES:[CMD],BYTE PTR 11000B
                PUSH    ES
                MOV     ES,SS:SEG_ST
                MOV     AL,ES:[00]
                MOV     AL,ES:[00]
                POP     ES
                MOV     ES:[CMD],BYTE PTR 10101B
                MOV     ES:[FSTT],BYTE PTR 0FFH
                MOV     SI,NACH_BUF[BX]
                MOV     SMESH_PRIEMA[BX],SI
                MOV     KOL_PRIEMA[BX],0
                ; --- {Paul.N} --- [01/07/2013]
                test    FG_PROGRAM, _fg_prg_log_sorm
                jz      No_SORM_LOG_Recv_m
                push    es
                pushad
                call    PUMA_Start_Tar
                mov     al, 8fh
                @_STOSB
                xor     al, al
                @_STOSB
IF _MSP EQ TRUE
                mov     al, cl
                @_STOSB
                mov     al, bl
                @_STOSB
                cmp     cx, 56
                jbe     SORM_LOG_Recv_Loop_m
                mov     cx, 56
ELSE
                mov     al, bl          ; номер порта - 0..2
                @_STOSB
                mov     ax, SOST_SDL[bx]; состояние LAPB
                @_STOSB
                mov     ax, SOST_L3[bx] ; состояние Х25
                @_STOSB
                mov     al, cl          ; длина принятого пакета
                @_STOSB
ENDIF
SORM_LOG_Recv_Loop_m:
                lodsb
                @_STOSB
                loop    SORM_LOG_Recv_Loop_m
                call    PUMA_Send_Tar
                popad
                pop     es
No_SORM_LOG_Recv_m:
                ; --- {Paul.N} --- [END]
                PUSH    ES
                DB      9AH
                DW      OFFSET  SIG_ED_LEV1D
                DW      224
                POP     ES
                JMP     SHORT NET_PRIEMA
BUF_PUST:
                MOV     SMESH_PRIEMA[BX],SI
                MOV     KOL_PRIEMA[BX],CX
NET_PRIEMA:
                ADD     BX,2                    ; перейдем на следующий с101
                POP     CX                      ; востановим текущий с101
                DEC     CX                      ; обработали все с101?
                JNZ     SLED_C101_2             ; нет -> сначала
                RET
C101_GO         ENDP
ENDIF
;;;if _SORM_NO_MODEM ne TRUE
C101_GO_NEW 	PROC               ;Обработчик HSCX на новом ИКМ
;ПРОЦЕДУРА ПРОВЕРКИ ПРИЕМА И ПЕРЕДАЧИ

		MOV	ES,SS:SEG_GS	  	;УСТАНАВЛИВАЕМ ES НА D000H
                MOV     BX,0                   ;12-13 HDLC ДЛЯ С101
		MOV	CX,2
;;		MOV 	AL,ES:[05026H]          ;ОБРАБОТКА ПРЕРЫВАНИЙ

CICKL:
		PUSH	CX
		push	bx
		MOV	DI,ADR_ST_PRIEM
		MOV 	AL,ES:[DI]
		MOV	CL,TAB_PRIEM[BX]
                TEST    AL,CL
		JNZ	PRIEM         	        ;*********************
CON_PERED:
		MOV	DI,ADR_ST_PERED
		MOV 	AL,ES:[DI]
		MOV	CL,TAB_PERED[BX]
		TEST	AL,CL
	        JNZ	PERED
CON_CICKL:
		pop	bx
		POP	CX
		INC	BX
		LOOP	CICKL
IF _MSP NE TRUE
                mov     es, ss:SEG_GS
                mov     BYTE PTR es:[5040h], 0
ENDIF
		RET

PRIEM:		;ОБРАБОТКА ПРИЕМА
                MOV     ES,SS:SEG_GS            ;УСТАНАВЛИВАЕМ ES НА D000H
		MOV	AL,TAB_OKNA[BX]
		MOV	ES:[05040H],AL          ;ВЫБОР "ОКНА" HDLC
		MOV	DI,BX
		SHL	DI,8               ;АДРЕСАЦИЯ
		LEA	DI,BUF_PRIEM[DI]   ;БУФФЕРА

		MOV	ES,SS:SEG_GS
		MOV	AL,ES:[05020H]
                TEST    AL,01H
		JNZ	PRINYAL_PAKET               ;A
		MOV	ES,SS:SEG_GS
		MOV	AL,ES:[05020H]
		TEST	AL,08H
		JNZ	BUFER_POLON                 ;B
CON_PRIEM:
		MOV	ES,SS:SEG_GS	  	;УСТАНАВЛИВАЕМ ES НА D000H
		mov	TEKUSHAYA_DLINA[BX],0
		MOV 	ES:[05020H],BYTE PTR 02H    ;В ПРОТИВНОМ СЛУЧАЕ
		MOV 	ES:[05020H],BYTE PTR 08H    ;ЕСЛИ НЕ А ИЛИ В
		JMP	CON_PERED

BUFER_POLON:    MOV	ES,SS:SEG_GS	  	;УСТАНАВЛИВАЕМ ES НА D000H
IF _MSP EQ TRUE
		MOV	SI,5500H
ELSE
		MOV	SI,800H
ENDIF
		MOVZX	CX,TEKUSHAYA_DLINA[BX]
		ADD	DI,CX                    ;Увеличиваем BUF_PRIEM
		MOV  	CX,064    		 ;ДЛИНА КУСКА ПАКЕТА
		PUSH	ES
		MOV	ES,SS:SEG_AX
		PUSH	DS
IF _MSP EQ TRUE
		MOV	DS,SS:SEG_GS
ELSE
		MOV	DS,SS:SEG_FL
ENDIF
 		CLD
        REP     MOVSB
		POP 	DS
		POP	ES
		ADD 	TEKUSHAYA_DLINA[BX],64
		MOV 	ES:[05020H],BYTE PTR 0AH  ;ЗАБРАЛ БУФЕР
		JMP	CON_PERED


PRINYAL_PAKET:
		MOV     ES,SS:SEG_GS
		MOV	AL,ES:[05020H]
		TEST	AL,02H
		JZ	CON_PRIEM	;В СЛУЧАЕ ЕСЛИ НЕ СОВПАЛ CRC
IF _MSP EQ TRUE
		MOV	SI,5500H
ELSE
                MOV     SI,800h
ENDIF
		MOVZX	CX,TEKUSHAYA_DLINA[BX]

		ADD	DI,CX
		MOVZX	CX,ES:[05024H]     ;ДЛИНА ПАКЕТА -1
                inc     cx
		AND	CX,3FH
		JZ	SHORT NET_LOAD_DATA
		PUSH	ES
		MOV	ES,SS:SEG_AX
		PUSH	DS
IF _MSP EQ TRUE
		MOV	DS,SS:SEG_GS
ELSE
                MOV     DS,SS:SEG_FL;;;!!!
ENDIF
		CLD

        REP     MOVSB ;!!!!
		POP 	DS
		POP	ES
NET_LOAD_DATA:
		PUSH	BX
		MOV	SI,BX
		SHL	SI,8               ;АДРЕСАЦИЯ
		LEA	SI,BUF_PRIEM[SI]   ;БУФФЕРА
		ADD	BX,BX               ;НОМЕР КАНАЛА HDLC
		MOVZX	CX,ES:[05024H]      ;ДЛИНА ПАКЕТА - 1
                inc     cx
;;!!
                test    FG_PROGRAM, _fg_prg_log_sorm
                jz      No_SORM_LOG_Recv
                push    es
                pushad
                call    PUMA_Start_Tar
                mov     al, 8fh
                @_STOSB
                xor     al, al
                @_STOSB
IF _MSP EQ TRUE
                mov     al, cl
                @_STOSB
                mov     al, bl
                @_STOSB
                cmp     cx, 56
                jbe     SORM_LOG_Recv_Loop
                mov     cx, 56
ELSE
                mov     al, bl          ; номер порта - 0..2
                @_STOSB
                mov     ax, SOST_SDL[bx]; состояние LAPB
                @_STOSB
                mov     ax, SOST_L3[bx] ; состояние Х25
                @_STOSB
                mov     al, cl          ; длина принятого пакета
                @_STOSB
ENDIF
SORM_LOG_Recv_Loop:
                lodsb
                @_STOSB
                loop    SORM_LOG_Recv_Loop
                call    PUMA_Send_Tar
                popad
                pop     es
No_SORM_LOG_Recv:
;;!!
IF _MSP NE TRUE
                ; --- вывод всего приёма СОРМ на экран
                cmp     cx, 5
                jbe     No_SORM_Scr_Recv
                test    FG_PROGRAM, _fg_prg_s1
                jz      No_SORM_Scr_Recv
                or      bx, bx
                jnz     No_SORM_Scr_Recv
                xor     dl, dl
                DB      9ah
                DW      OFFSET SORM_Scr_Debug_Far
                DW      224
No_SORM_Scr_Recv:

ENDIF
                PUSH    ES
                DB      9AH
                DW      OFFSET  SIG_ED_LEV1D
                DW      224
                POP     ES


;		CALL	SIG_ED_LEV1         ;ПЕРЕДАЧА НА 2 УРОВЕНЬ
		POP	BX
		JMP	CON_PRIEM



PERED:          ;ОБРАБОТКА ПЕРЕДАЧИ
		push	bx
		ADD	BX,BX         	;для l1_edinica bx=0,2,4,6

                PUSH    ES
                PUSH    BX
                DB      9AH
                DW      OFFSET  L1_EDINICAD
                DW      224
                POP     BX
                POP     ES

;		CALL 	L1_EDINICA
		pop	bx
                OR      CX,CX
		JZ	CON_CICKL
;;!!
                test    FG_PROGRAM, _fg_prg_log_sorm
                jz      No_SORM_LOG_Send
                push    es
                pushad
                mov     PEREM_DI, di
                call    PUMA_Start_Tar
                mov     al, 8fh
                @_STOSB
                mov     al, 1
                @_STOSB
IF _MSP EQ TRUE
                mov     al, cl
                @_STOSB
                shl     bx, 1
                mov     al, bl
                @_STOSB
                cmp     cx, 56
                jbe     SORM_LOG_Send_Loop
                mov     cx, 56
ELSE
                mov     al, bl          ; номер порта - 0..2
                @_STOSB
                mov     ax, SOST_SDL[bx]; состояние LAPB
                @_STOSB
                mov     ax, SOST_L3[bx] ; состояние Х25
                @_STOSB
                mov     al, cl          ; длина принятого пакета
                @_STOSB
ENDIF
SORM_LOG_Send_Loop:
                lodsb
                @_STOSB
                loop    SORM_LOG_Send_Loop
                call    PUMA_Send_Tar
                popad
                pop     es
No_SORM_LOG_Send:
;;!!

;                TEST    FG_POTOKA[BX],M_OR_12
;                JZ      SHORT NET_USER_PERED1
;                XOR     DS:[SI],BYTE PTR M_OR_1
;NET_USER_PERED1:

IF _MSP NE TRUE
                ; --- вывод всей передачи СОРМ на экран
                cmp     cx, 5
                jbe     No_SORM_Scr_Send
SORM_Scr_Send_1:test    FG_PROGRAM, _fg_prg_s1
                jz      SORM_Scr_Send_2
                mov     dl, 1
                or      bx, bx
                jz      SORM_Scr_Send
SORM_Scr_Send_2:test    FG_PROGRAM, _fg_prg_s2
                jz      No_SORM_Scr_Send
                mov     dl, 2
                or      bx, bx
                jz      No_SORM_Scr_Send
SORM_Scr_Send:  DB      9ah
                DW      OFFSET SORM_Scr_Debug_Far
                DW      224
No_SORM_Scr_Send:
ENDIF
		MOV	ES,SS:SEG_GS   	  ;УСТАНАВЛИВАЕМ ES НА D000H
		MOV	AL,TAB_OKNA[BX]
		MOV	ES:[05040H],AL    ;ВЫБОР "ОКНА" HDLC
IF _MSP EQ TRUE
		MOV 	DI,05400H
ELSE
                MOV     DI,0;;;!!!
ENDIF
		PUSH	CX
IF _MSP NE TRUE
                push    es;;;!!!
                mov     es, ss:SEG_FL;;;!!!
ENDIF
		CLD
                REP     MOVSB
IF _MSP NE TRUE
                pop     es;;;!!!!
ENDIF
		POP	CX
		MOV	ES:[05022H],CL
IF _MSP NE TRUE
		mov     BYTE PTR es:[5023h], 0
ENDIF
		MOV	ES:[05020H],BYTE PTR 01H
		MOV	ES:[05020H],BYTE PTR 04H
		JMP	CON_CICKL
C101_GO_NEW		ENDP
;;;endif


; *** обработчик 40 мс прерываний ********************
IRQ_40ms        PROC
                pushf
                pushad
                push    es
                push    ds
if 01;;;!!!

                call    PUMA_CTO_40ms
IF _MSP NE TRUE
; - - - - - - - - - - - - - - -  Ф А П Ч - - - - - - - - - - - - - - - - - - -
;
                MOV     ES,SS:SEG_PLL_DATA
                INC     ES:_CURRENT_TIME
ENDIF
                MOV     DS,SS:SEG_AX

;               В 40-ка МИЛЛИСЕКУНДНОЕ ПРЕРЫВАНИЕ ДОБАВИТЬ :
ASSUME  DS:DATA_BUS
MOV     DS,SS:SEG_BUS
                MOV     POINT_R_W,16
MOV     DS,SS:SEG_AX
ASSUME  DS:DATA_AXS
IF _MSP EQ TRUE
                MOV     POINT_SS,16
ENDIF

IRQ11:
                MOV     ES,SS:SEG_GS
                TEST    FG_PROG,1           ;Тест на железо
                JZ      Proba_prod          ;Если старое
		JMP	NEW_INDIK     	    ;Если новое железо
Proba_prod:
                MOV     CX,1;KL_1
                XOR     BX,BX
CN_PO_POT:
                MOV     DI,AGFA[BX]
                MOV     AL,ES:[DI]
                AND     RSR_0[BX],11011000B
                TEST    AX,M_OR_7
                JZ      SHORT DL_MTP1_1
                OR      RSR_0[BX],M_OR_0
DL_MTP1_1:
                TEST    AX,M_OR_6
                JZ      SHORT DL_MTP1_2
                OR      RSR_0[BX],M_OR_1
DL_MTP1_2:
                TEST    AX,M_OR_5
                JZ      SHORT DL_MTP1_3
                OR      RSR_0[BX],M_OR_2
DL_MTP1_3:
                TEST    AX,M_OR_4
                JZ      SHORT DL_MTP1_4
                OR      RSR_0[BX],M_OR_5
DL_MTP1_4:
                ADD     BX,2
                LOOP    CN_PO_POT

                JMP     CONT_PROG

;******* Индикация контроля ошибок на новом ИКМ *********
;Экран
if 1    ; выбор между новой и тарой индикацией
NEW_INDIK:      PUSH    DX
                push    bx
                push    cx
		MOV	ES,SS:SEG_GS
                xor     bx, bx
IF _MSP EQ TRUE
                MOV     CX,4
ELSE
                MOV     CX,1;4
ENDIF
CIKL:           PUSH    CX
                MOV     CL,BL
                MOV     AL,ES:[ADR_NVP]
IF _MSP NE TRUE
		AND	AL,1
		AND	RSR_0,0FEH
		OR	RSR_0,AL
ENDIF
                SHR     AL,CL
IF _MSP EQ TRUE
                and     rsr_0[bx], 0feh
                OR      RSR_0[BX], al
ELSE
                TEST    Al,00000001B
                JZ      PROV_SIA
                OR      RSR_0[BX],byte ptr 00000001B
ENDIF
PROV_SIA:
                MOV     AL,ES:[ADR_SIA]
IF _MSP EQ TRUE
                SHR     AL,CL
ELSE
                ADD     CL,4
ENDIF
                shr     al, 4
                AND     AL,1
		SHL	AL,1
IF _MSP EQ TRUE
                AND     RSR_0[bx],011111101B
                OR      RSR_0[BX],al
ELSE
		AND	RSR_0,011111101B
		OR	RSR_0,AL
                TEST    Al,00000001B
                JZ      PROV_PCS
                OR      RSR_0[BX],byte ptr 00000010B
ENDIF
PROV_PCS:
                MOV     AL,ES:[ADR_PCS]
IF _MSP EQ TRUE
                shr     al,cl
ENDIF
                AND     AL,1
		SHL	AL,2
IF _MSP EQ TRUE
                AND     RSR_0[bx],011111011B
                OR      RSR_0[bx],AL
ELSE
		AND	RSR_0,011111011B
		OR	RSR_0,AL
                SUB     CL,4
                SHR     Al,CL
                TEST    AL,00000001B            ;
                JZ      PROV_AUS
                OR      RSR_0[BX],byte ptr 00000100B
ENDIF
PROV_AUS:
                MOV     AL,ES:[ADR_AUS]
IF _MSP EQ TRUE
                SHR     AL,cl
                shr     al, 4
                AND     AL,1
		SHL	AL,5
                AND     RSR_0[bx],011011111B
                OR      RSR_0[bx],AL
ELSE
                SHR     AL,4
		AND	AL,1
		SHL	AL,5
		AND	RSR_0,011011111B
		OR	RSR_0,AL
                ADD     CL,4
                SHR     AL,CL
                TEST    AL,00000001B            ;
                JZ      PROV_E_3_5              ;
                JZ      ALL_RIGHT
                OR      RSR_0[BX],byte ptr 00100000B
                jmp     prov_e_3_5
ENDIF
ALL_RIGHT:

                POP     CX
                INC     BL
                DEC     CX
                JNZ     CIKL
;                LOOP    CIKL

;;;call    Alt_Alarm

                pop     cx
                pop     bx
		POP	DX
                JMP     CONT_PROG


PROV_E_3_5:     call    Alt_Alarm
                jmp     ALL_RIGHT
Alt_Alarm:
                PUSHA
                xor     si, si
                mov     dx, 1001h       ; в dh/dl маски для выделения 7..4/3..0
Alt_Alarm_Loop: movzx   bx, FEC_Ptr[si]
                inc     bx
                and     bl, 127
                mov     FEC_Ptr[si], bl         ; указатель в накопители FEC по mod 128

                movzx   cx, BYTE PTR ES:[05030H + si] ; прочитать FEC
                mov     BYTE PTR ES:[05030H + si], 0  ; очистить счетчик FEC
                shld    di, si, 16 + 7
                movzx   ax, FEC1[di + bx]       ; прочитать старый FEC из накопителя
                mov     FEC1[di + bx], cl       ; заменить старый FEC на новый
                shl     si, 1
                add     cx, FEC_Sum[si]         ; добавить новый FEC к сумме
                sub     cx, ax                  ; вычесть старый
                mov     FEC_Sum[si], cx         ; = новая скользящая сумма
                mov     bx, cx
                shr     si, 1

                cmp     bx, 40                  ; анализ FEC: ошибок > 40 ?
                jbe     SHORT Alt_FEC_5         ; НЕТ - на проверку E-5
                or      RSR_0[SI], 00011000B    ; ДА -  E-3 и  E-5
Alt_FEC_5:      or      bx, bx
                jz      SHORT Alt_Send_Err      ; ошибок нет
                jz      SHORT Alt_Alarm_Next    ; ошибок нет
                or      RSR_0[SI], 00010000B    ; есть ошибки -  E-5

Alt_Send_Err:   ;mov     PER[si], ah            ; запомнить байт ошибок
                mov     al, ES:4408h
                not     dl
                and     al, dl
                not     dl
                test    ah, 0fh                 ; есть E-3, ПЦС, СИА или НВП ?
                jz      SHORT Alt_Send          ; НЕТ -
                or      al, dl
Alt_Send:       mov     ES:4408h, al            ; пошлем АУС для соответ. ИКМ

Alt_Alarm_Next: shl     dx, 1
                inc     si
                cmp     si, 4
                jb      Alt_Alarm_Loop

                popa
                ret


else
;; **********************************************************
;; **********************************************************
;; **********************************************************

NEW_INDIK:      PUSH    DX
                push    bx
                push    cx
		MOV	ES,SS:SEG_GS
		MOV	BH,0
		MOV 	BL,0
                MOV     CX,4
CIKL:           PUSH    CX
		MOV	RSR_0[BX],0
		MOV	RSR_0_GLB[BX],0
                MOV     CL,BL
                MOV     Al,ES:[ADR_NVP]
                SHR     Al,CL
                TEST    Al,00000001B
		JZ     	PROV_SIA
                OR      RSR_0[BX],byte ptr 00000001B
                OR      RSR_0_GLB[BX],byte ptr 00000001B
PROV_SIA:
                MOV     Al,ES:[ADR_SIA]
                ADD     CL,4
                SHR     Al,CL
                TEST    Al,00000001B
                JZ     PROV_PCS
                OR      RSR_0[BX],byte ptr 00000010B
PROV_PCS:
                MOV     Al,ES:[ADR_PCS]
		SUB	CL,4
                SHR     Al,CL
                TEST    AL,00000001B            ;
                JZ      PROV_AUS
                OR      RSR_0[BX],byte ptr 00000100B
                OR      RSR_0_GLB[BX],byte ptr 00000010B
PROV_AUS:
                MOV     AL,ES:[ADR_AUS]
		ADD	CL,4
                SHR     AL,CL
                TEST    AL,00000001B            ;
                JZ      PROV_E_3_5              ;
;                JZ      ALL_RIGHT
                OR      RSR_0_GLB[BX],byte ptr 00010000B
                OR      RSR_0[BX],byte ptr 00100000B
                jmp     prov_e_3_5
ALL_RIGHT:

                POP     CX
                INC     BL
                LOOP    CIKL
                pop     cx
                pop     bx
		POP	DX
                JMP     CONT_PROG


PROV_E_3_5:     call    Alt_Alarm
                jmp     ALL_RIGHT
Alt_Alarm:
                PUSHA
                xor     si, si
                mov     dx, 1001h       ; в dh/dl маски для выделения 7..4/3..0
Alt_Alarm_Loop: movzx   bx, FEC_Ptr[si]
                inc     bx
                and     bl, 127
                mov     FEC_Ptr[si], bl ; указатель в накопители FEC по mod 128

                movzx   cx, BYTE PTR ES:[05030H + si] ; прочитать FEC
                mov     BYTE PTR ES:[05030H + si], 0  ; очистить счетчик FEC
                shld    di, si, 16 + 7
                movzx   ax, FEC1[di + bx]; прочитать старый FEC из накопителя
                mov     FEC1[di + bx], cl; заменить старый FEC на новый
                shl     si, 1
                add     cx, FEC_Sum[si] ; добавить новый FEC к сумме
                sub     cx, ax          ; вычесть старый
                mov     FEC_Sum[si], cx ; = новая скользящая сумма
                mov     bx, cx
                shr     si, 1

                cmp     bx, 40                  ; анализ FEC: ошибок > 40 ?
                jbe     SHORT Alt_FEC_5         ; НЕТ - на проверку E-5
                or      RSR_0[SI], 00011000B    ; ДА -  E-3 и  E-5
                OR      RSR_0_GLB[si],byte ptr 000001100b
Alt_FEC_5:      or      bx, bx
;                jz      SHORT Alt_Send_Err      ; ошибок нет
                jz      SHORT Alt_Alarm_Next    ; ошибок нет
                OR      RSR_0_GLB[si],byte ptr 00000100B
                or      RSR_0[SI], 00010000B    ; есть ошибки -  E-5

;Alt_Send_Err:   mov     PER[si], ah             ; запомнить байт ошибок
;                mov     al, ES:[ALT_AUS_WR]
;                not     dl
;                and     al, dl
;                not     dl
;                test    ah, 0fh                 ; есть E-3, ПЦС, СИА или НВП ?
;                jz      SHORT Alt_Send          ; НЕТ -
;                or      al, dl
;Alt_Send:       mov     ES:[ALT_AUS_WR], al     ; пошлем АУС для соответ. ИКМ

Alt_Alarm_Next: shl     dx, 1
                inc     si
                cmp     si, 4
                jb      Alt_Alarm_Loop

                popa
                ret
endif


;; **********************************************************
;; **********************************************************
;; **********************************************************

CONT_PROG:
        MOVZX   BX,C_FEC
        INC     BX
        AND     BX,127
        MOV     C_FEC,BL
        SHL     BX,2
        CMP     ON_PCM,0
        JE      SHORT   PCM_1S
        MOV     BYTE PTR ES:[ADR_CCR0],10h
        MOV     AL,ES:[ADR_RSR0]
        MOV     RSR_0,AL
        MOVZX   EDX,BYTE PTR ES:[ADR_FEC0]
        MOV     BYTE PTR ES:[ADR_CCR0],0
        XOR     AL,AL
        MOV     ECX,EDX
        ADD     EDX,FEC
        SUB     EDX,FEC_0[BX]
        MOV     FEC_0[BX],ECX
        MOV     FEC,EDX
        JZ      SHORT   NE_0
        MOV     AL,00010000b
        CMP     EDX,40
        JBE     SHORT   NE_0
        MOV     AL,00011000b
NE_0:   MOV     COD_E0,AL
PCM_1S: CMP     ON_PCM[1],0
        JE      SHORT   PCM_2S
        MOV     BYTE PTR ES:[ADR_CCR1],10h
        MOV     AL,ES:[ADR_RSR1]
        MOV     RSR_1,AL
        MOVZX   EDX,BYTE PTR ES:[ADR_FEC1]
        MOV     BYTE PTR ES:[ADR_CCR1],0
        XOR     AL,AL
        MOV     ECX,EDX
        ADD     EDX,FEC[4]
        SUB     EDX,FEC_1[BX]
        MOV     FEC_1[BX],ECX
        MOV     FEC[4],EDX
        JZ      SHORT   NE_1
        MOV     AL,00010000b
        CMP     EDX,40
        JBE     SHORT   NE_1
        MOV     AL,00011000b
NE_1:   MOV     COD_E1,AL
PCM_2S: CMP     ON_PCM[2],0
        JE      SHORT   PCM_3S
        MOV     BYTE PTR ES:[ADR_CCR2],10h
        MOV     AL,ES:[ADR_RSR2]
        MOV     RSR_2,AL
        MOVZX   EDX,BYTE PTR ES:[ADR_FEC2]
        MOV     BYTE PTR ES:[ADR_CCR2],0
        XOR     AL,AL
        MOV     ECX,EDX
        ADD     EDX,FEC[8]
        SUB     EDX,FEC_2[BX]
        MOV     FEC_2[BX],ECX
        MOV     FEC[8],EDX
        JZ      SHORT   NE_2
        MOV     AL,00010000b
        CMP     EDX,40
        JBE     SHORT   NE_2
        MOV     AL,00011000b
NE_2:   MOV     COD_E2,AL
PCM_3S: CMP     ON_PCM[3],0
        JE      SHORT   E_12
        MOV     BYTE PTR ES:[ADR_CCR3],10h
        MOV     AL,ES:[ADR_RSR3]
        MOV     RSR_3,AL
        MOVZX   EDX,BYTE PTR ES:[ADR_FEC3]
        MOV     BYTE PTR ES:[ADR_CCR3],0
        XOR     AL,AL
        MOV     ECX,EDX
        ADD     EDX,FEC[12]
        SUB     EDX,FEC_3[BX]
        MOV     FEC_3[BX],ECX
        MOV     FEC[12],EDX
        JZ      SHORT   NE_3
        MOV     AL,00010000b
        CMP     EDX,40
        JBE     SHORT   NE_3
        MOV     AL,00011000b
NE_3:   MOV     COD_E3,AL
E_12:   MOV     AL,RR_PHA
        AND     AL,3
        CMP     AL,2
        JNE     SHORT   R_12
        MOVZX   BX,RR_PCM
        CMP     ON_PCM[BX],0                    ;Данный ИКМ включен ?
        JE      SHORT   FAPCH_C                 ;НЕТ - изменить источник
        TEST    RSR_0[BX],11100000b             ;Есть ошибки (НВП,СИА,ПЦС) ?
        JNZ     SHORT   FAPCH_C                 ;ДА - изменить источник
        TEST    COD_E0[BX],00010000b            ;Есть Е-3 ?
        JZ      SHORT   R_12                    ;НЕТ -
FAPCH_C:MOV     CX,3                            ;ПОИСК ИКМ ИСТОЧНИКА ФАПЧ
FAPCH_0:INC     BX
        AND     BX,00000011b                    ;Номер следующего ИКМа
        CMP     ON_PCM[BX],0                    ;Данный ИКМ включен ?
        JE      SHORT   FAPCH_1                 ;НЕТ - следующий
        TEST    RSR_0[BX],11100000b             ;Есть ошибки (НВП,СИА,ПЦС) ?
        JNZ     SHORT   FAPCH_1                 ;ДА -
        TEST    COD_E0[BX],00010000b            ;Есть Е-3 ?
        JZ      SHORT   FAPCH_N                 ;НЕТ -
FAPCH_1:LOOP    FAPCH_0
        JMP     SHORT   R_12
FAPCH_N:MOV     RR_PCM,BL
        MOV     ES:[ADR_RPCM],BL                ;Установка ИКМ - источника ФАПЧ
R_12:

        MOV     CX,KL_1
        XOR     BX,BX
CN_IRQ12_11:
        MOV     AL,COD_E0[BX]
        SHL     BX,1
        AND     RSR_0[BX],00100111B
        OR      RSR_0[BX],AL
;        CMP     SOST_SDL[BX],6
;        JE      SHORT DL_IRQ12_11
;        OR      RSR_0[BX],10000000B
;        CMP     SOST_SDL[BX],7
;        JE      SHORT DL_IRQ12_11
;        OR      RSR_0[BX],01000000B
DL_IRQ12_11:
        SHR     BX,1
        INC     BX
        LOOP    CN_IRQ12_11


endif;;;!!!
Exit_IRQ11:
        MOV     AL,20h
        OUT     0A0h,AL
	JMP	$+2
        OUT     20h,AL
	JMP	$+2
        mov     al, 020h
        out     020h, al
        out     0a0h, al
        pop     ds
        pop     es
        popad
        popf
        IRETD
IRQ_40ms        ENDP


IF _MSP EQ TRUE
L1_VIVOD_       PROC
        MOV     CX,8
L1_VIV:
        MOV     SI,SOST_L1_V[BX]
        SHL     SI,1
        JMP     ADR_L1_V[SI]
VIV_PUST:                                      ;0-ВЫВОД ПУСТОГО ПАКЕТА
;        CALL    L1_EDINICA
        MOV     AL,PUST_POKET
        OR      CX,CX
        JZ      L1_VIV_OK
        MOV     PER_CX[BX],CX
        MOV     PER_SI[BX],SI
        MOV     SOST_L1_V[BX],1
        RET

VIV_PREAM:                                     ;1-ВЫВОД ПРЕАМБУЛЫ
        MOV     AL,PREAMBULA
        MOV     SOST_L1_V[BX],2
        RET
VIV_CX:                                 ;2-ВЫВОД ДЛИННЫ
        MOV     AX,PER_CX[BX]
        MOV     SOST_L1_V[BX],3
        RET
VIV_POKET:                                      ;3-ВЫВОД ПАКЕТА
        MOV     SI,PER_SI[BX]
        LODSB
        MOV     PER_SI[BX],SI
        DEC     PER_CX[BX]
        JNZ     L1_VIV_OK
        MOV     SOST_L1_V[BX],0
        RET
L1_VIV_OK:
        RET
L1_VIVOD_       ENDP

L1_VVOD         PROC

VVOD_IZ_PORTA:
                MOV     SI,SOST_L1_VV[BX]
                SHL     SI,1
                JMP     ADR_L1_VV[SI]
VVOD_FLAG:                                              ;0-ПОИСК ПРЕАМБУЛЫ
                CMP     AL,PREAMBULA
                JNE     L1_VIV_OK
                MOV     SOST_L1_VV[BX],1
                RET
VVOD_FLAG_1:                                            ;1-ВВОД ДЛИННЫ
                MOVZX   CX,AL
                CMP     CX,MAX_DLINNA
                JA      BAD_POKETA
                CMP     CX,MIN_DLINNA
                JB      BAD_POKETA
                MOV     PRI_CX[BX],CX
                MOV     PRI_CX2[BX],CX
                MOV     SI,BX
                SHL     SI,6
                LEA     SI,BUF_L1_L1[SI]
                MOV     PRI_SI[BX],SI
                MOV     PRI_SI2[BX],SI
                MOV     SOST_L1_VV[BX],2
                RET
BAD_POKETA:
                MOV     SOST_L1_VV[BX],0
                RET

VVOD_POKET:                                            ;2-ВВОД ПАКЕТА
                MOV     SI,PRI_SI[BX]
                MOV     DS:[SI],AL
                INC     SI
                MOV     PRI_SI[BX],SI
                DEC     PRI_CX[BX]
                JNZ     L1_VIV_OK
                MOV     SOST_L1_VV[BX],0
                MOV     CX,PRI_CX2[BX]
                MOV     SI,PRI_SI2[BX]
;                JMP     SIG_ED_LEV1
                ENDP
ENDIF

; *** обработчик 2 мс прерываний *********************
IRQ_2ms         PROC
                pushf
                pushad
                push    es
                push    ds
if 01;;;!!!
IF _MSP EQ TRUE
;               В 2-х МИЛЛИСЕКУНДНОЕ ПРЕРЫВАНИЕ ДОБАВИТЬ :
                PUSH    FS
                MOV     ES,SS:SEG_BABA
                MOV     DS,SS:SEG_BUS
                MOV     FS,SS:SEG_AX
                ASSUME  DS:DATA_BUS
                MOV     DX,13
                MOV     BP,POINT_R_W
                ADD     BP,4000h
NEXT_P:
                MOV     DI,DX
                CMP     FS:Modem_KSL[DI],BYTE PTR 0ffh
                JE      SHORT NET_TH
                SHLD    DI,DX,25
                ADD     DI,BP
                MOV     CX,16
R_A_W:          MOVZX   BX,BYTE PTR ES:[DI-4000h]
                SHL     BX,1
                MOV     SI,TAB_AL[BX]
                MOVZX   BX,BYTE PTR ES:[DI-2000h]
                SHL     BX,1
                ADD     SI,TAB_AL[BX]
                MOV     AL,TAB_L[SI];
                STOSB
                LOOP    R_A_W;
NET_TH:
                DEC     DX
                JNS     SHORT   NEXT_P
                ADD     POINT_R_W,16
                CMP     POINT_R_W,320
                JB      CONTINUE1
                MOV     POINT_R_W,0
CONTINUE1:
                POP     FS
ELSE
                @COMM_SUM_AB
ENDIF
endif;;;!!!

                ASSUME  DS:DATA_AXS
                MOV     DS,SS:SEG_AX
IF _MSP EQ TRUE
                inc     Sys_Time        ;;;!!!<Yu>
                inc     ss:Curr_T       ;;;!!!<Yu>

if 01;;;!!!
                call    PUMA_CTO_2ms
endif;;;!!!
ENDIF
                ADD     TIME_2MC,2
                ADD     DTIME_2MC,2
                MOV     AX,TIME_2MC
                AND     AX,3FFH
                JNZ     SHORT NET_SBROS_IKM
                MOV     ES,SS:SEG_GS
                MOV     ES:[5041H],AL
NET_SBROS_IKM:

                mov     es, SS:SEG_GS
FAPCH_Handler:
IF _MSP EQ TRUE
                ; REBROV variant fapch
                MOVZX   EAX,BYTE PTR ES:[ADR_RD0_PHA]   ;Читаем показания регистра фазы
                MOV     AH,ES:[ADR_RD1_PHA]
                MOV     CL,AH
                AND     AH,0Fh                          ;Берем 12 разрядов
                MOV     IND_PHA,2
                TEST    CL,10000000b                    ;Есть входной поток ?
                JNZ     E_PHA                           ;НЕТ - пропустим рег-ку ФАПЧ
                MOV     IND_PHA,0
                MOV     EBX,FI                          ;в EBX предыдущее полож-е фазы
                MOV     FI,EAX                          ;Запомним положение фазы
                AND     BX,0C00h                        ;10 и 11 разр.предыд. = 00 ?
                JNZ     SHORT   I15_0                   ;НЕТ -
                AND     AX,0C00h
                CMP     AX,0C00h                        ;10 и 11 разр.тек. = 11 ?
                JNE     SHORT   I15_1                   ;НЕТ - диапазон не изменился
                SUB     DIA,4096                        ;Сдвигаем диап-н влево на кадр
                CMP     DIA,-8192                       ;Вышли за предел ?
                JGE     SHORT   I15_1                   ;НЕТ -
                MOV     DIA,-8192                       ;Возвращаем диапазон на предел
                JMP     SHORT   I15_1
I15_0:          CMP     BX,0C00h                        ;10 и 11 разр.предыд. = 11 ?
                JNE     SHORT   I15_1                   ;НЕТ - диапазон не изменился
                AND     AX,0C00h                        ;10 и 11 разр.тек. = 00 ?
                JNZ     SHORT   I15_1                   ;НЕТ - диапазон не изменился
                ADD     DIA,4096                        ;Сдвигаем диап-н вправо на кадр
                CMP     DIA,8192                        ;Вышли за предел ?
                JLE     SHORT   I15_1                   ;НЕТ -
                MOV     DIA,8192                        ;Возвращаем диапазон на предел
;
I15_1:          MOV     EAX,FI
                ADD     EAX,DIA
                ADD     SUM_PHA,EAX
                DEC     TIF                             ;Цикл ФАПЧ закончился ?
                JNZ     E_PHA                           ;НЕТ - пропустим рег-ку ФАПЧ
                MOV     TIF,32
                MOV     EBX,SUM_PHA
                MOV     SUM_PHA,0
;
                MOV     EAX,TKI                         ;\
                SHL     EAX,5
                SUB     EAX,EBX                         ;В EAX - ДЕЛЬТА
                JA      SHORT   I15_2
                CMP     C_I[1], 0
                JNE     SHORT   I15_2
                DEC     EAX
I15_2:          ADD     I_SUM,EAX                       ;\
                MOV     EBX,I_SUM
                MOVZX   CX,C_I
                JCXZ    I15_3
                MOV     EDX,1
                DEC     CL
                SHL     EDX,CL
                INC     CL
                ADD     EBX,EDX
                SAR     EBX,CL
I15_3:          ADD     EBX,EAX
                MOVZX   CX,C_P
                JCXZ    I15_4
                MOV     EDX,1
                DEC     CL
                SHL     EDX,CL
                INC     CL
                ADD     EAX,EDX
                SAR     EAX,CL
I15_4:          ADD     EAX,EBX
                MOVZX   CX,C_P[1]
                JCXZ    I15_5
                MOV     EDX,1
                DEC     CL
                SHL     EDX,CL
                INC     CL
                ADD     EAX,EDX
                SAR     EAX,CL
I15_5:          MOVZX   EDX,VCAPI
                ADD     EAX,EDX
                JNS     SHORT   I15_7                   ; |     ВЫЧИСЛЯЕМ
                MOV     IND_PHA,1                       ; |
                MOV     EAX,0                           ; |     И
                JMP     SHORT   I15_8                   ; >     ОГРАНИЧИВАЕМ
I15_7:          CMP     EAX,65471                       ; |     ОБЩУЮ
                JLE     SHORT   I15_8                   ; |     СУММУ
                MOV     IND_PHA,1                       ; |
                MOV     EAX,65471                       ;/      КОД ЦАП ( в EAX )
I15_8:
                MOV     ES:[ADR_RD0_PHA],AL             ;Записываем код ЦАП
                MOV     ES:[ADR_RD1_PHA],AH
                MOV     VCAP,AX
E_PHA:
ELSE
                CALL_PLL PLL2MS                         ;Обработка ФАПЧ
ENDIF
Exit_IRQ15:
IF _MSP NE TRUE
                inc     Sys_Time        ;;;!!!<Yu>
                inc     ss:Curr_T       ;;;!!!
                inc     Current_Time
                ; ---
                call    PUMA_CTO_2ms
ENDIF
                mov     es, SS:SEG_ST
                mov     al, es:[COM_MODE]       ; сброс прерывания

                mov     al, 020h
                out     020h, al
                out     0a0h, al

                pop     ds
                pop     es
                popad
                popf
                IRETD

IRQ_2ms         ENDP

;╔══════════════════════════════════════════════════════════════════════╗
;║     ==           прерывание модема по приему пакета            ==    ║ █
;╚══════════════════════════════════════════════════════════════════════╝ █
;  ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
INTIR10         PROC
        push    ax ds gs
        mov     DS,SS:SEG_AX
        MOV     GS,SS:SEG_GS
IF _MSP EQ TRUE
        xor     ax, ax
ENDIF
;        mov     al, gs:[MO_IN]
;        mov     MO_Length, ax
        mov     MO_Flg, 0ffh
        mov     al, 20h
        out     0a0h, al
        out     020h, al
        pop     GS ds ax
                IRETD
INTIR10         ENDP

;╔═════════════════════════════════════════════════════════════════════════╗
;║   пустое прерывание (заглушка), в принципе сюда мы не должны попадать   ║ █
;╚═════════════════════════════════════════════════════════════════════════╝ █
;  ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
INT_NULL1       PROC
        PUSH    AX
        mov     al, 20h
        out     0a0h, al
        out     020h, al
        POP     AX
        IRETD
INT_NULL1       ENDP
IF _MSP NE TRUE
INTIR11         PROC    FAR                             ;ОБРАБОТЧИК 2-х МС ПРЕРЫВАНИЙ
                PUBLIC  INTIR11
                CLI
                cld
                PUSH    AX
                PUSH    DS
                PUSH    ES
                PUSH    GS

                mov     gs, ss:SEG_GS
                mov     al,BYTE PTR GS:[5870h]
INTIR11_1:
                TEST    al,2h
                JZ      SHORT   INTIR11_0
                MOV     BYTE PTR GS:[5870H],2
                INT     43        ;40MS
INTIR11_0:
                TEST    al,1h
                JZ      SHORT   INTIR11_2
                MOV     BYTE PTR GS:[5870H],1
                INT     47        ;2MS
INTIR11_2:
                TEST    al,4h
                JZ      SHORT   INTIR11_3
                MOV     BYTE PTR GS:[5870H],4
                INT     42        ;MODEM
INTIR11_3:
                mov     al,BYTE PTR GS:[5870h]
                TEST    al,7h  ;7h
                JNZ     SHORT   INTIR11_1

EXIT_11:
                MOV     AL,20h
                OUT     0A0h,AL
                JMP     $+2
                OUT     20h,AL
                JMP     $+2
                POP     GS
                POP     ES
                POP     DS
                POP     AX
                STI
                IRETD
INTIR11         ENDP
ENDIF
