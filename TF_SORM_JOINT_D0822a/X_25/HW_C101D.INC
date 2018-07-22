
KL_1            EQU     1       ;КОЛИЧЕСТВО ЗВЕНЬЕВ
; *** Определения адресов ****************
RAM_DAT         EQU     02800h  ;

ADR_RU          EQU     1B00H

COM_1           EQU     03000h  ; ОЗУ коммутации 1
COM_MODE        EQU     03420h  ; регистр режима коммутатора

SUPPLY1         EQU     03441h  ; регистр первого источника питания, RD/WR
;
;               чтение
;    ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐ 1 - норма
;    │-7.5В│+7.5B│-15B │+15B │ -5B │-12B │+12B │ +5B │ 0 - авария
;    └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
;
;               запись
;    ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐ 1 - норма
;    │  X  │  X  │  X  │  X  │  X  │  X  │ ИП2 │ ИП1 │ 0 - авария
;    └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
;
SUPPLY2         EQU     03442h  ; регистр второго источника питания, RD only


EX_COM          EQU     03800h  ; дополнительное ОЗУ коммутации


TC_BUFF         EQU     05000h
IPAT            EQU     05200h
IF _MSP EQ TRUE
MO_LEN          EQU     03425h  ; регистр длины пакета МО
ENDIF
PCM_MODE        EQU     05300h  ; регистр управления ИКМ
PCM_RES_NUM     EQU     05340h  ; регистр индикации номера / сброса ИКМ
PCM_ERR         EQU     05380h  ; регистр индикации ошибок ИКМ
PCM_RESET       EQU     05340h  ; регистр сброса ИКМ
IF _MSP EQ TRUE
MO_MODE         EQU     03424h  ; регистр режима модема
MO_LEN          EQU     03425h  ; регистр длины пакета МО
MO_STAT         EQU     03426h  ; регистр сотояния модема
ENDIF
MO_CODE         EQU     04200h  ; ОЗУ кодировки модема
MO_DECODE       EQU     04300h  ; ОЗУ декодировки модема

RFIFO           EQU     05500h  ; базовый адрес канала В контроллера HSCX
XFIFO           EQU     05500h  ;

MASC            EQU     05520h  ;
CMDR            EQU     05521h  ;
  RMC           EQU     080h    ;
  RHR           EQU     040h    ;
  XTF           EQU     8       ;
  XME           EQU     2       ;
  XRES          EQU     1       ;

MODE            EQU     05522h  ;
CCR2            EQU     0552ch  ;
CCR1            EQU     0552fh  ;
TSAX            EQU     05530h  ;
TSAR            EQU     05531h  ;
XCCR            EQU     05532h  ;
RCCR            EQU     05533h  ;

EXIR            EQU     05524H  ;

ISTA            EQU     05520h  ;
  RME           EQU     080h    ;
  RPF           EQU     040h    ;
  XPR           EQU     010h    ;
  ICA           EQU     4       ;

STAR            EQU     05521h  ;
  XFW           EQU     64      ;
  CEC           EQU     4       ;

EXIR            EQU     05524h  ;
RBCL            EQU     05525h  ;
RSTA            EQU     05527h  ;
  VFR           EQU     080h    ;
  RDO           EQU     040h    ;
  CRC           EQU     020h    ;
  RAB           EQU     010h    ;

RBCH            EQU     0552dh  ;
  OV            EQU     010h    ;


ADR_RD0_PHA     EQU     0bf80h  ;Адрес младшего байта данных ФАПЧ
                                ; При записи младшие разряды кода ЦАП
                                ; При чтении младшие разряды положения фазы
ADR_RD1_PHA     EQU     0bf81h  ;Адрес старшего байта данных ФАПЧ
ADR_RR_PHA      EQU     0bf82h  ;Адрес регистра режима ФАПЧ
ADR_5300H       EQU     5300H


; *** Конфигурация ФАПЧ *****************
        ; эталоны
