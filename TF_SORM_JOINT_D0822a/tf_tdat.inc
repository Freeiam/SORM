_Cfg_Start      LABEL BYTE
    ;**************************************
    ; �᫮ ����筨��� � ���
KL_TIS          EQU     64
    ; ��砫� ��࠭塞�� ���䨣��樨 �� 䫥�


    ; 16 * ������⢮ ����ﭨ���, ����� ���� ᮮ⢥����� ������ ������
TIS_ROAD        DB      16 * KL_TIS DUP(0FFh)

    ; ����� ����� � �⠭樨
LENGTH_NUM      DB      6, 0

    ; 䫠�� ����஥� �ணࠬ��
FG_PROGRAM      DW      0

_fg_prg_debug           EQU     _b_00   ; �⫠��� - �뢮� __ DD xx
_fg_prg_sorm_alarm      EQU     _b_01   ; �뤠���� ���ਨ ����
_fg_prg_ck0             EQU     _b_02   ; ࠡ�� � ��0
_fg_prg_in_bunch        EQU     _b_03   ; ࠡ�� � ��室�騬� ��窠��
_fg_prg_cats            EQU     _b_04   ; �������஢����� ���
_fg_prg_mi              EQU     _b_05   ; �⫠��� �室�饣� ��
_fg_prg_mo              EQU     _b_06   ; �⫠��� ��室�饣� ��
_fg_prg_s1              EQU     _b_07   ; �⫠��� ������ 1 ����
_fg_prg_s2              EQU     _b_08   ; �⫠��� ������ 2 ����
_fg_prg_multy_pref      EQU     _b_09   ; ࠡ�� � ���⨯�䨪᭮� ���䨣��樥�
_fg_prg_rel_no_aon      EQU     _b_10   ; �⡮� ᮥ������� ��� ���
_fg_prg_bl_new          EQU     _b_11   ; ���� ����஢���� ���� ᯨ᮪
_fg_prg_bl_out          EQU     _b_12   ; ��室�騩 ���� ᯨ᮪
_fg_prg_bl_in           EQU     _b_13   ; �室�騩 ���� ᯨ᮪
_fg_prg_check_sorm      EQU     _b_14   ; �஢�ઠ ����ᠭ�� ������� ����
_fg_prg_log_sorm        EQU     _b_15   ; ����஢���� ����

; --- �࠭�� ���ᠭ� ��窮�
;
Tranks          Trank       _MAX_TRANK DUP(<>)
; --- ��䨪�� ��室� �� ᯥ歠�ࠢ�����:  ��, �� � ��
;;;IF _MSP EQ TRUE
Prefixes        DW      _PREF_MAX DUP(0ffffh)
; --- ᯨ᮪ ����஢ ᯥ��㦡 �ᯮ�������� �� ���
SS_Nums         DW      _SS_MAX   DUP(0ffffh)
; --- ��� ��࠭���� ����室���� 䫠��� �롮�筮�� ��᫥������� ������ ��
FG_Curr_MO_Conf DW              0                       ; 䫠�� ���� �롮� ������ ��
; --- 䫠�� �롮� ������ �� ---
_spc_press         EQU   0000000000000001b              ; ����� �஡��
_page_down         EQU   0000000000000010b              ; ����� page_down
_page_up           EQU   0000000000000100b              ; ����� page_up
_arrows            EQU   0000000000001000b              ; ����� �����-� ��५��: , , , 
_235_btn           EQU   0000000000010000b              ; ����� �롮� 235-��
_235_cmd           EQU   0000000000100000b              ; ࠡ�⠥� �� 235-��
_find_cmd          EQU   0000000001000000b              ; ��� �뢮�� ����祭��� ������
_find_numb         EQU   0000000010000000b              ; ��諨 �⬥祭��� �������
_clear_MO          EQU   0000000100000000b              ; ���� �� ������� � ����஫� ������� ⨯�
_set_all_MO        EQU   0000001000000000b              ; ���⠢��� �� ������� �� ����஫�
_reset_all_MO      EQU   0000010000000000b              ; ���� �� ������� � ����஫�
_set_sorm_MO       EQU   0000100000000000b              ; ���⠢��� ������� ���� �� ����஫�
_set_black_MO      EQU   0001000000000000b              ; ���⠢��� ������� "�୮��" ᯨ᪠ �� ����஫�
_set_tarif_MO      EQU   0010000000000000b              ; ���⠢��� ��䨪�樮��� ������� �� ����஫�
IF _MSP NE TRUE
    ; ��६���� ��� �८�ࠧ������ ⨯�� ࠧ����஢ �� � ⨯� ����
                ; --- �� ��䨪�� �� 1-� �� 3-� ���
