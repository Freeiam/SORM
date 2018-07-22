Clear_ULOG      PROC
                ; --- очистка SEG_ULOG
                mov     Cfg_ULOG_Count, 0       ; !!!
                mov     es, ss:SEG_ULOG
                xor     edi, edi
                mov     al, 0ffh
                xor     ecx, ecx
                mov     cl, DESCR_SEG_ULOG.ATTR_2
                mov     ah, cl
                and     cl, 0fh
                shl     ecx, 10h
                mov     cx, DESCR_SEG_ULOG.LIM
                test    ah, 80h
                jz      SHORT @@clear_ULOG
                shl     ecx, 12
@@clear_ULOG:   @_RSTOSB
                ret
Clear_ULOG      ENDP

; === работа с номерами УПАТС ===========================================================
;
; --- преобразование номера из формата СИДС в двоичное DD -------------------------------
; вход:         eax - номер в формате СИДС: ax - № телефона в УПАТС, следующий байт - № УПАТС,
; выход:        eax - номер в формате ULOG
;
@Format_U_Num   MACRO
                and     eax, 0ffffffh
                ENDM

; --- найти номер в SEG_ULOG ------------------------------------------------------------
; вход:         eax - номер УПАТС в формате ULOG
; выход:        CF  -  ОК,  не найден
;               edx - абс. физ. № линии в формате СИДС
;               esi - указатель на место, где прекращён поиск
;
Find_U_Num      PROC
                ; --- начальные установки
                push    ds
                push    ebx
                push    ecx
                mov     ds, ss:SEG_ULOG         ; сегмент номеров телефонов
                mov     esi, 03ffh              ; середина (если * 8 и не брать последние 64 к)
                mov     cx, 10                  ; максимальное количество итераций поиска
                ; --- цикл поиска
@@loop:         xor     ebx, ebx                ; 0
		cmp     eax, [esi * 8]          ;
                je      @@found                 ; равны => нашли, выходим
                jb      @@below                 ; меньше => на уменьшение esi
@@above:        mov     bl, 10b                 ; увеличить ESI xor 1100..00b
@@below:        or      bl, 01b                 ; уменьшить ESI xor 0100..00b
                dec     cx                      ; превращаем № итерации в степень двойки
                shl     ebx, cl                 ; задаём изменяемые разряды esi
                xor     esi, ebx                ; следующее деление отрезка пополам
                inc     cx                      ; восстановим № итерации
                loop    @@loop
                ; --- последняя проверка
                cmp     eax, [esi * 8]          ;
                je      @@found                 ; равны => нашли, выходим
		jb	@@not_found
		inc	esi
                ; --- не нашли
@@not_found:    shl     esi, 3
                pop     ecx
                pop     ebx
                pop     ds
                clc
                ret
                ; --- нашли
@@found:        shl     esi, 3
                mov     edx, [esi + 4]
                pop     ecx
                pop     ebx
                pop     ds
                stc
                ret
Find_U_Num      ENDP

Find_U_Num_Far  PROC FAR
                call    Find_U_Num
                retf
Find_U_Num_Far  ENDP

; --- добавить номер в SEG_ULOG ---------------------------------------------------------
; вход:         eax - номер УПАТС в формате СИДС
;               edx - абс. физ. № линии в формате СИДС
; выход:        CF  -  ОК,  не выполнено
;               eax - номер в формате ULOG
;               esi - указатель на вставленный номер
;               данные в SEG_KONF
;
Add_U_Num       PROC
                push    ecx
                @Format_U_Num                   ; номер в форму пригодную для двоичного поиска
                mov     es, ss:SEG_ULOG
                xor     esi, esi                ; если будет пусто, вставим в начало ULOG
                mov     ecx, Cfg_ULOG_Count     ; есть номера в ULOG ?
                jecxz   @@insert                ; НЕТ -
                cmp     ecx, _CFG_ULOG_MAX - 1  ; ДА - но не много ли их ?
                jae     @@not_add               ; МНОГО -
                push    edx
                call    Find_U_Num              ; ищем
                pop     edx
                jnc     @@not_found             ; НЕ нашли -
                ; --- очень странно - нашли !!! => просто заменим АФН
