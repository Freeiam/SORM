; ╔════════════════════════════════════════════════════════════════════════════╗
; ║  Файл:         Config.ASM                                                  ║
; ║  Программист:  Тоневицкий Ю.Д.                                             ║
; ║----------------------------------------------------------------------------║
; ║  Использует:   Config.DAT                                                  ║
; ║----------------------------------------------------------------------------║
; ║  Назначение:   Содержит процедуры необходимые для сбора конфигурации АТС   ║
; ║----------------------------------------------------------------------------║
; ║  Содержит:                                                                 ║
; ║                                                                            ║
; ║                                                                            ║
; ║                                                                            ║
; ╚════════════════════════════════════════════════════════════════════════════╝


; --- основная процедура - зовётся в главном цикле для сбора конфигурации АТС
;     вход:     рабочие переменные
;     выход:    рабочие переменные
;
GetNewConfig    PROC
                test    FG_PROGRAM, _fg_prg_ck0 ; работаем с ЦК0 ?
                jnz     @@exit                  ; ДА - сами конфигу не собираем
                ; ---
                test    FG_PROGRAM, _fg_prg_multy_pref ; старая конфигурация ?
                jz      @@exit                  ; ДА -
                ; ---
                cmp     Cfg_Run, 0              ; сбор конфигурации закончен ?
                je      @@exit                  ; ДА -
		cmp	MO_IDX, 0
		jne	@@exit
                call    IsConfigOK              ; НЕТ - но может его пора закончить ?
                jc      @@exit                  ; ПОРА -
                ; --- проверим возможность опроса ЦК
                mov     gs, ss:SEG_KONF
                movzx   si, Cfg_Curr_CK         ; очередной ЦК
                mov     es, ss:SEG_AXD
                cmp     es:TIS_TIS[si], KL_TIS  ; ЦК с № Cfg_Curr_CK есть в сети ?
                jae     @@next                  ; НЕТ - вычёркиваем
                test    Cfg_CK_Flags[si], cf_DONE; вся конфигурация этого ЦК уже получена ?
                jnz     @@next                  ; ДА - к следующему
                cmp     Cfg_Check_Level, 0      ; проверять уровень ЦК (первый проход) ?
                je      @@no_level_check        ; НЕТ -
                mov     al, Cfg_CK_Level[si]    ; уровень этого ЦК (расстояние от тарификатора)
                cmp     al, Cfg_Curr_Level      ; ЦК дальше от тарификатора чем нужно ?
                jne     @@next                  ; ДА - пропустим, т.к. работаем с другим уровнем
@@no_level_check:
                ; --- проверим этап опроса
                test    Cfg_CK_Flags[si], cf_SEND_CD ; запрос даты конфигурации отправлен ?
                jz      @@send_CD                    ; НЕТ - отправим
                test    Cfg_CK_Flags[si], cf_SEND_TN ; тарификационная сеть отправлена ?
                jz      @@send_TN                    ; НЕТ - отправим
                test    Cfg_CK_Flags[si], cf_RECV_TN ; тарификационную сеть получена ?
                jz      @@recv_TN                    ; НЕТ - ждём-с
                test    Cfg_CK_Flags[si], cf_SEND_CK ; запрос общей конфигурации ЦК отправлен ?
                jz      @@send_CK                    ; НЕТ - отправим
                test    Cfg_CK_Flags[si], cf_RECV_CK ; общая конфигурация ЦК получена ?
                jz      @@recv_CK                    ; НЕТ - ждём-с
                test    Cfg_CK_Flags[si], cf_SEND_MOD; запрос конфигурации блока отправлен ?
                jz      @@send_Mod                   ; НЕТ - отправим
                ; --- ожидание конфигурации блока ---------------------------------------
@@recv_Mod:
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
                sub     eax, Cfg_TOut
                cmp     eax, _CFG_TOUT          ; время ожидания вышло ?
                jb      @@recv_Mod_3            ; ещё НЕТ -
                cmp     Cfg_Attempt_Count, _CFG_MAX_ATTEMPTS ; переспросили достаточное число раз ?
                jb      @@recv_Mod_2                         ; НЕТ - сделаем ещё одну попытку отправить сеть
                or      Cfg_CK_Errs[si], ce_BLK_FAULT        ; отметим что нет конфигурации с блока
                ;;;and     Cfg_CK_Flags[si], not cf_SEND_CD     ;
                mov     Cfg_CK_Flags[si], 0     ; -{Paul} - <15/12/12>-
                movzx   ax, Cfg_Curr_Mod        ;
                shl     si, 1                   ;
                bts     Cfg_CK_Mod_Err[si], ax  ;
                shr     si, 1                   ;
                jmp     @@next                  ; и перейдём к следующему ЦК
		; ---
@@recv_Mod_2:   cmp     Cfg_Curr_Line, 0        ; хоть одну линию приняли ?
                je      @@send_Mod_2            ; НЕТ - запросим ещё раз конфигурацию блока
                @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
                or      dl, dl                  ; есть возможность послать?
                jnz     @@exit                  ; НЕТ -
                mov     eax, 40404ebh           ; 235-я длиной 4
                stosd
                mov     al, 213                 ; d5h - тип 235-той
                stosb
                mov     al, Cfg_Curr_CK         ; номер ЦК
                stosb
                movzx   ax, Cfg_Curr_Mod
                shl     ax, 9
                or      ax, Cfg_Curr_Line       ; физ. номер линии на ЦК
                stosw
                @ZAPIS_OK_MO                    ; пошлем
IF _MSP NE TRUE
if _DebugKonfig
	@dword_to_file 0eb0213h,8
	call write_cr_lf_to_debug_file
endif
endif
                ; - copy cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 0fbh
                @_STOSB
                mov     al, Cfg_Curr_CK         ; номер ЦК
                @_STOSB
                movzx   ax, Cfg_Curr_Mod
                shl     ax, 9
                or      ax, Cfg_Curr_Line       ; физ. номер линии на ЦК
                @_STOSW
                call    PUMA_Send_Tar
                ; - copy cmd to PUMA
                inc     Cfg_Attempt_Count       ; счётчик попыток - очередная
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
                mov     Cfg_TOut, eax           ; / отправки
                jmp     @@exit
		; ---
