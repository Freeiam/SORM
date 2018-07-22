; ╔════════════════════════════════════════════════════════════════════════════╗
; ║  Файл:         PUMAProt.ASM                                                ║
; ║  Программист:  Тоневицкий Ю.Д.                                             ║
; ╟────────────────────────────────────────────────────────────────────────────╢
; ║  Использует:   PUMAProt.DAT, RingBuff.ASM                                  ║
; ╟────────────────────────────────────────────────────────────────────────────╢
; ║  Назначение:   Модуль протокола ПУМА.                                      ║
; ║  Обеспечивает: 1. Обрабатывает неформатированный байтовый поток,  который  ║
; ║                   получает из выходного буфера драйвера  указанного Инфор- ║
; ║                   мационного Канала, выделяет из него сообщения ПУМА, про- ║
; ║                   веряет правильность сообщений  и  порядка их следования, ║
; ║                   при необходимости организует переспрос сообщений. Прове- ║
; ║                   ренные сообщения  ПУМА  помещаются в указанный  входной  ║
; ║                   буфер Коммутатора Пакетов.                               ║
; ║                2. Забирает из указанного выходного буфера Коммутатора  Па- ║
; ║                   кетов сообщения ПУМА, добавляет к ним байты транспортно- ║
; ║                   го протокола  и  направляет их в указанный входной буфер ║
; ║                   соответствующего драйвера для передачи  в Информационный ║
; ║                   Канал.                                                   ║
; ╟────────────────────────────────────────────────────────────────────────────╢
; ║  Содержит:                                                                 ║
; ║                                                                            ║
; ║                                                                            ║
; ╚════════════════════════════════════════════════════════════════════════════╝


; !!! при исполнении кода этого модуля DS должен стоять на _D_PUMA  - сегмент !!!
; !!! данных ПУМовского протокола. Структура сегмента описана в начале модуля !!!
; !!! PUMAProt.DAT                                                            !!!

; *** ПЕРВЫЙ УРОВЕНЬ **********************************************************
; --- управление вводом - выделение сообщений из выходного             -------┐
; L--                     неформатированного байтового потока драйвера --------
; вход:  edx    - номер байтового буфера драйвера, связанного с транспортным каналом
;        ebx    - указатель на запись PUMA_Trans выбранного транспортного канала
;        fs     - селектор сегмента, содержащего используемый буфер драйвера
; выход: НЕТ    - просто по приёму сообщения зовётся 2-ой уровень
;
;               !!! на L2 передаются только NN, LI и Info !!!
;
PUMA_L1_In      PROC
                ; --- подкачка в br1 - приёмный буфер 1-го уровня -------------
                push    ds
                pop     es
                mov     ecx, _PUMA_MESS_LEN     ; полный размер br1
                mov     eax, [ebx].lr1          ; уже лежит байт в br1
                mov     edi, ebx
                add     edi, br1                ; указатель на начало br1
                mov     ebp, edi                ; !! ДАЛЬШЕ В ПРОЦЕДУРЕ EBP ХРАНИТ НАЧАЛО BR1 !!
                add     edi, eax                ; указатель на пустое место в br1
                sub     ecx, eax                ; ecx байт можно ещё подкачать в br1
                jbe     SHORT PL1I              ; br1 полон - подкачивать некуда
                cli
                call    RB_BGet                 ; пробуем забрать что-нибудь у драйвера
                sti
                add     [ebx].lr1, edx          ; отметим скока ещё приняли
                jnz     SHORT PL1I              ; в br1 что-то есть - будем анализировать
PL1I_Exit:
                ret
PL1I:
                mov     al, [ebx].L1_state
                dec     al
                jz      PL1I_Sync2              ; если L1_state == _PL1I_SYNC2
                jns     PL1I_Mess               ; если L1_state == _PL1I_MESS

PL1I_Sync1:     ; --- поиск синхропосылки ( СП ) ------------------------------
                mov     al, _PUMA_SYN1          ; первый байт СП
                mov     edi, ebp                ; от начала br1
                mov     ecx, [ebx].lr1          ; по всему, что на сейчас есть в br1
                repne   scasb                   ; пытаемся найти
                jecxz   PL1I_S1                 ; jmp если просканировали всё что есть в буфере
                mov     [ebx].lr1, ecx          ; память сколько просканировали, ecx - остаток
                mov     esi, edi                ; 1-й необработанный байт в br1
                mov     edi, ebp                ; начало br1
                cld
                rep     movsb                   ; отсканированные байты выбрасываем из br1
                jmp     PL1I_Sync2              ; и сразу же поищем 2-й байт СП
PL1I_S1:        mov     DWORD PTR [ebx].lr1, 0  ; чистим br1
                jne     PL1I_Exit               ; в следующий раз опять будем искать 1-й байт СП
                mov     [ebx].L1_state, _PL1I_SYNC2
                jmp     PL1I_Exit               ; в следующий раз будем искать 2-й байт СП

PL1I_Sync2:     ; --- проверка 2-го байта синхропосылки -----------------------
                mov     al, _PUMA_SYN2          ; второй байт СП
                mov     edi, ebp                ; искомый байт может быть только первым в br1
                scasb                           ; есть такой байт ?
                je      PL1I_S2                 ; ДА -
                mov     [ebx].L1_state, _PL1I_SYNC1 ; НЕТ - опять начнём поиск 1-го байта СП
                jmp     PL1I_Exit
PL1I_S2:        mov     [ebx].L1_state, _PL1I_MESS  ; отметим переход на приём сообщения
                dec     [ebx].lr1                   ; отметим, что нашли 2-й байт СП
                jz      PL1I_Exit
                mov     ecx, [ebx].lr1          ; число необработанных байт в br1
                mov     esi, edi                ; 1-й необработанный байт в br1
                mov     edi, ebp                ; начало br1
                cld
                rep     movsb                   ; сдвинем весь остаток на байт влево

PL1I_Mess:      ; --- приём собственно сообщения: NN, LI, Info и CS -----------
                mov     edx, [ebx].lr1
                cmp     edx, 2                  ; приняли индикатор длины ?
                jb      PL1I_Exit               ; НЕТ - дальше делать нечего
                mov     esi, ebp
                movzx   ecx, BYTE PTR [esi + 1] ; получим индикатор длины
                add     ecx, 3                  ; учтём NN, LI и CS
                sub     edx, ecx                ; приняли сообщение целиком, включая байт К.С.?
                jb      PL1I_Exit               ; НЕТ - дальше делать нечего
                push    edx                     ; запомним сколько ещё байт в br1 сверх сообщения
                push    ecx                     ; и длину сообщения в br1
                xor     edx, edx                ; обнулим ячейку под накопление К.С.
                mov     ah, [esi][ecx - 1]      ; запомним К.С.
                mov     BYTE PTR [esi][ecx - 1], 0      ; обнулим байт К.С.
PL1I_CS:        lodsb;    BYTE PTR [esi]
                mov     dh, dl                  ; 1
                xor     al, dl                  ; 2
                rol     dh, 1                   ; 3
                xor     al, dh                  ; 4
                rol     dh, 1                   ; 5
                and     dh, 0fdh                ; 6
                xor     al, dh                  ; 7
                shr     dl, 4                   ; 8
                and     dl, 0ch                 ; 9
                xor     al, dl                  ; 10
                shr     dl, 1                   ; 11
                and     dl, 2                   ; 12
                xor     al, dl                  ; 13
                mov     dl, al                  ; 14 dl - рассчитываемая К.С.
                loop    PL1I_CS

                pop     ecx
                dec     ecx                     ; длина полученного сообщения без байта К.С.
                mov     edi, ebp                ; указатель на начало br1
                cmp     dl, ah                  ; рассчитанная и полученная К.С. совпали ?
                jne     PL1I_M1                 ; НЕТ -

PL1I_Cont:      mov     al, [edi + 1]           ; индикатор длины принятого сообщения
                cmp     al, 1
                jbe     PL1I_Not_Info
                inc     [ebx].info_in           ; отметим приём информационного сообщения
