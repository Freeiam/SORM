
@PEREH_ZR       MACRO
IF _MSP NE TRUE
; - - - - - - - - - - - - - - -  H D D - - - - - - - - - - - - - - - - - - - -
;
                XOR     EAX,EAX
                MOV     AX,HDD_CODE
                SHL     EAX,4
                MOV     DESCR_HDD_CODE.BASE_L,AX
                SHR     EAX,16
                MOV     DESCR_HDD_CODE.BASE_M,AL

; - - - - - - - - - - - - - - -  � � � � - - - - - - - - - - - - - - - - - - -
;
                MOV     AX,PLL_CS                       ;\
                SHL     EAX,4                           ;|
                MOV     DESCR_PLL_CS.BASE_L,AX          ;|
                SHR     EAX,16                          ;|
                MOV     DESCR_PLL_CS.BASE_M,AL          ;|
                                                        ;>�����⮢�� ���ਯ��
                MOV     AX,PLL_DATA                     ;|ᥣ���� "PLL_CS" � "PLL_DATA"
                SHL     EAX,4                           ;|
                MOV     DESCR_PLL_DATA.BASE_L,AX        ;|
                SHR     EAX,16                          ;|
                MOV     DESCR_PLL_DATA.BASE_M,AL        ;/
ENDIF
                ; ��� ���� ����������������������������Ŀ
                ; �                                     �
                XOR     EAX,EAX                       ; �
                MOV     AX, SEG SCREEN_SEG_PUMA       ; �
                SHL     EAX,4                         ; �
                MOV     DESCR_SEG_PUMA_SCR.BASE_L,AX  ; �
                SHR     EAX,16                        ; �
                MOV     DESCR_SEG_PUMA_SCR.BASE_M,AL  ; �
                ; �                                     �
                ; ��� ���� ������������������������������

                XOR     EAX,EAX
                MOV     AX,SS:SEG_AX                    ;\
                SHL     EAX,4                           ;>�����⮢�� ���ਯ��
                MOV     DESCR_SEG_AX.BASE_L,AX          ;|�᭮����� ᥣ���� ������
                SHR     EAX,16                          ;|
                MOV     DESCR_SEG_AX.BASE_M,AL          ;/

                XOR     EAX,EAX
                MOV     AX,SEG DATA_AXD                 ;\
                SHL     EAX,4                           ;>�����⮢�� ���ਯ��
                MOV     DESCR_SEG_AXD.BASE_L,AX         ;|ᥣ���� ������ ��䨪�樮���� ��
                SHR     EAX,16                          ;|
                MOV     DESCR_SEG_AXD.BASE_M,AL         ;/

                XOR     EAX,EAX
                MOV     AX,SEG DATA_TELEF               ;\
                SHL     EAX,4                           ;>�����⮢�� ���ਯ��
                MOV     DESCR_SEG_OBJ.BASE_L,AX         ;|ᥣ���� ������ ��ꥪ⮢ ����
                SHR     EAX,16                          ;|
                MOV     DESCR_SEG_OBJ.BASE_M,AL         ;/

                XOR     EAX,EAX
                MOV     AX,SEG DATA_SORM                ;\
                SHL     EAX,4                           ;>�����⮢�� ���ਯ��
                MOV     DESCR_SEG_SORM.BASE_L,AX        ;|ᥣ���� ������ ��ᨩ ����
                SHR     EAX,16                          ;|
                MOV     DESCR_SEG_SORM.BASE_M,AL        ;/

                XOR     EAX,EAX
                MOV     AX,SCREEN_SEG_PCM               ;\
                SHL     EAX,4                           ;>�����⮢�� ���ਯ��
                MOV     DESCR_SEG_SCREN.BASE_L,AX       ;|ᥣ���� ��࠭�
                SHR     EAX,16                          ;|
                MOV     DESCR_SEG_SCREN.BASE_M,AL       ;/

                XOR     EAX,EAX
                MOV     AX,SS:SEG_BUS                   ;\
                SHL     EAX,4                           ;>�����⮢�� ���ਯ��
                MOV     DESCR_SEG_BUS.BASE_L,AX         ;|ᥣ���� ⠡��� �����䬠 � ��⨫����䬠
                SHR     EAX,16                          ;|
                MOV     DESCR_SEG_BUS.BASE_M,AL         ;/

                XOR     EAX,EAX
                MOV     AX,CS                           ;\
                SHL     EAX,4                           ;>�����⮢�� ���ਯ��
                MOV     DESCR_SORM.BASE_L,AX            ;|ᥣ���� �᭮����� ����
                SHR     EAX,16                          ;|
                MOV     DESCR_SORM.BASE_M,AL            ;/

                XOR     EAX,EAX
                MOV     AX,CODE_AXD                     ;\
                SHL     EAX,4                           ;>�����⮢�� ���ਯ��
                MOV     DESCR_DOP.BASE_L,AX             ;|ᥣ���� �������⥫쭮�� ����
                SHR     EAX,16                          ;|
                MOV     DESCR_DOP.BASE_M,AL             ;/