__PREFIX_MG       DW      080h            ; ��䨪� ��室� �� ����
__PREFIX_ZN       DW      082h            ; ����ਧ����� �� ��䨪�
__PREFIX_MN       DW      00081h          ; ��䨪� ��� ��室� �� ��
__PREFIX_ATS      DW      02081h          ; ᮡ�⢥��� ��䨪� ���
                ; --- ��᪨ ��� �뤥����� ��䨪ᮢ
__PREFIX_MG_MASK  DW      0f0h            ; ��᪠ ��� �뤥����� ��䨪� �� �� � �
__PREFIX_ZN_MASK  DW      00fffh          ; ��᪠ ��� �뤥����� ����ਧ������� �� ��䨪� �� � �
__PREFIX_MN_MASK  DW      00fffh          ; ��᪠ ��� �뤥����� ��䨪� �� �� � �
__PREFIX_ATS_MASK DW      00fffh          ; ��᪠ ��� �뤥����� ᮡ�⢥����� ��䨪� �� � �

__PREFIX_SS       DB      000h            ; ��䨪� ᯥ��㦡�
__PREFIX_SS_MASK  DB      0f0h            ; � ��� ��᪠
_Cfg_End	= $ - _Cfg_Start
; �������
                   DB      199 DUP(0)

;;;KONF_TAIL       DB      _FILE_CFG_SIZE - _Cfg_End DUP(0cch) - �� �ᯮ�짮������--{Paul}--
ELSE
; �������
                   DB      217 DUP(0)
    ; ����� ��࠭塞�� ���䨣��樨
    ;**************************************
END_KONF_FLASH     DW      0                       ; ��⪠ ���� ���䨣��樨 ��� ����� �� FLASH
ENDIF
; --- ��� ������ ������⢠ ᥪ�஢ ���䨣��樨 ---
LCO                DB      0                    ; �� �ᯮ������

                ; --- ���
CHAN_TO_CTO     DW      15      ;����� ��� ���� ��।�� �� ���
CHAN_FROM_CTO   DW      14      ;����� ��� ���� �ਥ�� �� ���
IF _MSP EQ TRUE
FiltrRM         DB      040h    ; Watch Dog ��� ��ண� ������ + ��⨥ "��1" <-{Paul.N - 21/04/14}
ENDIF
                ; ���� ���䨣���� - ��� ���७�� � ������� �� ��䨪�
;        LENGTH_NUM      0   1   2   3   4  5           6           7
ROL_PREF_TIS    DB      32, 28, 24, 20, 16, 12,         8,          4
M_OR_PREF_TIS   DD      -1, -1, -1, -1, -1, 0FFFFFFF0H, 0FFFFFF00H, 0FFFFF000H
IF _MSP EQ TRUE
T_F_D           DB      55h
PERV_SEG_KONF   EQU     14H

TIP_SL          EQU     1
TIP_AB          EQU     2
TIP_AB_UP       EQU     3

TAB_TIP_LIN     DB      0, 2, 2, 2, 4, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 4, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0 ;
TAB_TIP_TF_AB   DB      0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 2, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0 ;
TAB_TIP_TF_SL   DB      0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1 ;
ENDIF
                ;       ���   ��   �30  �� �� �15  PRI  ���  ��   ��   ������
TAB_DL_BL       DW      200h, 80h, 80h, 0, 0, 80h, 80h, 80h, 80h, 80h, 6 DUP(0)
TAB_TIP_BL      DB      2,    1,   1,   0, 0, 1,   1,   1,   1,   1,   6 DUP(0)
TAB_TIP_LN      DB      6,    4,   4,   0, 0, 4,   4,   4,   4,   4,   6 DUP(0)

KL_LEV          EQU     5               ; ��㡨�� ������筨���
IF _MSP EQ TRUE
SCH_FAPCH       DW      0

