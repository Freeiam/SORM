; 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様融
; �  ����:         PUMA_Cmd.ASM                                                �
; �  蹍������痰:  ���ア�罟┤ �.�.                                             �
; 把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超
; �  �甎��讌礇�:   PUMA_Scr.DAT, RingBuff.ASM                                  �
; 把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超
; �  ��Л�腑�┘:   ｡��｀砒┴ �爬�閧諷 ������ ����                             �
; 把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超
; �  ��ぅ爨��:                                                                 �
; �                                                                            �
; �                                                                            �
; 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様夕

;CfgOn
;201                 ; ゛�� 礒� � 爛Θ�� ���筥ｃ��罔�
;202                 ; �アム�覃 皋� ゛���
;203                 ; �アム�覃 ��Кム
;204                 ; �荐！� 甌Г����
;205                 ; 皋� ����諷 �� ��いムΘ��モ瘴
;206                 ; ��ム�罔� �ク�いムΘ��モ瘴
; 207 - 爛Д燿, �荐！� �皖琺皋�
;CfgOff
;208                 ; ゛�� �� � 爛Θ�� ���筥ｃ��罔�
;209                 ; ��ム�罔� �� ��▲琥キ�
;210                 ; �荐！� ���琺皋� �����
;217                 ; �荐！� �´�←キ�� ����諷
;218                 ; 痰�����, �� ���� - �モ �皖琺皰�� �����
;CfgIn-CfgOff
;211                 ; ゛�� �� � 爛Θ�� ���筥ｃ��罔�
;212                 ; ゛�� �� � 爛Θ�� ����瓱
;213                 ; �アム�覃 ���ム �腑爛き��� 瓮�皰��
;214                 ; ���牀� ��茘ィ� 瓮�皰��
;215                 ; �モ �皖琺皰�� �����
;216                 ; �荐！� ����瓱 瓮�皰�� (皰�讓� � CfgIn)
;CfgOut
;220                 ; �荐！� 艪キ�� 瓮�皰�� ����諷


PUMA_Cmd        PROC
                ; === �祀赳��襯 =====================================
                ; --- ���牀� 瓮燿ム� �� ��お�鈑キ┘
PC_037f:        cmp     WORD PTR es:[di + 4], 07f03h
                jne     PC_8000
                call    PC_Serv_On
                jmp     PC_Exit
                ; === �♂┘ =========================================
                ; --- ���牀� 皋�� � 甌痰�鎰�� ゛���
PC_8000:        cmp     WORD PTR es:[di + 4], 00080h
                jne     PC_8002
                call    PC_Get_Type
                jmp     PC_Exit
                ; --- ���牀� �´�←キ�� 蹣����
PC_8002:        cmp     WORD PTR es:[di + 4], 00280h
                jne     PC_8008
                call    PC_Refresh_Scr
                jmp     PC_Exit
                ; --- ���牀� 祚��←キ��
PC_8008:        cmp     WORD PTR es:[di + 4], 00880h
                jne     PC_8009
                call    PC_Ctrl_On
                jmp     PC_Exit
                ; --- �皙キ� 祚��←キ��
PC_8009:        cmp     WORD PTR es:[di + 4], 00980h
                jne     PC_800a
                call    PC_Ctrl_Off
                jmp     PC_Exit
                ; --- ���※��
PC_800a:        cmp     WORD PTR es:[di + 4], 00a80h
                jne     PC_800c
                call    PC_Key
                jmp     PC_Exit
                ; --- ���牀� 爛Θ�� ���筥ｃ��罔�
PC_800c:        cmp     WORD PTR es:[di + 4], 00c80h
                jne     PC_800d
                call    PC_Cfg_On
                jmp     PC_Exit
                ; --- ��▲琥キ┘ 爛Θ�� ���筥ｃ��罔�
PC_800d:        cmp     WORD PTR es:[di + 4], 00d80h
                jne     PC_800e
                call    PC_Cfg_Off
                jmp     PC_Exit
                ; --- �ムイ��� 瓮�皰�� ����諷 ゛���
PC_800e:        cmp     WORD PTR es:[di + 4], 00e80h
                jne     PC_8010
                call    PC_Sector_In
                jmp     PC_Exit
                ; --- ���牀� 瓮�皰�� ����諷 � ゛���
