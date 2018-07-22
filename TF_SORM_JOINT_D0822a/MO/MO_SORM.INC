;;; ---------------------------------------------------------------------------
;;; --- процедуры СОРМ - НАЧАЛО                                             ---
;;; ---------------------------------------------------------------------------
;
; --- срабатывание будильников
; вход:         ds:[bp] - указатель на 123 с кодом ДВО 37h или 38h
;
DVO_Alarm       PROC
                ; --- вычисляем указатель на конфигурацию
                mov     gs, ss:SEG_KONF            ;
                movzx   edi, BYTE PTR ds:[bp + 1]  ;№ ЦК
                shl     edi, 4                     ; 16 блоков в ЦК
                movzx   ax, ds:[bp + 3]            ; \ номер
                shr     ax, 1                      ; / блока
                and     ax, 0fh                    ;
                or      di, ax                     ; № ЦК и № блока вместе
                shl     edi, 3                     ; 8 байт на конфигурацию блока ЦК
                mov     edi, gs:[edi + 1]          ; адрес линий блока
                cmp     edi, Cfg_Free_Ptr          ;
                jae     @@exit                     ;
                movzx   eax, WORD PTR ds:[bp + 2]  ;
                and     eax, 1ffh                  ;
                shl     eax, 4                     ;
                add     edi, eax                   ;
                cmp     edi, Cfg_Free_Ptr          ;
                jae     @@exit                     ;
                ; --- получаем номер А
                mov     eax, gs:[edi + 10]         ;
                rol     al, 4                      ;
                rol     ah, 4                      ;
                rol     eax, 16                    ;
                rol     al, 4                      ;
                rol     ah, 4                      ;
                rol     eax, 16                    ;
                ; --- смотрим наблюдается ли абонент
                mov     cx, Obj_Count              ;
                or      cx, cx                     ;
                jz      @@exit                     ;
                lea     bx, Objects                ;
                mov     gs, ss:SEG_OBJ             ;
                ; --- цикл поиска
@@find_ab:      cmp     WORD PTR gs:[bx].obj_type, 0ffffh
                je      @@next_ab                  ;
		cmp	eax, DWORD PTR gs:[bx].obj_tel_num
                je      @@found_ab                 ;
@@next_ab:      add     bx, _OBJ_SIZE              ;
                loop    @@find_ab                  ;
                jmp     @@exit                     ; абонент не на контроле
@@found_ab:     ; --- нашли абоненнта, искуственно создаём сессию
                mov     es, ss:SEG_SORM            ;
                xor     si, si                     ;
                mov     cx, _SESSION_MAX           ;
@@find_ses:     test    es:[si].sorm_flags, ssf_REL
                jnz     @@found_ses                ;
                add     si, _SORM_SIZE             ;
                loop    @@find_ses                 ;
                jmp     @@exit                     ; невозможно создать сессию
@@found_ses:    ; --- флаги - сразу завершение сессии
                mov     es:[si].sorm_flags, ssf_REL;
                ; --- ссылка на объект
                mov     es:[si].sorm_obj, bx       ;
                ; --- №а
                mov     es:[si].sorm_attr_a, PRIZN_STAN
                mov     dl, LENGTH_NUM             ;
                mov     es:[si].sorm_len_a, dl     ;
                mov     DWORD PTR es:[si].sorm_num_a,     eax
                mov     DWORD PTR es:[si + 4].sorm_num_a, 0ffffffffh
                mov     es:[si + 8].sorm_num_a,           0ffh
                ; --- №б
                mov     DWORD PTR es:[si].sorm_attr_b,    0ffffffffh
                mov     DWORD PTR es:[si + 4].sorm_attr_b,0ffffffffh
                mov     WORD PTR es:[si + 8].sorm_attr_b, 0ffffh
                ; --- номер вызова
                inc     SORM_Curr_Call             ;
                mov     ax, SORM_Curr_Call         ;
                mov     es:[si].sorm_call, ax      ;
                ; --- признак отбора номера
                mov     al, gs:[bx].obj_get_num    ;
                mov     es:[si].sorm_get_num, al   ;
                ; --- прочие параметры
                mov     al, gs:[bx].obj_prior      ;
                mov     es:[si].sorm_prior, al     ;
                mov     es:[si].sorm_opcode, 0     ;
                mov     es:[si].sorm_dvo_phase, PHASE_USE_DVO
                mov     al, ds:[bp + 4]            ;
                mov     es:[si].sorm_dvo_code, al  ;
		mov	WORD PTR es:[si].sorm_ksl_a, 0ffffh
                ; --- сормим !
                mov     dl, SO_DVO                 ;
                call    Message_SORM               ;
                ; ---
@@exit:         ret
DVO_Alarm       ENDP

;
; --- установка/отмена ДВО с клавиатуры телефона
; вход:         ds:[bp] - указатель на 235/150
;
DVO_SR          PROC
                cmp     BYTE PTR ds:[bp + 11], 177 ; отмена ДВО ?
                je      @@continue                 ; ДА -
                cmp     BYTE PTR ds:[bp + 11], 172 ; заказ ДВО ?
                je      @@continue                 ; ДА -
                ; --- {Paul.N} --- <20/05/13>
                cmp     Byte ptr ds:[bp + 11], 176
                je      @@check_soed_dvo
                cmp     Byte ptr ds:[bp + 11], 178
                je      @@check_soed_dvo
                cmp     Byte ptr ds:[bp + 11], 179
                je      @@check_soed_dvo
                cmp     Byte ptr ds:[bp + 11], 180
                jne     @@exit
@@check_soed_dvo:
                ; --- получим указатель на соединение
                mov     edi, Call_Ptr
                ; --- отметим, чтобы не слать разъединение
                or      fs:[edi].soed_flags, sf_DVO_SR
                jmp     @@exit
                ; --- {Paul.N} --- <END>
@@continue:     ; --- вычисляем указатель на конфигурацию
                mov     gs, ss:SEG_KONF            ;
                movzx   edi, BYTE PTR ds:[bp + 5]  ;№ ЦК
                shl     edi, 4                     ; 16 блоков в ЦК
                movzx   ax, ds:[bp + 2]            ; \ номер
                shr     ax, 1                      ; / блока
                and     ax, 0fh                    ;
                or      di, ax                     ; № ЦК и № блока вместе
                shl     edi, 3                     ; 8 байт на конфигурацию блока ЦК
                mov     edi, gs:[edi + 1]          ; адрес линий блока
                cmp     edi, Cfg_Free_Ptr          ;
                jae     @@exit                     ;
                movzx   eax, WORD PTR ds:[bp + 1]  ;
                and     eax, 1ffh                  ;
;                shl     eax, 4                     ;
; MVT сделал обработку для конфигурации с 64 байтами на линию
                push    cx
		mov	cl, 4
                test    FG_PROGRAM, _fg_prg_multy_pref ; старая конфигурация ?
                jz      @@odnopref              ; ДА -
                push    bx
                movzx   bx, BYTE PTR gs:[edi]
                mov     cl, TAB_TIP_LN[bx]       ; и размер конф. линии в сдвигах
                pop	bx
@@odnopref:
                shl     eax, cl                  ; ? - байт на конфигурацию линии
                pop     cx

                add     edi, eax                   ;
                cmp     edi, Cfg_Free_Ptr          ;
                jae     @@exit                     ;
                ; --- разбираемся что за ДВО
                mov     dx, _CFU_                  ; CFU
		cmp     WORD PTR ds:[bp + 14], 102h;
                je      @@dvo                      ;
                mov     dx, _CFB_                  ; CFB
		cmp     WORD PTR ds:[bp + 14], 202h;
                je      @@dvo                      ;
                mov     dx, _CFNRY_                ; CFNRY
		cmp     WORD PTR ds:[bp + 14], 302h;
                je      @@dvo                      ;
		cmp     WORD PTR ds:[bp + 14], 106h;
                je      @@dvo                      ;
                mov     dx, _HT_                   ; горячий телефон
                cmp     WORD PTR ds:[bp + 14], 305h;
                je      @@dvo                      ;
                mov     dx, _OA_                   ; однократный будильник
                cmp     WORD PTR ds:[bp + 14], 505h;
                je      @@dvo                      ;
                mov     dx, _MA_                   ; многократный будильник
                cmp     WORD PTR ds:[bp + 14], 605h;
                je      @@dvo                      ;
                jmp     @@exit
@@dvo:          cmp     BYTE PTR ds:[bp + 11], 177 ; отмена ДВО ?
                je      @@reset_dvo                ; ДА -
@@set_dvo:      and     gs:[edi + 9], dl           ;
                mov     dl, PHASE_SET_DVO          ; заказ ДВО
                jmp     @@get_soed_ptr             ;
