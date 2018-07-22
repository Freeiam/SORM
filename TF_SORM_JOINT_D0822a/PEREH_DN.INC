;���������������������������������������������������������������������ͻ
;�  ��।������ ������� ���ਯ�஢ ᥣ���⮢, ᠬ�� ᥣ���⮢,      �Ŀ
;�  ⠡���� ���뢠���, �ᥢ�����ਯ��                               � �
;���������������������������������������������������������������������ͼ �
;  �����������������������������������������������������������������������
        ; --- ������� ���ਯ�� ᥣ���⮢
DESCR   STRUC
LIM     DW      0       ;�࠭�� (����襥 ᫮��)
BASE_L  DW      0       ;���� (����襥 ᫮��)
BASE_M  DB      0       ;���� (�।��� ����)
ATTR_1  DB      0b      ;��� 1    00000000
                                ; �������� A �뫮 ���饭��
                                ; ��ٳ��� R (��� ᥣ���� ����) R=1 �⥭�� ࠧ�襭�
                                ; �� ���  W (��� ᥣ���� ������) W=1 ������ ࠧ�襭�
                                ; �� ���            ��� ������ �⥭�� ࠧ�襭� �ᥣ��
                                ; �� ��� � (��� ᥣ���� ����) �=1 - ���稭���� C=0 - �����
                                ; �� ��  ED (��� ���������筮�� ०���) ED=1 - ᥣ���� �⥪�
                                ; �� ��                                 ED=0 - ᥣ���� ������
                                ; �� �� I I=1 - ��� I=0 - ����� � �⥪
                                ; �� � S S=1 - �� ᥣ���� S=0 - �� ��⥬�� ��쥪�
                                ; �� dpl - �஢��� �ਢ������ ᥣ���� (0-3)
                                ; � P ������⢨� P=1 ᥣ���� ��������� � �����
ATTR_2  DB   00000000b  ;��� 2    00000000
                                ; ��������� �࠭�� (���訩 ���� ����)
                                ; ����� VL
                                ; ���� n
                                ; ��� D - ࠧ�來���� ������ ��� �ᯮ������
                                ; �      (D=1 - 32 ࠧ�鸞 D=0 - 16 ࠧ�冷�)
                                ; �� G - ��� �࠭��୮�� (G=1 - LIM * 4095 + 4095 = NLIM)
BASE_H  DB      0       ;���� (���訩 ����)
DESCR   ENDS

        ; --- ������� ���ਯ�� ����襪
TRAP  STRUC             ;������� � ����誨
TRAP_OFFS_L     DW  0   ;Offset �窨 �室� (����訩)
TRAP_SEL        DW  120 ;������� ᥣ���� ��ࠡ��稪�
TRAP_RSRV       DB  0   ;�����
TRAP_ATTR       DB  8Fh ;��ਡ�� 00000000
                        ;        ��������� ⨯ �맮��: F-����誠(�ணࠬ��� ���뢠���);
                        ;        � ��                  E-���뢠���; C-�맮�; 5-�����
                        ;        � ���� always NULL
                        ;        � ���� dpl - �஢��� �ਢ������ ��ࠡ��稪� (0-3)
                        ;        ������ P ������⢨� P=1 ��ࠡ��稪 ��������� � �����
TRAP_OFFS_H     DW  0   ;Offset �窨 �室� (���訩)
TRAP    ENDS

        ; --- ������� ���ਯ�� ���뢠���
INTR    STRUC           ;������� � ���뢠���
OFFSI_L DW      0       ;Offset �窨 �室� (����訩)
SELI    DW      120     ;������� ᥣ���� ��ࠡ��稪�
RSRVI   DB      0       ;�����
ATTRI   DB      8Eh     ;��ਡ��
OFFSI_H DW      0       ;Offset �窨 �室� (���訩)
INTR	ENDS

        ; --- ������쭠� ⠡��� ���ਯ�஢ ----------------------------------