@@recv_Mod_3:   mov     ax, Cfg_Curr_Line       ;
		mov	di, si                  ;
		shl	di, 5                   ; Nлинии * 32
                movzx   dx, Cfg_Curr_Mod        ;
		shl	dx, 1                   ; Nмодема * 2
		add	di, dx                  ;
		mov	Cfg_CK_Lines[di], ax    ; запомним N последней принятой
                cmp     ax, Cfg_Lines_Total     ; все линии получены ?
                jb      @@exit                  ; НЕТ -
		mov     Cfg_Show_Blk, -1        ;
		and     Cfg_CK_Flags[si], not cf_SEND_MOD ;
                shl     si, 1                   ;
                movzx   ax, Cfg_Curr_Mod        ;
                bts     Cfg_CK_Mod_OK[si], ax   ;
                inc     Cfg_Curr_Mod            ;
                cmp     Cfg_Curr_Mod, 16        ;
                jb      @@exit                  ;
                mov     Cfg_Curr_Mod, 0         ;
		cmp	Cfg_CK_Mod_Err[si], 0	;
		jne	@@next			;
		shr	si, 1			;
		or	Cfg_CK_Flags[si], cf_DONE;
                jmp     @@next                  ;
                ; --- отправка запроса конфигурации блока -------------------------------
@@send_Mod:
                movzx   ebx, Cfg_Curr_CK        ;
                shl     ebx, 4                  ;
                or      bl, Cfg_Curr_Mod        ;
                shl     ebx, 3                  ;
                cmp     BYTE PTR gs:[ebx], 4    ;
                je      @@send_Mod_0            ;
                cmp     BYTE PTR gs:[ebx], 0ffh ;
                jne     @@send_Mod_1            ;
@@send_Mod_0:   inc     Cfg_Curr_Mod            ;
                cmp     Cfg_Curr_Mod, 16        ;
                jb      @@send_Mod              ;
                mov     Cfg_Curr_Mod, 0         ;
		shl	si, 1			;
		cmp	Cfg_CK_Mod_Err[si], 0	;
		jne	@@next			;
		shr	si, 1			;
		or	Cfg_CK_Flags[si], cf_DONE;
                jmp     @@next                  ;
@@send_Mod_1:   ; ---
                movzx   si, Cfg_Curr_CK         ;
                mov     ax, gs:[ebx + 6]        ;
                mov     Cfg_Lines_Total, ax     ;
                mov     Cfg_Curr_Line, 0        ;
                ; ---
                mov     Cfg_Attempt_Count, 0    ; счётчик попыток - дальше получится первая
@@send_Mod_2:
                @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
                or      dl, dl                  ; есть возможность послать?
                jnz     @@exit                  ; НЕТ -
                or      Cfg_CK_Flags[si], cf_SEND_MOD ; отметим запрос конфигурации блока
                inc     Cfg_Attempt_Count       ; счётчик попыток - очередная
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
                mov     Cfg_TOut, eax           ; / отправки
                ; ---
                mov     eax, 30404ebh           ; 235-я длиной 3
                stosd
                mov     al, 212                 ; d4h - тип 235-той
                mov     ah, Cfg_Curr_CK         ; ЦК
                stosw
                mov     al, Cfg_Curr_Mod        ; модем
		mov     Cfg_Show_Blk, al
                stosb
                ; ---
                @ZAPIS_OK_MO                    ; пошлем
IF _MSP NE TRUE
if _DebugKonfig
	@dword_to_file 0eb0212h,8
	call write_cr_lf_to_debug_file
endif
endif
                ; - copy cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 0f9h
                @_STOSB
                mov     al, Cfg_Curr_CK         ; номер ЦК
                @_STOSB
                mov     al, Cfg_Curr_Mod        ; модем
                mov     Cfg_Show_Blk, al
                @_STOSB
                call    PUMA_Send_Tar
                ; - copy cmd to PUMA
        ; --- {Paul} --- <17/09/2012> ---
        ;// don't send time
        if 0
                cmp     MES, 0                  ;
                je      @@exit                  ;
                cmp     DEN_MES, 0              ;
                je      @@exit                  ;
                ;
                @POLUCH_ADR_MO
                or      dl, dl
                jnz     @@exit                  ; БУФЕР ЗАНЯТ
                mov     al, 135                 ; дата и время для ЦК
                stosb
                mov     al, Cfg_Curr_CK
                stosb
                mov     al, GOD_
                stosb
                mov     al, GOD
                stosb
                mov     ax, WORD PTR MES
                stosw
                mov     ax, WORD PTR MIN
                stosw
                @ZAPIS_OK_MO
IF _MSP NE TRUE
if _DebugKonfig
	@dword_to_file 0ffff0135h,8
	call write_cr_lf_to_debug_file
endif
endif
                ; ----
        endif
        ; --- {Paul} --- <end> ---
                jmp     @@exit
                ; --- ожидание общей конфигурации ЦК ------------------------------------
@@recv_CK:
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
                sub     eax, Cfg_TOut
                cmp     eax, _CFG_TOUT          ; время ожидания вышло ?
                jb      @@exit                  ; ещё НЕТ -
                cmp     Cfg_Attempt_Count, _CFG_MAX_ATTEMPTS ; переспросили достаточное число раз ?
                jb      @@send_CK_2             ; НЕТ - сделаем ещё одну попытку отправить сеть
                or      Cfg_CK_Errs[si], ce_CK_FAULT ; отметим что не получена общая конфигурация ЦК
		mov	Cfg_CK_Flags[si], 0     ; гасим флаги чтоб потом начать сначала
                jmp     @@next
                ; --- отправка запроса общей общей конфигурации ЦК ----------------------