LAST_ADR        DW      OFFSET END_KONF_FLASH           ;��������� ����� ���䨣��樨
NOM_CONF        DB      0


DLIN_KONFIG     DD      11H
ENDIF
DLIN_KONF       EQU     0150000H
SMESH_KONF      EQU     0A000H

; ����
FG_SOST         DB      0               ;���� ���������
FG_SOST_STOP    DB      0               ; <Yu> - 䫠� ��⠭���� ����
IF _MSP EQ TRUE
FG_SORM         DW      100000B         ;���� ����
                        ;0-����� ����������
                        ;1-�������� ������������
                        ;2-���� ������ ����� ��� �������
                        ;3-���� ��� ��������� ��������
                        ;4-���� �� ������� �����
                        ;5-���� ������ ����.���������
                        ;6-���� ������ �� ������������ OBJ
                        ;7-���� ������������ ���
                        ;8-���� ������ ����������
                        ;12-���� �������� ���������� ������ �� �����������
                        ;13-��������� ������� ������ ������� �����
                        ;14-������ ������ ������
                        ;15-������ ����� ������ ��������

FG_SORM2        DW      0
                        ;0-���
                        ;1-�������� �������
                        ;2-�������� ����� ���
                        ;3-���� ��� ����������� - �� ����稫��� ������ � SEG_TF
                        ;4-������ ������������ �� �����������
                        ;5-�� ������ �������
ENDIF
FG_SORM3        DW      0       ;0-���������� �������
				;1-�������
; --- <Paul> --- {13/02/2013}
; --> �������⥫��(�㦥���) 䫠�� ����
FG_SORM4        DB 0
  _3_pty_sorm   EQU     00000001b      ; ��� 䨪�樨 3PTY � ᮥ������� B ->C(���� B(*)->C)
; --- ������� ��� ࠡ��� � ���ﬨ ���� - 3PTY
; -- �������筮 ���������� ��ᨨ� ����, � ���ன ����室��� ��४������� <Recomm_3_PTY>
Sorm_3PTY       STRUC
  session_ptr         DW       0FFFFH        ;  �-�� �� ����
  perem_ebx_3pty      DD       0FFFFFFFFH    ;  �-�� �� 2-�� ᮥ������� ����७樨
  sorm_numb_3pty      DD       0FFFFFFFFH    ;  � ������� �� ����஫�, �믮����襣� ����७��
  perem_tms           DB       0FFH          ;  ����� ⠩�᫮� �� ������ �� 2-� ᮥ�. ��� ��४���.
  soed_ptr            DD       0FFFFFFFFh    ;  �-�� �� 2-� soed., ��� �࠭���� ��뫪� �� ����.
Sorm_3PTY       ENDS
; --- ᫥���騩 ᢮����� ᫮�
_sorm_3pty_next       DW      0
; --- ࠧ��� ��������
_sorm_3pty_size = size Sorm_3PTY
; -- ���-�� �������
_sorm_3pty_max  = 100
; -- ����� �������
_sorm_3pty_last = _sorm_3pty_size * _sorm_3pty_max
; -- ᮧ����� 100 �������
Sess_3PTY       Sorm_3PTY      _sorm_3pty_max DUP(<>)
; --- ��६����� ��� �࠭���� ����� ⠩�᫮�.
_tmslot         DB      0
; --- <Paul> --- {END}
SLED_TF         DD      0
POS_TF          DD      0
M_TF            EQU     1FFFH
IF _MSP EQ TRUE
Dlina_Packet    DB      0
Ptr_edi         DD      0

SCH_AVA1        DW      0               ;���-�� ����������� �������
DL_OKNA         EQU     64
NOM_POKETA      DB      0
OLD_PODTVER     DB      7FH
NOM_VIDAN       DB      0
M_NOM_POK       EQU     7FH
SCH_ERR_TF      DW      0
OLD_TIM_TIME    DD      0
POROG_5MIN      EQU     300000
tek_vvod_tf     db      7FH
                ;                                       FEDCBA9876543210
