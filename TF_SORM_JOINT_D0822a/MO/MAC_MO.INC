;********************************************************************************
;**                       ОПИСАНИЕ МАКРОСОВ для МО                             **
;**                   AT! все макросы начинаються с @                          **
;********************************************************************************


@POLUCH_ADR_MO  MACRO
                CALL    VIBOR_MOD
		ENDM

@ZAPIS_OK_MO	MACRO
                CALL    ZAPIS_OK_MOO
		ENDM


@MOIN_1         MACRO
		PUSH	BX
		PUSH	DI
		PUSH	ES
                CALL    MOIN_1
		POP	ES
		POP	DI
		POP	BX
                ENDM




@_STOSB         MACRO
                DB      67H
                STOSB
                ENDM

@_STOSW         MACRO
                DB      67H
                STOSW
                ENDM

@_STOSD         MACRO
                DB      67H
                STOSD
                ENDM

@_LODSB         MACRO
                DB      67H
                LODSB
                ENDM

@_LODSW         MACRO
                DB      67H
                LODSW
                ENDM

@_MOVSB         MACRO
                DB      67H
                MOVSB
                ENDM

@_MOVSD         MACRO
                DB      67H
                MOVSD
                ENDM

@_RMOVSB        MACRO
                DB      67H
        REP     MOVSB
                ENDM

@_RMOVSW        MACRO
                DB      67H
        REP     MOVSW
                ENDM

@_RMOVSD        MACRO
                DB      67H
        REP     MOVSD
                ENDM

@_RSTOSB        MACRO
                DB      67H
        REP     STOSB
                ENDM

@_RSTOSW        MACRO
                DB      67H
        REP     STOSW
                ENDM

@_RSTOSD        MACRO
                DB      67H
        REP     STOSD
                ENDM


; --- макрос отпраки 169-ой команды
; вход:         ds:bp  - указатель на только что принятую команду
;               fs:edi - указатель на SOED
;
@Send_MO169     MACRO
                LOCAL   @@No_Send
                MOV     ESI, EDI                ; Теперь edi в esi
                @POLUCH_ADR_MO
                OR      DL,DL                   ; проверим есть место ?
                JNZ     @@No_Send               ; нет - уйдем
                ; есть место - все же запишем
if _NEW_REL_FROM_TS EQ TRUE
                mov     eax, 070404ebh          ; Команда 169 - абонент в черном списке
                stosd
endif
                MOV     AL, 169                 ; Команда 169 - абонент в черном списке
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
                mov     AX, WORD PTR FS:SOED[ESI+10]; абсолютный физический номер А
                STOSW
if _NEW_REL_FROM_TS EQ TRUE
                mov     al, 3
                stosb
endif
                @ZAPIS_OK_MO
                ; --- для совместимости со старой версией ----------------------
                @POLUCH_ADR_MO
                OR      DL,DL                   ; проверим есть место ?
                JNZ     @@NO_Send               ; нет - уйдем
                ; есть место - все же запишем
                mov     al, 169
                stosb
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
                mov     AX, WORD PTR FS:SOED[ESI+10]; абсолютный физический номер А
                STOSW
                @ZAPIS_OK_MO
                ; --- для совместимости со старой версией ----------------------
@@NO_Send:      mov     edi, esi
                ENDM

; --- макрос отпраки 169-ой команды
; вход:         ds:bp  - указатель на только что принятую команду
;               fs:edi - указатель на SOED
;
@Send_MO169     MACRO
                LOCAL   @@No_Send
                MOV     ESI, EDI                ; Теперь edi в esi
                @POLUCH_ADR_MO
                OR      DL,DL                   ; проверим есть место ?
                JNZ     @@No_Send               ; нет - уйдем
                ; есть место - все же запишем
                mov     eax, 070404ebh          ; Команда 169 - абонент в черном списке
                stosd
                MOV     AL, 169                 ; Команда 169 - абонент в черном списке
                STOSB
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
                mov     ax, WORD PTR fs:[esi].phys_a; абсолютный физический номер А
                STOSW
                mov     al, 3
                stosb
                @ZAPIS_OK_MO
                ; --- для совместимости со старой версией ----------------------
                @POLUCH_ADR_MO
                OR      DL,DL                   ; проверим есть место ?
                JNZ     @@NO_Send               ; нет - уйдем
                ; есть место - все же запишем
                mov     al, 169
                stosb
                MOV     AL, BYTE PTR DS:[BP+1]  ; номер тысячника
                STOSB
                mov     AX, WORD PTR DS:[BP+2]  ; абсолютный рабочий номер
                STOSW
                mov     ax, WORD PTR fs:[esi].phys_a; абсолютный физический номер А
                STOSW
                @ZAPIS_OK_MO
                ; --- для совместимости со старой версией ----------------------
@@NO_Send:      mov     edi, esi
                ENDM


; === макросы поиска вхождений в таблицы наблюдаемых объектов (№ и пучков) ====
;
; --- поиск вхождения короткого номера из соединения (без кода зоны) в список наблюдаемых объектов
; вход:         константа num_x из SOED_Struc
;               fs:[edi]        - уазатель на соединение
; выход:        флаг переноса   - CF  - номер не на контроле, CF  - номер на контроле
;
@Is_Short_Num_on_Ctrl MACRO   what_num
                LOCAL   @@cmp, @@exit, @@find, @@found, @@next, @@nothing

		push	edi
		add	edi, what_num		;
		; ---
                lea     bx, Objects             ;
                mov     cx, Obj_Count           ;
                jcxz    @@nothing
@@find:         cmp     gs:[bx].obj_type, 0ffh  ;
                je      @@next                  ;
		mov	dl, fs:[edi - 2]        ;
                cmp     dl, gs:[bx].obj_attr    ;
		jne	@@next                  ;
                xor     si, si                  ;
                cmp     gs:[bx].obj_type, NEPOLN_NOM
                jne     @@cmp                   ;
                cmp     BYTE PTR fs:[edi - 2], PRIZN_STAN
                je      @@next                  ;
                movzx   si, gs:[bx].obj_tel_len ;
                shl     si, 2                   ;
@@cmp:          mov     edx, fs:[edi]		;
                or      edx, Obj_Num_Mask_1[si] ;
                cmp     edx, DWORD PTR gs:[bx].obj_tel_num
                je      @@found                 ;
@@next:         add     bx, _OBJ_SIZE           ;
                loop    @@find                  ;
@@nothing:      clc                             ;
                jmp     @@exit                  ;
                ; ---
@@found:        stc
@@exit:         pop	edi
                ENDM

; --- поиск вхождения номера из соединения в список наблюдаемых объектов
; вход:         константа num_x из SOED_Struc
;               fs:[edi]        - уазатель на соединение
; выход:        флаг переноса   - CF  - номер не на контроле, CF  - номер на контроле
;
@Is_Num_on_Ctrl MACRO   what_num
                LOCAL   @@cmp, @@exit, @@find, @@found, @@next, @@nothing
                push    edi
                add     edi, what_num           ;
                lea     bx, Objects             ;
                mov     cx, Obj_Count           ;
                jcxz    @@nothing
@@find:         cmp     gs:[bx].obj_type, 0ffh  ;
                je      @@next                  ;
		mov	dl, fs:[edi - 2]        ;
                cmp     dl, gs:[bx].obj_attr    ;
		jne	@@next                  ;
                xor     si, si                  ;
                cmp     gs:[bx].obj_type, NEPOLN_NOM
                jne     @@cmp                   ;
                cmp     BYTE PTR fs:[edi - 2], PRIZN_STAN
                je      @@next                  ;
                movzx   si, gs:[bx].obj_tel_len ;
                shl     si, 2                   ;
