WIN_SORM_GO     PROC    FAR
                CALL    WIN_VIU_GO
                CALL    KLA_WIN
                PUSH    GS
                MOV     GS,SS:SEG_OBJ
                CALL    MTP_GO
                POP     GS
                call    Obj_Data_Output                        ; выдача объектов наблюдения \
                call    Bunch_Data_Output                      ; выдача пучков ............./ СОРМ!
                RETF
                ENDP

END_WIN_VIU:
                RET

WIN_VIU_GO:
                        ;ПРОСЧЕТ ВРЕМЕНИ ВРЕМЯ
                MOV     AX,TIME_2MC
                MOV     DX,AX
                SUB     AX,OLD_TIME
                MOV     OLD_TIME,DX
                ADD     NEW_TIME,AX
                MOV     AX,NEW_TIME
                CMP     AX,POROG_SEC
                JB      END_TIME
                INC     TEK_SECUND
                SUB     NEW_TIME,POROG_SEC
                MOV     AL,SECUNDA
                INC     AL
                DAA
                MOV     SECUNDA,AL
                CMP     AL,60H
                JB      END_TIME
                MOV     SECUNDA,0
                MOV     AL,MIN
                INC     AL
                DAA
                MOV     MIN,AL
                CMP     AL,60H
                JB      END_TIME
                MOV     MIN,0
                MOV     AL,CHAS
                INC     AL
                DAA
                MOV     CHAS,AL
                CMP     AL,1
                JNE     NE_1_CHAS
                OR      FG_TIME,M_OR_1
NE_1_CHAS:
                CMP     AL,2
                JNE     NE_2_CHAS
                OR      FG_TIME,M_OR_2
NE_2_CHAS:
                CMP     AL,3
                JNE     NE_3_CHAS
                OR      FG_TIME,M_OR_3
NE_3_CHAS:
                CMP     AL,24H
                JB      END_TIME

                MOV     CHAS,0
                OR      FG_TIME,M_OR_0

                MOV     AL,DEN_MES
                INC     AL
                DAA
                MOV     DEN_MES,AL
                MOVZX   SI,MES
                AND     SI,1FH
                MOV     DL,DLIT_MES[SI]
                CMP     DL,0FFH
                JNE     SHORT NE_FEVRAL
                MOV     DL,28H
                PUSH    DX
                PUSH    AX
                MOVZX   AX,GOD_
                XOR     DX,DX
                MOV     CX,4
                DIV     CX
                MOV     CX,DX
                POP     AX
                POP     DX
                OR      CL,CL
                JNZ     SHORT NE_FEVRAL
                INC     DL
NE_FEVRAL:
                CMP     AL,DL
                JBE     END_TIME
                MOV     DEN_MES,1
                MOV     AL,MES
                INC     AL
                DAA
                MOV     MES,AL
                CMP     AL,12H
                JBE     END_TIME
                MOV     MES,1
                MOV     AL,GOD_
                INC     AL
                DAA
                MOV     GOD_,AL
END_TIME:
                        ;ПОДСЧЕТ НАГРУЗКИ
                INC     SCH_NAG
                CMP     SCH_NAG,256;;;0
                JB      SHORT DL_1_1
                MOV     SCH_NAG,0
                MOV     AX,TIME_2MC
                SUB     AX,OLD_T_NAG
                SHR     AX,8
                MOV     NAGRUS,AX
                MOV     AX,TIME_2MC
                MOV     OLD_T_NAG,AX
DL_1_1:
                ; автоматическая отчистка строки ЛОГОВ (нижняя строка экрана)
                cmp     LOG_CHANGE, 0
                je      @@no_clear_log
                mov     ax, TIME_2MC
                sub     ax, LOG_TIMEOUT
                cmp     ax, _log_timeout
                jb      @@no_clear_log

                mov     si, offset LOG_CLEAR
                mov     di, _pos_log
                mov     ah, _color_cl_log
                call    printS

                mov     LOG_CHANGE, 0
@@no_clear_log:

NET_VIV_GUK:
                JMP     VIV_SUBMENU

GLOB_WIN:
                MOV     AX,TIME_2MC     ;\
                SUB     AX,OLD_TWV      ; > что бы выводить раз в POROG_WV
                CMP     AX,POROG_WV;;50     ;|  времени (20 * 2 мс)
                JB      END_WIN_VIU     ;/
                MOV     AX,TIME_2MC
                MOV     OLD_TWV,AX

                JMP     VIV_NIGNEGO_OKNA

ADR_VERX_OKNA   DW      OFFSET  GLOB_WIN                ;0
                DW      OFFSET  Refresh_Cfg_Win         ;1
                DW      OFFSET  Refresh_Cfg_CK_Win      ;2
                DW      OFFSET  Refresh_Cfg_Ln_Win      ;3
                DW      OFFSET  GLOB_WIN                ;4
                DW      OFFSET  Refresh_Tel_Num_Win     ;5
                DW      OFFSET  Refresh_U_Num_Win       ;6
                DW      OFFSET  Refresh_MO_Slct_Win     ;7
                DW      OFFSET  Refresh_BL_DBG_Win      ;8
                DW      OFFSET  Refresh_Puma_cnts_Win   ;9
                DW      OFFSET  GLOB_WIN                ;10
                DW      OFFSET  GLOB_WIN                ;11
                DW      OFFSET  GLOB_WIN                ;12
                DW      OFFSET  GLOB_WIN                ;13
                DW      OFFSET  GLOB_WIN                ;14

VIV_SUBMENU:
                MOV     DI,NOM_VERX_OKNA
                SHL     DI,1
                JMP     ADR_VERX_OKNA[DI]

; === процедуры обновления верхних окон =====================================
;
; --- просмотр процесса сбора конфигурации
;
Refresh_Cfg_Win	PROC
		push	bx
                ; --- вывод состояния процесса
                mov     ah, _color_Win_Cfg
                mov     si, offset str_CfgRun
                cmp     Cfg_Run, 0
                jne     @@draw_Status
                mov     si, offset str_CfgDone
@@draw_Status:  mov     di, _pos_CfgStatus
                call    printS
		; --- очищаем предыдущий ЦК
		movzx	ax, pos_Prev_CK
		and	ax, 1fh
		inc     ax
		mov     cx, 160
		mul	cx
		mov	di, ax
		cmp     pos_Prev_CK, 32
		jb	@@draw_Prev_CK
		add	di, 80
@@draw_Prev_CK: mov	al, ch_Prev_CK
		mov	ah, _color_Win_Cfg
		mov	cx, 1
		call	printC
		; --- отмечаем текущий ЦК
		movzx	ax, pos_Curr_CK
		and	ax, 1fh
		inc     ax
		mov     cx, 160
		mul	cx
		mov	di, ax
		cmp     pos_Curr_CK, 32
		jb	@@draw_Curr_CK
		add	di, 80
@@draw_Curr_CK: mov	al, ch_Curr_CK
		mov	ah, _color_Win_Cfg_Curr
		mov	cx, 1
		call	printC
                ; --- подсветка текущих состояний имеющихся в наличии ЦК
		xor	bx, bx
		xor	si, si
@@CK_loop:      ; --- проверяем есть ли ЦК в тарификационоой сети
		mov    	es, ss:SEG_AXD
		cmp	BYTE PTR es:TIS_TIS[si], KL_TIS
		jae	@@next_CK
		; --- вычисляем Y
		mov     ax, 160
		movzx	cx, bh
		and	cx, 1fh
		mul	cx
		mov	di, ax
		add	di, 160 + 2
		; --- вычисляем X
;IF _MSP EQ TRUE MVT закомментировал
;		mov	ax, 40
;ELSE
                mov     ax, 80
;ENDIF
		movzx	cx, bh
		shr	cx, 5
		mul	cx
		add	di, ax
		; --- определяем каким цветом подсвечивать номер ЦК
		mov	ax, si
		mov     PEREM_AH, 1
		mov	ah, _color_curr
		cmp	al, Cfg_Show_CK
		je	@@draw_CK_state
		mov     PEREM_AH, 0
		mov	ah, _color_avar
		cmp	Cfg_CK_Errs[si], 0
		jne	@@draw_CK_state
		mov	ah, _color_work
		test	Cfg_CK_Flags[si], cf_DONE
		jnz	@@draw_CK_state
                mov     ah, _color_Win_Cfg
		; --- подсвечиваем номер ЦК
@@draw_CK_state:
IF _MSP EQ TRUE
                mov     cx, 2
		call	printA
ELSE
                push    bx
		movzx	ebx, bl
		mov	cx, 2
		call	printD
		pop	bx
ENDIF
                ; --- флаги и ошибки текущего ЦК
		mov	dl, Cfg_CK_Flags[si]
		mov	dh, Cfg_CK_Errs[si]
		push	si
                ; --- запрос и получение даты конфигурации
		lea     si, str_CfgClear_2
		mov	ah, _color_Win_Cfg
                test    dl, cf_SEND_CD
                jz      @@draw_CD
                lea     si, str_CfgDate
		mov	ah, _color_avar
		test	dh, ce_TN_FAULT
                jnz     @@draw_CD
		mov	ah, _color_work
                test    dl, cf_RECV_CD
                jnz     @@draw_CD
		mov	ah, _color_curr
@@draw_CD:      add     di, 10
		call	printS
                ; --- проверяем отправку тар. сети и ее подтверждение
		lea     si, str_CfgClear_2
		mov	ah, _color_Win_Cfg
		test	dl, cf_SEND_TN
		jz	@@draw_TN
                lea	si, str_TarNet
		mov	ah, _color_avar
		test	dh, ce_TN_FAULT
		jnz  	@@draw_TN
		mov	ah, _color_work
		test	dl, cf_RECV_TN
		jnz	@@draw_TN
		mov	ah, _color_curr
@@draw_TN:      add     di, 6
		call	printS
		; --- проверяем запрос общей конфигурации и ее получение
		lea     si, str_CfgClear_2
		mov	ah, _color_Win_Cfg
		test	dl, cf_SEND_CK
		jz	@@draw_CK
	        lea	si, str_CfgCK
		mov	ah, _color_avar
		test	dh, ce_CK_FAULT
		jnz  	@@draw_CK
		mov	ah, _color_work
		test	dl, cf_RECV_CK
		jnz	@@draw_CK
		mov	ah, _color_curr
@@draw_CK:      add	di,  6
		call	printS
		; --- выводим список блоков ЦК
		mov	ah, _color_Win_Cfg_G
 		lea     si, str_CfgClear_16
                add     di, 6
                test    dl, cf_RECV_CK
                jnz     @@to_draw_Blk
                call    printS
                add     di, 36
                pop     si
                jmp     @@draw_Ln
@@to_draw_Blk:  pop     si
                movzx   ebp, si
                shl     ebp, 7
                mov     gs, ss:SEG_KONF
                xor     cx, cx
@@Blk_loop:     push    bx
                push    cx
                movzx   bx, gs:[ebp]
                mov     al, ch_NoBlk
		mov	ah, _color_Win_Cfg_G
                cmp     bl, 0ffh
                je      @@draw_Blk
                mov     al, ch_BadBlk
                cmp     bl, 9
                ja      @@draw_Blk
                mov     al, ch_BlkNames[bx]
                mov     ah, _color_curr
		cmp     PEREM_AH, 1
		jne	@@not_Curr_Blk
                cmp     cl, Cfg_Show_Blk
                je      @@draw_Blk
@@not_Curr_Blk: shld    bx, si, 17
                mov     ah, _color_work
                bt      Cfg_CK_Mod_OK[bx], cx
                jc      @@draw_Blk
                mov     ah, _color_avar
                bt      Cfg_CK_Mod_Err[bx], cx
                jc      @@draw_Blk
                mov     ah, _color_Win_Cfg_G
@@draw_Blk:     mov     cx, 1
                call    printC
                pop     cx
                pop     bx
                inc     cx
                add     di, 2
                add     ebp, 8
                cmp	cx, 16
		jb	@@Blk_loop
                add     di, 4
                ; --- счётчик принятых линий с текущего блока текущего ЦК
@@draw_Ln:
		movzx   dx, Cfg_Curr_CK
                cmp     dx, si
                jne     @@clr_Ln_Count
		test    Cfg_CK_Flags[si], cf_DONE
		jnz	@@clr_Ln_Count
		test    Cfg_CK_Flags[si], cf_RECV_CK
		jz	@@clr_Ln_Count
		cmp     Cfg_CK_Errs[si], 0
		je	@@draw_Ln_Count
@@clr_Ln_Count: mov     ah, _color_Win_Cfg_G
		push	si
                lea     si, str_CfgClear_5
                call    printS
		pop	si
                jmp     @@to_CK_next
                ; ---
@@draw_Ln_Count:mov     ah, _color_Win_Cfg
		push    bx
                movzx   ebx, Cfg_Curr_Line
                mov     cx, 5
                call    printD
		pop	bx
                ; ---