;
                XOR     EAX,EAX
                MOV     AX,STACK_AXS                    ;\
                SHL     EAX,4                           ;>�����⮢�� ���ਯ��
                MOV     DESCR_STACK.BASE_L,AX           ;|ᥣ���� �⥪�
                SHR     EAX,16                          ;|
                MOV     DESCR_STACK.BASE_M,AL           ;/
;
                MOV     EAX,KL_TIS*2048*2               ; 64*2048*2 = 40000h
                SHR     EAX,10H                         ;������ ADR_SOED ࠧ������ �� 64KB
                MOV     DESCR_SEG_ADR_SOED.ATTR_2,AL    ; 4 !
                INC     AL                              ; 5 !!!
                SHL     EAX,10H
                MOV     EDX,DWORD PTR DESCR_SEG_ADR_SOED.BASE_L
                AND     EDX,0FFFFFFH                    ; 3 ������ ���� ��� SEG_ADR_SOED
                ADD     EAX,EDX                         ; 2b0000+50000 = 300000h - ��砫� ᢮������� ����

                ; === ����� ᥣ����� ࠧ��頥� ��᫥����⥫쭮
                        ;��������� SEG_SOED
                MOV     ECX,EAX                         ; � ecx ��砫� ᢮������� ���� - 300000h
                MOV     DESCR_SEG_SOED.BASE_L,AX
                SHR     EAX,10H
                MOV     DESCR_SEG_SOED.BASE_M,AL
                MOV     DESCR_SEG_SOED.BASE_H,AH        ; ���� SEG_SOED
                MOV     EAX, _SOED_MAX * _SOED_SIZE     ; 134000h !
                ADD     ECX,EAX
                SHR     EAX,12                          ;������� 4K
                DEC     EAX
                MOV     DESCR_SEG_SOED.LIM,AX
                SHR     EAX,10H
                MOV     DL,DESCR_SEG_SOED.ATTR_2
                AND     DL,0F0H
                OR      DL,AL                           ;������ ������
                OR      DL,80H                          ;������ �� 4K
                MOV     DESCR_SEG_SOED.ATTR_2,DL
                        ;� ECX-������ ���������� ����� - 434000h

                        ;��������� SEG_KONF
                MOV     EAX,ECX
                MOV     DESCR_SEG_KONF.BASE_L,AX
                SHR     EAX,10H
                MOV     DESCR_SEG_KONF.BASE_M,AL
                MOV     DESCR_SEG_KONF.BASE_H,AH        ;����� SEG_KONF
                MOV     EAX,DLIN_KONF                   ;������ SEG_KONF 150000h
                ADD     ECX,EAX
IF _MSP EQ TRUE
                MOV     DLIN_KONFIG,EAX