@@reset_dvo:    not     dl                         ;
                or      gs:[edi + 9], dl           ;
                mov     dl, PHASE_RESET_DVO        ; отмена ДВО
                ; --- получим указатель на соединение
@@get_soed_ptr: mov     edi, Call_Ptr              ; предварительно GetCallPtr !!!
                ; --- получим короткий номер абонента, установившего/отменившего ДВО
                mov     eax, DWORD PTR fs:[edi].num_a
                cmp     fs:[edi].len_a, 7          ;
                je      @@get_short_num            ;
		mov	eax, DWORD PTR fs:[edi + 1].num_a
                shr     eax, 4                     ;
@@get_short_num:movzx   si, LENGTH_NUM             ;
                and     si, 7                      ;
                mov     cl, ANI7_Shift[si]         ;
                shr     eax, cl                    ;
                shl     si, 2                      ;
                or      eax, ANI7_Mask[si]         ;
                ; --- готовимся искать полученный номер в списке наблюдаемых объектов
                mov     cx, Obj_Count              ;
                or      cx, cx                     ;
                jz      @@exit                     ;
                lea     bx, Objects                ;
                mov     gs, ss:SEG_OBJ             ;
                ; --- цикл поиска
@@find:         cmp     WORD PTR gs:[bx].obj_type, 0ffffh  ;
                je      @@next                     ;
		cmp	eax, DWORD PTR gs:[bx].obj_tel_num
                je      @@found                    ;
@@next:         add     bx, _OBJ_SIZE              ;
                loop    @@find                     ;
                ; --- ищем вызывающего из УПАТС в списке объектов
                test    fs:[edi].soed_flags, sf_UPATS_A
                jz      @@exit
                mov     eax, DWORD PTR fs:[edi].upats_a_num     ;
                push    cx si edx
                push    es
                call    Ux_Find                                 ;
                pop     es
                pop     edx si cx
                jnc     @@exit                              ; НЕ НАШЛИ
                ; --- Find! ->: bx -> sorm object ---
@@found:        ; --- чисто на всякий случай чистим поле №б
                mov     eax, 0ffffffffh            ;
		mov	DWORD PTR fs:[edi].attr_b, eax
		mov	DWORD PTR fs:[edi + 4].attr_b, eax
		mov	WORD  PTR fs:[edi + 8].attr_b, ax
                mov     fs:[edi + 10].attr_b, al   ;
                ; --- заполняем поле номера Б аль нет ?
                cmp     dl, PHASE_SET_DVO          ; установка ДВО ?
                jne     @@sorm                     ; НЕТ - поле №б оставляем пустым
                ; установка ДВО - есть доп. инфа ?
                movzx   cx, ds:[bp + 3]            ;
                sub     cx, 13 + 1                 ;
                js      @@exit                     ; НЕТ -
                jz      @@exit                     ; НЕТ -
                ; есть доп. инфа для передачи в поле номера Б
                cmp     dh, OA                     ; будильник ?
                je      @@alarms                   ; ДА -
                cmp     dh, MA                     ; будильник ?
                je      @@alarms                   ; ДА -
                ; не будильники
                cmp     cl, 18                     ; не много ли её ?
                jbe     @@set_len                  ; НОРМАЛЬНО -
                mov     cl, 18                     ; ОГРАНИЧИМ -
@@set_len:      mov     fs:[edi].len_b, cl         ;   длина доп. инфы в знаках
                shr     cx, 1                      ; \ а теперь
                adc     cx, 0                      ; / в байтах
                ; копируем доп. инфу в поле №б
                push    bp                         ;
                push    edi                        ;
@@num_b_loop:   mov     ax, ds:[bp + 17]           ;
                shl     al, 4                      ;
                shr     ax, 4                      ;
                mov     fs:[edi].num_b, al         ;
                add     bp, 2                      ;
                inc     edi                        ;
                loop    @@num_b_loop               ;
                pop     edi                        ;
                pop     bp                         ;
                ; --- для будильника поле признак номера не заполняем
                call    Num_B_Attr                 ;
                jmp     @@sorm                     ;
@@alarms:       ; --- спец случай - будильники
                mov     fs:[edi].len_b, 4          ;   длина доп. инфы в знаках
                mov     ax, ds:[bp + 17]           ;
                shl     al, 4                      ;
                shr     ax, 4                      ;
                mov     fs:[edi].num_b, al         ;
                mov     ax, ds:[bp + 20]           ;
                shl     al, 4                      ;
                shr     ax, 4                      ;
                mov     fs:[edi + 1].num_b, al     ;
@@sorm:         ; --- отправка сообщения: bx- ук.на оьъект, dl-пар-ры ДВО
                cmp     gs:[bx].obj_type, 0ffh
                je      @@only_UPATS
                push    dx                         ;
                xor     dl, dl                     ;
                mov     eax, calling_short_ptr     ;
                @Session_Create                    ;
                pop     dx                         ;
                mov     es:[si].sorm_dvo_code,  dh ;
                mov     es:[si].sorm_dvo_phase, dl ;
                push    dx
                mov     dl, SO_DVO                 ;
                call    Message_SORM               ;
                pop     dx
                or      es:[si].sorm_flags, ssf_REL;
                or      fs:[edi].soed_flags, sf_DVO_SR
                ; --- и здесь ищем вызывающего из УПАТС в списке объектов
                test    fs:[edi].soed_flags, sf_UPATS_A
                jz      @@exit
                mov     eax, DWORD PTR fs:[edi].upats_a_num     ;
                push    cx si edx
                push    es
                call    Ux_Find                                 ;
                pop     es
                pop     edx si cx
                jnc     @@exit                              ; НЕ НАШЛИ
                ; --- Find! ->: bx -> sorm object ---
@@only_UPATS:   push    dx                         ;
                xor     dl, dl                     ;
                mov     eax, Ua_called_ptr         ;
                @Session_Create                    ;
                xor     dl, dl
                call    Ux_Session
                pop     dx
                mov     es:[si].sorm_dvo_code,  dh ;
                mov     es:[si].sorm_dvo_phase, dl ;
                mov     dl, SO_DVO                 ;
                call    Message_SORM               ;
                or      es:[si].sorm_flags, ssf_REL;
                or      fs:[edi].soed_flags, sf_DVO_SR
@@exit:
                ret
DVO_SR          ENDP

;
; --- приём АОН для СОРМа - формирование признака №А
; вход:         fs:[edi]        - указатель на соединение
; выход:        fs:[edi].attr_a - новое значение
;
Num_A_Attr	PROC
                ; === вычисление признака номера А ======================================
                ; --- проверка № A на МН
NAA_MN:         mov     dl, PRIZN_MESHDUN       ; = 5
                cmp     fs:[edi].len_a, 10      ; ЗДЕСЬ МОЖЕТ БЫТЬ ЗАСАДА ДЛЯ ВСЯКИХ КАРЛИКОВ !!!
                ja      NAA_Attr                ;
                ; --- проверка на МГ
NAA_MG:         mov     dl, PRIZN_MESHGOR       ; = 4
		mov     ax, WORD PTR fs:[edi].num_a
                and     ax, PREF_A_ZN_MASK      ; ???
                cmp     ax, PREF_A_ZN           ; ???
                jne     NAA_Attr                ;
                ; --- проверка на зону
NAA_Zone:       mov     dl, PRIZN_ZONI          ; = 3
                movzx   bx, LENGTH_NUM          ;
                shl     bx, 2                   ;
                mov     eax, DWORD PTR fs:[edi].num_a
                and     eax, PREF_A_FULL_MASK[bx] ;
                push    edx
                mov     edx, PREF_A_FULL          ; ???xx
                and     edx, PREF_A_FULL_MASK[bx] ;
                cmp     eax, edx                ;
                pop     edx
                jne     NAA_Attr_MG;;; зоны нет ! все неместные номера -  межгород !
                ; --- проверка на свой или сеть
NAA_Local:      ; преобразуем номер A из формата SOED в формат NLOG
                @Na_from_Soed_to_Log            ;
                ; проверяем есть ли номер A в конфигурации АТС
                call    Find_Tel_Num            ; портит edx и esi
                mov     dl, PRIZN_SETI          ; = 2
                jnc     NAA_Attr                ;
                mov     dl, PRIZN_STAN          ; = 1
NAA_Attr:       ; --- запоминаем признак номера
                mov     fs:[edi].attr_a, dl     ;
		ret
NAA_Attr_MG:    mov     fs:[edi].attr_a, PRIZN_MESHGOR
                ret
Num_A_Attr	ENDP

; --- приём АОН для СОРМа
; вход:         fs:[edi] - указатель на соединение
; выход:        нет
;
Num_A_Received  PROC
                ; === проверка по исходящему пучку ======================================