;0 - TIME - NET_PODTVERSHDENIY  ����������������������������������������
;1 - TIME - �������� �� ��������� �����  ������������������������������
;2 - INIC_TF - ����� ��������� ���������������� �����  ���������������
;3 - ��� ��� �����  �������������������������������������������������
;4 - ������ �� ����������   ����������������������������������������
;5 - ��� ����� � ���� - ���������� ��������������������������������
;6 - ��������� NOM_POKETA  ���������������������������������������
;7 - ��� ����� � ����  ������������������������������������������
;8 - ������ ������� �����������   ������������������������������
;9 - �������� ������������    ���������������������������������
;10 - ����� ������ ��� ���������������������������������������
;11 - ��� ���� ����������������������������������������������
;12 - ������ ������ � �������������� �����������������������
;14 - ������ ����������� ������������� �������������������

; --- ��ࠬ���� -=> ���1 <=- ���� ��� ����� !!!
N_PORT          DW      3F8H    ; PortCOM1
DIV_SPEED       DB      1       ; ����⥫� �����
COM_TX          DW      3F8H    ; ���� ���-����
COM_TX2         DW      3FAh    ; ���� ���-���� + 2
COM_TX5         DW      3FDh    ; ���� ���-���� + 5
COM_TX6         DW      3FEh    ; ���� ���-���� + 6

; ��䨪���
PREDEL_TF       EQU     1000H
OLD_TIME_TF0    DW      0
OLD_TIME_TF1    DW      0
OLD_TIME_TF4    DW      0
POROG_TF        EQU     1000
POROG_TF1       EQU     100
POROG_TF4       EQU     5000
NACH_TF_POKETA  DD      0
END_TF_POKETA   DW      0

SCH_PEREZAP     DW      0
SCH_INI_TF      DB      0
INI_PAKET       DB      40 DUP(0)
NOM_INI_P       DB      254,7FH
                DB      50 DUP(0)
DL_INI_PAK      EQU     50
KONFIG          EQU     128
SCH_TIME        DW      0

; ************************************ ���������� ������������ ��� ������ � ���
SCREEN_SEG      DW      0B800h  ;������� �����������
POINT_SS        DW      0       ;�����⥫� ����� � ��� ����
ENDIF
FG_TF           DW                                      0000000000000000b
INT_5           DB      0       ;���� ��������୮�� ���뢠���
IF _MSP EQ TRUE
KLAV            DB      0       ;����-��� ����⮩ ������
ENDIF
ALA_MO          DB      0       ;�ਧ��� ���ਨ �� �� NO_MOD = 0 ( ���� 0 )
                DB      2       ;�ਧ��� ���ਨ �� �� NO_MOD = 1 ( ���� 1 )
                DB      2       ;�ਧ��� ���ਨ �� �� NO_MOD = 2 ( ���� 2 )
                DB      2       ;�ਧ��� ���ਨ �� �� NO_MOD = 3 ( ���� 3 )
                DB      2       ;�ਧ��� ���ਨ �� �� NO_MOD = 4 ( ���� 4 )
                DB      2       ;�ਧ��� ���ਨ �� �� NO_MOD = 5 ( ���� 5 )
                DB      2       ;�ਧ��� ���ਨ �� �� NO_MOD = 6 ( ���� 6 )
FG_TIS          DB      KL_TIS DUP(0)
                                        ;0-��������� DATA ������������
                                        ;1-����� ������������
                                        ;2-������������ ��������
                                        ;3-���� ���� (���� ��������� ������.)
                                        ;4-��������� ������������
                                        ;5-DATA ��������
                                        ;6-���� ����� � TIS
                                        ;7-������ ������


IF _MSP EQ TRUE
FG_SEG          DB      256 DUP(0FFH)
                                        ;0-0-ZANYT/1-��������
                ;���������� FLASH
