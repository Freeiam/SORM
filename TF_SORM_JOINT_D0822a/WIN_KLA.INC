include lib/screen.prc

; hex   key
;   0 - 0
;   1 - 1
;   2 - 2
;   3 - 3
;   4 - 4
;   5 - 5
;   6 - 6
;   7 - 7
;   8 - 8
;   9 - 9
;   A - Back Space
;     -
;  11 - 
;  12 - 
;  13 - ENTER
;  14 - 
;  15 - 
;  16 - F1
;  17 - F2
;  18 - F3
;  19 - F4
;  1A - F5
;  1B - F6
;  1C - F7
;  1D - F8
;  1E - F9
;  1F - F10
;  20 - F11
;  21 - F12
;  22 -
;  23 - TAB
;  24 - ESCape
;     -
;  41 - A
;  42 - B
;  43 - C
;  44 - D
;  45 - E
;  46 - F
;  47 - G
;  48 - H
;  49 - I
;  4A - J
;  4B - K
;  4C - L
;  4D - M
;  4E - N
;  4F - O
;  50 - P
;  51 - Q
;  52 - R
;  53 - S
;  54 - T
;  55 - U
;  56 - V
;  57 - W
;  58 - X
;  59 - Y
;  5A - Z
;
;  F9 - END
;  FA - HOME
;  FB - Page Down
;  FC - Page Up
;  FD - Alt OFF
;  FE - Alt ON


KLA_WIN         PROC
                @KLAIN2
                CMP     AH,0FFH         ; не правильная клавиша
                JE      END_KW          ;

                cmp     ah, 21h         ; [ F12 ] - выход
                je      OBSHIY_WIN
                CMP     AH,19H
                JNE     NET_ZAVOROTA

                XOR     FG_SORM3,M_OR_1
                TEST    FG_SORM3,M_OR_1
                JZ      SHORT NET_ZAVOROTA2
                ; поставим заворот
SET_ZAVOROT:
                MOV     ES,SS:SEG_GS
                MOV     AL,1
                mov     cl, _nom_ikm
                shr     cl, 5
                shl     al, cl
                MOV     ES:[5036H],AL           ; поставим ближний заворот на нужном ИКМ
                mov     es:[5076h], al          ; поставим ближний заворот на нужном ИКМ
                mov     BYTE PTR es:[5077h], 0ffh
                mov     BYTE PTR es:[5078h], 10h
                mov     BYTE PTR es:[5079h], 1

if _SORM_NO_MODEM EQ FALSE
        IF _MSP EQ TRUE
        ELSE
                ; ставим коммутацию в ИКМ из АК
                mov     es, ss:SEG_ST
                mov     di, ADR_OZU_K1 + 256 + 30 * 2
                mov     BYTE PTR es:[di],     1
                mov     BYTE PTR es:[di + 1], 6
                mov     BYTE PTR es:[di + 2], 0
                mov     BYTE PTR es:[di + 3], 6
        ENDIF
endif

                JMP     SHORT NET_ZAVOROTA
NET_ZAVOROTA2:
                ; уберем заворот
                MOV     ES,SS:SEG_GS
                xor     al, al
                MOV     ES:[5036H],AL           ; уберем ближний заворот на всех ИКМ
                mov     es:[5076h], al          ; уберем ближний заворот на всех ИКМ
                mov     BYTE PTR es:[5077h], 0ffh
                mov     BYTE PTR es:[5078h], 10h
                mov     BYTE PTR es:[5079h], 1

if _SORM_NO_MODEM EQ FALSE
        IF _MSP  EQ TRUE
        ELSE
                ; ставим коммутацию в ИКМ из АК
                mov     es, ss:SEG_ST
                mov     di, ADR_OZU_K1 + 256 + 30 * 2
                mov     BYTE PTR es:[di],     0
                mov     BYTE PTR es:[di + 1], 6
                mov     BYTE PTR es:[di + 2], 1
                mov     BYTE PTR es:[di + 3], 6
        ENDIF
endif
net_zavorota:

                cmp     NOM_VERX_OKNA, 0        ;\
                jne     @@no_global_help        ;| если открыто какое, либо окно, то
                cmp     SOST_VIU5, 0            ;| запускать глобальный HELP не имеет
                jne     @@no_global_help        ;/ смысла
                cmp     ah, 16h                 ; [ F1 ] - справка
                je      View_Help               ; посмотрим справку
@@no_global_help:

                CMP     AH,254                  ; ALT on
                JE      ALT_ON_OB
                CMP     AH,253                  ; ALT off
                JE      ALT_OFF_OB

                PUSH    AX
                PUSH    BX
                CALL    KLA_VERX_OKNA           ; обработаем верхнее окно
                POP     BX
                POP     AX
                jc      END_KW
                CALL    KLA_NIGNEGO_OKNA        ; обработаем нижнее окно
END_KW:
                RET
KLA_WIN         ENDP

OBSHIY_WIN:     CALL    INIC_W_VN
                mov 	__inf_y, 0;;;!!!
                RET


ALT_ON_OB:      OR      FG_KLA, _b_00
                RET
ALT_OFF_OB:     and     FG_KLA, NOT _b_00
                RET
; ---


ADR_KLA_VERX    LABEL   WORD
                DW      OFFSET  GLOB_KLA
                DW      OFFSET  Cfg_Win_kla     ;1
                DW      OFFSET  Cfg_CK_Win_kla  ;2
                DW      OFFSET  Cfg_Ln_Win_kla  ;3
                DW      OFFSET  Tar_Net_Win_kla ;4
                DW      OFFSET  Tel_Num_Win_kla ;5
                DW      OFFSET  U_Num_Win_kla   ;6
                DW      OFFSET  MO_Select_Win_kla        ;7
                DW      OFFSET  BL_Win_Kla      ;8
                DW      OFFSET  Puma_cnts_kla   ;9
                DW      OFFSET  GLOB_KLA        ;10
                DW      OFFSET  GLOB_KLA        ;11

_CFG_WIN	= 1
_Cfg_CK_Win	= 2
_Cfg_Ln_Win	= 3
_Tar_Net_Win    = 4
_Tel_Num_Win    = 5
_U_Num_Win      = 6
_MO_Slct_Win    = 7
_MO_BL_DBG_Win  = 8
_Puma_debug     = 9

KLA_VERX_OKNA:
                MOV     DI,NOM_VERX_OKNA
                SHL     DI,1
                JMP     ADR_KLA_VERX[DI]


; --- обработка клавиш общего (основного) окна (центр)
GLOB_KLA        PROC
                ; ALT +
                test    FG_KLA, _b_00
                jz      @@NO_ALT
		mov	No_Debug_Flag, 0ffh
                ; --- просмотр процесса сбора конфигурации
                cmp     ah, 43H         ; [ C ]
                je      Create_Cfg_Win
                ; --- просмотр списка телефонных номеров
                cmp     ah, 4EH         ; [ N ]
                je      Create_Tel_Num_Win
                ; --- просмотр тарификационной сети
                cmp     ah, 54H         ; [ T ]
                je      Create_Tar_Net_Win
                ; --- просмотр списка телефонных номеров
                cmp     ah, 55H         ; [ U ]
                je      Create_U_Num_Win
                ; --- просмотр окна выбора команд МО
                cmp     ah, 41h         ; [ A ]
                je      Create_MO_Select_Win
                ; --- просмотр окна отладчика BlackList
                cmp     ah, 57h         ; [ W ]
                je      Create_BL_Debug_Win
                ; --- счетчики ошибок ПУМы
                cmp     ah, 56h         ; [ V ]
                je      Create_PUMA_Debug

		mov	No_Debug_Flag, 0
@@NO_ALT:
		clc
                ret
GLOB_KLA        ENDP

; === обработка клавиш верхних окон =========================================
;
; --- просмотр процесса сбора конфигурации
;
Cfg_Win_kla	PROC
                test    FG_KLA, _b_00
                jnz     @@Alt
                cmp     ah, 13H         ; [ ENTER ]
                je      @@Enter
                cmp     ah, 14H         ; [  ]
                je      @@Up
                cmp     ah, 15H         ; [  ]
                je      @@Dn
@@exit:         clc
		ret
		; ---
@@Alt:          cmp     ah, 13H         ; [ ALT + ENTER ]
                jne     @@exit
@@Alt_Enter:    cmp     Cfg_Run, 0
		jne	@@no
                cmp     Cfg_CK_Count, 0
                je      @@no
@@yes:          ; --- определяем N ЦК
		mov	es, ss:SEG_AXD
		xor	al, al
		xor	ebx, ebx
@@find_loop:	cmp 	es:TIS_TIS[bx], 16
		jae	@@find_next_2
		cmp	al, pos_Curr_CK
		jb	@@find_next
		ja	@@quit
                mov     Cfg_from_One_CK,    bl
		mov	Cfg_Curr_CK, bl
                mov     Cfg_Check_Level,    0
                mov     Cfg_CK_Flags[bx],   0
                mov     Cfg_CK_Errs[bx],    0
		shl	bx, 1
                mov     Cfg_CK_Lines[bx],   0
                mov     Cfg_CK_Mod_OK[bx],  0
		mov	Cfg_CK_Mod_Err[bx], 0
                mov     Cfg_Run,            255
@@quit:		stc
		ret
@@find_next:	inc	al
@@find_next_2:	inc	bx
		cmp	bl, KL_TIS
		jb	@@find_loop
		stc
		ret
@@no:           lea	si, _str_no_One_CK
		call	print_Log_Err
		stc
		ret
		; ---
@@Enter:  	call	Create_Cfg_CK_Win
		stc
		ret
		; ---
@@Up:           mov	al, pos_Curr_CK
		mov	pos_Prev_CK, al
		dec	al
		jns	@@Save_New_Pos
		mov	al, pos_Max
		dec	al
		jmp	@@Save_New_Pos
		; ---
@@Dn:           mov	al, pos_Curr_CK
		mov	pos_Prev_CK, al
		inc	al
		cmp	al, pos_Max
		jb	@@Save_New_Pos
		xor	al, al
		; ---
@@Save_New_Pos: mov	pos_Curr_CK, al
		stc
		ret
Cfg_Win_kla	ENDP

; --- просмотр конфигурации ЦК
;
Cfg_CK_Win_kla	PROC
                cmp     ah, 13H         ; [ ENTER ]
                je      @@Enter
                cmp     ah, 14H         ; [  ]
                je      @@Up
                cmp     ah, 15H         ; [  ]
                je      @@Dn
                cmp     ah, 58H         ; [ X ]
                je      @@X
		clc
		ret
		; ---
@@Enter:  	movzx	ebx, Cfg_CK_Num
		shl	ebx, 4
		or	bl, pos_Curr_Blk
		shl	ebx, 3
		mov	es, ss:SEG_KONF
		mov	al, es:[ebx]
		cmp	al, 4
		je	@@exit
		cmp	al, 9
		ja	@@exit
		mov	Cfg_Blk_Type, al
		call	Create_Cfg_Ln_Win
		stc
		ret
		; ---
@@Up:           mov	al, pos_Curr_Blk
		mov	pos_Prev_Blk, al
		dec	al
		jns	@@Save_New_Pos
		mov	al, 15
		jmp	@@Save_New_Pos
		; ---
@@Dn:           mov	al, pos_Curr_Blk
		mov	pos_Prev_Blk, al
		inc	al
		cmp	al, 16
		jb	@@Save_New_Pos
		xor	al, al
		; ---
@@Save_New_Pos: mov	pos_Curr_Blk, al
		stc
		ret
		; ---
@@X:            test    FG_KLA, _b_00
                jz      @@exit
@@AltX:		call	Create_Cfg_Win
@@exit:		stc
		ret
Cfg_CK_Win_kla	ENDP

; --- просмотр конфигурации линий блока ЦК
;
Cfg_Ln_Win_kla	PROC
                cmp     ah, 14H         ; [  ]
                je      @@Up
                cmp     ah, 15H         ; [  ]
                je      @@Dn
                cmp     ah, 58H         ; [ X ]
                je      @@X
                cmp     ah, 0f9H        ; [ END ]
                je      @@End
                cmp     ah, 0faH        ; [ HOME ]
                je      @@Home
                cmp     ah, 0fbH        ; [ PgDn ]
                je      @@PgDn
                cmp     ah, 0fcH        ; [ PgUp ]
                je      @@PgUp
		clc
		ret
		; ---