DESCR_NULL           DESCR   <>                          ;0
DESCR_SEG_AX         DESCR   <AXS_SIZE-1,,,     92h,,>   ;8     ; ����� �।����������, �� ᥣ���� < 64kb
DESCR_SEG_FS         DESCR   <0FFFFH,8000H, 0BH,92h,,>   ;16
DESCR_SEG_ST         DESCR   <0FFFFH,8000H, 0CH,92h,,>   ;24
DESCR_SEG_GS         DESCR   <0FFFFH,,      0DH,92h,,>   ;32
DESCR_SEG_OBJ        DESCR   <0FFFFH,,,         92h,,>   ;40
DESCR_SEG_BABA       DESCR   <0FFFFH,8000H, 0DH,92h,,>   ;48
DESCR_SEG_TF         DESCR   <0FFFFH,,      10H,92h,0FH,>;56     ;1M-1M
DESCR_SEG_KONF       DESCR   <0FFFFH,,,         92h,,>   ; 3! 64 : 43.4000 - 58.3fff
IF _MSP EQ TRUE
DESCR_SEG_ULOG       DESCR   <0FFFFH,,      25H,92h,,>   ;72
ELSE
DESCR_SEG_PARAM      DESCR   <0FFFFH,,28H,92h,7,>       ;72
ENDIF
DESCR_SEG_NLOG       DESCR   <0FFFFH,,      20H,92h,4,>  ;80     ;2M-327KB
DESCR_SEG_AL         DESCR   <0FFFFH,,      25H,92h,1,>  ; -- 88
DESCR_SEG_SOED       DESCR   <0FFFFH,,,         92h,,>   ; 2! 96 : 30.0000 - 43.3fff
DESCR_SEG_BUS        DESCR   <0FFFFH,,,         92h,,>   ;104
DESCR_SEG_NULL       DESCR   <0FFFFH,,,         92h,,>   ;112
DESCR_SORM           DESCR   <SORM_SIZE-1,,,    9Ah,,>   ;120 ��� !!!
DESCR_STACK          DESCR   <SS_SIZE-1,,,      92h,,>   ;128
DESCR_C101           DESCR   <0FFFFH,0C000H,0CH,92h,,>   ;136
DESCR_C101_1         DESCR   <0FFFFH,,      0DH,92h,,>   ;144
DESCR_SEG_KONF_UP    DESCR   <0FFFFH,,      3AH,92h,7,>  ; -- 152
DESCR_SEG_C000       DESCR   <0FFFFH,0000H, 0CH,92h,,>   ;160
DESCR_SEG_SCREN      DESCR   <0FFFFH,,,         92h,,>   ;168
DESCR_SEG_VIZ        DESCR   <0FFFFH,,      37H,92h,1,>  ; -- 176
DESCR_SEG_ADR_SOED   DESCR   <0FFFFH,,      2BH,92h,,>   ; 1! 184: 2b.0000 - 2f.ffff
DESCR_SEG_FL         DESCR   <0FFFFH,,,         92h,,>   ;192
DESCR_SEG_FLU        DESCR   <0FFFFH,,,         92h,,>   ;200
DESCR_SEG_SYS        DESCR   <0FFFFH,,,         92h,,>   ;208
DESCR_SEG_AXD        DESCR   <AXD_SIZE-1,,,     92h,,>   ;216
DESCR_DOP            DESCR   <DOP_SIZE-1,,,     9Ah,,>   ;224 ��� !!!
DESCR_SEG_COM        DESCR   <0FFFFH,,      29H,92h,01H,>;232
DESCR_SEG_FLSH       DESCR   <07FFFH,,      0BH,92h>     ;240    ;1M-1M FLASH???    0B0000-beg
DESCR_SEG_ZAG        DESCR   <0FFFFH,,      0FH,92h>     ;248    ; �����稪
DESCR_SEG_BLCK_LST1  DESCR   <0FFFFH,,      40H,92h,7,>  ; 4! 256: 58.4000 - 60.3fff
DESCR_SEG_BLCK_LST2  DESCR   <0FFFFH,,      48H,92h,7,>  ; 5! 264: 60.4000 - 68.3fff
descr_seg_flash_0    DESCR   <0FFFFh,,      0Fh,92h>     ;272
descr_seg_flash_1    DESCR   <0FFFFh,,      0Bh,92h>     ;280
descr_seg_all        DESCR   <0FFFFh,,,         92h,8Fh> ;288    ; ��� ������!!!