@@cmp:          mov     edx, DWORD PTR fs:[edi]
                or      edx, Obj_Num_Mask_1[si] ;
                cmp     edx, DWORD PTR gs:[bx].obj_tel_num
                jne     @@next                  ;
                mov     edx, DWORD PTR fs:[edi + 4]
                or      edx, Obj_Num_Mask_2[si] ;
                cmp     edx, DWORD PTR gs:[bx + 4].obj_tel_num
                jne     @@next                  ;
                mov     dl, fs:[edi + 8]        ;
                shr     si, 2                   ;
                or      dl, Obj_Num_Mask_3[si]  ;
                cmp     dl, gs:[bx + 8].obj_tel_num
                je      @@found                 ;
@@next:         add     bx, _OBJ_SIZE           ;
                loop    @@find                  ;
@@nothing:      clc                             ;
                jmp     @@exit                  ;
                ; ---
@@found:        stc                             ;
@@exit:         pop     edi                     ;
                ENDM


; --- если №Б - это выход на МН с выбором оператора, он наблюдается ?
; вход:         fs:[edi]        - уазатель на соединение
; выход:        флаг переноса   - CF  - номер не на контроле, CF  - номер на контроле
;
@Is_Take_MN_Op_on_Ctrl  MACRO
                LOCAL   @@cmp, @@exit, @@find, @@found, @@next, @@quit, @@outgo, @@nothing

                ; --- входные проверки - не во всяком номере может быть код выбора МН оператора
                mov     al, fs:[edi].num_b              ;
                and     al, 0fh                         ;
                cmp     al, 8                           ; это МГ номер ?
                jne     @@outgo                         ; НЕТ - значит и выбора оператора нет
                cmp     fs:[edi].len_b, 13              ; если выбор оператора, то МГ номер минимум 11+2 знаков
                jb      @@outgo                         ;
                cmp     fs:[edi].attr_b, PRIZN_MESHGOR  ;
                jne     @@outgo                         ;
                ; --- сохраним номер Б из соединения
                push    DWORD PTR fs:[edi].num_b        ;
                push    DWORD PTR fs:[edi + 4].num_b    ;
                push    WORD PTR  fs:[edi + 8].num_b    ;
                ; --- в номер Б вместо 2 и 3 цифр впишем 10 - МН префикс
                and     WORD PTR  fs:[edi].num_b, 0f00fh
                or      WORD PTR  fs:[edi].num_b, 00010h
                ;
                lea     bx, Objects                     ;
                mov     cx, Obj_Count                   ;
                jcxz    @@nothing
@@find:         cmp     gs:[bx].obj_type, 0ffh          ;
                je      @@next                          ;
                cmp     gs:[bx].obj_attr, PRIZN_MESHDUN ;
                jne     @@next                          ;
                xor     si, si                          ;
                cmp     gs:[bx].obj_type, NEPOLN_NOM    ;
                jne     @@cmp                           ;
                movzx   si, gs:[bx].obj_tel_len         ;
                shl     si, 2                           ;
@@cmp:          mov     edx, DWORD PTR fs:[edi].num_b   ;
                or      edx, Obj_Num_Mask_1[si]         ;
                cmp     edx, DWORD PTR gs:[bx].obj_tel_num
                jne     @@next                          ;
                mov     edx, DWORD PTR fs:[edi + 4].num_b;
                or      edx, Obj_Num_Mask_2[si]         ;
                cmp     edx, DWORD PTR gs:[bx + 4].obj_tel_num
                jne     @@next                          ;
                mov     dl, fs:[edi + 8].num_b          ;
                shr     si, 2                           ;
                or      dl, Obj_Num_Mask_3[si]          ;
                cmp     dl, gs:[bx + 8].obj_tel_num     ;
                je      @@found                         ;
@@next:         add     bx, _OBJ_SIZE                   ;
                loop    @@find                          ;
@@nothing:      pop     WORD PTR  fs:[edi + 8].num_b    ;
                pop     DWORD PTR fs:[edi + 4].num_b    ;
                pop     DWORD PTR fs:[edi].num_b        ;
@@outgo:        clc                                     ;
                jmp     @@quit                          ;
                ; ---
@@found:        stc                                     ;
@@exit:         pop     ax                              ;
                pop     eax                             ;
                pop     eax                             ;
@@quit:
                ENDM


; --- если №Б - это выход на МГ с выбором оператора, он наблюдается ?
; вход:         fs:[edi]        - уазатель на соединение
; выход:        флаг переноса   - CF  - номер не на контроле, CF  - номер на контроле
;
@Is_Take_MG_Op_on_Ctrl  MACRO
                LOCAL   @@cmp, @@exit, @@find, @@found, @@next, @@quit, @@outgo, @@nothing

                ; --- входные проверки - не во всяком номере может быть код выбора МН оператора
                mov     al, fs:[edi].num_b              ;
                and     al, 0fh                         ;
                cmp     al, 8                           ; это МГ номер ?
                jne     @@outgo                         ; НЕТ - значит и выбора оператора нет
                cmp     fs:[edi].len_b, 13              ; если выбор оператора, то МГ номер минимум 11+2 знаков
                jb      @@outgo                         ;
                cmp     fs:[edi].attr_b, PRIZN_MESHGOR  ;
                jne     @@outgo                         ;
                ; --- сохраним номер Б из соединения
                push    DWORD PTR fs:[edi].num_b        ;
                push    DWORD PTR fs:[edi + 4].num_b    ;
                push    WORD PTR  fs:[edi + 8].num_b    ;
                ; --- в номер Б удаляем цифры 2 и 3 - возможно это код оператора
                mov     ax, WORD PTR  fs:[edi].num_b    ;
                and     ax, 0f00fh                      ;
                or      al, ah                          ;
                mov     fs:[edi].num_b, al              ;
                mov     eax, DWORD PTR fs:[edi + 2].num_b
                mov     DWORD PTR fs:[edi + 1].num_b, eax
                mov     ax, WORD PTR fs:[edi + 6].num_b ;
                mov     WORD PTR fs:[edi + 5].num_b, ax ;
                mov     al, fs:[edi + 8].num_b          ;
                mov     ah, 0ffh                        ;
                mov     WORD PTR fs:[edi + 7].num_b, ax ;
                ;
                lea     bx, Objects                     ;
                mov     cx, Obj_Count                   ;
                jcxz    @@nothing
@@find:
                cmp     gs:[bx].obj_type, 0ffh          ;
                je      @@next                          ;
                cmp     gs:[bx].obj_attr, PRIZN_MESHGOR ;
                jne     @@next                          ;
                xor     si, si                          ;
                cmp     gs:[bx].obj_type, NEPOLN_NOM    ;
                jne     @@cmp                           ;
                movzx   si, gs:[bx].obj_tel_len         ;
                shl     si, 2                           ;
@@cmp:          mov     edx, DWORD PTR fs:[edi].num_b   ;
                or      edx, Obj_Num_Mask_1[si]         ;
                cmp     edx, DWORD PTR gs:[bx].obj_tel_num
                jne     @@next                          ;
                mov     edx, DWORD PTR fs:[edi + 4].num_b;
                or      edx, Obj_Num_Mask_2[si]         ;
                cmp     edx, DWORD PTR gs:[bx + 4].obj_tel_num
                jne     @@next                          ;
                mov     dl, fs:[edi + 8].num_b          ;
                shr     si, 2                           ;
                or      dl, Obj_Num_Mask_3[si]          ;
                cmp     dl, gs:[bx + 8].obj_tel_num     ;
                je      @@found                         ;
@@next:         add     bx, _OBJ_SIZE                   ;
                loop    @@find                          ;
@@nothing:      pop     WORD PTR  fs:[edi + 8].num_b    ;
                pop     DWORD PTR fs:[edi + 4].num_b    ;
                pop     DWORD PTR fs:[edi].num_b        ;
