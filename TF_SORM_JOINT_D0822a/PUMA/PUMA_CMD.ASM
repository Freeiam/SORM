; ╔════════════════════════════════════════════════════════════════════════════╗
; ║  Файл:         PUMA_Cmd.ASM                                                ║
; ║  Программист:  Тоневицкий Ю.Д.                                             ║
; ╟────────────────────────────────────────────────────────────────────────────╢
; ║  Использует:   PUMA_Scr.DAT, RingBuff.ASM                                  ║
; ╟────────────────────────────────────────────────────────────────────────────╢
; ║  Назначение:   Обработчик принятых команд ПУМы                             ║
; ╟────────────────────────────────────────────────────────────────────────────╢
; ║  Содержит:                                                                 ║
; ║                                                                            ║
; ║                                                                            ║
; ╚════════════════════════════════════════════════════════════════════════════╝

;CfgOn
;201                 ; блок уже в режиме конфигурации
;202                 ; неверный тип блока
;203                 ; неверный размер
;204                 ; ошибка создания
;205                 ; тип данных не поддерживается
;206                 ; операция неподдерживается
; 207 - резерв, ошибка открытия
;CfgOff
;208                 ; блок не в режиме конфигурации
;209                 ; операция не завершена
;210                 ; ошибка закрытия файла
;217                 ; ошибка обновления данных
;218                 ; странно, но факт - нет открытого файла
;CfgIn-CfgOff
;211                 ; блок не в режиме конфигурации
;212                 ; блок не в режиме записи
;213                 ; неверный номер очередного сектора
;214                 ; запрос лишнего сектора
;215                 ; нет открытого файла
;216                 ; ошибка записи сектора (только в CfgIn)
;CfgOut
;220                 ; ошибка чтения сектора данных


PUMA_Cmd        PROC
                ; === пультовые =====================================
                ; --- запрос сервера на подключение
PC_037f:        cmp     WORD PTR es:[di + 4], 07f03h
                jne     PC_8000
                call    PC_Serv_On
                jmp     PC_Exit
                ; === общие =========================================
                ; --- запрос типа и состояния блока
PC_8000:        cmp     WORD PTR es:[di + 4], 00080h
                jne     PC_8002
                call    PC_Get_Type
                jmp     PC_Exit
                ; --- запрос обновления экрана
PC_8002:        cmp     WORD PTR es:[di + 4], 00280h
                jne     PC_8008
                call    PC_Refresh_Scr
                jmp     PC_Exit
                ; --- запрос управления
PC_8008:        cmp     WORD PTR es:[di + 4], 00880h
                jne     PC_8009
                call    PC_Ctrl_On
                jmp     PC_Exit
                ; --- отмена управления
PC_8009:        cmp     WORD PTR es:[di + 4], 00980h
                jne     PC_800a
                call    PC_Ctrl_Off
                jmp     PC_Exit
                ; --- клавиша
PC_800a:        cmp     WORD PTR es:[di + 4], 00a80h
                jne     PC_800c
                call    PC_Key
                jmp     PC_Exit
                ; --- запрос режима конфигурации
PC_800c:        cmp     WORD PTR es:[di + 4], 00c80h
                jne     PC_800d
                call    PC_Cfg_On
                jmp     PC_Exit
                ; --- завершение режима конфигурации
PC_800d:        cmp     WORD PTR es:[di + 4], 00d80h
                jne     PC_800e
                call    PC_Cfg_Off
                jmp     PC_Exit
                ; --- передача сектора данных блоку
PC_800e:        cmp     WORD PTR es:[di + 4], 00e80h
                jne     PC_8010
                call    PC_Sector_In
                jmp     PC_Exit
                ; --- запрос сектора данных у блока
PC_8010:        cmp     WORD PTR es:[di + 4], 01080h
                jne     PC_8100
                call    PC_Sector_Out
                jmp     PC_Exit
                ; === тарификационные ===============================
                ; --- подтверждение тарификации
PC_8100:        cmp     WORD PTR es:[di + 4], 00081h
                jne     PC_8120
                call    PC_Tar_Ack
                jmp     PC_Exit
                ; --- время