@@found:        mov     es:[esi + 4], edx       ; на всякий случай перезапишем АФН этого лог. номера
                pop     ecx
                stc                             ; всё равно ОК ?
                ret
                ; --- очень нормально - не нашли => раздвинем ULOG, вставим номера и Count++
@@not_found:    ; раздвигаем
                push    edi
                push    esi
                shl     ecx, 3                  ; 8 байт на номер
                mov     edi, ecx                ; указатель на начало свободного номера в ULOG
		add	edi, 4
                sub     ecx, esi                ; столько байт надо подвинуть
                shr     ecx, 2                  ; а столько DD
                mov     esi, edi                ; \ отсюда начнём
                sub     esi, 8                  ; / двигать
                push    ds
                mov     ds, ss:SEG_ULOG
                std                             ; обратный сдвиг
                @_RMOVSD                        ; двигаем
                cld                             ; восстановим флаг направления
                pop     ds
                pop     esi
                pop     edi
@@insert:       ; вставляем
		mov     es:[esi], eax           ; запоминаем ЛН
                mov     es:[esi + 4], edx       ; запоминаем АФН
                ; отмечаем
                inc     Cfg_ULOG_Count          ; отмечаем что стало на номер больше
                pop     ecx
                stc                             ; ОК
                ret
                ; --- не смогли вставить
@@not_add:      pop     ecx
                clc                             ; не ОК
                ret
Add_U_Num       ENDP

; --- удалить номер из SEG_ULOG ---------------------------------------------------------
; вход:         eax - номер УПАТС в формате СИДС
; выход:        CF  -  ОК,  не выполнено
;               eax - номер в формате ULOG
;               edx - АФН удалённого номера
;               esi - указатель на удалённый номер
;               данные в SEG_KONF
;
Del_U_Num       PROC
                cmp     Cfg_ULOG_Count, 0       ; есть что удалять ?
                je      @@not_del               ; НЕЧЕГО -
                @Format_U_Num                   ; преобразуем номер
                call    Find_U_Num              ; ищем
                jnc     @@not_del               ; НЕ нашли -
                ; --- нашли, щас удалим
                push    ecx
                push    edi
                push    esi
                mov     ecx, Cfg_ULOG_Count     ; столько номеров в SEG_ULOG
                shl     ecx, 3                  ; 8 байт на номер
                sub     ecx, esi                ; столько байт надо подвинуть
                shr     ecx, 2                  ; а столько DD
                mov     edi, esi                ; сюда будем надвигать
                add     esi, 8                  ; отсюда будем двигать
                push    ds
                mov     ds, ss:SEG_ULOG
                mov     es, ss:SEG_ULOG
                cld                             ; кашу маслом не испортишь !
                @_RMOVSD                        ; двигаем
                pop     ds
                pop     esi
                pop     edi
                pop     ecx
                dec     Cfg_ULOG_Count          ; отметим что один номер удалили
                stc                             ; ОК
                ret
                ; --- не удалили
@@not_del:      clc                             ; не ОК
                ret
Del_U_Num       ENDP