@@outgo:        clc                                     ;
                jmp     @@quit                          ;
                ; ---
@@found:        sub     fs:[edi].len_b, 2               ;
                stc                                     ;
@@exit:         pop     ax                              ;
                pop     eax                             ;
                pop     eax                             ;
@@quit:
                ENDM


; --- поиск вхождения физической линии а или б в наблюдаемый пучок
; вход:         константа ck_x из SOED_Struc
;               fs:[edi]        - указатель на соединение
; выход:        флаг переноса   - CF  - пучок не на контроле, CF  - пучок на контроле
;
@Is_Bunch_on_Ctrl MACRO what_ck
                LOCAL   @@exit, @@find, @@found, @@next, @@not_found

                push    edi
                add     edi, what_ck            ;
                mov     es, ss:SEG_BUNCH        ;
                movzx   ebx, BYTE PTR fs:[edi]  ; ck_a или ck_b
                shl     ebx, _PHYS_NUM_SHL      ;
                or      bx, fs:[edi + 3]        ; phys_a или phys_b
                movzx   eax, BYTE PTR es:[ebx]  ;
                cmp     al, _BUNCH_MAX          ;
                ja      @@not_found             ;
                or      al, al                  ;
                jz      @@not_found             ;
                ; --- линия входит в пучок - получаем условный номер пучка
                mov     ebx, _BUNCH_PRM_OFFS    ;
		dec	al			;
                shl     eax, _BUNCH_PRM_SHL     ;
                add     ebx, eax                ;
                mov     edx, es:[ebx]           ;
                cmp     dl, 0ffh                ;
                je      @@not_found             ;
                shr     edx, 8                  ;
                ; --- проверка наблюдается ли пучок
                mov     gs, ss:SEG_OBJ          ;
                lea     bx, Bunches             ;
                mov     cx, _BUNCH_MAX          ;
@@find:         cmp     gs:[bx].obj_type, 0ffh  ;
                je      @@next                  ;
                cmp     dx, gs:[bx].obj_bunch   ;
                je      @@found                 ;
@@next:         add     bx, _BUNCH_SIZE         ;
                loop    @@find                  ;
@@not_found:    clc                             ;
                jmp     @@exit                  ;
@@found:        stc                             ;
@@exit:         pop     edi
                ENDM


; === макросы преобразования номеров из формата СОРМ в формат NLOG =============
;
; --- преобразование номера А из формата SOED (SORM) в NLOG
; вход:         fs:[edi]        - указатель на соединение
;               LENGTH_NUM      - длина номера в станции
; выход:        eax             -
;
@Na_from_Soed_to_Log  MACRO
                push    bx                      ;
                push    cx                      ;
                mov     eax, DWORD PTR fs:[edi + 1].num_a    ; последние 8 цифр номера
                movzx   bx, LENGTH_NUM          ;
                mov     cl, Cfg_Num_Shift[bx]   ;
                shr     eax, cl                 ;
		rol	al, 4			;
		rol	ah, 4			;
                xchg    al, ah                  ;
                rol     eax, 16                 ;
		rol	al, 4			;
		rol	ah, 4			;
                xchg    al, ah                  ;
                shr     eax, cl                 ;
                pop     cx                      ;
                pop     bx                      ;
                ENDM

; --- преобразование номера Б из разных форматов SOED (SORM) в один формат NLOG
; вход:         fs:[edi]        - указатель на соединение
;               LENGTH_NUM      - длина номера в станции
; выход:        eax             -
;
@Nb_from_Soed_to_Log  MACRO
                LOCAL   @@Nb_Local
                push    bx                      ;
                push    cx                      ;
                mov     eax, DWORD PTR fs:[edi].num_b    ; первые 8 цифр номера
                movzx   bx, LENGTH_NUM          ;
                cmp     fs:[edi].len_b, 10      ;
                jb      @@Nb_Local              ;
                ;
                mov     cl, PREF_A_FULL_SHR[bx] ;
                mov     eax, DWORD PTR fs:[edi + 1].num_b; весь номер с последней цифрой кода зоны
                shr     eax, cl                 ;
                shl     bx, 2                   ;
                or      eax, PREF_A_FULL_OR[bx] ;
		shr	bx, 2			;
                ;
@@Nb_Local:     rol     al, 4                   ;
		rol	ah, 4			;
                xchg    al, ah                  ;
                rol     eax, 16                 ;
		rol	al, 4			;
		rol	ah, 4			;
                xchg    al, ah                  ;
                mov     cl, Cfg_Num_Shift[bx]   ;
                shr     eax, cl                 ;
                pop     cx                      ;
                pop     bx                      ;
                ENDM

; === макросы сохранения/преобразования и восстановления номеров ===============
;
; --- укорачивание №а (удаление кода зоны)
; вход:         fs:[edi]        - уазатель на соединение
;               LENGTH_NUM      - длина номера в станции
; выход:        нет
;
@Na_Short       MACRO
                LOCAL   @@exit

                ; проверять на короткий имеет смысл только номер АТС или местной сети
                cmp     fs:[edi].attr_a, PRIZN_SETI
                ja      @@exit
                ; обрезаем
                mov     eax, DWORD PTR fs:[edi + 1].num_a
                shr     eax, 4                  ;
                movzx   si, LENGTH_NUM          ;
                and     si, 7                   ;
		mov	cl, ANI7_Shift[si]	;
		shr	eax, cl                 ;
                shl     si, 2                   ;
                or      eax, ANI7_Mask[si]      ;
                ; заменяем
                mov     DWORD PTR fs:[edi].num_a, eax
                mov     fs:[edi + 4].num_a, 0ffh;
                mov     al, LENGTH_NUM          ;
                mov     fs:[edi].len_a, al      ;
@@exit:
                ENDM

; --- укорачивание любого № сессии (удаление кода зоны)
; вход:         es:[si]         - уазатель на сессию
;               LENGTH_NUM      - длина номера в станции
; выход:        нет
;
@Nx_Session     MACRO   what_attr
                LOCAL   @@exit
		push	eax
                push    bx
                push    cx
                push    dx
                push    si
                add     si, what_attr           ;
                movzx   bx, LENGTH_NUM          ;
                and     bx, 7                   ;
                ; проверять на короткий имеет смысл только номер АТС или местной сети
                cmp     BYTE PTR es:[si], PRIZN_SETI ; обрезаем только внутренние и местные
                ja      @@exit                  ;
                cmp     BYTE PTR es:[si + 1], 10; обрезаем только полные федеральные
                jne     @@exit                  ;
                shl     bx, 2                   ;
                mov     eax, es:[si + 2]        ; и только начинающиеся с нашего префикса
                and     eax, PREF_A_FULL_MASK[bx];
                mov     edx, PREF_A_FULL         ;
                and     edx, PREF_A_FULL_MASK[bx];
                shr     bx, 2                   ;
                cmp     eax, edx                ;
                jne     @@exit                  ;
                ; обрезаем
                mov     es:[si + 1], bl         ;
                mov     eax, es:[si + 3]        ;
                shr     eax, 4                  ;
                mov     cl, ANI7_Shift[bx]      ;
		shr	eax, cl                 ;
                shl     bx, 2                   ;
                or      eax, ANI7_Mask[bx]      ;
                ; заменяем
                mov     es:[si + 2], eax        ;
                mov     BYTE PTR es:[si + 6], 0ffh
@@exit:         pop     si
                pop     dx
                pop     cx
                pop     bx
		pop	eax
                ENDM

