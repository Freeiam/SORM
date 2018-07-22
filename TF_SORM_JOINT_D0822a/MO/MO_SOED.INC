;;; ---------------------------------------------------------------------------
;;; --- работа с соединениями - НАЧАЛО                                      ---
;;; ---------------------------------------------------------------------------
;
; --- получение по рабочему номеру ссылки на слот SOED
;     запоминание параметров занятия
;     установление ссылки по рабочесму номеру на слот SOED
;
Seizure         PROC
                ; --- вычисление абсоютного рабочего номера
                mov     es, ss:SEG_ADR_SOED
                movzx   ebx, BYTE PTR ds:[bp + 1]
		and	bl, 07fh
                shl     ebx, _WORK_NUM_SHL
                or      bx, ds:[bp + 2]
                shl     ebx, 1
                ; --- получаем по рабочему номеру ссылку на SOED
;                movzx   eax, WORD PTR es:[ebx]
;                cmp     eax, _SOED_MAX
;                jae      @@find
;                mov     ecx, _SOED_SIZE
;                mul     ecx
;                mov     esi, eax
;                test    fs:[esi].soed_flags, sf_REL
;                jnz     @@found_2
; ------------- !!! ВОЗМОЖНО ЗДЕСЬ КАКИЕ-ТО ДЕЙСТВИЯ ПО ОЧИСТКЕ !!! ----------------------
;                inc     SOED_Busy_Err
;                jmp     @@found_2
                ; --- поиск следующего свободного SOED
@@find:         mov     esi, SOED_Next
                mov     cx,  _SOED_MAX
@@find_loop:    add     esi, _SOED_SIZE
                cmp     esi, _SOED_LAST
                jb      @@tst_rel_flag
                xor     esi, esi
@@tst_rel_flag: test    fs:[esi].soed_flags, sf_REL  ; проверим приём отбоя
                jnz     @@found
                loop    @@find_loop
; ------------- !!! ВОЗМОЖНО ЗДЕСЬ КАКИЕ-ТО ДЕЙСТВИЯ ПО ОЧИСТКЕ !!! ----------------------
                inc     SOED_Full_Err
                ; чтобы протолкнуть SOED_Next на следующий шаг
                add     esi, _SOED_SIZE
                cmp     esi, _SOED_LAST
                jb      @@found
                xor     esi, esi
                ; --- есть SOED для работы
@@found:        mov     SOED_Next, esi
                ; --- чистка найденного слота SOED"а
@@found_2:      mov     al, 0ffh
                mov     ecx, _SOED_SIZE
                push    fs
                pop     es
                mov     edi, esi
                @_RSTOSB
		xor	eax, eax
                mov     fs:[esi].len_a, al
                mov     fs:[esi].len_b, al
                mov     fs:[esi].len_c, al
                mov     DWORD PTR fs:[esi].soed_flags, eax
                ; --- запоминаем параметры занятия
                mov     al, ds:[bp + 1]
		and	al, 07fh
                mov     fs:[esi].ck_a, al
                mov     eax, ds:[bp + 2]
                mov     DWORD PTR fs:[esi].work_a, eax	; work & phys
                or      fs:[esi].soed_flags, sf_SZR
		test    MO_CK, 80h
                jz      @@upats
                or      fs:[esi].soed_flags, sf_IP_A
                ; --- UPATS