@@to_CK_next:   inc     bh
@@next_CK:	inc	bl
		inc	si
		cmp	bl, KL_TIS
		jb	@@CK_Loop
		pop	bx
		jmp	GLOB_WIN
Refresh_Cfg_Win	ENDP

; --- просмотр конфигурации ЦК
;
Refresh_Cfg_CK_Win	PROC
		push	bx
		movzx	ebx, Cfg_CK_Num
                ; --- вывод состояния конфигурации
                mov     ah, _color_work
                mov     si, offset str_Done
		test	Cfg_CK_Flags[bx], cf_DONE
                jnz     @@draw_Status
                mov     ah, _color_avar
                mov     si, offset str_Started
		cmp	Cfg_CK_Flags[bx], 0
                jne     @@draw_Status
                mov     ah, _color_Cfg_CK_Win
                mov     si, offset str_not_Started
@@draw_Status:  mov     di, _pos_Cfg_CK_State
                call    printS
		; --- очищаем предыдущий блок
		mov	di, _pos_Cfg_CK_Curr
		movzx	ax, pos_Prev_Blk
		mov     cx, 160
		mul	cx
		add	di, ax
                mov	al, ch_Prev_CK
		mov	ah, _color_Cfg_CK_Win
		mov	cx, 1
		call	printC
		; --- отмечаем текущий блок
		mov	di, _pos_Cfg_CK_Curr
		movzx	ax, pos_Curr_Blk
		mov     cx, 160
		mul	cx
		add	di, ax
                mov	al, ch_Curr_CK
		mov	ah, _color_Cfg_CK_Curr
		mov	cx, 1
		call	printC
                ; --- теперь рисуем общую конфигурацию блоков
		mov	ah, _color_Cfg_CK_Win
                shl     ebx, 7
                xor     dx, dx
		mov	di, _pos_Cfg_CK_Blk
                mov     gs, ss:SEG_KONF
@@Blk_loop:     push	ebx
		push    cx
		push	di
		mov     al, ch_NoBlk
		mov	cx, 1
		cmp	BYTE PTR gs:[ebx], 255
		je	@@Blk_not_draw
		mov     al, ch_BadBlk
		cmp	BYTE PTR gs:[ebx], 9
		ja	@@Blk_not_draw
		; ---
@@Bk_draw:      mov	esi, ebx
		movzx	bx, gs:[esi]
		call	printH
		mov	al, ch_BlkNames[bx]
		add	di, 4
		call	printC
		mov	bl, gs:[esi + 5]
		add	di, 4
		call	printH
		cmp	BYTE PTR gs:[esi], 4
		je	@@CK_not_draw
		mov	bx, gs:[esi + 6]
		mov	cx, 3
		add	di, 6
                call	printD
		movzx	bx, Cfg_CK_Num
		shl	bx, 5
		add	bx, dx
		add	bx, dx
		mov	bx, Cfg_CK_Lines[bx]
		add	di, 14
                call	printD
		mov	ebx, gs:[esi + 1]
		mov	cx, 8
		add	di, 16
                call	printH
		jmp	@@Blk_next
		; ---
@@Blk_not_draw: call	printC
		add	di, 4
                call	printC
		add	di, 4
                call	printC
@@CK_not_draw:	mov	al, ch_Clear
		mov	cx, 3
		add	di, 6
                call	printC
		add	di, 14
                call	printC
		mov	cx, 8
		add	di, 16
                call	printC
		; ---
@@Blk_next:	pop	di
		pop	cx
		pop	ebx
		add	ebx, 8
		add	di, 160
		inc	dx
		cmp	dx, 16
		jnz	@@Blk_Loop
		pop	bx
		jmp	GLOB_WIN
Refresh_Cfg_CK_Win	ENDP
; --- просмотр конфигурации линий блока ЦК
;
Refresh_Cfg_Ln_Win	PROC
		push	bx
		; --- очищаем предыдущую линию
		mov	di, _pos_Cfg_Ln_Curr
		mov	ax, pos_Prev_Ln
		and	ax, 1fh
		mov     cx, 160
		mul	cx
		add	di, ax
                mov	al, ch_Prev_CK
		mov	ah, _color_Win_Cfg
		mov	cx, 1
		call	printC
		; --- отмечаем текущую линию
		mov	di, _pos_Cfg_Ln_Curr
		mov	ax, pos_Curr_Ln
		and	ax, 1fh
		mov     cx, 160
		mul	cx
		add	di, ax
                mov	al, ch_Curr_CK
		mov	ah, _color_Win_Cfg
		mov	cx, 1
		call	printC
                ; --- теперь рисуем конфигурации линий
                ; --- {Paul} -- <18/06/12> --
                mov     cl, 4
                test    FG_PROGRAM, _fg_prg_multy_pref
                jz      @@Go_old_conf
                movzx   bx, Cfg_Blk_Type
                mov     cl, TAB_TIP_LN[bx]
                ; --- {Paul} -- <END>
 @@Go_old_conf: movzx   ebx, pos_Curr_Ln
		and	bl, 0e0h
		movzx   esi, bx
                shl     esi, cl ;;;;;!!!было - 4
		add	esi, pos_Base_Ptr
		xor	cx, cx
		mov	di, _pos_Cfg_Ln_Curr
		mov	es, ss:SEG_KONF
@@line_loop:	push	ebx
		push	cx
		push	di
		; нумеруем линии
		mov	cx, 3
		add	di, 2
		call	printD
		; код типа линии
		movzx	bx, es:[esi]
		mov	cx, 2
		add	di, 8
		call	printH
		cmp	bl, 1fh
;;;		ja      @@no_ln_type
		ja      @@line_next
		; аббревиатура типа линии
		shl	bx, 2
		push	si
		lea	si, lnTypeNames[bx]
		add	di, 6
		call	printS
		pop	si
		;
                mov     bl, es:[esi + 8]
		mov	cx, 1
		add	di, 8
		call	printH
		;
		add	di, 4
		cmp	Cfg_Blk_Type, 0
		jne	@@draw_SPUS
		mov	ebx, es:[esi + 10]
                call    printNsids
                ; --- UPATS
                cmp     bl, 0ffh
                jne     @@draw_SPUS
                movzx   ebx, BYTE PTR es:[esi + 5]
                cmp     bl, 0ffh
                je      @@draw_SPUS
                mov     cx, 2
                call    printD
                mov     al, '-'
                mov     cl, 1
                push    di
                add     di, 4
                call    printC
                add     di, 2
                movzx   ebx, WORD PTR es:[esi + 14]
                mov     cx, 4
                call    printD
                pop     di
                ;
@@draw_SPUS:	mov	ebx, es:[esi + 1]
		mov	cx, 32
		add	di, 20
		call	printBx
                ;
@@draw_DVO:     add     di, 82
                push    si
                lea     si, str_no_DVO
                call    printS
                pop     si
                mov     al, es:[esi + 9] ; MVT и для многопрефиксной и для однопрефиксной
                cmp     al, 0ffh
                je      @@line_next
                xor     cx, cx
@@draw_DVO_loop:bt      ax, cx
                jc      @@draw_DVO_next
                push    esi
                mov     si, cx
                shl     si, 3
                add     si, OFFSET str_DVO
                call    printSdi
                add     di, 2
                pop     esi
@@draw_DVO_next:inc     cx
                cmp     cx, 6
                jbe     @@draw_DVO_loop
                ; ---
@@line_next:	pop	di
		pop	cx
		pop	ebx
		inc	bx
		add	di, 160
                ; --- {Paul} -- <19/06/12> -
                push    eax bx cx
                mov     eax, 1
                mov     cl, 4
                test    FG_PROGRAM, _fg_prg_multy_pref
                jz      @@Go_old_shl
                movzx   bx, Cfg_Blk_Type
                mov     cl, TAB_TIP_LN[bx]
 @@Go_old_shl:  shl     eax, cl
                add     esi, eax ;;;16
                pop     cx bx eax
                ; --- {Paul} -- <END>
		inc	cx
		cmp	cx, 32
		jb	@@line_loop
		pop	bx
		jmp	GLOB_WIN
Refresh_Cfg_Ln_Win	ENDP


; --- просмотр тарификационной сети (зовётся из Win_kla.INC)
;
Refresh_Tar_Net_Win     PROC
                push    bx
                ; --- вывод номера ЦК
		mov	es, ss:SEG_AXD
		xor	al, al
		xor	ebx, ebx
@@find_loop:	cmp 	es:TIS_TIS[bx], 16
		jae	@@find_next_2
		cmp	al, pos_Curr_CK
		jb	@@find_next
		je	@@found
		pop	bx
		ret
@@find_next:	inc	al
@@find_next_2:	inc	bx
		cmp	bl, KL_TIS
		jb	@@find_loop
		pop	bx
		ret
		; ---
@@found:        mov     ah, _color_Tar_Net_Win
		mov	cx, 2
                mov     di, _pos_Tar_Net_CK_Num
		call	printD
                ; --- выводим саму сеть для этого ЦК
                mov     bp, bx
                shl     bp, 7
                xor     bx, bx
                xor     si, si
                mov     es, ss:SEG_AXD
@@loop:         mov     bl, es:TIS_TIS[bp + si]
                cmp     bl, KL_TIS
                jae     @@next
                ; --- считаем координаты
                mov     di, _pos_Tar_Net
                ; --- вычисляем Y
		mov     ax, 160
		movzx	cx, bh
                and     cx, 7
		mul	cx
                add     di, ax
		; --- вычисляем X
                mov     ax, 18;;;9
		movzx	cx, bh
                shr     cx, 3
		mul	cx
		add	di, ax
                ; --- выводим № ЦК в сети
                mov     ah, _color_Tar_Net_Win
                push    bx
                movzx   ebx, si
                mov     cx, 2
                call    printD
                pop     bx
                ; --- выводим № модема для ЦК с № в сети
                add     di, 10
                mov     cx, 1
                cmp     bl, 0fh
                ja      @@bad_modem
@@modem:        call    printH
                jmp     @@continue
@@bad_modem:    mov     al, ch_Query
                call    printC
@@continue:     inc     bh
                ; ---
@@next:         inc     si
                cmp     si, KL_TIS
                jb      @@loop
                ; ---
                pop     bx
		jmp	GLOB_WIN
Refresh_Tar_Net_Win     ENDP

; --- просмотр списка телефонных номеров (зовётся из Win_kla.INC)
;
Refresh_Tel_Num_Win     PROC
		; ---
		mov	eax, Cfg_NLOG_Count
		cmp	eax, count_Prev_NLOG
		je	GLOB_WIN
		mov	count_Prev_NLOG, eax
		; ---
Refresh_Tel_Num_Win_2:
                push    bx
                ; --- вывод числа линий
                mov     ah, _color_Tel_Num_Hi
                mov     ebx, Cfg_NLOG_Count
                mov     cx, 5
                mov     di, _pos_Tel_Num_Count
                call    printD
		; ---
                mov     ebx, pos_Curr_Tel_Num
                mov     cx, _count_Lines_in_Scr
                mov     esi, ebx
                shl     esi, 3
                mov     es, ss:SEG_NLOG
@@loop:         push    cx
                ; --- вычисляем позицию
                mov     di, _pos_Tel_Num_Num
                ; --- вычисляем Y
		mov     ax, 160
                mov     ecx, ebx
                and     ecx, 1fh
		mul	cx
                add     di, ax
                ; --- вычисляем X
                mov     eax, ebx
                sub     eax, pos_Curr_Tel_Num
                cmp     eax, 32
                jb      @@draw_column
                add     di, _width_Column
                cmp     eax, 64
                jb      @@draw_column
                add     di, _width_Column
                ; --- выводим счётчик линий
@@draw_column:  mov     ah, _color_Tel_Num_Win
		cmp     ebx, pos_Found_Tel_Num
		jne	@@draw_Npp
                mov     ah, clYELLOW * 16 + cBLUE;_color_Tel_Num_Hi
@@draw_Npp:     mov     cx, 5
                call    printD
		; --- проверяем SEG_NLOG на конец данных
                cmp	ebx, Cfg_NLOG_Count
		jae	@@data_end
                ; --- выводим логический номер абонента
                push    ebx
                mov     ebx, es:[esi]
                add     di, 14
                call    printNlog
                ; --- выводим абсолютный физический номер абонента
                mov     ebx, es:[esi + 4]
                add     di, 18
                call    printNphys
                pop     ebx
		jmp	@@next
                ; ---
@@data_end:     mov	al, ch_Clear
		mov	ah, _color_Tel_Num_Win
		mov	cx, 7
		add	di, 14
		call	printC
		mov	cx, 9
		add	di, 18
		call	printC
                ; ---