; --- укорачивание любого № (удаление кода зоны)
; вход:         константа num_x из SOED_Struc
;               fs:[edi]        - указатель на соединение
;               LENGTH_NUM      - длина номера в станции
; выход:        eax             - №а или б укороченный до LENGTH_NUM
;
@Nx_Short       MACRO   what_num
                LOCAL   @@exit

		push	bx
		push	cx
                push    edi
                add     edi, what_num
                ; проверяем
                movzx   bx, LENGTH_NUM          ;
		mov	eax, fs:[edi]		;
                cmp     bl, fs:[edi - 1]        ;
                je      @@exit
                ; обрезаем
                mov     eax, fs:[edi + 1]       ;
                shr     eax, 4                  ;
                and     bx, 7                   ;
                mov     cl, ANI7_Shift[bx]      ;
		shr	eax, cl                 ;
                shl     bx, 2                   ;
                or      eax, ANI7_Mask[bx]      ;
                ;
@@exit:         pop	edi
		pop	cx
		pop	bx
                ENDM

; --- сохранение во временном буфере и укорачивание любого № (удаление кода зоны)
; вход:         константа attr_x из SOED_Struc
;               fs:[edi]        - уазатель на соединение
;               LENGTH_NUM      - длина номера в станции
; выход:        флаг переноса   - CF  - номер укорочен и сохранён, CF  - номер нельзя укоротить
;
@Nx_Store_and_Short MACRO   what_attr
                LOCAL   @@exit, @@not_short

                push    cx
                push    si
                push    edi
                add     edi, what_attr
                movzx   si, LENGTH_NUM          ;
                and     si, 7                   ;
                ; проверять на короткий имеет смысл только номер АТС или местной сети
                cmp     BYTE PTR fs:[edi], PRIZN_SETI
                ja      @@not_short             ;
                cmp     BYTE PTR fs:[edi + 1], 10;
                jne     @@not_short             ;
		shl	si, 2                   ;
                mov     eax, fs:[edi + 2]       ; и только начинающиеся с нашего префикса
                and     eax, PREF_A_FULL_MASK[si];
                mov     edx, PREF_A_FULL         ;
                and     edx, PREF_A_FULL_MASK[si];
                shr     si, 2                   ;
                cmp     eax, edx                ;
                jne     @@not_short             ;
                ; сохраняем
                mov     eax, fs:[edi + 1]       ;
                mov     DWORD PTR Tmp_Num_Buff, eax
                mov     eax, fs:[edi + 5]       ;
                mov     DWORD PTR Tmp_Num_Buff[4], eax
                mov     ax, fs:[edi + 9]        ;
                mov     WORD PTR Tmp_Num_Buff[8], ax
                ; обрезаем
                mov     eax, fs:[edi + 3]       ;
                shr     eax, 4                  ;
		mov	cl, ANI7_Shift[si]	;
		shr	eax, cl                 ;
                shl     si, 2                   ;
                or      eax, ANI7_Mask[si]      ;
                ; заменяем
                mov     fs:[edi + 2], eax       ;
                mov     fs:[edi + 6], 0ffffffffh;
                mov     BYTE PTR fs:[edi + 10], 0ffh
                mov     al, LENGTH_NUM          ;
                mov     fs:[edi + 1], al        ;
                ; OK
                stc                             ;
                jmp     @@exit
                ; не ОК
@@not_short:    clc                             ;
@@exit:         pop     edi
                pop     si
                pop     cx
                ENDM

; --- сохранение №б во временном буфере и удаление МГ/МН префиксов
; вход:         fs:[edi]        - уазатель на соединение
; выход:        флаг переноса   - CF  - номер укорочен и сохранён, CF  - префикс не выделен
;
@Nx_Store_and_Del_Pref  MACRO   what_len
                LOCAL   @@exit, @@found, @@not_found, @@shift_loop

		push	eax
		push	cx
		push	dx
                push    edi
                add     edi, what_len                   ;
                ; --- сразу сохраняем №б
                mov     eax, fs:[edi]                   ;
                mov     DWORD PTR Tmp_Num_Buff, eax     ;
                mov     eax, fs:[edi + 4]               ;
                mov     DWORD PTR Tmp_Num_Buff[4], eax  ;
                mov     ax, fs:[edi + 8]                ;
                mov     WORD PTR Tmp_Num_Buff[8], ax    ;
                ; ---                                   ;
                cmp     BYTE PTR fs:[edi], 11           ;
                jb      @@not_found                     ;
                ; --- не начинается ли №б с МН префикса
                mov     dx, PREF_B_MN_MASK              ;
                mov     ax, fs:[edi + 1]                ;
                and     ax, dx                          ;
                cmp     ax, PREF_B_MN                   ;
                je      @@found                         ;
                ; --- не начинается ли №б с МГ префикса
                mov     dx, PREF_B_MG_MASK              ;
                mov     ax, fs:[edi + 1]                ;
                and     ax, dx                          ;
                cmp     ax, PREF_B_MG                   ;
                jne     @@not_found                     ;
                ; --- собственно обрезание того или иного префикса
@@found:        bsr     cx, dx                          ; \ количество
                inc     cx                              ; / сдвигов
                mov     al, cl                          ; \
                shr     al, 2                           ;  > корректировка длины №б
                sub     fs:[edi], al                    ; /
@@shift_loop:   stc                                     ; вдвигать справа будем 1
                rcr     DWORD PTR fs:[edi + 6], 1       ;
                rcr     DWORD PTR fs:[edi + 2], 1       ;
                rcr     BYTE PTR fs:[edi + 1], 1        ;
                loop    @@shift_loop                    ;
                stc                                     ;
                jmp     @@exit                          ;
                ; ---
@@not_found:    clc                                     ;
@@exit:         pop     edi
		pop	dx
		pop	cx
		pop	eax
                ENDM

; --- восстановление любого № из временного буфера
; вход:         константа len_x из SOED_Struc
;               fs:[edi]        - уазатель на соединение
; выход:        нет
;
@Nx_Restore     MACRO   what_len
		push	eax
                push    edi
                add     edi, what_len
                mov     eax, DWORD PTR Tmp_Num_Buff
                mov     fs:[edi], eax
                mov     eax, DWORD PTR Tmp_Num_Buff[4]
                mov     fs:[edi + 4], eax
                mov     ax, WORD PTR Tmp_Num_Buff[8]
                mov     fs:[edi + 8], ax
                pop     edi
		pop	eax
                ENDM

; --- создание сессии ---------------------------------------------------------
; вход:         fs:[edi]        - указатель на соединение
;               eax             - кто в соединении ссылаетя на сессию
;               gs:[bx]         - указатель на объект
;               dl              - 0, 2 или 4 - число сдвигов для get_num
; выход:        es:[si]         - указатель на созданную сессию
;
@Session_Create MACRO
                LOCAL   @@quit, @@find, @@found, @@Nb, @@Nc, @@add_9, @@tmp
                LOCAL   @@get_attr_b
                ; ---
                mov     es, ss:SEG_SORM         ;
                xor     si, si                  ;
                mov     cx, _SESSION_MAX        ;
@@find:         test    es:[si].sorm_flags, ssf_REL
                jnz     @@found                 ;
                add     si, _SORM_SIZE          ;
                loop    @@find                  ;
                clc                             ;
                jmp     @@quit                  ;
@@found:        ; --- флаги
                mov     es:[si].sorm_flags, sSf_NUM_FULL
                ; --- ссылки
                mov     es:[si].sorm_soed, edi  ; на соединение
                add     es:[si].sorm_soed, eax  ; на конкретный указатель в соединении
                mov     es:[si].sorm_obj, bx    ; на объект
                ; --- время
                mov     eax, ss:Curr_T          ;
                mov     es:[si].sorm_time, eax  ;
                ; --- №а
                mov     eax, DWORD PTR fs:[edi].attr_a
                mov     DWORD PTR es:[si].sorm_attr_a, eax
                mov     eax, DWORD PTR fs:[edi + 4].attr_a
                mov     DWORD PTR es:[si + 4].sorm_attr_a, eax
                mov     ax, WORD PTR fs:[edi + 8].attr_a
                mov     WORD PTR es:[si + 8].sorm_attr_a, ax
                mov     al, fs:[edi + 10].attr_a
                mov     es:[si + 10].sorm_attr_a, al
                @Nx_Session  sorm_attr_a
                ; --- №б
