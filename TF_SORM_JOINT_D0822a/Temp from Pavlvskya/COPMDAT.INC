; --- ࠡ�稥 ��६���� ���� -----------------------------------------------------------
                ; --- �ᯮ����⥫�� ��ଥ��� ��� ࠡ��� � ���
Oper_Do_Flag    DB      0               ; �ਧ��� �믮������ �������
SORM_KSL_Clear  DW      0ffffh          ; �� ����� @KSL_USE �ॡ�� �᢮������ ��� 0ffffh
SORM_CK         DB      0               ; ��� ����������� � ��
SORM_Work_Num   DW      0               ; ��� ����������� ࠡ�祣� �
; --- <Yu> [14.04.2010] - ����஫� ������᪮� ������樨
SORM_Mod_Nums   DW      0               ; �� ⠩�-᫮⮢ ������, ����襭�� � ��_120

                ; --- �ᯮ����⥫�� ��६���� ��� �ନ஢���� ᮮ�饭��
                ; ��ࠬ���� �裡
                ;        0     1     2     3     4     5     6     7     ��뢠�騩
                ;       ---   ���   ����  ����  ��    ��    ��    ---    ��뢠���
SORM_Link_Prms  DB      000h, 011h, 000h, 000h, 000h, 000h, 000h, 000h  ; 0 - ���ᯮ������
                DB      000h, 01ch, 012h, 012h, 012h, 012h, 000h, 000h  ; 1 - ������� ���
                DB      000h, 011h, 018h, 018h, 018h, 014h, 000h, 000h  ; 2 - ����� �������
                DB      000h, 011h, 018h, 018h, 018h, 014h, 000h, 000h  ; 3 - ������ �������
                DB      000h, 011h, 018h, 018h, 018h, 014h, 000h, 000h  ; 4 - ����㣮த�� ������
                DB      000h, 011h, 018h, 018h, 018h, 014h, 000h, 000h  ; 5 - ����㭠த�� ������
                DB      000h, 011h, 018h, 018h, 018h, 014h, 000h, 000h  ; 6 - ᯥ��㦡�
                DB      000h, 011h, 000h, 000h, 000h, 000h, 000h, 000h  ; 7 - ���ᯮ������

		; --- ��४���஢�� �ਧ���� �⡮� ����� ��� CW, CC, HOLD, HOLE
                ;        0     1     2     3     4     5     6     7
SORM_DVO_Get_Num DB      0,   004h, 008h, 00ch, 001h,  0,    0,    0    ; 0
                DB      002h,  0,    0,    0,   003h,  0,    0,    0    ; 8
                DB      010h,  0,    0,    0,    0,    0,    0,    0    ; 16
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 24
                DB      020h,  0,    0,    0,    0,    0,    0,    0    ; 32
                DB      030h,  0,    0,    0,    0,    0,    0,    0    ; 48
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 56
                DB      192 DUP(0)
                ;
SORM_DVO_Flag 	DW	0	; 䫠�� ��� ��� ��� CW, CC, HOLD, HOLE
SORM_3PTY_CT    DB      0       ; 䫠� ��宦����� ����� �� ��⮪ �ண� ��� 3PTY ��� CT

                ; --- ��४���஢�� �ਧ���� �⡮� ����� ��� ᮮ�饭�� ��� CF
                ;        0     1     2     3     4     5     6     7
SORM_RA_DVO_Get_Num DB   0,   040h, 080h, 0c0h, 001h,  0,    0,    0    ; 0
                DB      002h,  0,    0,    0,   003h,  0,    0,    0    ; 8
                DB      010h,  0,    0,    0,    0,    0,    0,    0    ; 16
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 24
                DB      020h,  0,    0,    0,    0,    0,    0,    0    ; 32
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 40
                DB      030h,  0,    0,    0,    0,    0,    0,    0    ; 48
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 56
                DB      192 DUP(0)

                ; --- ��४���஢�� �ਧ���� �⡮� ����� ��� ᮮ�饭�� ��᫥ ��� CF
                ;        0     1     2     3     4     5     6     7