PL1I_Not_Info:  dec     al
                mov     dx, _PTF_NULL
                js      SHORT PL1I_Type         ; LI был == 0 - отметим NULL
                mov     dx, _PTF_INFO
                jnz     SHORT PL1I_Type         ; LI был > 1 - отметим INFO
                mov     al, [edi + 2]           ; первый информационный байт
                or      al, al
                mov     dx, _PTF_RTP
                jz      SHORT PL1I_Type         ; был == 0 - отметим RTP
                cmp     al, 5
                mov     dx, _PTF_RTW
                je      SHORT PL1I_Type         ; был == 5 - отметим RTW
                xor     dx, dx                  ; неизвестная команда - отмечать нечего
PL1I_Type:      or      [ebx].pt_flags, dx      ; отметим тип принятого сообщения
                ;-----
        ; --- {Paul} -- <debug> ----
                ; -- Check for beginning
                cmp     WORD PTR [edi + 6], 00c80h
                jne     @@some_sector
                push    gs  bp
                mov     gs, ss:SEG_AX
                inc     gs:ptr_conf_dbg
                cmp     gs:ptr_conf_dbg, 5
                jbe     @@ok_beg
                mov     gs:ptr_conf_dbg, 1
                mov     bp, 0
                ; -- initialization
                mov     gs:Pak_800E[bp], 0
                mov     gs:Pak_808E[bp], 0
                mov     gs:tmr_800E[bp], 0
                mov     gs:tmr_808E[bp], 0
                ; ------       -------
                jmp     @@end_beg
    @@ok_beg:   mov     bp, gs:ptr_conf_dbg
                cmp     bp, 0
                jbe     @@end_beg
                dec     bp
                shl     bp, 1
                cmp     gs:Pak_800E[bp], 0
                jz      @@end_beg
                ; -- initialization
                mov     gs:Pak_800E[bp], 0
                mov     gs:Pak_808E[bp], 0
                shl     bp, 1
                mov     gs:tmr_800E[bp], 0
                mov     gs:tmr_808E[bp], 0
                ; ------       -------
   @@end_beg:   pop     bp gs
                jmp     @@out_debug
                ; ---- Check for sector in
@@some_sector:  cmp     WORD PTR [edi + 6], 00e80h
                jne     @@out_debug
                push    gs ax bp
                mov     gs, ss:SEG_AX
                mov     bp, gs:ptr_conf_dbg
                cmp     bp, 0
                jbe     @@out_debug_1
                dec     bp
                mov     ax, WORD PTR [edi + 8]
                shl     bp, 1
                mov     gs:Pak_800E[bp], ax
                ; -- обновим таймер
                push    ebx
                @_timer_ebx
                shl     bp, 1
                mov     gs:tmr_800E[bp], ebx
                pop     ebx
                ; --- timer - end --
@@out_debug_1:  pop     bp ax gs
@@out_debug:    ; --- END ---
        ; --- {debug} --- <end> --

                push    esi
                push    edi
                mov     esi, ebp
                call    PUMA_L2_In              ; и попытаемся отдать пакет на L2
                pop     edi
                pop     esi
                jmp     PL1I_M2                 ; не отмечаем ошибку К.С.
PL1I_M1:        inc     [ebx].ecsc              ; отмечаем ошибку К.С.
PL1I_M2:        pop     ecx                     ; остаток необработанных байт в br1
                mov     [ebx].lr1, ecx          ; и запомним его
                or      ecx, ecx                ; что-нибудь осталось ?
                jz      SHORT PL1I_Quit         ; НЕТ - br1 чист как стёклышко
                cld                             ; ДА -
                rep     movsb                   ; обработанные байты долой, оставим только необр-е
PL1I_Quit:      mov     [ebx].L1_state, _PL1I_SYNC1   ; и снова начинаем искать СП
                ret
PUMA_L1_In      ENDP


; --- управление выводом - побайтовая запись синхропосылки, сообщения и ------┐
; L--                      К.С. во входной байтовый поток драйвера      -------
; вход:  edx    - номер байтового буфера драйвера, связанного с транспортным каналом
;        ebx    - указатель на запись PUMA_Struc выбранного транспортного канала
;        es     - селектор сегмента, содержащего используемый буфер драйвера
; выход: ebx.lt1 - обнуляется, что означает готовность к передаче след. сообщения
;
PUMA_L1_Out     PROC
                mov     esi, ebx
                add     esi, bt1                ; указатель на начало передатчика на L1
                mov     ecx, [ebx].lt1          ; байт осталось передать
                sub     ecx, [ebx].tt1
                add     esi, [ebx].tt1          ; указатель на первый не переданный байт
if _MSP NE true
if _debug_Puma
		push	eax
		mov	eax,offset SendString
		@String_to_file SS:SEG_AX,eax,5
		cmp	ecx,6
		jne	@@not_6
		mov	eax,offset rtp_array
		@compare_arrays SS:SEG_AX,eax,ds,esi,6
		jne	@@not_rtp
		mov	eax,offset puma_RTP
		@String_to_file SS:SEG_AX,eax,6
		jmp	@@end_debug_puma
@@not_rtp:
		mov	eax,offset rtw_array
		@compare_arrays SS:SEG_AX,eax,ds,esi,6
		jne	@@not_rtw
		mov	eax,offset puma_RTW
		@String_to_file SS:SEG_AX,eax,6
		jmp	@@end_debug_puma
@@not_rtw:
@@not_6:
		cmp	ecx,5
		jne	@@not_5
		mov	eax,offset null_array
		@compare_arrays SS:SEG_AX,eax,ds,esi,5
		jne	@@not_null
		mov	eax,offset puma_NULL
		@String_to_file SS:SEG_AX,eax,6
		jmp	@@end_debug_puma
@@not_null:
@@not_5:
		@Array_to_file ds,esi,ecx
@@end_debug_puma:
		pop	eax
endif
endif
                cli
                call    RB_BPut
                sti

                add     [ebx].tt1, edx          ; правим остаток с учётом реально переданных байт
                mov     edx, [ebx].tt1
                cmp     edx, [ebx].lt1
                jb      PL1O_Exit
                mov     [ebx].lt1, 0
                mov     [ebx].tt1, 0
                cmp     [ebx].l3_state, _PL3_UN_WORK
                jb      PL1O_Exit
                cmp     [ebx].l3_state, _PL3_PHASE
                ja      PL1O_Exit
                inc     [ebx].cmnd_cntr         ; отметим (сосчитаем) отправленный пакет
PL1O_Exit:      ret
PUMA_L1_Out     ENDP


; *** ВТОРОЙ УРОВЕНЬ **********************************************************
; --- управление приёмом - проверка порядка следования сообщений,      -------┐
; │                        при необходимости запрос повторной передачи        │
; │                        и передача если всё правильно и есть место         │
; L--                      для принятого сообщения с L1 сразу на L4    --------
; вход:  ebx    - указатель на запись PUMA_Trans выбранного транспортного канала
;        ecx    - длина сообщения на первом уровне: NN, LI и Info. CS - не входит в ecx !
;        esi    - указатель на байт NN сообщения на первом уровне
; выход: НЕТ    - или приём пакета, или приём запроса на повтор или формирование запроса на повтор
;
;               !!! на L2 нет приемных буферов, с L1 сразу на L4 !!!
;
PUMA_L2_In      PROC
                ;-----
                ; --- перезапуск таймера приема сообщения ---------------------
                mov     edi, _PUMA_CTRL_OFFS
                mov     eax, [edi].pc_time      ; текущее системное время
                mov     [ebx].t5, eax           ; перезапуск таймера приема
                ; --- забираем и раскладываем байт номеров --------------------
                mov     al, [esi]               ; BIBR & BSNR
                mov     ah, al
                shr     ah, 4                   ; FIBR & FSNR
                mov     [ebx]._bibr, al
                and     [ebx]._bibr, 8          ; BIBR
                and     al, 7                   ; BSNR
                mov     [ebx]._bsnr, al
                ; --- проверка BSNR -------------------------------------------
                mov     edx, DWORD PTR [ebx]._fsnf ; FSNX.FSNL.FSNT.FSNF
                dec     dl
                and     dl, 7                   ; |FSNF - 1| mod 8
                cmp     al, dl                  ;
                je      SHORT PL2I_Tst_FIBR     ;
                cmp     dl, dh                  ;
                je      PL2I_ULB                ;
                ja      PL2I_Reverse            ;