@@upats:        movzx   edi, BYTE PTR ds:[bp + 1]       ; № ЦК
                shl     edi, 4                          ; 16 блоков в ЦК
                mov     dx, ds:[bp + 4]                 ; \ номер
                shr     dx, 9                           ; / блока
                and     dx, 0fh
                or      di, dx                          ; № ЦК и № блока вместе
                shl     edi, 3                          ; 8 байт на конфигурацию блока ЦК
                mov     gs, ss:SEG_KONF
                ; --- {Paul} -- <15/10/2012>
                push    bx
                movzx   bx, BYTE PTR gs:[edi]           ; -- вспомним тип блока
                mov     cl, TAB_TIP_LN[bx]
                pop     bx
                cmp     BYTE PTR gs:[edi], 0            ; вызывающий на БАЛе ?
                jne     @@seg_adr_soed                  ; НЕТ - и какой-же УПАТС ?
                mov     edi, gs:[edi + 1]               ; адрес линий блока
                cmp     edi, Cfg_Free_Ptr
                jae     @@seg_adr_soed
                movzx   edx, WORD PTR ds:[bp + 4]       ; \ номер
                and     dx, 1ffh                        ; / линии
                test    FG_PROGRAM, _fg_prg_multy_pref
                jnz     @@New_conf
                mov     cl, 4                           ; размер конф(old). линии в сдвигах
   @@New_conf:  shl     edx, cl                         ; ? байт на конфигурацию линии
                ; --- {Paul} -- <END>
                add     edi, edx                        ; адрес линии в SEG_KONF
                cmp     edi, Cfg_Free_Ptr               ;
                jae     @@seg_adr_soed                  ; выйдем с ОК, но запишем
		; --- приём данных
                cmp     WORD PTR gs:[edi + 5], 0ffffh   ;
                je      @@seg_adr_soed                  ;
                or      fs:[esi].soed_flags, sf_UPATS_A ;
                mov     ax, gs:[edi + 5]                ; \ № логический
                mov     fs:[esi].upats_a_num, ax        ; / в УПАТС
                mov     al, gs:[edi + 7]                ; \ №
                mov     fs:[esi].upats_a, al            ; / в УПАТС
                ; --- UPATS
@@seg_adr_soed: ; --- отмечаем в SEG_ADR_SOED
                mov     es, ss:SEG_ADR_SOED
                xor     edx, edx
                mov     eax, esi
                mov     ecx, _SOED_SIZE
                div     ecx
                mov     es:[ebx], ax
                ; ---
		mov	edi, esi
                ret
Seizure         ENDP

; --- получение по рабочему № А команды корректной ссылки на SOED
; вход:         ds:[bp+1] - абсолютный рабочий номер А
; выход:        fs:[edi] и CF  - ОК
;               CF  - ссылки нет или она не корректна
;
Check_Work_Num_A PROC
                ; --- получаем указатель в SEG_ADR_SOED
                mov     es, ss:SEG_ADR_SOED
                movzx   ebx, BYTE PTR ds:[bp + 1]
                shl     ebx, _WORK_NUM_SHL
                or      bx, ds:[bp + 2]
                shl     ebx, 1
                ; --- проверка самой ссылки
                movzx   edi, WORD PTR es:[ebx]
                cmp     edi, _SOED_MAX
                jae     @@no_ref
                ; --- проверка корректности ссылки
                mov     eax, edi
                mov     ecx, _SOED_SIZE
                mul     ecx
                mov     edi, eax
                mov     al, ds:[bp + 1]
                cmp     al, fs:[edi].ck_a
                jne     @@bad_ref_a
                mov     ax, ds:[bp + 2]
                cmp     ax, fs:[edi].work_a
                jne     @@bad_ref_a
                ; ---
@@OK:
		stc
                ret
                ; ---
@@no_ref:       inc     SOED_no_Ref_Err
                jmp     @@not_OK
@@bad_ref_a:    inc     SOED_Ref_A_Err
                ; ---
@@not_OK:       clc
                ret
Check_Work_Num_A ENDP

; --- получение по рабочему № команды (А или Б) корректной ссылки на SOED
; вход:         ds:[bp+1] - абсолютный рабочий номер А
; выход:        fs:[edi] и CF  - ОК
;               CF  - ссылки нет или она не корректна
;
Check_Work_Num_AB PROC
                ; --- получаем указатель в SEG_ADR_SOED
		push	ebx
                mov     es, ss:SEG_ADR_SOED
                movzx   ebx, BYTE PTR ds:[bp + 1]
                shl     ebx, _WORK_NUM_SHL
                or      bx, ds:[bp + 2]
                shl     ebx, 1
                ; --- проверка самой ссылки
                movzx   edi, WORD PTR es:[ebx]
                cmp     edi, _SOED_MAX
                jae     @@no_ref
                ; --- проверка корректности ссылки по рабочему номеру А
                mov     eax, edi
                mov     ecx, _SOED_SIZE
                mul     ecx
                mov     edi, eax
                mov     al, ds:[bp + 1]
                cmp     al, fs:[edi].ck_a
                jne     @@check_work_b
                mov     ax, ds:[bp + 2]
                cmp     ax, fs:[edi].work_a
                je      @@OK
                ; --- проверка корректности ссылки по рабочему номеру Б