@@next:         pop     cx
                inc     ebx
                add     esi, 8
		dec	cx
		jnz	@@loop
                pop     bx
		jmp	GLOB_WIN
Refresh_Tel_Num_Win     ENDP

; --- просмотр списка номеров УПАТС (зовётся из Win_kla.INC)
;
Refresh_U_Num_Win       PROC
		; ---
                mov     eax, Cfg_ULOG_Count
		cmp	eax, count_Prev_NLOG
		je	GLOB_WIN
		mov	count_Prev_NLOG, eax
		; ---
Refresh_U_Num_Win_2:
                push    ebx
                ; --- вывод числа линий
                mov     ah, _color_Tel_Num_Hi
                mov     ebx, Cfg_ULOG_Count
                mov     cx, 5
                mov     di, _pos_Tel_Num_Count + 4
                call    printD
		; ---
                mov     ebx, pos_Curr_Tel_Num
                mov     cx, _count_Lines_in_Scr
                mov     esi, ebx
                shl     esi, 3
                mov     es, ss:SEG_ULOG
@@loop:         push    cx
                ; --- вычисляем позицию
                mov     di, _pos_Tel_Num_Num
                ; --- вычисляем Y
		mov     ax, 160
                mov     ecx, ebx
                and     ecx, 1fh
		mul	cx
                add     di, ax
                ; --- вычисляем X
                mov     eax, ebx
                sub     eax, pos_Curr_Tel_Num
                cmp     eax, 32
                jb      @@draw_column
                add     di, _width_Column
                cmp     eax, 64
                jb      @@draw_column
                add     di, _width_Column
                ; --- выводим счётчик линий
@@draw_column:  mov     ah, _color_Tel_Num_Win
		cmp     ebx, pos_Found_Tel_Num
		jne	@@draw_Npp
                mov     ah, clYELLOW * 16 + cBLUE;_color_Tel_Num_Hi
@@draw_Npp:     mov     cx, 5
                call    printD
                ; --- проверяем SEG_ULOG на конец данных
                cmp     ebx, Cfg_ULOG_Count
		jae	@@data_end
                ; --- выводим логический номер абонента УПАТС
                push    ebx
                mov     ebx, es:[esi]
                add     di, 14
                call    printUlog
                ; --- выводим абсолютный физический номер абонента
                mov     ebx, es:[esi + 4]
                add     di, 18
                call    printNphys
                pop     ebx
		jmp	@@next
                ; ---
@@data_end:     mov	al, ch_Clear
		mov	ah, _color_Tel_Num_Win
		mov	cx, 7
		add	di, 14
		call	printC
		mov	cx, 9
		add	di, 18
		call	printC
                ; ---
@@next:         pop     cx
                inc     ebx
                add     esi, 8
		dec	cx
		jnz	@@loop
                pop     ebx
		jmp	GLOB_WIN
Refresh_U_Num_Win       ENDP

; ---- обновление окна по сохраненным параметрам во Flash
Refresh_MO_Slct_Win_Far PROC    FAR
push    ax cx di
                mov     cx, 5
                mov     di, frst_dig_MO;;;_pos_Win_Cfg + 2*160 + 2
                mov     ah, _color_normal_MO
                ; --- рисуем изменения ---
                test    FG_Curr_MO_Conf, _set_all_MO
                jnz     @@_set_all_MO
                test    FG_Curr_MO_Conf, _set_sorm_MO
                jnz     @@_set_sorm_MO
                test    FG_Curr_MO_Conf, _set_black_MO
                jnz     @@_set_black_MO
                test    FG_Curr_MO_Conf, _set_tarif_MO
                jnz     @@_set_tarif_MO
                jmp     @@go_end

@@_set_all_MO:  ;  ---  SET ALL MO ---
                push    eax bx cx si di
                push    es
                mov     ax, ds
                mov     es, ax
                @Clear_Buffs
                ; =--- заполним все буфера ---=
                mov     cx, 255
                mov     bx, 1
                lea     si, MO_Numb
  @@set_MO:     mov     [si], bl
                inc     bx
                inc     si
                loop    @@set_MO
                mov     curr_MO_ptr, 0FFFFh                 ; <- bad pointer
                mov     cx, 255
                mov     bx, 1
                lea     si, MO_Numb_235
  @@set_MO_235: mov     [si], bl
                inc     bx
                inc     si
                loop    @@set_MO_235
                mov     curr_MO_ptr_235, 0FFFFh             ; <- bad pointer
                pop     es
                pop     di si cx bx eax
                jmp     @@go_end
@@_set_sorm_MO: ; --- SET SORM MO ----
                push    eax cx si di
                push    es
                mov     ax, ds
                mov     es, ax
                ; --- запишем в рабочие буфера команды данного типа ---
                @Control_Type_MO  SORM_Numb, SORM_Numb_235, 20
                pop     es
                pop     di si cx eax
                jmp     @@go_end
@@_set_black_MO:; --- SET BLACK MO ----
                push    eax cx si di
                push    es
                mov     ax, ds
                mov     es, ax
                ; --- запишем в рабочие буфера команды данного типа ---
                @Control_Type_MO  Black_Numb, Black_Numb_235, 10
                pop     es
                pop     di si cx eax
                jmp     @@go_end
@@_set_tarif_MO:; --- SET TARIF MO ----
                push    eax cx si di
                push    es
                mov     ax, ds
                mov     es, ax
                ; --- запишем в рабочие буфера команды данного типа ---
                @Control_Type_MO  Tarif_Numb, Tarif_Numb_235, 20
                pop     es
                pop     di si cx eax
                ; --- the end ---
 @@go_end:      pop     di cx ax
                retf
Refresh_MO_Slct_Win_Far ENDP
; --- перерисовка окна выбора команд МО ---
Refresh_MO_Slct_Win     PROC
push    ax cx di
                mov     cx, 5
                mov     di, frst_dig_MO;;;_pos_Win_Cfg + 2*160 + 2
                mov     ah, _color_normal_MO
                ; --- рисуем изменения ---
                test    FG_Curr_MO_Num, _spc_press
                jnz     @@_press_space
                test    FG_Curr_MO_Num, _page_down
                jnz     @@_page_down
                test    FG_Curr_MO_Num, _page_up
                jnz     @@_page_up
                test    FG_Curr_MO_Num, _235_btn
                jnz     @@_235_Btn
                test    FG_Curr_MO_Num, _clear_MO
                jnz     @@_clear_MO
                test    FG_Curr_MO_Num, _set_all_MO
                jnz     @@_set_all_MO
                test    FG_Curr_MO_Num, _reset_all_MO
                jnz     @@_reset_all_MO
                test    FG_Curr_MO_Num, _set_sorm_MO
                jnz     @@_set_sorm_MO
                test    FG_Curr_MO_Num, _set_black_MO
                jnz     @@_set_black_MO
                test    FG_Curr_MO_Num, _set_tarif_MO
                jnz     @@_set_tarif_MO

                jmp     @@_print                         ; уйдем на перерисовку по стрелкам
                ; ***
@@_clear_MO:    ; ------  CLEAR MO  ------
                push    eax cx di
                push    es
                mov     ax, ds
                mov     es, ax
                ; ---- clear usual type MO
                lea     di, MO_Numb
                test    FG_Curr_MO_Num, _235_cmd
                jz      @@no_235
                lea     di, MO_Numb_235                  ; очистим 235-ую
@@no_235:       mov     cx, 64
                xor     eax, eax
                CLD
                rep     STOSD
                ; --- перерисуем страницу ---
                @Refresh_MO_PAGE
                pop     es
                pop     di cx eax
                and     FG_Curr_MO_Num, not _clear_MO
                jmp     @@ok_print
                ; ------       -------
@@_set_all_MO:  ;  ---  SET ALL MO ---
                push    eax bx cx si di
                push    es
                mov     ax, ds
                mov     es, ax
                @Clear_Buffs
                ; =--- заполним все буфера ---=
                mov     cx, 255
                mov     bx, 1
                lea     si, MO_Numb
  @@set_MO:     mov     [si], bl
                inc     bx
                inc     si
                loop    @@set_MO
                mov     curr_MO_ptr, 0FFFFh                 ; <- bad pointer
                mov     cx, 255
                mov     bx, 1
                lea     si, MO_Numb_235
  @@set_MO_235: mov     [si], bl
                inc     bx
                inc     si
                loop    @@set_MO_235
                mov     curr_MO_ptr_235, 0FFFFh             ; <- bad pointer
                ; --- перерисуем страницу ---
                @Refresh_MO_PAGE
                pop     es
                pop     di si cx bx eax
                and     FG_Curr_MO_Conf, 1100010111111111b      ; обновим сохраняемые флаги
                or      FG_Curr_MO_Conf, _set_all_MO            ; поставим нужный флаг
                and     FG_Curr_MO_Num, not _set_all_MO
                jmp     @@ok_print
@@_reset_all_MO:; ---- RESET ALL MO  ----
                push    eax cx di
                push    es
                mov     ax, ds
                mov     es, ax
                @Clear_Buffs
                ; --- перерисуем страницу ---
                @Refresh_MO_PAGE
                pop     es
                pop     di cx eax
                and     FG_Curr_MO_Conf, 1100010111111111b      ; обновим сохраняемые флаги
                and     FG_Curr_MO_Num, not _reset_all_MO
                jmp     @@ok_print
@@_set_sorm_MO: ; --- SET SORM MO ----
                push    eax cx si di
                push    es
                mov     ax, ds
                mov     es, ax
                ; --- запишем в рабочие буфера команды данного типа ---
                @Control_Type_MO  SORM_Numb, SORM_Numb_235, 20
                ; --- перерисуем страницу ---
                @Refresh_MO_PAGE
                pop     es
                pop     di si cx eax
                and     FG_Curr_MO_Conf, 1100010111111111b      ; обновим сохраняемые флаги
                or      FG_Curr_MO_Conf, _set_sorm_MO           ; поставим нужный флаг
                and     FG_Curr_MO_Num, not _set_sorm_MO
                jmp     @@ok_print
@@_set_black_MO:; --- SET BLACK MO ----
                push    eax cx si di
                push    es
                mov     ax, ds
                mov     es, ax
                ; --- запишем в рабочие буфера команды данного типа ---
                @Control_Type_MO  Black_Numb, Black_Numb_235, 10
                ; --- перерисуем страницу ---
                @Refresh_MO_PAGE
                pop     es
                pop     di si cx eax
                and     FG_Curr_MO_Conf, 1100010111111111b      ; обновим сохраняемые флаги
                or      FG_Curr_MO_Conf, _set_black_MO          ; поставим нужный флаг
                and     FG_Curr_MO_Num, not _set_black_MO
                jmp     @@ok_print
@@_set_tarif_MO:; --- SET TARIF MO ----
                push    eax cx si di
                push    es
                mov     ax, ds
                mov     es, ax
                ; --- запишем в рабочие буфера команды данного типа ---
                @Control_Type_MO  Tarif_Numb, Tarif_Numb_235, 20
                ; --- перерисуем страницу ---
                @Refresh_MO_PAGE
                pop     es
                pop     di si cx eax
                and     FG_Curr_MO_Conf, 1100010111111111b      ; обновим сохраняемые флаги
                or      FG_Curr_MO_Conf, _set_tarif_MO          ; поставим нужный флаг
                and     FG_Curr_MO_Num, not _set_tarif_MO
                jmp     @@ok_print
@@_press_space:  ; --- SPACE ----
                ; ---- найдем по номеру pos_Curr_MO_Num код команды ---
        push    bx dx si di
                mov     dx, pos_Curr_MO_Num
                call    Find_MO_number                 ; найдем код команды текущей строки
                ;;;jc      @@bad_print                    ; "плохая" команда
                ; ---------              -----         ; и поместим его в bx
                call    Find_Numb_MO_win               ; узнаем, отмечена ли команда?
                test    FG_Curr_MO_Num, _find_numb
                jnz     @@clear_star                   ; да - отмечена, уберем звездочку
                ;;;jc      @@clear_star                  ; уберем звездочку
                mov     si, offset Star_MO_Win        ; поставим звездочку
                call    Put_Numb_MO                   ; и поместим код команды в буфер отмеченных команд
                jnc     @@bad_print
                jmp     @@print_star
@@clear_star:   and     FG_Curr_MO_Num, not _find_numb
                mov     si, offset Null_MO_Win
@@print_star:   ; ---- прорисуем изменения ---
                mov     ah, _color_MO_slct
                add     di, pos_Curr_MO_Num
                add     di, 8
                call    printS
@@bad_print:    and     FG_Curr_MO_Num, not _spc_press
        pop     di si dx bx
                jmp     @@ok_print
                ; ----                      ----