IF _MSP NE TRUE
                test    FG_PROGRAM, _fg_prg_cats; комбинированная ?
                jz      NAR_Log_Check
                test    FG_PROGRAM, _fg_prg_in_bunch
                jz      NAR_Log_Check           ;
		test	fs:[edi].soed_flags, sf_RA
		jnz	NAR_Log_Check
NAR_Bunch:      @Is_Bunch_on_Ctrl ck_b          ;
                jnc     NAR_Log_Check           ;
                ; --- исходящий пучок, которому принадлежит линия Б - наблюдается
		; поиск исходной физической линии
		push	ebx
                mov     ebx, SOED_Next                          ;
                mov     cx,  _SOED_MAX                          ;
NAR_Find_1:     sub     ebx, _SOED_SIZE                         ;
                jns     NAR_Tst_Rel                             ;
                mov     ebx, _SOED_LAST - _SOED_SIZE            ;
NAR_Tst_Rel:    test    fs:[ebx].soed_flags, sf_REL             ; проверим приём отбоя
                jnz     NAR_Next                                ;
		test    fs:[ebx].soed_flags, sf_RA              ;
		jnz     NAR_Next                                ;
                mov     eax, DWORD PTR fs:[edi].attr_a          ;
                cmp     eax, DWORD PTR fs:[ebx].attr_a          ;
                jne     NAR_Next                                ;
                mov     eax, DWORD PTR fs:[edi + 4].attr_a      ;
                cmp     eax, DWORD PTR fs:[ebx + 4].attr_a      ;
                jne     NAR_Next                                ;
                mov     ax, WORD PTR fs:[edi + 8].attr_a        ;
                cmp     ax, WORD PTR fs:[ebx + 8].attr_a        ;
                jne     NAR_Next                                ;
                mov     al, fs:[edi + 10].attr_a                ;
                cmp     al, fs:[ebx + 10].attr_a                ;
                je      NAR_Found_1                             ;
NAR_Next:       loop    NAR_Find_1                              ;
                jmp     NAR_Bunch_B                             ;
                ; первичная линия А найдена
NAR_Found_1:    mov     al, fs:[ebx].ck_a                       ;
                mov     fs:[edi].ck_a, al                       ;
                mov     ax, fs:[ebx].work_a                     ;
                mov     fs:[edi].work_a, ax                     ;
                mov     ax, fs:[ebx].phys_a                     ;
                mov     fs:[edi].phys_a, ax                     ;
		;
NAR_Bunch_B:    pop	ebx
		mov     dl, 4                   ; отбор по пучку !
                mov     eax, bunch_b_ptr        ;
                call    Session_Create
                mov     fs:[edi].bunch_b_ptr, si;
		; --- проверка п/а на IP
                mov     cx, _DBL_RA_MAX
		mov     eax, DWORD PTR es:[si].sorm_num_b
                xor     bx, bx
NAR_Find:       cmp     Dbl_RA_Buff[bx], eax
                je      NAR_Found
                add     bx, 4
                loop    NAR_Find
		jmp	NAR_Log_Check
		; ---
NAR_Found:      cmp	Dbl_RA_Buff_B1[bx], 0ffffffffh
		je	NAR_Log_Check
		mov	eax, Dbl_RA_Buff_B1[bx]
		mov	DWORD PTR es:[si].sorm_attr_c, eax
		mov	eax, Dbl_RA_Buff_B2[bx]
		mov	DWORD PTR es:[si + 4].sorm_attr_c, eax
                mov     eax, Dbl_RA_Buff_DVO[bx]
		mov	WORD PTR es:[si].sorm_dvo_code, ax
		or	es:[si].sorm_flags, ssf_SWAP_AC
		mov	dl, SO_DVO
		call	Message_SORM
		and	es:[si].sorm_flags, not ssf_SWAP_AC
		jmp	NAR_Exit
ENDIF
                ; ===  проверка по логическому номеру A =================================
NAR_Log_Check:  mov     gs, ss:SEG_OBJ          ;
                cmp     Obj_Count, 0            ;
                je      NAR_Exit                ; таблица пуста !
NAR_Log_Short:  ; --- проверка по короткому №а ------------------------------------------
                @Nx_Store_and_Short   attr_a    ; пробуем выделить из 10 знаков 5-7 знаков №
                jnc     NAR_Log_Full            ; №а нельзя укоротить (не из нашего города)
                ; ищем короткий номер в объектах
                @Is_Short_Num_on_Ctrl   num_a   ;
                jnc     NAR_Short_not           ; короткий №а не наблюдается
                ; ставим на контроль по короткому №а
                ; --- {Paul} -- <07/11/2012>
                test    fs:[edi].soed_flags, sf_CALL_UPATS
                jz      @@go_to_usual;;@@go_to_Upats                 ; -- вызов №Б(городской)
                ; --- {Paul.N} --- [27/05/13]
                ; --- только при наборе номера упатс!
                ; -- и только в МО 112
                cmp     BYTE PTR ds:[bp], 112
                jne     @@go_to_Upats
                ; --- пробуем найти №Б(городской)
                push    eax edx bx cx
                push    gs
                push    edi
                mov     gs, SS:SEG_KONF
                mov     ax, ds:[bp + 7]
                shr     ax, 9
                and     ax, 0fh
                movzx   edi, BYTE PTR ds:[bp + 4]
                shl     di, 4
                or      di, ax
                and     edi, 7FFh
                shl     edi, 3
                movzx   bx, BYTE PTR gs:[edi]           ; -- вспомним тип блока
                mov     cl, TAB_TIP_LN[bx]
                cmp     BYTE PTR gs:[edi], 0            ; вызывающий на БАЛе ?
                jne     @@exit_1                        ; НЕТ - и какой-же УПАТС ?
                mov     edi, gs:[edi + 1]               ; адрес линий блока
                cmp     edi, Cfg_Free_Ptr
                jae     @@exit_1
                movzx   edx, WORD PTR ds:[bp + 7]       ; \ номер
                and     dx, 1ffh                        ; / линии
                test    FG_PROGRAM, _fg_prg_multy_pref
                jnz     @@New_conf
                mov     cl, 4                           ; размер конф(old). линии в сдвигах
   @@New_conf:  shl     edx, cl                         ; ? байт на конфигурацию линии
                add     edi, edx                        ; адрес линии в SEG_KONF
                cmp     edi, Cfg_Free_Ptr               ;
                jae     @@exit_1                        ; выйдем
                mov     eax, gs:[edi + 10]
                cmp     eax, 0FFFFFFFFh
                je      @@exit_1                        ; номера нет
                ; --- приведем номер к формату soed(sess)
                ror     al, 4
                ror     ah, 4
                ror     eax, 16
                ror     al, 4
                ror     ah, 4
                rol     eax, 16
                pop     edi
                pop     gs
                mov     edx, eax                ; сохраним номер
                xor     bx, bx                  ; счетчик длины
                mov     cx, 7                   ; max length
     @@circle:  cmp     dl, 0FFh
                je      @@to_len
                shr     edx, 4
                inc     bl
                loop    @@circle
     @@to_len:  cmp     bl, 0
                jz      @@exit_2                        ; некорректная длина, выйдем
                cmp     LENGTH_NUM, bl
                jne     @@exit_2                        ; некорректная длина, выйдем
                mov     fs:[edi]. attr_b, PRIZN_STAN    ; аттрибут
                mov     fs:[edi].len_b, bl              ; запишем длину
                mov     DWORD PTR fs:[edi].num_b, eax   ; и сам номер
                pop     cx bx edx eax
                jmp     @@go_to_usual          ; -> есть номер, можно создавать сессию
@@exit_1:       pop     edi
                pop     gs
@@exit_2:       pop     cx bx edx eax
                jmp     @@go_to_Upats           ; -> здесь только УПАТС
@@go_to_usual:  xor     dl, dl                  ; отбор по №а !
                mov     eax, calling_short_ptr  ;
                call    Session_Create          ;
                mov     fs:[edi].calling_short_ptr, si
@@go_to_Upats:  call    Session_calling_Ub
                ; --- {Paul.N} -- [THE END]
                ; ---
NAR_Short_not:  @Nx_Restore   len_a             ;
NAR_Log_Full:   ; --- проверка по полному №а --------------------------------------------
                @Is_Num_on_Ctrl   num_a         ;
                jnc     NAR_Exit                ; полный №а не наблюдается
                ; ставим на контроль по полному №а
                xor     dl, dl                  ; отбор по №а !
                mov     eax, calling_ptr        ;
                call    Session_Create          ;
                mov     fs:[edi].calling_ptr, si;
                ; ---
NAR_Exit:       ret                             ;
Num_A_Received  ENDP