SORM_RA_Mes_Get_Num DB   0,   001h, 002h, 003h, 040h,  0,    0,    0    ; 0
                DB      080h,  0,    0,    0,   0c0h,  0,    0,    0    ; 8
                DB      010h,  0,    0,    0,    0,    0,    0,    0    ; 16
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 24
                DB      020h,  0,    0,    0,    0,    0,    0,    0    ; 32
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 40
                DB      030h,  0,    0,    0,    0,    0,    0,    0    ; 48
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 56
                DB      192 DUP(0)

                ; --- �㬥��� �맮��� ����
SORM_Call_Num   DW      0
SORM_Curr_Call  DW      0

                ; --- �������, ����뢠��� ����஫��㥬� ��ꥪ�
of_AK_STATE     EQU     1               ; 䫠� ��ﭨ� ��

Obj_Struc       STRUC
  ; --- ����� ����� 5-�� �������
  obj_num       DW      0       ; 00 - 01 - �᫮��� ����� ��ꥪ�
  obj_type      DB      0       ; 02      - ⨯ ��ꥪ� ��� 0FFh
  obj_attr      DB      0       ; 03      - �ਧ��� ����� ⥫�䮭�
  obj_tel_len   DB      0       ; 04      - ������⢮ ������ � ����� ⥫�䮭�
  obj_tel_num   DB    9 DUP(0ffh);05 - 13 - ���� ����� ⥫�䮭�
  obj_bunch     DW      0ffffh  ; 14 - 15 - �᫮��� ����� ��窠
  obj_cat_ctrl  DB      0       ; 16      - ��⥣��� ����஫�
  obj_gr_ksl    DB      0       ; 17      - ����� ��㯯� ���
  obj_prior     DB      0       ; 18      - �ਮ��� ����஫�
  obj_ak_state  DB      0       ; 19      - ���ﭨ� �������᪮�� ��������
  ; --- 䫠��
  obj_flags     DB      0       ; 20      - 䫠� ���ﭨ� ��
  ; --- �ନ஢����
  obj_get_num   DB      0       ; 21      - �ਧ��� �⡮� �����
  ; --- १��
  obj_reserve   DB     10 DUP(0); 22 - 31 - १��
Obj_Struc       ENDS

                ; ����⠭��
_OBJ_SIZE       = size Obj_Struc        ; ࠧ��� ����� � ����஫��㥬�� ��ꥪ�
_OBJ_SHL        EQU     5               ; ⮦� ᠬ�� ��� �⥯��� 2 (������⢮ ᤢ����)


                ; --- �������, ����뢠��� ���� ����
SORM_Struc      STRUC
                ; ---
sorm_attr_a     DB      0               ; 00      - �ਧ��� ����� A
sorm_len_a      DB      0               ; 01      - ������⢮ ��� � ����� �
sorm_num_a      DB      9 DUP(0)        ; 02 - 10 - ����� �
                ; ---
sorm_attr_b     DB      0               ; 11      - �ਧ��� ����� �
sorm_len_b      DB      0               ; 12      - ������⢮ ��� � ����� � �� ����� ������
sorm_num_b      DB      9 DUP(0)        ; 13 - 21 - ᠬ� ���� � ����� � �� ����� ������
                ; ---
sorm_attr_c     DB      0               ; 22      - �ਧ��� ����� C
sorm_len_c      DB      0               ; 23      - ������⢮ ��� � ����� C
sorm_num_c      DB      9 DUP(0)        ; 24 - 32 - ����� C
                ; ---
sorm_ksl_a      DB      0               ; 33      -
sorm_ksl_b      DB      0               ; 34      -
sorm_ck_a       DB      0               ; 35      -
sorm_work_a     DW      0               ; 36 - 37 -
sorm_phys_a     DW      0               ; 38 - 39 -
sorm_ck_b       DB      0               ; 40      -
sorm_work_b     DW      0               ; 41 - 42 -
                ; ---
sorm_call       DW      0               ; 43 - 44 -
sorm_opcode     DB      0               ; 45      -
sorm_get_num    DB      0               ; 46      -
sorm_prior      DB      0               ; 47      -
                ; ---
sorm_obj        DW      0ffffh          ; 48 - 49 - ��뫪� �� ��ꥪ�, ᮧ���訩 ����
sorm_soed       DD      0ffffffffh      ; 50 - 53 - ��뫪� �� ᮥ�������, ᮧ���襥 ����
                ; ---