PC_8120:        cmp     WORD PTR es:[di + 4], 02081h
                jne     PC_8121
                call    PC_Set_Time
                jmp     PC_Exit
                ; --- подписка на тарификацию
PC_8121:        cmp     WORD PTR es:[di + 4], 02181h
                jne     PC_8122
                call    PC_Subscr_Tar
                jmp     PC_Exit
                ; --- подписка на аварии
PC_8122:        cmp     WORD PTR es:[di + 4], 02281h
                jne     PC_Exit
                call    PC_Subscr_Alarm
                jmp     PC_Exit
                ; ===
PC_Exit:        ret
PUMA_Cmd        ENDP



                ; === пультовые =====================================
                ; --- запрос сервера на подключение
PC_Serv_On      PROC
                xor     al, al
                call    PUMA_Answer
                ret
PC_Serv_On      ENDP

                ; === общие =========================================
                ; --- запрос типа и состояния блока
PC_Get_Type     PROC
                push    ds
                push    es
                pushad
                ; ук. на отпраляемый пакет ds:[esi] из ук. на принятый пакет es:[edi]
                push    es
                pop     ds
                mov     esi, edi
                ; меняем ПУМовский адрес на ответный
                mov     edx, [esi]
                @EDX_back_Addr
                mov     [esi], edx
                ; меняем код команды на ответный
                or      BYTE PTR [esi + 5], 80h
                ; длина
                mov     ecx, 16
                ; параметры ответа
                mov     BYTE  PTR [esi + 6],  _TYPE_BLOCK
                mov     WORD  PTR [esi + 7],  0fffh
                mov     DWORD PTR [esi + 8],  0
                mov     DWORD PTR [esi + 12], 0
                ; из № канала № передающего буфера
                mov     edx, ebx
                shl     edx, 1
                mov     es, ss:SEG_PUMA
                ; --- отправка
                call    RB_MPut
                ; формируем результат
                popad
                pop     es
                pop     ds
                ret
PC_Get_Type     ENDP

                ; --- запрос обновления экрана
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

                ; --- запрос управления
PC_Ctrl_On      PROC
                call    PUMA_Set_Ctrl
                ret
PC_Ctrl_On      ENDP

                ; --- отмена управления
PC_Ctrl_Off     PROC
                call    PUMA_Reset_Ctrl
                ret
PC_Ctrl_Off     ENDP

                ; --- клавиша
PC_Key          PROC
		push	es
		pushad

                mov     es, ss:SEG_PUMA_SCR
                cmp     bl, es:bCtrlChnl
                jne     PCK_Exit
IF _MSP NE TRUE
                mov     INT_5, 0                ; сброс признака приема клавиши
ENDIF
                mov     al, es:[di + 6]         ;
                @INT_5

PCK_Exit:       popad
		pop	es
		ret
PC_Key          ENDP

                ; --- запрос режима конфигурации
PC_Cfg_On       PROC
                push    ds
                pushad
                push    ebx                     ; для PUMA_Answer
                ;
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  ds:SCREEN_SEG_PUMA
                ; --- запоминаем параметры режима конфигурации
                mov     eax, es:[edi + 6]
                mov     DWORD PTR bBlockType, eax
                mov     eax, es:[edi + 10]
                mov     DWORD PTR wStartSector, eax
                ; --- входные проверки
                ; текущий режим
                mov     al, 201                 ; блок уже в режиме конфигурации
                cmp     bCfg, 0
                jne     PCCOn_Answer
                ; тип блока
                mov     al, 202                 ; неверный тип блока
                cmp     bBlockType, _TYPE_BLOCK
                jne     PCCOn_Answer
                ; ----- -- - - -- -
                cmp     bDataType, 3
                jne     @@Not_Urengoy
                ;;mov     bDataType, 5                    ; Special for New Urengoy!
                ; --- тип данных ?
@@Not_Urengoy:  mov     ecx, _FILE_PRG_SIZE             ; для МСП
                ; --- программа ?
                cmp     bDataType, 0
IF _MSP NE TRUE
                je      PCCOn_Prg