@@send_CK:      mov     Cfg_Attempt_Count, 0    ; счётчик попыток - дальше получится первая
@@send_CK_2:
                @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
                or      dl, dl                  ; есть возможность послать?
                jnz     @@exit                  ; НЕТ -
                or      Cfg_CK_Flags[si], cf_SEND_CK ; отметим запрос общей конфигурации ЦК
                mov     Cfg_CK_Errs[si], 0      ;
                inc     Cfg_Attempt_Count       ; счётчик попыток - очередная
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
                mov     Cfg_TOut, eax           ; / отправки
		mov	Cfg_Curr_Line, 0	;
                ; ---
                mov     eax, 20404ebh           ; 235-я длиной 2
                stosd
                mov     al, 210                 ; тип 235-той
                mov     ah, Cfg_Curr_CK         ; ЦК
                stosw
                ; ---
                @ZAPIS_OK_MO                    ; пошлем
IF _MSP NE TRUE
if _DebugKonfig
	@dword_to_file 0eb0210h,8
	call write_cr_lf_to_debug_file
endif
endif
                ; - copy cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 0f7h
                @_STOSB
                mov     al, Cfg_Curr_CK         ; номер ЦК
                @_STOSB
                call    PUMA_Send_Tar
                ; - copy cmd to PUMA
                jmp     @@exit
                ; --- ожидание подтверждения получения тарификационной сети -------------
@@recv_TN:
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
                sub     eax, Cfg_TOut
                cmp     eax, _CFG_TOUT          ; время ожидания вышло ?
                jb      @@exit                  ; ещё НЕТ -
                cmp     Cfg_Attempt_Count, _CFG_MAX_ATTEMPTS ; переспросили достаточное число раз ?
                jb      @@send_TN_2             ; НЕТ - сделаем ещё одну попытку отправить сеть
                or      Cfg_CK_Errs[si], ce_TN_FAULT     ; отметим что нет подтверждения тарификационной сети
                mov     Cfg_CK_Flags[si], 0     ; гасим флаги чтоб потом начать сначала
                jmp     @@next                  ; и перейдём к следующему ЦК
                ; --- отправка тарификационной сети -------------------------------------
@@send_TN:      mov     Cfg_Attempt_Count, 0    ; счётчик попыток - дальше получится первая
@@send_TN_2:
                @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
                or      dl, dl                  ; есть возможность послать?
                jnz     @@exit                  ; НЕТ -
                or      Cfg_CK_Flags[si], cf_SEND_TN ; отметим отправку тарификационной сети
                mov     Cfg_CK_Errs[si], 0      ;
                inc     Cfg_Attempt_Count       ; счётчик попыток - очередная
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
                mov     Cfg_TOut, eax           ; / отправки
                ; ---
                mov     eax, 820404ebh          ; 235-я длиной 130
                stosd
                mov     al, 208                 ; тип 235-той
                mov     ah, Cfg_Curr_CK         ; ЦК
		mov	Cfg_Show_CK, ah
                stosw
                shl     si, 7
		add	si, OFFSET TIS_TIS
                mov     cx, 128
                push    si
                push    ds
                mov     ds, ss:SEG_AXD
                rep     movsb                   ; и его тар. сеть
                pop     ds
                pop     si
                ; ---
                @ZAPIS_OK_MO                    ; пошлем
IF _MSP NE TRUE
if _DebugKonfig
	@dword_to_file 0eb0208h,8
	call write_cr_lf_to_debug_file
endif
endif
                ; - copy cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 0f5h
                @_STOSB
                mov     al, Cfg_Curr_CK         ; номер ЦК
                @_STOSB
                mov     cx, 128
                push    ds
                mov     ds, ss:SEG_AXD
                and     esi, 0ffffh
                @_RMOVSB                        ; и его тар. сеть
                pop     ds
                call    PUMA_Send_Tar
                ; - copy cmd to PUMA
                jmp     @@exit
                ; --- отправка запроса даты конфигурации --------------------------------
@@send_CD:      @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
                or      dl, dl                  ; есть возможность послать?
                jnz     @@exit                  ; НЕТ -
                mov     Cfg_CK_Flags[si], cf_SEND_CD ; отметим отправку запроса даты конфигурации
                mov     Cfg_CK_Errs[si], 0      ;
                ; ---
                mov     al, 134                 ; 134-ая команда
                mov     ah, Cfg_Curr_CK         ; ЦК
		mov	Cfg_Show_CK, ah
                stosw
                ; ---
                @ZAPIS_OK_MO                    ; пошлем
IF _MSP NE TRUE
if _DebugKonfig
	@dword_to_file 0ffff0134h,8
	call write_cr_lf_to_debug_file
endif
endif
                ; - copy cmd to PUMA
;                call    PUMA_Start_Tar
;                mov     al, ???
;                @_STOSB
;                mov     al, Cfg_Curr_CK         ; номер ЦК
;                @_STOSB
;                call    PUMA_Send_Tar
                ; - copy cmd to PUMA
                jmp     @@exit
                ; --- переход к опросу следующего ЦК
@@next:         mov	Cfg_Show_CK, -1
                mov	Cfg_Show_Blk, -1
		cmp	Cfg_from_One_CK, -1
		je      @@continue
		mov	Cfg_from_One_CK, -1
		mov	Cfg_Run, 0
		ret
@@continue:	inc     Cfg_Curr_CK
                cmp     Cfg_Curr_CK, KL_TIS     ; max № ЦК == KL_TIS
                jb      @@exit
                mov     Cfg_Curr_CK, 1          ; ЦК0 (себя) не опрашиваем
                ; ---
@@exit:         ret
GetNewConfig    ENDP