@@_page_down:    ; --- PAGE_DOWN ----
        push    ebx
                cmp     num_page_MO, 2          ; столько всего страниц
                jae     @@exit_pge_dn
                inc     num_page_MO             ; следующая страница
                mov     ebx, 151;;;curr_MO
                ; --- Refresh page ---
                @Refresh_pge_MO ebx, 15, 255
                ; -----          ------
@@exit_pge_dn:  and     FG_Curr_MO_Num, not _page_down
        pop     ebx
                jmp     @@ok_print
                ; ---                  ---
@@_page_up:      ; --- PAGE_UP ---
        push    ebx
                cmp     num_page_MO, 1
                jbe     @@exit_pge_up
                mov     ebx, 1
                dec     num_page_MO             ; предыдущая страница
                ; --- refresh page ---
                @Refresh_pge_MO ebx, 15, 150
                ; ----           ------
@@exit_pge_up:  and     FG_Curr_MO_Num, not _page_up
        pop     ebx
                jmp     @@ok_print
                ; ----           ------
@@_235_Btn:       ; --- 235 ON ---
                push    di cx
                mov     cl, 12
                mov     ah, _color_MO_235
                test    FG_Curr_MO_Num, _235_cmd
                jnz     @@_235_on
                mov     ah, _color_normal_MO
 @@_235_on:     add     di, 26*160 + 94
                call    PrintA
                pop     cx di
                ; --- Refresh page ---
                push    edx
                mov     edx, 255                         ; max значение для тек. окна
                mov     ebx, 151
                cmp     num_page_MO, 2
                jae     @@snd_pge
                mov     ebx, 1
                mov     edx, 150                         ; или такое
  @@snd_pge:    mov     ah, _color_normal_MO
                @Refresh_pge_MO ebx, 15, edx
                pop     edx
                ; -----          ------
                and     FG_Curr_MO_Num, not _235_btn
                jmp     @@ok_print
                ; ***
                ; ---
@@_print:       test    FG_Curr_MO_Num, _arrows
                jz      @@ok_print
                ; --- перерисуем пред. позицию ---
                push    di
                mov     ah, _color_normal_MO
                add     di, prev_Curr_MO_Num
                call    PrintA
                pop     di
                ; --- выделим команду
                mov     ah, _color_MO_slct
                add     di, pos_Curr_MO_Num
                call    PrintA
                and     FG_Curr_MO_Num, not _arrows
                ; ---                  ---
@@ok_print:     test    FG_KLA, _b_00
                jnz     @@ok_ok
                cmp     prev_dig_MO, 0
                jz      @@ok_ok
                mov     prev_dig_MO, 0             ; при отпускании ALT - начнем заново ловить цифры
   @@ok_ok:     pop     di cx ax
                jmp     GLOB_WIN
Refresh_MO_Slct_Win     ENDP

; --- Перерисовка окна отладчика "черного" списка ---
Refresh_BL_DBG_Win      PROC
                ; --- выделяем текущие счетчики
        push    ax cx si di
                cmp     _black_active, 255
                jne      @@EXIT
                cmp     BL_dbg_q, 255
                jne     @@BL_DBG                                   ; обычный ч.с.
                ; ---- Здесь квитированный(quit) черный список
                ; --- нарисуем окно
                mov     ah, _color_normal_BL;;;_color_normal_MO
                mov     di, _pos_Tel_Num_Win + 2720  ; не с самого верха
                mov     si, offset Bl_Debug_Win_guit
                add     si, 86
                mov     cx, 13
                call    printWin
                ; --- пробуем выделить основные точки прохода
                ; --  есть префикс или нет ?
                cmp     _differ_black_q, 0
                je      @@curr_str_q
                mov     al, 2
               @PrintPre_Black_Debug  _color_BL_DBG_slct, 2560
                cmp     _differ_black_q, 1
                jne     @@next_pref
                ; --- АОН с префиксом
                mov     ah, _color_BL_DBG_prev
                jmp     @@print_pref
 @@next_pref:   cmp     _differ_black_q, 2
                jne     @@what_rul
@@print_pref:   call    PrintA
                ; ---------------------------
                ; --- узнаем правило
@@what_rul:     cmp     _black_differ_q, 0
                je      @@curr_str_q
                cmp     _black_differ_q, 1
                jne     @@anth_rul_q
                ; --- Другие правила
                mov     al, 7
                jmp     @@print_rul_q
@@anth_rul_q:   ; --- правило №0
                mov     al, 6
                cmp     _black_differ_q, 2
                jne     @@curr_str_q
@@print_rul_q: @PrintPre_Black_Debug _color_BL_DBG_prev, 2560
                call    PrintA
                ; ----------------------------
@@curr_str_q:   ; --- перерисуем фон тек. строки для исх. ч.с.(quit) ---
                mov     al, _numb_dds_bl_q
               @PrintPre_Black_Debug  _color_BL_DBG_slct, 2560
                cmp     _numb_dds_bl_q, 2
                jne     @@next_point_q
                ; -- здесь неопределенный префикс
                mov     ah, _color_BL_DBG_bad
                jmp     @@print_slct_q
@@next_point_q: cmp     _numb_dds_bl_q, 5
                jne     @@next_point_q_1
                ; -- здесь "Белый спискок"
                mov     ah, _color_BL_DBG_prev
                jmp     @@print_slct_q
@@next_point_q_1:
                cmp     _numb_dds_bl_q, 10
                jne     @@print_slct_q
                ; --- здесь все ОК
                mov     ah, _color_normal_BL
@@print_slct_q: call    PrintA
                ; --- перерисуем фон тек. строки для вх. ч.с.(quit)
                cmp     _numb_dds_bl_in_q, 5
                jbe     @@slct_print_q
                ; -- отметим правило №0
                mov     al, 5
               @PrintPre_Black_Debug _color_BL_DBG_prev, 2602
                call    PrintA
@@slct_print_q: mov     al, _numb_dds_bl_in_q
               @PrintPre_Black_Debug _color_BL_DBG_slct, 2602
                cmp     _numb_dds_bl_in_q, 8
                jne     @@print_slct_in_q
                mov     ah, _color_normal_BL
@@print_slct_in_q:      call    PrintA
                ; --- OK or not OK ? ---
                mov     ah, _color_avar
                mov     cx, 8
                mov     di, frst_dig_MO + 2560
                add     di, 160*12 + 12
                ; --- здесь исходящий ч.с.
                cmp     _numb_dds_bl_q, 10
                jne     @@print_not_ok_q
                mov     ah, _color_BL_DBG_prev
@@print_not_ok_q:       call    PrintA
                ; --- здесь входящий ч.с.
                mov     ah, _color_avar
                cmp     _numb_dds_bl_in, 8
                jne     @@not_ok_print_q
                mov     ah, _color_BL_DBG_prev
@@not_ok_print_q:
                add     di, 42
                call    PrintA
                ; =============================================
@@BL_DBG:       ; ---- Здесь обычный ч.с.
                ; --- надо перерисовывать окно
                mov     ah, _color_normal_BL;;;_color_normal_MO
                mov     di, _pos_Tel_Num_Win + 160  ; не с самого верха
                mov     si, offset Bl_Debug_Win
                add     si, 86
                mov     cx, 13
                call    printWin
                ; --- пробуем выделить основные точки прохода
                mov     al, 7
                cmp     _differ_black, 1
                ja      @@print_rul                  ; выделим "Правило №0"
                cmp     _differ_black, 0
                je      @@curr_str
                mov     al, 8                        ; выделим "Другие правила"
@@print_rul:   @PrintPre_Black_Debug _color_BL_DBG_prev, 0
                call    PrintA
                ; -----    -----      -----    -----
                ; --- перерисуем фон тек. строки для исх. ч.с. ---
@@curr_str:     mov     al, _numb_dds_bl
               @PrintPre_Black_Debug  _color_BL_DBG_slct, 0
                cmp     _numb_dds_bl, 6
                jne     @@next_point
                ; -- здесь "Белый список"
                mov     ah, _color_BL_DBG_prev
                jmp     @@print_slct
@@next_point:   cmp     _numb_dds_bl, 11
                jne     @@print_slct
                ; --- здесь все ОК
                mov     ah, _color_normal_BL
@@print_slct:   call    PrintA
                ; --- перерисуем фон тек. строки для вх. ч.с. ---
                cmp     _numb_dds_bl_in, 4
                jbe     @@slct_print
                ; -- отметим правило №0
                mov     al, 4
               @PrintPre_Black_Debug _color_BL_DBG_prev, 42
                call    PrintA
@@slct_print:   mov     al, _numb_dds_bl_in
               @PrintPre_Black_Debug _color_BL_DBG_slct, 42
                cmp     _numb_dds_bl_in, 7
                jne     @@print_slct_in
                mov     ah, _color_normal_BL
@@print_slct_in:      call    PrintA

                ; --- OK or not OK ? ---
                mov     ah, _color_avar
                mov     cx, 8
                mov     di, frst_dig_MO
                add     di, 160*12 + 12
                ; --- здесь исходящий ч.с.
                cmp     _numb_dds_bl, 11
                jne     @@print_not_ok
                mov     ah, _color_BL_DBG_prev
@@print_not_ok: call    PrintA
                ; --- здесь входящий ч.с.
                mov     ah, _color_avar
                cmp     _numb_dds_bl_in, 7
                jne     @@not_ok_print
                mov     ah, _color_BL_DBG_prev
@@not_ok_print: add     di, 42
                call    PrintA
                ; ----------------------
 @@EXIT:        pop     di si cx ax
                jmp     GLOB_WIN
Refresh_BL_DBG_Win      ENDP
; --------------------------------------
; --- перерисовка окна вывода счетчиков ошибок ПУМы
Refresh_Puma_cnts_Win   PROC
                push    fs
                mov     fs, SS:SEG_PUMA
                push    ax ebx edx cx ebp edi esi
                mov     cx, 2                   ; for PUMA-cto
 @@Refr_loop:   push    cx
                xor     ebp, ebp
                add     ebp, puma_dbg_ofs
                ; -- КС
                ; ====----====
                mov     edi, _PUMA_TRAN_OFFS
                add     edi, PUMA_trans_ofs
                mov     ah, _color_puma_v
                movzx   ebx, BYTE PTR fs:[edi].ecsc
                mov     di, _pos_Win_Cfg + 3*160 + 22 + 4
                add     di, puma_win_ofs
                cmp     bl, Puma_cnts_tar[ebp]
                je      @@out_ks
                mov     Puma_cnts_tar[ebp], bl
                ; -- обновим таймер
                push    ebx di
                xor     ebx, ebx
                ; -- Puma counts value is null?
                cmp     Puma_cnts_tar[ebp], 0
                jz      @@bad_timer                 ; yes
                @_timer_ebx
 @@bad_timer:   ; -- запомним значение timer для KC
                mov     Puma_tmr_tar[ebp], ebx
                mov     cl, 8
                mov     di, _pos_Win_Cfg + 3*160 + 38
                add     di, puma_win_ofs
                ; -- Puma timer value is null?
                cmp     ebx, 0
                jnz     @@prnt_tmr
                ; -- print position for timers
                push    si
                mov     si,  offset Puma_counts_val
                call    printS
                pop     si
                jmp     @@go_cnts
  @@prnt_tmr:   call    printH
   @@go_cnts:   pop     di ebx
                ; -- счетчик KC
                mov     cl, 2
                call    printH
  @@out_ks:     sub     di, 4
                ; ---------------------
                ; -- остальные счетчики
                ; =====------======----
                ; --- prepear for others
                mov     cx, 9
                xor     esi, esi
                add     esi, puma_dbg_ofs
                xor     edx, edx
                ; ---- let's refresh counts ---
  @@pr_cnts:    push    edi
                mov     edi, _PUMA_TRAN_OFFS
                add     edi, PUMA_trans_ofs
                movzx   ebx, WORD PTR fs:[edx + edi].ulbc
                pop     edi
                cmp     bx, WORD PTR Puma_cnts_tar[ebp + 1]
                je      @@out_othrs
                mov     WORD PTR Puma_cnts_tar[ebp + 1], bx
                ; -- обновим таймеры
                push    cx
                push    ebx di
                push    eax edx
                xor     ebx, ebx                                ;  пока bad timer
                ; -- Puma counts value is null?
                cmp     WORD PTR Puma_cnts_tar[ebp + 1], 0
                jz      @@bad_timer_1                           ; yes!
                @_timer_ebx
 @@bad_timer_1: ; -- прорисовка timer-ов
                ; --- запомним timer для каждого счетчика
                mov     Puma_tmr_tar[esi + 4], ebx
                mov     cl, 8
                mov     di, _pos_Win_Cfg + 4*160 + 38           ; timer .ulbc
                add     di, puma_win_ofs
                mov     eax, 80
                mul     edx
                add     di, ax
                pop     edx eax
                ; -- Puma timer value is null?
                cmp     ebx, 0
                jnz     @@prnt_tmr_1
                ; -- yes
                push    si
                mov     si,  offset Puma_counts_val
                call    printS
                pop     si
                jmp     @@go_cnts_1
 @@prnt_tmr_1:  call    printH
  @@go_cnts_1:  pop     di ebx
                ; -- счетчики
                add     di, 160
                mov     cl, 4
                call    printH
                pop     cx
                jmp     @@go_next
                ; -- без перерисовки
   @@out_othrs: add     di, 160
                ; -- по всем счетчикам
    @@go_next:  add     ebp, 2          ; it's for counts
                add     esi, 4          ; it's for timer
                add     edx, 2          ; it's for Puma trans struct
                dec     cx
                jcxz    @@go_cto
                jmp     @@pr_cnts
   @@go_cto:    pop     cx
                ; --- CTO is active ?
                test    Puma_flg, _cto_active
                jz      @@go_conf
                mov     PUMA_trans_ofs, PUMA_Trans_Size
                mov     puma_win_ofs, 60
                mov     puma_dbg_ofs, 59
                dec     cx
                jcxz    @@go_zero
                jmp     @@Refr_loop                 ; // Redrawing CTO-channnel
      @@go_zero:; --  zeroing params
                mov     PUMA_trans_ofs, 0
                mov     puma_win_ofs, 0
                mov     puma_dbg_ofs, 0
                ; --- Puma conf is active?
     @@go_conf: test    Puma_flg, _conf_active
                jz      @@go_out
                call    Refresh_Debug_conf_Win      ; // Redrawing conf mode
      @@go_out: pop     esi edi ebp cx edx ebx ax
                pop     fs
                jmp     GLOB_WIN
