SORM_Input_init PROC
                mov     ah, _color_soft_SORM
                cmp     SORM_Emul_Type, _SET_SOFT
                je      Input_init_win_color
                mov     ah, _color_hard_SORM
                jmp     Input_init_win_color
SORM_Input_init ENDP

;
SORM_Emul_init  PROC
                cmp     Pass_OK, 0
                jne     @@emul
                ; --- ввод пароля
@@pass:         lea     si, Input_Pass
                call    Input_init_win
                mov     SOST_VIU5, _SORM_Pass_kla
                ret
                ; --- выбор режима эмулятора
@@emul:         cmp     SORM_State, _SORM_EMUL
                jne     @@input
SORM_Emul_dlg:  mov     al, ah
                ;
                mov     ah, _color_soft_SORM
                mov     si, OFFSET SORM_soft_Emul_win
                mov     cx, 9
                mov     SORM_Emul_Type, _SET_SOFT
                ;
                cmp     al, 20h                 ; зашли сюда через F11 ?
                je      @@draw                  ; - ДА
                ;
                mov     ah, _color_hard_SORM    ; - НЕТ, F10 !
                mov     si, OFFSET SORM_hard_Emul_win
                mov     SORM_Emul_Type, _SET_HARD
                mov     cx, 11
                ;
@@draw:         mov     di, _pos_SORM_win
                call    printWin_ds
                mov     SOST_VIU5, _SORM_Emul_kla
                ret
                ; --- ввод параметра
@@input:        cmp     SORM_State, _SORM_INPUT
                jne     @@exit
                movzx   si, SORM_In_State
                shl     si, 1
                mov     si, SORM_Input_Data[si]
                call    SORM_Input_init
                mov     SOST_VIU5, _SORM_Input_kla
@@exit:         ret
SORM_Emul_init  ENDP

SORM_Emul_rslt  PROC
                lea     si, SORM_Exec_Log
                ;
                mov     al, SORM_Exec_Data
                cmp     al, 9
                jbe     @@symb_1
                add     al, 'A' - '9' - 1
@@symb_1:       add     al, '0'
                mov     [si + 11], al
                ;
                mov     al, SORM_Exec_Data[1]
                cmp     al, 9
                jbe     @@symb_2
                add     al, 'A' - '9' - 1
@@symb_2:       add     al, '0'
                mov     [si + 12], al
                ;
                mov     al, SORM_Exec_Data[2]
                cmp     al, 9
                jbe     @@symb_3
                add     al, 'A' - '9' - 1
@@symb_3:       add     al, '0'
                mov     [si + 33], al
                ;
                mov     al, SORM_Exec_Data[3]
                cmp     al, 9
                jbe     @@symb_4
                add     al, 'A' - '9' - 1
@@symb_4:       add     al, '0'
                mov     [si + 34], al
                ;
                cmp     WORD PTR SORM_Exec_Data[2], 0
                je      @@ok
                call    Print_log_err
                ret
@@ok:           call    Print_log
                ret
SORM_Emul_rslt  ENDP

SORM_Pass_kla   PROC
                call    Input_kla
                jnc     @@exit
                cmp     DWORD PTR Input_Buff, _PASS_LO
                jne     @@exit
                cmp     DWORD PTR Input_Buff[4], _PASS_HI
                jne     @@exit
                call    INIC_W_VN
                mov     SORM_State, _SORM_EMUL
                mov     Pass_Ok, 255
;;;                jmp     SORM_Emul_dlg
@@exit:         ret
SORM_Pass_kla   ENDP

SORM_Emul_kla   PROC
                cmp     ah, 1
                je      SORM_Start
                cmp     ah, 2
                je      SORM_Stop
                cmp     ah, 3
                je      SORM_KSL
                cmp     ah, 4
                je      SORM_Object
                cmp     ah, 5
                jne     @@key_6
                cmp     SORM_Emul_Type, _SET_SOFT
                je      SORM_Off
                jmp     SORM_Comm_TC
@@key_6:        cmp     ah, 6
                jne     @@key_7
                cmp     SORM_Emul_Type, _SET_HARD
                je      SORM_UnComm_TC
                jmp     @@exit
@@key_7:        cmp     ah, 7
                jne     @@exit
                cmp     SORM_Emul_Type, _SET_HARD
                je      SORM_Off
@@exit:         ret
                ; ---
SORM_Start:     xor     di, di
                xor     bx, bx
                mov     gs, ss:SEG_OBJ
                lea     si, m_Pok_start         ; запустим СОРМ
                call    LEV_L4
                call    SORM_Emul_rslt
                ret
                ; ---