@@Up:           mov	ax, pos_Curr_Ln
		mov	pos_Prev_Ln, ax
		dec	ax
		jns	@@Save_New_Pos
		mov	ax, pos_Ln_Count
		dec	ax
		jmp	@@Save_New_Pos
		; ---
@@Dn:           mov	ax, pos_Curr_Ln
		mov	pos_Prev_Ln, ax
		inc	ax
		cmp	ax, pos_Ln_Count
		jb	@@Save_New_Pos
		xor	ax, ax
		jmp	@@Save_New_Pos
		; ---
@@End:          mov	ax, pos_Curr_Ln
		mov	pos_Prev_Ln, ax
		mov	ax, pos_Ln_Count
		dec	ax
		jmp	@@Save_New_Pos
		; ---
@@Home:        	mov	ax, pos_Curr_Ln
		mov	pos_Prev_Ln, ax
		xor	ax, ax
		jmp	@@Save_New_Pos
		; ---
@@PgUp:         mov	ax, pos_Curr_Ln
		mov	pos_Prev_Ln, ax
		sub	ax, 32
		jns	@@Save_New_Pos
		mov	dx, pos_Ln_Count
		add	dx, ax
		mov	ax, dx
		jmp	@@Save_New_Pos
		; ---
@@PgDn:         mov	ax, pos_Curr_Ln
		mov	pos_Prev_Ln, ax
		add	ax, 32
		cmp	ax, pos_Ln_Count
		jb	@@Save_New_Pos
		sub	ax, pos_Ln_Count
		jmp	@@Save_New_Pos
		; ---
@@Save_New_Pos: mov	pos_Curr_Ln, ax
		stc
		ret
		; ---
@@X:            test    FG_KLA, _b_00
                jz      @@exit
@@AltX:		call	Create_Cfg_Win
       		call	Create_Cfg_CK_Win
@@exit:		stc
		ret
Cfg_Ln_Win_kla	ENDP

; --- просмотр тарификационной сети
;
Tar_Net_Win_kla PROC
                cmp     ah, 14H         ; [  ]
                je      @@Up
                cmp     ah, 15H         ; [  ]
                je      @@Dn
		clc
		ret
		; ---
@@Up:           mov	al, pos_Curr_CK
		mov	pos_Prev_CK, al
		dec	al
		jns	@@Save_New_Pos
		mov	al, pos_Max
		dec	al
		jmp	@@Save_New_Pos
		; ---
@@Dn:           mov	al, pos_Curr_CK
		mov	pos_Prev_CK, al
		inc	al
		cmp	al, pos_Max
		jb	@@Save_New_Pos
		xor	al, al
		; ---
@@Save_New_Pos: mov	pos_Curr_CK, al
                call    Refresh_Tar_Net_Win
                stc
		ret
Tar_Net_Win_kla     ENDP

; --- просмотр списка телефонных номеров
;
Tel_Num_Win_kla  PROC
                cmp     ah, 0aH         ; [ 0..9 или BACKSPACE ]
                jb      @@Digits        ; -> [ 0..9 ]
                je      @@BackSpace     ; -> [ BACKSPACE ]
                cmp     ah, 13H         ; [ ENTER ]
                je      @@Enter
                cmp     ah, 14H         ; [  ]
                je      @@Up
                cmp     ah, 15H         ; [  ]
                je      @@Dn
                cmp     ah, 0f9H        ; [ END ]
                je      @@End
                cmp     ah, 0faH        ; [ HOME ]
                je      @@Home
                cmp     ah, 0fbH        ; [ PgDn ]
                je      @@PgDn
                cmp     ah, 0fcH        ; [ PgUp ]
                je      @@PgUp
		clc
		ret
		; ---
@@Digits:
                movzx   bx, Tel_Num_Input_Len
		cmp	bl, 7
		jae	@@no_more_digits
		mov	Tel_Num_Input[bx], ah
		inc	Tel_Num_Input_Len
Tel_Num_for_Find:
@@draw_input:   mov     di, _pos_Tel_Num_Input
		mov	ah, _color_Tel_Num_Hi
		mov	es, ss:SEG_FS
		xor	bx, bx
@@loop:         cmp	bl, Tel_Num_Input_Len
		jae	@@draw_input_2
		mov     al, Tel_Num_Input[bx]
		add	al, '0'
		stosw
		inc	bx
		jmp	@@loop
@@draw_input_2:	mov	al, '▒'
@@loop_2:	cmp	di, _pos_Tel_Num_Input + 14
		jae	@@no_more_digits
		stosw
		jmp	@@loop_2
@@no_more_digits:
                stc
                ret
		; ---
@@BackSpace:    cmp	Tel_Num_Input_Len, 0
		je	@@no_more_digits
		dec	Tel_Num_Input_Len
		jmp	@@draw_input
		; ---
@@Enter:        ; преобразуем ввод в формат NLOG
		xor	eax, eax
		xor	bx, bx
@@format:       cmp	bl, Tel_Num_Input_Len
		jae	@@find
		shl	eax, 4
		or      al, Tel_Num_Input[bx]
		inc 	bx
		jmp	@@format
@@find:         ; ищем
		DB      9AH
                DW      OFFSET Find_Tel_Num_Far
                DW      120
                jnc	@@not_found
		; указатель / 8 - № п/п найденного телефона
@@found:	shr	esi, 3
		mov	pos_Found_Tel_Num, esi
		; № п/п делённый на 96 - начальный № в экране
		mov	eax, esi
		mov	ecx, _count_Lines_in_Scr
		xor	edx, edx
		div	ecx
		mov	ecx, _count_Lines_in_Scr
		mul	ecx
                mov     pos_Curr_Tel_Num, eax
                call    Refresh_Tel_Num_Win_2
                lea	si, _str_Tel_Found
		call	Print_Log
                stc
                ret
		; ---
@@not_found:    mov	pos_Found_Tel_Num, -1
                call    Refresh_Tel_Num_Win_2
		lea	si, _str_Tel_not_Found
		call	Print_Log_Err
                stc
                ret
		; ---
@@Up:           mov     eax, pos_Curr_Tel_Num
                sub     eax, _count_Lines_in_Scr
                jns     @@Save_New_pos
                xor     eax, eax
                jmp     @@Save_New_pos
		; ---
@@Dn:           mov     eax, pos_Curr_Tel_Num
                add     eax, _count_Lines_in_Scr
                cmp     eax, Cfg_NLOG_Count
                jb      @@Save_New_pos
                mov     eax, pos_Curr_Tel_Num
                jmp     @@Save_New_pos
		; ---
@@PgUp:         mov     eax, pos_Curr_Tel_Num
                sub     eax, _count_Lines_in_Scr * 10
                jns     @@Save_New_pos
                xor     eax, eax
                jmp     @@Save_New_pos
		; ---
@@PgDn:         mov     eax, pos_Curr_Tel_Num
                add     eax, _count_Lines_in_Scr * 10
                cmp     eax, Cfg_NLOG_Count
                jb      @@Save_New_pos
                mov     eax, pos_Curr_Tel_Num
                jmp     @@Save_New_pos
		; ---
@@End:          mov     eax, Cfg_NLOG_Count
                mov     ebx, _count_Lines_in_Scr
                div     ebx
                mul     ebx
                jmp     @@Save_New_pos
                ; ---
@@Home:         xor     eax, eax
		; ---
                jmp     @@Save_New_pos
		; ---
@@Save_New_Pos: mov     pos_Curr_Tel_Num, eax
                call    Refresh_Tel_Num_Win_2
                stc
                ret
Tel_Num_Win_kla  ENDP

; --- просмотр списка номеров УПАТС
;
U_Num_Win_kla   PROC
                cmp     ah, 14H         ; [  ]
                je      @@Up
                cmp     ah, 15H         ; [  ]
                je      @@Dn
                cmp     ah, 0f9H        ; [ END ]
                je      @@End
                cmp     ah, 0faH        ; [ HOME ]
                je      @@Home
                cmp     ah, 0fbH        ; [ PgDn ]
                je      @@PgDn
                cmp     ah, 0fcH        ; [ PgUp ]
                je      @@PgUp
		clc
		ret
		; ---
@@Up:           mov     eax, pos_Curr_Tel_Num
                sub     eax, _count_Lines_in_Scr
                jns     @@Save_New_pos
                xor     eax, eax
                jmp     @@Save_New_pos
		; ---
@@Dn:           mov     eax, pos_Curr_Tel_Num
                add     eax, _count_Lines_in_Scr
                cmp     eax, Cfg_NLOG_Count
                jb      @@Save_New_pos
                mov     eax, pos_Curr_Tel_Num
                jmp     @@Save_New_pos
		; ---
@@PgUp:         mov     eax, pos_Curr_Tel_Num
                sub     eax, _count_Lines_in_Scr * 10
                jns     @@Save_New_pos
                xor     eax, eax
                jmp     @@Save_New_pos
		; ---
@@PgDn:         mov     eax, pos_Curr_Tel_Num
                add     eax, _count_Lines_in_Scr * 10
                cmp     eax, Cfg_NLOG_Count
                jb      @@Save_New_pos
                mov     eax, pos_Curr_Tel_Num
                jmp     @@Save_New_pos
		; ---
@@End:          mov     eax, Cfg_NLOG_Count
                mov     ebx, _count_Lines_in_Scr
                div     ebx
                mul     ebx
                jmp     @@Save_New_pos
                ; ---
@@Home:         xor     eax, eax
		; ---
                jmp     @@Save_New_pos
		; ---
@@Save_New_Pos: mov     pos_Curr_Tel_Num, eax
                call    Refresh_U_Num_Win_2
                stc
                ret
U_Num_Win_kla   ENDP

; --- обработка клавиш окна выбора команд МО ---
MO_Select_Win_kla       PROC
                push    bx  cx
                        cmp     SOST_VIU5, _OneLn
                        je      @@exit
                        mov     bx, Lim_Win_MO
                        sub     bx, 2240                        ; верх. правая граница
                        mov     cx, Lim_Win_MO                  ; limit window - нижн. правая граница окна
@@old_limit:            ; -------------==========----------------
                        ; ---- будем обрабатывать клавиши -------
                        test    FG_KLA, _b_00
                        jnz     @@ALT           ; [ ALT ON    ]
                        ; --- ALT off ---
                        cmp     ah, 22h         ; [ space     ]
                        je      @@space
                        cmp     ah, 0FBh        ; [ page_down ]
                        je      @@page_down
                        cmp     ah, 0FCh        ; [ page up   ]
                        je      @@page_up
                        cmp     ah, 45h         ; [ E - end   ]
                        je      @@_end
                        cmp     ah, 48h         ; [ H - home  ]
                        je      @@_home
                        cmp     ah, 13h         ; [ Enter ]
                        je      @@_enter
                        ;cmp     ah, 21h
                        ;je      @@exit_f12
                        ; --- здесь стрелки ------
                        cmp     ah, 14H         ; [  ]
                        je      @@Up
                        cmp     ah, 15H         ; [  ]
                        je      @@Dn
                        cmp     ah, 11h         ; [  ]
                        je      @@Lft
                        cmp     ah, 12h         ; [  ]
                        je      @@Rht
                        jmp     @@exit
                        ; --- ALT ON ----