if 0
                test    fs:[edi].soed_flags, sf_UPATS_B
                jz      @@Nb
                push    dx
                mov     dl, 1
                call    Ux_Session
                pop     dx
                jmp     @@Nc
endif
@@Nb:           mov     eax, DWORD PTR fs:[edi].attr_b
                mov     DWORD PTR es:[si].sorm_attr_b, eax
                mov     eax, DWORD PTR fs:[edi + 4].attr_b
                mov     DWORD PTR es:[si + 4].sorm_attr_b, eax
                mov     ax, WORD PTR fs:[edi + 8].attr_b
                mov     WORD PTR es:[si + 8].sorm_attr_b, ax
                mov     al, fs:[edi + 10].attr_b
                mov     es:[si + 10].sorm_attr_b, al
                @Nx_Session  sorm_attr_b
          ; --- <Paul.N> ---- (29/04/13) ---
                test    fs:[edi].soed_flags, sf_UPATS_A
                jz      @@Nc
                cmp     BYTE PTR fs:[edi].tcop_dig, 0FFh
                je      @@Nc
                push    ax bx
                mov     ax, si
                mov     bl, _SORM_SIZE
                div     bl
                movzx   bx, al
                mov     al, fs:[edi].tcop_dig                 ;\
                mov     UPATS_TCOP_dig[bx], al                ;-> запишем цифру выхода в ТСОП
                pop     bx ax
        if 0
                ; --- Добавим цифру выхода в ТСОП для - УПАТС ---
                pushf
                push    ax bx cx di
                mov     cx, 11
                cld
                mov     bl, BYTE PTR fs:[edi].tcop_dig
                lea     di, Tmp_UPATS_Buff
                push    di si
   @@tmp:       mov     al, BYTE PTR es:[si].sorm_attr_b
                ;lods    BYTE PTR es:[si].sorm_attr_b
                mov     ds:[di], al
                inc     si
                inc     di
                loop    @@tmp
                pop     si di
                mov     cx, 4
   @@add_9:     stc
                rcl     DWORD PTR ds:[di + 2],  1
                rcl     DWORD PTR ds:[di + 6],  1
                rcl     BYTE  PTR ds:[di + 10], 1
                loop    @@add_9
                mov     al, BYTE PTR ds:[di + 2]
                and     al, 0F0h
                or      al, bl                          ; добавим цифру
                mov     ds:[di + 2], al
                inc     BYTE PTR ds:[di + 1]            ; length +1
                ; --- перенесем полученные данные обратно в сессию
                mov     cx, 11
                push    si
  @@get_attr_b: mov     al, BYTE PTR ds:[di]
                mov     es:[si].sorm_attr_b, al
                inc     si
                inc     di
                loop    @@get_attr_b
                pop     si
                ; --- отметим выполнение данной операции
                or      es:[si].sorm_flags, ssf_TCOP_UPATS
                pop     di cx bx ax
                popf
          ; --- <Paul> ---- (END) ---
        endif
                ; --- отметим выполнение данной операции
                or      es:[si].sorm_flags, ssf_TCOP_UPATS
                ; --- <Paul> ---- (END) ---
                ; --- №с
@@Nc:           mov     eax, DWORD PTR fs:[edi].attr_c
                mov     DWORD PTR es:[si].sorm_attr_c, eax
                mov     eax, DWORD PTR fs:[edi + 4].attr_c
                mov     DWORD PTR es:[si + 4].sorm_attr_c, eax
                mov     ax, WORD PTR fs:[edi + 8].attr_c
                mov     WORD PTR es:[si + 8].sorm_attr_c, ax
                mov     al, fs:[edi + 10].attr_c
                mov     es:[si + 10].sorm_attr_c, al
                @Nx_Session  sorm_attr_c
                ; --- абс. рабочий номер
                mov     al, fs:[edi].ck_a       ;
                mov     es:[si].sorm_ck_a, al   ;
                mov     ax, fs:[edi].work_a     ;
                mov     es:[si].sorm_work_a, ax ;
                mov     ax, fs:[edi].phys_a     ;
                mov     es:[si].sorm_phys_a, ax ;
                mov     al, fs:[edi].ck_b       ;
                mov     es:[si].sorm_ck_b, al   ;
                mov     ax, fs:[edi].work_b     ;
                mov     es:[si].sorm_work_b, ax ;
                ; --- номер вызова
                inc     SORM_Curr_Call          ;
                mov     ax, SORM_Curr_Call      ;
                mov     es:[si].sorm_call, ax   ;
                ; --- признак отбора номера
                mov     al, gs:[bx].obj_get_num ;
                mov     cl, dl                  ;
                shl     al, cl                  ;
                mov     es:[si].sorm_get_num, al;
                ; --- прочие параметры
                mov     al, gs:[bx].obj_prior   ;
                mov     es:[si].sorm_prior, al  ;
                mov     es:[si].sorm_opcode, 0  ;
                mov     es:[si].sorm_dvo_phase,0;
                mov     es:[si].sorm_dvo_code, 0;
		mov	WORD PTR es:[si].sorm_ksl_a, 0ffffh
                ; ---
                stc
@@quit:
                ENDM


; --- проверка полного номера как короткого
; вход:         константа attr_x из SOED_Struc
;               fs:[edi]        - уазатель на соединение
;               LENGTH_NUM      - длина номера в станции
; выход:        флаг переноса   - CF  - номер укорочен и сохранён, CF  - номер нельзя укоротить
;
@Is_Full_as_Short_on_Ctrl MACRO   what_num
                LOCAL   @@exit, @@find, @@found, @@next, @@nothing, @@start
                push    ecx
                push    edi
		add	edi, what_num		;
		; ---
                movzx   ebx, BYTE PTR fs:[edi - 1] ;
                cmp     bl, LENGTH_NUM          ;
                jbe     @@nothing               ;
                sub     bl, LENGTH_NUM          ;
                shr     ebx, 1                  ;
                mov     edx, fs:[edi + ebx]     ;
                jnc     @@start                 ;
                shr     edx, 4                  ;
                or      edx, 0f0000000h         ;
                ; ---
@@start:        lea     bx, Objects             ;
                mov     cx, Obj_Count           ;
                jcxz    @@nothing               ;
@@find:         cmp     gs:[bx].obj_type, 0ffh  ;
                je      @@next                  ;
                cmp     edx, DWORD PTR gs:[bx].obj_tel_num
                je      @@found                 ;
@@next:         add     bx, _OBJ_SIZE           ;
                loop    @@find                  ;
@@nothing:      clc                             ;
                jmp     @@exit                  ;
                ; ---
@@found:        stc
@@exit:         pop	edi
                pop     ecx
                ENDM
; --- Макрос формирования сообщений для "Горячего" телефона
; --- In: fs:[edi] - ук-ль на соединение
;         es:[si]  - ук-ль на сессию
@Try_Hot_tel    MACRO
                LOCAL   @@exit_44
                push    ax dx
                mov     ax, WORD PTR fs:[edi].dvo_code          ;
                cmp     al, HT                                  ;
                jne     @@exit_44                               ;
                cmp     ah, PHASE_USE_DVO                       ;
                jne     @@exit_44                               ;
                mov     WORD PTR es:[si].sorm_dvo_code, ax      ;
                mov     dl, SO_DVO                              ;
                call    Message_SORM                            ;
    @@exit_44:  pop     dx ax
                ENDM