_ET_DGP         equ     350
        ; переменные
TKI             DD      2048    ;Требуемое значение регистра фазы
DGP             DD      _ET_DGP ;Допуск перехода на грубую подстройку(+)11
MING            DD      0       ;Допуск перехода на грубую подстройку(-)
TOF             DW      30      ;Цикл ФАПЧ в двухмиллисекундных интервалах
;;;DELTA           DD      0
SUM_PHA         DD      0       ;DELTA           DD      0 морально устарела
FSUM            DD      0       ; для ОКС варианта вместо SUM_PHA
MAX_UR          DW      3       ;Максимальный уровень регулирования
VCAPI           DW      8000h   ;Код ЦАП - исходное значение
RR_PHA_I        DB      2       ;Регистр режима ФАПЧ - исходное значение
C_P             DB      2,3,0,0 ;10,8,6,4;Коэффициенты пропорцианальной составляющей
C_I             DB      7,0,0,0 ;7,5,3,1 ;Коэффициенты интегральной составляющей
;C_P             DB      10,8,6,4;Коэффициенты пропорцианальной составляющей
;C_I             DB      7,5,3,1 ;Коэффициенты интегральной составляющей

_ADR_FAPCH_0    EQU     0bf80h  ; смещение младшего байта данных ФАПЧ
_ADR_FAPCH_1    EQU     0bf81h  ; смещение старшего байта данных ФАПЧ
_ADR_FAPCH_MODE EQU     0bf82h  ; смещение регистра режима ФАПЧ
_ADR_FAPCH_RD0  EQU     0bf80h  ;
_ADR_FAPCH_RD1  EQU     0bf81h  ;

; *** Состояние ФАПЧ ********************
IND_PHA         DB      0       ;Индекс состояния ФАПЧ
INI_PHA         DB      2       ;Признак инициализации ФАПЧ
RR_PHA          DB      0;2       ;Регистр режима ФАПЧ - текущее значение
PCM_CTRL        DB      0;4       ;Содержимое регистра PCM_MODE
UR              DW      0       ;Номер текущего уровня регулировки
VCAP            DW      0       ;Текущее значение кода ЦАП
TIF             DW      30      ;Счетчик цикла ФАПЧ
FI              DD      0       ;Значение регистра фазы
I_SUM           DD      0;8000h   ;Интегральная сумма "дельт"
DFI             DD      0       ;Текущее значение разности фаз
DIA             DD      0       ;Диапазон (кадр) положения фазы


; *** Конфигурация ИКМ ******************
AGFA            DW      5220H,5260H,52A0H,52E0H

IKM_DISPL       DW      498, 21 ; смещение передачи и приема 0 ИКМ
                DW      496, 18 ; смещение передачи и приема 1 ИКМ
                DW      494, 14 ; смещение передачи и приема 2 ИКМ
                DW      492, 10 ; смещение передачи и приема 3 ИКМ

; *** Рабочие переменные ИКМ ************
IKM_ALARM       DW      0       ; индикация аварий ИКМ
FEC             DD      0       ; счетчик FEC
FEC_SUB         DD      0
FEC_PP          DD      0
NUM_PCM_FAPCH   DB      0       ; номер ИКМ - задатчика ФАПЧ
RR_PCM          DB      0       ;Регистр режима ИКМ
C_FEC           DB      127     ;Счетчик цикла расчета FEC
FEC_0           DD  128 DUP(0)          ;Буфер ошибок FEC ИКМ 0
FEC_1           DD  128 DUP(0)          ;Буфер ошибок FEC ИКМ 1
FEC_2           DD  128 DUP(0)          ;Буфер ошибок FEC ИКМ 2
FEC_3           DD  128 DUP(0)          ;Буфер ошибок FEC ИКМ 3
RSR_0           DB      0               ;Буфер чтения регистра RSR ИКМ 0
RSR__0          DB      0
RSR_0_GLB       DB      0
RSR_1           DB      0               ;Буфер чтения регистра RSR ИКМ 1
RSR__1          DB      0
RSR_2           DB      0               ;Буфер чтения регистра RSR ИКМ 2
RSR__2          DB      0
RSR_3           DB      0               ;Буфер чтения регистра RSR ИКМ 3
RSR__3          DB      0
COD_E0          DB      0               ;Коды ошибок E-3, E-5 ИКМ 0
COD_E1          DB      0               ;Коды ошибок E-3, E-5 ИКМ 1
COD_E2          DB      0               ;Коды ошибок E-3, E-5 ИКМ 2
COD_E3          DB      0               ;Коды ошибок E-3, E-5 ИКМ 3