ENDIF
                SHR     EAX,12                          ;������� 4K
                DEC     EAX
                MOV     DESCR_SEG_KONF.LIM,AX
                SHR     EAX,10H
                MOV     DL,DESCR_SEG_KONF.ATTR_2
                AND     DL,0F0H
                OR      DL,AL                           ;������ ������
                OR      DL,80H                          ;������ �� 4K
                MOV     DESCR_SEG_KONF.ATTR_2,DL
                        ;� ECX-������ ���������� ����� - 5a0000h
                ; ---------------------------------------
                ; ---------------------------------------
                ; --- ��������� MRALEX ��� ����� ᯨ᪮�
                MOV     EAX,ECX
                ;��������� SEG_BLCK_LST1
                MOV     DESCR_SEG_BLCK_LST1.BASE_L,AX        ; ��।���� ����
                SHR     EAX,10H
                MOV     DESCR_SEG_BLCK_LST1.BASE_M,AL        ; ��� ��ࢮ�� ᥣ����
                MOV     DESCR_SEG_BLCK_LST1.BASE_H,AH        ; �୮�� ᯨ᪠
                MOV     EAX, 80000h                          ; ��� ����� 512�� (��⠫�� ��ࠬ���� 㦥 ���� � PEREh_dn.inc)
                ADD     ECX,EAX
                        ;� ECX-������ ���������� �����
                MOV     EAX,ECX
                ;��������� SEG_BLCK_LST2
                MOV     DESCR_SEG_BLCK_LST2.BASE_L,AX        ; ��।���� ����
                SHR     EAX,10H
                MOV     DESCR_SEG_BLCK_LST2.BASE_M,AL        ; ��� ��ࢮ�� ᥣ����
                MOV     DESCR_SEG_BLCK_LST2.BASE_H,AH        ; �୮�� ᯨ᪠
                MOV     EAX, 80000h                          ; ��� ����� 512�� (��⠫�� ��ࠬ���� 㦥 ���� � PEREh_dn.inc)
                ADD     ECX,EAX
                ; --- MRALEX ����� ������������
                ; ------------------------------------
                        ;� ECX-������ ���������� �����
                MOV     WORD PTR PDESCR,DESCR_SIZE-1    ;\
                XOR     EAX,EAX
                MOV     AL,DESCR_SEG_AX.BASE_M          ;\�����⮢�� � ����㧪� ⠡-
                SHL     EAX,10H
                MOV     AX,DESCR_SEG_AX.BASE_L          ;|
                XOR     EDX,EDX
                MOV     DX,OFFSET DESCR_NULL
                ADD     EAX,EDX
                MOV     WORD PTR PDESCR[2],AX           ;|
                SHR     EAX,10H
                MOV     BYTE PTR PDESCR[4],AL           ;/���� ��������� ���ਯ�஢
                MOV     AL,DESCR_SEG_AX.BASE_H          ;|
                MOV     BYTE PTR PDESCR[5],AL           ;|
                LGDT    PDESCR                          ;/

                ;MOV     BYTE PTR ES:[RG_REG_KOMM],0           ;���� �������� MVT ���������஢�� - ES ���ந��樠����஢��

                CLI                                     ;\
                MOV     AL,80h                          ;|����� �������� ���뢠���
                OUT     70h,AL                          ;/� NMI
IF _MSP NE TRUE
                in      al, 21h
                mov     DOS_mask_0, al
                in      al, 0a1h
                mov     DOS_mask_1, al
ENDIF
                MOV     AL,0FFh                         ;\
                OUT     21h,AL                          ;|��᪨஢���� ���뢠���
                OUT     0A1h,AL                         ;/

                MOV     AL,0D1h                         ;\
                OUT     64h,AL                          ;|����⨥ ����� A20
                MOV     AL,0DFh                         ;|
                OUT     60h,AL                          ;/


                MOV     WORD PTR PDESCR,IDT_SIZE-1      ;\
                MOV     AH,DESCR_SEG_AX.BASE_H          ;|
                MOV     AL,DESCR_SEG_AX.BASE_M          ;|
                SHL     EAX,16                          ;|
                MOV     AX,DESCR_SEG_AX.BASE_L          ;>�����⮢�� � ����㧪� ⠡-
                MOV     EDX,DESCR_SIZE                  ;|���� ���ਯ�஢ ���뢠���
                ADD     EAX,EDX                         ;|
                XOR     EDX,EDX
                MOV     DX,OFFSET DESCR_NULL
                ADD     EAX,EDX
                MOV     DWORD PTR PDESCR[2],EAX         ;|
                LIDT    PDESCR                          ;/;