; --- Макрос записи ссылок на сессии по номеру С при перадресации
; --
@Save_Sess_ptr   MACRO   sess_Max, buff_Sess
                LOCAL   @@check, @@found, @@fail
                clc
                push    bx cx
                mov     cx, sess_Max
                lea     bx, buff_Sess
       @@check: cmp     WORD PTR [bx], 0FFFFh
                je      @@found
                add     bx, 2
                loop    @@check
                jmp     @@fail
     @@found:   mov     WORD PTR [bx], si                ; запомним ссылку на сессию
                stc
     @@fail:    pop     cx bx
                ENDM
; --- Макрос создания сессий по номеру С при перадресации
; -- вызывается в процедуре Session_RA_Create и
; -- записывает в спец. буфер ссылки на созданные сессии
; -- In :  fs:[edi] - soed ptr
;          gs:[bx] - object ptr
;          dl      - признак отбора
;          eax     - кто в соединении ссылаетя на сессию
; -- Out:  CF - OK, CF - NOK
@Ra_Nc_sess_create      MACRO  what_ptr_u
                LOCAL   @@maybe_Upats, @@OK_out, @@go_1, @@go_2, @@go_go, @@exit
                LOCAL   @@check, @@found, @@fail, @@maybe_ALL
                ; -- {Paul.N - add - 19/07/17}
                test    fs:[edi].soed_flags, sf_ANI             ; был ли АОН?
                jnz     @@maybe_ALL;;; @@maybe_Upats            ; с АОН м.б. всё)
                or      fs:[edi].soed_flags, sf_2VSK_Chelnok    ;<- отметим "челнок", т.к здесь СОРМ работает без АОНа
                jmp     @@maybe_Upats
                ; ---
@@maybe_ALL:    mov     PEREM_SOED_PTR, eax                     ; сохраним на время ук-ль
                @Session_Create                                 ;
                jnc     @@exit                                  ; выходим не раздумывая
                ; -- сразу занесем ссылку на сесс. в буфер
                @Save_Sess_ptr _Sess_RA_MAX, Buff_Sess_RA
                jnc     @@exit
                ; -- проверим №С УПАТС
                test    fs:[edi].soed_flags, sf_RA_UPATS_C
                jz      @@go_1
                ; -- заносим №УПАТС в №С
                mov     eax, DWORD PTR UPATS_Buff
                mov     DWORD PTR es:[si].sorm_attr_c, eax
                mov     eax, DWORD PTR UPATS_Buff[4]
                mov     DWORD PTR es:[si + 4].sorm_attr_c, eax
                mov     ax, WORD PTR UPATS_Buff[8]
                mov     WORD PTR es:[si + 8].sorm_attr_c, ax
                mov     al, BYTE PTR UPATS_Buff[10]
                mov     es:[si + 10].sorm_attr_c, al
                ; ---          Решение проблемы с подстановкой значений в макросы(отбор по №С), проверка последних доработок в коде, правка кода, проверка последней версии ПО на комплексе.
     @@go_1:    or      es:[si].sorm_flags, ssf_SWAP_BC         ;
                ; --- связываем соединение и сессию
                push    ebp
                mov     ebp, PEREM_SOED_PTR                     ; вспомним ук-ль в соед.
                mov     fs:[edi+ebp], si                        ; ук-ль на сессию
                pop     ebp
        IF 0    ; --- {Paul.N} -- [06/06/13]
                test    fs:[edi].soed_flags, sf_NA_sess
                jnz     @@exit                                  ; все остальное уже сформировали
        ENDIF
                ; -- {Paul.N - add - 19/07/17}
                test    fs:[edi].soed_flags, sf_2VSK_Chelnok    ;<- если был "челнок", то по УПАТС снова не формируем
                jnz     @@exit
                ; --- сессиия с №А - УПАТС(возможна без АОН)
 @@maybe_Upats: test    fs:[edi].soed_flags, sf_UPATS_A         ; здесь необходимо пров-ть от некоррект. МО112
                jz      @@exit                                  ; BAD MO112
                ; --- UPATS - OK!
    @@go_go:    mov     eax, what_ptr_u                         ; конкретный указатель в соединении
                @Session_Create
                jnc     @@exit                                  ; также выход
                ; --- и здесь ->
                ; -- сразу занесем ссылку на сесс. в буфер
                @Save_Sess_ptr _Sess_RA_MAX, Buff_Sess_RA
                jnc     @@exit
                ; --- заносим вызывающего из УПАТС в сессию как № А
                push    dx
                xor     dl, dl                                  ;
                call    Ux_Session                              ;
                pop     dx
                ; -- проверим №С УПАТС
                test    fs:[edi].soed_flags, sf_RA_UPATS_C
                jz      @@go_2
                ; -- заносим №УПАТС в №С
                mov     eax, DWORD PTR UPATS_Buff
                mov     DWORD PTR es:[si].sorm_attr_c, eax
                mov     eax, DWORD PTR UPATS_Buff[4]
                mov     DWORD PTR es:[si + 4].sorm_attr_c, eax
                mov     ax, WORD PTR UPATS_Buff[8]
                mov     WORD PTR es:[si + 8].sorm_attr_c, ax
                mov     al, BYTE PTR UPATS_Buff[10]
                mov     es:[si + 10].sorm_attr_c, al
                ; ---
    @@go_2:     or      es:[si].sorm_flags, ssf_SWAP_BC
                ; --- связываем соединение и сессию
                mov     fs:[edi].what_ptr_u, si                 ; ук-ль на сессию
        @@exit:
          ; -- [THE END]
                ENDM
;;Макрос обновления страницы окна выбора команд МО
;; input: reg_num(ebx) - порядковый номер первой команды окна
;;      ; num_str  - количество строк
;;      : MO_max - последняя цифра в окне
;;      ; ah - атрибут
;; output:

@Refresh_pge_MO MACRO  reg_num, num_str, MO_max
        LOCAL   @@loop_digs, @@go_stars
        PUSH    reg_num  cx
                mov     cx, num_str
                ; ---- обновим окошко ---
                push    si di
                mov     di, _pos_Tel_Num_Win + 320    ; начнем с самого верха
                mov     si,  offset Select_Mo_Win + 124
                call    printWin
                pop     di si
                ; ---           ---
                ; --- нарисуем цифры ---
                push    di
@@loop_digs:    call    printDigs
                inc     reg_num
                add     di, line_digs_draw
                loop    @@loop_digs
                cmp     reg_num, MO_max
                ja      @@go_stars
                sub     di, num_str * 160
                add     di, 12
                mov     cx, num_str
                jmp     @@loop_digs
                ; ---        ---
@@go_stars:     pop     di
                call    Stars_MO_Win                         ; нарисуем stars, если они есть
        POP     cx reg_num
ENDM
 ; ---- Дополнительный макрос ---
@Refresh_MO_PAGE        MACRO
                LOCAL @@refr_pge
        PUSH    ax ebx edx di
                ; ---- обновим страницу ----
                mov     di, frst_dig_MO
                mov     ebx, 1
                mov     edx, 150
                cmp     num_page_MO, 1
                je      @@refr_pge
                mov     ebx, 151
                mov     edx, 255
                ; --- Refresh page ---
@@refr_pge:     mov     ah, _color_normal_MO              ; установим атрибут
                @Refresh_pge_MO ebx, 15, edx
        POP     di edx ebx ax
ENDM
; --- Макрос очистки буферов команд МО
; --- In:     es = ds - сегмент буферов
;     Out:    пустые буфера с обнуленным указателем на свободное место в буфере
;     Портит: di, eax, cx
@Clear_Buffs    MACRO
                ; =--- очистим все буфера ---=
                lea     di, MO_Numb
                xor     eax, eax
                mov     cx, 64
                CLD
                rep     STOSD
                mov     curr_MO_ptr, 0                 ; обнулим ук-ль
                lea     di, MO_Numb_235
                xor     eax, eax
                mov     cx, 64
                CLD
                rep     STOSD
                mov     curr_MO_ptr_235, 0             ; обнулим ук-ль