; --- основная процедура - зовётся в главном цикле для сбора конфигурации АТС
;     вход:     рабочие переменные
;     выход:    рабочие переменные
;
GetOldConfig    PROC
                test    FG_PROGRAM, _fg_prg_ck0 ; работаем с ЦК0 ?
                jnz     @@exit                  ; ДА - сами конфигу не собираем
                ; ---
                test    FG_PROGRAM, _fg_prg_multy_pref ; старая конфигурация ?
                jnz     @@exit                  ; НЕТ -
                ; ---
                inc     Cfg_Wait
                cmp     Cfg_Wait, _CFG_WAIT_MAX
                jbe     @@exit
                mov     Cfg_Wait, 0
                ; ---
                cmp     Cfg_Run, 0              ; сбор конфигурации закончен ?
                je      @@exit                  ; ДА -
		cmp	MO_IDX, 0
		jne	@@exit
                call    IsConfigOK              ; НЕТ - но может его пора закончить ?
                jc      @@exit                  ; ПОРА -
                ; --- проверим возможность опроса ЦК
                mov     gs, ss:SEG_KONF
                movzx   si, Cfg_Curr_CK         ; очередной ЦК
                mov     es, ss:SEG_AXD
                cmp     es:TIS_TIS[si], KL_TIS  ; ЦК с № Cfg_Curr_CK есть в сети ?
                jae     @@next                  ; НЕТ - вычёркиваем
                test    Cfg_CK_Flags[si], cf_DONE; вся конфигурация этого ЦК уже получена ?
                jnz     @@next                  ; ДА - к следующему
                cmp     Cfg_Check_Level, 0      ; проверять уровень ЦК (первый проход) ?
                je      @@no_level_check        ; НЕТ -
                mov     al, Cfg_CK_Level[si]    ; уровень этого ЦК (расстояние от тарификатора)
                cmp     al, Cfg_Curr_Level      ; ЦК дальше от тарификатора чем нужно ?
                jne     @@next                  ; ДА - пропустим, т.к. работаем с другим уровнем
@@no_level_check:
                ; --- проверим этап опроса
                test    Cfg_CK_Flags[si], cf_SEND_CD ; запрос даты конфигурации отправлен ?
                jz      @@send_CD                    ; НЕТ - отправим
                test    Cfg_CK_Flags[si], cf_SEND_TN ; тарификационная сеть отправлена ?
                jz      @@send_TN                    ; НЕТ - отправим
                test    Cfg_CK_Flags[si], cf_SEND_CK ; запрос общей конфигурации ЦК отправлен ?
                jz      @@send_CK                    ; НЕТ - отправим
                test    Cfg_CK_Flags[si], cf_RECV_CK ; общая конфигурация ЦК получена ?
                jz      @@recv_CK                    ; НЕТ - ждём-с
                ; --- ожидание конфигурации всего ЦК ------------------------------------
@@recv_Cfg:
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
                sub     eax, Cfg_TOut
                cmp     eax, _CFG_TOUT          ; время ожидания вышло ?
                jb      @@exit                  ; ещё НЕТ -
                ; ---
                or      Cfg_CK_Errs[si], ce_BLK_FAULT    ; отметим что нет конфигурации с блока
                ;;;and     Cfg_CK_Flags[si], not cf_SEND_CD ;
                mov     Cfg_CK_Flags[si], 0     ; - <Paul> - {15/12/12} -
                movzx   ax, Cfg_Show_Blk        ;
                shl     si, 1                   ;
                bts     Cfg_CK_Mod_Err[si], ax  ;
                shr     si, 1                   ;
                jmp     @@exit
                ; --- ожидание общей конфигурации ЦК ------------------------------------
@@recv_CK:
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
                sub     eax, Cfg_TOut
                cmp     eax, _CFG_TOUT          ; время ожидания вышло ?
                jb      @@exit                  ; ещё НЕТ -
                cmp     Cfg_Attempt_Count, _CFG_MAX_ATTEMPTS ; переспросили достаточное число раз ?
                jb      @@send_CK_2             ; НЕТ - сделаем ещё одну попытку отправить сеть
                or      Cfg_CK_Errs[si], ce_CK_FAULT ; отметим что не получена общая конфигурация ЦК
		mov	Cfg_CK_Flags[si], 0     ; гасим флаги чтоб потом начать сначала
                jmp     @@next
                ; --- отправка запроса общей общей конфигурации ЦК ----------------------
@@send_CK:      mov     Cfg_Attempt_Count, 0    ; счётчик попыток - дальше получится первая
@@send_CK_2:
                @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
                or      dl, dl                  ; есть возможность послать?
                jnz     @@exit                  ; НЕТ -
                or      Cfg_CK_Flags[si], cf_SEND_CK ; отметим запрос общей конфигурации ЦК
                mov     Cfg_CK_Errs[si], 0      ;
                inc     Cfg_Attempt_Count       ; счётчик попыток - очередная
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
                mov     Cfg_TOut, eax           ; / отправки
		mov	Cfg_Curr_Line, 0	;
                ; ---
                mov     al, 124                 ; 124-ая команда
                mov     ah, Cfg_Curr_CK         ; ЦК
                stosw
                ; ---
                @ZAPIS_OK_MO                    ; пошлем
                ; ---
        ; --- {Paul} --- <17/09/2012> ---
        ;// don't send time
        if 0
                @POLUCH_ADR_MO
                or      dl, dl
                jnz     @@exit                  ; БУФЕР ЗАНЯТ
                mov     al, 135                 ; дата и время для ЦК
                stosb
                mov     al, Cfg_Curr_CK
                stosb
                mov     al, GOD_
                stosb
                mov     al, GOD
                stosb
                mov     ax, WORD PTR MES
                stosw
                mov     ax, WORD PTR MIN
                stosw
                @ZAPIS_OK_MO
                ; ----
        endif
        ; --- {Paul} --- <end> --
                ; - copy cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 14
                @_STOSB
                mov     al, Cfg_Curr_CK         ; номер ЦК
                @_STOSB
                call    PUMA_Send_Tar
                ; - copy cmd to PUMA
                jmp     @@exit
                ; --- отправка тарификационной сети -------------------------------------