PL2I_Normal:    cmp     al, dh                  ;
                ja      PL2I_ULB                ;
                cmp     al, dl                  ;
                jb      PL2I_ULB                ; jbe !!! хотя je PL2I_Tst_FIBR
                jmp     SHORT PL2I_Tst_FIBR     ;
PL2I_Reverse:   cmp     al, dh                  ;
                ja      PL2I_Tst_FIBR           ;
                cmp     al, dl                  ;
                jb      PL2I_Tst_FIBR           ;
                jmp     PL2I_ULB                ;
                ; --- проверка FIBR -------------------------------------------
PL2I_Tst_FIBR:  mov     [ebx]._fsnr, ah
                and     [ebx]._fsnr, 7          ; FSNR
                and     ah, 8                   ; FIBR
                cmp     ah, [ebx]._fibx         ; FIBR == FIBX ?
                jne     PL2I_ULF                ; НЕТ - нелогичный FIBR
                mov     [ebx].ulf, 0            ; FIBR правильный, сброс счётчика нелогичных FIB
                ; --- очистка буфера передачи от подтверждённых сообщений -----
                mov     ah, al
                sub     ah, dl                  ; BSNR - (FSNF - 1)
                and     ah, 7                   ; |BSNR + 1 - FSNF| mod 8 - подтверждено сообщений
                sub     [ebx].mt2, ah           ; в передатчике сообщений меньше
                ; --- новое значение FSNF -------------------------------------
                inc     al
                and     al, 7                   ; |BSNR + 1| mod 8
                mov     [ebx]._fsnf, al         ; FSNF := BSNR + 1
                ; --- проверка требования повторной передачи ------------------
                mov     al, [ebx]._bibr
                cmp     al, [ebx]._fibt         ; FIBT == BIBR ?
                je      PL2I_FSNR               ; ДА - нет запроса
                inc     [ebx].rtic              ; НЕТ - счётчик принятых требований повторов ++
                mov     [ebx]._fibt, al         ; FIBT := BIBR
                mov     al, [ebx]._fsnf
                dec     al
                and     al, 7
                mov     [ebx]._fsnt, al         ; FSNT := |FSNF - 1| mod 8    FSNT := BSNR !!!
                ; --- проверка FSNR -------------------------------------------
PL2I_FSNR:      mov     al, [ebx]._fsnx
                cmp     al, [ebx]._fsnr         ; FSNR == FSNX ?
                jne     PL2I_FSNR_2             ; НЕТ - проверим не повтор ли это сообщения
                cmp     BYTE PTR [esi + 1], 1   ; ДА - номер что надо, но приняли Info ?
                jbe     PL2I_Request            ; НЕТ - сообщение пропущено, на переспрос
                mov     [ebx]._bsnt, al         ; BSNT := FSNX
                inc     al
                and     al, 7
                mov     [ebx]._fsnx, al         ; FSNX := |FSNX + 1| mod 8
                or      [ebx].pt_flags, _PTF_QUICK ; не задерживать подтверждение FSNR
                and     [ebx].pt_flags, not (_PTF_ULB + _PTF_ULF) ; чистим ошибки чтоб не копились
                ; --- передача сообщения с L1 сразу на L4 ---------------------
                add     esi, 2                  ; \ не передаём наверх
                sub     ecx, 2                  ; / 2 байта: NN и LI
                push    ds
                pop     es                      ; es на себя: сегмент с выходным буфером ПУМы
                movzx   edx, [ebx].pt_num       ; \  по № транспортной структуры
                shl     edx, 1                  ;  > определим соответсвующий
                inc     edx                     ; /  № выходного буфера ПУМы
                call    RB_MPut                 ; прямая пересылка L1 -> L4
                or      edx, edx                ; успешно ?
                jnz     PL2I_Exit               ; ДА - that's all
                inc     [ebx].l14c              ; НЕТ - отметим
                ret
                ; --- проверка: не повтор ли предыдущего сообщения ------------
PL2I_FSNR_2:    dec     al
                and     al, 7
                cmp     al, [ebx]._fsnr         ; FSNR == |FSNX - 1| mod 7 ?
                jne     SHORT PL2I_Request      ; НЕТ - это не предыдущее сообщение - на повтор
                ret                             ; ДА - сообщение пришло дважды - просто выбросим
                ; --- требования повторной передачи для встречной стороны -----
PL2I_Request:   xor     [ebx]._bibt, 8          ; для передачи запроса встречной стороне
                xor     [ebx]._fibx, 8          ; запомним, что запросили повтор
                inc     [ebx].rtoc              ; счётчик переданных требований повторов ++
                or      [ebx].pt_flags, _PTF_QUICK ; и срочно уведомить встречную сторону
                ret
                ; --- подсчёт нелогичных BSNR, разрыв связи если их много -----
PL2I_ULB:       inc     [ebx].ulb
                cmp     [ebx].ulb, _ULB_MAX
                jbe     SHORT PL2I_Exit
                or      [ebx].pt_flags, _PTF_ULB ; слишком много нелогичных BSNR подряд
                ret
                ; --- подсчёт нелогичных FIBR, разрыв связи если их много -----
PL2I_ULF:       inc     [ebx].ulf
                cmp     [ebx].ulf, _ULF_MAX
                jbe     SHORT PL2I_Exit
                or      [ebx].pt_flags, _PTF_ULF ; слишком много нелогичных FIBR подряд
                mov     [ebx].ulf, 0            ; сброс счётчика нелогичных FIB
PL2I_Exit:
		ret
PUMA_L2_In      ENDP

; --- управление передачей - отправка в нужном порядке сообщений из      -----┐
; L--                        bt2, bt2c или bt2n в bt1                    ------
; вход:  ebx    - указатель на запись PUMA_Trans выбранного транспортного канала
;        ebp    - номер входного буфера драйвера, обслуживающего канал
;        fs     - селектор входного буфера драйвера
; выход: НЕТ    - или отправка на L1 сообщения, или повторная отправка сообщения
;                 или отправка команды или отправка нулевого сообщения
;
;               !!! на L2 в передающих буферах хранится: слово длины, NN,   !!!
;               !!! LI, Info и место под байт КС. при записи на L1 в начало !!!
;               !!! добавить 2 байта СП                                     !!!
;
PUMA_L2_Out     PROC
                ; --- получение сообщения с L4 --------------------------------
                cmp     [ebx].mt2, 7            ; есть место на L2 под ещё одно сообщение ?
                jae     SHORT PL2O_0            ; НЕТ - отметим
                cmp     [ebx].l3_state, _PL3_WORK ; состояние протокола - РАБОТА ?
                jb      SHORT PL2O_1            ; НЕТ - ну и нечего принимать пакеты в L2
                mov     eax, _PUMA_MESS_LEN     ; максимальный размер собщения ПУМА
                movzx   edx, [ebx]._fsnw        ; номер сокета в bt2 для записи
                mul     edx
                mov     edi, eax                ; смещение сокета от начала bt2
                add     edi, ebx
                add     edi, bt2                ; а теперь от начала сегмента
                push    fs
                push    edi
                mov     ax, ds
                mov     es, ax                  ; принимаем в свой сегмент
                mov     fs, ax                  ; буфер из которого берём в этом же сегменте
                add     edi, 4                  ; пропустим слово длины и байты NN и LI
                movzx   edx, [ebx].pt_num       ; \ по № транспортной структуры определим
                shl     edx, 1                  ; / № соответсвующего входного буфера ПУМы
                call    RB_MGet
                pop     edi
                pop     fs
                or      edx, edx                ; есть что-нибудь на L4 ?
                jz      PL2O_1                  ; НЕТ -
                mov     [edi + 3], dl           ; впишем в bt2 LI == длине полученного пакета
                add     edx, 3                  ; в пакет добавятся 3 байта: NN, LI и CS
                mov     [edi], dx               ; впишем в bt2 слово длины == LI + 2
                inc     [ebx].mt2               ; пакетов на L2 прибыло
                inc     [ebx]._fsnw             ; \ указатель записи
                and     [ebx]._fsnw, 7          ; / + 1 по модулю 8
                jmp     SHORT PL2O_1