Refresh_Puma_cnts_Win   ENDP
; --------------------------------------
; =======================================================
ADR_VIU5        LABEL   WORD
                DW      offset  VIU5_OBSH               ; 0
                DW      offset  Refresh_null            ; 1
                DW      offset  Refresh_WinTarNet       ; 2  Tar_net
                DW      offset  Refresh_null            ; 3  DOS
                DW      offset  Refresh_null            ; 4  X25
                DW      offset  Refresh_WinNumLen       ; 5  NumLen
                DW      offset  Refresh_null            ; 6
                DW      offset  Refresh_null            ; 7  Prefix
                DW      offset  VIU5_CHNG_PRGFLGS       ; 8  Flags
                DW      offset  Refresh_null            ; 9  BlackList
                DW      offset  Refresh_null            ; 10 Reboot
                DW      offset  Refresh_null            ; 11
                DW      offset  Refresh_null;;;TC            ; 12 контроль ТЧ СОРМ
                DW      offset  Refresh_null;;;OZUK            ; 13 контроль ОЗУ коммутации
                DW      offset  Refresh_bad_MO          ; 14 контроль ошибочных пакетов

                DW      offset  Refresh_null            ; 15 проверка тарификации
                DW      offset  Refresh_null            ; 16 ввод пароля эмулятора СОРМ
                DW      offset  Refresh_null            ; 17 выбор режима эмулятора СОРМ
                DW      offset  Refresh_null            ; 18 ввод данных для эмулятора СОРМ

                DW      offset  Refresh_null            ; 19 просмотр прохождения контрольных точек

Refresh_null:   ret                                     ; те которые не надо рефрешить...


VIV_NIGNEGO_OKNA:
                MOV     DI,SOST_VIU5
                SHL     DI,1
                JMP     ADR_VIU5[DI]

; --- Вывод общего нижнего окна ------------------------------------------------
VIU5_OBSH       PROC
                ; --- обработка вывода заворота
                mov     ah, _color_normal_f
                LEA     SI,NET_ZV               ; настроим SI на ═══════
                TEST    FG_SORM3,M_OR_1         ; есть ли заворот?
                JZ      SHORT NET_ZV2           ; если нет, то пропустим
                LEA     SI,EST_ZV               ; настроим SI на ЗАВОРОТ
NET_ZV2:
                MOV     DI, _pos_loopback       ;\  выведем заворот или
                CALL    printSscr               ;/  его отсутствие на экран

                ; стадия МО
                MOV     DI,MO_IDX               ;\
                AND     DI,7H                   ;|
                MOV     AH,WIN_MO[DI]           ;|
                SHL     DI,3                    ;|
                LEA     SI,VIV_MO[DI]           ;|   mo stade
                MOV     DI,_pos_MO              ;|
                call    printS                  ;/

                ; --- индикация источников питания
                ; индикация первого источника питания
                LEA     SI,NORMA_WR     ;
                MOV     AH,NORMA        ;
                TEST    statusPit, _b_00;
                JZ      SHORT DL_V5_111 ;
                LEA     SI,AVARIY_WR    ;
                MOV     AH,AVA_VIV      ;
DL_V5_111:
                MOV     DI,_pos_ip1     ;
                call    printS          ;
                ; индикация второго источника питания
                LEA     SI,NORMA_WR     ;
                MOV     AH,NORMA        ;
                TEST    statusPit, _b_04;
                JZ      SHORT DL_V5_11  ;
                LEA     SI,AVARIY_WR    ;
                MOV     AH,AVA_VIV      ;
DL_V5_11:
                MOV     DI,_pos_ip2     ;
                call    printS          ;

                ; --- вывод состояния ЦТО
                mov     al, _PT_CTO
		DB      9AH
                DW      OFFSET PUMA_Get_State_Far
                DW      120
                jnc     VIU5_COM1
                mov     bl, al
                mov     al, clRED * 16 + clWHITE
                or      ah, ah
                jz      VIU5_CTO_Show
                mov     al, clGREEN * 16 + clWHITE
VIU5_CTO_Show:  mov	ah, al
		mov     cx, 1
                mov     di, 46 * 160 + 62 * 2
                call    printH
                ; --- вывод состояния СОМ1
VIU5_COM1:
IF _MSP EQ TRUE
                mov     al, _PT_COM2
ELSE
                mov     al, _PT_COM1
ENDIF
		DB      9AH
                DW      OFFSET PUMA_Get_State_Far
                DW      120
                jnc     VIU5_End_PUMA
                mov     bl, al
                mov     al, clRED * 16 + clWHITE
                or      ah, ah
                jz      VIU5_COM1_Show
                mov     al, clGREEN * 16 + clWHITE
VIU5_COM1_Show: mov	ah, al
		mov     cx, 1
                mov     di, 47 * 160 + 62 * 2
                call    printH
VIU5_END_PUMA:

IF _MSP EQ TRUE

                mov     ah, NORMA               ;\
                MOV     ebx,FI                  ;|
                mov     cx, 3                   ;|
                MOV     DI,SMESH_T_F            ;| ФАЗА ФАПЧа
                CALL    printH                  ;/

                mov     ah, NORMA               ;\
                MOV     bx, VCAP                ;|
                mov     cx, 4                   ;|
                MOV     DI,SMESH_FAZ            ;| ЦАП ФАПЧа
                CALL    printH                  ;/
ELSE
; - - - - - - - - - - - - - - -  Ф А П Ч - - - - - - - - - - - - - - - - - - -
;
                MOV     ES,SS:SEG_PLL_DATA

                mov     ah, _color_pll
                lea     si, _str_pll_src
                cmp     WORD PTR es:_PRIORITY_SOURCE, 4        ; Источник ФАПЧ - Модем ?
                je      PLL_Show_Src
                mov     ah, _color_pll_err
                add     si, 4
PLL_Show_Src:   mov     di, _pos_pll_src
                call    printS

;		mov	al, 2
;                cmp     es:_IND_PHA, 0
;		je      PLL_Err_LED
;		dec	al
;PLL_Err_LED:	push	es
;		mov	es, ss:SEG_GS
;		mov	es:[440bh], al
;		pop	es

                mov     ah, _color_pll_err
                lea     si, _str_pll_err
                mov     di, _pos_pll_err
                cmp     es:_IND_PHA, 2
                ja      PLL_Show_Err
                movzx   si, es:_IND_PHA
                shl     si, 2
                add     si, OFFSET _str_pll_err
                cmp     es:_IND_PHA, 0
                jne     PLL_Show_Err
                mov     ah, _color_pll
PLL_Show_Err:   call    printS

                mov     ah, _color_pll
                mov     di, _pos_pll_pha
                MOV     ES,SS:SEG_PLL_DATA
                mov     ebx, es:_PHASE_IMAGE
                mov     cl, 4
                call    printH

		mov     di, _pos_pll_dac
                mov     dx, es:_CODE_DAC
		rol	dx, 4
		mov	bx, dx
		and	bx, 0fh
		mov	al, ASCII[bx]
                mov     es, ss:SEG_FS
		stosw
		rol	dx, 4
		mov	bx, dx
		and	bx, 0fh
		mov	al, ASCII[bx]
		stosw
		rol	dx, 4
		mov	bx, dx
		and	bx, 0fh
		mov	al, ASCII[bx]
		stosw
		rol	dx, 4
		mov	bx, dx
		and	bx, 0fh
		mov	al, ASCII[bx]
		stosw
ENDIF

                TEST    FG_PROG,1               ;Тест железа
                JZ      STARIE                  ;если старое
                CALL    NEW_IND_IKM             ;Если новое железо
STARIE:
        IF _MSP EQ TRUE
                MOV     CX,4;;;KL_1
        ELSE
                MOV     CX, KL_1
        ENDIF
                XOR     BP,BP
CN_PO_L1_L2:
                PUSH    CX
                mov     ax, _nom_ikm
                shr     ax, 5
                shl     ax, 1
                cmp     ax, bp
                jne     NEXT_L1_L2
                MOV     CX,6;;;8
                MOV     DI,SMESH_LL1[BP]
                LEA     SI,SPACE_VV
                shr     bp, 1
                mov     al, RSR_0[bp]
                shl     bp, 1
                TEST    FG_PROG,M_OR_0
		JNZ	SHORT CN_V5_113
                MOV     ES,SS:SEG_GS
                MOV     DL,AL
                AND     DL,1FH
                MOV     ES:[5380H],DL           ; лампочки ???
CN_V5_113:
                PUSH    CX
                MOV     AH,NORMA
                SHR     AL,1
                JNC     SHORT DL_V5_113
                MOV     AH,AVA_VIV
DL_V5_113:
                call    printS
                ADD     DI,160
                POP     CX
                LOOP    CN_V5_113
NEXT_L1_L2:     ADD     BP,2
                POP     CX
                LOOP    CN_PO_L1_L2

NOVIY:
        cmp fg_prog, 3
        je mono_viu
jmp mono_viu;;;<Yu>
                MOV     CX,KL_C101
                XOR     BP,BP
CN_PO_C1:
                PUSH    CX
                MOV     CX,4
                MOV     DI,SMESH_CC1[BP]
                LEA     SI,SPACE_VV
                MOV     ES,SEG_C101[BP]
                MOVZX   AX,ES:[ST3]
                OR      AL,0100B
                CMP     SOST_SDL[BP],6
                JE      SHORT DL_PO_C1_1
                AND     AL,1011B        ;ПРОПИСЬ DSR
DL_PO_C1_1:
                XOR     AL,07H
CN_C1_113:
                PUSH    CX
                MOV     AH,NORMA
                SHR     AL,1
                JNC     SHORT DL_C1_113
                MOV     AH,AVA_VIV
DL_C1_113:
                call    printS
                ADD     DI,160
                POP     CX
                LOOP    CN_C1_113
                ADD     BP,2
                POP     CX
                LOOP    CN_PO_C1
        jmp viu_end
mono_viu:
                ; состояние LAPB
                xor     si, si
                mov     di, 160 * 39 + 2 * 58
Loop_Sost_SDL:  mov     bx, SOST_SDL[si]
                mov     ah, cGREEN * 16 + clWHITE
                cmp     bl, 6
                je      Show_Sost_SDL
                cmp     bl, 7
                je      Show_Sost_SDL
                mov     ah, clRED * 16 + clWHITE
Show_Sost_SDL:  mov     cl, 1
                call    printH
                add     si, 2
                add     di, 2 * 4
                cmp     si, 2
                jbe     Loop_Sost_SDL
                ; состояние Х25
                xor     si, si
                mov     di, 160 * 40 + 2 * 58
Loop_Sost_L3:   mov     bx, SOST_L3[si]
                mov     ah, cGREEN * 16 + clWHITE
                cmp     bl, 6
                je      Show_Sost_L3
                mov     ah, clRED * 16 + clWHITE
Show_Sost_L3:   mov     cl, 1
                call    printH
                add     si, 2
                add     di, 2 * 4
                cmp     si, 2
                jbe     Loop_Sost_L3
                ; состояние СОРМ
                mov     al, '-'
                cmp     FG_SOST, 0
                je      Show_SORM
                mov     al, '+'
		cmp	SORM_Stoped, 0
		je      Show_SORM
		mov	al, 'x'