; --- приём полного номера Б для СОРМа - формирование признака №Б
; вход:         fs:[edi]        - указатель на соединение
; выход:        fs:[edi].attr_b - новое значение
;               eax - абс. физ. № линии в формате СИДС
Num_B_Attr      PROC
		push	bx
		push	dx
		mov	al, LENGTH_NUM          ;
		cmp	al, fs:[edi].len_b      ;
		je	NBA_Local		;
                ; --- проверка № Б на спецслужбу
NBA_SS:         mov     dl, PRIZN_SLUSHB        ; = 6
                mov     al, fs:[edi].num_b      ;
                and     al, PREF_B_SS_MASK      ; 0fh
                cmp     al, PREF_B_SS           ; 0
                je      NBA_Attr                ;
                ; --- проверка № Б на МН
NBA_MN:         mov     dl, PRIZN_MESHDUN       ; = 5
                mov     ax, WORD PTR fs:[edi].num_b
                and     ax, PREF_B_MN_MASK      ; 00fffh
                cmp     ax, PREF_B_MN           ; 00018h
                je      NBA_Attr                ;
                ; --- проверка № Б на МГ
NBA_MG:         mov     dl, PRIZN_MESHGOR       ; = 4
                mov     ax, WORD PTR fs:[edi].num_b
                cmp     fs:[edi].len_b, 11      ;
                je      NBA_MG_Pref             ;
                cmp     fs:[edi].len_b, 10      ;
                jb      NBA_Local               ;
                and     ax, PREF_A_ZN_MASK      ; ???
                cmp     ax, PREF_A_ZN           ; ???
                jne     NBA_Attr                ;
                ;;;jmp     NBA_Zone                ;
NBA_MG_Pref:    jmp     NBA_Attr;;; зоны нет ! все 11-ти значные номера считаем МГ
                and     ax, PREF_B_ZN_MASK      ; ????
                cmp     ax, PREF_B_ZN           ; ????
                jne     NBA_Attr                ;
                jmp     NBA_Zone_Pref           ;
                ; --- проверка на зону
NBA_Zone:       mov     dl, PRIZN_ZONI          ; = 3
                movzx   bx, LENGTH_NUM          ;
                shl     bx, 2                   ;
                mov     eax, DWORD PTR fs:[edi].num_b
                and     eax, PREF_A_FULL_MASK[bx] ;
                push    edx
                mov     edx, PREF_A_FULL          ; ???xx
                and     edx, PREF_A_FULL_MASK[bx] ;
                cmp     eax, edx                ;
                pop     edx
                jne     NBA_Attr                ;
                jmp     NBA_Local               ;
NBA_Zone_Pref:  mov     dl, PRIZN_ZONI          ; = 3
                movzx   bx, LENGTH_NUM          ;
                shl     bx, 2                   ;
                mov     eax, DWORD PTR fs:[edi].num_b
                and     eax, PREF_B_FULL_MASK[bx] ;
                push    edx
                mov     edx, PREF_B_FULL          ; ???xx
                and     edx, PREF_B_FULL_MASK[bx] ;
                cmp     eax, edx                ;
                pop     edx
                jne     NBA_Attr                ;
                ; --- проверка на свой или сеть
NBA_Local:      ; преобразуем номер Б из разных форматов SOED в один формат NLOG
                @Nb_from_Soed_to_Log            ;
                ; проверяем есть ли номер Б в конфигурации АТС
                call    Find_Tel_Num            ; портит edx и esi
                mov     dl, PRIZN_SETI          ; = 2
                jnc     NBA_Attr                ;
                mov     dl, PRIZN_STAN          ; = 1
                test    fs:[edi].soed_flags, sf_RA  ; а была ли переадресация ?
                jnz     NBA_Attr                    ; Да
                ; --- {Paul} -- <16/10/2012>
                ; --- пробуем найти номер в УПАТС для №Б
                push    gs es
                mov     gs, ss:SEG_KONF
                mov     es, ss:SEG_NLOG
                push    eax esi bx
                mov     eax, es:[esi + 4]                   ; абс. физ. № линии в формате СИДС
                mov     esi, eax
                shr     esi, 9                              ; № ЦК и № блока вместе
                and     esi, 7FFh
                shl     esi, 3                              ; на конфигур. блока
                movzx   bx, BYTE PTR gs:[esi]               ; запомним тип блока
                cmp     bx, 0
                jnz     @@Go_far                            ; не наш блок
                mov     esi, gs:[esi + 1]                   ; адрес линий блока
                cmp     esi, Cfg_Free_Ptr
                jae     @@Go_far                            ; вышли за сегмент конфигурации
                and     eax, 1FFh                           ; получим номер линиии
                push    cx
                mov     cl, 4
                test    FG_PROGRAM, _fg_prg_multy_pref
                jz      @@Old_conf
                mov     cl, TAB_TIP_LN[bx]
   @@Old_conf:  shl     eax, cl                             ; ? байт под конфигурацию линии
                pop     cx
                add     esi, eax                            ; на конфигурацию линии
                cmp     esi, Cfg_Free_Ptr
                jae     @@Go_far
                ; --- Прием № УПАТС для абонента Б
                cmp     WORD PTR gs:[esi + 5], 0ffffh   ;
                je      @@Go_far                        ;
                or      fs:[edi].soed_flags, sf_UPATS_B ;
                mov     bx, gs:[esi + 5]                ; \ № логический
                mov     fs:[edi].upats_b_num, bx        ; / в УПАТС
                mov     bl, gs:[esi + 7]                ; \ №
                mov     fs:[edi].upats_b, bl            ; / в УПАТС
    @@Go_far:   pop     bx esi eax
                pop     es gs
                ; --- {Paul} -- <END>
NBA_Attr:       ; --- запоминаем признак номера
                mov     fs:[edi].attr_b, dl     ;
		pop	dx
		pop	bx
		ret
Num_B_Attr      ENDP


; --- приём полного номера Б для СОРМа
; вход:         fs:[edi] - указатель на соединение
; выход:        нет
;
Num_B_Received  PROC
                ; === проверка по входящему пучку =======================================
IF _MSP NE TRUE
                test    FG_PROGRAM, _fg_prg_cats; комбинированная ?
                jz      NBR_Log_Check           ; НЕТ -
NBR_Bunch:      @Is_Bunch_on_Ctrl ck_a          ;
                jnc     NBR_Log_Check           ;
                ; --- {Paul.N} --- [28/05/2013]
                test    fs:[edi].soed_flags, sf_ANI
                jz      NBR_Log_Check
                ; --- {Paul.N} --- [END]
                ; --- входящий пучок, которому принадлежит линия А - наблюдается
                mov     dl, 4                   ; отбор по пучку !
                mov     eax, bunch_a_ptr        ;
                call    Session_Create          ;
                mov     fs:[edi].bunch_a_ptr, si;
ENDIF
                ; ===  проверка по логическому номеру Б =================================
NBR_Log_Check:  mov     gs, ss:SEG_OBJ          ;
                cmp     Obj_Count, 0            ;
                je      NBR_Exit                ; таблица объектов пуста
IF _MSP NE TRUE
                test    FG_PROGRAM, _fg_prg_cats; комбинированная ?
                jz      NBR_Log_Full            ; НЕТ -
NBR_Log_Short:  ; --- проверка по короткому №б ------------------------------------------
                @Nx_Store_and_Short   attr_b    ; пробуем выделить из 10 знаков 5-7 знаков №
                jnc     NBR_Log_Full            ; №б нельзя укоротить (не из нашего города)
                ; ищем короткий номер в объектах
                @Is_Short_Num_on_Ctrl   num_b   ;
                jnc     NBR_Short_not           ; короткий №б не наблюдается
                ; --- {Paul.N} --- [28/05/2013]
                test    fs:[edi].soed_flags, sf_ANI
                jz      NBR_Short_not
                ; --- {Paul.N} --- [END]
                ; ставим на контроль по короткому №б
                mov     dl, 2                   ; отбор по №б !
                mov     eax, called_short_ptr   ;
                call    Session_Create          ;
                mov     fs:[edi].called_short_ptr, si
                ; по укороченному номеру УПАТС быть не может, т.к. такой номер всегда набран абонентом другой АТС
NBR_Short_not:  @Nx_Restore   len_b             ;
ENDIF
                ; --- проверка по полному №б --------------------------------------------
NBR_Log_Full:
                @Is_Num_on_Ctrl   num_b         ;
                jnc     NBR_Take_MN_Op          ; полного номера Б нет на контроле
                ; --- {Paul.N} --- [28/05/2013]
                ; -- {add - 19/07/17}