; === сессии с участием абонентов УПАТС =========================
;
; --- распаковка двоичного № УПАТС в указанный буфер
; вход:         eax     - двоичный № УПАТС в формате СИДС
;               es:[si] - указатель на буфер: признак, длина и сам номер
;
Ux_UnPack       PROC
                push    ebx
                mov     BYTE PTR es:[si], PRIZN_STAN ; признак номера - всегда станция !
                mov     BYTE PTR es:[si + 1], 3      ; длина номера - 2 цифры на № УПАТС и 1 на № в УПАТС
                cmp     ax, 10                       ;
                jb      @@clear_num                  ; № в УПАТС 1 - цифра
                mov     BYTE PTR es:[si + 1], 4      ; длина номера - 2 цифры на № УПАТС и 2 на № в УПАТС
                cmp     ax, 100
                jb      @@clear_num                  ; № в УПАТС 2 - цифры
                mov     BYTE PTR es:[si + 1], 5      ; длина номера - пока 2 цифры на № УПАТС и 3 на № в УПАТС
                cmp     ax, 1000                     ; УПАТС трёх- или четырёхзначный ?
                jb      @@clear_num                  ; ТРЁХ-
                mov     BYTE PTR es:[si + 1], 6      ; ЧЕТЫРЁХ- плюс ещё одна цифра
@@clear_num:    ; --- чистим сам номер
                add     si, 2
                mov     DWORD PTR es:[si],     0ffffffffh
                mov     DWORD PTR es:[si + 4], 0ffffffffh
                mov     BYTE  PTR es:[si + 8], 0ffh
                ; --- распакуем № УПАТС
                and     eax, 00ffffffh          ; номер УПАТС - это всегда 3 байта !
                mov     ecx, eax                ; сохраним
                shr     eax, 16                 ; байт с № УПАТС
                aam
                shl     al, 4
                shr     ax, 4
                rol     al, 4
                mov     es:[si], al             ; номер начинается с номера УПАТС
                ; --- распаукем № телефона в УПАТС
                mov     ax, cx                  ; слово с № телефона в УПАТС
                xor     dx, dx
                mov     bx, 100
                div     bx
                ; первые две цифры
                aam
                shl     al, 4
                shr     ax, 4
                rol     al, 4
                ; вторые две цифры
                xchg    dx, ax
                aam
                shl     al, 4
                shr     ax, 4
                rol     al, 4
                ; склеиваем
                shl     ax, 8
                mov     al, dl
                ; проверяем длину
                cmp     cx, 1000
                jae     @@put_num
                cmp     cx, 100
                jae     @@3_digs
                cmp     cx, 10
                jae     @@2_digs
                ; --- 1 цифра
                shr     ax, 12
                or      ax, 0FFF0h
                jmp     @@put_num
                ; --- 2 цифры
   @@2_digs:    shr     ax, 8
                or      ax, 0FF00h
                jmp     @@put_num
                ; --- 3-х значный номер
   @@3_digs:    shr     ax, 4
                or      ax, 0f000h
@@put_num:      mov     es:[si + 1], ax         ; теперь № телефона в УПАТС
                pop     ebx
                ret
Ux_UnPack       ENDP

; --- перенос двоичного № УПАТС из соединения в сессию
; вход:         dl              - 0 - upats_a_num, 1 - upats_b_num
;               fs:[edi]        - указатель на соединение
;               es:[si]         - указатель на сессию
;
Ux_Session      PROC
                pushad
                mov     eax, DWORD PTR fs:[edi].upats_a_num
                mov     bx, sorm_attr_a
                or      dl, dl
                jz      @@begin
                mov     eax, DWORD PTR fs:[edi].upats_b_num
                mov     bx, sorm_attr_b
@@begin:        and     eax, 0ffffffh
                add     si, bx
                call    Ux_UnPack
                popad
                ret
Ux_Session      ENDP