@@send_TN:      @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
                or      dl, dl                  ; есть возможность послать?
                jnz     @@exit                  ; НЕТ -
                or      Cfg_CK_Flags[si], cf_SEND_TN ; отметим отправку тарификационной сети
                mov     Cfg_CK_Errs[si], 0      ;
                ; ---
                mov     al, 130                 ; 130-ая команда
                mov     ah, Cfg_Curr_CK         ; ЦК
		mov	Cfg_Show_CK, ah
                stosw
                shl     si, 7
		add	si, OFFSET TIS_TIS
                mov     cx, 128
                push    si
                push    ds
                mov     ds, ss:SEG_AXD
                rep     movsb                   ; и его тар. сеть
                pop     ds
                pop     si
                ; ---
                @ZAPIS_OK_MO                    ; пошлем
                ; - copy cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 0f5h
                @_STOSB
                mov     al, Cfg_Curr_CK         ; номер ЦК
                @_STOSB
                mov     cx, 128
                push    ds
                mov     ds, ss:SEG_AXD
                and     esi, 0ffffh
                @_RMOVSB                        ; и его тар. сеть
                pop     ds
                call    PUMA_Send_Tar
                ; - copy cmd to PUMA
                jmp     @@exit
                ; --- отправка запроса даты конфигурации --------------------------------
@@send_CD:      @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
                or      dl, dl                  ; есть возможность послать?
                jnz     @@exit                  ; НЕТ -
                mov     Cfg_CK_Flags[si], cf_SEND_CD ; отметим отправку запроса даты конфигурации
                mov     Cfg_CK_Errs[si], 0      ;
                ; ---
                mov     al, 134                 ; 134-ая команда
                mov     ah, Cfg_Curr_CK         ; ЦК
		mov	Cfg_Show_CK, ah
                stosw
                ; ---
                @ZAPIS_OK_MO                    ; пошлем
                ; - copy cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 0f1h                ; фактически начало сбора конфигурации с ЦК № ххх
                @_STOSB
                mov     al, Cfg_Curr_CK         ; номер ЦК
                @_STOSB
                call    PUMA_Send_Tar
                ; - copy cmd to PUMA
                jmp     @@exit
                ; --- переход к опросу следующего ЦК
@@next:         mov     Cfg_Show_CK,     -1
                mov     Cfg_Show_Blk,    -1
		cmp	Cfg_from_One_CK, -1
		je      @@continue
                mov     Cfg_from_One_CK, -1
		mov	Cfg_Run, 0
		ret
@@continue:	inc     Cfg_Curr_CK
                cmp     Cfg_Curr_CK, KL_TIS     ; max № ЦК == KL_TIS
                jb      @@exit
                mov     Cfg_Curr_CK, 1          ; ЦК0 (себя) не опрашиваем
                ; ---
@@exit:         ret
GetOldConfig    ENDP


IsConfigOK      PROC
		cmp	Cfg_from_One_CK, -1
		jne	@@exit
		; ---
		xor	al, al			; != 0 => есть ЦК на тек.уровне без ошибок и конфигурации
		xor	bl, bl                  ; != 0 => не вся конфигурация
                mov     si, 1                   ; ЦК0 пропускаем
		mov	es, ss:SEG_AXD          ;
@@CK_loop:	cmp	es:TIS_TIS[si], KL_TIS  ; есть ЦК с номером si
		jae	@@CK_next               ; НЕТ -
		test	Cfg_CK_Flags[si], cf_DONE;конфигурация этого ЦК получена ?
		jnz	@@CK_done               ; ДА -
		mov	bl, 255                 ; НЕТ - отметим незавершёнку
@@CK_done:	mov	ah, Cfg_CK_Level[si]    ; уровень этого ЦК
		cmp	ah, Cfg_Curr_Level      ; == текущему уровню опроса ?
		jne	@@CK_next               ; НЕТ -
		test	Cfg_CK_Flags[si], cf_DONE;конфигурация этого ЦК получена ?
		jnz	@@CK_next               ; НЕТ -
		cmp	Cfg_CK_Errs[si], 0      ; есть ошибки на этом ЦК ?
		jne	@@CK_next               ; ДА -
		mov	al, 0ffh                ; есть ЦК на тек.уровне без ошибок и конфигурации
@@CK_next:	inc	si                      ;
		cmp	si, KL_TIS              ; все ЦК проверили ?
		jb	@@CK_loop               ; НЕТ -
		or	al, al                  ; прошли все ЦК с тек.уровнем?
		jnz	@@exit                  ; НЕТ -
		mov	al, Cfg_Curr_Level      ;
		cmp	al, CFG_MAX_Level       ;
		jae	@@is_exit               ;
		inc	Cfg_Curr_Level          ;
@@exit:         clc
		ret
@@is_exit:	or	bl, bl                  ; есть ЦК без конфигурации ?
		jnz	@@second_pass           ; ДА -
		mov	Cfg_Run, 0              ; НЕТ - завершаем
                xor     dl, dl                  ; сбор конфигурации закончился сам !!!!
                call    SendStopConfig
                stc
		ret
@@second_pass: 	mov	Cfg_Check_Level, 0	;
		clc
                ret
IsConfigOK      ENDP


; вход:         dl - признак конца сбора конфигурации
SendStopConfigFar       PROC    FAR
                call    SendStopConfig
		retf
SendStopConfigFar       ENDP

SendStopConfig  PROC
                call    PUMA_Start_Tar
                mov     al, 0f2h                ; конец сбора конфигурации
                @_STOSB
                mov     al, dl                  ; признак конца сбора конфигурации
                @_STOSB
                call    PUMA_Send_Tar
                ret
SendStopConfig  ENDP


StartConfigFar  PROC    FAR
                test    FG_PROGRAM, _fg_prg_ck0
                jnz     StartConfig_CK0
                call    StartConfig
		retf
StartConfig_CK0:; -- работа с ЦК0 ---------------------------------------------
@@is_stop:      ; остановим конфигу с ЦК0 ?
                cmp     Cfg_Stop, 0
                je      @@is_start
                mov     Cfg_Stop, 0
                @POLUCH_ADR_MO
                or      dl, dl                  ; есть возможность послать?
                jnz     @@exit                  ; НЕТ -
                mov     eax, 0020404ebh
                stosd
                mov     ax, 00feh
                stosw
                @ZAPIS_OK_MO
                retf