NBR_Full_Set:   test    fs:[edi].soed_flags, sf_ANI             ; есть АОН?
                jnz     @@get_Nb_full;; @@get_Nb_Ua             ; есть - отработаем и по гор. и по УПАТС
                or      fs:[edi].soed_flags, sf_2VSK_Chelnok    ;<- отметим "челнок", т.к здесь СОРМ работает без АОНа
                jmp     @@get_Nb_Ua
                ; ---
                ; ставим на контроль по полному №б
@@get_Nb_full:  mov     dl, 2                   ; отбор по №б !
                mov     eax, called_ptr         ;
                call    Session_Create          ;
                mov     fs:[edi].called_ptr, si ;
        if 0    ; --- {Paul.N} -- [06/06/13]
                test    fs:[edi].soed_flags, sf_NA_sess
                jnz     NBR_Log_MG              ; далее
        endif
                ; -- {Paul.N - add - 19/07/17}
                test    fs:[edi].soed_flags, sf_2VSK_Chelnok    ;<- если был "челнок", то по УПАТС снова не формируем
                jnz     NBR_Log_MG
                ; --- {Paul.N} --- [END]
  @@get_Nb_Ua:  mov     dl, 2
                call    Session_Nb_Ua_Create    ; возможна ещё и сессия по номеру УПАТС
                jmp     NBR_Log_MG
                ; --- проверка полного №б на выбор абонентом оператора МН связи ---------
NBR_Take_MN_Op: @Is_Take_MN_Op_on_Ctrl
                jc     NBR_Full_Set             ; реально ставим по стандартному МН номеру Б
                ; --- проверка полного №б на выбор абонентом оператора МГ связи ---------
NBR_Take_MG_Op: @Is_Take_MG_Op_on_Ctrl
                jc     NBR_Full_Set             ; реально ставим по стандартному МГ номеру Б
                ; --- проверка по №б без МГ или МН префиксов ----------------------------
NBR_Log_MG:
IF _MSP NE TRUE
                test    FG_PROGRAM, _fg_prg_cats; комбинированная ?
                jz      NBR_Exit                ; НЕТ -
                @Nx_Store_and_Del_Pref  len_b   ;
                jnc     NBR_Exit                ; в №б нет МН или МГ префикса
                ; ищем номер без префикса в объектах
                @Is_Num_on_Ctrl   num_b         ; номера Б без префикса нет на контроле
                jnc     NBR_MG_not              ;
                ; --- {Paul.N} --- [28/05/2013]
                test    fs:[edi].soed_flags, sf_ANI
                jz      @@get_Nb_mg_Ua
                ; ставим на контроль по №б без префикса
                mov     dl, 2                   ; отбор по №б !
                mov     eax, called_mg_ptr      ;
                call    Session_Create          ;
                mov     fs:[edi].called_mg_ptr, si ;
        if 0    ; --- {Paul.N} -- [06/06/13]
                test    fs:[edi].soed_flags, sf_NA_sess
                jnz     NBR_MG_not              ; далее
        endif
                ; --- {Paul.N} --- [END]
@@get_Nb_mg_Ua: mov     dl, 2
                call    Session_Nb_mg_Ua_Create ; возможна ещё и сессия по номеру УПАТС
NBR_MG_not:     @Nx_Restore   len_b             ;
ENDIF
NBR_Exit:       ret
Num_B_Received  ENDP


; --- приём номера переадресации для СОРМ - формирование его признака
; вход:         fs:[edi] - указатель на соединение
; выход:        нет
;
Num_C_Attr      PROC
                push    edi
                add     edi, attr_c - attr_b
                call    Num_B_Attr
                pop     edi
                ret
Num_C_Attr      ENDP


; --- приём номера переадресации для СОРМ
; вход:         fs:[edi] - указатель на соединение
; выход:        нет
;
Num_C_Received  PROC
                ; === проверка по исходящему пучку =======================================
IF _MSP NE TRUE
                test    FG_PROGRAM, _fg_prg_cats; комбинированная ?
                jz      NCR_Log_Check           ; НЕТ -
                test    FG_PROGRAM, _fg_prg_in_bunch
                jz      NCR_Log_Check           ;
NCR_Bunch:      @Is_Bunch_on_Ctrl ck_ra         ;
                jnc     NCR_Log_Check           ;
                ; --- исходящий пучок, на который ушёл С - наблюдается
                mov     dl, 4                   ; отбор по пучку !
                mov     eax, bunch_ra_ptr       ;
                call    Session_RA_Create       ;
                ;jnc     NCR_log_Check
                ;mov     fs:[edi].bunch_ra_ptr, si;
ENDIF
                ; ===  проверка по логическому номеру С =================================
NCR_Log_Check:  mov     gs, ss:SEG_OBJ          ;
                cmp     Obj_Count, 0            ;
                je      NCR_DVO                 ; таблица объектов пуста
                ; --- проверка по полному номеру как по короткому -----------------------
                @Is_Full_as_Short_on_Ctrl  num_c;
                jnc     NCR_Log_Full            ;
                ; укорачиваем №с до длины номера в АТС
                mov     fs:[edi].attr_c, PRIZN_STAN              ;
                mov     DWORD PTR fs:[edi].num_c, edx            ;
                mov     DWORD PTR fs:[edi + 4].num_c, 0ffffffffh ;
                mov     fs:[edi + 8].num_c, 0ffh                 ;
                mov     dl, LENGTH_NUM                           ;
                mov     fs:[edi].len_c, dl                       ;
                ; ставим на контроль по полному №c как по короткому
                mov     dl, 2                   ; отбор по №c !
                mov     eax, ra_ptr
                call    Session_RA_Create       ;
                ;jnc     NCR_DVO
                ;mov     fs:[edi].ra_ptr, si     ;

                jmp     NCR_DVO                 ;
                ; --- проверка по полному №c --------------------------------------------
NCR_Log_Full:   @Is_Num_on_Ctrl   num_c         ;
                jnc     NCR_Log_MG              ; полного номера C нет на контроле
                ; ставим на контроль по полному №c
                mov     dl, 2                   ; отбор по №c !
                mov     eax, ra_ptr
                call    Session_RA_Create       ;
                ;jnc     NCR_log_MG
                ;mov     fs:[edi].ra_ptr, si     ;
                ; --- проверка по №c без МГ или МН префиксов ----------------------------
NCR_Log_MG:     @Nx_Store_and_Del_Pref  len_c   ;
                jnc     NCR_DVO                 ; в №c нет МН или МГ префикса
                ; ищем номер без префикса в объектах
                @Is_Num_on_Ctrl   num_c         ; номера C без префикса нет на контроле
                jnc     NCR_MG_not              ;
                ; ставим на контроль по №c без префикса
                mov     dl, 2                   ; отбор по №c !
                mov     eax, ra_mg_ptr
                call    Session_RA_Create       ;
                ;jnc     NCR_MG_not
                ;mov     fs:[edi].ra_mg_ptr, si  ;
NCR_MG_not:     @Nx_Restore   len_c             ;
                ; --- номер С при п/а не наблюдается, но могут наблюдаться А или В
NCR_DVO:        ; ищем сессию по короткому №б, т.к. это всегда наш абонент
                @Nx_Short   num_b
                ; --- {Paul.N} --- [21/05/13]
                test    fs:[edi].soed_flags, sf_UPATS_B         ;
                jz      @@go_start                              ;
                push    es                                      ;
                pushad                                          ;
                push    ds                                      ;
                pop     es                                      ;
                lea     si, UPATS_Buff_B                        ;
                mov     eax, DWORD PTR fs:[edi].upats_b_num     ;
                call    Ux_UnPack                               ;
                popad                                           ;
                pop     es                                      ;
                ; --- {Paul.N} --- [END]
  @@go_start:   mov     es, ss:SEG_SORM
                xor     si, si
                mov     dl, SO_DVO
                mov     cx, _SESSION_MAX
                ;
NCR_Find:	test    es:[si].sorm_flags, ssf_REL
                jnz     NCR_Next
                test    es:[si].sorm_flags, ssf_RA
                jnz     NCR_Next
                cmp     DWORD PTR es:[si].sorm_num_b, eax
                je      NCR_A_Full
                ; --- {Paul.N} --- [21/05/13]
                test    fs:[edi].soed_flags, sf_UPATS_B
                jz      NCR_Next
                mov     ebx, DWORD PTR UPATS_Buff_B[2]
                cmp     DWORD PTR es:[si].sorm_num_b, ebx
                jne     NCR_Next
                ; --- {Paul.N} --- [END]
		; --- свое ли соединение нашли по №б (№а тоже наш) ? ---------