PC_8010:        cmp     WORD PTR es:[di + 4], 01080h
                jne     PC_8100
                call    PC_Sector_Out
                jmp     PC_Exit
                ; === ��爬筥��罔���襯 ===============================
                ; --- ��も▲爨ぅ�┘ ��爬筥��罔�
PC_8100:        cmp     WORD PTR es:[di + 4], 00081h
                jne     PC_8120
                call    PC_Tar_Ack
                jmp     PC_Exit
                ; --- ∇ガ�
PC_8120:        cmp     WORD PTR es:[di + 4], 02081h
                jne     PC_8121
                call    PC_Set_Time
                jmp     PC_Exit
                ; --- ��く�瓷� �� ��爬筥��罔�
PC_8121:        cmp     WORD PTR es:[di + 4], 02181h
                jne     PC_8122
                call    PC_Subscr_Tar
                jmp     PC_Exit
                ; --- ��く�瓷� �� ���爬�
PC_8122:        cmp     WORD PTR es:[di + 4], 02281h
                jne     PC_Exit
                call    PC_Subscr_Alarm
                jmp     PC_Exit
                ; ===
PC_Exit:        ret
PUMA_Cmd        ENDP



                ; === �祀赳��襯 =====================================
                ; --- ���牀� 瓮燿ム� �� ��お�鈑キ┘
PC_Serv_On      PROC
                xor     al, al
                call    PUMA_Answer
                ret
PC_Serv_On      ENDP

                ; === �♂┘ =========================================
                ; --- ���牀� 皋�� � 甌痰�鎰�� ゛���
PC_Get_Type     PROC
                push    ds
                push    es
                pushad
                ; 礫. �� �皴���錺�覃 ���モ ds:[esi] ├ 礫. �� �爬�閧覃 ���モ es:[edi]
                push    es
                pop     ds
                mov     esi, edi
                ; �キ錺� ����≡�┤ �むメ �� �癶モ�覃
                mov     edx, [esi]
                @EDX_back_Addr
                mov     [esi], edx
                ; �キ錺� ��� �����る �� �癶モ�覃
                or      BYTE PTR [esi + 5], 80h
                ; か┃�
                mov     ecx, 16
                ; �����モ琺 �癶モ�
                mov     BYTE  PTR [esi + 6],  _TYPE_BLOCK
                mov     WORD  PTR [esi + 7],  0fffh
                mov     DWORD PTR [esi + 8],  0
                mov     DWORD PTR [esi + 12], 0
                ; ├ � ������ � �ムイ�鉗ィ� ＜筌��
                mov     edx, ebx
                shl     edx, 1
                mov     es, ss:SEG_PUMA
                ; --- �皴��→�
                call    RB_MPut
                ; 筮爼�珮ガ 爛с�赳��
                popad
                pop     es
                pop     ds
                ret
PC_Get_Type     ENDP

                ; --- ���牀� �´�←キ�� 蹣����
PC_Refresh_Scr  PROC
                push    ds
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  ds:SCREEN_SEG_PUMA
                cmp     bl, bCtrlChnl
                jne     PCRS_Exit
                call    PUMA_Refresh_Scr
                xor     al, al
                call    PUMA_Answer
PCRS_Exit:      pop	ds
                ASSUME  ds:DATA_AXS
		ret
PC_Refresh_Scr  ENDP

                ; --- ���牀� 祚��←キ��
PC_Ctrl_On      PROC
                call    PUMA_Set_Ctrl
                ret
PC_Ctrl_On      ENDP

                ; --- �皙キ� 祚��←キ��
PC_Ctrl_Off     PROC
                call    PUMA_Reset_Ctrl
                ret
PC_Ctrl_Off     ENDP

                ; --- ���※��
PC_Key          PROC
		push	es
		pushad

                mov     es, ss:SEG_PUMA_SCR
                cmp     bl, es:bCtrlChnl
                jne     PCK_Exit
IF _MSP NE TRUE
                mov     INT_5, 0                ; 瓠牀� �爬Л��� �爬ガ� ���※荐
ENDIF
                mov     al, es:[di + 6]         ;
                @INT_5

PCK_Exit:       popad
		pop	es
		ret
PC_Key          ENDP

                ; --- ���牀� 爛Θ�� ���筥ｃ��罔�
PC_Cfg_On       PROC
                push    ds
                pushad
                push    ebx                     ; か� PUMA_Answer
                ;
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  ds:SCREEN_SEG_PUMA
                ; --- �����┃�ガ �����モ琺 爛Θ�� ���筥ｃ��罔�
                mov     eax, es:[edi + 6]
                mov     DWORD PTR bBlockType, eax
                mov     eax, es:[edi + 10]
                mov     DWORD PTR wStartSector, eax
                ; --- √�き襯 �牀▲爲�
                ; 皀�竕┤ 爛Θ�
                mov     al, 201                 ; ゛�� 礒� � 爛Θ�� ���筥ｃ��罔�
                cmp     bCfg, 0
                jne     PCCOn_Answer
                ; 皋� ゛���
                mov     al, 202                 ; �アム�覃 皋� ゛���
                cmp     bBlockType, _TYPE_BLOCK
                jne     PCCOn_Answer
                ; ----- -- - - -- -
                cmp     bDataType, 3
                jne     @@Not_Urengoy
                ;;mov     bDataType, 5                    ; Special for New Urengoy!
                ; --- 皋� ����諷 ?
@@Not_Urengoy:  mov     ecx, _FILE_PRG_SIZE             ; か� ���
                ; --- �牀������ ?
                cmp     bDataType, 0
IF _MSP NE TRUE
                je      PCCOn_Prg
ELSE
                je      PCCOn_Size
ENDIF
                mov     ecx, _FILE_CFG_SIZE
                ; --- ���筥ｃ��罔� ?
                cmp     bDataType, 1
                je      PCCOn_Size
                mov     ecx, _FILE_BLST_SIZE            ; か� ���
                ; --- �牀荐→� ?
                cmp     bDataType, 3
IF _MSP NE TRUE
                je      PCCOn_Prg

ELSE
                je      PCCOn_Size
ENDIF
                ; --- "腑爿覃" 甎�甌� ?
                cmp     bDataType, 5
                je      PCCOn_Size
                mov     al, 205                 ; 皋� ����諷 �� ��いムΘ��モ瘴
                jmp     PCCOn_Answer
                ; --- ��Кム ����諷 ?
PCCOn_Size:     mov     al, 203                 ; �アム�覃 ��Кム
                movzx   edx, wSectorCount
                shl     edx, 7
                cmp     ecx, edx
                jne     PCCOn_Answer
                ; --- ��茱� ┼� 腮��ガ
PCCOn_Prg:
                cmp     bOperType, 0
                je      PCCOn_Rd
                cmp     bOperType, 1
                je      PCCOn_Wr
                mov     al, 206                 ; ��ム�罔� �ク�いムΘ��モ瘴
                jmp     PCCOn_Answer
                ; === ����瘡 ========================================
PCCOn_Wr:       ;
IF _MSP NE TRUE
                ; --- �牀������
                cmp     bDataType, 0
                jne     PCCOn_Wr_Cfg
                lea     edx, _FILE_PRG_NAME_T
                jmp     PCCOn_Is_Opn
                ; ----        -----
PCCOn_Wr_Cfg:   ; --- ���筥ｃ��罔�
                cmp     bDataType, 1
                jne     PCCOn_Wr_Rbf
                lea     edx, _FILE_CFG_NAME_T
                jmp     PCCOn_Is_Crte
                ; ----        -----
PCCOn_Wr_Rbf:   ; --- ぎ�����皀�讚�� ┃筮爼�罔�(.rbf)
                cmp     bDataType, 3
                jne     PCCOn_Wr_Blst
                lea     edx, _FILE_RBF_NAME_T
                jmp   PCCOn_Is_Opn
                ; -------      -------
PCCOn_Wr_BLst:  ; --- "苟爿覃" 甎�甌�
                mov     al, 205                 ; 皋� ����諷 �� ��いムΘ��モ瘴
                cmp     bDataType, 5
                jne     PCCOn_Err
                lea     edx, _FILE_BLST_NAME_T
PCCOn_Is_Crte:  ; --- 甌Г�え�
                cmp     dFileID, 0FFFFFFFFh
                je      @@good_create
                ; --- 甅����� ���牀ガ 轤�� ID
                push    eax
                mov     eax, dFileID
		CALL_HDD FileClose
                pop     eax
                mov     bFileOpen, 0
                ; --- Гメ� ��爼��讚�� 甌Г��┘ �����
@@good_create:  push    ds
		mov	ds, ss:SEG_AX
                CALL_HDD FileCreateSize
		pop	ds
                jc      PCCOn_Err
                mov     dFileID, eax
                jmp     PCCOn_OK
                ; --------------
PCCOn_Is_Opn:   ; --- 甌Г�え�
                cmp     dFileID, 0FFFFFFFFh
                je      @@ok_create
                ; --- 甅����� ���牀ガ 轤�� ID
                push    eax
                mov     eax, dFileID
		CALL_HDD FileClose
                pop     eax
                mov     bFileOpen, 0
                ; --- Гメ� ��爼��讚�� 甌Г��┘ �����
@@ok_create:    push    ds
		mov	ds, ss:SEG_AX
                CALL_HDD FileCreate
		pop	ds
                jc      PCCOn_Err
                mov     dFileID, eax
ENDIF
                jmp     PCCOn_OK
                ; --------------
                ; === 艪キ┘ ========================================
PCCOn_Rd:
IF _MSP NE TRUE
                ; --- �牀������ ---
                cmp     bDataType, 0
                jne     PCCOn_Rd_Cfg
                lea     edx, _FILE_PRG_NAME
                jmp     PCCOn_Rd_Open
PCCOn_Rd_Cfg:   ; --- ���筥ｃ��罔�
                cmp     bDataType, 1
                jne     PCCOn_Rd_Rbf
                lea     edx, _FILE_CFG_NAME
                jmp     PCCOn_Rd_Open
PCCOn_Rd_Rbf:   ; --- �牀荐→� ���硅
                cmp     bDataType, 3
                jne     PCCOn_Rd_BLst
                lea     edx, _FILE_RBF_NAME
                jmp     PCCOn_Rd_Open
                ; --- "苟爿覃" 甎�甌�
PCCOn_Rd_BLst:  mov     al, 205                 ; 皋� ����諷 �� ��いムΘ��モ瘴
                cmp     bDataType, 5
                jne     PCCOn_Answer
                lea     edx, _FILE_BLST_NAME
PCCOn_Rd_Open:  ; --- �皖牀ガ
                cmp     dFileID, 0FFFFFFFFh
                je      @@ok_open
                ; --- 甅����� ���牀ガ 轤�� ID
                push    eax
                mov     eax, dFileID
		CALL_HDD FileClose
                pop     eax
                mov     bFileOpen, 0
                ; --- Гメ� ��爼��讚�� �皖琺皋� �����
@@ok_open:      push    ds
                mov     ds, ss:SEG_AX
                CALL_HDD FileOpen
                pop     ds
                jc      PCCOn_Err
                mov     dFileID, eax
ENDIF
                jmp     PCCOn_OK

PCCOn_Err:   ; --- �荐！� 甌Г����
                mov     al, 204
                jmp     PCCOn_Answer
PCCOn_OK:       ;--- 〓茫� � 爛Θ� ���筥ｃ��罔�
                mov     wNextSector, 0
                mov     bCfg, 255
IF _MSP NE TRUE
                mov     bFileOpen, 255
ENDIF
                xor     al, al
PCCOn_Answer:   pop     ebx
                call    PUMA_Answer
                ;
                popad
                pop     ds
                ASSUME  ds:DATA_AXS
                ret
PC_Cfg_On       ENDP

                ; --- ��▲琥キ┘ 爛Θ�� ���筥ｃ��罔�
PC_Cfg_Off      PROC
                push    ds
                pushad
                push    ebx                     ; か� PUMA_Answer
                push    edi
                ;
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  ds:SCREEN_SEG_PUMA
                ; 皀�竕┤ 爛Θ�
                mov     al, 208                 ; ゛�� �� � 爛Θ�� ���筥ｃ��罔�
                cmp     bCfg, 0
                je      PCCOff_Answer

                ; --- °�� ����瘡 ?
                cmp     bOperType, 1
                jne     PCCOff_Clear               ; �モ

                ; ≡� �������� ?
                mov     al, 209                 ; ��ム�罔� �� ��▲琥キ�
                mov     bx, wNextSector
                cmp     bx, wSectorCount
                jb      PCCOff_Answer
