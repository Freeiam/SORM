;浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様�
;�  ｯ爛ぅ�キ┘ 痰珮�矣� え瓷爬�皰牀� 瓮��キ皰�, ����� 瓮��キ皰�,      把�
;�  ��゛�肓 �ム襤��┤, �瓮△�ぅ瓷爬�皰��                               � �
;藩冤様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様� �
;  青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳�
        ; --- 痰珮�矣�� ぅ瓷爬�皰�� 瓮��キ皰�
DESCR   STRUC
LIM     DW      0       ;������� (���よゥ 甄�〓)
BASE_L  DW      0       ;���� (���よゥ 甄�〓)
BASE_M  DB      0       ;���� (痼イ�┤ ����)
ATTR_1  DB      0b      ;�矗 1    00000000
                                ; 崖崖崖垣 A °�� �÷�薀�┘
                                ; 鈎抒崖� R (��� 瓮��キ�� ����) R=1 艪キ┘ ��оヨキ�
                                ; 崖 崖�  W (��� 瓮��キ�� ����諷) W=1 ����瘡 ��оヨキ�
                                ; 崖 崖�            か� ����諷 艪キ┘ ��оヨキ� ≡ィ��
                                ; 崖 崖� � (��� 瓮��キ�� ����) �=1 - ��ょ┃キ�覃 C=0 - �°膈覃
                                ; 崖 崖  ED (か� ���������膈��� 爛Θ��) ED=1 - 瓮��キ� 痰オ�
                                ; 崖 崖                                 ED=0 - 瓮��キ� ����諷
                                ; 崖 垣 I I=1 - ��� I=0 - ����襯 � 痰オ
                                ; 崖 � S S=1 - 轤� 瓮��キ� S=0 - 轤� 瓱痰ガ�覃 �′オ�
                                ; 垣 dpl - 窶�▲�� �爬▲�ィ┤ 瓮��キ�� (0-3)
                                ; � P �爬痺矚癶┘ P=1 瓮��キ� �爬痺矚癶礇� � ���閧�
ATTR_2  DB   00000000b  ;�矗 2    00000000
                                ; 崖崖請疏� ������� (痰�琥┤ ���� ����)
                                ; 崖垣� VL
                                ; 崖青 n
                                ; 垣� D - ��о錣��痰� ����諷 ┼� �甎���キ��
                                ; �      (D=1 - 32 ��о錣� D=0 - 16 ��о錣��)
                                ; 青 G - ；� ����祀閠��痰� (G=1 - LIM * 4095 + 4095 = NLIM)
BASE_H  DB      0       ;���� (痰�琥┤ ����)
DESCR   ENDS

        ; --- 痰珮�矣�� ぅ瓷爬�皰�� ��≪茱�
TRAP  STRUC             ;�矗礫矣�� 茫逾� ��≪茯�
TRAP_OFFS_L     DW  0   ;Offset 皰腦� √��� (���よ┤)
TRAP_SEL        DW  120 ;�カオ皰� 瓮��キ�� �÷�｀砒┴�
TRAP_RSRV       DB  0   ;�ェム�
TRAP_ATTR       DB  8Fh ;�矗─竄 00000000
                        ;        垣干請疏� 皋� �襷���: F-��≪茯�(�牀������� �爛琺���┘);
                        ;        � 崖                  E-�爛琺���┘; C-�襷��; 5-������
                        ;        � 垣陳 always NULL
                        ;        � 青陳 dpl - 窶�▲�� �爬▲�ィ┤ �÷�｀砒┴� (0-3)
                        ;        青陳陳 P �爬痺矚癶┘ P=1 �÷�｀砒┴ �爬痺矚癶礇� � ���閧�
TRAP_OFFS_H     DW  0   ;Offset 皰腦� √��� (痰�琥┤)
TRAP    ENDS

        ; --- 痰珮�珮�� ぅ瓷爬�皰�� �爛琺���┤