SORM_Stop:      xor     di, di
                xor     bx, bx
                mov     gs, ss:SEG_OBJ
                lea     si, m_Pok_stop          ; остановим СОРМ
                call    LEV_L4
                call    SORM_Emul_rslt
                ret
                ; ---
SORM_KSL:       call    INIC_W_down
                lea     si, Input_KSL_Group
                call    SORM_Input_init
                mov     SOST_VIU5, _SORM_Input_kla
                mov     SORM_State, _SORM_INPUT
                mov     SORM_In_State, _SI_KSL_GROUP
                ret
                ; ---
SORM_Object:    call    INIC_W_down
                lea     si, Input_Obj_Num
                call    SORM_Input_init
                mov     SOST_VIU5, _SORM_Input_kla
                mov     SORM_State, _SORM_INPUT
                mov     SORM_In_State, _SI_OBJ_NUM
                ret
                ; ---
SORM_Off:       mov     SORM_State, _SORM_OFF
                mov     SORM_Emul_Type, 0
                mov     Pass_Count, 0
                mov     Pass_OK, 0
                call    INIC_W_VN
                ret
                ; ---
SORM_Comm_TC:
                ret
                ; ---
SORM_UnComm_TC:
                ret
SORM_Emul_kla   ENDP

SORM_Input_kla  PROC
                call    Input_kla
                jnc     @@exit
                movzx   si, SORM_In_State
                shl     si, 1
                call    SORM_Input_Act[si]
@@exit:         ret
SORM_Input_kla  ENDP

SORM_Emul_No_Act        PROC
                ret
SORM_Emul_No_Act        ENDP

SORM_Emul_Next_Act      PROC
                inc     SORM_In_State
                call    INIC_W_down
                movzx   si, SORM_In_State
                shl     si, 1
                mov     si, SORM_Input_Data[si]
                call    SORM_Input_init
                ret
SORM_Emul_Next_Act      ENDP

SORM_Emul_KSL_Done      PROC
                ; № группы
                mov     ax, WORD PTR KSL_Gr_Data
                shl     ah, 4
                rol     ax, 4
                mov     m_Pok_KSL[10], al
                ; тип группы
                mov     ax, WORD PTR KSL_Type_Data
                shl     ah, 4
                rol     ax, 4
                mov     m_Pok_KSL[11], al
                ; № КСЛ А
                mov     ax, WORD PTR KSL_A_Data
                shl     ah, 4
                rol     ax, 4
                mov     m_Pok_KSL[12], al
                ; № КСЛ Б
                mov     ax, WORD PTR KSL_B_Data
                shl     ah, 4
                rol     ax, 4
                mov     m_Pok_KSL[13], al
                ; отправка сформированного пакета
                xor     di, di
                xor     bx, bx
                mov     gs, ss:SEG_OBJ
                lea     si, m_Pok_KSL          ; закрепим КСЛ
                call    LEV_L4
                ; интерфейс пользователя
                mov     SORM_State, _SORM_EMUL
                call    INIC_W_down
                call    SORM_Emul_rslt
                mov     ah, 20h
                cmp     SORM_Emul_Type, _SET_SOFT
                je      SORM_Emul_dlg
                mov     ah, 1fh
                jmp     SORM_Emul_dlg
SORM_Emul_KSL_Done      ENDP

SORM_Emul_Obj_Done      PROC
                ; условный № объекта
                mov     ax, WORD PTR Obj_Num_Data[2]
                shl     ah, 4
                rol     ax, 4
                mov     m_Pok_Obj[10], al
                mov     ax, WORD PTR Obj_Num_Data
                shl     ah, 4
                rol     ax, 4
                mov     m_Pok_Obj[11], al
                ; тип объекта
                mov     ax, WORD PTR Obj_Type_Data
                shl     ah, 4
                rol     ax, 4
                mov     m_Pok_Obj[12], al
                ; признак номера телефона
                mov     ax, WORD PTR Obj_Attr_Data
                shl     ah, 4
                rol     ax, 4
                mov     m_Pok_Obj[13], al
                ; количество знаков в номере телефона
                mov     ax, WORD PTR Obj_Len_Data
                shl     ah, 4
                rol     ax, 4
                mov     m_Pok_Obj[14], al
                ; перед выводом добьём номер телефона fff
                cld
                push    ds
                pop     es
                mov     al, 0fh
                movzx   cx, Obj_Tel_Count
                lea     di, Obj_Tel_Data
                add     di, cx
                mov     cl, 18
                sub     cl, Obj_Tel_Count
                rep     stosb
                ; номер телефона
                mov     cx, 9
                lea     bx, Obj_Tel_Data
                mov     di, 15