;
                MOV     AL,11h                          ;\
                OUT     20h,AL                          ;|
                MOV     AL,20h                          ;|
                OUT     21h,AL                          ;|
                MOV     AL,4                            ;|
                OUT     21h,AL                          ;|
                MOV     AL,1                            ;|
                OUT     21h,AL                          ;\�ணࠬ��஢���� ����஫-
                MOV     AL,11h                          ;/��஢ ���뢠���
                OUT     0A0h,AL                         ;|
                MOV     AL,28h                          ;|
                OUT     0A1h,AL                         ;|
                MOV     AL,2                            ;|
                OUT     0A1h,AL                         ;|
                MOV     AL,1                            ;|
                OUT     0A1h,AL                         ;/

                MOV     EAX,CR0                          ;�⥭�� ����� ����ﭨ� ��設�
                OR      AX,1                            ;<<< ���饭�� ०�� >>>
                MOV     CR0,EAX                          ;������ ����� ����ﭨ� ��設�
;
                DB      0EAh                            ;\
                DW      OFFSET CONTINUE                 ;|JMP FAR CS:CONTINUE
                DW      120                             ;/
CONTINUE:
        cli
                MOV     SS_SEG, SS
                MOV     AX, 128
                MOV     SS, AX
                MOV     AX, 8
                MOV     SS:SEG_AX, AX
                MOV     AX, 16
                MOV     SS:SEG_FS, AX

                MOV     AX, 168
                MOV     SS:SEG_PCM, AX
                MOV     AX,24
                MOV     SS:SEG_ST, AX
                MOV     AX,32
                MOV     SS:SEG_GS, AX
                MOV     AX,40
                MOV     SS:SEG_OBJ, AX
                MOV     AX,48
                MOV     SS:SEG_GS8, AX
                MOV     AX,48
                MOV     SS:SEG_BABA, AX
                MOV     AX,56
                MOV     SS:SEG_TF, AX
                MOV     AX,64
                MOV     SS:SEG_KONF, AX
IF _MSP EQ TRUE
                MOV     AX,72
                MOV     SS:SEG_ULOG,AX
ELSE
                MOV     AX,416
                MOV     SS:SEG_ULOG,AX
ENDIF
                MOV     AX,80
                MOV     SS:SEG_NLOG,AX
                MOV     AX,96
                MOV     SS:SEG_SOED,AX
                MOV     AX,104
                MOV     SS:SEG_BUS,AX
                MOV     AX,112
                MOV     SS:SEG_NULL,AX
IF _MSP EQ TRUE
                MOV     AX ,152
;                MOV     SS:SEG_KONF_UP,AX
                MOV     AX,160
                MOV     SS:SEG_C000,AX
                MOV     AX,176
                MOV     SS:SEG_VIZ,AX
ENDIF
                MOV     AX,184
                MOV     SS:SEG_ADR_SOED,AX
IF _MSP EQ TRUE
                MOV     AX,208
                MOV     SS:SEG_SYS,AX
ENDIF
                MOV     AX,216
                MOV     SS:SEG_AXD,AX
IF _MSP EQ TRUE
                MOV     AX,224
                MOV     SS:SEG_DOP,AX
ENDIF
                MOV     AX,232
                MOV     SS:SEG_COM,AX
		MOV	AX,248
		MOV	SS:SEG_ZAG,AX
IF _MSP EQ TRUE
                mov     ax, 328
ELSE
                mov     ax, 368
ENDIF
                mov     ss:SEG_SORM, ax
IF _MSP NE TRUE
                mov     ax, 376
                mov     ss:SEG_MAX_II, ax