@@ALT:                  cmp     ah, 58h         ; [ X ]       - отметим 235-ую
                        je      @@235_X
                        cmp     ah, 43h         ; [ С ]       - уберем все звездочки с данного типа команд
                        je      @@CLEAR_MO
                        cmp     ah, 4Ch         ; [ L ]       - поставить все команды
                        je      @@SET_ALL_MO
                        cmp     ah, 44h         ; [ D ]       - убрать все команды
                        je      @@RESET_ALL_MO
                        cmp     ah, 52h         ; [ R ]       - поставить на контроль команды СОРМ
                        je      @@SET_SORM_MO
                        cmp     ah, 59h         ; [ Y ]       - поставить на контроль команды СОРМ
                        je      @@SET_BLACK_MO
                        cmp     ah, 54h         ; [ T ]       - поставить на контроль тарификационные команды
                        je      @@SET_TARIF_MO

                        cmp     ah, 0h
                        jae     @@alt_digs
                        jmp     @@exit
                        ; --- ALT - digs ---
        @@alt_digs:     cmp     ah, 09h
                        ja      @@exit
                        push    ax bx dx di
                        xor     bx, bx
                        xor     dx, dx
                        xor     di, di
                        mov     dl, ah
                        mov     al, 10
                        mul     prev_dig_MO
                        add     ax, dx
                        cmp     ax, 255
                        ja      @@bad_numb
                        mov     prev_dig_MO, al
                        cmp     al, 99                          ; ввели три цифры - ?
                        jbe     @@null_prev                     ; нет
                        mov     prev_dig_MO, 0
         @@null_prev:
                        mov     bl, 15                   ; кол-во строк в окне под коды команд
                        cmp     al, 150
                        jbe     @@frst_pge
        @@scnd_pge:     sub     al, 150
                        or      FG_Curr_MO_Num, _page_down
                        mov     Lim_Win_MO, 72                  ;
                        add     Lim_Win_MO, 2240                ; новая нижн.правая граница
                        jmp     @@ok_numb
        @@frst_pge:     cmp     num_page_MO, 1
                        je      @@ok_numb
                        cmp     al, 99                  ; число от 100 до 150 ?
                        jbe     @@exit_alt_digs         ; нет
                        or      FG_Curr_MO_Num, _page_up
                        mov     Lim_Win_MO, 108
                        add     Lim_Win_MO, 2240
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
                        mul     line_digs_draw                            ;  кол-во символов в строке
                        add     di, ax                                    ; |
                        mov     ax, pos_Curr_MO_Num
                        mov     prev_Curr_MO_Num, ax
                        mov     pos_Curr_MO_Num, di
                        jmp     @@exit_alt_digs
        @@bad_numb:     pop     di dx bx ax
                        mov     prev_dig_MO, 0
                        jmp     @@exit
   @@exit_alt_digs:     pop     di dx bx ax
                        jmp     @@exit_arrows
                        ; --- 235 ON ---
        @@235_X:        test    FG_Curr_MO_Num, _235_cmd
                        jnz     @@235_y
                        or      FG_Curr_MO_Num, _235_btn
                        or      FG_Curr_MO_Num, _235_cmd
                        jmp     @@exit
        @@235_y:        and     FG_Curr_MO_Num, not _235_cmd
                        or      FG_Curr_MO_Num, _235_btn
                        jmp     @@exit
                        ; --- clear type MO---
@@CLEAR_MO:             or      FG_Curr_MO_Num, _clear_MO
                        jmp     @@exit
                        ; --- set all MO comands ---
@@SET_ALL_MO:           or      FG_Curr_MO_Num, _set_all_MO
                        jmp     @@exit
                        ; --- resert all MO comands
@@RESET_ALL_MO:         or      FG_Curr_MO_Num, _reset_all_MO
                        jmp     @@exit
@@SET_SORM_MO:          or      FG_Curr_MO_Num, _set_sorm_MO
                        jmp     @@exit
@@SET_BLACK_MO:         or      FG_Curr_MO_Num, _set_black_MO
                        jmp     @@exit
@@SET_TARIF_MO:         or      FG_Curr_MO_Num, _set_tarif_MO
                        jmp     @@exit
                        ; ------   ------
                        ; --- Enter ----
@@_enter:               test    FG_PROGRAM, _fg_prg_mi
                        jnz     @@go_mo
                        or      FG_PROGRAM, _fg_prg_mi
        @@go_mo:        test    FG_PROGRAM, _fg_prg_mo
                        jnz     @@go_out
                        or      FG_PROGRAM, _fg_prg_mo
        @@go_out:       call    INIC_W_VN
                        jmp     @@exit
                        ; --- ARROWS ---
@@Up:                   cmp     pos_Curr_MO_Num, cx
                        ja      @@exit_arrows
                        push    ax
                        mov     ax, pos_Curr_MO_Num
                        mov     prev_Curr_MO_Num, ax
                        pop     ax
                        cmp     pos_Curr_MO_Num, bx
                        jbe     @@Up_1
                        sub     pos_Curr_MO_Num, 160
                        jmp     @@exit_arrows
@@Up_1:                 add     pos_Curr_MO_Num, 2228
                        cmp     pos_Curr_MO_Num, 2240
                        jb      @@Up_2
                        jmp     @@exit_arrows
@@Up_2:                 mov     pos_Curr_MO_Num, cx
                        cmp     num_page_MO, 2
                        jne     @@exit_arrows
                        mov     pos_Curr_MO_Num, 2348
                        jmp     @@page_up
@@Dn:                   cmp     pos_Curr_MO_Num, cx
                        ja      @@exit_arrows
                        push    ax
                        mov     ax, pos_Curr_MO_Num
                        mov     prev_Curr_MO_Num, ax
                        pop     ax
                        cmp     pos_Curr_MO_Num, 2240
                        jae     @@Dn_1
                        add     pos_Curr_MO_Num, 160
                        jmp     @@exit_arrows
@@Dn_1:                 sub     pos_Curr_MO_Num, 2228
                        cmp     pos_Curr_MO_Num, bx
                        ja      @@Dn_2
                        jmp     @@exit_arrows
@@Dn_2:                 mov     pos_Curr_MO_Num, 0
                        cmp     num_page_MO, 1                  ; перелистнем страничку ?
                        je      @@page_down                     ; Да
                        jmp     @@exit_arrows
@@Lft:                  push    ax cx dx
                        mov     cx, 160
                        mov     ax, pos_Curr_MO_Num
                        cmp     ax, 0
                        je      @@lim_left
                        push    ax
                        xor     dx, dx
                        div     cx
                        pop     ax
                        or      dx, dx
                        jz      @@exit_l_r
                        mov     prev_Curr_MO_Num, ax
                        sub     pos_Curr_MO_Num, 12
                        jmp     @@exit_l_r
@@lim_left:             mov     prev_Curr_MO_Num, ax
                        mov     ax, Lim_Win_MO
                        mov     pos_Curr_MO_Num, ax
                        jmp     @@exit_l_r
@@Rht:                  push    ax cx dx
                        mov     cx, 160
                        mov     ax, pos_Curr_MO_Num
                        cmp     ax, bx
                        jb      @@first_rht
                        cmp     ax, Lim_Win_MO
                        jae     @@lim_rght
                        push    ax
                        sub     ax, bx
                        xor     dx, dx
                        div     cx
                        pop     ax
                        or      dx, dx
                        jz      @@exit_l_r
@@first_rht:            mov     prev_Curr_MO_Num, ax
                        add     pos_Curr_MO_Num, 12
                        jmp     @@exit_l_r
@@lim_rght:             mov     prev_Curr_MO_Num, ax
                        mov     pos_Curr_MO_Num, 0
@@exit_l_r:             pop     dx cx ax
                        jmp     @@exit_arrows
                        ; --- arrows end    -------
@@space:                or      FG_Curr_MO_Num, _spc_press          ; нажали пробел
                        jmp     @@exit
@@page_down:            or      FG_Curr_MO_Num, _page_down          ; перелистнем страницу
                        mov     Lim_Win_MO, 72                  ;
                        add     Lim_Win_MO, 2240                ; новая нижн.правая граница
                        jmp     @@exit
@@page_up:              or      FG_Curr_MO_Num, _page_up
                        mov     Lim_Win_MO, 108
                        add     Lim_Win_MO, 2240
                        jmp     @@exit
@@_end:                 push    ax
                        cmp     num_page_MO, 2                     ; last page - ?
                        jb      @@end_end                          ; no!
                        mov     ax, pos_Curr_MO_Num
                        mov     prev_Curr_MO_Num, ax
                        mov     pos_Curr_MO_Num, 14 * 160 + 72
                        pop     ax
                        jmp     @@exit_arrows
@@end_end:              mov     ax, pos_Curr_MO_Num
                        mov     prev_Curr_MO_Num, ax
                        mov     pos_Curr_MO_Num, 2348
                        pop     ax
                        jmp     @@exit_arrows
@@_home:                push    ax
                        mov     ax, pos_Curr_MO_Num
                        mov     prev_Curr_MO_Num, ax
                        mov     pos_Curr_MO_Num, 0
                        pop     ax
@@exit_arrows:          or      FG_Curr_MO_Num, _arrows             ; перерисуем стрелки
@@exit:                 pop     cx bx
                        ret
MO_Select_Win_kla       ENDP

; --- Обработка клавиш окна BlackList Debuger
BL_Win_Kla      PROC
                ; ---- будем обрабатывать клавиши -------
                test    FG_KLA, _b_00
                jz      @@EXIT             ; [ ALT OFF ]
                ; --- Alt ON
                cmp     ah, 58h            ; [ X ]
                jne     @@EXIT
                cmp     BL_dbg_q, 255
                jne     @@put_quit
                mov     BL_dbg_q, 0
        push    ax cx si di
                ; --- перерисуем не все верхнее окно
                LEA     SI,MENU                 ;|
                ADD     SI, 1280
                XOR     DI,DI                   ;| переинициализируем верхнее окно
                ADD     DI, 2560
                mov     ah,_color_normal_f      ;|
                CALL    printSscr               ;/
        pop     di si cx ax
                jmp     @@EXIT
                ; --- отметим "квитированный ч.с."
@@put_quit:     mov     BL_dbg_q, 255
                ; --- нарисуем окно
                mov     ah, _color_normal_BL;;;_color_normal_MO
                mov     di, _pos_Tel_Num_Win + 2560  ; не с самого верха
                mov     si, offset Bl_Debug_Win_guit
                mov     cx, 16
                call    printWin
                mov     ah, _color_avar
                mov     di, 2580
                mov     si, OFFSET _Bl_dbg_name_q
                call    printS
                ; --------------------------------
@@EXIT:         ret
BL_Win_Kla      ENDP
; ------------------------------------------------
; ----- обработка клавиш окна счетчиков ошибок ПУМА
Puma_cnts_kla   PROC
                cmp     ah, 52h         ; [ R ]
                je      @@go_reset_tar
                cmp     ah, 43h         ; [ C ]
                je      @@go_reset_cto
                cmp     ah, 48h         ; [ H ]
                je      @@go_reset_conf
                ; -------=======--------
                cmp     ah, 56h         ; [ V ]
                je      @@go_conf
                cmp     ah, 23h         ; [ TAB ]
                je      @@go_cto
                ; ----====================
                JMP     @@go_out
                ; --- Debug window of PUMA cto channel
    @@go_cto:   test    Puma_flg, _cto_active
                jz      @@refr_cto
                ; // clear Puma cto window
                push    ax cx si di
                push    ds
                mov     ds, SS:SEG_PCM
                mov     si, offset MENU
                mov     di, _pos_Win_Cfg
                mov     ah, _color_normal_f
                mov     cx, 16
@@refr_cnt_win: push    cx
                mov     cx, 80
                call    printSLen
                pop     cx
                add     si, 80
                add     di, 160
                loop    @@refr_cnt_win
                pop     ds
                pop     di si cx ax
                @_Puma_Debug_Init  Puma_debug_tar, Puma_cnts_tar, Puma_tmr_tar, 0, Puma_menu_tar
                and     Puma_flg, not _cto_active
                JMP     @@go_out
   @@refr_cto:  @_Puma_Debug_Init  Puma_debug_cto, Puma_cnts_cto, Puma_tmr_cto, 60, Puma_menu_cto
                or      Puma_flg, _cto_active
                JMP     @@go_out
                ; --- Debug window of Configuration mode
  @@go_conf:    test    Puma_flg, _conf_active
                jz      @@draw_conf
                ; --// clear Puma conf window
                push    ax cx si di
                push    ds
                mov     ds, SS:SEG_PCM
                mov     si, offset MENU
                add     si, 17*80
                mov     di, _pos_Win_Cfg
                add     di, 17*160
                mov     ah, _color_normal_f
                mov     cx, 10