; PUMA _start
DESCR_PKTMGR_CODE    DESCR   <0FFFFh,0,0,       9ah,,>   ;296 ��� !!!
DESCR_PKTMGR_DATA    DESCR   <0FFFFh,0,0,       92h,,>   ;304
DESCR_EXTRA_SCREEN   DESCR   <7999,0,       08h,92h,,>   ;312
DESCR_PUMA_HEAP      DESCR   <0FFFFh,,      07h,92h,,>   ;320
; PUMA _end
IF _MSP EQ TRUE
DESCR_SEG_SORM       DESCR   <0FFFFH,,,         92h,,>   ;328
DESCR_SEG_BUNCH      DESCR   <258,,         69h,92h,80h,>;336

                ; ��� ���� ����������������������������Ŀ
                ; �                                     �
DESCR_SEG_PUMA_DRV   DESCR   <4095,8000h,   08h,92h,,> ;� 344
DESCR_SEG_PUMA_SCR   DESCR   <SEG_PUMA_SIZE,,  ,92h,,> ;� 352
DESCR_SEG_PUMA       DESCR   <0ffffh,,      09h,92h,,> ;� 360
                ; �                                     �
                ; ��� ���� -�����������������������������

ELSE
; - - - - - - - - - - - - - - -  H D D - - - - - - - - - - - - - - - - - - - -
;
DESCR_HDD_HEAP       DESCR   <0ffffh,,72h,92h,03h,>     ;328
DESCR_HDD_CODE       DESCR   <HDD_SIZE - 1,,,9ah,,>     ;336

; ��� ᯨ᪨ ��� 525 * 4096 = 20d000h ����
DESCR_ANI            DESCR   <525,,80h,92h,80h,>        ;344

; - - - - - - - - - - - - - - -  � � � � - - - - - - - - - - - - - - - - - - -
;
DESCR_PLL_CS         DESCR   <PLL_CS_SIZE-1,,,98h,,>    ;352
DESCR_PLL_DATA       DESCR   <PLL_SIZE-1,,,92h,,>       ;360

DESCR_SEG_SORM       DESCR   <0FFFFH,,,92h,,>           ;368
DESCR_SEG_MAX_II     DESCR   <0ffffh, 00000h, 0dh, 92h> ;376

                ; ��� ���� ����������������������������Ŀ
                ; �                                     �
DESCR_SEG_PUMA_DRV   DESCR   <4095,8000h,   08h,92h,,> ;� 384
DESCR_SEG_PUMA_SCR   DESCR   <SEG_PUMA_SIZE,,  ,92h,,> ;� 392
DESCR_SEG_PUMA       DESCR   <0ffffh,,      09h,92h,,> ;� 400
                ; �                                     �
                ; ��� ���� -�����������������������������
;    ----------      ᫥���訩 �� DESCR_ANI ��� ��窮� �� ���-� -----------
DESCR_SEG_BUNCH      DESCR   <258,0D001h,  0A0h,92h,80h,>;408    ; A0.D001 - B1.0000 (1MB+12Kb)
;    ----------      ᫥���訩 �� DESCR_SEG_BUNCH �� ���-� -----------
DESCR_SEG_ULOG       DESCR   <0FFFFH,0001h, 0B1h,92h,,> ;416    ; B1.0001 - B2.0000 (64Kb)


ENDIF
DESCR_SIZE=$-DESCR_NULL                                  ;

