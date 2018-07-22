TITLE	HDD
.486P
LOCALS $$
PAGE	60,130
;
;===============================================================================
; ���樠������ 䠩����� ��⥬�
;
; InitHDD PROC    C FAR
;===============================================================================

;===============================================================================
; ������ �������騩 䠩�
;
; � DS:EDX 㪠��⥫� �� ��� 䠩�� (ASCIZ-��ப�)
; ��室 ��� �訡��:
; CF = 0, EAX = �����䨪��� 䠩��
; ��室 � �訡���:
; CF = 1, EAX - �� ��।���
;
; FileOpen        PROC    C FAR
;===============================================================================

;===============================================================================
; ������� 䠩�
;
; EAX = �����䨪��� 䠩��
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
;
; FileErase       PROC    C FAR
;===============================================================================

;===============================================================================
; ������� � ������ 䠩�
;
; � DS:EDX 㪠��⥫� �� ��� 䠩�� (ASCIZ-��ப�)
; ��室 ��� �訡��:
; CF = 0, EAX = �����䨪��� 䠩��
; ��室 � �訡���:
; CF = 1, EAX - �� ��।���
;
; FileCreate      PROC    C FAR
;===============================================================================

;===============================================================================
; ������� � ������ 䠩� ��������� ࠧ���
;
; � DS:EDX 㪠��⥫� �� ��� 䠩�� (ASCIZ-��ப�)
; � ECX ࠧ��� 䠩��
; ��室 ��� �訡��:
; CF = 0, EAX = �����䨪��� 䠩��
; ��室 � �訡���:
; CF = 1, EAX - �� ��।���
;
; FileCreateSize          PROC    C FAR
;===============================================================================

;===============================================================================
; ������� � ������ ���⮩ 䠩� (���������� ��ﬨ) ��������� ࠧ���
;
; � DS:EDX 㪠��⥫� �� ��� 䠩�� (ASCIZ-��ப�)
; � ECX ࠧ��� 䠩��
; ��室 ��� �訡��:
; CF = 0, EAX = �����䨪��� 䠩��
; ��室 � �訡���:
; CF = 1, EAX - �� ��।���
;
; FileCreateEmpty         PROC    C FAR
;===============================================================================

;===============================================================================
; ������� 䠩�
;
; EAX - �����䨪��� 䠩��
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
;
; FileClose       PROC    C FAR
;===============================================================================

;===============================================================================
; ������� ࠧ��� 䠩��
;
; EAX - �����䨪��� 䠩��
; ��室 ��� �訡��:
; CF = 0
; ECX - ࠧ��� 䠩��
; ��室 � �訡���:
; CF = 1
;
; FileSize        PROC    C FAR
;===============================================================================

;===============================================================================
; ��⠭����� 㪠��⥫� �⥭��/����� � 䠩��
;
; EAX - �����䨪��� 䠩��
; ECX - ����ﭨ� �� ��砫� 䠩��
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
;
; FileSeek        PROC    C FAR
;===============================================================================

;===============================================================================
; ������� 㪠��⥫� �⥭��/����� � 䠩��
;
; EAX - �����䨪��� 䠩��
; ��室 ��� �訡��:
; CF = 0
; ECX - 㪠��⥫� �⥭��/����� � 䠩��
; ��室 � �訡���:
; CF = 1
;
; FilePos         PROC    C FAR
;===============================================================================

;===============================================================================
; �⥭�� �� 䠩��
;
; EAX - �����䨪��� 䠩��
; DS:EDX - 㪠��⥫� �� ���� ��� ������
; ECX - �᫮ ����
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
;
; FileRead        PROC    C FAR
;===============================================================================

;===============================================================================
; ������ � 䠩�
;
; EAX - �����䨪��� 䠩��
; DS:EDX - 㪠��⥫� �� ���� � ����묨
; ECX - �᫮ ����
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
;
; FileWrite       PROC    C FAR
;===============================================================================

;===============================================================================
; ������� 䠩� �� �����
;
; � DS:EDX 㪠��⥫� �� ��� 䠩�� (ASCIZ-��ப�)
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
;
; FileEraseName   PROC    C FAR
;===============================================================================

;===============================================================================
; ��२�������� 䠩�
;
; EAX = �����䨪��� 䠩��
; � DS:EDX 㪠��⥫� �� ��� 䠩�� (ASCIZ-��ப�)
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
;
; FileRename      PROC    C FAR
;===============================================================================

;

HDDBasePortAddr equ     01F0h
HDDNumber       equ     0
RootDirSize     equ     32
RootDirItems    equ     512
MaxFiles        equ     8

TDirItem        struc
	DIR_Name	        db	11 DUP (?)	;���⪮� ��� 䠩��
	DIR_Attr	        db	?		;��ਡ��� 䠩��
	DIR_NTRes               db	?		;��१�ࢨ஢��� ��� (������ ᮤ�ঠ�� ���祭�� 0)
	DIR_CrtTimeTenth        db	?        	;����, �����饥 �६� ᮧ����� 䠩�� (����⪨ �����ᥪ㭤)
                                                        ;���祭�� ���� ����� ��室����� � �।���� �� 0 �� 199
	DIR_CrtTime             dw	?        	;�६� ᮧ����� 䠩��
	DIR_CrtDate             dw	?        	;��� ᮧ����� 䠩��
	DIR_LstAccDate          dw	?        	;��� ��᫥����� ���饭�� � 䠩��
	DIR_FstClusHI           dw	?        	;���襥 ᫮�� ����� ��ࢮ�� ������ 䠩��
                                                        ;� ��⥬�� FAT12 � FAT16 ���� ��१�ࢨ஢��� � ᮤ�ন� 0
	DIR_WrtTime             dw	?        	;�६� �믮������ �������� ����樨 ����� � 䠩�
	DIR_WrtDate             dw	?        	;��� �믮������ �������� ����樨 ����� � 䠩�
	DIR_FstClusLO           dw	?        	;����襥 ᫮�� ����� ��ࢮ�� ������ 䠩��
	DIR_FileSize            dd	?        	;������ 䠩�� � ����� (32-ࠧ�來�� �᫮)
TDirItem	ends

;
TFileRec        struc
        DirItem                 dw      ?               ;����� ����� ��⠫���
        FileRezerv              dw      ?               ;�����
        FilePosition            dd      ?               ;�����⥫� �⥭��/����� � 䠩��
TFileRec        ends

;	***** ������ � SEG_HDD_HEAP *****
VarFSTop = 0 ;���樠������ ���設� "����᪮� ���"
;
VAR_FS  macro   VarName,VarSize
	VarName = VarFSTop
	VarFSTop = VarFSTop+VarSIze
endm

VAR_FS  ATAFeatures,1
VAR_FS  ATASectorCount,1;
VAR_FS  ATASectorNumber,1;
VAR_FS  ATACylinder,2;
VAR_FS  ATAHead,1;
VAR_FS  ATAAdressMode,1;
VAR_FS  ATACommand,1
VAR_FS  HDDErrorCode,1;
VAR_FS  SectorAddress,4;
VAR_FS  SectorDataBuffer,512
VAR_FS  PriDOS_StartSector,4;
VAR_FS  SectorsInCluster,2;
VAR_FS  RSects,4;
VAR_FS  FATsOnDisk,2;
VAR_FS  FATSize,2;
VAR_FS  HiddenSectors,4;
VAR_FS  FATStartSect,4;
VAR_FS  RootDirStartSect,4
VAR_FS  RootDirBuffer,512*32
VAR_FS  FATBuffer,128*1024;
VAR_FS  TmpDirName,11
VAR_FS  Files,MaxFiles*8

;
;STACK_SEG	SEGMENT PARA STACK USE16 'STACK'
EXTRN           SEG_HDD_HEAP:WORD
;STACK_SEG	ENDS