@@refr_conf_win:
                push    cx
                mov     cx, 80
                call    printSLen
                pop     cx
                add     si, 80
                add     di, 160
                loop    @@refr_conf_win
                pop     ds
                pop     di si cx ax
                and     Puma_flg, not _conf_active
                jmp     @@go_out
                ; -- drawing
  @@draw_conf:  push    ebx ax cx si di
                ; --- нарисуем главное окно
                mov     si, offset Conf_mode_win
                mov     ah, 30h
                mov     di, _pos_Win_Cfg + 17*160
                mov     cx, 10
                call    printWin
                ; -- нарисуем начальные значения №№ пакетов
                ; = 80.0E
                xor     ebx, ebx
                mov     cx, 5
                mov     ah, _color_puma_v
                mov     di, _pos_Win_Cfg + 17*160 + 3*160 + 8
    @@init_800E:;
                push   cx
                mov     cl, 4
                call    printH
                pop    cx
                add     di, 160
                loop    @@init_800E
                ; = 80.8E
                mov     cx, 5
                mov     di, _pos_Win_Cfg + 17*160 + 3*160 + 48
    @@init_808E:;
                push   cx
                mov     cl, 4
                call    printH
                pop    cx
                add     di, 160
                loop    @@init_808E
                ; -- нарисуем начальные значения timer 80.0E
                mov     si, offset Puma_counts_val
                mov     di, _pos_Win_Cfg + 17*160 + 3*160 + 24
                mov     cx, 5
  @@init_tmr_0E:;
                call    printS
                add     di, 160
                loop    @@init_tmr_0E
                ; -- нарисуем начальные значения timer 80.8E
                mov     di, _pos_Win_Cfg + 17*160 + 3*160 + 66
                mov     cx, 5
  @@init_tmr_8E:;
                call    printS
                add     di, 160
                loop    @@init_tmr_8E
                ; --- END debug conf mode
                pop     di si cx ax ebx
                or      Puma_flg, _conf_active
                jmp     @@go_out
                ; ---  End conf ----
@@go_reset_tar: ; --- очистка данных tar channel
                ; =====------======------
                @_Puma_cnts_Init  0,0,0
                jmp     @@go_out
@@go_reset_cto: ; --- очистка данных cto channel
                ; =====------======------
                test    Puma_flg, _cto_active
                jz      @@go_out
                @_Puma_cnts_Init  59, PUMA_Trans_Size, 60
                jmp     @@go_out
@@go_reset_conf:; -- очистка данных conf channel
                test    Puma_flg, _conf_active
                jz      @@go_out
                ; --- clear conf buffs
                push    eax cx di
                push    es
                mov     es, SS:SEG_AX
                mov     di, offset Pak_800E
                xor     eax, eax
                mov     cx, 15
                rep     STOSD
                pop     es
                pop     di cx eax
                ; -- Exit
     @@go_out:  RET
Puma_cnts_kla   ENDP
; =================================================

; === открытие (создание) верхних окон ======================================
;
; --- просмотр процесса сбора конфигурации
;
Create_Cfg_Win  PROC
		; ---
		mov	NOM_VERX_OKNA, _Cfg_Win
                ; --- определяем сколько у нас всего ЦК
		mov	es, ss:SEG_AXD
		xor	al, al
		xor	si, si
@@count_loop:	cmp	es:TIS_TIS[si], 16
		ja	@@count_next
		inc	al
@@count_next:	inc	si
		cmp	si, KL_TIS
		jb	@@count_loop
		mov	pos_Max, al
                ; --- вывод самого окна
                mov     si, offset Cfg_Win
                mov     di, _pos_Win_Cfg
                mov     ah, _color_Win_Cfg
                push    ds
                mov     ds, ss:Seg_PCM
                call    printS
                pop     ds
                ; --- вывод состояния процесса
                mov     si, offset str_CfgRun
                cmp     Cfg_Run, 0
                jne     @@draw_Status
                mov     si, offset str_CfgDone
@@draw_Status:  mov     di, _pos_CfgStatus
                call    printS
                ; --- вывод имеющихся в наличии ЦК и их уровней
                mov     ah, _color_Win_Cfg
		mov    	es, ss:SEG_AXD
		xor	bx, bx
		xor	si, si
@@CK_loop:      ; ---
		cmp	BYTE PTR es:TIS_TIS[si], KL_TIS
		jae	@@CK_next
		; ---
		mov     ax, 160
		movzx	cx, bh
		and	cx, 1fh
		mul	cx
		mov	di, ax
		add	di, 160 + 2
		; ---
;IF _MSP EQ TRUE MVT закомментировал
;		mov	ax, 40
;ELSE
                mov     ax, 80
;ENDIF
		movzx	cx, bh
		shr	cx, 5
		mul	cx
		add	di, ax
		push	ebx
		; ---
                mov     ah, _color_Win_Cfg
		movzx	ebx, bl
		mov	cx, 2
		call	printD
		; ---
		mov	bl, Cfg_CK_Level[si]
		mov	cx, 1
		add	di, 6
		call    printH
		; ---
		pop	ebx
		inc	bh
@@CK_next:	inc	bl
		inc	si
		cmp	bl, KL_TIS
		jb	@@CK_Loop
                ret
Create_Cfg_Win  ENDP

; --- просмотр конфигурации ЦК
;
Create_Cfg_CK_Win  PROC
		; ---
		mov	NOM_VERX_OKNA, _Cfg_CK_Win
                ; --- вывод самого окна
		mov	ah, _color_Cfg_CK_Win
                mov     si, offset Cfg_CK_Win
		mov	cx, _height_Cfg_CK_Win
		mov	di, _pos_Cfg_CK_Win
@@draw_loop: 	push    ds
                mov     ds, ss:Seg_PCM
		call	printS
		pop	ds
		add	si, _width_Cfg_CK_Win
		add	di, 160
		loop	@@draw_loop
                ; --- вывод N ЦК
		mov	es, ss:SEG_AXD
		xor	al, al
		xor	ebx, ebx
@@find_loop:	cmp 	es:TIS_TIS[bx], 16
		jae	@@find_next_2
		cmp	al, pos_Curr_CK
		jb	@@find_next
		ja	@@exit
		mov	Cfg_CK_Num, bl
		mov	cx, 2
		mov	di, _pos_Cfg_CK_Num
		call	printD
@@exit:		ret
@@find_next:	inc	al
@@find_next_2:	inc	bx
		cmp	bl, KL_TIS
		jb	@@find_loop
		ret
Create_Cfg_CK_Win  ENDP

; --- просмотр конфигурации линий блока ЦК
;
Create_Cfg_Ln_Win  PROC
		; ---
		mov	NOM_VERX_OKNA, _Cfg_Ln_Win
                ; --- вывод самого окна
                mov     si, offset Cfg_Ln_Win
                mov     di, _pos_Cfg_Ln_Win
                mov     ah, _color_Win_Cfg
                push    ds
                mov     ds, ss:Seg_PCM
                call    printS
                pop     ds
                ; --- вывод номера ЦК и модема
		movzx	ebx, Cfg_CK_Num
		mov	cx, 2
		mov	di, _pos_Cfg_Ln_CK
		call	printD
		movzx	ebx, pos_Curr_Blk
		mov	cx, 2
		mov	di, _pos_Cfg_Ln_Blk
		call	printD
		; --- получаем указатель на и количество линий
		movzx	ebx, Cfg_CK_Num
		shl	ebx, 4
		or	bl, pos_Curr_BLK
		shl	ebx, 3
		mov	es, ss:SEG_KONF
		mov	eax, es:[ebx + 1]
		mov	pos_Base_Ptr, eax
		mov	ax, es:[ebx + 6]
		mov	pos_Ln_Count, ax
		ret
Create_Cfg_Ln_Win  ENDP

; --- просмотр тарификационной сети
;
Create_Tar_Net_Win      PROC
                mov     NOM_VERX_OKNA, _Tar_Net_Win
                ; --- определяем сколько у нас всего ЦК
		mov	es, ss:SEG_AXD
		xor	al, al
		xor	si, si
@@count_loop:	cmp	es:TIS_TIS[si], 16
		ja	@@count_next
		inc	al
@@count_next:	inc	si
		cmp	si, KL_TIS
		jb	@@count_loop
		mov	pos_Max, al
                ; --- вывод самого окна
                mov     ah, _color_Tar_Net_Win
                mov     si, offset Tar_Net_Win
                mov     cx, _height_Tar_Net_Win
                mov     di, _pos_Tar_Net_Win
@@draw_loop: 	push    ds
                mov     ds, ss:Seg_PCM
		call	printS
		pop	ds
                add     si, _width_Tar_Net_Win
		add	di, 160
		loop	@@draw_loop
                ; ---
                call    Refresh_Tar_Net_Win
                ret
Create_Tar_Net_Win      ENDP

; --- просмотр списка телефонных номеров
;
Create_Tel_Num_Win      PROC
		; ---
                mov     NOM_VERX_OKNA, _Tel_Num_Win
                ; --- вывод самого окна
                mov     ah, _color_Tel_Num_Win
                mov     di, _pos_Tel_Num_Win
                mov     si, offset Tel_Num_Win
                push    ds
                mov     ds, ss:SEG_PCM
                call    printS
                pop     ds
                ; --- вывод числа линий
                mov     ah, _color_Tel_Num_Hi
                mov     ebx, Cfg_NLOG_Count
                mov     cx, 5
                mov     di, _pos_Tel_Num_Count
                call    printD
                ; --- подсветка поля ввода
;                mov     di, _pos_Tel_Num_Input
;                mov     cx, 7
;                call    printA
                ; --- выведем искомый № телефона
                call    Tel_Num_for_Find
                ; ---
		mov	count_Prev_NLOG, -1
		call	Refresh_Tel_Num_Win
                ; ---
                ret
Create_Tel_Num_Win      ENDP

; --- просмотр списка номеров УПАТС
;
Create_U_Num_Win        PROC
		; ---
                mov     NOM_VERX_OKNA, _U_Num_Win
                ; --- вывод самого окна
                mov     ah, _color_Tel_Num_Win
                mov     di, _pos_Tel_Num_Win
                mov     si, offset U_Num_Win
                push    ds
                mov     ds, ss:SEG_PCM
                call    printS
                pop     ds
                ; --- вывод числа линий
                mov     ah, _color_Tel_Num_Hi
                mov     ebx, Cfg_ULOG_Count
                mov     cx, 4
                mov     di, _pos_Tel_Num_Count + 4
                call    printD
                ; ---
		mov	count_Prev_NLOG, -1
                call    Refresh_U_Num_Win
                ; ---
                ret
Create_U_Num_Win        ENDP

; ---- просмотр окна выбора команд МО ---
;
Create_MO_Select_Win    PROC
push    ax cx di si
                ; --- нарисуем окно
                mov     NOM_VERX_OKNA, _MO_Slct_Win
                mov     ah, _color_normal_MO
                mov     di, _pos_Tel_Num_Win    ; начнем с самого верха
                mov     si,  offset Select_Mo_Win
                mov     cx, 30
                call    printWin
                push    ax
                mov     ah, _color_norm_help
                mov     di, 26
                mov     si, OFFSET _MO_name
                call    printS
                mov     di, 18*160 + 8
                mov     si, OFFSET _help_pge
                call    printS
                mov     di, 28*160 + 10
                mov     si, OFFSET _addit_MO
                call    printS
                pop     ax
                ; --- напечатаем цифры
                mov     cx, 15
                mov     di, frst_dig_MO;;;_pos_Win_Cfg + 2*160 + 2
                mov     ebx, 1
@@loop_digs:    call    printDigs
                inc     ebx
                add     di, line_digs_draw
                loop    @@loop_digs
                cmp     ebx, 150
                ja      @@go_stars
                sub     di, 15 * 160
                add     di, 12
                mov     cx, 15
                jmp     @@loop_digs
@@go_stars:     mov     curr_MO, ebx
                call    Stars_MO_Win                    ;  нарисуем stars, если они есть