; *** Рабочие переменные первого уровня ****

X_Empty         DB      0               ; Флажок XFIFO пуст - обнаруживается
                                        ; опросом в IRQ_2ms, сбрасывается
                                        ; в IRQ_HSCX
VIU_LEV1        DW      0
;;;;WR1             DB      512 DUP(0)      ; выходной буфер 1 уровня
WR1             DW      0               ; указатель на буфер 2 уровня, готового
                                        ; для передачи в канал
WR1_Len         DW      0               ; длина пакета в WR1
WR1_WR          DW      0               ; передано байт из WR1_Len
RD1             DB      400H DUP(0)     ; входной буфер 1 уровня
RD1_Len         DW      0               ; принято в RD1
BUF_ISTA        DB      0
FG_LEV1         DW      0
CAN_B           EQU     1               ;ПРЕРЫВАНИЕ КАНАЛА В
BUF_START       DB      11111111B       ;ПЕРВЫЙ ПЕРЕДАВАЕМЫЙ ПОКЕТ
INI_PRER            DB  10H,10H,10H,10H
INI_KAN             DB  0
HSCX            DW      40H,0B0H,140H,1B0H
NOM_SLOT1       DB      3FH;73H;    3FH
NOM_SLOT2       DB      3BH;77H;    3BH
SLOT_NOM1       DB      43H;77H;    43H
SLOT_NOM2       DB      3FH;7DH;    3FH
OLD_FILT        DB      0
ON_PCM          DB      0,0,0,0 ;Признак включения ИКМ

PRI_CX          DW      2 DUP(0)
PRI_SI          DW      2 DUP(0)
PRI_CX2         DW      2 DUP(0)
PRI_SI2         DW      2 DUP(0)
PER_CX          DW      2 DUP(0)
PER_SI          DW      2 DUP(0)
PREAMBULA       DB      055H
IF _MSP EQ TRUE
pere            db  0
pere2           db  0
PUST_POKET      DB      1
MAX_DLINNA      DW      100
MIN_DLINNA      DW      2
;ADR_VIU         DB  0
BUF_L1_L1       DB      2 DUP(128 DUP(0))
ADR_L1_V        DW      OFFSET VIV_PUST
                DW      OFFSET VIV_PREAM
                DW      OFFSET VIV_CX
                DW      OFFSET VIV_POKET

ADR_L1_VV       DW      OFFSET VVOD_FLAG
                DW      OFFSET VVOD_FLAG_1
                DW      OFFSET VVOD_POKET

SOST_L1_V       DW      2 DUP(0)
SOST_L1_VV      DW      2 DUP(0)
BUFFFE          DB      0F8H,0F9H,0FAH,0FBH,0FCH,0FDH,0FEH,0FFH,0F0H,0F1H
SLED_BYTE       DW      2 DUP(0)        ;СЛЕДУЮЩИЙ БАЙТ ДЛЯ ПЕРЕДАЧИ
SCH_SLED        DW      2 DUP(0)        ;СЧЕТЧИК ВЫДАННЫХ БИТ С 1 УРОВНЯ
SCH_BITE        DW      2 DUP(0)
SCH_BYT         DW      2 DUP(0)
SCH_BIT         DW      2 DUP(0)
NACH_VVOD       DW      2 DUP(0)
SLED_VV         DW      2 DUP(0)
SLE_BYT         DW      2 DUP(0)
MAX_VVOD        DW      2 DUP(100)
END_POKET       DW      2 DUP(0)
BYTE_BYTE       DW      2 DUP(0)
BUF_CRC         DW      2 DUP(0)
;               ADR_VIU DB  0
ENDIF
;ДАННЫЕ С101
KL_C101         EQU     2
SM_1D00         EQU     1D00H
SM_1F00         EQU     1F00H
SM_1E00         EQU     1E00H