sorm_flags      DW      0               ; 54 - 55 -
sorm_time       DD      0               ; 56 - 59 - �६� ᮧ����� ��ᨨ
                ; ---
sorm_dvo_code   DB      0               ; 60      -
sorm_dvo_phase  DB      0               ; 61      -
                ; ---
sorm_chnl_a     DB      0               ; 62      -
sorm_chnl_b     DB      0               ; 63      -
                ;
SORM_Struc      ENDS

                ; ����⠭��
_SORM_SIZE      = size SORM_Struc
                ; 䫠��
ssf_NUM_FULL    =       0001h           ;
ssf_ANS         =       0002h           ;
ssf_REL         =       0004h           ;
		;
ssf_SWAP_AB     =       0008h           ;
ssf_SWAP_AC     =       0010h           ;
ssf_SWAP_BA     =       0020h           ;
ssf_SWAP_BC     =       8000h;;;0040h           ;
		;
ssf_HOLD        =       0080h           ;
ssf_HOLE        =       0100h           ;
ssf_CW          =       0200h           ;
ssf_CC          =       0400h           ;
ssf_RA          =       0800h           ;
ssf_3PTY        =       1000h           ;
ssf_CT          =       2000h
ssf_DVO         =       ssf_HOLD + ssf_HOLE + ssf_CW + ssf_RA + ssf_3PTY + ssf_CT ;
ssf_TCOP_UPATS  =       4000h           ;
ssf_CMND_8      =       0040h;;;8000h           ;
		;

                ; --- ࠡ�� � ��窠�� --------------------------------------------------
                ; ����⠭��
IF _MSP EQ TRUE
_BUNCH_MAX      EQU     25                 ; max ������⢮ ��ꥪ⮢ - ��窮�
        ELSE
_BUNCH_MAX      EQU     100                ; max ������⢮ ��ꥪ⮢ - ��窮�
ENDIF
_BUNCH_SIZE     EQU     _OBJ_SIZE
_BUNCH_SHL      EQU     _OBJ_SHL
                ; ����⠭��, ����뢠�騥 ࠧ��饭�� ���� � 䠩�� � ���ᠭ��� ��窮�
_BUNCH_PRM_OFFS  EQU    100000h         ; ᬥ饭�� ��ࠬ��஢ ��窮�
                                        ;       ����  0   - �᫨ 0, ��箪 ����।���
                                        ;       ����� 1-2 - �᫮��� ����� ��窠
                                        ;       ����  3   - १�� 0
_BUNCH_PRM_SIZE  EQU    4               ; ��ࠬ���� �������� 4 ����
_BUNCH_PRM_SHL   EQU    2               ; � �� ᠬ�� � ᤢ����
_BUNCH_NAME_OFFS EQU    _BUNCH_PRM_OFFS + _BUNCH_MAX * 4 ; ᬥ饭�� ��� ��窮�
_BUNCH_NAME_SIZE EQU    43              ; ॠ��� ࠧ��� ����� � �����

                ; --- ࠡ�� � ��ꥪ⠬� (⥫�䮭���) -----------------------------------
                ; ��६����
Obj_Count       DW      0               ; ������⢮ ⥫�䮭��� ����஢ �� ����஫�
                ; ����⠭��
_OBJ_MAX        EQU     1536            ; max ������⢮ ��ꥪ⮢ - ⥫�䮭��

                ; --- ࠡ�� � ���ﬨ -------------------------------------------------
                ; ����⠭��
_SESSION_MAX    EQU     1023;;;4            ; max ������⢮ ��ᨩ

                ; --- ࠡ�� � ��� ------------------------------------------------------
_MAX_E1         EQU     1               ; max ������⢮ ��⮪�� �1 � ��஭� ���
_MAX_KSL        EQU     _MAX_E1 * 28    ; max ������⢮ ��� �� ��� �1
_MAX_GROUP      EQU     _MAX_KSL        ; max ������⢮ ��㯯 � ����
IF _MSP EQ TRUE
_MAX_MODEM_TS   EQU     _MAX_KSL / 2    ; max ������⢮ ��
        else
_MAX_MODEM_TS   EQU     _MAX_KSL        ; max ������⢮ ��
ENDIF
                ; ������᪠� ��������
_MODEM_1ST_TS   EQU     60              ; ����� ��砫쭮�� ⠩�-᫮� �� ������
IF _MSP EQ TRUE
_COMM_CHNLS     EQU     14              ; ᪮�쪮 ������� �������㥬 � ������
_COMM_AB_SHIFT  EQU     16              ; ᬥ饭�� ������樨 ��� ������� � � �
_COMM_SUM_SHIFT EQU     32              ; ᬥ饭�� ������樨 ��� �㬨஢����� ������� � � �

                ; ���譠� ��������
_COMM_TO_CTO    EQU     15              ;
_COMM_FROM_CTO  EQU     14              ;
ELSE
;;;<mmm>
_COMM_1ST_BABA  EQU     16              ; ᪮�쪮 ������� �������㥬 � ������
_COMM_CHNLS     EQU     _MAX_MODEM_TS   ; ᪮�쪮 ������� �������㥬 � ������
_COMM_AB_SHIFT  EQU     _MAX_MODEM_TS   ; ᬥ饭�� ������樨 ��� ������� � � �
_COMM_SUM_SHIFT EQU     _MAX_MODEM_TS*2 ; ᬥ饭�� ������樨 ��� �㬨஢����� ������� � � �
ENDIF
                ; ���ᨢ�, �������㥬� �� � ��㯯�
KSL_Group       DB      _MAX_GROUP DUP(0ffh)    ; ff - ��㯯� ���
KSL_Group_Type  DB      _MAX_GROUP DUP(0ffh)    ; ff ��� ⨯ ��㯯� ���
KSL_Last        DB      _MAX_GROUP DUP(0ffh)    ; � ��� � ��㯯� ��࠭��� � �।��騩 ࠧ
KSL_Count       DB      _MAX_GROUP DUP(0ffh)    ; ������⢮ ��� ���९���� �� ��㯯��
                ; ���ᨢ�, �������㥬� �� � ���
KSLs            DB      256 DUP(0ffh)           ; ff ��� � ��㯯�
KSL_A           DB      256 DUP(0ffh)           ; ff ��� 0 �᫨ �� ��� �
KSL_Pair        DB      256 DUP(0ffh)           ; ff ��� � ��� ��୮� ⥪�饩 ��� �� ���ᨢ� KSL
KSL_Busy        DB      256 DUP(0ffh)           ; ff ��� �᫨ ��� ����� - �ਮ��� �� ������
KSL_Session     DW      256 DUP(0ffffh)         ; ffff - �᫨ ��� ᢮����� ��� �. �� ����

Modem_KSL       DB      _MAX_MODEM_TS DUP(0ffh) ; ff ⠩�-᫮� �� ������ �� ����� ��� ����� ���
_KSL_DATA_SIZE = $ - KSL_Group

                ; 䫠�� �����⨬�� ���
KSL_Enabled     DB      0, 28 DUP(01), 3 DUP(0)
                DB      223 DUP(0), 01

                ; --- ��ࠡ�⪠ 10-�� ������� -------------------------------------------
                ; ��騥 ��६����
Obj_Data_Run    DB      0               ; 䫠� - ���� �뤠� ������
Obj_Data_Time   DD      0               ; �६� ��᫥���� ��।��
_OBJ_DATA_TOUT  =       250             ; ⠩�-��� ��।��
                ; ��ꥪ�� - �������� � ��᫠���
Obj_Data_Count  DW      0               ; �᫮ ��������� ��ꥪ⮢
Obj_Data_Send   DW      0               ; �᫮ ��ࠢ������ ��ꥪ⮢
Obj_Data_Buf    DW      _BUNCH_MAX + _OBJ_MAX DUP(0) ; ���� � १���⠬� ���᪠ - 㪠��⥫� �� ��ꥪ��
                ; ���� ��� ���� ᮮ�饭�� N 3
Obj_Data_Out_Buf DB     10 + 20 DUP(0)  ;
Obj_Data_Out_Len DW     0               ; ���� � ��।��饬 ����

                ; --- ��ࠡ�⪠ 11-�� ������� -------------------------------------------