Show_SORM:      mov     es, ss:SEG_FS
                mov     di, 160 * 38 + 2 * 60
                mov     es:[di], al

        cmp fg_prog, 3
        jne viu_end
if _SORM_NO_MODEM ne TRUE
                push    es
                mov     es, ss:SEG_GS
                mov     bl, es:[04411h]
                pop     es
                mov     bh, 1
                mov     cx, 2
                lea     si, _SPACE_CHAR
                mov     di, 160 * 41 + 2 * 58
Loop_C101:      push    cx
                mov     cx, 2
Loop_C101_2:    mov     ah, cGREEN * 16 + clWHITE
                test    bl, bh
                jnz     Show_C101
                mov     ah, clRED * 16 + clWHITE
Show_C101:      call    printS
                shl     bh, 1
                add     di, 2 * 4
                loop    Loop_C101_2
                pop     cx
                shl     bh, 2
                add     di, 160 - 2 * 8
                loop    Loop_C101
endif
viu_end:
IF _MSP EQ TRUE
                ; --- размеры ОЗУ и ПЗУ
                mov     cx, 1
                mov     bl, Memory_Size         ;\
                mov     di, _pos_Memory_size    ;|  выведем размер памяти
                mov     ah, NORMA               ;|
                call    printH                  ;/

                mov     bl, Flash_Size          ;\
                mov     di, _pos_Flash_size     ;|  выведем размер флеша
                mov     ah, NORMA               ;|
                call    printH                  ;/
ENDIF

                ; --- исходящий чёрный список
                test    FG_Program, _fg_prg_bl_out
                jz      @@no_indic_bl

                push    ebx
                push    cx
                mov     bx, BL_Count_Out        ;\
                mov     di, _pos_BL_count_Out   ;|
                mov     ah, _color_normal_f     ;| вывод количество срабатываний ЧС
                mov     cx, 4                   ;|
                call    PrintH                  ;/
                mov     ebx, BL_CurrNum_Out     ;\
                mov     di, _pos_BL_number_Out  ;| вывод последнего обработанного номера
                mov     cx, 7                   ;| на экран
                call    PrintH                  ;/
                pop     cx
                pop     ebx
@@no_indic_bl:

                ; --- входящий чёрный список
                test    FG_Program, _fg_prg_bl_in
                jz      @@no_indic_bl_in
                push    ebx
                push    cx
                mov     bx, BL_Count_In         ;\
                mov     di, _pos_BL_count_In    ;|
                mov     ah, _color_normal_f     ;| вывод количество срабатываний ЧС
                mov     cx, 4                   ;|
                call    PrintH                  ;/
                mov     ebx, BL_CurrNum_In      ;\
                mov     di, _pos_BL_number_In   ;| вывод последнего обработанного номера
                mov     cx, 7                   ;| на экран
                call    PrintH                  ;/
                pop     cx
                pop     ebx
@@no_indic_bl_in:
IF _MSP NE TRUE
                ; вывод стороны Х25
                mov     ah, _color_type
                mov     si, _USER
                shl     si, 3
                add     si, OFFSET TYPE_DCE
                mov     di, _pos_DCE
                call    printS
ENDIF
                ; тип блока
IF _MSP EQ TRUE
                mov     si, offset TYPE_UNDEF   ;\
                cmp     fg_prog, _TYPE_BUN      ;|
                ja      @@print_type            ;|   вывод типа блока
                movzx   si, fg_prog             ;|
                shl     si, 4                   ;|
                add     si, OFFSET TYPE_OLD     ;|
@@print_type:                                   ;|
ELSE
                mov     si, offset TYPE_BUN    ;\
ENDIF
                mov     di, _pos_TypeBlock      ;|
                mov     ah, _color_type         ;|
                call    printS                  ;/

                ; параметры компиляции
                mov     ah, _color_type
                mov     cx, 12
        if _SORM_NO_MODEM eq TRUE
                mov     di, _pos_NoModem
                call    printA
        endif
                mov     di, _pos_IsModem
                call    printA
                mov     si, _USER
                shl     si, 3
                add     si, OFFSET TYPE_DCE
                mov     di, _pos_DCE
                call    printS

                ; версия железа
                mov     ah, _color_type
                mov     bl, HardVers_hi
                mov     cx, 2
                mov     di, _pos_HardVers_hi
                call    printH
                mov     bl, HardVers_lo
                mov     di, _pos_HardVers_lo
                call    printH

                ; тип конфигурации и тип СОРМ
                mov     ah, _color_type
                mov     di, _pos_TypePrefix
                lea     si, _str_monoPref
                test    FG_PROGRAM, _fg_prg_multy_pref
                jz      @@draw_pref
                lea     si, _str_multyPref
@@draw_pref:    call    printS
                mov     cx, _len_TypeSORM
                mov     di, _pos_TypeSORM
                call    printA

                ; вывод номера пакета
                mov     ah, _color_normal_f
                movzx   ebx, MO_NUM
                mov     cx, 3
                mov     di, _pos_MO_Num
                call    printD

                ; вывод нагрузки МО
                movzx   ebx, ACC_PTR
                mov     edx, 256
                mov     cx, 3
                mov     di, _pos_MO_Load
                call    printDavar

                ; тайм-аут МО
                movzx   ebx, OSH_T_O
IF _MSP EQ TRUE
                xor     edx, edx
ELSE
                mov     edx, 1
ENDIF
                mov     cx, 3
                mov     di, _pos_MO_TO
                call    printDavar

                ; нет входного потока
                movzx   ebx, OSH_VX_P
IF _MSP NE TRUE
                mov     edx, 1
ENDIF
                mov     cx, 3
                mov     di, _pos_MO_NVP
                call    printDavar

                ; пропадание готовности
                movzx   ebx, OSH_GOT
IF _MSP NE TRUE
                mov     edx, 1
ENDIF
                mov     cx, 3
                mov     di, _pos_MO_GOT
                call    printDavar

                ; ошибки в пакетах
                movzx   ebx, OSH_PAK
IF _MSP NE TRUE
                mov     edx, 1
ENDIF
                mov     cx, 3
                mov     di, _pos_MO_PAK
                call    printDavar

                ; вывод нагрузки блока
                movzx   ebx, NAGRUS
                mov     edx, 20
                mov     cx, 3
                mov     di, _pos_nag
                call    printDavar

                ; вывод даты
                mov     ah, _color_normal_f
                mov     cx, 2
                mov     di, _pos_year
                mov     bl, GOD_[2]
                call    printH
                add     di, 6
                mov     bl, GOD_[1]
                call    printH
                add     di, 6
                mov     bl, GOD_[0]
                call    printH
                ; вывод времени
                mov     di, _pos_time
                mov     bl, GOD_[5]
                call    printH
                add     di, 6
                mov     bl, GOD_[4]
                call    printH
                add     di, 6
                mov     bl, GOD_[3]
                call    printH

                RET
VIU5_OBSH       ENDP


; MrAlex ******редактирование флагов программы - экран
VIU5_CHNG_PRGFLGS       PROC
                ; Просто выведем весь байт побитно и выведем курсор
                MOV     CX,16                   ; 16 бит
@@CHNG_PRGFLG_S:
                PUSH    CX
                dec     cx
                AND     _CHNG_CUR_BIT, 0FH      ; на всякий случай
                ; выберем цвет
                MOV     AH,NORMA                ; цвет всех
                CMP     _CHNG_CUR_BIT, CX       ; это бит, который мы меняем ?
                JNE     @@CHNG_PRGFLG           ; нет
                MOV     AH, CURR_VIV            ; да - поменяем цвет
@@CHNG_PRGFLG:
                LEA     SI,DUB_N                ; Надпись по умолчанию
                push    ax                      ;
                MOV     aX, _CHNG_TMP_VAL       ; возмем редактируемое слово
                shr     aX, cl                  ;
                and     aX, 1                   ; возмем этот бит
                pop     ax
                jz      @@CHNG_PRGFLG1
                LEA     SI,DUB_Y                ; для тех, у кого 1
@@CHNG_PRGFLG1:
                                        ; рассчитаем где будем выводить надпись
                mov     di, SMEV1+2*160+50      ; смещение  надписи целиком
                shl     cx, 2
                add     di, cx                  ; смщение для этого бита
                CALL    printS                  ; выведем
                POP     CX
                LOOP    @@CHNG_PRGFLG_S
                ret
VIU5_CHNG_PRGFLGS       ENDP
; ******MrAlex


; ----=> Refresh_WinTarNet <=-----------------------
; процедура перерисовки окна редактирования сети тысячнико
; Created by Smaller on 10/21/04 9:26
Refresh_WinTarNet       PROC
                MOV     SI,LINIY
                CMP     SI,KL_TIS
                JB      @@NORM_TIS_ROAD_VIV
                MOV     SI,KL_TIS-1
@@NORM_TIS_ROAD_VIV:
                ; вывод номера текущего ЦК
                mov     cx, 2
                mov     bx, si                  ;\
                mov     ah, _color_TarNet_Ck    ;|
                mov     di, _pos_TarNet_CK      ;| № текущего тысячника
                call    printH                  ;/
                ; вывод таблицы маршрутизации
                SHL     SI,4
                MOV     DI,_pos_TarNet_Mod
;;;сохраняем где-то di                CALL    P50IN2
                MOV     CX,16
                MOV     ES,SS:SEG_FS
@@CN_VIV_ROAD_S:
                PUSH    CX

                MOV     AX, PEREM_VIV1
                SHR     AX, 1
                MOV     DX, SI
                AND     DX, 0FH
                CMP     AX, DX
                MOV     CH, _color_TarNet_Mod           ; обычный вид
                JNE     @@END_VIV_ROAD
                MOV     CH, _color_TarNet_CurMod        ; специальный вид
@@END_VIV_ROAD:

                mov     ah, _color_TarNet_Mod
                mov     bl, BYTE PTR TIS_ROAD[si]
                inc     si
                push    cx
                mov     cx, 2
                call    printH
                pop     cx
                add     di, 4
                PUSH    DI
                SUB     DI, 4
                MOV     AX, PEREM_VIV1
                AND     AX, 1
                SHL     AX, 1
                ADD     DI, AX
                INC     DI
                MOV     ES:[DI], CH
                POP     DI
                POP     CX
                ADD     DI, 4
                LOOP    @@CN_VIV_ROAD_S

                ret
Refresh_WinTarNet       ENDP


; --- процедура инициализации --------------------------------------------------
; ----=> EditLenNum <=-----------------------
; Created by Smaller on 10/22/04 11:28
; редактирование длинны номера в станции
Refresh_WinNumLen       PROC
                movzx   bx, Length_Num_Cpy
                mov     ah, _color_TarNet_Ck
                mov     di, _pos_NumLen
                mov     cx, 1
                call    printH
                ret
Refresh_WinNumLen       ENDP


INIC_W_V        PROC    FAR
                CALL    INIC_W_VN
                CALL    INIC_LEV2
                CALL    INIC_SORM
                RETF
INIC_W_V        ENDP

RESTART_SORM    PROC    FAR
                CALL    INIC_LEV2
                CALL    INIC_SORM
                RETF
RESTART_SORM    ENDP

; --- процедура сброса оконных данных в первоначальное состояние ---------------
INIC_W_VN       PROC
		mov	No_Debug_Flag, 0
                cmp     NOM_VERX_OKNA, _MO_Slct_Win
                jne     @@check_Puma_dbg
 @@save_MO:     mov     FG_Curr_MO_Num, 0      ; сбросим флаги окна МО
                mov     num_page_MO, 1         ; |
                mov     pos_Curr_MO_Num, 0     ; |
                mov     Lim_Win_MO, 2348       ; |
                jmp     @@usual_inic
@@check_Puma_dbg:
                cmp     NOM_VERX_OKNA, _Puma_debug
                jne     @@usual_inic
                ; --- Init puma debug parametrs
                mov     Puma_flg, 0
                mov     PUMA_trans_ofs, 0
                mov     puma_win_ofs, 0
                mov     puma_dbg_ofs, 0
                ; ---------------------
 @@usual_inic:  MOV     NOM_VERX_OKNA,0         ;\
                LEA     SI,MENU                 ;|
                XOR     DI,DI                   ;| переинициализируем верхнее окно
                mov     ah,_color_normal_f      ;|
                CALL    printSscr               ;/
                JMP     @@NORM_INIC