NCR_A_Full:     ; сначала совпадает ли полный №а
		mov     ebx, DWORD PTR fs:[edi].attr_a
		cmp	ebx, DWORD PTR es:[si].sorm_attr_a
                jne     NCR_A_Short
		mov	ebx, DWORD PTR fs:[edi + 4].attr_a
		cmp	ebx, DWORD PTR es:[si + 4].sorm_attr_a
                jne     NCR_A_Short
		mov	bx, WORD PTR fs:[edi + 8].attr_a
		cmp	bx, WORD PTR es:[si + 8].sorm_attr_a
                jne     NCR_A_Short
		mov	bl, fs:[edi + 10].attr_a
		cmp	bl, es:[si + 10].sorm_attr_a
                jne     NCR_A_Short
                jmp     NCR_A_Found
NCR_A_Short:    ; потом совпадает ли короткий №а
		push    eax
		@Nx_Short  num_a
		mov	ebx, eax
		pop	eax
                cmp     ebx, DWORD PTR es:[si].sorm_num_a
                jne     NCR_A_UPATS
                jmp     NCR_A_Found
NCR_A_UPATS:    ; потом совпадает ли номер А из УПАТС
                push    es
                pushad
                push    ds
                pop     es
                mov     eax, DWORD PTR fs:[edi].upats_a_num     ;
                lea     si, UPATS_Buff
                call    Ux_UnPack
                mov     ebx, es:[si]
                mov     PEREM_EBX, ebx
                popad
                pop     es
                mov     ebx, PEREM_EBX
                cmp     ebx, DWORD PTR es:[si].sorm_num_a
                jne     NCR_Next
NCR_A_Found:    ; переносим №с в сессию
                mov     ebx, DWORD PTR fs:[edi].attr_c
                mov     DWORD PTR es:[si].sorm_attr_c, ebx
                mov     ebx, DWORD PTR fs:[edi + 4].attr_c
                mov     DWORD PTR es:[si + 4].sorm_attr_c, ebx
                mov     bx, WORD PTR fs:[edi + 8].attr_c
                mov     WORD PTR es:[si + 8].sorm_attr_c, bx
                mov     bl, fs:[edi + 10].attr_c
                mov     es:[si + 10].sorm_attr_c, bl
                @Nx_Session  sorm_attr_c
                ; переносим в сессию код ДВО
                mov     bx, WORD PTR fs:[edi].dvo_code
                mov     WORD PTR es:[si].sorm_dvo_code, bx
                ; меняем признак отбора № для ДВО
                movzx   bx, es:[si].sorm_get_num
                mov     dh, SORM_RA_DVO_Get_Num[bx]
                mov     es:[si].sorm_get_num, dh
                ; отправляем собственно ДВО
                or      es:[si].sorm_flags, ssf_SWAP_AB + ssf_SWAP_BC + ssf_RA
                call    Message_SORM
                and     es:[si].sorm_flags, not ssf_SWAP_AB
                ; меняем признак отбора № для последующих сообщений
                mov     dh, SORM_RA_Mes_Get_Num[bx]
                mov     es:[si].sorm_get_num, dh
                ;
NCR_Next:       add     si, _SORM_SIZE
                dec     cx
                jnz     NCR_Find
                ;
NCR_Exit:       ret
Num_C_Received  ENDP


; --- пытаемся обратиться к СОРМу
; вход:         fs:[edi] - указатель на соединение
; выход:        нет
;
Call_SORM       PROC
                ;test    fs:[edi].soed_flags, sf_ANI             ; есть №а
                ;jz      @@exit                                  ;
                test    fs:[edi].soed_flags, sf_BLK_B           ; есть выход на линию Б - есть ТЧ
                jz      @@exit                                  ;
                test    fs:[edi].soed_flags, sf_NUM_FULL        ; есть №б
                jz      @@exit                                  ;
                ; --- {Paul} --- <09/11/2012>
		test    fs:[edi].soed_flags, sf_RA
		jnz     @@ignore_ip
		; ---
                test    fs:[edi].soed_flags, sf_IP              ; если не IP
                jnz     @@exit                                  ;
                ; --- {Paul} --- <END>
@@ignore_ip:    test    fs:[edi].soed_flags, sf_SORM_CALLED     ; к СОРМу ещё не обращались
                jz      @@go_SORM;;;@@exit;;;@@check_numA       ; идем дальше, как раньше
                ; -- {Paul.N - add - 18/07/17}
                ; -- проверим был ли "челнок", т.к. возможен приход АОНа после МО112
                cmp     BYTE PTR ds:[bp], 118                   ; если вызов из АОН, тогда это "челнок"
                jne     @@exit
                ; -- здесь будет только "челнок" - вызов из МО_118
                ; -- пробуем отработать по №А
@@go_SORM:      or      fs:[edi].soed_flags, sf_SORM_CALLED     ;
                ; --- {Paul.N} --- [28/05/2013]
                ; -- если нет АОНа, по город. не формируем сессии
                test    fs:[edi].soed_flags, sf_ANI
                jz      @@to_numB
                ; --- {Paul.N} --- [END]
                ; --- защита от МГ петли
                ; -- есть ли в сессии такой вызов???
                ; --- Проверим!
                ; --- № A
                mov     eax, DWORD PTR fs:[edi].attr_a
                mov     DWORD PTR Tmp_Num_Buff, eax
                mov     eax, DWORD PTR fs:[edi + 4].attr_a
                mov     DWORD PTR Tmp_Num_Buff[4], eax
                mov     ax, WORD PTR fs:[edi + 8].attr_a
                mov     WORD PTR Tmp_Num_Buff[8], ax
                mov     al, fs:[edi + 10].attr_a
                mov     Tmp_Num_Buff[10], al
		push    ds
		pop     es
		lea     si, Tmp_Num_Buff
		@Nx_Session  0
		; ---
		xor	si, si
		mov     es, ss:SEG_SORM
                mov     cx, _SESSION_MAX        ;
@@find_a:       test    es:[si].sorm_flags, ssf_REL + ssf_DVO
                jnz     @@next_a
		mov	eax, DWORD PTR es:[si].sorm_attr_a
		cmp     eax, DWORD PTR Tmp_Num_Buff
		jne	@@next_a
                mov	eax, DWORD PTR es:[si + 4].sorm_attr_a
		cmp     eax, DWORD PTR Tmp_Num_Buff[4]
		jne	@@next_a
                mov	ax, WORD PTR es:[si + 8].sorm_attr_a
		cmp     ax, WORD PTR Tmp_Num_Buff[8]
		jne	@@next_a
                mov	al, es:[si + 10].sorm_attr_a
		cmp     al, Tmp_Num_Buff[10]
                jne     @@next_a                ;;;je      @@exit
                ; ---
        ; --- {Paul} --- <09/11/2012> ---
                mov	eax, DWORD PTR fs:[edi].attr_b
		cmp	eax, DWORD PTR es:[si].sorm_attr_b
		jne	@@next_a
		mov	eax, DWORD PTR fs:[edi + 4].attr_b
		cmp	eax, DWORD PTR es:[si + 4].sorm_attr_b
                je @@num_c
                ; ----------------
                mov     eax, ss:Curr_T          ;
                sub     eax, es:[si].sorm_time  ;
                cmp     eax, 500 * 2
                jbe     @@num_c
                ; ----------------
@@next_a:       add     si, _SORM_SIZE
		loop    @@find_a
                ; --- защита от МГ петли
                xor     bx, bx
                mov     cx, _DBL_RA_MAX
@@find_ra:      mov     eax, DWORD PTR fs:[edi].num_a
                cmp     Dbl_RA_Buff_A1[bx], eax
                jne     @@next_ra
                mov     eax, DWORD PTR fs:[edi + 4].num_a
                cmp     Dbl_RA_Buff_A2[bx], eax
                jne     @@next_ra
		; ---
		mov     eax, DWORD PTR fs:[edi].num_b
                cmp     Dbl_RA_Buff[bx], eax
                jne     @@next_ra
                mov     eax, DWORD PTR fs:[edi + 4].num_b
                cmp     Dbl_RA_Buff_2[bx], eax
                je      @@exit
@@next_ra:      add     bx, 4
                loop    @@find_ra
        ; --- {Paul} --- <END>
                ;;;;jmp     @@to_numA      ; --- {Paul.N} -- [06/06/13]
        IF 0
                ; --- {Paul.N} -- [06/06/13]
 @@check_numA:  test    fs:[edi].soed_flags, sf_NA_sess
                jnz     @@exit
                test    fs:[edi].soed_flags, sf_ANI
                jz      @@exit
                ; -- только  для МО118!
                cmp     BYTE PTR ds:[bp], 118
                jne     @@exit
                or      fs:[edi].soed_flags, sf_NA_sess         ; отметим формирование сесс. по №Аг
                ; --- {Paul.N} -- [END]
        ENDIF
                ; --- прием номера A для СОРМ
   @@to_numA:   call    Num_A_Received                          ;
                ; --- приём полного номера Б для СОРМ
   @@to_numB:   call    Num_B_Received                          ;
                ; --- прием номера переадресации для СОРМ
