
KL_1            EQU     1       ;���������� �������
; *** ��।������ ���ᮢ ****************
RAM_DAT         EQU     02800h  ;

ADR_RU          EQU     1B00H

COM_1           EQU     03000h  ; ��� ������樨 1
COM_MODE        EQU     03420h  ; ॣ���� ०��� ��������

SUPPLY1         EQU     03441h  ; ॣ���� ��ࢮ�� ���筨�� ��⠭��, RD/WR
;
;               �⥭��
;    �����������������������������������������������Ŀ 1 - ��ଠ
;    �-7.5��+7.5B�-15B �+15B � -5B �-12B �+12B � +5B � 0 - �����
;    �������������������������������������������������
;
;               ������
;    �����������������������������������������������Ŀ 1 - ��ଠ
;    �  X  �  X  �  X  �  X  �  X  �  X  � ��2 � ��1 � 0 - �����
;    �������������������������������������������������
;
SUPPLY2         EQU     03442h  ; ॣ���� ��ண� ���筨�� ��⠭��, RD only


EX_COM          EQU     03800h  ; �������⥫쭮� ��� ������樨


TC_BUFF         EQU     05000h
IPAT            EQU     05200h
IF _MSP EQ TRUE
MO_LEN          EQU     03425h  ; ॣ���� ����� ����� ��
ENDIF
PCM_MODE        EQU     05300h  ; ॣ���� �ࠢ����� ���
PCM_RES_NUM     EQU     05340h  ; ॣ���� ������樨 ����� / ��� ���
PCM_ERR         EQU     05380h  ; ॣ���� ������樨 �訡�� ���
PCM_RESET       EQU     05340h  ; ॣ���� ��� ���
IF _MSP EQ TRUE
MO_MODE         EQU     03424h  ; ॣ���� ०��� ������
MO_LEN          EQU     03425h  ; ॣ���� ����� ����� ��
MO_STAT         EQU     03426h  ; ॣ���� ��ﭨ� ������
ENDIF
MO_CODE         EQU     04200h  ; ��� ����஢�� ������
MO_DECODE       EQU     04300h  ; ��� ������஢�� ������

RFIFO           EQU     05500h  ; ������ ���� ������ � ����஫��� HSCX
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


ADR_RD0_PHA     EQU     0bf80h  ;���� ����襣� ���� ������ ����
                                ; �� ����� ����訥 ࠧ��� ���� ���
                                ; �� �⥭�� ����訥 ࠧ��� ��������� 䠧�
ADR_RD1_PHA     EQU     0bf81h  ;���� ���襣� ���� ������ ����
ADR_RR_PHA      EQU     0bf82h  ;���� ॣ���� ०��� ����
ADR_5300H       EQU     5300H


; *** ���䨣���� ���� *****************
        ; �⠫���
_ET_DGP         equ     350
        ; ��६����
TKI             DD      2048    ;�ॡ㥬�� ���祭�� ॣ���� 䠧�
DGP             DD      _ET_DGP ;����� ���室� �� ����� �����ன��(+)11
MING            DD      0       ;����� ���室� �� ����� �����ன��(-)
TOF             DW      30      ;���� ���� � ���嬨���ᥪ㭤��� ���ࢠ���
;;;DELTA           DD      0
SUM_PHA         DD      0       ;DELTA           DD      0 ��ࠫ쭮 ���५�
FSUM            DD      0       ; ��� ��� ��ਠ�� ����� SUM_PHA
MAX_UR          DW      3       ;���ᨬ���� �஢��� ॣ㫨஢����
VCAPI           DW      8000h   ;��� ��� - ��室��� ���祭��
RR_PHA_I        DB      2       ;������� ०��� ���� - ��室��� ���祭��
C_P             DB      2,3,0,0 ;10,8,6,4;�����樥��� �ய��樠���쭮� ��⠢���饩
C_I             DB      7,0,0,0 ;7,5,3,1 ;�����樥��� ��⥣ࠫ쭮� ��⠢���饩
;C_P             DB      10,8,6,4;�����樥��� �ய��樠���쭮� ��⠢���饩
;C_I             DB      7,5,3,1 ;�����樥��� ��⥣ࠫ쭮� ��⠢���饩

_ADR_FAPCH_0    EQU     0bf80h  ; ᬥ饭�� ����襣� ���� ������ ����
_ADR_FAPCH_1    EQU     0bf81h  ; ᬥ饭�� ���襣� ���� ������ ����
_ADR_FAPCH_MODE EQU     0bf82h  ; ᬥ饭�� ॣ���� ०��� ����
_ADR_FAPCH_RD0  EQU     0bf80h  ;
_ADR_FAPCH_RD1  EQU     0bf81h  ;

; *** ����ﭨ� ���� ********************
IND_PHA         DB      0       ;������ ���ﭨ� ����
INI_PHA         DB      2       ;�ਧ��� ���樠����樨 ����
RR_PHA          DB      0;2       ;������� ०��� ���� - ⥪�饥 ���祭��
PCM_CTRL        DB      0;4       ;����ন��� ॣ���� PCM_MODE
UR              DW      0       ;����� ⥪�饣� �஢�� ॣ㫨஢��
VCAP            DW      0       ;����饥 ���祭�� ���� ���
TIF             DW      30      ;���稪 横�� ����
FI              DD      0       ;���祭�� ॣ���� 䠧�
I_SUM           DD      0;8000h   ;��⥣ࠫ쭠� �㬬� "�����"
DFI             DD      0       ;����饥 ���祭�� ࠧ���� 䠧
DIA             DD      0       ;�������� (����) ��������� 䠧�


; *** ���䨣���� ��� ******************
AGFA            DW      5220H,5260H,52A0H,52E0H

IKM_DISPL       DW      498, 21 ; ᬥ饭�� ��।�� � �ਥ�� 0 ���
                DW      496, 18 ; ᬥ饭�� ��।�� � �ਥ�� 1 ���
                DW      494, 14 ; ᬥ饭�� ��।�� � �ਥ�� 2 ���
                DW      492, 10 ; ᬥ饭�� ��।�� � �ਥ�� 3 ���

; *** ����稥 ��६���� ��� ************
IKM_ALARM       DW      0       ; �������� ���਩ ���
FEC             DD      0       ; ���稪 FEC
FEC_SUB         DD      0
FEC_PP          DD      0
NUM_PCM_FAPCH   DB      0       ; ����� ��� - �����稪� ����
RR_PCM          DB      0       ;������� ०��� ���
C_FEC           DB      127     ;���稪 横�� ���� FEC
FEC_0           DD  128 DUP(0)          ;���� �訡�� FEC ��� 0
FEC_1           DD  128 DUP(0)          ;���� �訡�� FEC ��� 1
FEC_2           DD  128 DUP(0)          ;���� �訡�� FEC ��� 2
FEC_3           DD  128 DUP(0)          ;���� �訡�� FEC ��� 3
RSR_0           DB      0               ;���� �⥭�� ॣ���� RSR ��� 0
RSR__0          DB      0
RSR_0_GLB       DB      0
RSR_1           DB      0               ;���� �⥭�� ॣ���� RSR ��� 1
RSR__1          DB      0
RSR_2           DB      0               ;���� �⥭�� ॣ���� RSR ��� 2
RSR__2          DB      0
RSR_3           DB      0               ;���� �⥭�� ॣ���� RSR ��� 3
RSR__3          DB      0
COD_E0          DB      0               ;���� �訡�� E-3, E-5 ��� 0
COD_E1          DB      0               ;���� �訡�� E-3, E-5 ��� 1
COD_E2          DB      0               ;���� �訡�� E-3, E-5 ��� 2
COD_E3          DB      0               ;���� �訡�� E-3, E-5 ��� 3


; *** ����稥 ��६���� ��ࢮ�� �஢�� ****

X_Empty         DB      0               ; ������ XFIFO ���� - �����㦨������
                                        ; ���ᮬ � IRQ_2ms, ���뢠����
                                        ; � IRQ_HSCX
VIU_LEV1        DW      0
;;;;WR1             DB      512 DUP(0)      ; ��室��� ���� 1 �஢��
WR1             DW      0               ; 㪠��⥫� �� ���� 2 �஢��, ��⮢���
                                        ; ��� ��।�� � �����
WR1_Len         DW      0               ; ����� ����� � WR1
WR1_WR          DW      0               ; ��।��� ���� �� WR1_Len
RD1             DB      400H DUP(0)     ; �室��� ���� 1 �஢��
RD1_Len         DW      0               ; �ਭ�� � RD1
BUF_ISTA        DB      0
FG_LEV1         DW      0
CAN_B           EQU     1               ;���������� ������ �
BUF_START       DB      11111111B       ;������ ������������ �����
INI_PRER            DB  10H,10H,10H,10H
INI_KAN             DB  0
HSCX            DW      40H,0B0H,140H,1B0H
NOM_SLOT1       DB      3FH;73H;    3FH
NOM_SLOT2       DB      3BH;77H;    3BH
SLOT_NOM1       DB      43H;77H;    43H
SLOT_NOM2       DB      3FH;7DH;    3FH
OLD_FILT        DB      0
ON_PCM          DB      0,0,0,0 ;�ਧ��� ����祭�� ���

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
SLED_BYTE       DW      2 DUP(0)        ;��������� ���� ��� ��������
SCH_SLED        DW      2 DUP(0)        ;������� �������� ��� � 1 ������
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
;������ �101
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
FG_C101         DW      KL_C101 DUP(0)  ; 1 - ����稥 ����� 0 - ���

L1_PAKET        DW      KL_C101 DUP(0)
DLIN_L1_PAKET   DW      KL_C101 DUP(0)
KOL_PRIEMA      DW      KL_C101 DUP(0)
SMESH_PRIEMA    DW      KL_C101 DUP(0)
NACH_BUF        DW      KL_C101 DUP(0)
MAX_POKET       EQU     70              ;������������ �����
MAX_PAKET       EQU     70              ;������������ �����
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
;������ ����� ������
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

;**** ����稥 ��६���� ��ࠡ��稪� HDLC �� ����� ������ ****
if _SORM_NO_MODEM eq TRUE
IF _MSP NE TRUE
        TAB_OKNA        DB      00000000b,00000001b,00000010b,00000011b
        TAB_PRIEM       DB      00000001b,00000010b,00000100b,00001000b
        ADR_ST_PRIEM    DW      5059h
        TAB_PERED       DB      00000001b,00000010b,00000100b,00001000b
        ADR_ST_PERED    DW      5057h
ELSE
    if _SORM_MONO ;eq TRUE
        ; ���������� ��ਠ�� ��� ���������
        TAB_PRIEM       DB      00000001b,00000010b,00000100b,00001000b
        TAB_PERED       DB      00000001b,00000010b,00000100b,00001000b
        TAB_OKNA        DB      00000000b,00000001b,00000010b,00000011b
        ADR_ST_PRIEM    DW      5059h
        ADR_ST_PERED    DW      5057h
    else
        ; ᯥ樠�쭮 ��� ��ﭮ�᪠
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
        ; ���� ��ਠ��
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