IF _MSP NE TRUE
                ; ���牀ガ ����, ��皰琺� �ムイ���
                mov     al, 218
                cmp     bFileOpen, 0
                je      PCCOff_Answer
                mov     bFileOpen, 0
                mov     eax, dFileID
		CALL_HDD FileClose
                mov     al, 210                 ; �荐！� ���琺皋� �����
                jc      PCCOff_Answer
                ; �ムョ���ガ ���竍キ�襯 ����襯 � ���閧�
                ; ========     =========
                cmp     bDataType, 0
                jne     PCCOff_Cfg
                ; --- �牀������
                @The_End_Conf _FILE_PRG_NAME, _FILE_PRG_NAME_C, _FILE_PRG_NAME_T, @1, @2, @@1
                jmp     PCCOff_Clear
                ; ----          ----
PCCOff_Cfg:     cmp     bDataType, 1
                jne     PCCOff_Rbf
                ; --- ���筥ｃ��罔�
if 0
                push    ds
                mov     ds, ss:SEG_AX
                mov     edx, OFFSET _FILE_CFG_NAME_T
                CALL_HDD FileOpen
                jc      PCCOff_Err
                mov     edx, OFFSET _FILE_CFG_NAME
                CALL_HDD FileEraseName
                mov     edx, OFFSET _FILE_CFG_NAME
                CALL_HDD FileRename
                jc      PCCOff_Err
                xor     edx, edx
                mov     ecx, _FILE_CFG_SIZE
                CALL_HDD FileRead
                jc      PCCOff_Err
                pop     ds
endif
                @The_End_Conf _FILE_CFG_NAME, _FILE_CFG_NAME_C, _FILE_CFG_NAME_T, @3, @4, @@3
                ; --- �牀腮��ガ ���覃 ����
                push    ds
                mov     ds, ss:SEG_AX
                mov     edx, OFFSET _FILE_CFG_NAME
                CALL_HDD FileOpen
                jc      PCCOff_Err
                xor     edx, edx
                mov     ecx, _FILE_CFG_SIZE
                CALL_HDD FileRead
                jc      PCCOff_Err
                CALL_HDD FileClose
                jc      PCCOff_Err
                pop     ds
                ; -----       ------
                jmp     PCCOff_Clear
                ; ----          ----
PCCOff_Rbf:     cmp     bDataType, 3
                jne     PCCOff_Blst
                ; --- ぎ�. ┃��(.rbf)
                @The_End_Conf _FILE_RBF_NAME, _FILE_RBF_NAME_C, _FILE_RBF_NAME_T, @5, @6, @@5
                jmp     PCCOff_Clear
                ; ----          ----
PCCOff_BLst:    cmp     bDataType, 5
                jne     PCCOff_Answer
                ; --- "苟爿覃" 甎�甌�
if 0
                push    ds
                mov     ds, ss:SEG_AX
                mov     edx, OFFSET _FILE_BLST_NAME_T
		CALL_HDD FileOpen
                jc      PCCOff_Err
                mov     edx, OFFSET _FILE_BLST_NAME
                CALL_HDD FileEraseName
                CALL_HDD FileRename
                jc      PCCOff_Err
                mov     ds, ss:SEG_BLK_LST
                xor     edx, edx
                mov     ecx, _FILE_BLST_SIZE
		CALL_HDD FileRead
                jc      PCCOff_Err
                CALL_HDD FileClose
                jc      PCCOff_Err
                pop     ds
endif
                @The_End_Conf _FILE_BLST_NAME, _FILE_BLST_NAME_C, _FILE_BLST_NAME_T, @7, @8, @@7
                ; --- �牀腮��ガ ���覃 ����
                push    ds
                mov     ds, ss:SEG_AX
                mov     edx, OFFSET _FILE_BLST_NAME
                CALL_HDD FileOpen
                jc      PCCOff_Err
                mov     ds, ss:SEG_BLK_LST
                xor     edx, edx
                mov     ecx, _FILE_BLST_SIZE
                CALL_HDD FileRead
                jc      PCCOff_Err
                CALL_HDD FileClose
                jc      PCCOff_Err
                pop     ds
                ; -----       ------
                jmp     PCCOff_Clear
                ; ----          ----
                ; =======      ========