@@exit:
pop     si di cx ax
                ret
Create_MO_Select_Win    ENDP

; --- окно отладчика BlackList
Create_BL_Debug_Win     PROC
                ; --- нарисуем окно для начала
        push    ax cx si di
                mov     NOM_VERX_OKNA, _MO_BL_DBG_Win
                mov     ah, _color_normal_BL;;;_color_normal_MO
                mov     di, _pos_Tel_Num_Win  ; с самого верха
                mov     si, offset Bl_Debug_Win
                mov     cx, 16
                call    printWin
                mov     ah, _color_avar
                mov     di, 24
                mov     si, OFFSET _Bl_dbg_name
                call    printS
                cmp     BL_dbg_q, 255
                jne     @@go_out
                ; --- нарисуем окно BlackList Debuger Quit
                mov     ah, _color_normal_BL;;;_color_normal_MO
                mov     di, _pos_Tel_Num_Win + 2560  ; не с самого верха
                mov     si, offset Bl_Debug_Win_guit
                mov     cx, 16
                call    printWin
                mov     ah, _color_avar
                mov     di, 2580
                mov     si, OFFSET _Bl_dbg_name_q
                call    printS
    @@go_out:
        pop     di si cx ax
                RET
Create_BL_Debug_Win     ENDP
; -----------------------------------------
; ----- Создание окна счетчиков ошибок ПУМы
Create_PUMA_Debug       PROC
                mov     NOM_VERX_OKNA, _Puma_debug
                @_Puma_Debug_Init  Puma_debug_tar, Puma_cnts_tar, Puma_tmr_tar, 0, Puma_menu_tar
                RET
Create_PUMA_Debug       ENDP
; ------------------------------------------
; === РАБОТА С КЛАВИШАМИ НИЖНИХ ОКОН ====================================================
;
ADR_KLA_NIZ     LABEL   WORD
                DW      OFFSET  OBSH_KLA        ;0
                DW      OFFSET  OneLn_kla       ;1
                DW      OFFSET  Edit_TarNet_kla ;2  ; Edit_TarNet     ; новая сеть тарификации
                DW      OFFSET  DOS_kla         ;3  ; DOS
                DW      OFFSET  X25_kla         ;4  : X25
                DW      OFFSET  EditNumLen      ;5  ; длина номера
                DW      OFFSET  Begin_Cfg_kla   ;6  ; запрос сбора конфигурации
                DW      OFFSET  Prefix_kla      ;7  : Prefix
                DW      OFFSET  RED_CHNG_PRGFLGS;8  ; Flags
                DW      OFFSET  Test_number_BL  ;9  ; BlackList
                DW      OFFSET  reboot_kla      ;10 ; Reboot
                DW      OFFSET  End_Cfg_kla     ;11 ; запрос останова конфигурации
                DW      OFFSET  OBSH_KLA        ;12 ; контроль ТЧ
                DW      OFFSET  OBSH_KLA        ;13 ; контроль ОЗУ коммутации
                DW      OFFSET  OBSH_KLA        ;14 ; контроль ошбочных пакетов

                DW      OFFSET  Check_Tar_kla   ;15 ; проверка тарификации
                DW      OFFSET  SORM_Pass_kla   ;16 ; ввод пароля эмулятора СОРМ
                DW      OFFSET  SORM_Emul_kla   ;17 ; выбор режима эмулятора СОРМ
                DW      OFFSET  SORM_Input_kla  ;18 ; ввод данных для эмулятора СОРМ

                DW      OFFSET  OBSH_KLA        ;19 ; просмотр прохождения контрольной точки

_OneLn                  equ     1       ; контроль одной линии
_Edit_TarNet            equ     2       ; редактирование сети тарификации
_DOS_kla                equ     3       ; выход в DOS
_X25_Reset              equ     4       ; перезапуск СОРМ
_Edit_NumLen            equ     5       ; редактирование длины номера в станции
_Begin_Cfg_kla          equ     6       ; запрос сбора конфигурации АТС
_Edit_Prefix_Idx        equ     7       ; редактирование префиксов
_Test_number_BL         equ     9       ; тестирование номера на наличие его в черн. сп.
_Reboot_Programm        equ     10      ; перезагрузка программы
_End_Cfg_kla            equ     11      ; запрос останова конфигурации АТС
_TC_Ctrl                equ     12      ; контроль ТЧ
_OZUK                   equ     13      ; контроль ОЗУ коммутации
_bad_MO                 equ     14      ; контроль ошибочных пакетов

_Check_Tar_kla          equ     15      ; проверка тарификации
_SORM_Pass_kla          equ     16      ; ввод пароля эмулятора СОРМ
_SORM_Emul_kla          equ     17      ; выбор режима эмулятора СОРМ
_SORM_Input_kla         equ     18      ; ввод данных для эмулятора СОРМ

_Ctrl_Point             equ     19      ; контроль прохожденя контрольной точки

                ; --- диспетчер клавиш нижнего окна
KLA_NIGNEGO_OKNA:
                MOV     DI,SOST_VIU5
                SHL     DI,1
                JMP     ADR_KLA_NIZ[DI]

                ; --- обработка клавиш нижнего окна
OBSH_KLA        PROC
                ; alt
                TEST    FG_KLA, _b_00
                JZ      @@NO_ALT_OBSH
                cmp     ah, 17h         ; [ Alt + F2 ]
                je      CLEAR_Count     ; очистка счетчиков МО
                CMP     AH, 18h         ; [ Alt + F3 ]
                JE      SAVE_CONFIGUR   ; сохранить конфигурацию
                cmp     ah, 42h         ; [ Alt + B ]
                je      Begin_Cfg_init_win ; сбор конфигурации
;;;                cmp     ah, 44h         ; [ Alt + D ]
;;;                je      TC_Ctrl_init_win; контроль ТЧ
IF _MSP NE TRUE
if _debugFile  
                cmp     ah, 44h         ; [ Alt + D ]
		je	exec_clear_debug_file
endif
endif
                cmp     ah, 45h         ; [ Alt + E ]
                je      End_Cfg_init_win; останов конфигурации
                cmp     ah, 46h         ; [ Alt + F ]
                je      TC_Ctrl_init_win;контроль тональной частоты СОРМ
                cmp     ah, 47h         ; [ Alt + G ]
                je      OZUK_init_win   ; проверка ОЗУ коммутации
                cmp     ah, 4dh         ; [ Alt + M ]
                je      bad_MO_init_win ; контроль ошибочных пакетов
                cmp     ah, 4fh         ; [ Alt + O ]
                je      OneLn_init_win  ; контроль одной линии
                cmp     ah, 50h         ; [ Alt + P ]
                je      Ctrl_Point_init ; просмотр прохождения контрольной точки
        IF _MSP NE TRUE
                cmp     ah, 51h         ; [ Alt + Q ]
                je      DOS_init_win    ; выход в DOS
        ENDIF
                cmp     ah, 53h         ; [ Alt + S ]
                je      Reboot_init_win ; перезагрузка
@@NO_ALT_OBSH:
                cmp     ah, 17h         ; [ F2 ]
                je      printLastExc    ; вывод последней исключительной ситуации
                ; --- перезапуск СОРМ
IF _MSP NE TRUE
                cmp     ah, 18h         ; [ F3 ]
                je      X25_init_win    ;
ELSE
                CMP     AH, 1AH         ; [ F5 ]
                JE      EditPrefixGo
ENDIF
                cmp     ah, 1Bh         ; [ F6  ]
                je      init_test_BL    ; тестирование черного списка
                CMP     AH, 1CH         ; [ F7 ]
                je      EditNumLenGo    ; редактирование длины номера в станции
                CMP     AH, 1DH         ; [ F8 ]
                JE      EditTarNetGo    ; редактирование сети тысячников
                cmp     ah, 1eh         ; [ F9 ]
                je      Check_Tar_init_win ; проверка тарификации
                ; --- добавлено для редактирования настроек самой программы
                cmp     AH, 1FH         ; [ F10 ]
                je      CHANGE_PROG_FLAGS ;
                ; ---
                cmp     ah, 20h         ; [ F11 ] - эмуляция СОРМ
                je      SORM_Emul_init
                RET
OBSH_KLA        ENDP

IF _MSP NE TRUE
if _debugFile
exec_clear_debug_file proc
                db      9Ah
                dw      offset clear_debug_file
                dw      120
                ret
exec_clear_debug_file endp
endif
endif


; ----=> printLastExc <=-----------------------
; Crated by Smaller on 09/12/04 2:32
; рисует окно последней исключительной ситуации по F2
;Входные данные: ah - клавиша
printLastExc    PROC
                db      9Ah
                dw      offset printLastExcSeg
                dw      120
                ret
printLastExc    ENDP

; --- очистка счетчиков межпроцессорного обмена --------------------------------
CLEAR_Count     PROC
                cld
;                mov     SOED_Busy_Err, 0        ;
;                mov     SOED_Full_Err, 0        ;
;                mov     SOED_no_Ref_Err, 0      ;
;                mov     SOED_Ref_A_Err, 0       ;
;                mov     SOED_Ref_AB_Err, 0      ;
                mov     bad_MO_count, 0         ;

                mov     MO_Err, 0               ; Счетчик ошибок МО
                mov     OSH_T_O, 0              ; Тайм Аут
                mov     OSH_VX_P, 0             ; Нет входного потока
                mov     OSH_GOT, 0              ; Отсутствие готовности
                mov     OSH_PAK, 0              ; Ошибочных пакетов

                mov     BL_CurrNum_In, 0        ; очистка последнего АОН
                mov     BL_Count_In, 0          ; очистка счетчиков черного списка
                mov     BL_CurrNum_Out, 0       ; очистка последнего АОН
                mov     BL_Count_Out, 0         ; очистка счетчиков черного списка

                mov     DWORD PTR LAPB_Err, 0
                mov     DWORD PTR X25_Err, 0

                mov     cx, 50
                mov     es, ss:SEG_AX
                mov     di, offset _1st_dd
                xor     eax, eax
                rep     stosd
                ; --- clear exception_data
                push    es
                push    ss
                pop     es
                mov     cx, _exception_data_l
                mov     di, offset ex
                xor     eax, eax
                rep     stosb
                pop     es
                ; ----
                mov     si, offset log_clear_count
                call    print_log
                ret
CLEAR_Count     ENDP

; --- сохранение конфигурации на флеш ------------------------------------------
SAVE_CONFIGUR   PROC
IF _MSP EQ TRUE
                DB      9AH                     ;\
                DW      OFFSET SAVE_CONFD       ; > не слишком простой способ :-)
                DW      120                     ;/
                mov     si, offset LOG_SAVE_FLASH;\ выведем лог о сохранении
                call    print_log               ; /
                RET
ELSE
        clc
                mov     edx, OFFSET _FILE_CFG_NAME
		CALL_HDD FileOpen
		jc	SC_no_Conf
SC_Write:       xor     edx, edx
                mov     ecx, _FILE_CFG_SIZE
		CALL_HDD FileWrite
		jc  	SC_Err
		;;;vic
                CALL_HDD FileClose
                ;;;jc      SC_Err
		;;;vic
                mov     si, offset LOG
       	 	call    print_log
                mov     si, offset LOG_SAVED_CFG
                mov     di, _pos_log + 8
                mov     ah, color_log
                call    printS
                jmp     SC_BLst
SC_Err:
                mov     si, offset LOG
                call    Print_log
                mov     si, offset LOG_ERR_CFG
                mov     di, _pos_log + 8
                mov     ah, _color_err_log
                call    printS
                jmp     SC_BLst
SC_no_Conf:
                mov     edx, OFFSET _FILE_CFG_NAME
                mov     ecx, _FILE_CFG_SIZE
		CALL_HDD FileCreateSize
		jc	SC_Err
		jmp	SC_Write
                ; --- Black List ---
SC_BLst:        mov     edx, OFFSET _FILE_BLST_NAME
		CALL_HDD FileOpen
                jc      SC_no_Conf_BL