INTR    STRUC           ;�矗礫矣�� 茫逾� �爛琺�����
OFFSI_L DW      0       ;Offset 皰腦� √��� (���よ┤)
SELI    DW      120     ;�カオ皰� 瓮��キ�� �÷�｀砒┴�
RSRVI   DB      0       ;�ェム�
ATTRI   DB      8Eh     ;�矗─竄
OFFSI_H DW      0       ;Offset 皰腦� √��� (痰�琥┤)
INTR	ENDS

        ; --- ������讚�� ��゛��� ぅ瓷爬�皰牀� ----------------------------------
DESCR_NULL           DESCR   <>                          ;0
DESCR_SEG_AX         DESCR   <AXS_SIZE-1,,,     92h,,>   ;8     ; Гメ� �爛く�����モ瘴, 艪� 瓮��キ� < 64kb
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
DESCR_SEG_ZAG        DESCR   <0FFFFH,,      0FH,92h>     ;248    ; ���珮х┴
DESCR_SEG_BLCK_LST1  DESCR   <0FFFFH,,      40H,92h,7,>  ; 4! 256: 58.4000 - 60.3fff
DESCR_SEG_BLCK_LST2  DESCR   <0FFFFH,,      48H,92h,7,>  ; 5! 264: 60.4000 - 68.3fff
descr_seg_flash_0    DESCR   <0FFFFh,,      0Fh,92h>     ;272
descr_seg_flash_1    DESCR   <0FFFFh,,      0Bh,92h>     ;280
descr_seg_all        DESCR   <0FFFFh,,,         92h,8Fh> ;288    ; ≡� ���閧�!!!

; PUMA _start
DESCR_PKTMGR_CODE    DESCR   <0FFFFh,0,0,       9ah,,>   ;296 ��� !!!
DESCR_PKTMGR_DATA    DESCR   <0FFFFh,0,0,       92h,,>   ;304
DESCR_EXTRA_SCREEN   DESCR   <7999,0,       08h,92h,,>   ;312
DESCR_PUMA_HEAP      DESCR   <0FFFFh,,      07h,92h,,>   ;320
; PUMA _end
IF _MSP EQ TRUE
DESCR_SEG_SORM       DESCR   <0FFFFH,,,         92h,,>   ;328
DESCR_SEG_BUNCH      DESCR   <258,,         69h,92h,80h,>;336

                ; 敖� ���� 陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
                ; �                                     �
DESCR_SEG_PUMA_DRV   DESCR   <4095,8000h,   08h,92h,,> ;� 344
DESCR_SEG_PUMA_SCR   DESCR   <SEG_PUMA_SIZE,,  ,92h,,> ;� 352
DESCR_SEG_PUMA       DESCR   <0ffffh,,      09h,92h,,> ;� 360
                ; �                                     �
                ; 青� ���� -陳陳陳陳陳陳陳陳陳陳陳陳陳陳�

ELSE
; - - - - - - - - - - - - - - -  H D D - - - - - - - - - - - - - - - - - - - -
;
DESCR_HDD_HEAP       DESCR   <0ffffh,,72h,92h,03h,>     ;328
DESCR_HDD_CODE       DESCR   <HDD_SIZE - 1,,,9ah,,>     ;336

; ��� 甎�瓷� ��� 525 * 4096 = 20d000h ����
DESCR_ANI            DESCR   <525,,80h,92h,80h,>        ;344

; - - - - - - - - - - - - - - -  � � � � - - - - - - - - - - - - - - - - - - -
;
DESCR_PLL_CS         DESCR   <PLL_CS_SIZE-1,,,98h,,>    ;352
DESCR_PLL_DATA       DESCR   <PLL_SIZE-1,,,92h,,>       ;360

DESCR_SEG_SORM       DESCR   <0FFFFH,,,92h,,>           ;368
DESCR_SEG_MAX_II     DESCR   <0ffffh, 00000h, 0dh, 92h> ;376

                ; 敖� ���� 陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
                ; �                                     �
DESCR_SEG_PUMA_DRV   DESCR   <4095,8000h,   08h,92h,,> ;� 384
DESCR_SEG_PUMA_SCR   DESCR   <SEG_PUMA_SIZE,,  ,92h,,> ;� 392
DESCR_SEG_PUMA       DESCR   <0ffffh,,      09h,92h,,> ;� 400
                ; �                                     �
                ; 青� ���� -陳陳陳陳陳陳陳陳陳陳陳陳陳陳�
;    ----------      甄イ竡荐� �� DESCR_ANI か� �竍��� �� ���-� -----------
DESCR_SEG_BUNCH      DESCR   <258,0D001h,  0A0h,92h,80h,>;408    ; A0.D001 - B1.0000 (1MB+12Kb)
;    ----------      甄イ竡荐� �� DESCR_SEG_BUNCH �� ���-� -----------
DESCR_SEG_ULOG       DESCR   <0FFFFH,0001h, 0B1h,92h,,> ;416    ; B1.0001 - B2.0000 (64Kb)


ENDIF
DESCR_SIZE=$-DESCR_NULL                                  ;

; ------ for BL segments or other --------
_BLCK_LST1_SEG    equ   256
_BLCK_LST2_SEG    equ   264

; --- ���痰��硅 � ���ム��� 瓮��キ皰�
_ADR_SEG_FLASH_0     EQU     272        ;�ィ�キ皚覃 �むメ ����� ���珮х┴�
_ADR_SEG_FLASH_1     EQU     280        ;�ィ�キ皚覃 �むメ ����� (��〓��)
IF _MSP NE TRUE
_ANI_LST_SEG         equ     344
ENDIF
_SEG_ALL             equ     288        ; ≡� 4 �┌�

        ; --- ��゛��� ぅ瓷爬�皰牀� �爛琺���┤ ----------------------------------
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
PDESCR		DQ	0			;轒△�ぅ瓷爬�皰�
;
SS_SEG          DW      ?                       ;���腑�┘ 爛��痰�� SS
IF _MSP NE TRUE
BUF_DEBUG	DB	512	DUP(0)

;@GenerateDiscriptorC    <super_small>, 7000000h, 348980, true, true, false
;--------------------------------------
;
; _begin = 甃ラキ┘ ������ 瓮��キ��, _size = か┃� 瓮��キ�� � ������
; _dc = �����襯(1) ┼� ���(0), _bit = ��о錣��痰� 16(0) ┼� 32(1)
; _step = ��� 瓮��キ�� �°膈覃(0) ┼� �� 4096(1)
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


;//�祗�罔� PCI BIOS Revision 2.1
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
;//��Б����ガ襯 ��る 粃��罔�
successful              equ     0
func_not_supported      equ     81h
bad_vendor_id           equ     83h
device_not_found        equ     86h
bad_registr_number      equ     87h
set_failed              equ     88h
buffer_too_small        equ     89h
;
PCI_CONFIG              STRUC
;//���爐┃�硅 PCI

;//�����モ琺 竅矗��痰�� PCI
VendorId        dw      1172h   ;┐キ皋筥��皰� �牀├〓え皀��
DeviceId        dw      0008h   ;┐キ皋筥��皰� 竅矗��痰��
RevisionId      db      01h
BusNumber       db      ?       ;���ム 荐��
DeviceNumber    db      ?       ;���ム 竅矗��痰�� � 粃��罔�
                                ; NNNNNFFF
                                ; N-���ム
                                ; F - 粃��罔�
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
LenMemory       dd      0    ; ��Кム �゛�痰� ���閧�
LenPortArray    dd      0    ; ��Кム �゛�痰� ��珥�� □���/�襤��� 竅矗��痰��

;//�ムガキ�襯 竅矗��痰�� PCI
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
