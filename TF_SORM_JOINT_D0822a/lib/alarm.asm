_Alarm_No_Drebezg       equ     TRUE

; /====\  |       /====\  /====\  /\  /\
; |    |  |       |    |  |    |  |\  /|
; |    |  |       |    |  |    |  |\\//|
; |====|  |       |====|  |====/  | \/ |
; |    |  |       |    |  | \     |    |
; |    |  \====/  |    |  |  \    |    |

; *****************************************************************************
; *  Обработка и отсылка ошибок MO_94
; *****************************************************************************
; ----=> AlarmProccess <=-----------------------
; Created by Smaller on 11/12/04 18:53
; вызывается в основном цикле
; производит отработку и отсылку аварийной ситуации
AlarmProccess   PROC
        mov     ds, ss:seg_ax
        mov     ax, TIME_2MC            ; скока время?
        sub     ax, Alarm_MO_timeout    ; сверим часы
        cmp     ax, _Alarm_MO_timeout   ; надо корректировать?
        jb      @@exit                  ; нет, пропустим
        mov     ax, TIME_2MC            ; еще раз, скока время?
        mov     Alarm_MO_timeout, ax    ; подкоректируем

        call    AlarmProccessMO         ; обработаем ошибки МО
        call    AlarmProccessFapch      ; обработаем ошибки Фапча
        call    AlarmProccessIP         ; обработаем ошибки Источников Питания
        call    AlarmProccessTF         ; обработаем тарификационные ошибки

@@exit:
        ret
AlarmProccess   ENDP


; ----=> AlarmProccessMO <=-----------------------
; Created by Smaller on 11/12/04 19:19
; обрабатывает данные МО
AlarmProccessMO PROC
        xor     al, al
IF _MSP EQ TRUE
        mov     dx, OSH_VX_P
ELSE
        movzx   dx, OSH_VX_P
ENDIF
        cmp     Alarm_OSH_VX_P, dx
        je      @@no_Alarm_OSH_VX_P
        or      al, _Alarm_OSH_VX_P
        mov     Alarm_OSH_VX_P, dx
@@no_Alarm_OSH_VX_P:
IF _MSP EQ TRUE
        mov     dx, OSH_T_O
ELSE
        movzx   dx, OSH_T_O
ENDIF
        cmp     Alarm_OSH_T_O, dx
        je      @@no_Alarm_OSH_T_O
        or      al, _Alarm_OSH_T_O
        mov     Alarm_OSH_T_O, dx
@@no_Alarm_OSH_T_O:
IF _MSP EQ TRUE
        mov     dx, OSH_PAK
ELSE
        movzx   dx, OSH_PAK
ENDIF
        cmp     Alarm_OSH_PAK, dx
        je      @@no_Alarm_OSH_PAK
        or      al, _Alarm_OSH_PAK
        mov     Alarm_OSH_PAK, dx
@@no_Alarm_OSH_PAK:
IF _MSP EQ TRUE
        mov     dx, OSH_GOT
ELSE
        movzx   dx, OSH_GOT
ENDIF
        cmp     Alarm_OSH_GOT, dx
        je      @@no_Alarm_OSH_GOT
        or      al, _Alarm_OSH_GOT
        mov     Alarm_OSH_GOT, dx
@@no_Alarm_OSH_GOT:

        cmp     Alarm_MO, al            ; отправляли такую комбинацию?
        je      @@exit                  ; да -> уйдем на Х...
        mov     Alarm_MO, al            ; пометим что отправили

        mov     ah, _Alarm_MO_Group     ; первая группа
        call    AlarmProccessSend
@@exit:
        ret
AlarmProccessMO ENDP


; ----=> AlarmProccessFapch <=-----------------------
; Created by Smaller on 11/12/04 19:43
; Обработка данных ФАПЧа
AlarmProccessFapch      PROC
        mov     al, IND_PHA
        and     al, _b_00 OR _b_01

        cmp     Alarm_Fapch, al         ; отправляли такую комбинацию?
        je      @@exit                  ; да -> уйдем на Х...
        mov     Alarm_Fapch, al         ; пометим что отправили

        mov     ah, _Alarm_Fapch_Group  ; первая группа
        call    AlarmProccessSend
@@exit:
        ret
AlarmProccessFapch      ENDP


; ----=> AlarmProccessIP <=-----------------------
; Created by Smaller on 11/13/04 10:38
; обработка аварий источников питания
AlarmProccessIP PROC
        xor     al, al

        push    es
        mov     es, ss:seg_gs
        mov     dl, es:[_reg_statusPit]
        pop     es
        and     dl, _b_00 or _b_04
        or      al, dl
        rol     al, 4           ; \ смена источников, в данный момент не нужна
        mov     statusPit, dl   ; / индикация ?

        cmp     Alarm_IP, al            ; отправляли такую комбинацию?
        je      @@exit                  ; да -> уйдем на Х...
        mov     Alarm_IP, al            ; пометим что отправили

        mov     ah, _Alarm_IP_Group     ; первая группа
        call    AlarmProccessSend
@@exit:
        ret
AlarmProccessIP ENDP


; ----=> AlarmProccessTF <=-----------------------
; Created by Smaller on 02/08/05 11:57
; WHAT DO?
;Входные данные:
;На выходе:
AlarmProccessTF proc
        public  AlarmProccessTF

        test    FG_PROGRAM, _b_00
        jz      @@exit

        xor     al, al

        test    fg_tf, _b_05
        jz      @@no_peregruz
        or      al, _b_02