ELSE
                je      PCCOn_Size
ENDIF
                mov     ecx, _FILE_CFG_SIZE
                ; --- конфигурация ?
                cmp     bDataType, 1
                je      PCCOn_Size
                mov     ecx, _FILE_BLST_SIZE            ; для МСП
                ; --- прошивка ?
                cmp     bDataType, 3
IF _MSP NE TRUE
                je      PCCOn_Prg

ELSE
                je      PCCOn_Size
ENDIF
                ; --- "черный" список ?
                cmp     bDataType, 5
                je      PCCOn_Size
                mov     al, 205                 ; тип данных не поддерживается
                jmp     PCCOn_Answer
                ; --- размер данных ?
PCCOn_Size:     mov     al, 203                 ; неверный размер
                movzx   edx, wSectorCount
                shl     edx, 7
                cmp     ecx, edx
                jne     PCCOn_Answer
                ; --- пишем или читаем
PCCOn_Prg:
                cmp     bOperType, 0
                je      PCCOn_Rd
                cmp     bOperType, 1
                je      PCCOn_Wr
                mov     al, 206                 ; операция неподдерживается
                jmp     PCCOn_Answer
                ; === запись ========================================
PCCOn_Wr:       ;
IF _MSP NE TRUE
                ; --- программа
                cmp     bDataType, 0
                jne     PCCOn_Wr_Cfg
                lea     edx, _FILE_PRG_NAME_T
                jmp     PCCOn_Is_Opn
                ; ----        -----
PCCOn_Wr_Cfg:   ; --- конфигурация
                cmp     bDataType, 1
                jne     PCCOn_Wr_Rbf
                lea     edx, _FILE_CFG_NAME_T
                jmp     PCCOn_Is_Crte
                ; ----        -----
PCCOn_Wr_Rbf:   ; --- дополнительная информация(.rbf)
                cmp     bDataType, 3
                jne     PCCOn_Wr_Blst
                lea     edx, _FILE_RBF_NAME_T
                jmp   PCCOn_Is_Opn
                ; -------      -------
PCCOn_Wr_BLst:  ; --- "чёрный" список
                mov     al, 205                 ; тип данных не поддерживается
                cmp     bDataType, 5
                jne     PCCOn_Err
                lea     edx, _FILE_BLST_NAME_T
PCCOn_Is_Crte:  ; --- создадим
                cmp     dFileID, 0FFFFFFFFh
                je      @@good_create
                ; --- сначала закроем этот ID
                push    eax
                mov     eax, dFileID
		CALL_HDD FileClose
                pop     eax
                mov     bFileOpen, 0
                ; --- здесь нормальное создание файла
@@good_create:  push    ds
		mov	ds, ss:SEG_AX
                CALL_HDD FileCreateSize
		pop	ds
                jc      PCCOn_Err
                mov     dFileID, eax
                jmp     PCCOn_OK
                ; --------------
PCCOn_Is_Opn:   ; --- создадим
                cmp     dFileID, 0FFFFFFFFh
                je      @@ok_create
                ; --- сначала закроем этот ID
                push    eax
                mov     eax, dFileID
		CALL_HDD FileClose
                pop     eax
                mov     bFileOpen, 0
                ; --- здесь нормальное создание файла
@@ok_create:    push    ds
		mov	ds, ss:SEG_AX
                CALL_HDD FileCreate
		pop	ds
                jc      PCCOn_Err
                mov     dFileID, eax
ENDIF
                jmp     PCCOn_OK
                ; --------------
                ; === чтение ========================================
PCCOn_Rd:
IF _MSP NE TRUE
                ; --- программа ---
                cmp     bDataType, 0
                jne     PCCOn_Rd_Cfg
                lea     edx, _FILE_PRG_NAME
                jmp     PCCOn_Rd_Open
PCCOn_Rd_Cfg:   ; --- конфигурация
                cmp     bDataType, 1
                jne     PCCOn_Rd_Rbf
                lea     edx, _FILE_CFG_NAME
                jmp     PCCOn_Rd_Open