Inic_New_down:  MOV     SOST_VIU5,0             ;\
                LEA     SI,DnWin                ;|
                MOV     DI,_pos_PO_OBS - 480    ;|
                mov     ah,_color_normal_f      ;| переинициализируем нижнее окно
                CALL    printSscr               ;|
                JMP     Inic_W_down             ;|
  @@NORM_INIC:  MOV     SOST_VIU5,0             ;|
  Inic_W_down:  LEA     SI,PO_OBS               ;|
                MOV     DI,_pos_PO_OBS          ;|
                mov     ah,_color_normal_f      ;|
                CALL    printSscr               ;/

                mov     si, offset LOG_CLEAR    ;\
                mov     di, _pos_log            ;|
                mov     ah, _color_cl_log       ;| переинициализируем строку LOG
                call    printS                  ;/

                RET
INIC_W_VN       ENDP

; --- Индикация контроля ошибок на новом ИКМ -----------------------------------
;Часть 2 - индикация на ИКМ-е
NEW_IND_IKM     PROC
        MOV     BP,0
        MOV     CX,BP
        SHR     CX,1
        MOV     ES,SS:SEG_GS
        mov     ES:[501BH],byte PTR 00000000B

        PUSH    BP
        SHR     BP,1
        XOR     AL,AL
        TEST    RSR_0[BP],100B
        JZ      @@AUS
        OR      AL,BYTE PTR 00000010B   ; ПЦС
@@AUS:
        TEST    RSR_0[BP],00100000B
        JZ      @@E_3
        OR      AL,BYTE PTR 00010000B   ; АУС
@@E_3:
        TEST    RSR_0[BP],1000B
        JZ      @@E_5
        OR      AL,BYTE PTR 000000100B  ; E-3
@@E_5:
        TEST    RSR_0[BP],10000B
        JZ      @@END_P
        OR      AL,BYTE PTR 00001000B   ; E-5
@@END_P:
        CMP     VERSIY,3H
        JNE     @@NET_SDVIGA
        SHL     AX,1
@@NET_SDVIGA:
        TEST    RSR_0[BP],1             ;Индикация лампочек
        JZ      @@PCS
        OR      AL,BYTE PTR 00000001B   ; НВП
@@PCS:
        mov     ES:[501BH],AL
        MOV     AH,RSR_0[BP]
        POP     BP

        MOV     ES:[501AH],BYTE PTR 060H
        mov     es:[5076h],byte ptr 060h       ;версия 1.3
        mov     es:[5077h],al                  ;версия 1.3
        mov     es:[5078h],byte ptr 0          ;версия 1.3
        mov     es:[5079h],byte ptr 1          ;версия 1.3

        ret
NEW_IND_IKM     ENDP

; --- контроль ТЧ --------------------------------------------------------------
Refresh_TC_Far  PROC
                call    Refresh_TC
                retf
Refresh_TC_Far  ENDP

Refresh_TC      PROC
                cmp     TC_Step, _TC_STEP_MAX
                jae     @@refresh
                inc     TC_Step
                ret
                ; ---
@@refresh:      mov     TC_Step, 0
                mov     ah, _color_TC           ; нейтральный цвет
                ; ---
                mov     cx, _MAX_KSL            ; вывод первых КСЛ
                lea     si, KSLs
                mov     di, _pos_TC_KSL
@@ksl:          push    cx
                mov     cl, 2
                mov     bl, [si]
                call    printH
                add     di, 160
                inc     si
                pop     cx
                loop    @@ksl
                ; ---
                mov     cx, _MAX_KSL            ; вывод признаков занятия КСЛ
                lea     si, KSL_Busy
                mov     di, _pos_TC_Busy
@@busy:         push    cx
                mov     cl, 2
                mov     bl, [si]
                call    printH
                add     di, 160
                inc     si
                pop     cx
                loop    @@busy
                ; ---
                mov     cx, _MAX_MODEM_TS       ; вывод занятия каналов модема
                lea     si, Modem_KSL
                mov     di, _pos_TC_Modem
@@modem:        push    cx
                mov     cl, 2
                mov     bl, [si]
                call    printH
                add     di, 160
                inc     si
                pop     cx
                loop    @@modem
                ; === БАБА
                cmp     TC_Count, _TC_COM_IN_MAX + _TC_COM_OUT_MAX
                jb      @@is_com_in
                ; --- вывод очередного байта из "БАБЫ"
@@baba:         mov     gs, ss:SEG_BABA
                mov     ax, TC_Count
                sub     ax, _TC_COM_IN_MAX + _TC_COM_OUT_MAX
                shr     ax, 4
                push    ax
                xor     dx, dx
                div     WORD PTR __VAL_3
                mov     si, ax
                shl     si, 9                   ; вывод ТЧ абонента А
                or      dx, dx
                jz      @@baba_si
                add     si, 512 * 16            ; вывод ТЧ абонента Б
                cmp     dx, 1
                je      @@baba_si
                add     si, 512 * 16            ; вывод сумированных каналов
@@baba_si:      pop     ax                      ; здесь si указатель на читаемый байт БАБы
                mov     di, _pos_TC_BABA
                mul     WORD PTR __VAL_160
                add     di, ax                  ; прибавили строку
                mov     ax, TC_Count
                and     ax, 0fh
                add     si, ax
                shl     ax, 2
                add     di, ax                  ; прибавили смещение в строке
                jmp     @@read

@@is_com_in:    cmp     TC_Count, _TC_COM_OUT_MAX
                jb      @@com_out
                ; --- вывод очередного входной коммутации из ОЗУ коммутации
                ;     эта коммутация стоит не меняясь с момента инициализации
@@com_in:       mov     gs, ss:SEG_ST           ; C800h
                mov     si, ADR_OZU_K1          ; 3000h
                add     si, _MODEM_1ST_TS * 4   ; c 60-го шага коммутации
                mov     ax, TC_Count
                sub     ax, _TC_COM_OUT_MAX
                add     si, ax
                mov     di, _pos_TC_ComIn
                shr     ax, 2
                mul     WORD PTR __VAL_160
                add     di, ax                  ; прибавили строку
                mov     ax, TC_Count
                and     ax, 03h
                shl     ax, 2
                add     di, ax                  ; прибавили смещение в строке
                jmp     @@read

                ; --- вывод очередной выходной коммутации из ОЗУ коммутации
                ;     соединяет КСЛ (заданный ТС на ИКМ) с выбираемым каналом БАБЫ
@@com_out:      mov     gs, ss:SEG_ST           ; C800h
                mov     si, ADR_OZU_K1          ; 3000h
                add     si, ADD_4_KAN           ; 16
                mov     ax, TC_Count
                add     si, ax
                mov     di, _pos_TC_ComOut
                shr     ax, 2
                mul     WORD PTR __VAL_160
                add     di, ax                  ; прибавили строку
                mov     ax, TC_Count
                and     ax, 03h
                shl     ax, 2
                add     di, ax                  ; прибавили смещение в строке

@@read:         ; --- чтение байта из аппаратуры
                mov     ah, _color_TC
                mov     bl, gs:[si]
@@print:        ; --- собственно печать
                mov     cx, 2
                call    printH
                ; --- счётчик байт, прочитанных из аппаратуры
                inc     TC_Count
                cmp     TC_Count, _TC_COUNT_MAX
                jb      @@no_begin
                mov     TC_Count, 0
@@no_begin:
                ; --- подсветка проключенных каналов ТЧ ---------------------------------
                xor     bx, bx
@@find_tc:      cmp     Modem_KSL[bx], 0ffh
                je      @@next_tc
                ; --- есть что подсвечивать
                ; аттрибут для подсветки
                mov     ah, bl
                add     ah, 2
                shl     ah, 4
                add     ah, cBLACK;;;clWHITE
                ; --- собственно подсветка
                ; сначала № модема по № ТЧ
                mov     di, _pos_TC_Modem
                push    ax
                mov     ax, bx
                mul     WORD PTR __VAL_160      ; 160 байт в строке экрана
                add     di, ax
                pop     ax
                mov     cx, 2
                call    printA
                ; теперь соответствующую входную коммутацию по № ТЧ
                mov     di, _pos_TC_ComIn
                push    ax
                mov     ax, bx
                shl     ax, 1
                mul     WORD PTR __VAL_160      ; 160 байт в строке экрана
                add     di, ax
                pop     ax
                mov     cx, 8
                call    printA
                add     di, 160
                call    printA
                ; теперь выходную коммутацию по № КСЛ
                cmp     Modem_KSL[bx], _MAX_MODEM_TS
                jae     @@next_tc
                mov     di, _pos_TC_ComOut
                push    ax
                movzx   ax, Modem_KSL[bx]
                dec     ax
                shl     ax, 1
                mul     WORD PTR __VAL_160      ; 160 байт в строке экрана
                add     di, ax
                pop     ax
                mov     cx, 8
                call    printA
                add     di, 160
                call    printA
                ; подсветка 3-х каналов БАБЫ - А, Б и А + Б
                mov     di, _pos_TC_Baba
                push    ax
                movzx   ax, Modem_KSL[bx]
                dec     ax
                mul     WORD PTR __VAL_3        ; три канала БАБы
                mul     WORD PTR __VAL_160      ; 160 байт в строке экрана
                add     di, ax
                pop     ax
                cmp     di, 30 * 160
                jae     @@next_tc
                mov     cx, 32
                call    printA
                add     di, 160
                call    printA
                add     di, 160
                call    printA
                ; ---
@@next_tc:      inc     bx
                cmp     bx, _MAX_MODEM_TS
                jb      @@find_tc
                ; ---
                ret
Refresh_TC      ENDP


; --- контроль ОЗУ коммутации, обновляем окно ----------------------------------
Refresh_OZUK_Far        PROC
                call    Refresh_OZUK
                retf
Refresh_OZUK_Far        ENDP

Refresh_OZUK    PROC
                cmp     OZUK_Step, _OZUK_STEP_MAX
                jae     @@refresh
                inc     OZUK_Step
                ret
                ; ---
@@refresh:      mov     OZUK_Step, 0
                cmp     count_OZUK, 128 * 4
                jbe     @@OZUK1
                cmp     count_OZUK, _COUNT_OZUK_MAX
                jbe     @@OZUK3
                mov     count_OZUK, 0
                ret
                ; ---
@@OZUK1:        mov     es, ss:SEG_ST
                mov     si, count_OZUK
                add     si, ADR_OZU_K1
                ;
                mov     ax, count_OZUK
                shr     ax, 2
                and     ax, 01fh
                mul     WORD PTR __VAL_160
                add     ax, 160
                mov     di, count_OZUK
                shr     di, 7
                shl     di, 1
                add     ax, _x_OZUK1[di]
                mov     di, count_OZUK
                and     di, 3
                shl     di, 2
                add     ax, di
                mov     di, ax
                ;
                mov     ah, cBLUE * 16 + clWHITE
                mov     bl, es:[si]
                mov     cx, 2
                ;
                call    printH
                inc     count_OZUK
                ret
                ; ---
@@OZUK3:        mov     es, ss:SEG_ST
                mov     si, count_OZUK
                sub     si, 128 * 4
                add     si, ADR_OZU_K3
                ;
                mov     ax, count_OZUK
                sub     ax, 128 * 4
                shr     ax, 1
                and     ax, 01fh
                mul     WORD PTR __VAL_160
                add     ax, 160
                mov     di, count_OZUK
                sub     di, 128 * 4
                shr     di, 6
                shl     di, 1
                add     ax, _x_OZUK3[di]
                mov     di, count_OZUK
                sub     di, 128 * 4
                and     di, 1
                shl     di, 2
                add     ax, di
                mov     di, ax
                ;
                mov     ah, cBLUE * 16 + clWHITE
                mov     bl, es:[si]
                mov     cx, 2
                ;
                call    printH
                inc     count_OZUK
                ret
Refresh_OZUK    ENDP


; --- контроль ошибочных пакетов -----------------------------------------------
;
Refresh_bad_MO  PROC
                mov     ah, _color_bad_MO
                xor     bx, bx
                mov     di, _pos_bad_MO
                add     di, 160 + 2
                lea     si, bad_MO_buff
@@loop:         cmp     bl, bad_MO_count
                jae     @@exit
                inc     bx
                ;
                push    bx
                push    di
                push    si
                mov     PEREM_DI, di
                mov     PEREM_SI, si
                ; маркер последнего записанного пакета
                mov     al, char_Space
                cmp     bl, bad_MO_wr_ptr
                jne     @@not_curr_str
                mov     al, char_GT
@@not_curr_str: mov     cx, 1
                call    printC
                ; часы
                mov     bl, [si + _BAD_MO_TIME]
                mov     cx, 2
                add     di, 2
                call    printH
                ; минуты
                mov     bl, [si + _BAD_MO_TIME + 1]
                add     di, 6
                call    printH
                ; секунды
                mov     bl, [si + _BAD_MO_TIME + 2]
                add     di, 6
                call    printH
                ; вычисляем сколько байт реально выводить
                movzx   cx, [si];;;;;[si + _BAD_MO_PACKET]
                cmp     cx, _BAD_MO_SCR_SIZE
                jbe     @@bad_len
                mov     cx, _BAD_MO_SCR_SIZE