; ------ for BL segments or other --------
_BLCK_LST1_SEG    equ   256
_BLCK_LST2_SEG    equ   264

; --- ����⠭�� � ����ࠬ� ᥣ���⮢
_ADR_SEG_FLASH_0     EQU     272        ;�������� ���� ����� �����稪�
_ADR_SEG_FLASH_1     EQU     280        ;�������� ���� ����� (������)
IF _MSP NE TRUE
_ANI_LST_SEG         equ     344
ENDIF
_SEG_ALL             equ     288        ; �� 4 ����

        ; --- ������ ���ਯ�஢ ���뢠��� ----------------------------------
EXC0		TRAP	<EXC_0,,,,>
EXC1		TRAP	<EXC_1,,,,>
EXC2		TRAP	<EXC_2,,,,>
EXC3		TRAP	<EXC_3,,,,>
EXC4		TRAP	<EXC_4,,,,>
EXC5		TRAP	<EXC_5,,,,>
EXC6		TRAP	<EXC_6,,,,>
EXC7		TRAP	<EXC_7,,,,>
EXC8		TRAP	<EXC_8,,,,>
EXC9		TRAP	<EXC_9,,,,>
EXCA		TRAP	<EXC_A,,,,>
EXCB		TRAP	<EXC_B,,,,>
EXCC		TRAP	<EXC_C,,,,>
EXCD		TRAP	<EXC_D,,,,>
EXCE		TRAP	<EXC_E,,,,>
EXCF		TRAP	<EXC_F,,,,>
EXC10		TRAP	<EXC_10,,,,>
EXC11		TRAP	<EXC_11,,,,>
EXC12           TRAP    <EXC_12,,,,>
EXC13           TRAP    <EXC_13,,,,>
EXC14           TRAP    <EXC_14,,,,>
EXC15           TRAP    <EXC_15,,,,>
EXC16           TRAP    <EXC_16,,,,>
EXC17           TRAP    <EXC_17,,,,>
EXC18           TRAP    <EXC_18,,,,>
EXC19           TRAP    <EXC_19,,,,>
EXC1A           TRAP    <EXC_1A,,,,>
EXC1B           TRAP    <EXC_1B,,,,>
EXC1C           TRAP    <EXC_1C,,,,>
EXC1D           TRAP    <EXC_1D,,,,>
EXC1E           TRAP    <EXC_1E,,,,>
EXC1F           TRAP    <EXC_1F,,,,>

IRQ_0           INTR    <INT_NULL1,,,,>         ;IRQ_0
IF _MSP EQ TRUE
IRQ_1           INTR    <INT_NULL1,,,,>         ;IRQ_1
ELSE
IRQ_1           INTR    <KLAVII,,,,>           ;IRQ_1
ENDIF
                INTR    1 DUP(<INT_NULL1,,,,>)  ;IRQ_2
IF _MSP EQ TRUE
IRQ_3           INTR    < PUMA_COM2,,,,>
ELSE
                INTR    < INT_NULL1,,,,>
ENDIF

IRQ_4           INTR    < PUMA_COM1,,,,>

                INTR    5 DUP(<INT_NULL1,,,,>)  ;IRQ_5-9
IRQ_10          INTR    <INTIR10,,,,>           ;IRQ_10
IRQ_11          INTR    <IRQ_40MS,,,,>          ;IRQ_11
                INTR    3 DUP(<INT_NULL1,,,,>)  ;IRQ_12-14

IRQ_15          INTR    <IRQ_2MS,,,,>           ;IRQ_15
IDT_SIZE=$-EXC0
;
PDESCR		DQ	0			;�ᥢ�����ਯ��
;
SS_SEG          DW      ?                       ;���祭�� ॣ���� SS
IF _MSP NE TRUE
BUF_DEBUG	DB	512	DUP(0)