;===============================================================================
; ������� ���� 䠩����� ��⥬�
;===============================================================================

HDD_CODE        SEGMENT PARA PUBLIC USE16 'CODE'
                ASSUME CS:HDD_CODE,DS:NOTHING,SS:NOTHING;STACK_SEG
                
public          HDD_SIZE

SendCommandToHDD        PROC        NEAR
        pushad
        cmp     BYTE PTR GS:[ATAAdressMode],1
        ja      $$Error2
;        mov     bx,[ChannelNumber]
;        cmp     bx,1
;        jb      $$Error3
;        cmp     bx,4
;        ja      $$Error3
;        dec     bx
;        shl     bx,1
;        mov     ax,[BX+StStandardHDDBases]
        mov     dx,HDDBasePortAddr
        add     dx,0206h
        mov     al,1010b
        out     dx,al
        mov     dx,HDDBasePortAddr+7
$$WaitNotBSY:
        in      al,dx
        test    al,80h
        jnz     $$WaitNotBSY
        
        mov     dx,HDDBasePortAddr+6
        mov     al,HDDNumber
        shl     al,4
        or      al,10100000b
        out     dx,al
        inc     dx

$$WaitHDReady:
        in      al,dx
        test    al,80h
        jnz     $$WaitHDReady
        test    al,40h
        jz      $$WaitHDReady

        mov     dx,HDDBasePortAddr+1
        mov     al,GS:[ATAFeatures]
        out     dx,al
        inc     dx
        mov     al,GS:[ATASectorCount]
        out     dx,al
        inc     dx
        mov     al,GS:[ATASectorNumber]
        out     dx,al
        inc     dx
        mov     ax,GS:[ATACylinder]
        out     dx,al
        inc     dx
        mov     al,ah
        out     dx,al
        inc     dx
        mov     al,HDDNumber
        shl     al,4
        cmp     BYTE PTR GS:[ATAHead],0Fh
        ja      $$Error5
        or      al,GS:[ATAHead]
        or      al,10100000b
        mov     ah,GS:[ATAAdressMode]
        shl     ah,6
        or      al,ah
        out     dx,al
        inc     dx
        mov     al,GS:[ATACommand]
        out     dx,al
        mov     BYTE PTR GS:[HDDErrorCode],0
        jmp     SHORT $$End
$$Error1:                
        mov     BYTE PTR GS:[HDDErrorCode],1
        jmp     SHORT $$End
$$Error2:                
        mov     BYTE PTR GS:[HDDErrorCode],2
        jmp     SHORT $$End
$$Error3:                
        mov     BYTE PTR GS:[HDDErrorCode],3
        jmp     SHORT $$End
$$Error4:                
        mov     BYTE PTR GS:[HDDErrorCode],4
        jmp     SHORT $$End
$$Error5:                
        mov     BYTE PTR GS:[HDDErrorCode],5

$$End:
        popad
        ret
SendCommandToHDD        ENDP

;
ReadHDDSector   PROC    NEAR
        pushad
        mov     BYTE PTR GS:[ATAAdressMode],1
        mov     BYTE PTR GS:[ATAFeatures],0
        mov     BYTE PTR GS:[ATASectorCount],1
        mov     eax,GS:SectorAddress
        mov     GS:[ATASectorNumber],eax
        mov     BYTE PTR GS:[ATACommand],20h
        call    SendCommandToHDD
        cmp     BYTE PTR GS:[HDDErrorCode],0
        jne     $$End
        
        mov     dx,HDDBasePortAddr+7
$$WaitCompleet:
        in      al,dx
        test    al,80h
        jnz     $$WaitCompleet
        test    al,08h
        jz      $$WaitCompleet
        
        push    ES
        mov     ES,SS:SEG_HDD_HEAP
        mov     edi,SectorDataBuffer
        mov     dx,HDDBasePortAddr
        mov     ecx,256
        db      67h
        rep     insw
        pop     ES
        jmp     $$End
$$Error1:
        mov     BYTE PTR GS:[HDDErrorCode],1
        jmp     SHORT $$End
$$End:
        popad
        ret
ReadHDDSector   ENDP

;
WriteHDDSector   PROC    NEAR
        pushad
        mov     BYTE PTR GS:[ATAAdressMode],1
        mov     BYTE PTR GS:[ATAFeatures],0
        mov     BYTE PTR GS:[ATASectorCount],1
        mov     eax,GS:SectorAddress
        mov     GS:[ATASectorNumber],eax
        mov     BYTE PTR GS:[ATACommand],30h
        call    SendCommandToHDD
        cmp     BYTE PTR GS:[HDDErrorCode],0
        jne     $$End
        
        mov     dx,HDDBasePortAddr+7
$$WaitCompleet:
        in      al,dx
        test    al,80h
        jnz     $$WaitCompleet
        test    al,08h
        jz      $$WaitCompleet
        
        push    DS
        mov     DS,SS:SEG_HDD_HEAP
        mov     esi,SectorDataBuffer
        mov     dx,HDDBasePortAddr
        mov     ecx,256
        db      67h
        rep     outsw
        pop     DS
        jmp     $$End
$$Error1:
        mov     BYTE PTR GS:[HDDErrorCode],1
        jmp     SHORT $$End
$$End:
        popad
        ret
WriteHDDSector   ENDP

; DS:ESI - Buffer
WriteHDDSectorBuf       PROC    NEAR
        pushad
        mov     BYTE PTR GS:[ATAAdressMode],1
        mov     BYTE PTR GS:[ATAFeatures],0
        mov     BYTE PTR GS:[ATASectorCount],1
        mov     eax,GS:SectorAddress
        mov     GS:[ATASectorNumber],eax
        mov     BYTE PTR GS:[ATACommand],30h
        call    SendCommandToHDD
        cmp     BYTE PTR GS:[HDDErrorCode],0
        jne     $$End
        
        mov     dx,HDDBasePortAddr+7
$$WaitCompleet:
        in      al,dx
        test    al,80h
        jnz     $$WaitCompleet
        test    al,08h
        jz      $$WaitCompleet
        
        mov     dx,HDDBasePortAddr
        mov     ecx,256
        db      67h
        rep     outsw
        jmp     $$End
$$Error1:
        mov     BYTE PTR GS:[HDDErrorCode],1
        jmp     SHORT $$End
$$End:
        popad
        ret
WriteHDDSectorBuf       ENDP
;
;
ReadHDD_ID      PROC      NEAR
        pushad
        mov     BYTE PTR GS:[ATAAdressMode],0
        mov     BYTE PTR GS:[ATAFeatures],0
        mov     BYTE PTR GS:[ATAHead],0
        mov     BYTE PTR GS:[ATACommand],0ECh
        call    SendCommandToHDD
        cmp     BYTE PTR GS:[HDDErrorCode],0
        jne     $$End

        mov     dx,HDDBasePortAddr+7
$$WaitCompleet:
        in      al,dx
        test    al,80h
        jnz     $$WaitCompleet
        test    al,1
        jnz     $$Error6
        test    al,08h
        jz      $$WaitCompleet

        push    es
        mov     es,SS:SEG_HDD_HEAP
        mov     edi,SectorDataBuffer
        mov     dx,HDDBasePortAddr
        mov     ecx,256
        db      67h
        rep     insw
        pop     es
        jmp     SHORT $$End
        
$$Error1:                
        mov     BYTE PTR GS:[HDDErrorCode],1
        jmp     SHORT $$End
$$Error6:                
        mov     BYTE PTR GS:[HDDErrorCode],6
        
$$End:
        popad
        ret
ReadHDD_ID      ENDP

;
Mount   PROC    C NEAR
        uses    eax,ecx,edx,edi,esi