SC_Write_BL:    xor     edx, edx
                mov     ecx, _FILE_BLST_SIZE
                push    ds
                mov     ds, ss:SEG_BLK_LST
		CALL_HDD FileWrite
                pop     ds
                jc      SC_Err_BL
		;;;vic
                CALL_HDD FileClose
                ;;;jc      SC_Err_BL
		;;;vic
                mov     si, offset LOG_SAVED_BLST
                mov     di, _pos_log + 74
                mov     ah, color_log
                call    printS
        	RET
SC_Err_BL:
                mov     si, offset LOG_ERR_BLST
                mov     di, _pos_log + 74
                mov     ah, _color_err_log
                call    printS
                RET
SC_no_Conf_BL:
                mov     edx, OFFSET _FILE_BLST_NAME
                mov     ecx, _FILE_BLST_SIZE
		CALL_HDD FileCreateSize
                jc      SC_Err_BL
                jmp     SC_Write_BL
ENDIF
SAVE_CONFIGUR   ENDP


; ----=> EditRoadGo <=-----------------------
; Created by Smaller on 10/21/04 8:44
; Инициализирует редактирование сети тысячников
EditTarNetGo    PROC
                mov     perem_viv1, 0           ; номер редактируемого элемента
                mov     perem_viv2, 0           ; положение в элементе
                mov     perem_vivD, 0FFFFFFFFh
                mov     si, offset Win_TarNet
                mov     di, _pos_Win_TarNet
                mov     ah, _color_WinTarNet
                push    ds
                mov     ds, ss:Seg_PCM
                call    printS
                pop     ds
                mov     sost_viu5, _Edit_TarNet
                ret
EditTarNetGo    ENDP

Edit_TarNet_kla PROC
                CMP     AH,13H          ; [ Enter ]
                JE      EST_IZMEN_ROAD
                CMP     AH,10           ; [ 0 - 9 ]
                JB      EST_KLA_ROAD

                CMP     AH,0fch         ; [ PgUp ]
                JE      EST_KLA_PGUP
                CMP     AH,0fbh         ; [ PgDn ]
                JE      EST_KLA_PGDN
                CMP     AH,14h          ; [  ]
                JE      EST_KLA_UP
                CMP     AH,15h          ; [  ]
                JE      EST_KLA_DN

                CMP     AH,22H          ; [ Space ]
                JE      SPACE_ROAD
                        ;;;;
                CMP     AH,40H          ; [ A (- 1) ]
                JBE     END_KW
                CMP     AH,46H          ; [ F ]
                JA      END_KW
                SUB     AH,27H          ; -> A-F
                JMP     SHORT EST_KLA_ROAD
                RET
IF _MSP EQ TRUE
F2_SBROS_TIS:
                ;TEST    FG_KLA,M_OR_0
                ;JZ      END_KW
                XOR     ESI,ESI
                MOV     SI,LINIY
                CMP     SI,KL_TIS
                JB      NORM_SBROS_TIS
                MOV     SI,KL_TIS-1
NORM_SBROS_TIS:
                MOV     FG_TIS[SI],0
                SHL     ESI,3
                ADD     ESI,SMESH_DATA_T
                MOV     GS:[ESI],DWORD PTR 0FFFFFFFFH
                RET
ENDIF
EST_IZMEN_ROAD:
                PUSH    BX
                DB      9AH
                DW      OFFSET MOIN_1D
                DW      120
                POP     BX
                mov     ah, _color_normal_f
                LEA     SI,PO_OBS
                MOV     DI,_pos_PO_OBS
                CALL    printSscr
                MOV     SOST_VIU5,0
                RET
SPACE_ROAD:
                ADD     PEREM_VIV1,2
                AND     PEREM_VIV1,0001EH
                RET
EST_KLA_ROAD:
                MOV     DI,LINIY
                CMP     DI,KL_TIS
                JB      SHORT NORM_TIS_ROAD
IF _MSP NE TRUE
                xor     di, di
ENDIF
NORM_TIS_ROAD:
                SHL     DI,5
                ADD     DI,PEREM_VIV1
                SHR     DI,1
                TEST    PEREM_VIV1,M_OR_0
                JNZ     SHORT MLADSH_KLA_ROAD
                AND     BYTE PTR TIS_ROAD[DI],00FH
                SHL     AH,4
                OR      BYTE PTR TIS_ROAD[DI],AH
                JMP     SHORT END_TIS_ROAD_ALL
MLADSH_KLA_ROAD:
                AND     BYTE PTR TIS_ROAD[DI],0F0H
                AND     AH,0FH
                OR      BYTE PTR TIS_ROAD[DI],AH
                MOV     AL,BYTE PTR TIS_ROAD[DI]
                CMP     AL,KL_TIS
                JB      SHORT END_TIS_ROAD_R
                MOV     AL,0FFH
                ;JMP     SHORT END_TIS_ROAD_R
END_TIS_ROAD_R:
                MOV     BYTE PTR TIS_ROAD[DI],AL
END_TIS_ROAD_ALL:
                INC     PEREM_VIV1
                AND     PEREM_VIV1,001FH
                RET

EST_KLA_PGUP:   sub     LINIY, 32
                jmp     EST_KLA_MOVE
EST_KLA_PGDN:   add     LINIY, 32
                jmp     EST_KLA_MOVE
EST_KLA_UP:     dec     LINIY
                jmp     EST_KLA_MOVE
EST_KLA_DN:     inc     LINIY
EST_KLA_MOVE:   mov     ax, LINIY
                or      ax, ax
                js      EST_KLA_0
                cmp     ax, KL_TIS
                jae     EST_KLA_KL_TIS
                ret
EST_KLA_0:      mov     LINIY, 0
                ret
EST_KLA_KL_TIS: mov     LINIY, KL_TIS - 1
                ret
Edit_TarNet_kla ENDP



; ----=> EditNumLenGo <=-----------------------
; Created by Smaller on 10/22/04 10:44
; инициализирует окно изменения длины номера в станции
EditNumLenGo    PROC
                mov     si, OFFSET Win_NumLen
                mov     di, _pos_Win_TarNet
                mov     ah, _color_WinTarNet
                call    printSscr
                mov     sost_viu5, _Edit_NumLen
                mov     al, LENGTH_NUM
                mov     Length_Num_Cpy, al
                ret
EditNumLenGo    ENDP

                ; --- редактирование длины номера в АТС
EditNumLen:     cmp     ah, 13h
                je      ENL_Enter
                ;
                cmp     ah, 5
                jb      ENL_Exit
                cmp     ah, 7
                ja      ENL_Exit
ENL_Digit:      mov     Length_Num_Cpy, ah
ENL_Exit:       ret
                ;
ENL_Enter:      cmp     Length_Num_Cpy, 5
                jb      ENL_Exit
                cmp     Length_Num_Cpy, 7
                ja      ENL_Exit
                ;
                mov     al, Length_Num_Cpy
                mov     LENGTH_NUM, al
                ;
                mov     si, OFFSET PO_OBS
                mov     di, _pos_PO_OBS
                mov     ah, _color_normal_f
                call    printSscr
                mov     SOST_VIU5, 0
                ret

; вывод справки по программе
View_Help       PROC
                mov     si, offset Help_Win
                cmp     Pass_OK, 0
                je      @@draw
                mov     si, offset Help_Special_Win
@@draw:         mov     di, 3 * _y
                mov     ah, _color_normal_f
                call    printSscr
                ret
View_Help       ENDP

; Процедура инициализации теста черного списка
; Работает в любом режиме (т.е. без пароля)
Init_test_BL    PROC
                ; проверим включен ли хоть какой-нибудь чёрный список
                test    FG_PROGRAM, _fg_prg_bl_out + _fg_prg_bl_in
                jz      @@exit                  ; нет, выйдем
                ; --- выведем окно
                mov     si, offset Test_BL_Win
                mov     di, _pos_Win_BL
                mov     ah, _SM_WIN_COLOR
                push    ds
                mov     ds, SS:SEG_PCM
                call    printS
                pop     ds

                mov     si, offset BL_status_inNum
                mov     di, _pos_BL_status
                mov     ah, _color_log
                call    printS

                ; !!! здесь выведем белый список
                push    FS
                mov     FS, SS:SEG_BLK_LST          ; сегмент черного списка

                mov     esi, 80000h - (4+16*16*4)   ; Начнем с конца 512kb
                mov     di, _pos_BL_White
                mov     cx, 16

@@print_White:  lods    DWORD PTR FS:[ESI]
                mov     ebx, eax
                mov     ah, cBLUE*10h + clWHITE
                call    printBL
                add     di, 9*_x
                cmp     cx, 9
                jne     @@no_next_string
                mov     di, _pos_BL_White + _y
@@no_next_string:
                loop    @@print_White

                pop     FS
                mov     BL_number, 0
                mov     BL_numPos, 0
                MOV     SOST_VIU5, 9;_Test_number_BL
@@exit:         ret
Init_test_BL    ENDP

; процедура тестирования номеров черного списка
Test_number_BL  PROC
                pushad
                push    ds
                push    es

                cmp     ah, 10          ; [ 0 - 9 ]
                jb      @@key_num
                cmp     ah, 13h         ; [ Enter ]
                je      @@key_enter
                cmp     ah, 0Ah         ; [ BackSpace ]
                je      @@key_backSpace
                jmp     @@exit
@@key_num:      cmp     BL_numPos, 7
                jae     @@exit
                movzx   edx, ah
                mov     cl, BL_numPos
                inc     cl
                shl     cl, 2
                ror     edx, cl
                or      BL_number, edx
                mov     es, ss:SEG_FS
                mov     di, _pos_BL_inNum
                movzx   cx, BL_numPos
                shl     cx, 1
                add     di, cx
                mov     al, ah
                add     al, '0'
                mov     ah, cBLUE*10h + clWHITE
                stosw
                inc     BL_numPos
                jmp     @@exit
@@key_backSpace:cmp     BL_numPos, 0
                je      @@exit
                mov     edx, NOT 1111b
                mov     cl, BL_numPos
                shl     cl, 2
                ror     edx, cl
                and     BL_number, edx
                dec     BL_numPos
                mov     es, ss:SEG_FS
                mov     di, _pos_BL_inNum
                movzx   cx, BL_numPos
                shl     cx, 1
                add     di, cx
                mov     al, '░'
                mov     ah, cBLUE*10h + clCYAN
                stosw

                mov     si, offset BL_status_inNum
                mov     di, _pos_BL_status
                mov     ah, _color_log
                call    printS
                jmp     @@exit

@@key_enter:    cmp     BL_numPos, 7
                jne     @@exit
                mov     si, offset BL_status_Find
                mov     di, _pos_BL_status
                mov     ah, _color_log
                call    printS

                mov     ebx, BL_number
                DB      9AH
                DW      OFFSET Find_Black_tel_Scr
                DW      78h     ;cs
                cmp     ebx, 0ffffffffh
                je      @@no_find

                mov     si, offset BL_result_OK
                mov     di, _pos_BL_status
                mov     ah, _color_log
                call    printS

                push    ebx
                ; !! выводим правую часть
                mov     cx, 1                   ;\
                mov     di, _pos_BL_rules       ;|  выведем № правила
                mov     ah, _color_log          ;|
                test    bl, 01111b              ;|
                jnz     @@no_color_red          ;|
                mov     ah, _color_red          ;|
@@no_color_red: call    printH                  ;/
                mov     cx, 7                   ;\
                shr     ebx, 4                  ;|
                mov     di, _pos_BL_telef       ;|  выведем № правила
                call    printH                  ;/
                pop     ebx

                push    FS
                mov     FS, SS:SEG_BLK_LST          ; сегмент черного списка
                mov     esi, 80000h - (4+16*16*4)   ; Начнем с конца 512kb
                and     ebx, 0fh                    ; на всякий случай
                shl     ebx, 6                      ; *16*4
                add     esi, ebx

                ; теперь esi указывает на правило
                mov     di, _pos_BL_num_rules
                mov     cx, 4
@@print_BL_NUMS_1:
                push    cx
                mov     cx, 4