PCCOn_Rd_Rbf:   ; --- прошивка платы
                cmp     bDataType, 3
                jne     PCCOn_Rd_BLst
                lea     edx, _FILE_RBF_NAME
                jmp     PCCOn_Rd_Open
                ; --- "чёрный" список
PCCOn_Rd_BLst:  mov     al, 205                 ; тип данных не поддерживается
                cmp     bDataType, 5
                jne     PCCOn_Answer
                lea     edx, _FILE_BLST_NAME
PCCOn_Rd_Open:  ; --- откроем
                cmp     dFileID, 0FFFFFFFFh
                je      @@ok_open
                ; --- сначала закроем этот ID
                push    eax
                mov     eax, dFileID
		CALL_HDD FileClose
                pop     eax
                mov     bFileOpen, 0
                ; --- здесь нормальное открытие файла
@@ok_open:      push    ds
                mov     ds, ss:SEG_AX
                CALL_HDD FileOpen
                pop     ds
                jc      PCCOn_Err
                mov     dFileID, eax
ENDIF
                jmp     PCCOn_OK

PCCOn_Err:   ; --- ошибка создания
                mov     al, 204
                jmp     PCCOn_Answer
PCCOn_OK:       ;--- вошли в режим конфигурации
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

                ; --- завершение режима конфигурации
PC_Cfg_Off      PROC
                push    ds
                pushad
                push    ebx                     ; для PUMA_Answer
                push    edi
                ;
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  ds:SCREEN_SEG_PUMA
                ; текущий режим
                mov     al, 208                 ; блок не в режиме конфигурации
                cmp     bCfg, 0
                je      PCCOff_Answer

                ; --- была запись ?
                cmp     bOperType, 1
                jne     PCCOff_Clear               ; нет

                ; всё записали ?
                mov     al, 209                 ; операция не завершена
                mov     bx, wNextSector
                cmp     bx, wSectorCount
                jb      PCCOff_Answer
IF _MSP NE TRUE
                ; закроем файл, который передали
                mov     al, 218
                cmp     bFileOpen, 0
                je      PCCOff_Answer
                mov     bFileOpen, 0
                mov     eax, dFileID
		CALL_HDD FileClose
                mov     al, 210                 ; ошибка закрытия файла
                jc      PCCOff_Answer
                ; перечитаем полученные данные в память
                ; ========     =========
                cmp     bDataType, 0
                jne     PCCOff_Cfg
                ; --- программа
                @The_End_Conf _FILE_PRG_NAME, _FILE_PRG_NAME_C, _FILE_PRG_NAME_T, @1, @2, @@1
                jmp     PCCOff_Clear
                ; ----          ----
PCCOff_Cfg:     cmp     bDataType, 1
                jne     PCCOff_Rbf
                ; --- конфигурация
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
                ; --- прочитаем новый файл
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
                ; --- доп. инфа(.rbf)
                @The_End_Conf _FILE_RBF_NAME, _FILE_RBF_NAME_C, _FILE_RBF_NAME_T, @5, @6, @@5
                jmp     PCCOff_Clear
                ; ----          ----
PCCOff_BLst:    cmp     bDataType, 5
                jne     PCCOff_Answer
                ; --- "чёрный" список
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
                ; --- прочитаем новый файл
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
                ; сменим текущую копию и перечитаем
                push    ds
                mov     ds, ss:SEG_AX
                ASSUME  ds:DATA_AXS
                call    FLChangeActCopy         ; приняли
                call    FLReInicialize          ; перечитали если надо
                call    FLIndicate
                mov     FLCurTaskID, _NOP_ID
                pop     ds
                ASSUME  ds:SCREEN_SEG_PUMA
                jmp     PCCOff_Clear
ENDIF
                ; --- очистим параметры режима конфигурации
PCCOff_Clear:
                xor     eax, eax
                mov     DWORD PTR bBlockType, eax
                mov     DWORD PTR wStartSector, eax
                xor     al, al
IF _MSP NE TRUE
                jmp     PCCOff_Answer
                ; ошибка закрытия файла
PCCOff_Err:     pop     ds
                mov     al, 217                 ; ошибка обновления данных