;        mov     BYTE PTR GS:[HDDNumber],0
        call    ReadHDD_ID
        cmp     BYTE PTR GS:[HDDErrorCode],0
        jne     $$Error1
        
        mov     DWORD PTR GS:SectorAddress,0
        call    ReadHDDSector
        
        mov     eax,GS:[SectorDataBuffer+1BEh+8]
        mov     GS:PriDOS_StartSector,eax
        mov     GS:SectorAddress,eax
        call    ReadHDDSector
        
        movzx   ax,GS:SectorDataBuffer[0Dh]
        mov     GS:SectorsInCluster,ax
        movzx   eax,WORD PTR GS:SectorDataBuffer[0Eh]
        mov     GS:RSects,eax
        movzx   ax,GS:SectorDataBuffer[10h]
        mov     GS:FATsOnDisk,ax
        mov     ax,GS:SectorDataBuffer[16h]
        mov     GS:FATSize,ax
        mov     eax,GS:SectorDataBuffer[1Ch]
        mov     GS:HiddenSectors,eax

        mov     eax,GS:PriDOS_StartSector
        add     eax,GS:RSects
        mov     GS:FATStartSect,eax
        mov     GS:SectorAddress,eax

        push    DS
        push    ES
        mov     DS,SS:SEG_HDD_HEAP
        mov     ES,SS:SEG_HDD_HEAP
        mov     edi,FATBuffer
        mov     dx,GS:[FATSize]
$$NextFATSector:        
        call    ReadHDDSector
        inc     DWORD PTR GS:SectorAddress
        mov     esi,SectorDataBuffer
        mov     ecx,512
        db      67h
        rep     movsb
        dec     dx
        jnz     SHORT $$NextFATSector
        
        movzx   eax,WORD PTR GS:FATSize
        movzx   edx,WORD PTR GS:FATsOnDisk
        mul     edx
        add     eax,GS:FATStartSect
        mov     GS:RootDirStartSect,eax
        mov     GS:SectorAddress,eax

        mov     edi,RootDirBuffer
        mov     dx,RootDirSize
$$NextRootSector:
        call    ReadHDDSector
        inc     DWORD PTR GS:SectorAddress
        mov     esi,SectorDataBuffer
        mov     ecx,512
        db      67h
        rep     movsb
        dec     dx
        jnz     SHORT $$NextRootSector
        pop     ES
        pop     DS
        clc
        jmp     SHORT $$End
$$Error1:
        stc
;        mov     BYTE PTR GS:[HDDErrorCode],1
;        jmp     SHORT $$End
$$End:
        ret
Mount   ENDP

; ���樠������ 䠩����� ��⥬�
InitHDD PROC    C FAR
        public  InitHDD

        uses    ES,GS,eax,ecx,edi

        mov     GS,SS:SEG_HDD_HEAP

        call    Mount
        jc      $$End
        mov     ecx,MaxFiles*8
        mov     edi,Files
        mov     ES,SS:SEG_HDD_HEAP
        mov     al,0FFh
        db      67h
        rep     stosb
        clc
$$End:
        ret
InitHDD ENDP

;
PrepareFileName PROC    NEAR
        push    ES
        pushad
        mov     ES,SS:SEG_HDD_HEAP
        mov     edi,TmpDirName
        mov     eax,20202020h
        mov     GS:[TmpDirName],eax
        mov     GS:[TmpDirName+4],eax
        mov     GS:[TmpDirName+8],ax
        mov     GS:[TmpDirName+10],al
        mov     cx,8
$$NameLoop:
        db      67h
        lodsb
        cmp     al,'.'
        je      SHORT $$GetExt
        or      al,al
        jz      SHORT $$End
        db      67h
        stosb
        loop    $$NameLoop
$$GetExt:
        mov     edi,TmpDirName+8
        mov     cx,3
        cmp     BYTE PTR DS:[esi],'.'
        jne     SHORT $$ExtLoop
        inc     si
$$ExtLoop:
        db      67h
        lodsb
        or      al,al
        jz      SHORT $$End
        db      67h
        stosb
        loop    $$ExtLoop
$$End:
        popad
        pop     ES
        ret
PrepareFileName ENDP

;
; ���� 䠩�� � ��୥��� ��४�ਨ
; � DS:EDX 㪠��⥫� �� ��� 䠩�� (ASCIZ-��ப�)
; ��室 ��� �訡��:
; CF = 0, EAX = ����� ����� ��⠫���
; ��室 � �訡���:
; CF = 1, EAX - �� ��।���
FileSearch      PROC    C NEAR
        uses    DS,ES,ecx,edi,esi

        mov     esi,edx
        call    PrepareFileName
        
        mov     DS,SS:SEG_HDD_HEAP
        mov     ES,SS:SEG_HDD_HEAP
        mov     esi,TmpDirName
        mov     edi,RootDirBuffer
        mov     cx,512
$$NextElement:
        push    ecx
        push    edi
        push    esi
        mov     ecx,11
        db      67h
        repe    cmpsb
        pop     esi
        pop     edi
        pop     ecx
        je      SHORT $$FileFound
        add     edi,32
        loop    $$NextElement
        stc
        jmp     SHORT $$End
$$FileFound:
        sub     edi,RootDirBuffer
        shr     edi,5
        mov     eax,edi
        clc
$$End:
        ret
FileSearch      ENDP

;
GetFileRec      PROC    C NEAR
        uses    ES,ecx
        mov     ES,SS:SEG_HDD_HEAP
        mov     ecx,MaxFiles
        xor     edi,edi
$$NextFileRec:
        cmp     WORD PTR GS:Files[edi*8][TFileRec.DirItem],0FFFFh
        je      $$FileRecFound
        inc     edi
        loop    $$NextFileRec
        stc
        jmp     SHORT $$End
$$FileRecFound:
        clc
$$End:
        ret
GetFileRec      ENDP

; ������ �������騩 䠩�
; � DS:EDX 㪠��⥫� �� ��� 䠩�� (ASCIZ-��ப�)
; ��室 ��� �訡��:
; CF = 0, EAX = �����䨪��� 䠩��
; ��室 � �訡���:
; CF = 1, EAX - �� ��।���
FileOpen        PROC    C FAR
        public  FileOpen
        call    _FileOpen
        ret
FileOpen        ENDP

_FileOpen       PROC    C NEAR
        uses    ES,GS,ecx,edi

        mov     GS,SS:SEG_HDD_HEAP

        call    GetFileRec
        jc      SHORT $$End
        call    FileSearch
        jc      SHORT $$End

        mov     GS:Files[edi*8][TFileRec.DirItem],ax
        mov     DWORD PTR GS:Files[edi*8][TFileRec.FilePosition],0
        mov     eax,edi
        clc
$$End:
        ret
_FileOpen       ENDP

; ���� ᢮����� ������ � FAT
; ��室 ��� �訡��:
; CF = 0, EAX = ����� ������
; ��室 � �訡���:
; CF = 1, EAX - �� ��।���
FindFreeCluster PROC    C NEAR
        uses    ES,ecx,edi,esi
        
        mov     ES,SS:SEG_HDD_HEAP
        mov     edi,FATBuffer
        movzx   ecx,WORD PTR GS:FATSize
        shl     ecx,9
        xor     ax,ax
        db      67h
        repne   scasw
        jne     SHORT $$Error1
        sub     edi,FATBuffer+2
        shr     edi,1
        mov     eax,edi
        clc
        jmp     SHORT $$End
$$Error1:
        stc
;        jmp     SHORT $$End
$$End:
        ret
FindFreeCluster ENDP

; ������� 䠩� �� ������ ����� ��⠫���
; EAX = ����� ����� ��⠫���
FileEraseDir    PROC    C NEAR
        uses    DS,eax,ebx,ecx,edx,edi,esi
        mov     DS,SS:SEG_HDD_HEAP
        
        mov     edi,eax
        shl     edi,5
        add     edi,RootDirBuffer

        movzx   eax,GS:[edi][TDirItem.DIR_FstClusLO]
        mov     ebx,eax