@@print_BL_NUMS_2:
                lods    DWORD PTR FS:[ESI]
                mov     ebx, eax
                mov     ah, _color_log
                call    printBL
                add     di, 10*_x
                loop    @@print_BL_NUMS_2
                pop     cx
                add     di, 40*_x
                loop    @@print_BL_NUMS_1
                pop     FS
                jmp     @@exit

@@no_find:      mov     si, offset BL_result_NO
                mov     di, _pos_BL_status
                mov     ah, _color_red;_color_log
                call    printS
                jmp     @@exit

@@exit:         pop     es
                pop     ds
                popad
                ret
Test_number_BL  ENDP

; MrAlex - редакти-е битов настр-ки
CHANGE_PROG_FLAGS:
                cmp     Pass_OK, 0
                jne     SORM_Emul_init
                ; здесь надо вывести окно мое и перевести SOST_VIU5 в режим скажем 8(доп-ть табл-цы записью)
                lea     si, CHNG_PFLG_WND0
                mov     di, _pos_PO_OBS
                mov     ah,  _SM_WIN_COLOR
                call    printSscr
                ; инициализация
                mov     ax, FG_PROGRAM              ; Текущее состояние программы
                mov     _CHNG_TMP_VAL, ax           ; во временный буфер!
                MOV     SOST_VIU5,8
_CHANGE_PRG_FLGS_end:
                ret
; end MrAlex - редакти-е битов настр-ки


; MrAlex********************* редактирование флагов программы - клавиши
RED_CHNG_PRGFLGS        PROC
                cmp     ah, 16h                 ; [ F1 ]
                Je      @@view_help_change_prg
                CMP     AH, 13H                 ; ввод
                JE      @@Est_CHNG_PRGFLGS
                CMP     AH, 15H                 ; вниз
                JE      @@Dwn_CHNG_PRGFLGS
                CMP     AH, 14H                 ; вверх
                JE      @@Up__CHNG_PRGFLGS
                CMP     AH, 11H                 ; влево
                JE      @@Lft_CHNG_PRGFLGS
                CMP     AH, 12H                 ; вправо
                JE      @@Rht_CHNG_PRGFLGS
                RET
@@view_help_change_prg:
                mov     si, offset Help_Win_Change_Prg  ;\
                mov     ah, _SM_WIN_COLOR               ;|
                mov     di, 3 * _y                      ;| вывод справки
                call    printSscr
                ret
@@Lft_CHNG_PRGFLGS:                     ; Влево
                dec     _CHNG_CUR_BIT
                and     _CHNG_CUR_BIT, 0fh
                RET
@@Rht_CHNG_PRGFLGS:                     ; вправо
                inc     _CHNG_CUR_BIT
                and     _CHNG_CUR_BIT, 0fh
                RET
@@Up__CHNG_PRGFLGS:                     ; Вверх
@@Dwn_CHNG_PRGFLGS:                     ; вниз  - одно и то же
                mov     ax, _CHNG_TMP_VAL
                mov     cx, _CHNG_CUR_BIT
                ror     ax, cl
                xor     ax, 1
                rol     ax, cl
                mov     _CHNG_TMP_VAL, ax
                RET
@@Est_CHNG_PRGFLGS:
                ; сохраним и выйдем
                mov     ax, _CHNG_TMP_VAL
                mov     FG_PROGRAM, ax
                CALL    INIC_W_VN               ; перерисуем оба окна
                RET
RED_CHNG_PRGFLGS        ENDP
; end MrAlex************


; --- запрос конфигурации, рисуем окно --------------------------------------
Begin_Cfg_init_win      PROC
                ; --- выведем окно
                mov     si, offset Begin_Cfg_win
                mov     di, _pos_reboot_win - 8
                test    FG_PROGRAM, _fg_prg_ck0
                jz      @@draw_win
                mov     si, offset Begin_Cfg_win_CK0
                mov     di, _pos_reboot_win - 12
@@draw_win:     mov     cx, 3
                mov     ah, _color_reboot_win
                call    printWin
                MOV     SOST_VIU5, _Begin_Cfg_kla
                ret
Begin_Cfg_init_win      ENDP


; --- запрос конфигурации, обработка клавиш ---------------------------------
Begin_Cfg_kla   proc
                cmp     ah, 59h         ; [ Y ]
                je      @@yes           ; делаем
                cmp     ah, 4Eh         ; [ N ]
                je      @@no            ; на отмену
                ret
		; ---
@@yes:          cmp	Cfg_Run, 0
		jne	@@exit
		mov	Cfg_Run, 255
                DB      9AH
                DW      OFFSET StartConfigFar
                DW      120
		; ---
@@no:           mov     SOST_VIU5, 0            ;\
                mov     ah, _color_normal_f     ;|
                lea     si, DnWin               ;|
                mov     di, _pos_DnWin          ;| переинициализируем часть верхнего окна
                call    printSscr               ;/
@@exit:         ret
Begin_Cfg_kla   endp

; --- останов конфигурации, рисуем окно --------------------------------------
End_Cfg_init_win        PROC
                ; --- выведем окно
                mov     si, offset End_Cfg_win
                mov     di, _pos_reboot_win - 12
                test    FG_PROGRAM, _fg_prg_ck0
                jz      @@draw_win
                mov     si, offset End_Cfg_win_CK0
                mov     di, _pos_reboot_win - 22
                ; ---
@@draw_win:     mov     cx, 3
                mov     ah, _color_reboot_win
                call    printWin
                mov     SOST_VIU5, _End_Cfg_kla
                ret
End_Cfg_init_win        ENDP

; --- останов конфигурации, обработка клавиш ---------------------------------
End_Cfg_kla     proc
                cmp     ah, 59h         ; [ Y ]
                je      @@yes           ; делаем
                cmp     ah, 4Eh         ; [ N ]
                je      @@no            ; на отмену
                ret
		; ---
@@yes:          cmp     Cfg_Run, 0
		je      @@exit
		movzx	si, Cfg_Curr_CK
		mov	Cfg_CK_Flags[si], 0
		mov	Cfg_CK_Errs[si], ce_TN_FAULT
		mov	Cfg_Run, 0
		mov	Cfg_Show_CK, -1
		mov	Cfg_Show_Blk, -1
		; ---
                test    FG_PROGRAM, _fg_prg_ck0
                jz      @@no_ck0
		mov	Cfg_Stop, 255
                DB      9AH
                DW      OFFSET StartConfigFar
                DW      120
                jmp     @@no
                ; ---
@@no_ck0:       mov     dl, 0ffh        ; конфигурация остановлена ручками !!!
                DB      9AH
                DW      OFFSET SendStopConfigFar
                DW      120
		; ---
@@no:           mov     SOST_VIU5, 0            ;\
                mov     ah, _color_normal_f     ;|
                lea     si, DnWin               ;|
                mov     di, _pos_DnWin          ;| переинициализируем часть верхнего окна
                call    printSscr               ;/
@@exit:         ret
End_Cfg_kla     endp


; --- выход в DOS, рисуем окно ------------------------------------------------
DOS_init_win    PROC
                ; --- выведем окно
                mov     si, offset DOS_win
                mov     di, _pos_reboot_win
                mov     cx, 3
                mov     ah, _color_reboot_win
                call    printWin
                mov     SOST_VIU5, _DOS_kla
                ret
DOS_init_win    ENDP

; --- перезагрузка обработка клавиш --------------------------------------------
DOS_kla         proc
                cmp     ah, 59h         ; [ Y ]
                je      @@DOS           ; на перезагрузку
                cmp     ah, 4Eh         ; [ N ]
                je      @@no_DOS        ; на отмену перезагрузки
                ret
@@no_DOS:       call    INIC_W_VN       ; выйдем из перезагрузки блока
                ret
@@DOS:          mov     DOS_Flag, 1
                DB      0EAh                            ;\
                DW      OFFSET reboot                   ;|JMP FAR CS:reboot
                DW      120                             ;/
                ret
DOS_kla          endp


; --- перезагрузка, рисуем окно ------------------------------------------------
Reboot_init_win PROC
                ; --- выведем окно
                mov     si, offset reboot_win
                mov     di, _pos_reboot_win
                mov     cx, 3
                mov     ah, _color_reboot_win
                call    printWin
                mov     SOST_VIU5, _Reboot_Programm
                ret
Reboot_init_win ENDP

; --- перезагрузка обработка клавиш --------------------------------------------
Reboot_kla      proc
                cmp     ah, 59h         ; [ Y ]
                je      @@reboot        ; на перезагрузку
                cmp     ah, 4Eh         ; [ N ]
                je      @@no_reboot     ; на отмену перезагрузки
                ret
@@no_reboot:    call    INIC_W_VN       ; выйдем из перезагрузки блока
                ret
@@reboot:       
if _MSP NE TRUE
if _debugFile	
                DB      9Ah
                DW      OFFSET close_debug_file_far
                DW      120
endif
endif
		mov     DOS_Flag,0
                DB      0EAh                            ;\
                DW      OFFSET reboot                   ;|JMP FAR CS:reboot
                DW      120                             ;/
                ret
Reboot_kla      endp

; --- перезагрузка СОРМ, рисуем окно ------------------------------------------------
X25_init_win    PROC
                ; --- выведем окно
                mov     si, offset x25_win
                mov     di, _pos_reboot_win
                mov     cx, 3
                mov     ah, _color_reboot_win
                call    printWin
                mov     SOST_VIU5, _X25_Reset
                ret
X25_init_win    ENDP

; --- перезагрузка СОРМ, обработка клавиш --------------------------------------------
X25_kla         proc
                cmp     ah, 59h         ; [ Y ]
                je      @@reboot        ; на перезагрузку
                cmp     ah, 4Eh         ; [ N ]
                je      @@no_reboot     ; на отмену перезагрузки
                ret
@@reboot:       mov     X25_Reset, 4000h
@@no_reboot:    call    INIC_W_VN       ; выйдем из перезагрузки СООРМа
                ret
X25_kla         endp


; ----=> EditPrefixGo <=-----------------------
; Created by <Yu> on 24/11/05
; инициализирует окно ввода префиксов
EditPrefixGo    PROC
                ; --- вывод самого окна
                mov     si, offset Prefix_Win
                mov     di, _pos_Win_Prefix
                mov     ah, _color_Win_Prefix
                push    ds
                mov     ds, ss:Seg_PCM
                call    printS
                pop     ds
                ; --- настраиваем рабочие переменные окна и диспетчера окон
                mov     Pref_Str_Curr, 0
                mov     sost_viu5, _Edit_Prefix_Idx
                ; --- рисуем сами префиксы
                call    printPrefixes
                ret
EditPrefixGo    ENDP

                ; --- отображение префиксов на экране
printPrefixes   PROC
                xor     si, si                  ; начинаем с 1-го префикса (МГ)
prnP_Loop:      mov     ah, _color_Win_Pref_Edit; цвет для выбранного префикса
                cmp     si, Pref_Str_Curr       ; а он выбран ?
                jz      prnP_Print              ; ДА -
                mov     ah, _color_Win_Prefix   ; НЕТ - меняем цвет на фон окна
prnP_Print:     shl     si, 1                   ; все данные - DW
                mov     di, Pref_Str_Beg[si]    ; с этой точки экрана
                ; чистим
                push    si                      ; \
                lea     si, Pref_Str_Clr        ; | сначала очистим
                call    printS                  ; | соответствующую строку
                pop     si                      ; /
                ; получаем префикс
                mov     bx, Prefixes[si]
                ; выделяем и проверяем длину префикса
                mov     cx, bx
                and     cx, 0fh
                jcxz    prnP_Next
                cmp     cl, 3
                ja      prnP_Next
                ; и рисуем цифирька за цифирькой
prnP_Draw:      shr     bx, 4
                push    cx
                mov     cx, 1
                call    printH
                pop     cx
                add     di, 2
                loop    prnP_Draw
prnP_Next:      shr     si, 1                   ;
                inc     si                      ;
                cmp     si, _PREF_MAX + _SS_MAX ;
                jb      prnP_Loop                 ;
                ret
printPrefixes   ENDP