@@is_start:     ; запросим конфиг у ЦК0 ?
                cmp     Cfg_Run, 0
                je      @@exit
		; ---
                mov     Cfg_Free_Ptr, SMESH_KONF; указатель свободного места на начало SEG_KONF
		call    ClearConfig
		mov	es, ss:SEG_AXD
		mov	al, 0ffh
		mov	cx, 128
		lea	di, TIS_TIS
		rep	stosb
		; ---
                @POLUCH_ADR_MO
                or      dl, dl                  ; есть возможность послать?
                jnz     @@exit                  ; НЕТ -
                mov     eax, 0020404ebh
                stosd
                mov     ax, 01feh
                stosw
                @ZAPIS_OK_MO
@@exit:         retf
StartConfigFar  ENDP

StartConfig     PROC
                test    FG_PROGRAM, _fg_prg_ck0 ; работаем с ЦК0 ?
                jnz     ClearConfig             ; ДА - просто почистимся
                ; --- инициализация переменных
                mov     Cfg_Run, 0              ;
                mov     Cfg_CK_Count, 0         ; пока в тарификационной сети нет ЦК
                mov     Cfg_Curr_CK, 1          ; начинаем с 1-го, а не с 0-го (зачем мне своя конфигурация ?)
                mov     Cfg_Curr_Level, 1       ; начинаем с ЦК первого уровня
                mov     Cfg_Check_Level, 255    ; будем учитывать уровень ЦК в порядке опроса
                mov     Cfg_Free_Ptr, SMESH_KONF; указатель свободного места на начало SEG_KONF
                mov     Cfg_from_One_CK, -1     ;
                ; --- очистка рабочих массивов
                push    ds
                pop     es
                mov     cx, KL_TIS / 4
                xor     eax, eax
                lea     di, Cfg_CK_Flags        ; чистим все флаги (ничего не было)
                rep     stosd
                mov     cx, KL_TIS / 4
                lea     di, Cfg_CK_Errs
                rep     stosd                   ; чистим все ошибки (ничего не было)
                mov     cx, KL_TIS / 2
                lea     di, Cfg_CK_Mod_OK
                rep     stosd                   ; чистим флаги получения конфигурации от блоков
                mov     cx, KL_TIS / 2
                lea     di, Cfg_CK_Mod_Err
                rep     stosd                   ; чистим флаги не получения конфигурации от блоков
                mov     cx, KL_TIS * 8
                lea     di, Cfg_CK_Lines
                rep     stosd                   ; чистим счетчики принятых линий
                not     eax
                mov     cx, KL_TIS / 4
                lea     di, Cfg_CK_Level
                rep     stosd                   ; чистим все уровни
                xor     eax, eax
                mov     cx, KL_TIS
                lea     di, Cfg_CK_Prefix
                rep     stosd                   ; чистим все префиксы
                ; --- очистка SEG_NLOG
		mov	Cfg_NLOG_Count, 0	; !!!
                mov     es, ss:SEG_NLOG
                xor     edi, edi
                mov     al, 0ffh
                xor     ecx, ecx
                mov     cl, DESCR_SEG_NLOG.ATTR_2
                mov     ah, cl
                and     cl, 0fh
                shl     ecx, 10h
                mov     cx, DESCR_SEG_NLOG.LIM
                test    ah, 80h
                jz      SHORT @@clear_NLOG
                shl     ecx, 12
@@clear_NLOG:   @_RSTOSB
                ; --- {Paul} --- (15/11/2011) ---
                ; --- очистка SEG_KONF
                push    es
                mov     es, ss:SEG_KONF
                xor     edi, edi
                mov     al, 0ffh
                xor     ecx, ecx
                mov     cl, DESCR_SEG_KONF.ATTR_2
                mov     ah, cl
                and     cl, 0fh
                shl     ecx, 10h
                mov     cx, DESCR_SEG_KONF.LIM
                test    ah, 80h
                jz      SHORT @@clear_KONF
                shl     ecx, 12
@@clear_KONF:   @_RSTOSB
                pop     es
                ; --- {Paul} --- (End) ---
		; ---
                call    Clear_ULOG
                ; --- расчёт уровней ЦК по тар. сети
                mov     Cfg_CK_Level, 0         ; у тарификатора уровень 0
                movzx   bx, TIS_ROAD            ; ЦК на нулевом модеме тарификатора
                cmp     bl, KL_TIS              ; есть ?
                jae     @@exit                  ; НЕТ -
                mov     dx, 0201h               ; dl - текущий уровень, dh - следующий
                mov     Cfg_CK_Level[bx], dl    ; ЦК на 0 модеме тар. - единств. ЦК первого уровня
                inc     Cfg_CK_Count            ;
                mov     cx, KL_LEV              ; цикл до макс возможного уровня ЦК
@@lev_loop:     push    cx
		xor     bx, bx
                mov     cx, KL_TIS              ; по всем ЦК
                lea     si, Cfg_CK_Level        ; в массиве уровней
@@ck_loop:      lodsb                           ; получим уровень очередного ЦК
		cmp     al, dl                  ; он == текущему
                jne     @@ck_next               ; НЕТ - пока пропустим
		push    bx
                push    cx                      ; ДА - смотрим кто подключен к нему
                mov     cx, 16                  ; 16 модемов
                shl     bx, 4                   ; 16 байт на ЦК в TIS_ROAD
@@blk_loop:     cmp     TIS_ROAD[bx], KL_TIS    ; есть что-то на очередном модеме ?
                jae     @@blk_next              ; НЕТ -
                movzx   di, TIS_ROAD[bx]        ; ЕСТЬ - получим № ЦК
                cmp     Cfg_CK_Level[di], KL_TIS; в массиве уровней этот ЦК уже описан ?
                jb      @@blk_next              ; ДА - пропустим
                mov     Cfg_CK_Level[di], dh    ; НЕТ - отметим что это будет ЦК следующего уровня
                inc     Cfg_CK_Count            ;
                mov     Cfg_MAX_Level, dh
@@blk_next:     inc	bx
		loop    @@blk_loop
                pop     cx
                pop     bx