@@bad_len:      mov     PEREM_CX, cx
                ; выводим сам пакет
                add     si, _BAD_MO_PACKET
@@bad_Pack:     push    cx
                mov     bl, [si]
                mov     cx, 2
                add     di, 6
                call    printH
                pop     cx
                inc     si
                loop    @@bad_Pack
                ; если пакет короче _BAD_MO_SCR_SIZE - вычисляем начало завершающей строки
                mov     cx, PEREM_CX
                cmp     cx, _BAD_MO_SCR_SIZE
                jae     @@bad_ptr
                mov     si, PEREM_CX
                add     si, PEREM_CX
                add     si, PEREM_CX
                add     si, OFFSET bad_MO_str
                call    printS
                ; вычисляем и подсвечиваем начало ошибочной команды
@@bad_ptr:
        if 0
                mov     si, PEREM_SI
                movzx   dx, [si]
                cmp     dx, _BAD_MO_SCR_SIZE
                ja      @@cannt_bad_ptr
                mov     di, PEREM_DI
                add     di, _BAD_MO_SCR_BEG - 2
                shl     dx, 1
                add     di, dx
                add     di, dx
                add     di, dx
        endif
                mov     di, PEREM_DI
                add     di, _BAD_MO_SCR_BEG - 2
                push    ax
                mov     ah, _color_bad_MO_Ptr
                mov     cx, 2
                call    printA
                pop     ax
@@cannt_bad_ptr:
                ;
                pop     si
                pop     di
                pop     bx
                ;
                add     di, 160
                add     si, _BAD_MO_SIZE
                jmp     @@loop
@@exit:         ret
Refresh_bad_MO  ENDP

; --- процедура поиска номера команды, помеченной звездочкой для окна выбора команд МО ---
; --- In: bl - номер команды
; --- Out: _find_numb - команда помечена, уберем её из буфера, изменим значение флага;
; ---      not _find_numb - команда не помечена, будем помечать;
Find_Numb_MO_win   PROC
                cld
                ;;;clc
                and     FG_Curr_MO_Num, not _find_numb
        push    ax bx cx si di
                mov     si, offset MO_Numb_235
                lea     di, curr_MO_ptr_235
                test    FG_Curr_MO_Num, _235_cmd                ; выбрана 235-ая ?
                jnz     @@yes_235
                mov     si, offset MO_Numb
                lea     di, curr_MO_ptr
@@yes_235:      mov     cx, 255
@@go_find:      lodsb
                cmp     al, bl
                je      @@find
                loop    @@go_find
                jmp     @@exit
@@find:         dec     si
                mov     BYTE PTR ds:[si], 00h            ; освободим место в буфере

        ; --- изменим значение текущего ук-ля, если нужно ---
                cmp     si, [di]
                jae     @@save_ptr
                mov     [di], si                         ; изменим значение текущего ук-ля
  @@save_ptr:   or      FG_Curr_MO_Num, _find_numb
@@exit:
        pop     di si cx bx ax
                ret
Find_Numb_MO_win   ENDP
; -----------                    ----------
; --- процедура записи очередной ком-ды МО в буфер помеченных команд ---
; --- In: -  bx команда
;         -  curr_MO_ptr - ук-ль на на 1-ое свободное место в буфере помеченных команд
; --- Out: stc - есть место в буфере для записи команды
; ---      clc - буфер заполнен(такого быть не должно)
; ---      сurr_MO_ptr - ук-ль на тек. свободное место для записи кода команды

Put_Numb_MO     PROC
                cld
                clc
        push    cx dx si di
                mov     si, offset MO_Numb_235
                lea     di, curr_MO_ptr_235
                test    FG_Curr_MO_Num, _235_cmd                ; выбрана 235-ая ?
                jnz     @@yes_235
                mov     si, offset MO_Numb
                lea     di, curr_MO_ptr
@@yes_235:      cmp     WORD PTR [di], 0
                jne     @@next_MO
                mov     ds:[si], bl              ; сохраним код команды в буфере
                inc     si
                mov     [di], si                 ; сохраним ук-ль на место, куда будем записывать очередную выделенную ком-ду
                jmp     @@exit
@@next_MO:      mov     dx, si                   ; сохраним текущее значение ук-ля на буфер
                mov     si, [di]
                cmp     si, 0FFFFh
                je      @@bad_exit
                mov     ds:[si], bl              ; сохраним код команды в буфере
                mov     si, dx                   ; восстановим тек. значение ук-ля на буфер
                ; --- найдем место для ук-ля на свободное место в буфере помеченных команд МО ---
                mov     cx, 255
@@find_ptr:     cmp     BYTE PTR ds:[si], 00h
                je      @@go_ptr
                inc     si
                loop    @@find_ptr
                mov     WORD PTR [di], 0FFFFh
                jmp     @@bad_exit
@@go_ptr:       mov     [di], si                                ; сохраним значение тек. указателя
@@exit:         stc
@@bad_exit:
        pop     di si dx cx
                ret
Put_Numb_MO     ENDP
; --- процедура поиска номера команды МО для вывода на экран ---
; --- In: bl - номер команды
; --- Out: _find_cmd - команда уже помечена
; ---      not _find_cmd - команда не помечена
Find_Numb_MO    PROC   FAR
                cld
                and      FG_Curr_MO_Num, not _find_cmd
        push    ax cx
                mov     cx, 255
@@go_find:      lodsb
                cmp     al, bl
                je      @@find
                loop    @@go_find
                jmp     @@exit
@@find:         ;
                or    FG_Curr_MO_Num, _find_cmd                ; нашли помеченную команду
@@exit:
        pop     cx ax
                ret
Find_Numb_MO    ENDP

; --- Прорисовка выделенных команд ---
Stars_MO_Win  PROC
; ---  при каждом выводе окна нужно показать выделенные команды ---
; --- и звездочки                                       -----------
       push     ax bx cx dx si di
                mov     di, frst_dig_MO;;;_pos_Tel_Num_Win + 2*160 + 2        ; начало окна для кодов команд
                push    di
                ; --- начнем поиск    ----
                mov     si, offset MO_Numb_235
                test    FG_Curr_MO_Num, _235_cmd                ; выбрана 235-ая ?
                jnz     @@yes_235                               ; Да
                mov     si, offset MO_Numb                      ; здесь все выделенные команды
@@yes_235:
@@find:         mov     cx, 255
                mov     bl, 15                                  ; кол-во строк в окне под коды команд
                xor     ax, ax
@@find_numb:    lodsb
                cmp     al, 00h
                jne     @@go_numb
                loop    @@find_numb
                jmp     @@no_numb
@@go_numb:
                cmp     num_page_MO, 1
                je      @@frst_pge
                cmp     al, 150
                ja      @@scnd_pge
                loop    @@find_numb
                jmp     @@no_numb
@@scnd_pge:     sub     al, 150
                jmp     @@ok_numb
@@frst_pge:     cmp     al, 150
                jbe     @@ok_numb
                loop    @@find_numb
                jmp     @@no_numb
@@ok_numb:      div     bl
                cmp     ah, 0
                jne     @@not_0
                dec     al
                mov     ah, 14
                jmp     @@yes_0
@@not_0:        dec     ah
@@yes_0:        mov     bx, ax                                    ; №строки и №столбца в bx
                xor     ah, ah                                    ; перемножим № столбца(al) на
                mul     clmn_digs_draw                            ; ширину столбца
                add     di, ax                                    ; |
                mov     al, bh                                    ;  перемножим № строки(bh) на
                xor     ah, ah                                    ;
                mul     line_digs_draw                             ;  кол-во символов в строке
                add     ax, 8                                     ; |
                add     di, ax                                    ; |
                push    si                                        ; |
                mov     si, offset Star_Mo_Win                    ; |
                mov     ah, _color_normal_MO
                call    PrintS                                    ; и поставим "звезду"
                pop     si
                dec     cx                                        ; |
                jcxz    @@no_numb                                 ; |
                mov     bl, 15                                    ; продолжаем поиск
                mov     di,  frst_dig_MO;;;_pos_Tel_Num_Win + 2*160 + 2         ; |
                xor     ax, ax                                    ; |
                jmp     @@find_numb                               ; |
@@no_numb:      pop     di
                ; --- выделим текущую команду ---
                mov     cx, 5
                mov     ah, _color_MO_slct
        push    cx ax
                mov     cx, 15
                mov     ax, pos_Curr_MO_Num
                cmp     ax, 2240
                jae     @@def_lim
   @@curr_mo:   add     ax, 160
                cmp     ax, 2240
                jae     @@def_lim
                loop    @@curr_mo
   @@def_lim:   cmp     ax, Lim_Win_MO
                jbe     @@ok_pos
                sub     ax, Lim_Win_MO
                sub     pos_Curr_MO_Num, ax
   @@ok_pos:    pop     ax cx
                add     di, pos_Curr_MO_Num
                call    PrintA
                ; ---        ---
        pop     di si dx cx bx ax
                ret
; -------                                                   ----
Stars_MO_Win  ENDP

; --- процедура поиска номера команды МО по тек.позиции(смещение экрана)  ---
; --- In:  dx - текущее смещение на экране(pos_Curr_MO_Num)
; --- Out: bx - № команды МО
; --- портит BX!!!
Find_MO_number  PROC
                clc
                push    ax dx cx
                mov     ax, dx
                xor     dx, dx
                mov     cx, 160
                div     cx                               ; в ax - строка
                mov     bx, ax
                inc     bx
                or      dx, dx
                je      @@frst_column
                mov     ax, dx                           ; делим остаток - узнаем столбец
                xor     dx, dx
                mov     cx, 12
                div     cx                               ; в ax - столбец
                or      dx, dx
                jnz     @@exit_numb
                xor     ah, ah
                mov     cl, 15
                mul     cl                               ; на кол-во строк в столбце
                jmp     @@numb
@@frst_column:  mov     ax, 0
@@numb:         cmp     num_page_MO, 1
                je      @@first_pge
                add     ax, 150
@@first_pge:    add     bx, ax                           ; в bx номер команды(поместится в bl)
                cmp     bx, 0FFh
                ja      @@bad_numb
                jmp     @@exit_numb
@@bad_numb:     stc
@@exit_numb:    pop     cx dx ax                         ; восстановим атрибут
                ret
Find_MO_number  ENDP
; ---------------------------
; ---// Puma debugger proc // ------
Refresh_Debug_conf_Win  PROC
                push    bp
                mov     bp, 0
@@_refr:        shl     bp, 1                          ; pointer for paket
                ; ------ CHECK debug -------
                ;cmp     WORD PTR Pak_800E[bp], 0
                ;jz      @@not_refr
                ; --- Debug is working !
                push    ax ebx cx si di
                xor     ebx, ebx
                xor     di, di
                cmp     bp, 0
                jz      @@frst_string
                mov     bx, bp
                mov     al, 80
                mul     bl
                mov     di, ax
 @@frst_string: mov     ah, _color_puma_v
                mov     cl, 4
                ; -------- Redrawing ------------
                ; = 80.0E
                movzx   ebx, WORD PTR Pak_800E[bp]
                add     di, _pos_Win_Cfg + 17*160 + 3*160 + 8
                call    printH
                ; = 80.8E
                movzx   ebx, WORD PTR Pak_808E[bp]
                add     di, 40
                call    printH
                shl     bp, 1                   ; pointer for timer
                ; ---- timer - 80.0E
                mov     cl, 8
                mov     si, offset Puma_counts_val
                mov     ebx, tmr_800E[bp]
                sub     di, 24
                cmp     ebx, 0
                jnz     @@pr_time
                call    printS
                jmp     @@tmr_808E
   @@pr_time:   call    printH
                ; ---- timer - 80.8E
   @@tmr_808E:  mov     ebx, tmr_808E[bp]
                add     di, 42
                cmp     ebx, 0
                jnz     @@pr_time_1
                call    printS
                jmp     @@pr_result
   @@pr_time_1: call    printH
                ; ------------------
                ; -- print result
   @@pr_result: shr     bp, 1                   ; pointer for paket
                mov     ah, _color_avar
                mov     si, offset res_dbg_Cfg
                add     di, 24
                mov     bx, Pak_800E[bp]
                cmp     Pak_808E[bp], bx
                jne     @@print_res             ; NOK - result
                mov     ah, _color_puma_n
                add     si, 4
                cmp     bx, 0
                jnz     @@print_res             ; OK- result
                ; -- clear result
                mov     ah, _color_unknow
                add     si, 4
 @@print_res:   call    printS
                pop     di si cx ebx ax
    @@not_refr: shr     bp, 1                   ; start pointer
                inc     bp
                cmp     bp, 4
                jbe     @@_refr
                pop     bp
                RET
Refresh_Debug_conf_Win  ENDP

; ---------------- ---------------------