PL2O_0:         inc     [ebx].l42c
                ; --- отправка сообщения с L1
PL2O_1:         cmp     [ebx].lt1, 0            ; передатчик на L1 пуст ?
                jne     PL2O_PL1O               ; НЕТ - на вызов процедуры передачи первого уровня
                cmp     [ebx].mt2c, 0           ; надо передавать команду ?
                jne     PL2O_Cmnd               ; ДА -
                mov     al, [ebx]._fsnl
                cmp     al, [ebx]._fsnt         ; FSNT == FSNL ?
                jne     PL2O_Repeat             ; НЕТ - идёт повторная передача
                mov     ah, [ebx]._fsnw
                dec     ah
                and     ah, 7
                cmp     al, ah                  ; FSNT == |FSNW - 1| mod 8 ? FSNL а не FSNT !!!!!
                je      PL2O_Null               ; ДА - на отправку пустого сообщения
                inc     al                      ; НЕТ - есть неотправлявшиеся ни разу сообщения
                and     al, 7
                mov     [ebx]._fsnl, al
                mov     [ebx]._fsnt, al         ; следующий FSN к передаче
PL2O_Info:      ; --- формируем указатель на информационный пакет -------------
                mov     esi, ebx
                add     esi, bt2                ; edi - начало передающих буферов на L2
                movzx   eax, [ebx]._fsnt        ; FSNT - фактически N сообщения в bt2
                mov     edx, _PUMA_MESS_LEN
                mul     edx
                add     esi, eax                ; esi на начало одного из 8 буферов в bt2

PL2O_Out:       ; --- отправка сообщения: esi указывает на слово длины --------
                and     [ebx].pt_flags, not (_PTF_IN + _PTF_QUICK)
                ; --- перезапуск Т4 - отмечаем время отправки сообщения -------
PL2O_T4:        mov     edi, _PUMA_CTRL_OFFS + pc_time
                mov     edi, [edi]              ; текущее системное время
                mov     [ebx].t4, edi           ; перезапустим таймер передачи
                ; --- настройка байта номеров ---------------------------------
PL2O_NN:        mov     al, [ebx]._fsnt
                or      al, [ebx]._fibt
                shl     al, 4
                or      al, [ebx]._bsnt
                or      al, [ebx]._bibt
                mov     [esi + 2], al           ; впишем в пакет байт номеров NN
                ; --- получим длину отправляемого сообщения -------------------
                movzx   ecx, WORD PTR [esi]     ; байт в пакете: NN, LI, Info и CS
                add     esi, 2                  ; esi на байт NN - 1-й байт собственно сообщения
                ; --- подсчёт отправленных информационных пакетов -------------
                cmp     ecx, 4
                jbe     PL2O_Not_Info
                inc     [ebx].info_out
        ; --- {Paul} -- <debug> ---
                cmp     WORD PTR [esi + 6], 8E80h
                jne     @@not_debug
                push    gs ax bp
                mov     gs, ss:SEG_AX
                mov     bp, gs:ptr_conf_dbg
                cmp     bp, 0
                jbe     @@not_debug_1
                dec     bp
                mov     ax, WORD PTR [esi + 8]
                shl     bp, 1
                mov     gs:Pak_808E[bp], ax
                ; -- обновим таймер
                push    ebx
                @_timer_ebx
                shl     bp, 1
                mov     gs:tmr_808E[bp], ebx
                pop     ebx
                ; --- timer - end --
@@not_debug_1:  pop     bp ax gs
@@not_debug:    ; --- END ---
        ; --- {debug} -- <end> --
PL2O_Not_Info:  ; --- расчёт контрольной суммы --------------------------------
                push    ecx
                push    esi
                xor     edx, edx                ; обнулим ячейку под накопление К.С.
                mov     BYTE PTR [esi][ecx - 1], 0 ; обнулим байт К.С.
PL2O_CS:        lodsb
                mov     dh, dl                  ; 1
                xor     al, dl                  ; 2
                rol     dh, 1                   ; 3
                xor     al, dh                  ; 4
                rol     dh, 1                   ; 5
                and     dh, 0fdh                ; 6
                xor     al, dh                  ; 7
                shr     dl, 4                   ; 8
                and     dl, 0ch                 ; 9
                xor     al, dl                  ; 10
                shr     dl, 1                   ; 11
                and     dl, 2                   ; 12
                xor     al, dl                  ; 13
                mov     dl, al                  ; 14 dl - рассчитываемая К.С.
                loop    PL2O_CS
                mov     BYTE PTR [esi - 1], dl  ; впишем КС в отправляемый пакет
                pop     esi
                pop     ecx
                ; --- адресуем передатчик на L1 -------------------------------
                mov     ax, ds
                mov     es, ax
                mov     edi, ebx
                add     edi, bt1                ; edi на передатчик на L1
                mov     al, _PUMA_SYN1
                mov     ah, _PUMA_SYN2
                stosw                           ; запишем в начало bt1 синхропосылку
                mov     [ebx].lt1, ecx          ; займём передатчик на L1
                add     [ebx].lt1, 2            ; и ещё 2 байта СП
                mov     [ebx].tt1, 0
                shr     ecx, 2                  ; писать будем двойными словами
                inc     ecx                     ; а ВДРУГ реальная длина не кратна 4
                cld
                rep     movsd                   ; сообщение L2 -> L1
PL2O_PL1O:      ; --- вызов первого уровня для реальной передачи в канал ------
                mov     ax, fs
                mov     es, ax                  ; селектор входного буфера драйвера
                mov     edx, ebp                ; номер входного буфера драйвера
                call    PUMA_L1_Out             ; передача сообщения драйверу
PL2O_Exit:      ret

                ; --- отправка пустого сообщения ------------------------------
PL2O_Null:      mov     esi, ebx
                add     esi, bt2n               ; указатель на NULL
                mov     DWORD PTR [esi], 3      ; слово длины := 3, LI := 0
PL2O_FlgQuick:  test    [ebx].pt_flags, _PTF_QUICK ; был приём ?
                jnz     PL2O_Out                ; ДА - чтобы не задерживать подтверждение
                mov     edi, _PUMA_CTRL_OFFS    ; НЕТ - проверим Т4
                mov     eax, [edi].pc_time      ; текущее системное время
                sub     eax, [ebx].t4           ; перезапустим таймер передачи
                cmp     eax, [ebx].t4_tout      ; T4 истёк ?
                ja      PL2O_Out                ; ДА - на отправку NULL или CMND
                ret                             ; НЕТ - ничего не отправляем

                ; --- отправка команды ----------------------------------------
PL2O_Cmnd:      mov     esi, ebx
                add     esi, bt2c               ; указатель на CMND
                mov     DWORD PTR [esi], 1000004h  ; слово длины := 4, LI := 1
                jmp     SHORT PL2O_FlgQuick     ; на проверку условий отправки

                ; --- повторная передача Info ---------------------------------
PL2O_Repeat:    inc     [ebx]._fsnt
                and     [ebx]._fsnt, 7          ; FSNT := |FSNT + 1| mod 7
                jmp     PL2O_Info               ; на вычисление указателя на Info сообщение
PUMA_L2_Out     ENDP


; *** ТРЕТИЙ УРОВЕНЬ **********************************************************