;@GenerateDiscriptorC    <super_small>, 7000000h, 348980, true, true, false
;--------------------------------------
;
; _begin = ᬥ饭�� ��砫� ᥣ����, _size = ����� ᥣ���� � �����
; _dc = ������(1) ��� ���(0), _bit = ࠧ�來���� 16(0) ��� 32(1)
; _step = 蠣 ᥣ���� �����(0) ��� �� 4096(1)
@GenerateDiscriptorC    MACRO  _deskName, _begin, _size, _dc, _bit, _step
                LOCAL
_attr_1 = 0
_attr_2 = 0
_base_h = _begin shr 24
_base_m = (_begin shr 16) and 0FFh
_base_l = _begin and 0FFFFh
if _step eq true
_size = _size shr 12
_attr_2 = _attr_2 or _b_07
endif
if _dc eq true
_attr_1 = 92h
else
_attr_1 = 9Ah
endif
if _bit eq true
_attr_2 = _attr_2 or _b_06
else
_attr_2 = _attr_2 and _b_06
endif
_attr_2 = _attr_2 or ((_size shr 16) and 0Fh)
_limit = _size and 0FFFFh
&_deskName      descr   <_limit,_base_l,base_m,_attr_1,_attr_2,_base_h>
                ENDM


;//�㭪樨 PCI BIOS Revision 2.1
signatura_pci           equ     20494350h
signatura_pci32         equ     49435024h
pci_function_id         equ     0B1h
pci_bios_prezent        equ     0B101h
find_pci_device         equ     0B102h
find_pci_class_code     equ     0B103h
generate_special_cycle  equ     0B106h
read_config_byte        equ     0B108h
read_config_word        equ     0B109h
read_config_dword       equ     0B10Ah
write_config_byte       equ     0B10Bh
write_config_word       equ     0B10Ch
write_config_dword      equ     0B10Dh
get_irq_routing_options equ     0B10Eh
set_pci_irq             equ     0B10Fh
;//�����頥�� ���� �㭪権
successful              equ     0
func_not_supported      equ     81h
bad_vendor_id           equ     83h
device_not_found        equ     86h
bad_registr_number      equ     87h
set_failed              equ     88h
buffer_too_small        equ     89h
;
PCI_CONFIG              STRUC
;//���न���� PCI

;//��ࠬ���� ���ன�⢠ PCI
VendorId        dw      1172h   ;�����䨪��� �ந�����⥫�
DeviceId        dw      0008h   ;�����䨪��� ���ன�⢠
RevisionId      db      01h
BusNumber       db      ?       ;����� 設�
DeviceNumber    db      ?       ;����� ���ன�⢠ � �㭪樨
                                ; NNNNNFFF
                                ; N-�����
                                ; F - �㭪��
CommandPCI         dw      ?
StatusPCI       dw      ?
ClassCode       dd      ?
CashLine        db      ?
Lat_Timer       db      ?
HeaderType      db      ?
Bist            db      ?
Baddr0          dd      ?
Baddr1          dd      ?
Baddr2          dd      ?
Baddr3          dd      ?
Baddr4          dd      ?
Baddr5          dd      ?
CIS_PTR         dd      ?
SVendorId       dw      ?
SDeviceId       dw      ?
EXP_ROM         dd      ?
Int_Line        db      ?
Int_Pin         db      ?
Min_Gnt         db      ?
Max_Lat         db      ?
LenMemory       dd      0    ; ࠧ��� ������ �����
LenPortArray    dd      0    ; ࠧ��� ������ ���⮢ �����/�뢮�� ���ன�⢠

;//��६���� ���ன�⢠ PCI
addr_led        dw      ?
_LEN_PCI_CONFIG_ = $ - RevisionId
PCI_CONFIG      ENDS

;FirstPCI        PCI_CONFIG  <1172h, _DEVICE_ID_>
PCI_4           PCI_CONFIG  <1172h, 4>
PCI_8           PCI_CONFIG  <1172h, 8>
;
BaseAdr                 dd      0

PCI_4_InitFlag          db      0

PCI_8_InitFlag          db      0
ENDIF