ENDM
; ---- Макрос для записи в буфера выделенных команд, команд определеного типа
; --- сначала очищает буфера выделенных команд, затем записывает в них.
; --- In:     = In @@Clear_Buffs
;     Out:    буфера, заполненные набором соответствующего типа команд, указатели на свободные места в буферах
;     Портит: si, di, eax, cx

@Control_Type_MO       MACRO     type_buff, type_buff_235, len_buf
                LOCAL   @@usual_type, @@next_type, @@_type_235, @@refresh
                @Clear_Buffs
                ; =--- обновим рабочие буфера команд ---=
                lea     si, type_buff
                lea     di, MO_Numb
                CLD
                mov     cx, len_buf
                xor     ax, ax
@@usual_type:   lodsb
                cmp     al, 0
                je      @@next_type
                stosb
                loop    @@usual_type
@@next_type:    inc     di
                mov     curr_MO_ptr, di             ; сохраним тек. значение указателя
                lea     si, type_buff_235
                lea     di, MO_Numb_235
                CLD
                mov     cx, len_buf
                xor     ax, ax
 @@_type_235:   lodsb
                cmp     al, 0
                je      @@refresh
                stosb
                loop    @@_type_235
@@refresh:      inc     di
                mov     curr_MO_ptr_235, di         ; сохраним тек. значение указателя
ENDM
; ======= BlackList Debuger Macros's ===========
; ----    ------     ------     ------    ------
; --- BlackList Debuger ----
@_test_debug_black      MACRO  __number_bl_dd
                LOCAL   @@go_out, @@go_exit
                cmp     OneLn_active, 0             ; включен контроль одной линии?
                je      @@go_exit                   ; - нет
                cmp     _black_active, 255          ; Актив?
                jne     @@go_exit                   ; отладчик не активен!
                ; --- здесь работаем с выбранной линией ---
        push    ax
                mov     al, _numb_dds_bl            ;
                mov     _numb_dds_bl_prev, al
                mov     _numb_dds_bl, __number_bl_dd  ; установим № прохода в BlackList
                cmp     _numb_dds_bl_prev, 7
                jne     @@go_out
                mov     _differ_black, __number_bl_dd
                sub     _differ_black, 7
     @@go_out:
        pop     ax
@@go_exit:
ENDM
; --- BlackList_IN Debuger ----
@_test_debug_black_In   MACRO  __number_bl_dd_in
                LOCAL   @@go_out, @@go_exit
                cmp     OneLn_active, 0             ; включен контроль одной линии?
                je      @@go_exit                   ; - нет
                cmp     _black_active, 255          ; работаем по исходящему ч.с.?
                jne     @@go_exit                   ; да
                ; --- здесь работаем с выбранной линией ---
        push    ax
                mov     al, _numb_dds_bl_in            ;
                mov     _prev_dds_bl_in, al
                mov     _numb_dds_bl_in, __number_bl_dd_in  ; установим № прохода в BlackList
        pop     ax
@@go_exit:
ENDM
; --- BlackList Quit Debuger ----
@_test_debug_black_q    MACRO  __number_bl_dd_q
                LOCAL   @@go_out, @@go_exit, @@go_next
                cmp     OneLn_active, 0             ; включен контроль одной линии?
                je      @@go_exit                   ; - нет
                cmp     _black_active, 255          ; Актив?
                jne     @@go_exit                   ; отладчик не активен!
                ; --- здесь работаем с выбранной линией ---
        push    ax
                mov     al, _numb_dds_bl_q          ;
                mov     _numb_dds_bl_prev_q, al
                mov     _numb_dds_bl_q, __number_bl_dd_q  ; установим № прохода в BlackList_quit
                ; -- определимся с префиксом
                cmp     _numb_dds_bl_prev_q, 2
                jne     @@go_next
                mov     _differ_black_q, __number_bl_dd_q
                sub     _differ_black_q, 2
                jmp     @@go_out
                ; -- определимся с правилом
     @@go_next: cmp     _numb_dds_bl_prev_q, 6
                jne     @@go_out
                mov     _black_differ_q, __number_bl_dd_q
                sub     _black_differ_q, 6
  @@go_out:
        pop     ax
@@go_exit:
ENDM
; -----------------------------
; --- BlackList_IN Quit Debuger ----
@_test_debug_black_in_q MACRO  __number_bl_dd_in_q
                LOCAL   @@go_out, @@go_exit, @@go_next
                cmp     OneLn_active, 0             ; включен контроль одной линии?
                je      @@go_exit                   ; - нет
                cmp     _black_active, 255          ; Актив?
                jne     @@go_exit                   ; отладчик не активен!
                ; --- здесь работаем с выбранной линией ---
        push    ax
                mov     al, _numb_dds_bl_in_q
                mov     _prev_dds_bl_in_q, al
                mov     _numb_dds_bl_in_q, __number_bl_dd_in_q
        pop     ax
    @@go_exit:
ENDM

; --- Check BlackList Debug ---
@_check_debug_black     MACRO
                LOCAL   @@go_out_check
        push    ax
                cmp     OneLn_active, 0
                je      @@go_out_check
                ; -- проверим полный физ. номер линии
                mov     al, fs:[edi].ck_a
                cmp     al, OneLn_Ck
                jne     @@go_out_check
                mov     ax, fs:[edi].phys_a
                cmp     ax, OneLn_PhysNum
                jne     @@go_out_check
                ; --- обнулим все
                @_null_refr_black
                mov     _black_active, 255      ; запустим работу отладчика
@@go_out_check:
        pop     ax
ENDM
; --- Check BlackList_IN Debug ---
@_check_debug_black_in  MACRO
                LOCAL   @@go_out_check_in
        push    ax
                cmp     OneLn_active, 0
                je      @@go_out_check_in
                ; -- проверим полный физ. номер линии
                mov     al, fs:[edi].ck_b
                cmp     al, OneLn_Ck
                jne     @@go_out_check_in
                mov     ax, fs:[edi].phys_b
                cmp     ax, OneLn_PhysNum
                jne     @@go_out_check_in
                ; --- обнулим все
                @_null_refr_black
                mov     _black_active, 255      ; запустим работу отладчика
@@go_out_check_in:
        pop     ax
ENDM
; ----------------------------------------
; --- Макрос подготовки для выделения необходимой строки в Blacklist debug
; -- In: color_debug - цвет строки
;    --> displ       - смещение(для квит. ч.с.)
;    --> al          - № строки
; =============------------===============
@PrintPre_Black_Debug   MACRO color_debug, displ
                mov     cx, 19
                mov     ah, 160
                mul     ah
                mov     di, frst_dig_MO + displ
                add     di, ax
                add     di, 2
                mov     ah, color_debug
ENDM
; ----------------------------------------
; --- обнуление рабочих переменных для BlackList Debuger ---
@_null_refr_black       MACRO
                mov     _black_active, 0         ; не активно!
                mov     _numb_dds_bl_prev, 0     ; обнулим тек. значения счетчика prev. исх ч.с.
                mov     _prev_dds_bl_in, 0       ; обнулим тек. значения счетчика prev. вх ч.с.
                mov     _numb_dds_bl, 0          ; обнулим тек. значения счетчика исх ч.с.
                mov     _numb_dds_bl_in, 0       ; обнулим тек. значения счетчика вх ч.с.
                mov     _differ_black, 0         ; обнулим перем. для индикации исх. ч.с.
                ; --- квитированный ч.с.(quit)
                mov     _numb_dds_bl_prev_q, 0   ; \
                mov     _prev_dds_bl_in_q, 0     ;  | аналогично!
                mov     _numb_dds_bl_q, 0        ;  |
                mov     _numb_dds_bl_in_q , 0    ; /
                mov     _differ_black_q, 0       ; обнулим перем. корректности длины преф. №А
                mov     _black_differ_q, 0       ; обнулим перем. для определения правила исх. ч.с.