SMESH_DATA_T    EQU     4000H
SMESH_TIS_PREF  EQU     4400H
SMESH_PAROLI    EQU     5000H
SMESH_PRIOR     EQU     SMESH_PAROLI+100H
SMESH_FL_PAR    EQU     SMESH_PRIOR+100H
SMESH_PAR_BL    EQU     6000H
ADR_KONF_F      EQU     160000H         ;����� ����.�� FLASH
MIN_DLIN_BLOKA  EQU     80H*16
FG_FL           DW      0
FL_STEP         DW      0
ETIC_FL         DW      0EFBDH
SLED_SEG        DW      PERV_SEG_KONF
FL_ESI          DD      0
FL_DS           DW      0
FL_ECX          DD      0
FL_EDI          DD      0
OLD_FL_ESI      DD      0
OLD_FL_DS       DW      0
OLD_FL_ECX      DD      0
OLD_FL_EDI      DD      0
FL_SEG          DB      0
ERR_FLASH       DB      0
ERASE_SEG       DB      0
SLED_FL_STEP    DB      0
KL_SEG_KONF     EQU     26
ENDIF
;**************
FG_PROG         DB      1               ; 䫠� ������
                                        ; 0 - ��஥
                                        ; 1 - �����
                                        ; 3 - ��������
VERSIY          DB      0
HardVers_hi     DB      0
HardVers_lo     DB      0


;***** ��६���� ����஫� ������ �訡�� ( �-3 �-5 )*************
FEC_Ptr         DB      4 DUP(0); 㪠��⥫� � ������⥫� FEC
FEC1            DB    512 DUP(0); ������⥫� ��⠭��� ���祭�� FEC
FEC_Sum         DW      4 DUP(0); ���������� �㬬� FEC

;**************

; --- ��६���� ��� ������樨 �୮�� ᯨ᪠
BL_CurrNum_In   DD      0       ;
BL_Count_In     DW      0       ;
BL_CurrNum_Out  DD      0       ;
BL_Count_Out    DW      0       ;
IF _MSP EQ TRUE
; --- ��६����� ��� �����஢���� �ਥ�� �६��� � ����筨��
NO_TIME_OF_CK   DB      0       ; 0 - =1 - �� �ਭ����� �६� � ��
                                ; 1 - ��।����� �६� �� ��

; ---------------------- -= * ���� * =- -------------------------------
N_GEN_CTO_R	DW      14      ;����� ��� ���� �ਥ�� �� ���
N_GEN_CTO_T	DW      15      ;����� ��� ���� ��।�� �� ���

        ;!!!! �� �뫮 � hard.var ����� ���� � ��㣮� ᥣ����
ST_VOICE        DD      10000h                  ;��砫�� ���� �����
                                                ;����ᮢ�� ᮮ�饭��
ST_PROGRAM      DD      300000h                 ;��砫�� ���� �᭮����
                                                ;�ணࠬ��
ST_CONFIG       DD      200000h                 ;��砫�� ���� ���䨣��樨
LEN_CONF        DD      200                     ;����� ����� ���䨣��樨
CURR_PROG       DB      0