; Fix Dir
        mov     BYTE PTR GS:[edi][TDirItem.DIR_Name],0E5h
        cmp     BYTE PTR GS:[edi+32][TDirItem.DIR_Name],0
        jne     SHORT $$FixDir
        mov     WORD PTR GS:[edi][TDirItem.DIR_FstClusLO],0
$$FixDir:
        sub     edi,RootDirBuffer
        shr     edi,9
        mov     esi,edi
        add     edi,GS:RootDirStartSect
        mov     GS:SectorAddress,edi
        shl     esi,9
        add     esi,RootDirBuffer
        call    WriteHDDSectorBuf
        
$$NextCluster:
        mov     ecx,eax
        shr     ecx,8
        mov     edx,ebx
        shr     edx,8
        
        cmp     ecx,edx
        je      SHORT $$ClearCluster
        
        mov     esi,edx
        add     edx,GS:FATStartSect
        mov     GS:SectorAddress,edx
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf

$$ClearCluster:
        movzx   ebx,WORD PTR GS:FATBuffer[eax*2]
        mov     WORD PTR GS:FATBuffer[eax*2],0

        cmp     bx,0FFFFh
        je      SHORT $$ClearLastCluster
        xchg    eax,ebx
        jmp     SHORT $$NextCluster

$$ClearLastCluster:
;        shl     eax,1
;        shr     eax,9
        shr     eax,8
        mov     esi,eax
        add     eax,GS:FATStartSect
        mov     GS:SectorAddress,eax
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf

        ret
FileEraseDir    ENDP

; ������� 䠩�
; EAX = �����䨪��� 䠩��
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
FileErase       PROC    C FAR
        public  FileErase
        call    _FileErase
        ret
FileErase       ENDP

_FileErase      PROC    C NEAR
        uses    GS,eax
        
        cmp     eax,MaxFiles
        jae     SHORT $$Error1
        mov     GS,SS:SEG_HDD_HEAP
        movzx   eax,WORD PTR GS:Files[eax*8][TFileRec.DirItem]
        cmp     ax,0FFFFh
        je      SHORT $$Error1
        call    FileEraseDir
        clc
        jmp     SHORT $$End
$$Error1:
        stc
$$End:
        ret
_FileErase      ENDP

; ������� 䠩� �� �����
; � DS:EDX 㪠��⥫� �� ��� 䠩�� (ASCIZ-��ப�)
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
FileEraseName   PROC    C FAR
        public  FileEraseName
        call    _FileEraseName
        ret
FileEraseName   ENDP

_FileEraseName  PROC    C NEAR
        uses    GS,eax
        
        mov     GS,SS:SEG_HDD_HEAP
        
        call    FileSearch
        jc      SHORT $$End
        call    FileEraseDir
        clc
;        jmp     SHORT $$End
$$End:
        ret
_FileEraseName  ENDP

; ��२�������� 䠩�
; EAX = �����䨪��� 䠩��
; � DS:EDX 㪠��⥫� �� ��� 䠩�� (ASCIZ-��ப�)
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
FileRename      PROC    C FAR
        public  FileRename
        call    _FileRename
        ret
FileRename      ENDP

_FileRename     PROC    C NEAR
        uses    DS,ES,GS,eax,ecx,edi,esi
        
        cmp     eax,MaxFiles
        jae     SHORT $$Error1
        mov     GS,SS:SEG_HDD_HEAP
        movzx   eax,WORD PTR GS:Files[eax*8][TFileRec.DirItem]
        cmp     ax,0FFFFh
        je      SHORT $$Error1

        mov     edi,eax
        shl     edi,5
        add     edi,RootDirBuffer

        call    FileSearch
        jnc     SHORT $$Error1

        mov     DS,SS:SEG_HDD_HEAP
        mov     ES,SS:SEG_HDD_HEAP
        mov     esi,TmpDirName
        mov     ecx,11
        push    edi
        db      67h
        rep     movsb
        pop     edi

        sub     edi,RootDirBuffer
        shr     edi,9
        mov     esi,edi
        add     edi,GS:RootDirStartSect
        mov     GS:SectorAddress,edi
        shl     esi,9
        add     esi,RootDirBuffer
        call    WriteHDDSectorBuf
        
        clc
        jmp     SHORT $$End
$$Error1:
        stc
$$End:
        ret
_FileRename     ENDP

; ��१��� 䠩� �� ⥪�饬� 㪠��⥫� � ��
; EAX = �����䨪��� 䠩��
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
FileTrunc      PROC    C FAR
        public  FileTrunc
        call    _FileTrunc
        ret
FileTrunc      ENDP

_FileTrunc     PROC    C NEAR
        uses    DS,ES,GS,eax,ecx,edi,esi
        
        cmp     eax,MaxFiles
        jae     $$Error1
        mov     GS,SS:SEG_HDD_HEAP
        movzx   edi,WORD PTR GS:Files[eax*8][TFileRec.DirItem]
        cmp     di,0FFFFh
        je      $$Error1

        mov     eax,GS:Files[eax*8][TFileRec.FilePosition]		;��﫨 㪠��⥫� � 䠩��
        shl     edi,5
	push	edi
;	add     edi,RootDirBuffer
        mov     ecx,GS:RootDirBuffer[edi][TDirItem.DIR_FileSize]	;��﫨 ࠧ��� 䠩��
        mov     GS:RootDirBuffer[edi][TDirItem.DIR_FileSize],eax	;��⠭����� ���� ࠧ��� 䠩��
;	sub     edi,RootDirBuffer
;	pop	edi

; Fix Dir
        shr     edi,9
        mov     esi,edi
        add     edi,GS:RootDirStartSect
        mov     GS:SectorAddress,edi
        shl     esi,9
        add     esi,RootDirBuffer
        call    WriteHDDSectorBuf
	pop	edi
;
;
        movzx   eax,GS:RootDirBuffer[edi][TDirItem.DIR_FstClusLO]
        mov     ebx,eax

$$NextCluster:
        mov     ecx,eax
        shr     ecx,8
        mov     edx,ebx
        shr     edx,8
        
        cmp     ecx,edx
        je      SHORT $$ClearCluster
        
        mov     esi,edx
        add     edx,GS:FATStartSect
        mov     GS:SectorAddress,edx
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf

$$ClearCluster:
        movzx   ebx,WORD PTR GS:FATBuffer[eax*2]
        mov     WORD PTR GS:FATBuffer[eax*2],0

        cmp     bx,0FFFFh
        je      SHORT $$ClearLastCluster
        xchg    eax,ebx
        jmp     SHORT $$NextCluster

$$ClearLastCluster:
;        shl     eax,1
;        shr     eax,9
        shr     eax,8
        mov     esi,eax
        add     eax,GS:FATStartSect
        mov     GS:SectorAddress,eax
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf
;
;
        clc
        jmp     SHORT $$End
$$Error1:
        stc
$$End:
        ret
_FileTrunc     ENDP

; ������� � ������ 䠩�
; � DS:EDX 㪠��⥫� �� ��� 䠩�� (ASCIZ-��ப�)
; ��室 ��� �訡��:
; CF = 0, EAX = �����䨪��� 䠩��
; ��室 � �訡���:
; CF = 1, EAX - �� ��।���
FileCreate      PROC    C FAR
        public  FileCreate
        call    _FileCreate
        ret
FileCreate      ENDP

_FileCreate     PROC    C NEAR

LOCAL   FCluster:WORD        
LOCAL   FHandler:DWORD        

        uses    DS,ES,GS,ecx,edx,edi,esi

        mov     GS,SS:SEG_HDD_HEAP

        call    GetFileRec
        jc      $$Error1
        mov     FHandler,edi
        call    FileSearch
        jc      $$NewDirItem

        call    FileEraseDir