KSL_Conform_Count DW    0               ; ����稪 �⮡࠭��� ���
KSL_Conform_Send  DW    0               ; ����稪 ��� �� ����� ��ࠢ���� ���ଠ��
KSL_Conform_Buff  DD    _MAX_KSL DUP(0) ; ���� ��� �⮡࠭��� ���

                ; --- ��ࠡ�⪠ 12-�� ������� -------------------------------------------
_DVO_BUFF_SIZE  =       33              ; ࠧ��� ���� ���
DVO_Buff        DB      _DVO_BUFF_SIZE DUP(0) ; ᠬ ���� - ������� ⥫� 12-�� ������� �� 45 ����

                ; --- ��ࠡ�⪠ 16-�� ������� -------------------------------------------
                ; ��騥 ��६����
Bunch_Data_Run  DB      0               ; 䫠� - ���� �뤠� ������
Bunch_Data_Time DD      0               ; �६� ��᫥���� ��।��
_BUNCH_DATA_TOUT =      250             ; ⠩�-��� ��।��
                ; ��ꥪ�� - �������� � ��᫠���
Bunch_Data_Count   DW   0               ; �᫮ ��������� ��ꥪ⮢
Bunch_Data_Send    DW   0               ; �᫮ ��ࠢ������ ��ꥪ⮢
Bunch_Data_Buf     DD   _BUNCH_MAX DUP(0) ; ���� � १���⠬� ���᪠ - 㪠��⥫� �� ��ꥭ���
                ; ���� ��� ���� ᮮ�饭�� N 3
Bunch_Data_Out_Buf DB   10 + 43 DUP(0)  ;
Bunch_Data_Out_Len DW   0               ; ���� � ��।��饬 ����

                ; --- ࠡ�� � ��஫� ���� --------------------------
OLD_PAROL_0_3   DD      31313131H       ;�������������� ������ 0-3
OLD_PAROL_4_5   DW      3131H           ;4-5
PAROL_0_3       DD      1               ;������� ������ 0-3
PAROL_4_5       DW      1               ;4-5

                ; --- ࠡ�� � ���஬ 1-�� ������
POS_SORM        DW      0               ;����� ��������� ���������� � ����� SORM_COM
PER_SORM        DW      0               ;����� ������� ��� ������
M_COM           EQU     1023            ;������ ���-�� ������

                ; --- ࠡ�� � ���஬ 2-�� ������
SLED_SOOB       DW      0               ;��������� ��������� � ���������
POS_SOOB        DW      0               ;������� ����� ��� �������
M_SOOB          EQU     1023

                ; --- �㬥��� ᮥ������� ��� ���
NOM_VIZOVA      DW      0       ;����� ���������� ������
TEKUSH_VIZ      DW      0       ;������� �����

                ; --- ����� ���� ��� ���
BUF_PO          LABEL BYTE
                DB      'ALS SORM, version 6.01'
                DB      50 DUP(20h)

                ; --- �ନ஢���� ��⮢�� ᮮ�饭��
FG_TEST         DB      2 DUP(0) ; ��� �ନ஢���� �⢥� �� ����� ���
BUF_TEST1       DB      0BH,0,29H,3,1,0,1,0,0FFH,0FFH,9,01,01
BUF_TEST2       DB      15,0,0CCH,1,53H,3,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,1,1,1
KPD1_2          DW      101H            ;�������� ���

                ; ���� ���室�� �� ���� ������
KOMANDI         DW      17      DUP(0)  ;�������� ������
                ; ���� ���室�� �� ��ࠢ�� �⢥⮢ �� �������
ADR_SORM_C      DW      17      DUP(0)  ;������ ������ ������

                ; ????????????????????????????????????????
FG_INIC         DW      0
TIME_PEREZ      DW      0       ; cickl.inc -> mtp_go

                ; �ଠ�� ����
PREAMB_NOM_SORM DW      01CCH
DLIN_ZAGOLOVKA  EQU     10              ; LEV_L4 - ᬥ饭�� 㪠��⥫� �� ��ࠬ���� ����
                ; ������⢮ ��ࠬ��஢ � ����� � ������ ����
                ;           1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17
COM_LEN         DB      0,  0,  0,  6,  4,  19, 16, 6,  7,  3,  16, 4,  11, 0,  1,  5,  2,  0