@@num_c:        test    fs:[edi].soed_flags, sf_RA              ;
                jz      @@exit;;;@@exit_1                                  ;
                call    Num_C_Received                          ;
        IF 0    ; --- {Paul.N} -- [06/06/13]
   @@exit_1:    test    fs:[edi].soed_flags, sf_ANI
                jz      @@exit
                or      fs:[edi].soed_flags, sf_NA_sess         ; отметим формирование сесс. по №Аг
        ENDIF   ; --- {Paul.N} -- [END]
@@exit:         ret
Call_SORM       ENDP



; --- ответ для СОРМа
; вход:         fs:[edi] - указатель на соединение
;               dh       -
; выход:        нет
;
Answer          PROC
		test	fs:[edi].soed_flags, sf_ANS
		jnz	@@exit
                push    edi
                mov     es, ss:SEG_SORM                 ;
                ; --- проверяем все сесии, которые могут быть связаны с соединением
                mov     cx, _SOED_SESSIONs              ;
                mov     dl, SO_OTV                      ;
                add     edi, bunch_a_ptr                ;
Answer_Loop:    mov     si, fs:[edi]                    ;
                cmp     si, 0ffffh                      ;
                je      Answer_Next                     ;
		test    es:[si].sorm_flags, ssf_REL     ;
		jnz	Answer_Next                     ;
                cmp     es:[si].sorm_obj, 0ffffh        ;
                je      Answer_Next                     ;
                cmp     es:[si].sorm_soed, edi          ;
                jne     Answer_Next                     ;
                ; -- {Paul.N - add - 19/07/17}
                test    es:[si].sorm_flags, ssf_ANS     ; <- проверим был ли уже ответ по сессии(вариант с "челноком")
                jnz     Answer_Next                     ;  - да, пропускаем данную сессию
                ; ---
                or      es:[si].sorm_flags, ssf_ANS     ;
                call    Message_SORM                    ;
Answer_Next:    add     edi, 2                          ;
                loop    Answer_Loop                     ;
                ; ---
                pop     edi
@@exit:         ret
Answer          ENDP


; --- разъединение для СОРМа
; вход:         fs:[edi] - указатель на соединение
; выход:        нет
;
Disconnect      PROC
                test    fs:[edi].soed_flags, sf_DVO_SR  ;
                jnz     Disconnect_Exit                 ;
                ; --- <Yu> [08.05.2010] - защита от двойной переадресации
                call    Reset_RA                        ;
                ; --- формируем код разъединения ------------------------------
                test    fs:[edi].soed_flags, sf_BLK_B   ;
                jz      @@_go_nepoln                    ;
                test    fs:[edi].soed_flags, sf_NUM_FULL;
                jnz     Disconnect_Rostov               ;
                ; --- неполный набор номера
@@_go_nepoln:   mov     gs, ss:SEG_OBJ                  ;
                cmp     Obj_Count, 0                    ;
                je      Disconnect_Exit                 ; таблица пуста !
                ; --- {Paul.N} --- [17/06/2013]
                test    fs:[edi].soed_flags, sf_incmplt_REL ; были ли сообщения по данному soed.?
                jnz     Disconnect_Exit                 ; да !
                ; --- {Paul.N} --- [14/06/2013]
                ; -- №городского м. не быть, а только УПАТС
                test    fs:[edi].soed_flags, sf_ANI
                jz      Disconnect_UPATS
                ; --- {Paul.N} --- [THE END]
                ; проверка по короткому №а
                @Nx_Store_and_Short   attr_a            ; пробуем выделить из 10 знаков 5-7 знаков №
                jnc     Disconnect_Exit                 ; №а нельзя укоротить (не из нашего города)
                ; ищем короткий номер в объектах
                @Is_Short_Num_on_Ctrl   num_a           ;
                jnc     Disconnect_UPATS                ; короткий №а не наблюдается
                ; ставим на контроль по короткому №а
                xor     dl, dl                          ; отбор по №а !
                @Session_Create                         ;
                or      es:[si].sorm_flags, ssf_REL     ;
                mov     es:[si].sorm_opcode, PRI_NENAB  ;
                mov     dl, SO_RAZ                      ;
                call    Message_SORM                    ;
                ; --- MVT перенёс снизу
                @_Clear_tcop_dig                 ; уберем инфу о цифре выхода в ТСОП
                ; --- {Paul.N} --- [17/06/2013]
                or      fs:[edi].soed_flags, sf_incmplt_REL  ; отметим формрование сообщений по неполн.набору
                ; --- неполный набор номера для УПАТС
Disconnect_UPATS:
                ; --- {Paul} --- <29/10/2012>
                ; --- ищем вызывающего из УПАТС в списке объектов
                test    fs:[edi].soed_flags, sf_UPATS_A
                jz      Disconnect_Exit
                ; --- {Paul} -- <END>
                mov     eax, DWORD PTR fs:[edi].upats_a_num ;
                push    cx si edx
                push    es
                call    Ux_Find                                 ;
                pop     es
                pop     edx si cx
                jnc     Disconnect_Exit                 ; НЕ НАШЛИ
                ; --- нашли - создаём сессию
                xor     dl, dl                          ;
                @Session_Create                         ;
                jnc     Disconnect_Exit                 ;
                ; --- заносим вызывающего из УПАТС в сессию как № А
                xor     dl, dl                          ;
                call    Ux_Session                      ;
                or      es:[si].sorm_flags, ssf_REL     ;
                mov     es:[si].sorm_opcode, PRI_NENAB  ;
                mov     dl, SO_RAZ                      ;
                call    Message_SORM                    ;
                ; --- MVT перенёс снизу
                @_Clear_tcop_dig                 ; уберем инфу о цифре выхода в ТСОП
                ; --- {Paul.N} --- [17/06/13]
                or      fs:[edi].soed_flags, sf_incmplt_REL  ; отметим формрование сообщений по неполн.набору
Disconnect_Exit:cmp     fs:[edi].tcop_dig, 0FFh
                je      @@exit
                mov     fs:[edi].tcop_dig, 0FFh
                ; --- {Paul.N} --- <29/04/2013>   MVT закомментировал и перенёс выше
                ;;;@_Clear_tcop_dig                 ; уберем инфу о цифре выхода в ТСОП
                ; --- {Paul.N} --- <THE END>
                ;;;and     es:[si].sorm_flags, not ssf_TCOP_UPATS
     @@exit:    ret

Disconnect_Rostov:
                ; --- {Paul} -- <03/10/2012> ---
                ; --- техническая неисправность (Ростов!)
                mov     dl, PRI_TEX
                test    fs:[edi].soed_flags, sf_MN_rostov
                jnz     Disconnect_Do
                ; --- занятость
                mov     dl, PRI_ZAN                     ;
                test    fs:[edi].soed_flags, sf_BUSY    ;
                jnz     Disconnect_Do                   ;
                ; --- не ответ
                mov     dl, PRI_NEOT                    ;
                test    fs:[edi].soed_flags, sf_ANS  ;
                jz      Disconnect_Do                  ;
                ; --- после разговора
                mov     dl, PRI_RAZ                     ;
                ; --- {End} -- <Paul>
                ; --- собственно разъединение ---------------------------------
Disconnect_Do:  push    edi                             ;
                mov     es, ss:SEG_SORM                 ;
                ; --- проверяем все сесcии, которые могут быть связаны с соединением
                mov     cx, _SOED_SESSIONs              ;
                add     edi, bunch_a_ptr                ;