;;; kill it !!!!
NOM_BLOKA_T     DD      0
ELSE
CURRENT_TIME	DD	0			;����饥 �६� (� 2-� ��

; --- WatchDog
WD_Time         DD      0
_WD_TOUT        =       1000
WD_Value        DB      0
ADR_WD          =       4409h
WD_Reboot       DB      0
Reboot_Time     DB      0

                ; --- ���� ��� ��ࠢ�� �����᪨�  ᮮ�饭��
buff_PUMA       DB      _PUMA_MESS_LEN DUP(0)

;;;HDD!!!
                ; --- 䠩��, �ᯮ��㥬� �ணࠬ���, � �� ��ࠬ����
_FILE_PRG_SIZE   =      512*1024        ;;;!!!
_FILE_CFG_SIZE   =      512 * 12         ;;;!!!
_FILE_RBF_SIZE   =      0D100Ch
_FILE_BLST_SIZE  =      80000h          ;;;!!!
_FILE_ANI_SIZE   =      20a000h         ;;;!!!
_FILE_BUNCH_SIZE =      1053276


_FILE_CFG_       DB     'tf_sorm.cfg', 5 dup(0)
_FILE_PRG_NAME   DB     'PROG.EXE', 5 dup(0)
_FILE_PRG_NAME_T DB     'PROG.TMP', 5 dup(0)
_FILE_PRG_NAME_C DB     'PROG~.EXE', 5 dup(0)
_FILE_CFG_NAME   DB     'TF_SORM.CFG', 5 dup(0)
_FILE_CFG_NAME_T DB     'TF_SORM.TMP', 5 dup(0)
_FILE_CFG_NAME_C DB     'TF_SORM~.CFG', 5 dup(0)
_FILE_BLST_NAME  DB     'TF_SORM.BLS', 5 dup(0)
_FILE_BLST_NAME_T DB     'TF_SORM.TMP', 5 dup(0)
_FILE_BLST_NAME_C DB     'TF_SORM~.BLS', 5 dup(0)
_FILE_ANI_NAME   DB     'TF_SORM.ANI', 5 dup(0)
_FILE_BUNCH_NAME DB     'TF_SORM.BNC', 5 dup(0)
_FILE_RBF_NAME   DB     'CYCL_PCM.RBF', 5 dup(0)
_FILE_RBF_NAME_T DB     'CYCL_PCM.TMP', 5 dup(0)
_FILE_RBF_NAME_C DB     'CYCL_PC~.RBF', 5 dup(0)



; �����䨪���� ०���� ���䨣��樨
_NOP_ID	        =	0FFh
_PROGR_ID	=	000h
_CONFG_ID       =	001h
_DOPIN_ID       =	002h
_ANI_ID         =       003h
_BUNCH_ID       =       004h

_FILE_PRG_ID    DD      _PROGR_ID
_FILE_GFG_ID    DD      _CONFG_ID
_FILE_BLST_ID   DD      _DOPIN_ID
_FILE_ANI_ID    DD      _ANI_ID
_FILE_BUNCH_ID  DD      _BUNCH_ID
ENDIF

__dd_num	db	18   ; MVT ������� ������� �⫠���� ��६����
_1st_dd   label byte
_1st_dword   label dword
__dd1            DD      0    ; \
__dd2            DD      0    ; |
__dd3            DD      0    ; |
__dd4            DD      0    ; |   �⫠���� ��६����
__dd5            DD      0    ; |
__dd6            DD      0    ; |
__dd7            DD      0    ; |
__dd8            DD      0    ; |
__dd9            DD      0    ; |
__dd10           DD      0    ; /
_2nd_dd   label byte
__dd11           DD      0    ; \
__dd12           DD      0    ; |
__dd13           DD      0    ; |
__dd14           DD      0    ; |   �⫠���� ��६����
__dd15           DD      0    ; |
_16n_dd   label byte
__dd16           DD      0    ; |
__dd17           DD      0    ; |
__dd18           DD      0    ; |
__dd19           DD      0    ; |
__dd20           DD      0    ; /
_3rd_dd   label byte
__dd21           DD      0    ; \
__dd22           DD      0    ; |
__dd23           DD      0    ; |
__dd24           DD      0    ; |   �⫠���� ��६����
__dd25           DD      0    ; |
__dd26           DD      0    ; |
__dd27           DD      0    ; |
__dd28           DD      0    ; |
__dd29           DD      0    ; |
__dd30           DD      0    ; /
_4th_dd   label byte
__dd31           DD      0    ; \
__dd32           DD      0    ; |
__dd33           DD      0    ; |
__dd34           DD      0    ; |   �⫠���� ��६����
__dd35           DD      0    ; |
__dd36           DD      0    ; |
__dd37           DD      0    ; |
__dd38           DD      0    ; |
__dd39           DD      0    ; |
__dd40           DD      0    ; /
_5th_dd   label byte
__dd41           DD      0    ; \
__dd42           DD      0    ; |
__dd43           DD      0    ; |
__dd44           DD      0    ; |   �⫠���� ��६����
__dd45           DD      0    ; |
__dd46           DD      0    ; |
__dd47           DD      0    ; |
__dd48           DD      0    ; |
__dd49           DD      0    ; |
__dd50           DD      0    ; /

; MVT ��६���� ��� ࠡ��� � 䠩��� �⫠��筮� ���ଠ樨
if _MSP NE TRUE
DebugFile db 'DEBUG.TXT',0
DebugFileDescriptor	dd ?
DebugFileOpened	db 0  ; 0 - ������, 1 - �����, 2 - ���� ����� ������ ��� �������, �஢�ઠ ࠧ��� �⪫�祭�
DebugFileDDValue db 8 dup (?)
DebugFileHeader	db	' Debug started '
cr_lf	db 13,10
space_symb	db ' '
DebugArray db 0aah, 0bbh, 0cch
DebugFilePoint db '.'
DebugFileBottom db 13,10,'Debug stopped'
DebugFileHexBuffer	db 1024 dup(?)
ici db 'ici ',13,10 ; ��ப� ��� ����஢�� �ணࠬ�� � 䠩� ����ᮬ @String_to_file
if _isDebugBuffered
DebugFileBuffer db _DebugBufferSize dup (?)
DebugFileBufferPos dd 0
endif
BreakDebugFile db 13,10,'==out of max size==',13,10
BreakDebugFileLen db 23
DebugFileSizeInfoCol db 20
DebugFileSizeInfoRow db 33
NoDebugFileSize db 'OVERFLOW'
DebugFileSizeColor	EQU clYELLOW * 10h + clRED
CurrentDebugFileSize	dd	?
isDebugFileOverflow	db	?   ; 0 - ��� ��९������� ��� 䠩��, 1 - ����


string1	db 'asdf'
string1offset dd offset string1
string2 db 'asdf'
string2offset dd offset string2

if _debug_Puma
puma_RTP db 'RTP ',13,10
puma_RTW db 'RTW ',13,10
puma_INFO db 'INFO',13,10
puma_NULL db 'NULL',13,10
puma_unknown db '??? ',13,10
puma_RTP_offset dd offset puma_RTP
puma_RTW_offset dd offset puma_RTW
puma_INFO_offset dd offset puma_INFO
puma_NULL_offset dd offset puma_NULL
puma_unknown_offset dd offset puma_unknown

SendString db 'SEND>'
SendStringOffset dd offset SendString
ReceiveString db 'RECEIVE>'
ReceiveStringOffset dd offset ReceiveString

rtp_array db 98h,0E6h,0FFh,01h,00h,3Eh
rtp_array_offset dd offset rtp_array
rtw_array db 98h,0E6h,0FFh,01h,05h,25h
rtw_array_offset dd offset rtw_array
null_array db 98h,0E6h,0FFh,00h,0D7h
null_array_offset dd offset null_array
endif
endif
; /MVT

 ; ---- Black List DD's -----
_black_dd       label dword
__bl_dd1        DD      0
__bl_dd2        DD      0
__bl_dd3        DD      0
__bl_dd4        DD      0
__bl_dd5        DD      0
__bl_dd6        DD      0
__bl_dd7        DD      0
__bl_dd8        DD      0
__bl_dd9        DD      0
__bl_dd10       DD      0
__bl_dd11       DD      0
; --- Black List Debuger -------------------
_black_active           DB      0           ; ---> ���ﭨ� �⫠�稪�: 0 - �� ��⨢., 255 - ��⨢.
_numb_dds_bl            DB      0           ; ���稪 ��室� �� ���. �.�.
_numb_dds_bl_prev       DB      0   ; �।��騩 ��室 �� ���. �.�.
_numb_dds_bl_in         DB      0           ; ���稪 ��室� �� ��. �.�.
_prev_dds_bl_in         DB      0   ; �।��騩 ��室 �� ��. �.�.
_differ_black           DB      0   ; ��६. ��� ��।������ �ࠢ��� ���. �.�.
; ---- ����஢���� �.�.(quit)
_numb_dds_bl_q          DB      0           ; ���稪 ��室� �� ���. �.�.(quit)
_numb_dds_bl_prev_q     DB      0   ; �।��騩 ��室 �� ���. �.�.(quit)
_numb_dds_bl_in_q       DB      0           ; ���稪 ��室� �� ��. �.�.(quit)
_prev_dds_bl_in_q       DB      0   ; �।��騩 ��室 �� ��. �.�.(quit)
_black_differ_q         DB      0   ; ��६. ��� ��।������ �ࠢ��� ���. �.�.(quit)
_differ_black_q         DB      0   ; ��६. ��� ��।������ ���४⭮�� ����� ��䨪� ��(quit)
BL_dbg_q                DB      0   ; ����஢���� �.�.
; -----------------------------------------------------------------
;;;_Buff_Buff       DB      512 DUP(0) --- not used
; --- {Paul.N} --- [31/05/13]
; -- ���� ��� �࠭���� ��뫮� �� ��ᨨ �� �� �� ��ॠ���樨
Buff_Sess_RA            DW     _Sess_RA_MAX DUP(0FFFFh)