@@no_peregruz:

        cmp     Alarm_TF, al            ; отправляли такую комбинацию?
        je      @@exit                  ; да -> уйдем на Х...
        mov     Alarm_TF, al            ; пометим что отправили

        mov     ah, _Alarm_TF_Group     ; первая группа
        call    AlarmProccessSend
@@exit:
        ret
AlarmProccessTF endp
; ----=> AlarmProccessSend <=-----------------------
; Created by Smaller on 11/12/04 18:55
; отсылает аварийную ситуацию по МО
;Входные данные: в al - расшифровка аварии
;                в ah - номер группы аварии
;На выходе: CR  - все ОК
;           CR  - не удалось отправить
AlarmProccessSend       PROC
        push    es
        push    ax
        ; отсылка аварий блока
        @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
        OR      DL,DL                   ; есть возможность послать?
        JNZ     @@exit_error            ; нет -> пропустим

        MOV     AL,94                   ; МО31 - авария блока
        STOSB                           ; запишим
        pop     ax                      ; востановим группу и ошибку
        ror     ax, 8
        stosw                           ; запишим
        @ZAPIS_OK_MO                    ; пошлем
@@exit:
        pop     es
        stc
        ret
@@exit_error:
        pop     ax
        pop     es
        clc
        ret
AlarmProccessSend       ENDP



; ----=> AlarmManager <=-----------------------
; Created by Smaller on 12/07/04 6:30
; обработка ошибок и отсылка мо
AlarmManager    PROC
        push    gs
        mov     gs, ss:seg_gs
        ; обработаем источники питания
        mov     dl, gs:[_reg_statusPit] ; возьмем данные источников питания
;-------старое по позжей надо удалить--------------------------------
;        MOV     DH,DL                   ; сдублируем
;        SHR     DL,4                    ; оставим второй в виде первого
;        AND     DH,1                    ; очистим первый от второго
;        MOV     WORD PTR I_P,DX         ; сохраним для дальнейшей обработки
;        mov     dl ,statusPit           ; запишим для дальнейшего использования
;---------------------------------------
        and     dl, _b_00 OR _b_04      ; уберем лишнее
        mov     statusPit, dl           ; запишим для дальнейшего использования
        cmp     statusPitOld, dl
        je      @@noSendStatusPit
        ;mov     si, offset moSendBuf
        ;mov     [si], byte ptr 26       ; передача состояния источников питания
        ;mov     [si + 1], dl            ; регистр питания
        ;mov     [si + 2], byte ptr 7    ; всегда 7
        ;mov     cx, 3
        ;call    mo_Send
        ;jnc     @@noSendStatusPit

	; NEW EDITION _ STABLE
if 0;;;1 - ЦК не принимает 26-ую команду от другого ЦК (а Т&С прописывается как ЦК) !!!!!!!
        push    dx
        ; отсылка аварий блока
        @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
        mov     al, dl
        pop	dx
        or      al, al
        JNZ     @@noSendStatusPit       ; нет -> пропустим

        MOV     AL, 26                  ; МО26 - состояние источников питания
        STOSB                           ; запишим
        mov     al, dl
        stosb
        mov	al, 7
        stosb
        @ZAPIS_OK_MO                    ; пошлем

        mov     statusPitOld, dl
endif
@@noSendStatusPit:
        ; межпроцессорный ошибка
        xor     ax, ax
        mov     di, mo_idx
        mov     al, ala_mo[di]
        ; источники питания
        mov     dh, statusPit
        mov     dl, dh
        shr     dl, 4
        or      dl, dh
        and     dl, 1
        shl     dl, 1
        or      al, dl
        ; ФАПЧ
        ;or      al, IND_PHA
        ; SORM
        test    FG_PROGRAM, _b_01
        jz      @@no_sorm_alarm
        cmp     sost_sdl, 6
        je      @@chl1_norm
        or      al, _b_01
@@chl1_norm:
        cmp     sost_sdl[2], 6
        je      @@chl2_norm
        or      al, _b_01
@@chl2_norm:
@@no_sorm_alarm:
        ; TARIF
        test    FG_PROGRAM, _b_00
        jz      @@no_tarif_alarm

        test    fg_tf, _b_05
        jz      @@no_peregruz
        or      al, _b_00
@@no_peregruz:
@@no_tarif_alarm:

        push    ax

        mov     dx, time_2mc
        sub     dx, sendAlarm_timeOut
        cmp     dx, _sendAlarm_timeOut
        jb      @@no_send_alarm

        mov     dx, time_2mc
        mov     sendAlarm_timeOut, dx
if 0;;;1
        push    ax
        ; отсылка аварий блока
        @POLUCH_ADR_MO                  ; получаем указатель на буфер МО
        OR      DL,DL                   ; есть возможность послать?
        pop	dx
        JNZ     @@no_send_alarm         ; нет -> пропустим

        mov     al, 32

        or      dl, dl
        jz      @@no_alarm

        mov     al, 31

@@no_alarm:
        stosb

        mov     al, 34
        stosb

        @ZAPIS_OK_MO                    ; пошлем
endif

@@no_send_alarm:
        pop     ax

        or      al, al
        jnz     @@view_alarm
        mov     al, 4
@@view_alarm:

        mov     gs, ss:seg_st
        mov     ah, gs:[_reg_stativ]
        and     ah, 0C0h                ; выделим WatchDog
        or      al, ah                  ; смикшируем WatchDog со стативной сигнализацией
        and     al, NOT _b_06           ; на всякий случай включим WatchDog
        xor     al, _b_07               ; инвертируем WatchDog !!!! чтоб не висело!
        mov     gs:[_reg_stativ],al
        pop     gs
        ret
AlarmManager    ENDP