S_C             EQU     1F00H
RRC             EQU     S_C+3AH
TRB             EQU     S_C+20H
ST0             EQU     S_C+22H
ST1             EQU     S_C+23H
ST2             EQU     S_C+24H
ST3             EQU     S_C+25H
CMD             EQU     S_C+2CH
FSTT            EQU     S_C+26H
FG_C101         DW      KL_C101 DUP(0)  ; 1 - наличие пакета 0 - нет

L1_PAKET        DW      KL_C101 DUP(0)
DLIN_L1_PAKET   DW      KL_C101 DUP(0)
KOL_PRIEMA      DW      KL_C101 DUP(0)
SMESH_PRIEMA    DW      KL_C101 DUP(0)
NACH_BUF        DW      KL_C101 DUP(0)
MAX_POKET       EQU     70              ;МАКСИМАЛЬНЫЙ ПАКЕТ
MAX_PAKET       EQU     70              ;МАКСИМАЛЬНЫЙ ПАКЕТ
BUF_PRI         DB      KL_C101 DUP(256 DUP(0))

EQU_5           EQU     5
EQU_6           EQU     6
EQU_8           EQU     8
EQU_FLAG        EQU     7EH
EQU_CRC         EQU     1024H
KL_1            EQU     1

;adr_dop DB      0
BUF_LEV         DB      128  DUP(KL_C101 DUP(0))
GDE_BUF         DW      KL_C101 DUP(0)
TEKUSHEE_NACHALO    DW  KL_C101 DUP(0)
OSTAVSHAYCY_DLINNA  DW  KL_C101 DUP(0)
DLINNA_PRINYTOGO    DW  KL_C101 DUP(0)
TEKUSHEE_KUDA       DW  KL_C101 DUP(0)
SEG_C101        DW      KL_C101 DUP(0)
C101_S          DW      KL_C101 DUP(0)

DL_INI_C101     DW      30,30
SM_INI_C101     DW      00,00
DATA_INI_C101   DB      18H,010H     ;   1
                DB      1CH, 00H     ;   2
                DB      2CH, 21H     ;   3
                DB      2CH, 21H     ;   4
                DB      37H,000H     ;   5      ;C5
                DB      36H,000H     ;   6      ;
                DB      35H, 80H     ;   7      ;20
                DB      2EH, 87H     ;   8      ;87h
                DB      2FH, 00H     ;   9
                DB      30H, 00H     ;  10      ;3-LOOP BACK 0-FULL DUPLEX
                DB      31H, 31H     ;  11
                DB      28H, 00H     ;  12
                DB      29H, 00H     ;  13
                DB      2AH, 00H     ;  14
                DB      2BH, 00H     ;  15
                DB      34H, 7EH     ;  16
                DB      3AH, 00H     ;  17
                DB      38H, 1EH     ;  18
                DB      39H, 1FH     ;  19
                DB      02H, 40H     ;  20
                DB      03H, 80H     ;  21
                DB      04H, 00H     ;  22
                DB      05H, 00H     ;  23
                DB      06H, 00H     ;  24
                DB      08H, 18H     ;  25
                DB      09H, 00H     ;  26
                DB      2CH, 04H     ;  27
;                DB      2CH, 06H     ;  29
;                DB      20H, 11H     ;  28
                DB      2CH, 14H     ;  30
                DB      2CH, 12H     ;  31
                DB      2CH, 02H     ;  32