$$NewDirItem:
        xor     edx,edx
$$NextDirItem:
        mov     edi,edx
        shl     edi,5
        add     edi,RootDirBuffer
        mov     al,GS:[edi]
        or      al,al
        jz      SHORT $$NewFile 
        cmp     al,0E5h
        je      SHORT $$NewFile
        inc     edx
        cmp     edx,RootDirItems
        jb      SHORT $$NextDirItem
        stc
        jmp     $$Error2
        
$$NewFile:
        call    FindFreeCluster
        jc      $$Error3
        
        mov     FCluster,ax
        mov     GS:[edi][TDirItem.DIR_FstClusLO],ax
        xor     eax,eax
        mov     GS:[edi][TDirItem.DIR_Attr],al
        mov     DWORD PTR GS:[edi][TDirItem.DIR_NTRes],eax
        mov     DWORD PTR GS:[edi][TDirItem.DIR_CrtDate],eax
        mov     GS:[edi][TDirItem.DIR_FstClusHI],ax
        mov     GS:[edi][TDirItem.DIR_FileSize],eax

; �६� � ��� ᮧ����� 䠩��        
;        mov     al,0Bh
;        out     70h,al
;        in      al,71h
;        or      al,00000110b    ; 24 �ᮢ�� ������ ०��
;        mov     cl,al
;        mov     al,0Bh
;        out     70h,al
;        mov     al,cl
;        out     71h,al
        mov     ax,4                                    ; ��
        out     70h,al
        in      al,71h
        shl     ax,4
        shr     al,4                                    ; ����稫� ��㯠�������� BCD
        aad                                             ; ��ॢ��� � BIN
        movzx   cx,al
        shl     cx,6

        mov     ax,2                                    ; �����
        out     70h,al
        in      al,71h
        shl     ax,4
        shr     al,4                                    ; ����稫� ��㯠�������� BCD
        aad                                             ; ��ॢ��� � BIN
        or      cl,al
        shl     cx,5

        xor     ax,ax                                   ; ᥪ㭤�
        out     70h,al
        in      al,71h
        shl     ax,4
        shr     al,4                                    ; ����稫� ��㯠�������� BCD
        aad                                             ; ��ॢ��� � BIN
        shr     al,1
        
        or      cl,al
        mov     GS:[edi][TDirItem.DIR_WrtTime],cx       ; �६� ᮧ����� 䠩��

        mov     ax,32h                                  ; 2 ���訥 ���� ���� BCD
        out     70h,al
        in      al,71h
        shl     ax,4
        shr     al,4                                    ; ����稫� ��㯠�������� BCD
        aad                                             ; ��ॢ��� � BIN
        push    bx
        push    dx
        mov     bx,100
        mul     bx
        pop     dx
        pop     bx
        mov     cx,ax                                   ; 2 ���訥 ���� ���� BIN * 100
        xor     ax,ax
        mov     ax,9                                    ; 2 ����訥 ���� ����
        out     70h,al
        in      al,71h
        shl     ax,4
        shr     al,4                                    ; ����稫� ��㯠�������� BCD
        aad                                             ; ��ॢ��� � BIN
        add     cx,ax
        sub     cx,1980                                 ; ����� ���� ����� 1980
        shl     cx,4

        mov     ax,8                                    ; �����
        out     70h,al
        in      al,71h
        shl     ax,4
        shr     al,4                                    ; ����稫� ��㯠�������� BCD
        aad                                             ; ��ॢ��� � BIN
        or      cl,al
        shl     cx,5

        mov     ax,7                                    ; ����
        out     70h,al
        in      al,71h
        shl     ax,4
        shr     al,4                                    ; ����稫� ��㯠�������� BCD
        aad                                             ; ��ॢ��� � BIN
        or      cl,al
        mov     GS:[edi][TDirItem.DIR_WrtDate],cx       ; ��� ᮧ����� 䠩��

; ��� 䠩��        
        mov     DS,SS:SEG_HDD_HEAP
        mov     ES,SS:SEG_HDD_HEAP
        mov     esi,TmpDirName
        mov     ecx,11
        push    edi
        db      67h
        rep     movsb
        pop     edi

        sub     edi,RootDirBuffer
        shr     edi,9
        mov     esi,edi
        add     edi,GS:RootDirStartSect
        mov     GS:SectorAddress,edi
        shl     esi,9
        add     esi,RootDirBuffer
        call    WriteHDDSectorBuf

        movzx   eax,FCluster
        mov     WORD PTR GS:FATBuffer[eax*2],0FFFFh
        shl     eax,1
        shr     eax,9
        mov     esi,eax
        add     eax,GS:FATStartSect
        mov     GS:SectorAddress,eax
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf

        mov     eax,FHandler
        mov     GS:Files[eax*8][TFileRec.DirItem],dx
        mov     DWORD PTR GS:Files[eax*8][TFileRec.FilePosition],0
$$CreateOK:
        clc
        jmp     SHORT $$End
$$Error1:
        jmp     SHORT $$End
$$Error2:
        jmp     SHORT $$End
$$Error3:
        jmp     SHORT $$End
$$End:
        ret
_FileCreate     ENDP

; ������� � ������ 䠩� ��������� ࠧ���
; � DS:EDX 㪠��⥫� �� ��� 䠩�� (ASCIZ-��ப�)
; � ECX ࠧ��� 䠩��
; ��室 ��� �訡��:
; CF = 0, EAX = �����䨪��� 䠩��
; ��室 � �訡���:
; CF = 1, EAX - �� ��।���
FileCreateSize  PROC    C FAR
        public  FileCreateSize
        call    _FileCreateSize
        ret
FileCreateSize  ENDP

_FileCreateSize PROC    C NEAR
        
LOCAL   FCluster:WORD        
LOCAL   FSector:WORD        
LOCAL   FCount:DWORD        
;LOCAL   FDirPtr:DWORD        
LOCAL   FFATChanged:WORD        
        
        uses    DS,ES,GS,ebx,edx,edi,esi

        push    ecx

        call    _FileCreate
        jc      $$End

        mov     ebx,eax

        mov     GS,SS:SEG_HDD_HEAP
        mov     ES,SS:SEG_HDD_HEAP
        mov     DS,SS:SEG_HDD_HEAP
        mov     edi,SectorDataBuffer
        push    ecx
        mov     ecx,256                                 ;
        xor     eax,eax                                 ;
        db      67h                                     ;
        rep     stosw                                   ;
        pop     ecx

        movzx   edi,WORD PTR GS:Files[ebx*8][TFileRec.DirItem]
        shl     edi,5
        add     edi,RootDirBuffer

        movzx   eax,WORD PTR GS:[edi][TDirItem.DIR_FstClusLO]
        mov     FCluster,ax
        xor     edx,edx
        mov     FSector,dx
        mov     FFATChanged,dx

        mov     FCount,ecx

$$CalcSect:
        sub     eax,2
        movzx   edx,WORD PTR GS:SectorsInCluster
        mul     edx
        add     eax,GS:RootDirStartSect
        add     eax,RootDirSize
        mov     GS:SectorAddress,eax
;
$$WriteSect:        
;        call    WriteHDDSector
        mov     edx,512
        cmp     edx,ecx
        jbe     SHORT $$IncPos
        mov     edx,ecx
$$IncPos:
        add     GS:Files[ebx*8][TFileRec.FilePosition],edx
;        sub     FCount,edx
        sub     ecx,edx
        jz      $$WriteOK
        mov     dx,FSector
        inc     dx
        cmp     dx,GS:SectorsInCluster
        jae     SHORT $$NextCluster
        mov     FSector,dx
        inc     DWORD PTR GS:SectorAddress
        jmp     SHORT $$WriteSect