@@ck_next:      inc	bx
		loop    @@ck_loop
                add     dx, 0101h               ; ++ и текщий и следующий уровень
		pop     cx
                loop    @@lev_loop
                mov     Cfg_Run, 255
                ; - cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 0f1h                ; начало сбора общей конфигурации т.к. ЦК № 0 !!!
                @_STOSB
                xor     al, al                  ; № ЦК
                @_STOSB
                call    PUMA_Send_Tar
                ; - cmd to PUMA
                call    PUMA_Start_Tar
                mov     al, 0ffh                ; список всех ЦК !!!
                @_STOSB
                mov     al, Cfg_CK_Count        ; количество ЦК в списке
                inc     al
                @_STOSB
                xor     bx, bx
                mov     cx, KL_TIS
@@ck_list:      cmp     Cfg_CK_Level[bx], 0ffh
                je      @@ck_list_next
                mov     al, bl
                @_STOSB
                mov     al, Cfg_CK_Level[bx]
                @_STOSB
@@ck_list_next: inc     bx
                loop    @@ck_list
                call    PUMA_Send_Tar
                ; - cmd to PUMA
@@exit:         ret
StartConfig     ENDP

ClearConfig     PROC
                ; --- очистка рабочих массивов
		cld
                push    ds
                pop     es
                mov     cx, KL_TIS / 4
                xor     eax, eax
                lea     di, Cfg_CK_Flags        ; чистим все флаги (ничего не было)
                rep     stosd
                mov     cx, KL_TIS / 4
                lea     di, Cfg_CK_Errs
                rep     stosd                   ; чистим все ошибки (ничего не было)
                mov     cx, KL_TIS / 2
                lea     di, Cfg_CK_Mod_OK
                rep     stosd                   ; чистим флаги получения конфигурации от блоков
                mov     cx, KL_TIS / 2
                lea     di, Cfg_CK_Mod_Err
                rep     stosd                   ; чистим флаги не получения конфигурации от блоков
                mov     cx, KL_TIS * 8
                lea     di, Cfg_CK_Lines
                rep     stosd                   ; чистим счетчики принятых линий
                not     eax
                mov     cx, KL_TIS / 4
                lea     di, Cfg_CK_Level
                rep     stosd                   ; чистим все уровни
                xor     eax, eax
                mov     cx, KL_TIS
                lea     di, Cfg_CK_Prefix
                rep     stosd                   ; чистим все префиксы
                ; --- очистка SEG_NLOG
		mov	Cfg_NLOG_Count, 0	; !!!
                mov     es, ss:SEG_NLOG
                xor     edi, edi
                mov     al, 0ffh
                xor     ecx, ecx
                mov     cl, DESCR_SEG_NLOG.ATTR_2
                mov     ah, cl
                and     cl, 0fh
                shl     ecx, 10h
                mov     cx, DESCR_SEG_NLOG.LIM
                test    ah, 80h
                jz      SHORT @@clear_NLOG
                shl     ecx, 12
@@clear_NLOG:   @_RSTOSB
                ; --- очистка SEG_KONF
                mov     es, ss:SEG_KONF
                xor     edi, edi
                mov     al, 0ffh
                xor     ecx, ecx
                mov     cl, DESCR_SEG_KONF.ATTR_2
                mov     ah, cl
                and     cl, 0fh
                shl     ecx, 10h
                mov     cx, DESCR_SEG_KONF.LIM
                test    ah, 80h
                jz      SHORT @@clear_KONF
                shl     ecx, 12
@@clear_KONF:   @_RSTOSB
		; ---
                call    Clear_ULOG
		; ---
		ret
ClearConfig     ENDP

AskConfig	PROC
		test	FG_PROGRAM, _fg_prg_ck0
		jz	@@exit
		cmp	Cfg_Asked, 0
		jne	@@exit
		cmp	MO_Idx, 0
		jne	@@exit
	        mov     eax, SS:Curr_T
        	sub     eax, Cfg_Time
		cmp	eax, 500
		jb	@@exit
		mov	Cfg_Asked, 255
		mov	Cfg_Run, 255
                @POLUCH_ADR_MO
                or      dl, dl                  ; есть возможность послать?
                jnz     @@exit                  ; НЕТ -
                mov     eax, 0020404ebh
                stosd
                mov     ax, 01feh
                stosw
                @ZAPIS_OK_MO
@@exit:         ret
AskConfig	ENDP



; === работа с номерами телефонов =======================================================
;
; --- преобразование номера из формата СИДС в двоичное DD -------------------------------
; вход:         eax - номер в формате СИДС
; выход:        eax - номер в формате NLOG
;
@Format_Num     MACRO
                xchg    ah, al                  ; приводим номер из SOED
                ror     eax, 16                 ; к формату чёрного списка:
                xchg    ah, al                  ; 1-ая в ст. тетраде ст. байта
                push    bx
                push    cx
                movzx   bx, LENGTH_NUM
                mov     cl, Cfg_Num_Shift[bx]
                shr     eax, cl
                pop     cx
                pop     bx
                ENDM

; --- найти номер в SEG_NLOG ------------------------------------------------------------
; вход:         eax - номер длиной LENGTH_NUM в формате NLOG
; выход:        CF  -  ОК,  не найден
;               edx - абс. физ. № линии в формате СИДС
;               esi - указатель на место, где прекращён поиск
;
Find_Tel_Num    PROC
                ; --- начальные установки
                push    ds
                push    ebx
                push    ecx
                mov     ds, ss:SEG_NLOG         ; сегмент номеров телефонов
                mov     esi, 03fffh             ; середина (если * 8 и не брать последние 64 к)
                mov     cx, 14                  ; максимальное количество итераций поиска
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
Find_Tel_Num    ENDP

Find_Tel_Num_Far        PROC FAR
                call    Find_Tel_Num
                retf
Find_Tel_Num_Far        ENDP