ELSE
                ; 甃キ━ 皀�竕竡 ����� � �ムョ���ガ
                push    ds
                mov     ds, ss:SEG_AX
                ASSUME  ds:DATA_AXS
                call    FLChangeActCopy         ; �爬�鍮�
                call    FLReInicialize          ; �ムョ����� メ�� ��ぎ
                call    FLIndicate
                mov     FLCurTaskID, _NOP_ID
                pop     ds
                ASSUME  ds:SCREEN_SEG_PUMA
                jmp     PCCOff_Clear
ENDIF
                ; --- �腮痰━ �����モ琺 爛Θ�� ���筥ｃ��罔�
PCCOff_Clear:
                xor     eax, eax
                mov     DWORD PTR bBlockType, eax
                mov     DWORD PTR wStartSector, eax
                xor     al, al
IF _MSP NE TRUE
                jmp     PCCOff_Answer
                ; �荐！� ���琺皋� �����
PCCOff_Err:     pop     ds
                mov     al, 217                 ; �荐！� �´�←キ�� ����諷
ENDIF
                ; � �遏�� 甄竍�� �諷�� ├ 爛Θ�� ���筥ｃ��罔�
PCCOff_Answer:  mov     bCfg, 0
                pop     edi
                pop     ebx
                call    PUMA_Answer
                ;
                popad
                pop     ds
                ASSUME  ds:DATA_AXS
                ret
PC_Cfg_Off      ENDP

                ; --- �ムイ��� 瓮�皰�� ����諷 ゛���
PC_Sector_In    PROC
                push    ds
                pushad
                push    ebx                     ; か� PUMA_Answer
                push    edi
                ;
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  ds:SCREEN_SEG_PUMA
                mov     al, 211                 ; ゛�� �� � 爛Θ�� ���筥ｃ��罔�
                cmp     bCfg, 0
IF _MSP EQ TRUE
                je      PCSI_Answer
ELSE
                je      PCSI_Err
ENDIF
                ; 皀�竕�� ��ム�罔� 艪キ┘ ?
                mov     al, 212                 ; ゛�� �� � 爛Θ�� ����瓱
                cmp     bOperType, 1
IF _MSP EQ TRUE
                jne     PCSI_Answer
ELSE
                jne     PCSI_Err
ENDIF
                ; �牀▲爲� 皀�竕ィ� � 瓮�皰��
                mov     al, 213                 ; �アム�覃 ���ム �腑爛き��� 瓮�皰��
                mov     bx, es:[edi + 6]
                cmp     bx, wNextSector
IF _MSP EQ TRUE
                jne     PCSI_Answer
ELSE
                jne     PCSI_Err
ENDIF
                mov     al, 214                 ; ���牀� ��茘ィ� 瓮�皰��
                cmp     bx, wSectorCount
IF _MSP EQ TRUE
                ja     PCSI_Answer
ELSE
                ja     PCSI_Err
ENDIF

IF _MSP EQ TRUE
                mov     esi, edi                ; 甌縲��━ 礫���皀�� �� �爬�閧覃 ���モ
                ; �� 皋�� ����諷 ���竍�ガ ��М�覃 礫���皀��
                movzx   eax, bDataType
                cmp     al, _CONFG_ID
                jbe     PCSI_Set_Task
                mov     al, _DOPIN_ID
PCSI_Set_Task:  push    es
                mov     es, ss:SEG_AX
                mov     es:FLCurTaskID, al
                mov     dl, es:FLCurActCopyB[eax];皀�竕�� ���筥ｃ��罔�
                and     dl, 1                   ; �─� 0, �─� 1
                xor     dl, 1                   ; �キ錺�
                mov     edi, es:FLFirstCopyAddrD[eax * 4]
                cmp     dl, 1
                jne     PCSI_Get_Ptr
                mov     edi, es:FLSecndCopyAddrD[eax * 4]
PCSI_Get_Ptr:   pop     es
                ; 皀�ム� �諛�甄錺� 皀�竕┤ 礫���皀��
                and     ebx,0ffffh
                shl     ebx,7                   ; *128
                add     edi, ebx                ; � edi-�むメ 皀�竕ィ� ゛��� �� 筰ヨ
                ; 甌＝癶キ�� ����瘡
                add     esi, 8                  ; 礫���皀�� � ������ ���モ� �� ����襯 � ���モ�
                mov     ecx, 40h                ; 甄��
                call    ELAN400_WRITE_FLC
                inc     wNextSector
                jmp     PCSI_OK