Disconnect_Loop:mov     si, fs:[edi]                    ;
                cmp     si, 0ffffh                      ;
                je      Disconnect_Next                 ;
                cmp     es:[si].sorm_obj, 0ffffh        ;
                je      Disconnect_Next                 ;
                cmp     es:[si].sorm_soed, edi          ;
                jne     Disconnect_Next                 ;
		test    es:[si].sorm_flags, ssf_REL     ;
		jnz	Disconnect_Done                 ;
                ; --- {Paul} --- <06/02/2013>
                cmp     es:[si].sorm_dvo_code, _3PTY    ; есть ли конференция в сессии ?
                jne     @@normal_end
                ; -- есть, проверим, нужна ли перекоммутация?
                push    bp cx
                xor     bp, bp
                mov     cx, _sorm_3pty_max
  @@find_slot:  cmp     Sess_3PTY[bp].session_ptr, si
                jne     @@go_loop
                cmp     Sess_3PTY[bp].perem_tms, 0FFh
                jne     @@go_recomm                     ; здесь необходима переком-ция
                ; --- а здесь освободим неиспользуемый слот структуры
                mov     Sess_3PTY[bp].session_ptr, 0FFFFh
                mov     Sess_3PTY[bp].perem_ebx_3pty, 0FFFFFFFFh
                mov     Sess_3PTY[bp].sorm_numb_3pty, 0FFFFFFFFh
                mov     Sess_3PTY[bp].perem_tms, 0FFh
                mov     Sess_3PTY[bp].soed_ptr, 0FFFFFFFFh
                ; -- поставим наименьший ук-ль
                cmp     bp, _sorm_3pty_next
                jae     @@go_loop
                mov     _sorm_3pty_next, bp
    @@go_loop:  add     bp, _sorm_3pty_size
                loop    @@find_slot
                ; -- здесь ничего не нашли, перекомм. не будет
                pop     cx bp
                jmp     @@normal_end
                ; --- а здесь - перекоммутация!
   @@go_recomm: pop     cx
                call    Recomm_3_PTY
                pop     bp
                jmp     Disconnect_Done
                ; --- {END} --- <Paul>
  @@normal_end: or      es:[si].sorm_flags, ssf_REL     ;
                mov     es:[si].sorm_opcode, dl         ;
                xor     dh, dh                          ;
                call    KSL_Free                        ;
                push    dx                              ;
                mov     dl, SO_RAZ                      ;
                call    Message_SORM                    ;
                pop     dx                              ;
                @_Clear_tcop_dig                 ; уберем инфу о цифре выхода в ТСОП MVT перенёс сюда, где должно выполняться тлько для существующих сессий
Disconnect_Done:mov     WORD PTR fs:[edi], 0ffffh       ;
Disconnect_Next:add     edi, 2                          ;
                dec     cx
                jcxz    @@disc_out
                jmp     Disconnect_Loop
                ; ---
   @@disc_out:  pop     edi                             ;
                cmp     fs:[edi].tcop_dig, 0FFh
                je      @@exit_1
                mov     fs:[edi].tcop_dig, 0FFh
                ; --- {Paul.N} --- <29/04/13>
                ; @_Clear_tcop_dig MVT перенёс туда,где выполняется только для существующих сессий
                ; --- {Paul.N} --- <END>
                ;;and     es:[si].sorm_flags, not ssf_TCOP_UPATS
 @@exit_1:      ret
Disconnect      ENDP

; *** переустановка коммутации **********************************************************
;     вход:     es:ei   - указатель на сессию
;               dl      - код команды
;               dh      - номер тайм-слота
;     выход:    нет
;
MO_Comm         PROC
		push	es
		push	fs
                push    es
                pop     fs
                cmp     fs:[si].sorm_ck_a, _CK_MAX
                jae     @@exit
                cmp     fs:[si].sorm_work_a, _WORK_NUM_MAX
                jae     @@exit
                push    dx
                @POLUCH_ADR_MO                          ;
                or      dl, dl                          ; проверим есть место ?
                pop     dx
                jnz     @@exit                          ; нет - пропустим
                mov     al, dl                          ;
                stosb                                   ;
                mov     al, fs:[si].sorm_ck_a           ;
                stosb                                   ;
                mov     ax, fs:[si].sorm_work_a         ;
                stosw                                   ;
                mov     al, dh                          ;
                add     al, al                          ;
                add     al, _MODEM_1ST_TS               ;
                mov     ah, al                          ;
                inc     ah                              ;
                stosw                                   ;
                @ZAPIS_OK_MO                            ;
@@exit:         pop	fs
		pop 	es
		ret
MO_Comm         ENDP

; *** переустановка коммутации **********************************************************
;     вход:     es:ei   - указатель на сессию
;
ReComm          PROC
                pushad
                xor     bx, bx                          ;
                mov     cx, _MAX_MODEM_TS               ;
                mov     al, es:[si].sorm_ksl_a          ;
@@find_ksl:     cmp     al, Modem_KSL[bx]               ;
                je      @@ksl_found                     ;
                inc     bx                              ;
                loop    @@find_ksl                      ;
                jmp     @@exit                          ;
@@ksl_found:    mov     dl, 121                         ;
                mov     dh, bl                          ;
                call    MO_Comm                         ;
                mov     dl, 120                         ;
                call    MO_Comm                         ;
@@exit:         popad
                ret
ReComm          ENDP


; *** переустановка коммутации **********************************************************
;     вход:     es:ei   - указатель на сессию
;               dl      - код команды
;               dh      - номер тайм-слота
;     выход:    нет
;
MO_Comm_B       PROC
		push	es
		push	fs
                push    es
                pop     fs
                cmp     fs:[si].sorm_ck_b, _CK_MAX
                jae     @@exit
                cmp     fs:[si].sorm_work_b, _WORK_NUM_MAX
                jae     @@exit
                push    dx
                @POLUCH_ADR_MO                          ;
                or      dl, dl                          ; проверим есть место ?
                pop     dx
                jnz     @@exit                          ; нет - пропустим
                mov     al, dl                          ;
                stosb                                   ;
                mov     al, fs:[si].sorm_ck_b           ;
                stosb                                   ;
                mov     ax, fs:[si].sorm_work_b         ;
                stosw                                   ;
                mov     al, dh                          ;
                add     al, al                          ;
                add     al, _MODEM_1ST_TS               ;
                mov     ah, al                          ;
                inc     ah                              ;
                stosw                                   ;
                @ZAPIS_OK_MO                            ;
@@exit:         pop	fs
		pop 	es
		ret
MO_Comm_B       ENDP

; *** переустановка коммутации **********************************************************
;     вход:     es:ei   - указатель на сессию
;
ReComm_B        PROC
                pushad
                xor     bx, bx                          ;
                mov     cx, _MAX_MODEM_TS               ;
                mov     al, es:[si].sorm_ksl_a          ;
@@find_ksl:     cmp     al, Modem_KSL[bx]               ;
                je      @@ksl_found                     ;
                inc     bx                              ;
                loop    @@find_ksl                      ;
                jmp     @@exit                          ;
@@ksl_found:    mov     dl, 121                         ;
                mov     dh, bl                          ;
                call    MO_Comm                         ;
                mov     dl, 120                         ;
                call    MO_Comm_B                       ;
@@exit:         popad
                ret
ReComm_B        ENDP

; --- Вход:
;           -> es:[si] - ссылка на сессию СОРМ(A -> B(*)) 1-го соединения 3PTY
;           -> bp      - ук-ль на структуру, описывающую данную сессию
ReComm_3_PTY    PROC
                pushad
                push    es fs
                xor     bx, bx                          ;
                mov     cx, _MAX_MODEM_TS               ;
                mov     al, es:[si].sorm_ksl_a          ;
@@find_ksl:     cmp     al, Modem_KSL[bx]               ;
                je      @@ksl_found                     ;
                inc     bx                              ;
                loop    @@find_ksl                      ;
                jmp     @@exit                          ;
@@ksl_found:    mov     cl, al          ; save #KSL
                ; -- отметим освобождение
                mov     Modem_KSL[bx], 0ffh
                ; --- освободим таймслот на модеме
                mov     dl, 121
                mov     dh, bl
                call    MO_Comm
                ; ставим коммутацию
                movzx   bx, cl                          ; #KSL
                mov     al, bl                          ;
                mov     ah, KSL_Pair[bx]                ;
                ; -- возьмем нужный таймслот
                mov     dl, Sess_3PTY[bp].perem_tms
                @COMM_SET                               ;
                ; --- свяжем №КСЛ с таймслотом на модеме
                movzx   bx, dl
                mov     Modem_KSL[bx], al
                ; связываем сессию 1-го соед. со 2-м соединением 3PTY
                mov     ebx, Sess_3PTY[bp].soed_ptr         ;\
                mov     fs:[ebx], si                        ;  свяжем 2-е соед. и сессию,
                mov     es:[si].sorm_soed, ebx              ;/ которую перекоммутируем.
                ;;; -> clear 3PTY struct
                mov     Sess_3PTY[bp].session_ptr, 0FFFFh
                mov     Sess_3PTY[bp].perem_ebx_3pty, 0FFFFFFFFh
                mov     Sess_3PTY[bp].sorm_numb_3pty, 0FFFFFFFFh
                mov     Sess_3PTY[bp].perem_tms, 0FFh
                mov     Sess_3PTY[bp].soed_ptr, 0FFFFFFFFh
                ; --- end - <clear>
                ; -- поставим наименьший ук-ль
                cmp     bp, _sorm_3pty_next
                jae     @@exit
                mov     _sorm_3pty_next, bp
        @@exit: pop     fs es
                popad
 @@end_proc:    RET
ReComm_3_PTY    ENDP

;;; ---------------------------------------------------------------------------
;;; --- процедуры СОРМ - КОНЕЦ                                              ---
;;; ---------------------------------------------------------------------------