ENDIF
                MOV     DS,SS:SEG_AX
                MOV     FS,SS:SEG_SOED
		MOV	GS,SS:SEG_ST
		MOV	ES,SS:SEG_ST

                MOV     AL,0                            ;|����襭�� ��������
                OUT     70h,AL                          ;/���뢠��� � NMI

                ENDM
; ���� ��� StarTeam xx
IF _MSP NE TRUE
;//*****************************************************************************
;// ��楤�� ���樠������� ��I ���ன�⢮
;//*****************************************************************************

InitPCI_4       PROC    NEAR

pushad
push	ds fs gs

                push    ES
                push    DS
                pop     ES

		mov     PCI_4_InitFlag, 0
;**********************
                lea     si, PCI_4
;//***************************************
;// ��楤�� ���樠������� ��I ���ன�⢮
;// �����: <CF>=0 - ��ଠ�쭮� �����襭��
;//        <CF>=1 - �訡��:
;//        <AX> = 0 - ������� �� ������������ PCI BIOS!
;//        <AX> = 1 - �� ���������� ��������� PCI!
;//        <AX> = 2   ��������� ����� ��������!
;//***************************************
;�஢�ઠ ������ PCI BIOS
                mov     ax,pci_bios_prezent
                push    si
                int     1Ah
                pop     si
                jc      PciBiosNotFound
                cmp     ah,0
                jne     PciBiosNotFound
                cmp     edx,signatura_pci
                jne     PciBiosNotFound

                mov     ax, find_pci_device
                mov     cx,[si].DeviceId
                mov     dx,[si].VendorId
                push    si
                xor     si,si
                int     1Ah
                pop     si
                jc      DeviceNotFound

                mov     [si].BusNumber,bh
                mov     [si].DeviceNumber,bl
;��⠥� ���䨣���� ���ன�⢠ PCI
                mov     ax,read_config_dword
                mov     di,4
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     [si].CommandPCI,cx
                shr     ecx,16
                mov     [si].StatusPCI,cx
                mov     ax,read_config_dword
                mov     di,8
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     [si].RevisionId,cl
                shr     ecx,8
                mov     [si].ClassCode,ecx
                mov     ax,read_config_dword
                mov     di,0Ch
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     [si].CashLine,cl
                mov     [si].Lat_Timer,ch
                shr     ecx,16
                mov     [si].HeaderType,cl
                mov     [si].Bist,ch
                mov     ax,read_config_dword
                mov     di,10h
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     [si].Baddr0,ecx
                ; ��।���� ࠧ��� ����� ���ன�⢠
                mov     ax,write_config_dword
                mov     ecx, 0FFFFFFFFh
                mov     di,10h
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     ax,read_config_dword
                mov     di,10h
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                and     ecx, 0FFFFFFF8h
                neg     ecx
                mov     [si].LenMemory, ecx
                mov     ax,write_config_dword
                mov     ecx, [si].Baddr0
                mov     di,10h
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     ax,read_config_dword
                mov     di,14h
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     [si].Baddr1,ecx
                btr     cx,0
                add     cx,8
                mov     [si].addr_led,cx
                ; ��।���� ࠧ��� ������ ����� ���⮢ �����/�뢮��
                mov     ax,write_config_dword
                mov     ecx, 0FFFFFFFFh
                mov     di,14h
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     ax,read_config_dword
                mov     di,14h
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                and     ecx, 0FFFFFFFCh
                neg     ecx
                and     ecx, 0FFFFh
                mov     [si].LenPortArray, ecx
                mov     ax,write_config_dword
                mov     ecx, [si].Baddr1
                mov     di,14h
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     ax,read_config_dword
                mov     di,18h
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     [si].Baddr2,ecx

                mov     ax,read_config_dword
                mov     di,1Ch
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     [si].Baddr3,ecx

                mov     ax,read_config_dword
                mov     di,20h
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     [si].Baddr4,ecx

                mov     ax,read_config_dword
                mov     di,20h
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     [si].Baddr5,ecx

                mov     ax,read_config_dword
                mov     di,28h
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     [si].CIS_PTR,ecx

                mov     ax,read_config_dword
                mov     di,2Ch
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     [si].SVendorId,cx
                shr     ecx,16
                mov     [si].SDeviceId,cx

                mov     ax,read_config_dword
                mov     di,30h
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     [si].EXP_ROM,ecx

                mov     ax,read_config_dword
                mov     di,3Ch
                push    si
                int     1Ah
                pop     si
                jc      BadRegistrNumber

                mov     [si].Int_Line,cl
                mov     [si].Int_Pin,ch
                shr     ecx,16
                mov     [si].Min_Gnt,cl
                mov     [si].Max_Lat,ch
                clc
                jmp     @@End