ELSE
                ; �モ �皖琺皰�� �����
                mov     al, 215                 ; �モ �皖琺皰�� �����
                cmp     bFileOpen, 0
                je      PCSI_Answer
                ; 礫���皀�� �� �爬�閧覃 ���モ � ds:[edx] ├ es:[edi]
                push    ds
                push    es
                pop     ds
                mov     edx, edi
		add	edx, 8
                ; ��茱� !
                mov     ecx, 80h
                mov     eax, dFileID
                CALL_HDD FileWrite
                pop     ds
                mov     al, 216                 ; �荐！� ����瓱 瓮�皰��
                jc      PCSI_Err
                ; �� - ��������
                inc     wNextSector
                jmp     PCSI_OK
                ; �諷�� �� �荐！� � ���琺皋ガ �����
PCSI_Err:       push    eax
                mov     eax, dFileID
		CALL_HDD FileClose
                pop     eax
                mov     bFileOpen, 0
                jmp     PCSI_Answer
ENDIF
                ; ��も▲爨��ガ
PCSI_OK:        xor     al, al
PCSI_Answer:    pop     edi
                pop     ebx
                call    PUMA_Answer
                ;
                popad
                pop     ds
                ASSUME  ds:DATA_AXS
                ret
PC_Sector_In    ENDP

                ; --- ���牀� 瓮�皰�� ����諷 � ゛���
PC_Sector_Out   PROC
                push    ds
                pushad
                push    ebx                     ; か� PUMA_Answer
                ;
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  ds:SCREEN_SEG_PUMA
                mov     al, 211                 ; ゛�� �� � 爛Θ�� ���筥ｃ��罔�
                cmp     bCfg, 0
                je      PCSO_Answer
                ; 皀�竕�� ��ム�罔� 艪キ┘ ?
                mov     al, 212                 ; ゛�� �� � 爛Θ�� 艪キ��
                cmp     bOperType, 0
                jne     PCSO_Answer
                ; �牀▲爲� 皀�竕ィ� � 瓮�皰��
                mov     al, 213                 ; �アム�覃 ���ム �腑爛き��� 瓮�皰��
                mov     bx, es:[edi + 6]
                cmp     bx, wNextSector
                jne     PCSO_Answer
                mov     al, 214                 ; ���牀� ��茘ィ� 瓮�皰��
                cmp     bx, wSectorCount
                ja      PCSO_Answer
IF _MSP NE TRUE
                ; �モ �皖琺皰�� �����
                mov     al, 215                 ; �モ �皖琺皰�� �����
                cmp     bFileOpen, 0
                je      PCSO_Answer
                ; ��腮��ガ 筮爼�牀��碎 �癶モ�覃 ���モ �� es:[edi]
ELSE
                mov     ebp, edi
ENDIF
                mov     edx, es:[edi]
                @EDX_Back_Addr
                mov     es:[edi], edx              ; �÷�皚覃 �むメ
                or      BYTE PTR es:[edi + 5], 80h ; ��� �癶モ� ├ ���� ���牀��
IF _MSP EQ TRUE
                mov     WORD PTR es:[edi + 6], bx  ; ���ム �ムイ���ガ��� 瓮�皰��
                ; �� 皋�� ����諷 ���竍�ガ ��М�覃 礫���皀��
                movzx   eax, bDataType
                cmp     al, _CONFG_ID
                jbe     PCSO_Set_Task
                mov     al, _DOPIN_ID
PCSO_Set_Task:  push    es
                mov     es, ss:SEG_AX
                mov     es:FLCurTaskID, al
                mov     dl, es:FLCurActCopyB[eax];皀�竕�� ���筥ｃ��罔�
                and     dl, 1                   ; �─� 0, �─� 1
                mov     esi, es:FLFirstCopyAddrD[eax * 4]
                cmp     dl, 1
                jne     PCSO_Get_Ptr
                mov     esi, es:FLSecndCopyAddrD[eax * 4]
