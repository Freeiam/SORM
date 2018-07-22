;;; ---------------------------------------------------------------------------
;;; --- формирование сообщений СОРМ - НАЧАЛО                                ---
;;; ---------------------------------------------------------------------------
;
; вход:         es:[si]         - ссылка на сеccию
;               dl              - код сообщения
;
Message_SORM    PROC
                push    es
                push    fs
                push    gs
                pushad
		; ---
                push    es
                pop     fs
                mov     gs, ss:SEG_OBJ
                mov     bx, fs:[si].sorm_obj
                ; --- вычисление начального адреса записи и разрешение записи (POS_SOOB++)
		mov	es, ss:SEG_COM
                xor     edi, edi
                mov     di, POS_SOOB
                mov     ax, di
                inc     ax
                and     ax, M_SOOB
                cmp     SLED_SOOB, ax
                je      @@exit
		cmp	SORM_Stoped, 0
                jne     @@exit
                mov     POS_SOOB, ax
		; ---
                shl     edi, 6
                add     edi, 10000h + 2
                mov     ax, PREAMB_NOM_SORM
                @_STOSW                         ; преамбула и номер СОРМ
                mov     al, dl
                mov     ah, 36
                @_STOSW                         ; код сообщения и его длина
                ; ---
                mov     ax, fs:[si].sorm_call
                @_STOSW                         ; номер вызова
                mov     al, gs:[bx].obj_type
                @_STOSB                         ; тип объекта
                mov     ax, gs:[bx].obj_num
                @_STOSW                         ; условный номер объекта
                mov     al, fs:[si].sorm_get_num
                @_STOSB                         ; признак отбора номера
                ; ---
		push	bx
                movzx   bx, fs:[si].sorm_attr_a
                test    fs:[si].sorm_flags, ssf_SWAP_AB
                jz      @@no_swap_ab
                movzx   bx, fs:[si].sorm_attr_b
                jmp     @@no_swap_ac
@@no_swap_ab:   test    fs:[si].sorm_flags, ssf_SWAP_AC
                jz      @@no_swap_ac
                movzx   bx, fs:[si].sorm_attr_c
@@no_swap_ac:   and     bl, 7
                movzx   ax, fs:[si].sorm_attr_b
                test    fs:[si].sorm_flags, ssf_SWAP_BA
                jz      @@no_swap_ba
                movzx   ax, fs:[si].sorm_attr_a
                jmp     @@no_swap_bc
@@no_swap_ba:   test    fs:[si].sorm_flags, ssf_SWAP_BC
                jz      @@no_swap_bc
                movzx   ax, fs:[si].sorm_attr_c
@@no_swap_bc:   and     al, 7
                shl     ax, 3
                or      bx, ax
                mov     al, SORM_Link_Prms[bx]
                @_STOSB                         ; параметры связи
		pop	bx
		; ---
                xor     al, al
                cmp     dl, SO_DVO
                jne     @@no_dvo_1
                mov     al, fs:[si].sorm_dvo_phase
@@no_dvo_1:     @_STOSB                         ; нет ДВО
                ; === запись номеров абонентов ================================
                ; --- запись А - вызывающего
@@num_1:        mov     bp, sorm_attr_a
                test    fs:[si].sorm_flags, ssf_SWAP_AB
                jz      @@num_1_
                mov     bp, sorm_attr_b
                ; --- {Paul.N] --- <29/04/13>
                test    fs:[si].sorm_flags, ssf_TCOP_UPATS
                jz      @@num_1_wr
                ; --- Добавим цифру выхода в ТСОП для - УПАТС ---
                pushf
                push    ax bx cx bp
                mov     cx, 11
                cld
                mov     bl, _SORM_SIZE                  ;\
                mov     ax, si                          ; |> по сессии найдем цифру выхода
                div     bl                              ; |> в ТСОП
                movzx   bx, al                          ; |>
                mov     bl, UPATS_TCOP_dig[bx]          ;/
                lea     bp, Tmp_UPATS_Buff
                push    bp si
   @@tmp:       mov     al, BYTE PTR fs:[si].sorm_attr_b
                mov     ds:[bp], al
                inc     bp
                inc     si
                loop    @@tmp
                pop     si bp
                mov     cx, 4
   @@add_9:     stc
                rcl     DWORD PTR ds:[bp + 2],  1
                rcl     DWORD PTR ds:[bp + 6],  1
                rcl     BYTE  PTR ds:[bp + 10], 1
                loop    @@add_9
                mov     al, BYTE PTR ds:[bp + 2]
                and     al, 0F0h
                or      al, bl                          ; добавим цифру
                mov     ds:[bp + 2], al
                inc     BYTE PTR ds:[bp + 1]            ; length +1
                ; --- перенесем полученные данные обратно в сессию
                mov     eax, DWORD PTR ds:[bp]
                @_STOSD
                mov     eax, DWORD PTR ds:[bp + 4]
                @_STOSD
                mov     ax,  WORD PTR ds:[bp + 8]
                @_STOSW
                mov     al, BYTE PTR ds:[bp + 10]
                @_STOSB
                pop     bp cx bx ax
                popf
          ; --- <Paul> ---- (END) ---
                jmp     @@num_2
                ; -----             -------