; *********** ��������� ****************************
                ; ���� ������
COM_ZAP         EQU     0001B    ; 1 - �����
COM_OST         EQU     0010B    ; 2 - ��⠭����
COM_PAR         EQU     0011B    ; 3 -
COM_ZAK         EQU     0100B    ; 4 -
COM_POS         EQU     0101B    ; 5 -
COM_SNY         EQU     0110B    ; 6 -
COM_PDK         EQU     0111B    ; 7 -
COM_OSV         EQU     1000B    ; 8 - �᢮��������
COM_ISK         EQU     1001B    ; 9 -
COM_ZOB         EQU     1010B    ;10 -
COM_ZKS         EQU     1011B    ;11 -
COM_ZDV         EQU     1100B    ;12 -
COM_PRE         EQU     1101B    ;13 -
COM_TES_        EQU     1110B    ;14 -
COM_IZM         EQU     1111B    ;15 -
COM_ZUS         EQU    10000B    ;16 -
COM_ZPO         EQU    10001B    ;17 -
                ; ���� �⢥⮢ �� �������
COM_AVA         EQU     21H
COM_PER         EQU     22H
COM_OBJ         EQU     23H
COM_SOO         EQU     24H
COM_DVO         EQU     25H
COM_NES         EQU     26H
COM_POD         EQU     27H
COM_VIP         EQU     28H
COM_TES         EQU     29H
COM_DSO         EQU     2AH
COM_VPO         EQU     2BH
                ; ���� ������᪨� ᮮ�饭��
SO_NOM          EQU     41H
SO_OTV          EQU     42H
SO_RAZ          EQU     43H
SO_DVO          EQU     44H
SO_POD          EQU     51H
SO_OSV          EQU     52H
SO_TES          EQU     53H

                ; ��⥣��� ����஫�
SOVMEST         EQU     1               ;����������� ����������
RAZDEL          EQU     11H             ;���������� ����������
STATIC          EQU     2               ;����������� ����������
                ; �ਧ���� ��� / �믮������ �������
IND_GUD         EQU     0
IND_BAD         EQU     1
                ; ��� �᢮�������� ���
OSV_KOM8        EQU     1
OSV_PRI         EQU     2
OSV_KOM7        EQU     3
OSV_BAD         EQU     4
OSV_KOM6        EQU     5
                ; ��ࠬ���� �裡
UNDEFINED_SV    EQU     10000b
ISXOD_VIZ       EQU     10001B
VXOD_VIZ        EQU     10010B
; --- NOT USED!!!
MESHDUN         EQU     10100B  ; 100100B \ ��� ��⮬�⨪�,
MESHGOR         EQU     11000B  ; 101000B / � �� ���� !
; ---
VNUT_SV         EQU     11100B
                ; �ਧ��� ����� ⥫�䮭�
PRIZN_STAN      EQU     1
PRIZN_SETI      EQU     2
PRIZN_ZONI      EQU     3
PRIZN_MESHGOR   EQU     4
PRIZN_MESHDUN   EQU     5
PRIZN_SLUSHB    EQU     6
PRIZN_UNDEF	EQU	0ffh
                ; ⨯ ��ꥪ� �������
AB_STANC        EQU     1               ;������� �������
AB_SET          EQU     2               ;������� ����
NEPOLN_NOM      EQU     12H             ;�������� �����
BUNCH           EQU     3               ;��� - ����� �����
                ; ��⪠ �ਮ���
PRIOR_HI        EQU     1
PRIOR_LO        EQU     2
                ; ��� �����襭�� ᮥ�������
PRI_TEX         EQU     1               ; �� �孨�᪨� ��稭��
PRI_NENAB       EQU     2               ; ������� ����� (�� ��� ����)
PRI_ZAN         EQU     3               ; ��뢠��� �����
PRI_NEOT        EQU     4               ; ��뢠��� �� �⢥砥�
PRI_RAZ         EQU     5               ; ��ଠ�쭮� ࠧꥤ������ (��᫥ ࠧ�����)
                ; ��� ��������祭�� ���
NEPODKL_OK      EQU     0
NEPODKL_ZAN     EQU     1
NEPODKL_STAT    EQU     0ffh
NEPODKL_MO      EQU     2               ; ��� �ਤ㬪� - �� ��ࠢ�� ��120
NEPODKL_MODEM   EQU     3               ; ��� �ਤ㬪� - �� ᢮������ ⠩�-᫮⮢ �� ������, ��� ��� ����
                ; ��� 䠧� ���
PHASE_CALL      EQU     0
PHASE_SET_DVO   EQU     1
PHASE_TEST_DVO  EQU     2
PHASE_RESET_DVO EQU     3
PHASE_USE_DVO   EQU     4

                ; ���� ��� ��
MO_HT           EQU     35h             ; ����稩 ⥫�䮭
MO_OA           EQU     37h             ; �������� �㤨�쭨�
MO_MA           EQU     38h             ; ��������� �㤨�쭨�
;MO_CFU          EQU     ??h             ; ����� ������� 7b ���� ������� b6
MO_CFB          EQU     62h             ; �/� �� �������
MO_CFNRY        EQU     63h             ; �/� �� ���⢥�
MO_HOLE         EQU     64h             ; �����襭�� 㤥ঠ��� �맮��
MO_HOLD         EQU     65h             ; 㤥ঠ��� �맮��
MO_CC           EQU     66h             ; ��������� �ࠢ�� �� �६� ࠧ�����
MO_3PTY         EQU     67h             ; ������஭��� ����७���
MO_CT           EQU     68h             ; ��।�� �맮��
MO_CW           EQU     69h             ; 㢥�������� � ����㯫���� ������ �맮��
; ---------------------------------------------------------------------------------

                ; ���� ��� ����
CF              EQU     20h             ; ���� �/�
CFU             EQU     21h             ; ����᫮���� �/�
CFB             EQU     29h             ; �/� �� �������
CFNRY           EQU     2ah             ; �/� �� ���⢥�
CT              EQU     31h             ; ��।�� �맮��
CC              EQU     33h             ; ��������� �ࠢ�� �� �६� ࠧ�����
CW              EQU     41h             ; 㢥�������� � ����㯫���� ������ �맮��
HOLD            EQU     42h             ; 㤥ঠ��� �맮��
HOLE            EQU     40h             ; �����襭�� 㤥ঠ��� �맮��
_3PTY           EQU     52h             ; ������஭��� ����७���
                ; --- ���� ���⠭����� ���� ���
HT              EQU     MO_HT           ; ����稩 ⥫�䮭
OA              EQU     MO_OA           ; �������� �㤨�쭨�
MA              EQU     MO_MA           ; ��������� �㤨�쭨�

; --- 䫠�� ��� ��� ���䨣��樨 - ��� 0 => �����-� ��� ����
CFU_CFG         EQU     0feh            ; CFU
CFB_CFG         EQU     0fdh            ; CFB
CFNRY_CFG       EQU     0fbh            ; CFNRY
HT_CFG          EQU     0f7h            ; ����稩 ⥫�䮭
OA_CFG          EQU     0efh            ; �������� �㤨�쭨�
MA_CFG          EQU     0dfh            ; ��������� �㤨�쭨�

; --- ��ꥤ���� ���� ��� ��� ���� (��. ����) � ���䨣��樨 (��. ����)
_CFU_           EQU     CFU   * 256 + CFU_CFG  ; CFU
_CFB_           EQU     CFB   * 256 + CFB_CFG  ; CFB
_CFNRY_         EQU     CFNRY * 256 + CFNRY_CFG; CFNRY
_HT_            EQU     HT    * 256 + HT_CFG   ; ����稩 ⥫�䮭
_OA_            EQU     OA    * 256 + OA_CFG   ; �������� �㤨�쭨�
_MA_            EQU     MA    * 256 + MA_CFG   ; ��������� �㤨�쭨�


IF _MSP EQ TRUE
BUF_R_BABA      DB      224,0,240,0,225,0,241,0,226,0,242,0,227,0,243,0,228,0,244,0,229,0
                DB      245,0,230,0,246,0,231,0,247,0,232,0,248,0,233,0,249,0
                DB      234,0,250,0,235,0,251,0,236,0,252,0,237,0,253,0
ENDIF
ADD_4_KAN       EQU     16
ADD_31_KAN      DW      124             ;<- {Paul.N - 17/10/17 - add}