; --- основной вызов: делается в главном цикле программы, управляет всеми -----
; ---                 каналами работающими по ПУМе                        -----
; вход:         ss:Curr_T  - двойное слово в стеке, хранящее текущее системное время
; выход:        НЕТ
;
PUMA_Prot       PROC
                push    ds
                mov     ds, ss:SEG_PUMA
                mov     edx, ss:Curr_T
                mov     edi, _PUMA_CTRL_OFFS
                mov     [edi].pc_time, edx      ; запоминаем текущее системное время
                mov     ebx, _PUMA_TRAN_OFFS    ; ebx ук. на 1-ую структуру трансп. канала
                mov     ecx, _PUMA_CHNLS_CNT    ; количество транспортных каналов
                ; --- цикл по всем транспортным каналам ---------------------
PP_Main_Loop:
		cmp     [ebx].L3_state, _PL3_OFF; канал неподключен ?
                je      PP_Next                 ; ДА - на проверку следующего канала
		pushad
                push    ds
                mov     dx, [ebx].pt_flags      ; очистим флаги приёма и ошибок
                and     [ebx].pt_flags, not (_PTF_ULB + _PTF_ULF + _PTF_IN) ;
                mov     al, [ebx].L3_state
if _MSP NE true
if _debug_Puma
		push	ecx
		test    dx, _PTF_RTP                    ; получили RTP ?
                jz      skip_debug_rtp
		lea	ecx, puma_RTP
		jmp	inform_to_file
skip_debug_rtp:
		test    dx, _PTF_RTW                    ; получили RTW ?
                jz      skip_debug_rtw
		lea	ecx, puma_RTW
		jmp	inform_to_file
skip_debug_rtw:
		test    dx, _PTF_NULL                    ; получили NULL ?
                jz      skip_debug_null
		lea	ecx, puma_NULL
		jmp	inform_to_file
skip_debug_null:
		test    dx, _PTF_INFO                    ; получили INFO ?
                jz      end_debug
		lea	ecx,puma_INFO
inform_to_file:
		push	eax
		mov	eax,offset ReceiveString
		@String_to_file SS:SEG_AX,eax,8
		pop	eax
		push	ebx 
		movzx	ebx,[ebx].L3_state
		@dword_to_file ebx,2
		pop	ebx
		call	write_space_to_debug_file
		@String_to_file SS:SEG_AX, ecx, 6
end_debug:
		pop	ecx
endif
endif
                ; --- переход по рабочему состоянию транспортного протокола ---
                cmp     al, _PL3_WORK
                je      PP_Work
                ;
                push    fs
                push    edx
                mov     fs, ss:SEG_PUMA         ;!!!
                movzx   edx, [ebx].pt_num
                shl     edx, 1
                call    RB_Clear
                pop     edx
                pop     fs
                ; --- переходы по состояниям вхождения в связь ----------------
                cmp     al, _PL3_READY
                je      PP_Ready
                cmp     al, _PL3_PHASE
                je      PP_Phase
                cmp     al, _PL3_UN_WORK
                je      PP_Un_Work

                ; --- этап 0: ИСХОДНОЕ состояние транспортного протокола ------
PP_Init:        xor     eax, eax                        ; инициализация переменных
                ; ---
                cld
                push    ds
                pop     es
                push    ecx
                mov     ecx, PUMA_Trans_Size - 4
                mov     edi, ebx
		add	edi, 4
                rep     stosb
                pop     ecx
                ; ---
                mov     al, 1
                mov     [ebx].mt2c, al                  ; признак передачи команды
                mov     al, 7
                mov     [ebx]._bsnt, al
                mov     [ebx]._fsnt, al
                mov     [ebx]._fsnl, al
                mov     al, 8
                mov     [ebx]._bibt, al
                mov     [ebx]._fibt, al
                mov     [ebx]._fibx, al
                ;
                mov     eax, _PUMA_T4_BEG               ; \ настройка
                mov     [ebx].t4_tout, eax              ; / тайм-аута передачи
                mov     eax, [edi].pc_time              ; \ текущее системное время
                mov     [ebx].t4, eax                   ; / запуск Т4
                mov     [ebx].L3_state, _PL3_UN_WORK    ; на следующий этап
                or      [ebx].pt_flags, _PTF_QUICK      ; флаг "СРОЧНО"
                jmp     PUMA_Out_Pack

                ; --- этап 1: состояние транспортного протокола - НЕ РАБОТАЮ --
PP_Un_Work:
                test    dx, _PTF_RTP + _PTF_RTW         ; получили RTP ?
                jz      PUMA_Out_Pack                   ; НЕТ - ждём-с дальше бессрочно
;;;                cmp     [ebx].cmnd_cntr, _PL3_CMND_MAX  ; \ достаточно послали RTP
;;;                jb      PUMA_Out_Pack                   ; / чтобы перейти на след. этап ?
                mov     [ebx + 4].bt2c, 5               ; код RTW == 5
                mov     [ebx].mt2c, 1                   ; признак передачи команды
                mov     eax, [edi].pc_time
                mov     [ebx].t1, eax                   ; запустим Т1
                mov     [ebx].L3_state, _PL3_PHASE      ; на следующий этап
                or      [ebx].pt_flags, _PTF_QUICK      ; флаг "СРОЧНО"
                xor     eax, eax                        ; \ чистим счётчик команд для перехода
                mov     [ebx].cmnd_cntr, eax            ; / из ФАЗИРОВАНИЕ в ГОТОВ
                jmp     PUMA_Out_Pack

                ; --- этап 2: состояние транспортного протокола - ФАЗИРОВАНИЕ -
PP_Phase:
                test    dx, _PTF_RTW                    ; получили RTW ?
                jz      PP_Phase_T1                     ; НЕТ - проверим тайм-аут
;;;                cmp     [ebx].cmnd_cntr, _CMND_MAX      ; \ достаточно послали RTW
;;;                jb      PUMA_Out_Pack                   ; / чтобы перейти на след. этап ?
                mov     [ebx].L3_state, _PL3_READY      ; ДА - на следующий этап
                mov     [ebx].mt2c, 0                   ; стоп передача команд - будем слать NULL
                mov     eax, [edi].pc_time
                mov     [ebx].t2, eax                   ; запустим Т2
                jmp     PUMA_Out_Pack
PP_Phase_T1:    mov     eax, [edi].pc_time              ; текущее время
                sub     eax, [ebx].t1                   ; тиков с момента вхождения в ФАЗИРОВАНИЕ
                cmp     eax, [edi].pc_t1                ; тайм-аут истёк ?
                jbe     PUMA_Out_Pack                   ; НЕТ - остались на ФАЗИРОВАНИИ
PP_To_Init:     mov     [ebx].L3_state, _PL3_INIT       ; ДА - в ИСХОДНОЕ
                jmp     PUMA_Out_Pack

                ; --- этап 3: состояние транспортного протокола - ГОТОВ -------
PP_Ready:
                test    dx, _PTF_RTP                    ; получили RTP ?
                jnz     SHORT PP_To_Init                ; ДА - беда ! в исходное !
		; MVT отключил переход в Work по получении NULL при работе с COM-портом
		mov	ax, _PTF_INFO
		cmp	[ebx].pt_type, _PT_COM1
		je	@@com_port
		cmp	[ebx].pt_type, _PT_COM2
		je	@@com_port
		add	ax,_PTF_NULL
@@com_port:
                test    dx, ax ;_PTF_INFO ;+ _PTF_NULL     ; информационный обмен начался ?
		; /MVT
                jz      PP_Ready_T2                     ; НЕТ - провериим тайм-аут
                mov     [ebx].L3_state, _PL3_WORK       ; ДА - в работу
                mov     eax, [edi].pc_time
                mov     [ebx].t5, eax                   ; запустим Т5
;;;                mov     eax, _PUMA_T4_WORK              ; \ перенастройка
;;;                mov     [ebx].t4_tout, eax              ; / тайм-аута передачи
                jmp     PUMA_Out_Pack