PCSO_Get_Ptr:   pop     es
                ; 皀�ム� �諛�甄錺� 皀�竕┤ 礫���皀��
                and     ebx, 0ffffh
                shl     ebx, 7                  ; *128
                add     esi, ebx                ; � edi-�むメ 皀�竕ィ� ゛��� �� 筰ヨ
                ; 甌＝癶キ�� 艪キ┘
                add     edi, 8                  ; 礫���皀�� � ������ ���モ� �� ����襯 � ���モ�
                mov     ecx, 40h                ; 甄��
                call    ELAN400_READ_FLC
ELSE
                movzx   esi, wNextSector
                mov     WORD PTR es:[edi + 6], si  ; ���ム �ムイ���ガ��� 瓮�皰��

PCSO_Rd_Cnf:    ; ~~~ 腮��ガ
                push    edx
                mov     edx, edi
                add     edx, 8
                mov     ecx, 80h
                mov     eax, dFileID
                CALL_HDD FileRead
                jc      @@Rd_bad
                pop     edx
                jmp     @@Go_next
@@Rd_bad:
                pop     edx
                mov     al, 220         ; �荐！� 艪キ�� 瓮�皰�� ����諷
                push    eax
                mov     eax, dFileID
                CALL_HDD FileClose
                pop     eax
                mov     bFileOpen, 0
                jmp     PCSO_Answer


ENDIF
@@Go_next:
                inc     wNextSector
                ; �皴��←錺�
                pop     ebx
                mov     ecx, 128 + 8
		push	es
		pop	ds
IF _MSP EQ TRUE
                mov     esi, ebp
                call    PUMA_Out
                jmp     PCSO_Exit
                ;
PCSO_Answer:    pop     ebx
                mov     BYTE PTR es:[edi + 5], 8fh ; �諷�� ├ 爛Θ�� ���筥ｃ��罔�
                mov     es:[edi + 6], al  ; ���ム �ムイ���ガ��� 瓮�皰��
                mov     ecx, 7
		push	es
		pop	ds
ENDIF
		mov	esi, edi
                call    PUMA_Out
                ;;;jnc     PCSO_Ans                    ; �� �皴��※��
IF _MSP NE TRUE
                jmp     PCSO_Exit
                ;
PCSO_Answer:    pop     ebx
                call    PUMA_Answer
ENDIF
                ;
PCSO_Exit:      popad
                pop     ds
                ASSUME  ds:DATA_AXS
                ret
PC_Sector_Out   ENDP

                ; === ��爬筥��罔���襯 ===============================
                ; --- ��も▲爨ぅ�┘ ��爬筥��罔�
PC_Tar_Ack      PROC
		push	es
		pushad
                mov     ax, es:[edi + 6]        ; �� �� � ���モ�
                mov     PEREM_AX, ax
                @POLUCH_ADR_MO                  ;
                or      dl, dl                  ; �牀▲爬� メ碎 �メ皰 ?
                jnz     PCTA_Exit               ; �モ - 礬ぅ�
                ; メ碎 �メ皰 - ≡� Ε ����茱�
                mov     eax, 030404ebh          ; 235-�� か┃�� 3
                stosd
                mov     al, 13                  ; ������� 13 - ��も▲爨ぅ�┘ ���. ���モ��
                stosb
                mov     ax, PEREM_AX            ; �����モ琺
                stosw
                @ZAPIS_OK_MO
PCTA_Exit:      popad
		pop	es
		ret
PC_Tar_Ack      ENDP

                ; --- ∇ガ�
PC_Set_Time     PROC
                push    ds
                push    es
                pushad
                ; ---
                mov     ds, ss:SEG_PUMA_SCR
                mov     esi, edi
                add     esi, 6
                mov     es, ss:SEG_AX
                mov     edi, OFFSET GOD_
                mov     ecx, 6
PCST_Loop:      lodsb
                call    Hex_To_BCD
                stosb
                loop    PCST_Loop
                ; ---
                popad
                pop     es
                pop     ds
                ret
PC_Set_Time     ENDP

                ; --- ��く�瓷� �� ��爬筥��罔�
PC_Subscr_Tar   PROC
                xor     al, al
                call    PUMA_Answer
                ret
PC_Subscr_Tar   ENDP

                ; --- ��く�瓷� �� ���爬�
PC_Subscr_Alarm PROC
                xor     al, al
                call    PUMA_Answer
                ret
PC_Subscr_Alarm ENDP