@@obj_tel:      mov     ax, [bx]
                shl     al, 4
                shr     ax, 4
                mov     m_Pok_Obj[di], al
                add     bx, 2
                inc     di
                loop    @@obj_tel
                ; категория контроля
                mov     ax, WORD PTR Obj_Cat_Data
                shl     ah, 4
                rol     ax, 4
                mov     m_Pok_Obj[26], al
                ; номер группы КСЛ
                mov     ax, WORD PTR Obj_Gr_Data
                shl     ah, 4
                rol     ax, 4
                mov     m_Pok_Obj[27], al
                ; метка приоритета
                mov     ax, WORD PTR Obj_Pri_Data
                shl     ah, 4
                rol     ax, 4
                mov     m_Pok_Obj[28], al
                ; отправка сформированного пакета
                xor     di, di
                xor     bx, bx
                mov     gs, ss:SEG_OBJ
                lea     si, m_Pok_Obj          ; постановка объекта
                call    LEV_L4
                ; интерфейс пользователя
                mov     SORM_State, _SORM_EMUL
                call    INIC_W_down
                call    SORM_Emul_rslt
                mov     ah, 20h
                cmp     SORM_Emul_Type, _SET_SOFT
                je      SORM_Emul_dlg
                mov     ah, 1fh
                jmp     SORM_Emul_dlg
SORM_Emul_Obj_Done      ENDP

SORM_Emul_Comm_Done     PROC
                mov     SORM_State, _SORM_EMUL
                call    INIC_W_down
                ;
                mov     ah, 20h
                cmp     SORM_Emul_Type, _SET_SOFT
                je      SORM_Emul_dlg
                mov     ah, 1fh
                jmp     SORM_Emul_dlg
                ret
SORM_Emul_Comm_Done     ENDP

SORM_Emul_UnComm_Done   PROC
                mov     SORM_State, _SORM_EMUL
                call    INIC_W_down
                ;
                mov     ah, 20h
                cmp     SORM_Emul_Type, _SET_SOFT
                je      SORM_Emul_dlg
                mov     ah, 1fh
                jmp     SORM_Emul_dlg
                ret
SORM_Emul_UnComm_Done   ENDP


; === =========================================================================
IF _MSP EQ TRUE
SORM_Emul_cmnd_debug    PROC
ret;;;
                ; --- вывод на экран сообщений первого канала
                test    FG_PROGRAM, M_OR_7      ; если включена отладка 1-го (командного)
                jz      @@exit                  ; канала СОРМ
;cmp __inf_y, __INF_STR_COUNT * 160 - 160
;jae @@exit
                pushad
                movzx   ebp, si
                ; маркер вывода СОРМа
                mov     ah, cBLUE * 16 + cWHITE
                lea     si, __sorm_1_in_str
                mov     di, __inf_y
                call    printS
                ; первые 38 байт сообщения
                movzx   cx, ds:[ebp + 3]
                add     cx, 10
                mov     PEREM_CX, cx
                cmp     cx, 38
                jbe     @@beg
                mov     cx, 38
@@beg:          mov     di, __inf_y
                add     di, 2
                ;
@@head:         push    cx
                mov     bl, ds:[ebp]
                mov     cx, 2
                call    printH
                pop     cx
                inc     ebp
                add     di, 4
                loop    @@head
                add     __inf_y, 160
                cmp     __inf_y, __INF_STR_COUNT * 160
                jb      @@pre_tail
                mov     __inf_y, 0
                ; перешли на новую строку
@@pre_tail:     mov     cx, PEREM_CX
                sub     cx, 38
                js      @@end
                je      @@end
                lea     si, __inf_clear
                mov     di, __inf_y
                call    printS
                ; последние 10 байт сообщения
                lea     si, __sorm_empty_str
                mov     di, __inf_y
                call    printS
                mov     di, __inf_y
                add     di, 2
                ;
@@tail:         push    cx
                mov     bl, ds:[ebp]
                mov     cx, 2
                call    printH
                pop     cx
                inc     ebp
                add     di, 4
                loop    @@tail
                add     __inf_y, 160
                cmp     __inf_y, __INF_STR_COUNT * 160
                jb      @@empty
                mov     __inf_y, 0
                ; чистим следующую строку
@@empty:        lea     si, __inf_clear
                mov     di, __inf_y
                call    printS
@@end:          popad
@@exit:         ret
SORM_Emul_cmnd_debug    ENDP

SORM_Emul_answ_debug    PROC
                ; --- вывод на экран сообщений первого канала
                test    FG_PROGRAM, M_OR_7      ; если включена отладка 1-го (командного)
                jz      @@exit                  ; канала СОРМ