PP_Ready_T2:    
; MVT сделал сброс отсчёта времени по получении NULL при работе с COM-портом
; (как сделано выше, остаёмся на Ready)
		cmp	[ebx].pt_type, _PT_COM1
		je	@@com_port_t
		cmp	[ebx].pt_type, _PT_COM2
		je	@@com_port_t
		jmp	PP_check_t2
@@com_port_t:
		test    dx, _PTF_NULL
		jz	PP_check_t2
		mov     eax, [edi].pc_time
		mov	[ebx].t2,eax
		jmp     PUMA_Out_Pack
PP_check_t2:    ; /MVT
		mov     eax, [edi].pc_time              ; текущее время
                sub     eax, [ebx].t2                   ; тиков с момента вхождения в ГОТОВНОСТЬ
                cmp     eax, [edi].pc_t2                ; тайм-аут истёк ?
                jbe     PUMA_Out_Pack                   ; НЕТ - остались на ГОТОВНОСТИ
                jmp     PP_To_Init                      ; ДА - в ИСХОДНОЕ

                ; --- вспомогательные переходы с РАБОТы в ИСХОДНОЕ ------------
PP_Work_RTP:    inc     [ebx].rtpc                      ; счётчик разрывов связи из-за RTP
                jmp     PP_To_Init
PP_Work_RTW:    inc     [ebx].rtwc                      ; счётчик разрывов связи из-за RTW
                jmp     PP_To_Init
PP_Work_ULB:    inc     [ebx].ulbc                      ; счётчик разрывов связи из-за ULB
                jmp     PP_To_Init
PP_Work_ULF:    inc     [ebx].ulfc                      ; счётчик разрывов связи из-за ULF
                jmp     PP_To_Init
PP_Work_T5:     inc     [ebx].t5c                       ; счётчик разрывов связи из-за T5
                jmp     PP_To_Init
                ; --- этап 4: состояние транспортного протокола РАБОТАЮ -------
PP_Work:        ; --- проверки необходимости разрыва связи --------------------
                test    dx, _PTF_RTP                    ; приняли RTP ?
                jnz     PP_Work_RTP                     ; ДА - отметить и в исходное
                test    dx, _PTF_RTW                    ; приняли RTW ?
                jnz     PP_Work_RTW                     ; ДА - отметить и в исходное
                test    dx, _PTF_ULB                    ; слишком много нелогичных BSNR ?
                jnz     PP_Work_ULB                     ; ДА - отметить и в исходное
                test    dx, _PTF_ULF                    ; слишком много нелогичных FIBR ?
                jnz     PP_Work_ULF                     ; ДА - отметить и в исходное
                ; --- проверки таймеров ---------------------------------------
PP_Work_T:      mov     eax, [edi].pc_time              ; текущее время
                sub     eax, [ebx].t5                   ; время с момента приема последнего сообщения
                cmp     eax, [edi].pc_t5                ; тайм-аут истёк ?
                ja      PP_Work_T5                      ; ДА - на разрыв соединения

PUMA_Out_Pack:  ; --- передача в транспортный канал ---------------------------
                movzx   ebp, [ebx].pt_num               ; N буфера драйвера, связанного с каналом
                shl     ebp, 1                          ; --- НОМЕРА ВХОДНЫХ БУФЕРОВ ЧЁТНЫЕ !!!
                mov     fs, ss:SEG_PUMA_DRV             ; селектор этого буфера драйвера
                push    es
                push    esi
                call    PUMA_L2_Out
                pop     esi
                pop     es

PUMA_In_Pack:   ; --- приём с транспортного канала ----------------------------
                movzx   edx, [ebx].pt_num               ; N буфера драйвера, связанного с каналом
                shl     edx, 1
                inc     edx                             ; --- НОМЕРА ВЫХОДНЫХ БУФЕРОВ НЕЧЁТНЫЕ !!!
                mov     fs, ss:SEG_PUMA_DRV             ; селектор этого буфера драйвера
                push    es
                call    PUMA_L1_In                      ; приём: L1 -> L4
                pop     es

                pop     ds
                popad
PP_Next:        add     ebx, PUMA_Trans_Size            ; ds:ebx - на структуру трансп. канала ПУМы
                dec     ecx
                jnz     PP_Main_Loop                    ; и сначала
                ; ---
                pop     ds
                ret
PUMA_Prot       ENDP

; === внешний интерфейс ПУМы ==================================================
; --- инициализация протокола - делается перед входом в главный цикл ----------
; вход:  es     -       должен быть установлен в SEG_PUMA
;        eax    -       типы каналов ПУМы
;        bx     -       каналы ЦТО
; выход: НЕТ

PUMA_Init       PROC

                push    ds
                mov     ds, ss:SEG_PUMA_SCR
IF _MSP EQ TRUE
                cmp     bPUMAInited, 0
                jne     P_I_Exit
		mov	dPUMAInit, eax
ENDIF
		push    eax
                push    bx
                ; --- инициализация буферов драйверов
                mov     es, ss:SEG_PUMA_DRV
                mov     ecx, _PUMA_CHNLS_CNT * 2
                lea     esi, PUMA_Drv_List
                call    RB_Init
                ; --- инициализация буферов ПУМы
                mov     es, ss:SEG_PUMA
                mov     ecx, _PUMA_CHNLS_CNT * 2
                lea     esi, PUMA_Buf_List
                call    RB_Init
                ; ---  настройка таймеров в общей управляющей структуре --------
                mov     edi, _PUMA_CTRL_OFFS
                mov     eax, _PUMA_T1
                mov     es:[edi].pc_t1, eax
                mov     eax, _PUMA_T2
                mov     es:[edi].pc_t2, eax
                mov     eax, _PUMA_T5
                mov     es:[edi].pc_t5, eax
                ; --- очистка транспортных структур всех каналов --------------
                add     edi, PUMA_Ctrl_Size
                mov     eax, _PUMA_CHNLS_CNT    ; информационных каналов в блоке и на
                mov     edx, PUMA_Trans_Size    ; каждый структура транспортного канала ПУМЫ
                mul     edx
                mov     ecx, eax
                xor     eax, eax
                cld
                rep     stosb                   ; обнуляем всё
                ; --- нумерация транспортных структур всех каналов ------------
                mov     edi, _PUMA_TRAN_OFFS
                xor     al, al
                mov     ecx, _PUMA_CHNLS_CNT    ; информационных каналов в блоке и
PI_Loop:        mov     es:[edi], al
                mov     es:[edi].L3_state, _PL3_OFF ; пока всё отключено !
                mov     es:[edi].pt_type,  _PT_NONE ; пока всё отключено !
                mov     es:[edi].pt_cto_in,  0ffh   ; пока всё отключено !
                mov     es:[edi].pt_cto_out, 0ffh   ; пока всё отключено !
                inc     al
                add     edi, PUMA_Trans_Size
                loop    PI_Loop
                ; ---
                pop     bx
                pop     eax
                ; --- активизация каналов ПУМы по списку в eax
                mov     edi, _PUMA_TRAN_OFFS
                mov     ecx, _PUMA_CHNLS_CNT    ; информационных каналов в блоке и
PI_Loop_2:      cmp     al, _PT_NONE
                je      PT_Next
                mov     es:[edi].L3_state, _PL3_INIT
                mov     es:[edi].pt_type, al
                ;
                cmp     al, _PT_COM1
                jne     PT_Next_2
                call    PUMA_COM1_Init
                jmp     PT_Next
                ;
PT_Next_2:      cmp     al, _PT_COM2
                jne     PT_Next_3
                call    PUMA_COM2_Init
                jmp     PT_Next
                ;
PT_Next_3:      cmp     al, _PT_CTO
                jne     PT_Next
                mov     es:[edi].pt_cto_in,  bl
                mov     es:[edi].pt_cto_out, bh
;;;                call    PUMA_CTO_Init
                ;
PT_Next:        add     edi, PUMA_Trans_Size
                shr     eax, 8
                loop    PI_Loop_2
                ; ---
                push    ds
                pop     es
                mov     es:bPUMAInited, 255
                ; ---