@@check_work_b: mov     al, ds:[bp + 1]
                cmp     al, fs:[edi].ck_b
                jne     @@bad_ref_ab
                mov     ax, ds:[bp + 2]
                cmp     ax, fs:[edi].work_b
                jne     @@bad_ref_ab
                ; ---
@@OK:           pop	ebx
		stc
                ret
                ; ---
@@no_ref:       inc     SOED_no_Ref_Err
                jmp     @@not_OK
@@bad_ref_ab:   inc     SOED_Ref_AB_Err
@@not_OK:       pop	ebx
		clc
                ret
Check_Work_Num_AB ENDP

; --- запрос АОН
; вход:         fs:[edi] - указатель на SOED
; выход:        нет
; портит:       edi      - !!!!
;
ANI_Request     PROC
                mov     esi, edi
                test    fs:[esi].soed_flags, sf_ANI
                jnz     @@exit
                ; ---
                @POLUCH_ADR_MO
                or      dl, dl
                jnz     @@exit
                mov     al, 117                 ; запрос АОН
                stosb
                mov     al, fs:[esi].ck_a       ; у ЦК
                cmp al, 127
                ja      @@exit
                stosb
                mov     ax, fs:[esi].work_a     ; по такому рабочему № А
                cmp ax, 8191
                ja      @@exit
                stosw
                @ZAPIS_OK_MO
@@exit:         mov	edi, esi
		ret
ANI_Request     ENDP


; --- сохранение очередных цифр номера Б в SOED
; вход:         fs:[edi] - указатель на SOED
;               ds:[si]  - указатель на (первую) цифру
;               cx       - количество цифр
; выход:        нет
; портит:       edi      - !!!!
;
Put_Digits      PROC
                jcxz    @@exit                  ; для предотвращения CX = 0
                ; ---
@@loop:         cmp     fs:[edi].len_b, 18      ; кол-во цифр >= 18
                jae     @@exit                  ; ДА - выйдем   MVT исправил ja в jae иначе num_b залезало в ck_b
                lodsb                           ; загрузим цифру для записи
                movzx   ebx, fs:[edi].len_b     ; возьмем кол-во цифр на данный момент
                shr     ebx, 1                  ; /2
                jc      @@even                  ;
                ; --- пишем цифру с нечётным порядковым номером - первую, третью и т.п.
@@odd:  	or      al, 0f0h                ; признак конца номера (fh в старшей тетраде)
                mov     fs:[edi + ebx].num_b, al; запишим новую цифру
                inc     fs:[edi].len_b          ; увеличим количество записанных цифр
                loop    @@loop                  ; продолжим, если еще что-нибудь есть
                jmp     @@exit
                ; --- пишем цифру с чётным порядковым номером - вторую, четвёртую и т.п.
@@even:         and     fs:[edi + ebx].num_b, 0fh ; очистим старшую тетраду
	 	shl	al, 4                     ; цифру в старшую тетраду
                or      fs:[edi + ebx].num_b, al  ; запишим новую цифру
                inc     fs:[edi].len_b          ; увеличим количество записанных цифр
                loop    @@loop                  ; продолжим, если еще что-нибудь есть
                ; ---
@@exit:         ret
Put_Digits      ENDP

;;; ---------------------------------------------------------------------------
;;; --- работа с соединениями - КОНЕЦ                                       ---
;;; ---------------------------------------------------------------------------