ENDM
; -------------------------------------------------
; --- Инициализация окон счетчиков ошибок ПУМы ----
@_Puma_Debug_Init       MACRO     Name_win, Type_cnts, Type_tmr, offs, menu
                        LOCAL     @@_name, @@_cnts, @@tmr_val, @@null_tmr, @@next_cnt
        push    ax ebx ebp cx si di
                ; --- нарисуем главное окно
                mov     si, offset Puma_Counts
                mov     ah, 30h
                mov     di, _pos_Win_Cfg + offs
                mov     cx, 14
                call    printWin
                ; --- нарисуем название окна
                mov     ah, _color_puma_n
                mov     si, offset Name_win;;Puma_Name_debug
                mov     di, 12 + offs
                call    printS
                ; --- нарисуем меню
                mov     ah, 30h
                mov     si, offset menu
                mov     di, 14*160 + offs
                mov     cx, 2
                call    printWin
                ; --- нарисуем названия ошибок
                mov     ah, _color_puma
                mov     si, offset Puma_counts_name
                mov     di, _pos_Win_Cfg + 3*160 + 4 + offs
                mov     cx, 10                           ; количество строк
  @@_name:      call    printS
                add     di, 160
                add     si, 5
                loop    @@_name
                ; --- выведем начальные значения счетчиков
                ; =======--------========--------=========
                xor     ebp, ebp
                mov     ah, _color_puma_v
                movzx   ebx, BYTE PTR Type_cnts[ebp];;Puma_cnts_tar[ebp]
                ; -- KC
                mov     di, _pos_Win_Cfg + 3*160 + 22 + 4 + offs
                mov     cl, 2
                call    printH
                sub     di, 4
                mov     cx, 9
                ; -- others
     @@_cnts:   push    cx
                add     di, 160
                mov     cl, 4
                movzx   ebx, WORD PTR Type_cnts[ebp + 1];;Puma_cnts_tar[ebp + 1]
                call    printH
                pop     cx
                add     ebp, 2
                loop    @@_cnts
                ; --- выведем позиции для значений таймера счетчиков
                ; =======--------========--------========--------===
                xor     ebp, ebp
                mov     si, offset Puma_counts_val
                mov     di, _pos_Win_Cfg + 3*160 + 38 + offs
                mov     cx, 10                           ; количество строк
     @@tmr_val: ; -- проверим на наличие timer для данного счетчика
                cmp     DWORD PTR Type_tmr[ebp], 0;;Puma_tmr_tar[ebp], 0
                je      @@null_tmr
                mov     ebx, Type_tmr[ebp];;Puma_tmr_tar[ebp]
                push    cx
                mov     cl, 8
                call    printH
                pop     cx
                jmp     @@next_cnt
   @@null_tmr:  call    printS
   @@next_cnt:  add     di, 160
                add     ebp, 4
                loop    @@tmr_val
                ; ==================---------------==================
        pop     di si cx ebp ebx ax
ENDM
; ----------------------------------------------------
; --- Выдача значения системного времени в ebx -------
@_timer_ebx     MACRO
                LOCAL   @out_timer
                pushf
                push    dx ax
                push    fs
                mov     fs, SS:SEG_AX
                clc
                xor     dx, dx
                xor     ebx, ebx        ;  пока bad timer
                ; -- день
                mov     al, fs:DEN_MES
                add     al, dl
                daa
                jc      @out_timer
                mov     bh, al
                ; -- час
                mov     al, fs:CHAS
                add     al, dl
                daa
                jc      @out_timer
                mov     bl, al
                shl     ebx, 16
                ; -- минута
                mov     al, fs:MIN
                add     al, dl
                daa
                jc      @out_timer
                mov     bh, al
                ; -- секунда
                mov     al, fs:SECUNDA
                add     al, dl
                daa
                jc      @out_timer
                mov     bl, al
                ; ------------
@out_timer:     pop     fs
                pop     ax dx
                popf
ENDM
; --------------------------------------
; ---- Сброс ошибок ПУМы ---
; --// при нажатии клавиш "R" или "C" сбрасываются соответственно
; --// счетчики окна Puma_tar или Puma_cto
; ---> : dbgOfs   - puma_dbg_ofs
;        transOfs - PUMA_trans_ofs
;        winOfs   - puma_win_ofs
; -------================--------
@_Puma_cnts_Init    MACRO   dbgOfs, transOfs, winOfs
                LOCAL   @@nul_cnts, @@pr_cnts, @@cnt_val
                push    ax ebx edx edi ebp cx esi
                push    fs
                mov     fs, SS:SEG_PUMA
                mov     ebx, 0
                xor     ebp, ebp
                xor     edx, edx
                add     edx, dbgOfs
                mov     esi, edx
                mov     edi, _PUMA_TRAN_OFFS
                add     edi, transOfs
                ; -- KC
                mov     fs:[edi].ecsc, bl
                mov     Puma_cnts_tar[esi], bl
                ; -- timer KC
                mov     Puma_tmr_tar[esi], ebx
                ; -- остальные счетчики
                mov     cx, 9
   @@nul_cnts:  mov     fs:[ebp + edi].ulbc, bx
                mov     WORD PTR Puma_cnts_tar[ebp + edx + 1], bx
                ; -- timer others
                mov     Puma_tmr_tar[esi + 4], ebx
                add     ebp, 2
                add     esi, 4
                loop    @@nul_cnts
                ; --- сброс данных на экране в начальные значения
                ; --- выведем начальные значения счетчиков
                mov     ah, _color_puma_v
                        ; -- KC
                mov     di, _pos_Win_Cfg + 3*160 + 22 + 4
                add     di, winOfs
                mov     cl, 2
                call    printH
                sub     di, 4
                mov     cx, 9
                        ; -- others
    @@pr_cnts:  push    cx
                add     di, 160
                mov     cl, 4
                call    printH
                pop     cx
                loop    @@pr_cnts
                ; --- выведем позиции для значений таймера счетчиков
                mov     si, offset Puma_counts_val
                mov     di, _pos_Win_Cfg + 3*160 + 38
                add     di, winOfs
                mov     cx, 10                           ; количество строк
    @@cnt_val:  call    printS
                add     di, 160
                loop    @@cnt_val
                pop     fs
                pop     esi cx ebp edi edx ebx ax
        ENDM

; === {Paul.N} --- <29/04/13> ===
; -- Удаление инфы в сесиии о цифре ТСОП(сама цифра в массиве и флаг в сессии)
; --- Input:  es:[si] - si -> pointer to SORM session
; --  Output: clear flag ssf_TCOP_UPATS and element of massive UPATS_TCOP_dig
@_Clear_tcop_dig MACRO
                LOCAL   @@go_end
		cmp	si,_SESSION_MAX * _SORM_SIZE
		ja	@@go_end
                test    es:[si].sorm_flags, ssf_TCOP_UPATS
                jz      @@go_end                                ; убирать нечего
                and     es:[si].sorm_flags, not ssf_TCOP_UPATS
                push    ax bx dx  ; MVT переделал деление на деление словом bx, чтобы результат не вылезал за пределы al, а помещался бы в ax
		xor	dx, dx
                mov     ax, si
                mov     bx, _SORM_SIZE
                div     bx
                mov	bx, ax
                mov     UPATS_TCOP_dig[bx], 0FFh                ;-> освободим элемент в массиве
                pop     dx bx ax
   @@go_end:
                ENDM