; ===================-- ИСКУССТВЕННЫЕ СЕССИИ --===============================
; --- создание сесии для звонка с не наблюдаемого номера в УПАТС на наблюдаемый Nб
;
Session_Nb_Ua_Create    PROC
                test    fs:[edi].soed_flags, sf_UPATS_A         ;
                jz      @@exit                                  ;
                push    eax dx
                ; --- начинаем создание сессии
                mov     dl, 2                                   ;
                mov     eax, called_Ua_ptr
                @Session_Create                                 ;
                jnc     @@quit                                  ;
                ; --- заносим вызывающего из УПАТС в сессию как № А
                xor     dl, dl                                  ;
                call    Ux_Session                              ;
                ; --- связываем соединение и сессию
                mov     fs:[edi].called_Ua_ptr, si              ;
                ; --- а теперь вызываем СОРМ
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ---
                ; ----{Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {END} -------
   @@quit:      pop     dx eax
                ; ---
@@exit:         ret
Session_Nb_Ua_Create    ENDP

; --- создание сесии для звонка с не наблюдаемого номера в УПАТС на наблюдаемый Nб
;
Session_Nb_mg_Ua_Create PROC
                test    fs:[edi].soed_flags, sf_UPATS_A         ;
                jz      @@exit                                  ;
                push    eax dx
                ; --- начинаем создание сессии
                mov     dl, 2                                   ;
                mov     eax, called_mg_Ua_ptr                   ;
                @Session_Create                                 ;
                jnc     @@quit                                  ;
                ; --- заносим вызывающего из УПАТС в сессию как № А
                xor     dl, dl                                  ;
                call    Ux_Session                              ;
                ; --- связываем соединение и сессию
                mov     fs:[edi].called_mg_Ua_ptr, si           ;
                ; --- а теперь вызываем СОРМ
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ---
                ; ----{Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {END} -------
     @@quit:    pop     dx eax
                ; ---
  @@exit:       ret
Session_Nb_mg_Ua_Create ENDP
; -------------------------------
; ---  Создание сессии для звонка с не наблюдаемого №A на наблюдаемый №УПАТС
; --- in:       gs:[bx]   - ук-ль на объект
;               fs:[edi]  - указатель на соединение
Session_Na_Ub_Create    PROC
                push    eax dx
                ; --- Пробуем создать сессию ->
                mov     dl, 2
                mov     eax, Ub_calling_ptr
                @Session_Create
                jnc     @@quit
                ; --- заносим вызывающего из УПАТС в сессию как № Б
                mov     dl, 1                                  ;
                call    Ux_Session
                ; --- {Paul.N} --- <29/04/13>
                @_Clear_tcop_dig                 ; уберем инфу о цифре выхода в ТСОП
                ; --- {Paul.N} --- <END>
                ; --- связываем соединение и сессию
                mov     fs:[edi].Ub_calling_ptr, si
                ; --- а теперь вызываем СОРМ
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ---
                ; ----{Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {END} -------
     @@quit:    pop     dx eax
                ; ---
                ret
Session_Na_Ub_Create    ENDP
; -------------------------------

; ---  Создание сессии для звонка с наблюдаемого №УПАТС A на не наблюдаемый №Б
; --- in:       gs:[bx]   - ук-ль на объект
;               fs:[edi]  - указатель на соединение
Session_Ua_Nb_Create    PROC
                push    eax dx
                ; --- создаём сессию
                xor     dl, dl                                  ;
                xor     eax, eax                                ;
                @Session_Create                                 ;
                jnc     @@quit                                  ;
                ; --- заносим вызывающего из УПАТС в сессию как № А
                xor     dl, dl                                  ;
                call    Ux_Session                              ;
               ; -- связываем соединение и сессию
                mov     fs:[edi].Ua_called_ptr, si              ;
                add     es:[si].sorm_soed, Ua_called_ptr        ;
                ; --- а теперь вызываем СОРМ
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ----{Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {END} -------
    @@quit:     pop     dx eax
                ; ---
                ret
Session_Ua_Nb_Create    ENDP
; ---  Создание сессии для звонка с наблюдаемого №A на ненаблюдаемый №УПАТС
; --- in:       gs:[bx]   - ук-ль на объект
;               fs:[edi]  - указатель на соединение
Session_calling_Ub      PROC

                test    fs:[edi].soed_flags, sf_UPATS_B
                jz      @@exit
                push    eax dx
                ; ---  возможна сессия для УПАТС Б
                xor     dl, dl
                mov     eax, calling_Ub_ptr
                @Session_Create
                jnc     @@quit
                ; --- заносим вызывающего из УПАТС в сессию как № Б
                mov     dl, 1                    ;
                call    Ux_Session               ;
                ; --- {Paul.N} --- <29/04/13>
                @_Clear_tcop_dig                 ; уберем инфу о цифре выхода в ТСОП
                ; --- {Paul.N} --- <END>
                ; --- связываем соединение и сессию
                mov     fs:[edi].calling_Ub_ptr, si              ;
                ; --- а теперь вызываем СОРМ
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ---
                ; ----{Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {END} -------
      @@quit:   pop     dx eax
                ; ---
    @@exit:     ret
Session_calling_Ub      ENDP

; --- проверяем наблюдается ли номер УПАТС
; вход:         gs      - SEG_OBJ
;               eax     - двоичный № в УПАТС
; выход:        gs:[bx] - указатель на найденный объект
;
Ux_Find         PROC
                cmp     Obj_Count, 0            ;
                je      @@exit                  ; таблица пуста !
                ; ---
                push    ds
                pop     es
                lea     si, UPATS_Buff
                call    Ux_UnPack
                ; ---
                lea     bx, Objects             ;
                mov     cx, Obj_Count           ;
@@find:         cmp     WORD PTR gs:[bx].obj_type, 0ffffh  ;
                je      @@next                  ;
                cmp     gs:[bx].obj_type, 0ffh  ;
                jne     @@next                  ;
                mov     edx, es:[si]            ;
                cmp     edx, DWORD PTR gs:[bx].obj_tel_num
                je      @@found                 ;
@@next:         add     bx, _OBJ_SIZE           ;
                loop    @@find                  ;
                clc                             ;
                ret
                ; ---
@@found:        stc                             ;
@@exit:         ret
Ux_Find         ENDP

; --- проверка наблюдается ли вызывающий № из УПАТС
;
Ua_Received     PROC
                test    fs:[edi].soed_flags, sf_UPATS_CALLED    ;
                jnz     @@exit                                  ;
                ; ---
                test    fs:[edi].soed_flags, sf_UPATS_A         ;
                jz      @@exit                                  ;
                ; ---
                test    fs:[edi].soed_flags, sf_NUM_FULL        ;
                jz      @@exit;;;@@continue                              ;

                ;test    fs:[edi].soed_flags, sf_UPATS_B         ;
                ;jz      @@exit                                  ;
                ; ---
                ;;;@@continue:
                or      fs:[edi].soed_flags, sf_UPATS_CALLED    ;
                cmp     Obj_Count, 0                            ;
                je      @@exit                                  ; таблица пуста !
                mov     gs, ss:SEG_OBJ                          ;
                ; --- ищем вызывающего из УПАТС в списке объектов
                mov     eax, DWORD PTR fs:[edi].upats_a_num     ;
                push    cx si edx
                push    es
                call    Ux_Find                                 ;
                pop     es
                pop     edx si cx
                jnc     @@exit                                  ; НЕ НАШЛИ
                ; --- Нашли, продолжим!
                ; кто Б - УПАТС или обычный телефон ?
                test    fs:[edi].soed_flags, sf_UPATS_B         ;
                jz      @@usual_Nb                              ; ОБЫЧНЫЙ
                ; --- создаём сессию
                xor     dl, dl                                  ;
                xor     eax, eax                                ;
                @Session_Create                                 ;
                jnc     @@exit                                  ;
                ; --- заносим вызывающего из УПАТС в сессию как № А
                xor     dl, dl                                  ;
                call    Ux_Session                              ;
                ; №б в УПАТС
                mov     dl, 1                                   ;
                call    Ux_Session                              ;
                ; --- {Paul.N} --- <29/04/13>
                @_Clear_tcop_dig                 ; уберем инфу о цифре выхода в ТСОП
                ; --- {Paul.N} --- <END>
                ; связываем соединение и сессию
                mov     fs:[edi].upats_ab_ptr, si               ;
                add     es:[si].sorm_soed, upats_ab_ptr         ;
                ; --- а теперь вызываем СОРМ
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ---- {Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {Paul} --- <END>
                ; --- возможна также сессия ->
                ; --- {Paul.N} --- [27/05/13]
                test    fs:[edi].soed_flags, sf_CALL_UPATS
                jz      @@usual_Nb
                ; --- только при наборе номера упатс!
                ; -- и только в МО 112
                cmp     BYTE PTR ds:[bp], 112
                jne     @@exit
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
                ; --- а теперь можно создавать сессию
   @@usual_Nb:  call    Session_Ua_Nb_Create
                ret
@@exit_1:       pop     edi
                pop     gs
@@exit_2:       pop     cx bx edx eax
                ; --- {Paul.N} -- [THE END]
@@exit:         ret
Ua_Received     ENDP

; --- проверка наблюдается ли вызываемый № из УПАТС
;
Ub_Received     PROC
                test    fs:[edi].soed_flags, sf_UPATS_B         ;
                jz      @@exit                                  ;
                ; ---
                cmp     Obj_Count, 0                            ;
                je      @@exit                                  ; таблица пуста !
                mov     gs, ss:SEG_OBJ                          ;
                ; --- ищем вызывающего из УПАТС в списке объектов
                mov     eax, DWORD PTR fs:[edi].upats_b_num     ;
                push    cx si edx
                push    es
                call    Ux_Find                                 ;
                pop     es
                pop     edx si cx
                jnc     @@exit                                  ; НЕ НАШЛИ
                ; -- {Paul.N - add - 19/07/17}
 @@Go_Na_Ub:    test    fs:[edi].soed_flags, sf_ANI             ; есть №а
                jnz     @@Call_Na_Ub;; @@exit                   ; есть - пробуем отработать и по гор. и по УПАТС
                or      fs:[edi].soed_flags, sf_2VSK_Chelnok    ;<- отметим "челнок", т.к здесь СОРМ работает без АОНа
                jmp     @@Go_Upats_A
                ; ---
@@Call_Na_Ub:   call    Session_Na_Ub_Create
                ; --- возможна также сессия ->
                test    fs:[edi].soed_flags, sf_2VSK_Chelnok    ;<- если был "челнок", то по УПАТС снова не формируем
                jnz     @@exit
@@Go_Upats_A:   test    fs:[edi].soed_flags, sf_UPATS_A
                jz      @@exit;;;@@Go_Na_Ub                     ; только сессия для А городского
                ; --- у вызывающего ТОЧНО ДОЛЖЕН БЫТЬ № в УПАТС
                mov     dl, 2                                   ;
                xor     eax, eax                                ;
                @Session_Create                                 ;
                jnc     @@exit                                  ;
                ; заносим вызывающего из УПАТС в сессию как № А
                xor     dl, dl                                  ;
                call    Ux_Session                              ;
                ; заносим вызываемого из УПАТС в сессию как № Б
                mov     dl, 1                                   ;
                call    Ux_Session                              ;
                ; --- {Paul.N} --- <29/04/13>
                @_Clear_tcop_dig                 ; уберем инфу о цифре выхода в ТСОП
                ; --- {Paul.N} --- <END>
                ; связываем соединение и сессию
                mov     fs:[edi].upats_ba_ptr, si               ;
                add     es:[si].sorm_soed, upats_ba_ptr         ;
                ; вызываем СОРМ
                mov     dh, OSV_PRI                             ;
                call    KSL_Get                                 ;
                mov     dl, SO_NOM                              ;
                call    Message_SORM                            ;
                ; ---- {Paul} ------ (09/11/11) ---
                @Try_Hot_tel
                ; --- {Paul} -- <END>
@@exit:         ret
Ub_Received     ENDP