@@num_1_:       test    fs:[si].sorm_flags, ssf_SWAP_AC
                jz      @@num_1_wr
                mov     bp, sorm_attr_c
@@num_1_wr:     mov     eax, DWORD PTR fs:[si + bp]
                @_STOSD
                mov     eax, DWORD PTR fs:[si + bp + 4]
                @_STOSD
                mov     eax, DWORD PTR fs:[si + bp + 8]
                @_STOSW
		shr	eax, 16
		@_STOSB
                ; --- запись B - вызываемого
@@num_2:        mov     bp, sorm_attr_b
                test    fs:[si].sorm_flags, ssf_SWAP_BA
                jz      @@_num_2
                mov     bp, sorm_attr_a
                jmp     @@num_2_wr
@@_num_2:       test    fs:[si].sorm_flags, ssf_SWAP_BC
                jz      @@num_2_wr_1
                mov     bp, sorm_attr_c
                jmp     @@num_2_wr
                ; --- {Paul.N] --- <29/04/13>
  @@num_2_wr_1: test    fs:[si].sorm_flags, ssf_TCOP_UPATS
                jz      @@num_2_wr
                ; --- Добавим цифру выхода в ТСОП для - УПАТС ---
                pushf
                push    ax bx cx bp
                mov     cx, 11
                cld
                mov     bl, _SORM_SIZE                  ;\
                mov     ax, si                          ; |> по сессии найдем цифру выхода
                div     bl                              ; |> в ТСОП
                movzx   bx, al                          ; |>
                mov     bl, UPATS_TCOP_dig[bx]          ;/
                lea     bp, Tmp_UPATS_Buff
                push    bp si
   @@_tmp:      mov     al, BYTE PTR fs:[si].sorm_attr_b
                mov     ds:[bp], al
                inc     bp
                inc     si
                loop    @@_tmp
                pop     si bp
                mov     cx, 4
   @@_add_9:    stc
                rcl     DWORD PTR ds:[bp + 2],  1
                rcl     DWORD PTR ds:[bp + 6],  1
                rcl     BYTE  PTR ds:[bp + 10], 1
                loop    @@_add_9
                mov     al, BYTE PTR ds:[bp + 2]
                and     al, 0F0h
                or      al, bl                          ; добавим цифру
                mov     ds:[bp + 2], al
                inc     BYTE PTR ds:[bp + 1]            ; length +1
                ; --- перенесем полученные данные обратно в сессию
                mov     eax, DWORD PTR ds:[bp]
                @_STOSD
                mov     eax, DWORD PTR ds:[bp + 4]
                @_STOSD
                mov     ax,  WORD PTR ds:[bp + 8]
                @_STOSW
                mov     al, BYTE PTR ds:[bp + 10]
                @_STOSB
                pop     bp cx bx ax
                popf
          ; --- <Paul> ---- (END) ---
                jmp     @@num_end
@@num_2_wr:
                mov     eax, DWORD PTR fs:[si + bp]
                @_STOSD
                mov     eax, DWORD PTR fs:[si + bp + 4]
                @_STOSD
                mov     eax, DWORD PTR fs:[si + bp + 8]
                @_STOSW
		shr	eax, 16
		@_STOSB