ENDIF
                ; в любом случае выход из режима конфигурации
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

                ; --- передача сектора данных блоку
PC_Sector_In    PROC
                push    ds
                pushad
                push    ebx                     ; для PUMA_Answer
                push    edi
                ;
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  ds:SCREEN_SEG_PUMA
                mov     al, 211                 ; блок не в режиме конфигурации
                cmp     bCfg, 0
IF _MSP EQ TRUE
                je      PCSI_Answer
ELSE
                je      PCSI_Err
ENDIF
                ; текущая операция чтение ?
                mov     al, 212                 ; блок не в режиме записи
                cmp     bOperType, 1
IF _MSP EQ TRUE
                jne     PCSI_Answer
ELSE
                jne     PCSI_Err
ENDIF
                ; проверка текущего № сектора
                mov     al, 213                 ; неверный номер очередного сектора
                mov     bx, es:[edi + 6]
                cmp     bx, wNextSector
IF _MSP EQ TRUE
                jne     PCSI_Answer
ELSE
                jne     PCSI_Err
ENDIF
                mov     al, 214                 ; запрос лишнего сектора
                cmp     bx, wSectorCount
IF _MSP EQ TRUE
                ja     PCSI_Answer
ELSE
                ja     PCSI_Err
ENDIF

IF _MSP EQ TRUE
                mov     esi, edi                ; сохраним указатель на принятый пакет
                ; по типу данных получаем базовый указатель
                movzx   eax, bDataType
                cmp     al, _CONFG_ID
                jbe     PCSI_Set_Task
                mov     al, _DOPIN_ID
PCSI_Set_Task:  push    es
                mov     es, ss:SEG_AX
                mov     es:FLCurTaskID, al
                mov     dl, es:FLCurActCopyB[eax];текущая конфигурация
                and     dl, 1                   ; либо 0, либо 1
                xor     dl, 1                   ; меняем
                mov     edi, es:FLFirstCopyAddrD[eax * 4]
                cmp     dl, 1
                jne     PCSI_Get_Ptr
                mov     edi, es:FLSecndCopyAddrD[eax * 4]
PCSI_Get_Ptr:   pop     es
                ; теперь вычисляем текущий указатель
                and     ebx,0ffffh
                shl     ebx,7                   ; *128
                add     edi, ebx                ; в edi-адрес текущего блока на флеш
                ; собственно запись
                add     esi, 8                  ; указатель с начала пакета на данные в пакете
                mov     ecx, 40h                ; слов
                call    ELAN400_WRITE_FLC
                inc     wNextSector
                jmp     PCSI_OK
ELSE
                ; нет открытого файла
                mov     al, 215                 ; нет открытого файла
                cmp     bFileOpen, 0
                je      PCSI_Answer
                ; указатель на принятый пакет в ds:[edx] из es:[edi]
                push    ds
                push    es
                pop     ds
                mov     edx, edi
		add	edx, 8
                ; пишем !
                mov     ecx, 80h
                mov     eax, dFileID
                CALL_HDD FileWrite
                pop     ds
                mov     al, 216                 ; ошибка записи сектора
                jc      PCSI_Err
                ; ОК - записано
                inc     wNextSector
                jmp     PCSI_OK
                ; выход по ошибке с закрытием файла
PCSI_Err:       push    eax
                mov     eax, dFileID
		CALL_HDD FileClose
                pop     eax
                mov     bFileOpen, 0
                jmp     PCSI_Answer
ENDIF
                ; подтверждаем
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

                ; --- запрос сектора данных у блока
PC_Sector_Out   PROC
                push    ds
                pushad
                push    ebx                     ; для PUMA_Answer
                ;
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  ds:SCREEN_SEG_PUMA
                mov     al, 211                 ; блок не в режиме конфигурации
                cmp     bCfg, 0
                je      PCSO_Answer
                ; текущая операция чтение ?
                mov     al, 212                 ; блок не в режиме чтения
                cmp     bOperType, 0
                jne     PCSO_Answer
                ; проверка текущего № сектора
                mov     al, 213                 ; неверный номер очередного сектора
                mov     bx, es:[edi + 6]
                cmp     bx, wNextSector
                jne     PCSO_Answer
                mov     al, 214                 ; запрос лишнего сектора
                cmp     bx, wSectorCount
                ja      PCSO_Answer