PciBiosNotFound:
                stc
                mov     ax, 0
                jmp     @@End
DeviceNotFound:
                stc
                mov     ax, 1
                jmp     @@End
BadRegistrNumber:
                stc
                mov     ax, 2
@@End:

;***********************
                MOV     EAX,PCI_4.Baddr0
                ADD     EAX,0D8000h
                MOV     DESCR_SEG_BABA.BASE_L,AX          ;/
                SHR     EAX,16
                MOV     DESCR_SEG_BABA.BASE_M,AL          ;/
                MOV     DESCR_SEG_BABA.BASE_H,AH          ;/

                MOV     EAX,PCI_4.Baddr0
                ADD     EAX,0D0000h
                MOV     DESCR_SEG_GS.BASE_L,AX          ;/
                SHR     EAX,16
                MOV     DESCR_SEG_GS.BASE_M,AL          ;/
                MOV     DESCR_SEG_GS.BASE_H,AH          ;/

                MOV     EAX,PCI_4.Baddr0
                ADD     EAX,0B0000h
                MOV     DESCR_SEG_flash_1.BASE_L,AX          ;/
                SHR     EAX,16
                MOV     DESCR_SEG_flash_1.BASE_M,AL          ;/
                MOV     DESCR_SEG_flash_1.BASE_H,AH          ;/

		MOV     EAX,PCI_4.Baddr0
                ADD     EAX,0C8000h
                MOV     DESCR_SEG_ST.BASE_L,AX          ;/
                SHR     EAX,16
                MOV     DESCR_SEG_ST.BASE_M,AL          ;/
                MOV     DESCR_SEG_ST.BASE_H,AH          ;/

		MOV     EAX,PCI_4.Baddr0
                MOV     DESCR_SEG_ALL.BASE_L,AX          ;/
                SHR     EAX,16
                MOV     DESCR_SEG_ALL.BASE_M,AL          ;/
                MOV     DESCR_SEG_ALL.BASE_H,AH          ;/

;***********************

                movzx   bx,pci_4.int_line

                shl     bx,3
                lea     di, INTIR11
                mov     IRQ_0[bx].offsi_l,di
                mov     PCI_4_InitFlag, 1

@@Exit:
                pop     ES
                pop     gs fs ds
                popad
                retn
InitPCI_4       ENDP

if 01;;;
InitPCI_8       PROC    NEAR

                pushad
                push    ds fs gs
                push    ES
                push    DS
                pop     ES

		mov     PCI_8_InitFlag, 0
;**********************
                lea     si, PCI_8
;//***************************************
;// ��楤�� ���樠������� ��I ���ன�⢮
;// �����: <CF>=0 - ��ଠ�쭮� �����襭��
;//        <CF>=1 - �訡��:
;//        <AX> = 0 - ������� �� ������������ PCI BIOS!
;//        <AX> = 1 - �� ���������� ��������� PCI!
;//        <AX> = 2   ��������� ����� ��������!
;//***************************************
;�஢�ઠ ������ PCI BIOS
                mov     ax,pci_bios_prezent
                push    si
                int     1Ah
                pop     si
                jc      @@PciBiosNotFound
                cmp     ah,0
                jne     @@PciBiosNotFound
                cmp     edx,signatura_pci
                jne     @@PciBiosNotFound

                mov     ax, find_pci_device
                mov     cx,[si].DeviceId
                mov     dx,[si].VendorId
                push    si
                xor     si,si
                int     1Ah
                pop     si
                jc      @@DeviceNotFound

                mov     [si].BusNumber,bh
                mov     [si].DeviceNumber,bl