$$NextCluster:        
        movzx   eax,FCluster
        cmp     WORD PTR GS:FatBuffer[eax*2],0FFFFh
        je      SHORT $$AddCluster

        mov     ax,GS:FatBuffer[eax*2]
        mov     FCluster,ax
        xor     edx,edx
        mov     FSector,dx
        jmp     $$CalcSect
        
$$AddCluster:
        mov     edx,eax
        call    FindFreeCluster
        jc      $$Error4
        mov     FCluster,ax
        xchg    eax,edx

        mov     GS:FATBuffer[eax*2],dx
        mov     WORD PTR GS:FATBuffer[edx*2],0FFFFh

        mov     FFATChanged,1
        
; Fix FAT
        shr     eax,8
        shr     edx,8
        cmp     eax,edx
        je      SHORT $$FATFixed

        push    eax
        mov     esi,eax
        add     eax,GS:FATStartSect
        mov     GS:SectorAddress,eax
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf
        pop     eax

        mov     esi,edx
        add     edx,GS:FATStartSect
        mov     GS:SectorAddress,edx
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf        

        mov     FFATChanged,0
$$FATFixed:
        movzx   eax,FCluster
        xor     edx,edx
        mov     FSector,dx
        jmp     $$CalcSect

$$WriteOK:
        cmp     FFATChanged,1
        jne     SHORT $$FixDir
        movzx   eax,FCluster
        shr     eax,8
        mov     esi,eax
        add     eax,GS:FATStartSect
        mov     GS:SectorAddress,eax
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf

$$FixDir:        
        movzx   edi,WORD PTR GS:Files[ebx*8][TFileRec.DirItem]
        shl     edi,5
        add     edi,RootDirBuffer

        mov     eax,GS:Files[ebx*8][TFileRec.FilePosition]
        cmp     GS:[edi][TDirItem.DIR_FileSize],eax
        jae     $$DirFixed
        mov     GS:[edi][TDirItem.DIR_FileSize],eax
        
        sub     edi,RootDirBuffer
        shr     edi,9
        mov     esi,edi
        add     edi,GS:RootDirStartSect
        mov     GS:SectorAddress,edi
        shl     esi,9
        add     esi,RootDirBuffer
        call    WriteHDDSectorBuf

$$DirFixed:
        xor     eax,eax
        mov     GS:Files[ebx*8][TFileRec.FilePosition],eax
        mov     eax,ebx
        clc
        jmp     SHORT $$End
$$Error1:
        stc
        jmp     SHORT $$End
$$Error2:
        stc
        jmp     SHORT $$End
$$Error3:
        stc
        jmp     SHORT $$End
$$Error4:
        cmp     FFATChanged,1
        jne     SHORT $$Error4_1
        movzx   eax,FCluster
        shr     eax,8
        mov     esi,eax
        add     eax,GS:FATStartSect
        mov     GS:SectorAddress,eax
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf
$$Error4_1:
        stc
        jmp     SHORT $$End
$$End:
        pop     ecx
        ret
_FileCreateSize ENDP

; ������� � ������ ���⮩ 䠩� (���������� ��ﬨ) ��������� ࠧ���
; � DS:EDX 㪠��⥫� �� ��� 䠩�� (ASCIZ-��ப�)
; � ECX ࠧ��� 䠩��
; ��室 ��� �訡��:
; CF = 0, EAX = �����䨪��� 䠩��
; ��室 � �訡���:
; CF = 1, EAX - �� ��।���
FileCreateEmpty PROC    C FAR
        public  FileCreateEmpty
        call    _FileCreateEmpty
        ret
FileCreateEmpty ENDP

_FileCreateEmpty        PROC    C NEAR

LOCAL   FCluster:WORD        
LOCAL   FSector:WORD        
LOCAL   FCount:DWORD        
LOCAL   FFATChanged:WORD        
        
        uses    DS,ES,GS,ebx,edx,edi,esi

        push    ecx

        call    _FileCreateSize
;        call    _FileCreate
        jc      $$End

        mov     ebx,eax

        mov     GS,SS:SEG_HDD_HEAP
        mov     ES,SS:SEG_HDD_HEAP
        mov     DS,SS:SEG_HDD_HEAP
        mov     edi,SectorDataBuffer
        push    ecx
        mov     ecx,256                                 ;
        xor     eax,eax                                 ;
        db      67h                                     ;
        rep     stosw                                   ;
        pop     ecx

        movzx   edi,WORD PTR GS:Files[ebx*8][TFileRec.DirItem]
        shl     edi,5
        add     edi,RootDirBuffer

        movzx   eax,WORD PTR GS:[edi][TDirItem.DIR_FstClusLO]
        mov     FCluster,ax
        xor     edx,edx
        mov     FSector,dx
        mov     FFATChanged,dx

        mov     FCount,ecx

$$CalcSect:
        sub     eax,2
        movzx   edx,WORD PTR GS:SectorsInCluster
        mul     edx
        add     eax,GS:RootDirStartSect
        add     eax,RootDirSize
        mov     GS:SectorAddress,eax
;
$$WriteSect:        
        call    WriteHDDSector
        mov     edx,512
        cmp     edx,ecx
        jbe     SHORT $$IncPos
        mov     edx,ecx
$$IncPos:
        add     GS:Files[ebx*8][TFileRec.FilePosition],edx
;        sub     FCount,edx
        sub     ecx,edx
        jz      $$WriteOK
        mov     dx,FSector
        inc     dx
        cmp     dx,GS:SectorsInCluster
        jae     SHORT $$NextCluster
        mov     FSector,dx
        inc     DWORD PTR GS:SectorAddress
        jmp     SHORT $$WriteSect
$$NextCluster:        
        movzx   eax,FCluster
        cmp     WORD PTR GS:FatBuffer[eax*2],0FFFFh
        je      SHORT $$AddCluster

        mov     ax,GS:FatBuffer[eax*2]
        mov     FCluster,ax
        xor     edx,edx
        mov     FSector,dx
        jmp     $$CalcSect
        
$$AddCluster:
        mov     edx,eax
        call    FindFreeCluster
        jc      $$Error4
        mov     FCluster,ax
        xchg    eax,edx

        mov     GS:FATBuffer[eax*2],dx
        mov     WORD PTR GS:FATBuffer[edx*2],0FFFFh

        mov     FFATChanged,1
        
; Fix FAT
        shr     eax,8
        shr     edx,8
        cmp     eax,edx
        je      SHORT $$FATFixed

        push    eax
        mov     esi,eax
        add     eax,GS:FATStartSect
        mov     GS:SectorAddress,eax
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf
        pop     eax

        mov     esi,edx
        add     edx,GS:FATStartSect
        mov     GS:SectorAddress,edx
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf        

        mov     FFATChanged,0
$$FATFixed:
        movzx   eax,FCluster
        xor     edx,edx
        mov     FSector,dx
        jmp     $$CalcSect

$$WriteOK:
        cmp     FFATChanged,1
        jne     SHORT $$FixDir
        movzx   eax,FCluster
        shr     eax,8
        mov     esi,eax
        add     eax,GS:FATStartSect
        mov     GS:SectorAddress,eax
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf

$$FixDir:        
        movzx   edi,WORD PTR GS:Files[ebx*8][TFileRec.DirItem]
        shl     edi,5
        add     edi,RootDirBuffer

        mov     eax,GS:Files[ebx*8][TFileRec.FilePosition]
        cmp     GS:[edi][TDirItem.DIR_FileSize],eax
        jae     $$DirFixed
        mov     GS:[edi][TDirItem.DIR_FileSize],eax
        
        sub     edi,RootDirBuffer
        shr     edi,9
        mov     esi,edi
        add     edi,GS:RootDirStartSect
        mov     GS:SectorAddress,edi
        shl     esi,9
        add     esi,RootDirBuffer
        call    WriteHDDSectorBuf