;ВТОРОЙ БУФЕР ДАННЫХ
                DB      18H,010H     ;   1
                DB      1CH, 00H     ;   2
                DB      2CH, 21H     ;   3
                DB      2CH, 21H     ;   4
                DB      37H,000H     ;   5      ;C5
                DB      36H,000H     ;   6      ;
                DB      35H, 80H     ;   7      ;20
                DB      2EH, 87H     ;   8
                DB      2FH, 00H     ;   9
                DB      30H, 00H     ;  10      ;3-LOOP BACK 0-FULL DUPLEX
                DB      31H, 31H     ;  11
                DB      28H, 00H     ;  12
                DB      29H, 00H     ;  13
                DB      2AH, 00H     ;  14
                DB      2BH, 00H     ;  15
                DB      34H, 7EH     ;  16
                DB      3AH, 00H     ;  17
                DB      38H, 1EH     ;  18
                DB      39H, 1FH     ;  19
                DB      02H, 40H     ;  20
                DB      03H, 80H     ;  21
                DB      04H, 00H     ;  22
                DB      05H, 00H     ;  23
                DB      06H, 00H     ;  24
                DB      08H, 18H     ;  25
                DB      09H, 00H     ;  26
                DB      2CH, 04H     ;  27
;                DB      2CH, 06H     ;  29
;                DB      20H, 11H     ;  28
                DB      2CH, 14H     ;  30
                DB      2CH, 12H     ;  31
                DB      2CH, 02H     ;  32

;**** Рабочие переменные обработчика HDLC на новом железе ****
if _SORM_NO_MODEM eq TRUE
IF _MSP NE TRUE
        TAB_OKNA        DB      00000000b,00000001b,00000010b,00000011b
        TAB_PRIEM       DB      00000001b,00000010b,00000100b,00001000b
        ADR_ST_PRIEM    DW      5059h
        TAB_PERED       DB      00000001b,00000010b,00000100b,00001000b
        ADR_ST_PERED    DW      5057h
ELSE
    if _SORM_MONO ;eq TRUE
        ; безмодемный вариант для моноблока
        TAB_PRIEM       DB      00000001b,00000010b,00000100b,00001000b
        TAB_PERED       DB      00000001b,00000010b,00000100b,00001000b
        TAB_OKNA        DB      00000000b,00000001b,00000010b,00000011b
        ADR_ST_PRIEM    DW      5059h
        ADR_ST_PERED    DW      5057h
    else
        ; специально для ульяновска
        TAB_PRIEM       DB      00010000b,00100000b,01000000b,10000000b
        TAB_PERED       DB      00000001b,00000010b,00000100b,00001000b
        TAB_OKNA        DB      00000001b,00000010b,00000100b,00001000b
        ADR_ST_PRIEM    DW      5026h
        ADR_ST_PERED    DW      5026h
    endif
ENDIF
else
IF _MSP NE TRUE
        TAB_OKNA        DB      00010000b,00010001b,00010000b,00010001b
        TAB_PRIEM       DB      00010000b,00100000b,00010000b,00100000b
        ADR_ST_PRIEM    DW      5054h
        TAB_PERED       DB      00000001b,00000010b,00000001b,00000010b
        ADR_ST_PERED    DW      5054h
ELSE
        ; старый вариант
        TAB_PRIEM       DB      00010000B,00100000B,01000000B,10000000B
        TAB_PERED       DB      00010000B,00100000B,01000000B,10000000B
        TAB_OKNA        DB      00001100B,00001101B,00001110B,00001111B
        ADR_ST_PRIEM    DW      505AH
        ADR_ST_PERED    DW      5058H
ENDIF
endif
IF _MSP EQ TRUE
BUF_PRIEM       DB 1024 DUP(0)
ELSE
BUF_PRIEM       DB 1024 DUP(0abh)
ENDIF
TEKUSHAYA_DLINA DB   4  DUP(0)