; --- добавить номер в SEG_NLOG ---------------------------------------------------------
; вход:         eax - номер длиной LENGTH_NUM в формате СИДС
;               edx - абс. физ. № линии в формате СИДС
; выход:        CF  -  ОК,  не выполнено
;               eax - номер в формате NLOG
;               esi - указатель на вставленный номер
;               данные в SEG_KONF
;
Add_Tel_Num     PROC
                push    ecx
                @Format_Num                     ; номер в форму пригодную для двоичного поиска
                mov     es, ss:SEG_NLOG
                xor     esi, esi                ; если будет пусто, вставим в начало NLOG
                mov     ecx, Cfg_NLOG_Count     ; есть номера в NLOG ?
                jecxz   @@insert                ; НЕТ -
                cmp     ecx, _CFG_NLOG_MAX - 1  ; ДА - но не много ли их ?
                jae     @@not_add               ; МНОГО -
                push    edx
                call    Find_Tel_Num            ; ищем
                pop     edx
                jnc     @@not_found             ; НЕ нашли -
                ; --- очень странно - нашли !!! => просто заменим АФН
@@found:        mov     es:[esi + 4], edx       ; на всякий случай перезапишем АФН этого лог. номера
                pop     ecx
                stc                             ; всё равно ОК ?
                ret
                ; --- очень нормально - не нашли => раздвинем NLOG, вставим номера и Count++
@@not_found:    ; раздвигаем
                push    edi
                push    esi
                shl     ecx, 3                  ; 8 байт на номер
                mov     edi, ecx                ; указатель на начало свободного номера в NLOG
		add	edi, 4
                sub     ecx, esi                ; столько байт надо подвинуть
                shr     ecx, 2                  ; а столько DD
                mov     esi, edi                ; \ отсюда начнём
                sub     esi, 8                  ; / двигать
                push    ds
                mov     ds, ss:SEG_NLOG
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
                inc     Cfg_NLOG_Count          ; отмечаем что стало на номер больше
                pop     ecx
                stc                             ; ОК
                ret
                ; --- не смогли вставить
@@not_add:      pop     ecx
                clc                             ; не ОК
                ret
Add_Tel_Num     ENDP

; --- удалить номер из SEG_NLOG ---------------------------------------------------------
; вход:         eax - номер длиной LENGTH_NUM в формате СИДС
; выход:        CF  -  ОК,  не выполнено
;               eax - номер в формате NLOG
;               edx - АФН удалённого номера
;               esi - указатель на удалённый номер
;               данные в SEG_KONF
;
Del_Tel_Num     PROC
                cmp     Cfg_NLOG_Count, 0       ; есть что удалять ?
                je      @@not_del               ; НЕЧЕГО -
                @Format_Num                     ; преобразуем номер
                call    Find_Tel_Num            ; ищем
                jnc     @@not_del               ; НЕ нашли -
                ; --- нашли, щас удалим
                push    ecx
                push    edi
                push    esi
                mov     ecx, Cfg_NLOG_Count     ; столько номеров в SEG_NLOG
                shl     ecx, 3                  ; 8 байт на номер
                sub     ecx, esi                ; столько байт надо подвинуть
                shr     ecx, 2                  ; а столько DD
                mov     edi, esi                ; сюда будем надвигать
                add     esi, 8                  ; отсюда будем двигать
                push    ds
                mov     ds, ss:SEG_NLOG
                mov     es, ss:SEG_NLOG
                cld                             ; кашу маслом не испортишь !
                rep     movsd                   ; двигаем
                pop     ds
                pop     esi
                pop     edi
                pop     ecx
                dec     Cfg_NLOG_Count          ; отметим что один номер удалили
                stc                             ; ОК
                ret
                ; --- не удалили
@@not_del:      clc                             ; не ОК
                ret
Del_Tel_Num     ENDP


SetTime         PROC
                test    FG_PROGRAM, _fg_prg_ck0 ; работаем с ЦК0 ?
                jnz     @@exit                  ; ДА - сами конфигу не собираем
                ; --- если нет сервера АТС
                cmp     MES, 0                  ;
                je      @@exit                  ;
                cmp     DEN_MES, 0              ;
                je      @@exit                  ;
                ; --- проверим идёт ли раздача времени
                cmp     ST_Run, 0               ;
                je      @@can_start             ;
                ; --- проверим истёк ли тайм-аут с момента проверки предыдущего ЦК
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
                sub     eax, ST_TOut
                cmp     eax, _ST_TOUT           ; время ожидания вышло ?
                jb      @@exit                  ; ещё НЕТ -
                ; ---
                movzx   si, ST_Curr_CK          ; очередной ЦК
                mov     es, ss:SEG_AXD
                cmp     es:TIS_TIS[si], KL_TIS  ; ЦК с № TS_Curr_CK есть в сети ?
                jae     @@next_2                ; НЕТ - пропускаем
                ; ---
                @POLUCH_ADR_MO
                or      dl, dl
                jnz     @@exit                  ; БУФЕР ЗАНЯТ
                mov     al, 135                 ; дата и время для ЦК
                stosb
                mov     al, ST_Curr_CK
                stosb
                mov     al, GOD_
                stosb
                mov     al, GOD
                stosb
                mov     ax, WORD PTR MES
                stosw
                mov     ax, WORD PTR MIN
                stosw
                cmp     WORD PTR MES, 0
                jz      @@next
                @ZAPIS_OK_MO
                ; ---
@@next:
IF _MSP EQ TRUE
                mov     eax, Sys_Time
ELSE
                mov     eax, Current_Time
ENDIF
                mov     ST_TOut, eax
@@next_2:       inc     ST_Curr_CK
                cmp     ST_Curr_CK, KL_TIS
                jb      @@exit
                mov     ST_Done, 255
                mov     ST_Run, 0
                ; ---
@@exit:         ret
                ; --- определение момента начала раздачи времени - наступление новой минуты
@@can_start:    cmp     SECUNDA, 0
                jne     @@quit
                cmp     ST_Done, 0
                jne     @@exit
                mov     ST_Done, 0
                mov     ST_Run, 255
                mov     ST_Curr_CK, 1
                ret
@@quit:         mov     ST_Done, 0
                ret
SetTime         ENDP