$$DirFixed:
        xor     eax,eax
        mov     GS:Files[ebx*8][TFileRec.FilePosition],eax
        mov     eax,ebx
        clc
        jmp     SHORT $$End
$$Error1:
        stc
        jmp     SHORT $$End
$$Error2:
        stc
        jmp     SHORT $$End
$$Error3:
        stc
        jmp     SHORT $$End
$$Error4:
        cmp     FFATChanged,1
        jne     SHORT $$Error4_1
        movzx   eax,FCluster
        shr     eax,8
        mov     esi,eax
        add     eax,GS:FATStartSect
        mov     GS:SectorAddress,eax
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf
$$Error4_1:
        stc
        jmp     SHORT $$End
$$End:
        pop     ecx
        ret
_FileCreateEmpty        ENDP

; ������� 䠩�
; EAX - �����䨪��� 䠩��
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
FileClose       PROC    C FAR
        public  FileClose
        call    _FileClose
        ret
FileClose       ENDP

_FileClose      PROC    C NEAR
        uses    GS
        cmp     eax,MaxFiles
        jae     SHORT $$End
        mov     GS,SS:SEG_HDD_HEAP
        mov     WORD PTR GS:Files[eax*8][TFileRec.DirItem],0FFFFh
        clc
        jmp     SHORT $$End
$$Error1:
        stc
$$End:
        ret
_FileClose      ENDP

; ������� ࠧ��� 䠩��
; EAX - �����䨪��� 䠩��
; ��室 ��� �訡��:
; CF = 0
; ECX - ࠧ��� 䠩��
; ��室 � �訡���:
; CF = 1
FileSize        PROC    C FAR
        public  FileSize
        call    _FileSize
        ret
FileSize        ENDP

_FileSize       PROC    C NEAR
        uses    GS,esi
        cmp     eax,MaxFiles
        jae     SHORT $$Error1
        mov     GS,SS:SEG_HDD_HEAP
        cmp     WORD PTR GS:Files[eax*8][TFileRec.DirItem],0FFFFh
        je      SHORT $$Error1

        movzx   esi,WORD PTR GS:Files[eax*8][TFileRec.DirItem]
        shl     esi,5
        add     esi,RootDirBuffer
        mov     ecx,GS:[esi][TDirItem.DIR_FileSize]
        clc
        jmp     SHORT $$End
$$Error1:
        stc
$$End:
        ret
_FileSize       ENDP

; ��⠭����� 㪠��⥫� �⥭��/����� � 䠩��
; EAX - �����䨪��� 䠩��
; ECX - ����ﭨ� �� ��砫� 䠩��
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
FileSeek        PROC    C FAR
        public  FileSeek
        call    _FileSeek
        ret
FileSeek        ENDP

_FileSeek       PROC    C NEAR
        uses    GS,esi
        cmp     eax,MaxFiles
        jae     SHORT $$Error1
        mov     GS,SS:SEG_HDD_HEAP
        cmp     WORD PTR GS:Files[eax*8][TFileRec.DirItem],0FFFFh
        je      SHORT $$Error1

        movzx   esi,WORD PTR GS:Files[eax*8][TFileRec.DirItem]
        shl     esi,5
        add     esi,RootDirBuffer
        cmp     GS:[esi][TDirItem.DIR_FileSize],ecx
        jb      SHORT $$Error1
        mov     GS:Files[eax*8][TFileRec.FilePosition],ecx
        clc
        jmp     SHORT $$End
$$Error1:
        stc
$$End:
        ret
_FileSeek       ENDP

; ������� 㪠��⥫� �⥭��/����� � 䠩��
; EAX - �����䨪��� 䠩��
; ��室 ��� �訡��:
; CF = 0
; ECX - 㪠��⥫� �⥭��/����� � 䠩��
; ��室 � �訡���:
; CF = 1
FilePos         PROC    C FAR
        public  FilePos
        call    _FilePos
        ret
FilePos         ENDP

_FilePos        PROC    C NEAR
        uses    GS
        
        cmp     eax,MaxFiles
        jae     SHORT $$Error1
        mov     GS,SS:SEG_HDD_HEAP
        cmp     WORD PTR GS:Files[eax*8][TFileRec.DirItem],0FFFFh
        je      SHORT $$Error1

        mov     ecx,GS:Files[eax*8][TFileRec.FilePosition]
        clc
        jmp     SHORT $$End
$$Error1:
        stc
$$End:
        ret
_FilePos        ENDP

; ���� ������
; EAX - ����� ������ �� ��砫� 䠩��
; ESI - 㪠��⥫� �� ����� ��⠫���
; ��室 ��� �訡��:
; CF = 0
; EAX - ����� ������
; ��室 � �訡���:
; EAX - ����� ��᫥����� ������ 䠩��
; CF = 1
GetCluster      PROC    C NEAR
        uses    ecx
        
        mov     cx,ax
        movzx   eax,WORD PTR GS:[esi][TDirItem.DIR_FstClusLO]
        
$$FindCluster:
        or      cx,cx
        jz      SHORT $$FindOK
        
        cmp     WORD PTR GS:FatBuffer[eax*2],0FFFFh
        jz      SHORT $$Error1
        movzx   eax,WORD PTR GS:FatBuffer[eax*2]
        dec     cx
        jmp     SHORT $$FindCluster

$$FindOK:
        clc
        jmp     SHORT $$End
$$Error1:
        stc
$$End:
        ret
GetCluster      ENDP

; �⥭�� �� 䠩��
; EAX - �����䨪��� 䠩��
; DS:EDX - 㪠��⥫� �� ���� ��� ������
; ECX - �᫮ ����
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
FileRead        PROC    C FAR
        public  FileRead
        call    _FileRead
        ret
FileRead        ENDP

_FileRead       PROC    C NEAR

LOCAL   FCluster:WORD        
LOCAL   FSector:WORD        
LOCAL   FCount:DWORD        
        
        uses    DS,ES,GS,eax,ebx,edx,edi,esi

        push    ecx

        cmp     eax,MaxFiles
        jae     $$Error1
        mov     GS,SS:SEG_HDD_HEAP
        cmp     WORD PTR GS:Files[eax*8][TFileRec.DirItem],0FFFFh
        je      $$Error1

        push    DS
        pop     ES
        mov     edi,edx
        
        movzx   esi,WORD PTR GS:Files[eax*8][TFileRec.DirItem]
        shl     esi,5
        add     esi,RootDirBuffer

        mov     ebx,eax
        mov     eax,GS:Files[ebx*8][TFileRec.FilePosition]
        cmp     GS:[esi][TDirItem.DIR_FileSize],eax
        jbe     $$Error1
        
        lea     edx,[eax+ecx]
        cmp     edx,GS:[esi][TDirItem.DIR_FileSize]
        jbe     SHORT $$SetCount
        mov     ecx,GS:[esi][TDirItem.DIR_FileSize]
        sub     ecx,eax
$$SetCount:
        mov     FCount,ecx

        shr     eax,9
        xor     edx,edx
        push    ebx
        movzx   ebx,WORD PTR GS:SectorsInCluster
        div     ebx
        pop     ebx
        mov     FSector,dx

        call    GetCluster

;        add     ax,GS:[esi][TDirItem.DIR_FstClusLO]
;        cmp     ax,GS:[esi][TDirItem.DIR_FstClusLO]
;        je      SHORT $$SetCluster
;        movzx   eax,WORD PTR GS:FatBuffer[eax*2-2]
;$$SetCluster:
        mov     FCluster,ax
        mov     DS,SS:SEG_HDD_HEAP
$$CalcSect:
        sub     eax,2
        push    edx
        movzx   edx,WORD PTR GS:SectorsInCluster
        mul     edx
        pop     edx
        add     eax,GS:RootDirStartSect
        add     eax,RootDirSize
        add     eax,edx
        mov     GS:SectorAddress,eax