;cmp __inf_y, __INF_STR_COUNT * 160 - 160
;jae @@exit
                pushad
                mov     ebp, edi
                ; маркер вывода СОРМа
                mov     ah, cBLUE * 16 + clWHITE
                lea     si, __sorm_1_out_str
                mov     di, __inf_y
                call    printS
                ; первые 38 байт сообщения
                movzx   cx, es:[ebp + 1]
                add     cx, 8
                mov     PEREM_CX, cx
                cmp     cx, 38
                jbe     @@beg
                mov     cx, 38
@@beg:          mov     di, __inf_y
                add     di, 2
                ;
@@head:         push    cx
                mov     bl, es:[ebp]
                mov     cx, 2
                call    printH
                pop     cx
                inc     ebp
                add     di, 4
                loop    @@head
                add     __inf_y, 160
                cmp     __inf_y, __INF_STR_COUNT * 160
                jb      @@pre_tail
                mov     __inf_y, 0
                ; перешли на новую строку
@@pre_tail:     lea     si, __inf_clear
                mov     di, __inf_y
                call    printS
                ; последние 10 байт сообщения
                mov     cx, PEREM_CX
                sub     cx, 38
                jz      @@end
                js      @@end
                lea     si, __sorm_empty_str
                mov     di, __inf_y
                call    printS
                mov     di, __inf_y
                add     di, 2
                ;
@@tail:         push    cx
                mov     bl, es:[ebp]
                mov     cx, 2
                call    printH
                pop     cx
                inc     ebp
                add     di, 4
                loop    @@tail
                add     __inf_y, 160
                cmp     __inf_y, __INF_STR_COUNT * 160
                jb      @@empty
                mov     __inf_y, 0
                ; чистим следующую строку
@@empty:        lea     si, __inf_clear
                mov     di, __inf_y
                call    printS
@@end:          popad
@@exit:         ret
SORM_Emul_answ_debug    ENDP
ELSE
; --- вывод обмена СОРМ по обоим каналам на экран -----------------------------
; вход:         cx      - длина принятого пакета
;               ds:[si] - указатель на принятый пакет
;
SORM_Scr_Debug  PROC
                pushad
                movzx   ebp, si
                add     ebp, 5
                sub     cx, 5
                ; маркер вывода СОРМа
                cmp     dl, 0
                jne     @@out_1
@@in_1:         ; приём с первого канала
                cmp     Pass_OK, 0
                je      @@cmnd
                jmp     @@answ
@@out_1:        cmp     dl, 1
                jne     @@mess                  ; приём со второго канала
                ; передача в первый канал
                cmp     Pass_OK, 0
                jne     @@cmnd
                jmp     @@answ
                ; ----
@@cmnd:         mov     ah, cBLUE * 16 + cWHITE
                lea     si, __sorm_1_in_str
                jmp     @@pre_head
@@answ:         mov     ah, cBLUE * 16 + cWHITE
                lea     si, __sorm_1_out_str
                jmp     @@pre_head
@@mess:         mov     ah, cBLUE * 16 + clWHITE
                lea     si, __sorm_2_str
                ;
@@pre_head:     mov     di, __inf_y
                call    printS
                ; первые 38 байт сообщения
                mov     PEREM_CX, cx
                cmp     cx, 38
                jbe     @@beg
                mov     cx, 38
@@beg:          mov     di, __inf_y
                add     di, 2
                ;
@@head:         push    cx
                mov     bl, ds:[ebp]
                mov     cx, 2
                call    printH
                pop     cx
                inc     ebp
                add     di, 4
                loop    @@head
                add     __inf_y, 160
                cmp     __inf_y, __INF_STR_COUNT * 160
                jb      @@pre_tail
                mov     __inf_y, 0
                ; перешли на новую строку
@@pre_tail:     lea     si, __inf_clear
                mov     di, __inf_y
                call    printS
                ; последние 10 байт сообщения
                mov     cx, PEREM_CX
                sub     cx, 38
                jz      @@end
                js      @@end
                lea     si, __sorm_empty_str
                mov     di, __inf_y
                call    printS
                mov     di, __inf_y
                add     di, 2
                ;
@@tail:         push    cx
                mov     bl, ds:[ebp]
                mov     cx, 2
                call    printH
                pop     cx
                inc     ebp
                add     di, 4
                loop    @@tail
                add     __inf_y, 160
                cmp     __inf_y, __INF_STR_COUNT * 160
                jb      @@empty
                mov     __inf_y, 0
                ; чистим следующую строку
@@empty:        lea     si, __inf_clear
                mov     di, __inf_y
                call    printS
@@end:          popad
                ret
SORM_Scr_Debug  ENDP

SORM_Scr_Debug_Far      PROC
                call    SORM_Scr_Debug
                retf
SORM_Scr_Debug_Far      ENDP
ENDIF