; --- обработчик клавиш для окна редактирования префиксов
Prefix_kla      PROC
                cmp     ah, 10
                jb      Pre_kla_Digits
                je      Pre_kla_Del
                cmp     ah, 13h
                je      Pre_kla_Enter
                cmp     ah, 14h
                je      Pre_kla_Up
                cmp     ah, 15h
                je      Pre_kla_Dn
                ret

                ; --- Enter
Pre_kla_Enter:  call    Inic_W_VN
                call    SAVE_CONFIGUR
                ret

                ; --- удаление цифры
Pre_kla_Del:    mov     si, Pref_Str_Curr       ; \ получим индекс для работы
                shl     si, 1                   ; / с текущим префиксом
                ; получаем префикс
                mov     cx, Prefixes[si]
                ; выделяем и проверяем длину префикса
                and     cx, 0fh
                jcxz    dP_Exit
                cmp     cl, 3
                ja      dP_Exit
                ; просто уменьшаем тетраду длины
                dec     Prefixes[si]
                jmp     printPrefixes
dP_Exit:        ret

                ; --- ввод цифр
Pre_kla_Digits: mov     si, Pref_Str_Curr       ; \ получим индекс для работы
                shl     si, 1                   ; / с текущим префиксом
                ; получаем префикс
                mov     cx, Prefixes[si]
                ; выделяем и проверяем длину префикса
                and     cx, 0fh
                cmp     cl, 3
                je      iP_Exit
                jb      iP_Inc
                mov     Prefixes[si], 0
                xor     cx, cx
                ; просто увеличиваем тетраду длины
iP_Inc:         inc     Prefixes[si]
                ; чистим место под новую цифру
                shl     cx, 2
                mov     dx, 0ff0fh
                rol     dx, cl
                and     Prefixes[si], dx
                ; и наконец вписываем новую цифру
                xor     dx, dx
                mov     dh, ah
                shr     dx, 4
                shl     dx, cl
                or      Prefixes[si], dx
                ; перерисуем
                jmp     printPrefixes
iP_Exit:        ret

                ; --- стрелка вверх
Pre_kla_Up:     dec     Pref_Str_Curr
                jns     Pre_kla_Show
                mov     Pref_Str_Curr, _PREF_MAX + _SS_MAX - 1
                jmp     Pre_kla_Show
                ; --- стрелка вниз
Pre_kla_Dn:     inc     Pref_Str_Curr
                cmp     Pref_Str_Curr, _PREF_MAX + _SS_MAX
                jb      Pre_kla_Show
                mov     Pref_Str_Curr, 0
Pre_kla_Show:   call    printPrefixes
                ret
Prefix_kla      ENDP




; --- контроль ТЧ, рисуем окно -------------------------------------------------
TC_Ctrl_init_win PROC
                ; --- выведем окно
                mov     si, offset TC_Ctrl_Win
                mov     di, _pos_TC
                mov     ah, _color_TC

                push    ds
                mov     ds, ss:SEG_PCM
                call    printS
                pop     ds

                mov     SOST_VIU5, _TC_Ctrl
                mov     TC_Step, 0
                ret
TC_Ctrl_init_win ENDP

; --- контроль ОЗУ коммутации, рисуем окно -------------------------------------
OZUK_init_win   PROC
                ; --- выведем окно
                mov     si, offset OZUK_Win
                mov     di, _pos_OZUK
                mov     ah, _color_OZUK

                push    ds
                mov     ds, ss:SEG_PCM
                call    printS
                pop     ds

                mov     SOST_VIU5, _OZUK
                mov     OZUK_Step, 0
                ret
OZUK_init_win   ENDP


; --- контроль ошибочных пакетов МО --------------------------------------------
bad_MO_init_win PROC
                ; --- выведем окно
                mov     si, offset bad_MO_Win
                mov     di, _pos_bad_MO
                mov     ah, _color_bad_MO

                push    ds
                mov     ds, ss:SEG_PCM
                call    printS
                pop     ds

                mov     SOST_VIU5, _bad_MO
                ret
bad_MO_init_win ENDP


; --- контроль одной линии, рисуем окно ----------------------------------------
OneLn_init_win  PROC
                ; --- выведем окно
                mov     si, offset OneLn_win
                mov     di, _pos_OneLn_win
                mov     cx, 3
                mov     ah, _color_OneLn_win
                call    printWin
                ; ---
                mov     SOST_VIU5, _OneLn    ;
                ; ---
                cmp     OneLn_count, _ONELN_MAX
                jne      OneLn_reset_2
draw_OneLn_num: mov     ah, _color_OneLn_win
                movzx   cx, OneLn_count
                jcxz    @@exit
                mov     di, _pos_OneLn_num
                xor     si, si
@@loop_2:       push    cx
                mov     bl, OneLn_Digits[si]
                mov     cx, 1
                call    printH
                pop     cx
                inc     si
                movzx   dx, OneLn_add_di[si]
                add     di, dx
                loop    @@loop_2
@@exit:         ret
OneLn_init_win  ENDP

; --- контроль одной линии, обработка клавиш -----------------------------------
OneLn_kla       PROC
                cmp     ah, 10
                jb      OneLn_input
                je      OneLn_Del
                cmp     ah, 59h         ; [ Y ]
                je      OneLn_set       ; установить
                cmp     ah, 4Eh         ; [ N ]
                je      OneLn_reset     ; отменить
OneLn_exit:     ret
                ; ---
OneLn_input:    cmp     OneLn_count, _ONELN_MAX
                jae     OneLn_exit
                movzx   bx, OneLn_Count
                mov     OneLn_digits[bx], ah
                inc     OneLn_count
                jmp     OneLn_redraw
                ; ---
OneLn_Del:      cmp     OneLn_count, 0
                je      OneLn_exit
                dec     OneLn_count
OneLn_redraw:   mov     ah, _color_OneLn_win
                mov     di, _pos_OneLn_num
                lea     si, str_OneLn_empty
                call    printS
                jmp     draw_OneLn_num
                ; ---
OneLn_set:      mov     OneLn_active,  255
                mov     OneLn_running, 0
                mov     al, 10
                mul     OneLn_Digits
                add     al, OneLn_Digits[1]
                mov     OneLn_CK, al
                mov     al, 10
                mul     OneLn_Digits[2]
                add     al, OneLn_Digits[3]
                movzx   bx, al
                shl     bx, 9
                mov     ax, 100
                movzx   cx, OneLn_Digits[4]
                mul     cx
                add     bx, ax
                mov     ax, 10
                movzx   cx, OneLn_Digits[5]
                mul     cx
                add     bx, ax
                movzx   cx, OneLn_Digits[6]
                add     bx, cx
                mov     OneLn_PhysNum, bx
                cmp     NOM_VERX_OKNA, _MO_Slct_Win
                jne     @@norm_set
                call    Inic_New_down
                jmp     @@exit_inic
  @@norm_set:   call    INIC_W_VN
  @@exit_inic:  ret
                ; ---
OneLn_reset:    cmp     NOM_VERX_OKNA, _MO_Slct_Win
                jne     @@norm_reset
                call    Inic_New_down
                jmp     OneLn_reset_2
 @@norm_reset:  call    INIC_W_VN
OneLn_reset_2:  mov     OneLn_active,  0
                mov     OneLn_running, 0
                mov     OneLn_count,   0
                ret
OneLn_kla       ENDP

; --- проверка тарификации
Check_Tar_init_win      PROC
                cmp     Pass_OK, 0
                je      @@exit
                mov     si, OFFSET Check_Tar_win
                mov     di, _pos_Check_Tar_win
                mov     cx, 11
                mov     ah, _color_Check_Tar
                call    printWin
                mov     SOST_VIU5, _Check_Tar_kla
@@exit:         ret
Check_Tar_init_win      ENDP

Check_Tar_kla   PROC
                ; --- шаблон тар. пакета копируем в буфер передачи
                push    ds
                pop     es
                mov     cx, 29
                lea     si, m_Pok_Tar
                lea     di, m_Pok_Send_Tar
                rep     movsb
                ; --- вписываем текущее время в пакет
                push    ax
                movzx   ax, SECUNDA
                shl     ax, 4
                shr     al, 4
                aad
                mov     m_Pok_Send_Tar[5], al
                movzx   ax, MIN
                shl     ax, 4
                shr     al, 4
                aad
                mov     m_Pok_Send_Tar[6], al
                movzx   ax, CHAS
                shl     ax, 4
                shr     al, 4
                aad
                mov     m_Pok_Send_Tar[7], al
                movzx   ax, DEN_MES
                shl     ax, 4
                shr     al, 4
                aad
                mov     m_Pok_Send_Tar[8], al
                movzx   ax, MES
                shl     ax, 4
                shr     al, 4
                aad
                mov     m_Pok_Send_Tar[28], al
                ;
                dec     m_Pok_Send_Tar[7]
                pop     ax
                ; --- ставим длительность равной ah - чтоб потом проще было анализировать
                mov     m_Pok_Send_Tar[13], ah
                ; ---
                cmp     ah, 1
                je      Check_Tar_norma
                cmp     ah, 2
                je      Check_Tar_bad_Na
                cmp     ah, 3
                je      Check_Tar_bad_Nb
                cmp     ah, 4
                je      Check_Tar_bad_long
                cmp     ah, 5
                je      Check_Tar_future_call
                cmp     ah, 6
                je      Check_Tar_past_call
                cmp     ah, 7
                je      Check_Tar_bad_date
@@exit:         ret
                ; ---
Check_Tar_norma:
                jmp     Check_Tar_send
Check_Tar_bad_Na:
                mov     m_Pok_Send_Tar[20], 0abh
                jmp     Check_Tar_send
Check_Tar_bad_Nb:
                mov     m_Pok_Send_Tar[24], 0cdh
                jmp     Check_Tar_send
Check_Tar_bad_long:
                mov     WORD PTR m_Pok_Send_Tar[13], 0a301h
                mov     WORD PTR m_Pok_Send_Tar[15], 2
                jmp     Check_Tar_send
Check_Tar_future_call:
                mov     al, MES
                inc     al
                cmp     al, 13
                jne     @@future
                mov     al, 1
@@future:       mov     m_Pok_Send_Tar[28], al
                jmp     Check_Tar_send
Check_Tar_past_call:
                mov     al, MES
                dec     al
                cmp     al, 0
                jne     @@past
                mov     al, 12
@@past:         mov     m_Pok_Send_Tar[28], al
                jmp     Check_Tar_send
Check_Tar_bad_date:
                mov     m_Pok_Send_Tar[28], 13
                ;
Check_Tar_send: mov     bCheckTar, 255
                ret
Check_Tar_kla   ENDP


; --- отображение последней контрольной точки -------------------------------
Ctrl_Point_init PROC
                ; --- выведем окно
                mov     si, offset Ctrl_Point_win
                mov     di, _pos_reboot_win - 6
                mov     cx, 4
                call    printWin
                mov     SOST_VIU5, _Ctrl_Point
                ; ---
                push    fs
                mov     fs, ss:seg_baba
                mov     di, _pos_reboot_win - 6
                add     di, _y * 1 + _x * 11
                mov     cx, 2
                mov     si, 512 * _BABA_FOR_CP
                mov     bl, fs:[si + 14h]   ; год
                call    printH
                mov     bl, fs:[si + 15h]   ; месяц
                add     di, 6
                call    printH
                mov     bl, fs:[si + 16h]   ; день месяца
                add     di, 6
                call    printH
                mov     bl, fs:[si + 19h]   ; часы
                add     di, 6
                call    printH
                mov     bl, fs:[si + 18h]   ; минуты
                add     di, 6
                call    printH
                mov     bl, fs:[si + 17h]   ; секунды
                add     di, 6
                call    printH
                ; ---
                mov     di, _pos_reboot_win - 6
                add     di, _y * 2 + _x * 11
                mov     cx, 8
                mov     bh, fs:[si + 13h]   ;
                mov     bl, fs:[si + 12h]   ;
                shl     ebx, 16
                mov     bh, fs:[si + 11h]   ;
                mov     bl, fs:[si + 10h]       ;
                call    printH
                pop     fs
                ret
Ctrl_Point_init ENDP