@@num_end:      ; === конец записи номеров абонентов ==========================
                ; --- запись пучка
		push	fs
		push	ebx
		push	edx
                ; --- № пучка, если он задан в Tranks шлём всегда
                mov     ax, 0ffffh              ; пока пучка нет
                movzx   ebx, fs:[si].sorm_ck_a  ;
                shl     ebx, _PHYS_NUM_SHL      ;
                or      bx, fs:[si].sorm_phys_a ;
                mov     fs, ss:SEG_BUNCH        ;
                movzx   edx, BYTE PTR fs:[ebx]  ;
                cmp     dl, _BUNCH_MAX          ;
                ja      @@send_bunch            ;
                or      dl, dl                  ;
                jz      @@send_bunch            ;
                ; --- линия входит в пучок - получаем условный номер пучка
                mov     ebx, _BUNCH_PRM_OFFS    ;
		dec	dl			;
                shl     edx, _BUNCH_PRM_SHL     ;
                add     ebx, edx                ;
                mov     edx, fs:[ebx]           ;
                cmp     dl, 0ffh                ;
                je      @@send_bunch            ;
                shr     edx, 8                  ;
		mov	ax, dx			;
		;
@@send_bunch:   @_STOSW
		pop	edx
		pop	ebx
		pop	fs
		; ---
                mov     ax, WORD PTR fs:[si].sorm_ksl_a
                @_STOSW                         ; номера подключаемых КСЛ
                ; ---
		push	dx
		MOV	AL,DEN_MES
		MOV	DL,AL
		SHR	AL,4
		SHL	DL,4
		OR	AL,DL
		@_STOSB
		MOV	AL,DEN_MES[3]
		MOV	DL,AL
		SHR	AL,4
		SHL	DL,4
		OR	AL,DL
		@_STOSB
		MOV	AL,DEN_MES[2]
		MOV	DL,AL
		SHR	AL,4
		SHL	DL,4
		OR	AL,DL
		@_STOSB
		MOV	AL,DEN_MES[1]
		MOV	DL,AL
		SHR	AL,4
		SHL	DL,4
		OR	AL,DL
		@_STOSB
		pop	dx
		; ---
                mov     al, fs:[si].sorm_prior
                @_STOSB                         ; метка приоритета
                mov     al, fs:[si].sorm_opcode
                @_STOSB                         ; код операции
                mov     ax, 0ffffh              ;
                @_STOSW
                cmp     dl, SO_DVO              ;
                jne     @@no_dvo_2              ;
                mov     al, fs:[si].sorm_dvo_code;
@@no_dvo_2:     @_STOSW                         ; описание услуги ДВО
                ; --- вписываем в готовый пакет его длину
@@close_packet: MOV     EAX,EDI
                AND     EDI,0FFFFFFC0H
                SUB     EAX,EDI
                SUB     EAX,2
                @_STOSW
                ; ---
IF _MSP EQ TRUE
                test    FG_PROGRAM, M_OR_8      ; если включена отладка 2-го (статистического)
                jz      @@exit                  ; канала СОРМ
;cmp __inf_y, __INF_STR_COUNT * 160 - 160
;jae @@exit
                mov     ebp, edi
                ; маркер вывода СОРМа
                mov     ah, cBLUE * 16 + clWHITE
                lea     si, __sorm_2_str
                mov     di, __inf_y
                call    print_S
                ; первые 38 байт сообщения
                mov     cx, 38
                mov     di, __inf_y
                add     di, 2
@@print_head:   push    cx
                mov     bl, es:[ebp]
                mov     cx, 2
                call    print_H
                pop     cx
                inc     ebp
                add     di, 4
                loop    @@print_head
                add     __inf_y, 160
                cmp     __inf_y, __INF_STR_COUNT * 160
                jb      @@print_pre_tail
                mov     __inf_y, 0
                ; перешли на новую строку
@@print_pre_tail:
                lea     si, __inf_clear
                mov     di, __inf_y
                call    print_S
                ; последние 10 байт сообщения
                lea     si, __sorm_empty_str
                mov     di, __inf_y
                call    print_S
                mov     cx, 10
                mov     di, __inf_y
                add     di, 2
@@print_tail:   push    cx
                mov     bl, es:[ebp]
                mov     cx, 2
                call    print_H
                pop     cx
                inc     ebp
                add     di, 4
                loop    @@print_tail
                add     __inf_y, 160
                cmp     __inf_y, __INF_STR_COUNT * 160
                jb      @@print_empty
                mov     __inf_y, 0
                ; чистим следующую строку
@@print_empty:  lea     si, __inf_clear
                mov     di, __inf_y
                call    print_S
ENDIF
@@exit:         ; ---
                popad
                pop     gs
                pop     fs
                pop     es
                ret
Message_SORM    ENDP

;;; ---------------------------------------------------------------------------
;;; --- формирование сообщений СОРМ - НАЧАЛО                                ---
;;; ---------------------------------------------------------------------------
;