;��⠥� ���䨣���� ���ன�⢠ PCI
                mov     ax,read_config_dword
                mov     di,4
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     [si].CommandPCI,cx
                shr     ecx,16
                mov     [si].StatusPCI,cx
                mov     ax,read_config_dword
                mov     di,8
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     [si].RevisionId,cl
                shr     ecx,8
                mov     [si].ClassCode,ecx
                mov     ax,read_config_dword
                mov     di,0Ch
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     [si].CashLine,cl
                mov     [si].Lat_Timer,ch
                shr     ecx,16
                mov     [si].HeaderType,cl
                mov     [si].Bist,ch
                mov     ax,read_config_dword
                mov     di,10h
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     [si].Baddr0,ecx
                ; ��।���� ࠧ��� ����� ���ன�⢠
                mov     ax,write_config_dword
                mov     ecx, 0FFFFFFFFh
                mov     di,10h
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     ax,read_config_dword
                mov     di,10h
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                and     ecx, 0FFFFFFF8h
                neg     ecx
                mov     [si].LenMemory, ecx
                mov     ax,write_config_dword
                mov     ecx, [si].Baddr0
                mov     di,10h
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     ax,read_config_dword
                mov     di,14h
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     [si].Baddr1,ecx
                btr     cx,0
                add     cx,8
                mov     [si].addr_led,cx
                ; ��।���� ࠧ��� ������ ����� ���⮢ �����/�뢮��
                mov     ax,write_config_dword
                mov     ecx, 0FFFFFFFFh
                mov     di,14h
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     ax,read_config_dword
                mov     di,14h
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                and     ecx, 0FFFFFFFCh
                neg     ecx
                and     ecx, 0FFFFh
                mov     [si].LenPortArray, ecx
                mov     ax,write_config_dword
                mov     ecx, [si].Baddr1
                mov     di,14h
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     ax,read_config_dword
                mov     di,18h
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     [si].Baddr2,ecx

                mov     ax,read_config_dword
                mov     di,1Ch
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     [si].Baddr3,ecx

                mov     ax,read_config_dword
                mov     di,20h
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     [si].Baddr4,ecx

                mov     ax,read_config_dword
                mov     di,20h
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     [si].Baddr5,ecx

                mov     ax,read_config_dword
                mov     di,28h
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     [si].CIS_PTR,ecx

                mov     ax,read_config_dword
                mov     di,2Ch
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     [si].SVendorId,cx
                shr     ecx,16
                mov     [si].SDeviceId,cx

                mov     ax,read_config_dword
                mov     di,30h
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     [si].EXP_ROM,ecx

                mov     ax,read_config_dword
                mov     di,3Ch
                push    si
                int     1Ah
                pop     si
                jc      @@BadRegistrNumber

                mov     [si].Int_Line,cl
                mov     [si].Int_Pin,ch
                shr     ecx,16
                mov     [si].Min_Gnt,cl
                mov     [si].Max_Lat,ch
                clc
                jmp     @@End

@@PciBiosNotFound:
                stc
                mov     ax, 0
                jmp     @@End
@@DeviceNotFound:
                stc
                mov     ax, 1
                jmp     @@End
@@BadRegistrNumber:
                stc
                mov     ax, 2
@@End:

;***********************
		MOV     EAX,PCI_8.Baddr0
                ADD     EAX,0D0000h
                MOV     DESCR_SEG_MAX_II.BASE_L,AX          ;/
                SHR     EAX,16
                MOV     DESCR_SEG_MAX_II.BASE_M,AL          ;/
                MOV     DESCR_SEG_MAX_II.BASE_H,AH          ;/
;***********************

                mov     PCI_8_InitFlag, 1

@@Exit:
                pop     ES
                pop     gs fs ds
                popad
                retn
InitPCI_8       ENDP
endif
ENDIF