$$ReadSect:        
        call    ReadHDDSector

        push    ecx
;        push    esi
        mov     esi,SectorDataBuffer
        mov     edx,GS:Files[ebx*8][TFileRec.FilePosition]
        and     edx,511
        add     esi,edx
        mov     eax,512
        sub     eax,edx
        cmp     eax,ecx
        jae     SHORT $$MoveData
        mov     ecx,eax
$$MoveData:        
        mov     edx,ecx
        db      67h
        rep     movsb
;        pop     esi
        pop     ecx

        add     GS:Files[ebx*8][TFileRec.FilePosition],edx
        sub     FCount,edx
        sub     ecx,edx
        jz      SHORT $$ReadOK
;        movzx   eax,FCluster
;        movzx   edx,FSector
;        inc     edx
        mov     dx,FSector
        inc     dx
        cmp     dx,GS:SectorsInCluster
        jae     SHORT $$NextCluster
        mov     FSector,dx
        inc     DWORD PTR GS:SectorAddress
 
         jmp     SHORT $$ReadSect
;        jmp     $$CalcSect
$$NextCluster:        
        movzx   eax,FCluster
        mov     ax,GS:FatBuffer[eax*2]
        mov     FCluster,ax
        xor     edx,edx
        mov     FSector,dx
        jmp     $$CalcSect
$$ReadOK:
        
        clc
        jmp     SHORT $$End
$$Error1:
        stc
$$End:
        pop     ecx
        ret
_FileRead       ENDP

; ������ � 䠩�
; EAX - �����䨪��� 䠩��
; DS:EDX - 㪠��⥫� �� ���� � ����묨
; ECX - �᫮ ����
; ��室 ��� �訡��:
; CF = 0
; ��室 � �訡���:
; CF = 1
FileWrite       PROC    C FAR
        public  FileWrite
        call    _FileWrite
        ret
FileWrite       ENDP

_FileWrite      PROC    C NEAR

LOCAL   FCluster:WORD        
LOCAL   FSector:WORD        
LOCAL   FCount:DWORD        
LOCAL   FDirPtr:DWORD        
        
        uses    DS,ES,GS,eax,ebx,edx,edi,esi
        
        push    ecx

        cmp     eax,MaxFiles
        jae     $$Error1
        mov     GS,SS:SEG_HDD_HEAP
        cmp     WORD PTR GS:Files[eax*8][TFileRec.DirItem],0FFFFh
        je      $$Error2

        mov     ES,SS:SEG_HDD_HEAP
        mov     esi,edx
        
        movzx   edi,WORD PTR GS:Files[eax*8][TFileRec.DirItem]
        shl     edi,5
        add     edi,RootDirBuffer

        mov     ebx,eax
        mov     eax,GS:Files[ebx*8][TFileRec.FilePosition]
        cmp     GS:[edi][TDirItem.DIR_FileSize],eax
        jb      $$Error3

        mov     FCount,ecx

        shr     eax,9
        xor     edx,edx
        push    ebx
        movzx   ebx,WORD PTR GS:SectorsInCluster
        div     ebx
        pop     ebx
        mov     FSector,dx
        
        push    esi
        mov     esi,edi
        call    GetCluster
        pop     esi
        jc      $$AddCluster
$$SetCluster:
        mov     FCluster,ax
        
;        add     ax,GS:[edi][TDirItem.DIR_FstClusLO]
;        cmp     ax,GS:[edi][TDirItem.DIR_FstClusLO]
;        je      SHORT $$SetCluster
;        dec     eax
;        cmp     WORD PTR GS:FatBuffer[eax*2],0FFFFh
;        je      $$AddCluster
;        movzx   eax,WORD PTR GS:FatBuffer[eax*2]

;$$SetCluster:
;        mov     FCluster,ax
$$CalcSect:
        sub     eax,2
        push    edx
        movzx   edx,WORD PTR GS:SectorsInCluster
        mul     edx
        pop     edx
        add     eax,GS:RootDirStartSect
        add     eax,RootDirSize
        add     eax,edx
        mov     GS:SectorAddress,eax
;
$$WriteSect:        
        push    ecx
        mov     edi,GS:Files[ebx*8][TFileRec.FilePosition]
        and     edi,511
        mov     eax,512
        sub     eax,edi
        cmp     eax,ecx
        jae     SHORT $$ReadSect
        mov     ecx,eax
$$ReadSect:
        or      edi,edi
        jnz     SHORT $$ReadSect1
        cmp     ecx,512
        je      SHORT $$MoveData
$$ReadSect1:
        call    ReadHDDSector
$$MoveData:        
        mov     edx,ecx
        add     edi,SectorDataBuffer
        db      67h
        rep     movsb
;
        call    WriteHDDSector
        pop     ecx
        add     GS:Files[ebx*8][TFileRec.FilePosition],edx
;        sub     FCount,edx
        sub     ecx,edx
        jz      $$WriteOK
        mov     dx,FSector
        inc     dx
        cmp     dx,GS:SectorsInCluster
        jae     SHORT $$NextCluster
        mov     FSector,dx
        inc     DWORD PTR GS:SectorAddress
        jmp     SHORT $$WriteSect
$$NextCluster:        
        movzx   eax,FCluster
        cmp     WORD PTR GS:FatBuffer[eax*2],0FFFFh
        je      SHORT $$AddCluster

        mov     ax,GS:FatBuffer[eax*2]
        mov     FCluster,ax
        xor     edx,edx
        mov     FSector,dx
        jmp     $$CalcSect
        
$$AddCluster:
        mov     edx,eax
        call    FindFreeCluster
        jc      $$Error4
        mov     FCluster,ax
        xchg    eax,edx

        mov     GS:FATBuffer[eax*2],dx
        mov     WORD PTR GS:FATBuffer[edx*2],0FFFFh

; Fix FAT
        push    DS
        push    esi
        mov     DS,SS:SEG_HDD_HEAP
        shl     eax,1
        shr     eax,9
        push    eax
        mov     esi,eax
        add     eax,GS:FATStartSect
        mov     GS:SectorAddress,eax
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf
        pop     eax

        shl     edx,1
        shr     edx,9
        cmp     eax,edx
        je      SHORT $$FATFixed
        mov     esi,edx
        add     edx,GS:FATStartSect
        mov     GS:SectorAddress,edx
        shl     esi,9
        add     esi,FATBuffer
        call    WriteHDDSectorBuf        
$$FATFixed:
        pop     esi
        pop     DS
        movzx   eax,FCluster
        xor     edx,edx
        mov     FSector,dx
        jmp     $$CalcSect

$$WriteOK:
        movzx   edi,WORD PTR GS:Files[ebx*8][TFileRec.DirItem]
        shl     edi,5
        add     edi,RootDirBuffer

        mov     eax,GS:Files[ebx*8][TFileRec.FilePosition]
        cmp     GS:[edi][TDirItem.DIR_FileSize],eax
        jae     $$DirFixed
        mov     GS:[edi][TDirItem.DIR_FileSize],eax
        
        push    DS
        mov     DS,SS:SEG_HDD_HEAP
        sub     edi,RootDirBuffer
        shr     edi,9
        mov     esi,edi
        add     edi,GS:RootDirStartSect
        mov     GS:SectorAddress,edi
        shl     esi,9
        add     esi,RootDirBuffer
        call    WriteHDDSectorBuf
        pop     DS

$$DirFixed:
        clc
        jmp     SHORT $$End
$$Error1:
        stc
        jmp     SHORT $$End
$$Error2:
        stc
        jmp     SHORT $$End
$$Error3:
        stc
        jmp     SHORT $$End
$$Error4:
        jmp     SHORT $$End
$$End:
        pop     ecx
        ret
_FileWrite      ENDP

HDD_SIZE=$-HDD_CODE
HDD_CODE        ENDS

        END