P_I_Exit:       pop     ds
                ret
PUMA_Init       ENDP


; --- работа ПУМы: протокол, экран, команды -------------------------
; вход:         fs:[esi] - указатель на начало основного экрана
; выход:        dl       - номер ПУМы
;               bl       - команда
;               bh       - группа
;
PUMA_Call       PROC
IF _MSP EQ TRUE
                mov     es, ss:SEG_PUMA_SCR
                cmp     es:bPUMAInited, 0
                je      P_C_Exit
ENDIF
                ; --- транспорт
                push    fs
                push    esi
                call    PUMA_Prot
                pop     esi
                pop     fs
                ; --- подпихнём передачу
                push    ds
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  DS:SCREEN_SEG_PUMA
;                mov     eax, ss:Curr_T
;                sub     eax, dDiffsTime
;                cmp     eax, dDiffsTOut
;                jbe     PC_Screen
;                mov     eax, ss:Curr_T
;                mov     dDiffsTime, eax
                mov     es, ss:SEG_PUMA
                push    fs
                call    PUMA_COM1_Out
                call    PUMA_COM2_Out
                pop     fs
                ; --- экран
PC_Screen:      mov     eax, ss:Curr_T
                sub     eax, dDiffsTime
                cmp     eax, dDiffsTOut
                jbe     PC_Quit
                mov     eax, ss:Curr_T
                mov     dDiffsTime, eax
                call    Scr_Mark_Diffs          ; отмечаем изменившиеся области экрана
                call    Scr_Send_Diffs          ; пересылаем изменения экрана в ПУМу
PC_Quit:        pop     ds
                ASSUME  DS:DATA_AXS
                ; ---
P_C_Exit:       ret
PUMA_Call       ENDP


; --- количество каналов ПУМы ---------------------------------------
; вход:         нет
; выход:        eax     - количество каналов
;
PUMA_Chnls      PROC
                mov     eax, _PUMA_CHNLS_CNT
                ret
PUMA_Chnls      ENDP


; --- приём ПУМы ----------------------------------------------------
; вход:         ebx     - номер канала: 0 .. _PUMA_CHNLS_CNT - 1
; выход:        ecx     - количество принятых байт (0 - нет приёма)
;               es:[edi]- указатель на принятый пакет (1-ый байт адреса)
;
PUMA_In         PROC
		push	fs
		push	edx
IF _MSP EQ TRUE
		mov	fs, ss:SEG_PUMA_SCR
                cmp     fs:bPUMAInited, 0
                je      PI_Exit
ENDIF
                ; --- проверка № канала
                xor     ecx, ecx                ;
                cmp     ebx, _PUMA_CHNLS_CNT    ;
                jae     PI_Exit                 ;
                ; --- теперь можно читать
                mov     fs, ss:SEG_PUMA         ;!!!
                mov     edx, ebx                ; \
                shl     edx, 1                  ;  > по № канала № входного буфера
                inc     edx                     ; /
                lea     edi, buff_In            ; \ сюда будем
                mov     es, ss:SEG_PUMA_SCR     ; / читать
                call    RB_MGet                 ; пробуем забрать сообщение
                mov     ecx, edx                ; результат чтения
                ; ---
PI_Exit:        pop	edx
		pop     fs
                ret
PUMA_In         ENDP

; --- передача ПУМы -------------------------------------------------
; вход:         ebx     - номер канала: (0 .. _PUMA_CHNLS_CNT - 1)
;               ecx     - количество байт к передаче
;               ds:[esi]- указатель на передаваемый пакет
; выход:        CF      -  ответ отправлен,  не отправлен
;
PUMA_Out        PROC
                push    es
                push    edx
                mov     es, ss:SEG_PUMA
                mov     edx, ebx
                shl     edx, 1
                call    RB_MPut
                or      edx, edx
                jz      PO_not_OK
PO_OK:          stc
                pop     edx
                pop     es
                ret
PO_not_OK:      clc
                pop     edx
                pop     es
                ret
PUMA_Out        ENDP

; --- установка режима управления -----------------------------------
; вход:         al      - код ответа
;               ebx     - номер канала с которого получен запрос на управление: (0 .. _PUMA_CHNLS_CNT - 1)
;               es:[edi]- указатель на пакет с запросом управления
; выход:        CF      -  ответ отправлен,  не отправлен
;
PUMA_Answer     PROC
                push    ds
                push    es
		push	fs
                pushad
                ; ук. на отпраляемый пакет ds:[esi] из ук. на принятый пакет es:[edi]
                push    es
                pop     ds
                mov     esi, edi
                ; меняем ПУМовский адрес на ответный
                mov     edx, [esi]
                @EDX_back_Addr
                mov     [esi], edx
                ; из № канала № передающего буфера
                mov     edx, ebx
                shl     edx, 1
                mov     es, ss:SEG_PUMA
                mov     fs, ss:SEG_PUMA
                ; длину ответа вычисляем по коду запроса
                mov     ecx, 9
                cmp     WORD PTR [esi + 4], 00280h
                je      PA_Send
                cmp     WORD PTR [esi + 4], 00880h
                je      PA_Send
                cmp     WORD PTR [esi + 4], 00980h
                je      PA_Send
                cmp     WORD PTR [esi + 4], 00e80h
                je      PA_Send
                mov     ecx, 7
                cmp     WORD PTR [esi + 4], 00c80h
                je      PA_Send
                cmp     WORD PTR [esi + 4], 00d80h
                je      PA_Send
                cmp     WORD PTR [esi + 4], 02181h
                je      PA_Send
                cmp     WORD PTR [esi + 4], 02281h
                je      PA_Send
                cmp     WORD PTR [esi + 4], 07f03h
                je      PA_Send
                mov     ecx, 88h
                cmp     WORD PTR [esi + 4], 01080h
                je      PA_not_Send
                mov     ecx, 6
                cmp     WORD PTR [esi + 4], 07181h
                jne     PA_not_OK
                jmp     PA_not_Send
PA_Send:        ; вписываем код ответа
IF _MSP EQ TRUE
                push    ebp
                mov     ebp, ecx
                dec     ebp
                mov     [esi + ebp], al
                pop     ebp
ELSE
                add     edi, ecx
                dec     edi
                mov     [edi], al
ENDIF
                ; меняем код команды на ответный
PA_not_Send:    or      BYTE PTR [esi + 5], 80h
                ; --- отправка
		cmp	BYTE PTR [esi + 4], 80h
		jne	PA_Put
PA_Put:         call    RB_MPut
                ; формируем результат
                or      edx, edx
                jz      PA_not_OK
PA_OK:          stc
                popad
		pop	fs
                pop     es
                pop     ds
                ret
PA_not_OK:      clc
                popad
                pop     fs              ; не было
                pop     es
                pop     ds
                ret
                ;
PUMA_Answer     ENDP


; --- установка режима управления -----------------------------------
; вход:         ebx     - номер канала с которого получен запрос на управление: (0 .. _PUMA_CHNLS_CNT - 1)
;               es:[edi]- указатель на пакет с запросом управления
; выход:        CF      -  запрос удовлетворён,  блок уже управляется
;
PUMA_Set_Ctrl   PROC
                push    ds
                pushad
                ;
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  ds:SCREEN_SEG_PUMA
                ; проверка режима управления
                cmp     bCtrlChnl, _PUMA_CHNLS_CNT
                jae     PSC_Is_Cfg
                ; перехват управления - собщим об этом потерпевшему
;                push    ebx
;                lea     esi, buff_Tar
;                mov     eax, dCtrlAddr
;                mov     [esi], eax
;                mov     WORD PTR [esi + 4], 8b08h
;                mov     BYTE PTR [esi + 6], 1
;                mov     ecx, 7
;                movzx   ebx, bCtrlChnl
;                shl     ebx, 1
;                call    PUMA_Out
;                pop     ebx
                ; проверка режима конфигурации