IF _MSP NE TRUE
                ; нет открытого файла
                mov     al, 215                 ; нет открытого файла
                cmp     bFileOpen, 0
                je      PCSO_Answer
                ; начинаем формировать ответный пакет по es:[edi]
ELSE
                mov     ebp, edi
ENDIF
                mov     edx, es:[edi]
                @EDX_Back_Addr
                mov     es:[edi], edx              ; обратный адрес
                or      BYTE PTR es:[edi + 5], 80h ; код ответа из кода запроса
IF _MSP EQ TRUE
                mov     WORD PTR es:[edi + 6], bx  ; номер передаваемого сектора
                ; по типу данных получаем базовый указатель
                movzx   eax, bDataType
                cmp     al, _CONFG_ID
                jbe     PCSO_Set_Task
                mov     al, _DOPIN_ID
PCSO_Set_Task:  push    es
                mov     es, ss:SEG_AX
                mov     es:FLCurTaskID, al
                mov     dl, es:FLCurActCopyB[eax];текущая конфигурация
                and     dl, 1                   ; либо 0, либо 1
                mov     esi, es:FLFirstCopyAddrD[eax * 4]
                cmp     dl, 1
                jne     PCSO_Get_Ptr
                mov     esi, es:FLSecndCopyAddrD[eax * 4]
PCSO_Get_Ptr:   pop     es
                ; теперь вычисляем текущий указатель
                and     ebx, 0ffffh
                shl     ebx, 7                  ; *128
                add     esi, ebx                ; в edi-адрес текущего блока на флеш
                ; собственно чтение
                add     edi, 8                  ; указатель с начала пакета на данные в пакете
                mov     ecx, 40h                ; слов
                call    ELAN400_READ_FLC
ELSE
                movzx   esi, wNextSector
                mov     WORD PTR es:[edi + 6], si  ; номер передаваемого сектора

PCSO_Rd_Cnf:    ; ~~~ читаем
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
                mov     al, 220         ; ошибка чтения сектора данных
                push    eax
                mov     eax, dFileID
                CALL_HDD FileClose
                pop     eax
                mov     bFileOpen, 0
                jmp     PCSO_Answer


ENDIF
@@Go_next:
                inc     wNextSector
                ; отправляем
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
                mov     BYTE PTR es:[edi + 5], 8fh ; выход из режима конфигурации
                mov     es:[edi + 6], al  ; номер передаваемого сектора
                mov     ecx, 7
		push	es
		pop	ds
ENDIF
		mov	esi, edi
                call    PUMA_Out
                ;;;jnc     PCSO_Ans                    ; не отправили
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

                ; === тарификационные ===============================
                ; --- подтверждение тарификации
PC_Tar_Ack      PROC
		push	es
		pushad
                mov     ax, es:[edi + 6]        ; №№ ЦК и пакета
                mov     PEREM_AX, ax
                @POLUCH_ADR_MO                  ;
                or      dl, dl                  ; проверим есть место ?
                jnz     PCTA_Exit               ; нет - уйдем
                ; есть место - все же запишем
                mov     eax, 030404ebh          ; 235-ая длиной 3
                stosd
                mov     al, 13                  ; команда 13 - подтверждение тар. пакетов
                stosb
                mov     ax, PEREM_AX            ; параметры
                stosw
                @ZAPIS_OK_MO
PCTA_Exit:      popad
		pop	es
		ret
PC_Tar_Ack      ENDP

                ; --- время
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

                ; --- подписка на тарификацию
PC_Subscr_Tar   PROC
                xor     al, al
                call    PUMA_Answer
                ret
PC_Subscr_Tar   ENDP

                ; --- подписка на аварии
PC_Subscr_Alarm PROC
                xor     al, al
                call    PUMA_Answer
                ret
PC_Subscr_Alarm ENDP