PSC_Is_Cfg:     ; --- {Paul} -- [22/08/2013]
                ; --> сброс пар-ов режима конф.
                ;;mov     al, 201                 ; блок уже в режиме конфигурации
                ;;cmp     bCfg, 0                 ;
                ;;jne     PSC_Answer              ;
                push    eax
                xor     eax, eax
                mov     DWORD PTR bBlockType, eax
                mov     DWORD PTR wStartSector, eax
                pop     eax
                ; -- выйдем из режима конф.
                mov     bCfg, 0
                ; --- {Paul} -- <END>
IF _MSP EQ TRUE
                mov     edx, ebx                ;
                shl     edx, 1                  ;
                call    RB_Clear                ;
ENDIF
                ;
                xor     al, al                  ; ОК на запрос управления
                mov     bCtrlChnl, bl           ;
                mov     edx, es:[edi]           ;
                @EDX_Back_Addr                  ;
IF _MSP EQ TRUE
                and     dh, 0f0h
                or      dh, 2
ENDIF
                mov     dCtrlAddr, edx          ; обратный адрес
                mov     dx, es:[edi + 6]        ;
                mov     wCtrlCK, dx             ; ЦК и блок
;;;                mov     bScrEnabled, 0
;;;                test    BYTE PTR es:[edi + 8], 2
;;;                jz      PSC_Answer
                mov     bScrEnabled, 255        ;
                ;
                mov     dDiffsCount,  0         ; количество записей в буфере изменений
                mov     dCurrDiffNum, 0         ; текущий номер записи, передаваемой в ПУМу
                ;
                call    PUMA_Refresh_Scr        ;
PSC_Answer:     call    PUMA_Answer             ;
                ; формируем результат
                or      al, al                  ;
                jz      PSC_not_OK              ;
		;
PSC_OK:         stc
		popad
                pop     ds
                ASSUME  ds:DATA_AXS
                ret
		;
PSC_not_OK:     clc
		popad
                pop     ds
                ASSUME  ds:DATA_AXS
                ret
PUMA_Set_Ctrl   ENDP

PUMA_Set_Ctrl_Far   PROC
		call	PUMA_Set_Ctrl
		retf
PUMA_Set_Ctrl_Far   ENDP

; --- отмена режима управления -----------------------------------
; вход:         ebx     - номер канала для которого отменяем управление: (0 .. _PUMA_CHNLS_CNT - 1)
;               es:[edi]- указатель на пакет с отменой управления
; выход:        CF      -  запрос удовлетворён ВСЕГДА
;
PUMA_Reset_Ctrl PROC
                push    ds
                pushad
                ;
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  ds:SCREEN_SEG_PUMA
                xor     al, al                  ; ОК на отмену управления
                mov     bCtrlChnl, 255
                mov     bScrEnabled, al
                mov     bCfg, al
                call    PUMA_Answer
                ; формируем результат
                stc                             ; всегда ОК
                ;
                popad
                pop     ds
                ASSUME  ds:DATA_AXS
                ret
PUMA_Reset_Ctrl ENDP


; ---обновление экрана ------------------------------------------------------
; вход:         нет
; выход:        нет
;
PUMA_Refresh_Scr        PROC
                push    es
                push    ax
                push    cx
                push    di
                ;
                cld
                xor     al, al
                mov     cx, _SCRN_SIZE
                xor     di, di
                mov     es, ss:SEG_PUMA_SCR
                rep     stosb
                ;
                pop     di
                pop     cx
                pop     ax
                pop     es
                ret
PUMA_Refresh_Scr        ENDP

; --- запрос состояния ПУМы -------------------------------------------------
; вход:         al      - тип ПУМы: _PT_COM1, _PT_COM2, _PT_CTO
; выход:        CF      -  в ax запрошенные данные,  такая ПУМА не известна
;               al      - код состояния
;               ah      - 0 - не рабочее состояние, 1 - рабочее состояние
;
PUMA_Get_State  PROC
                push    es
                push    edi
                mov     es, ss:SEG_PUMA
                ; --- поиск работающего канала ЦТО
                xor     cx, cx
                mov     edi, _PUMA_TRAN_OFFS
@@find:         cmp     es:[edi].L3_state, _PL3_OFF ; отключено !
                je      @@next
                cmp     es:[edi].pt_type, al        ;
                je      @@found
@@next:         add     edi, PUMA_Trans_Size
                inc     cx
                cmp     cx, _PUMA_CHNLS_CNT         ; информационных каналов в блоке
                jb      @@find
                xor     ax, ax
                clc
                jmp     @@exit
@@found:        ; ---
                movzx   ax, es:[edi].L3_state
                cmp     al, _PL3_WORK
                jne     @@quit
                inc     ah
@@quit:         stc
                ; ---
@@exit:         pop     edi
                pop     es
                ret
PUMA_Get_State  ENDP

PUMA_Get_State_Far      PROC
                call    PUMA_Get_State
                retf
PUMA_Get_State_Far      ENDP

; === передача в ПУМу тарификации (статистики работы АТС) ===========
; --- установка канала тарификации ----------------------------------
; вход:         ebx     - номер канала: (0 .. _PUMA_CHNLS_CNT - 1) или 255 чтобы отключить
; выход:        CF      -  установлен,  нет
;
PUMA_Set_Tar    PROC
                cmp     ebx, 0ffh               ;
                je      @@OK                    ;
                cmp     ebx, _PUMA_CHNLS_CNT    ;
                jb      @@OK                    ;
@@not_OK:       clc                             ;
                ret                             ;
@@OK:           push    es                      ;
                mov     es, ss:SEG_PUMA_SCR     ;
                mov     es:bTarChnl, bl         ;
                pop     es                      ;
                stc                             ;
                ret
PUMA_Set_Tar    ENDP


; --- возвращает указатель на буфер ---------------------------------
; вход:         нет
; выход:        es:[edi]- указатель на буфер для формирования пакета ВСЕГДА !
;               CF      -  есть указатель,  тар. канал не задан
;
PUMA_Start_Tar  PROC
                mov     es, ss:SEG_PUMA_SCR     ;
                lea     edi, buff_Tar           ;
                cmp     es:bTarChnl, _PUMA_CHNLS_CNT ;
                jb      @@OK                    ;
@@not_OK:       clc                             ;
                ret                             ;
@@OK:           stc                             ;
                ret
PUMA_Start_Tar  ENDP


; --- передаёт пакет из буфера в тарификационный канал --------------
; вход:         es:[edi]- указатель за последний байт пакета, готового к передаче
; выход:        CF      -  передано,  нет
;
PUMA_Send_Tar   PROC
                push    ds
                push    es
                pushad
                ; --- проверочки
                mov     ax, es                  ;
                cmp     ax, ss:SEG_PUMA_SCR     ;
                jne     @@not_OK                ;
                lea     eax, buff_Tar           ;
                cmp     edi, eax                ;
                jb      @@not_OK                ;
                add     eax, 270                ;
                cmp     edi, eax                ;
                jae     @@not_OK                ;
                cmp     es:bTarChnl, _PUMA_CHNLS_CNT ;
                jae     @@not_OK                     ;
                ; --- определяем длину передаваемого пакета
                mov     ecx, edi                ;
                lea     eax, pre_buff_Tar       ;
                sub     ecx, eax                ;
                ; собственно передача
                movzx   edx, es:bTarChnl        ;
                shl     edx, 1                  ;
                lea     esi, pre_buff_Tar       ;
                push    ds                      ;
                mov     ds, ss:SEG_PUMA_SCR     ;
                mov     es, ss:SEG_PUMA         ;
                call    RB_MPut                 ;
                pop     ds                      ;
                or      edx, edx                ;
                jz      @@not_OK                ;
@@OK:           stc                             ;
                popad                           ;
                pop     es                      ;
                pop     ds                      ;
                ret                             ;
@@not_OK:       clc                             ;
                popad                           ;
                pop     es                      ;
                pop     ds                      ;
                ret                             ;
PUMA_Send_Tar   ENDP






