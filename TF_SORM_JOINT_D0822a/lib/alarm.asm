_Alarm_No_Drebezg       equ     TRUE

; /====\  |       /====\  /====\  /\  /\
; |    |  |       |    |  |    |  |\  /|
; |    |  |       |    |  |    |  |\\//|
; |====|  |       |====|  |====/  | \/ |
; |    |  |       |    |  | \     |    |
; |    |  \====/  |    |  |  \    |    |

; *****************************************************************************
; *  ��ࠡ�⪠ � ���뫪� �訡�� MO_94
; *****************************************************************************
; ----=> AlarmProccess <=-----------------------
; Created by Smaller on 11/12/04 18:53
; ��뢠���� � �᭮���� 横��
; �ந������ ��ࠡ��� � ���뫪� ���਩��� ���樨
AlarmProccess   PROC
        mov     ds, ss:seg_ax
        mov     ax, TIME_2MC            ; ᪮�� �६�?
        sub     ax, Alarm_MO_timeout    ; ᢥਬ ���
        cmp     ax, _Alarm_MO_timeout   ; ���� ���४�஢���?
        jb      @@exit                  ; ���, �ய��⨬
        mov     ax, TIME_2MC            ; �� ࠧ, ᪮�� �६�?
        mov     Alarm_MO_timeout, ax    ; �����४��㥬

        call    AlarmProccessMO         ; ��ࠡ�⠥� �訡�� ��
        call    AlarmProccessFapch      ; ��ࠡ�⠥� �訡�� ����
        call    AlarmProccessIP         ; ��ࠡ�⠥� �訡�� ���筨��� ��⠭��
        call    AlarmProccessTF         ; ��ࠡ�⠥� ��䨪�樮��� �訡��

@@exit:
        ret
AlarmProccess   ENDP


; ----=> AlarmProccessMO <=-----------------------
; Created by Smaller on 11/12/04 19:19
; ��ࠡ��뢠�� ����� ��
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

        cmp     Alarm_MO, al            ; ��ࠢ�﫨 ⠪�� ���������?
        je      @@exit                  ; �� -> 㩤�� �� �...
        mov     Alarm_MO, al            ; ����⨬ �� ��ࠢ���

        mov     ah, _Alarm_MO_Group     ; ��ࢠ� ��㯯�
        call    AlarmProccessSend
@@exit:
        ret
AlarmProccessMO ENDP


; ----=> AlarmProccessFapch <=-----------------------
; Created by Smaller on 11/12/04 19:43
; ��ࠡ�⪠ ������ �����
AlarmProccessFapch      PROC
        mov     al, IND_PHA
        and     al, _b_00 OR _b_01

        cmp     Alarm_Fapch, al         ; ��ࠢ�﫨 ⠪�� ���������?
        je      @@exit                  ; �� -> 㩤�� �� �...
        mov     Alarm_Fapch, al         ; ����⨬ �� ��ࠢ���

        mov     ah, _Alarm_Fapch_Group  ; ��ࢠ� ��㯯�
        call    AlarmProccessSend
@@exit:
        ret
AlarmProccessFapch      ENDP


; ----=> AlarmProccessIP <=-----------------------
; Created by Smaller on 11/13/04 10:38
; ��ࠡ�⪠ ���਩ ���筨��� ��⠭��
AlarmProccessIP PROC
        xor     al, al

        push    es
        mov     es, ss:seg_gs
        mov     dl, es:[_reg_statusPit]
        pop     es
        and     dl, _b_00 or _b_04
        or      al, dl
        rol     al, 4           ; \ ᬥ�� ���筨���, � ����� ������ �� �㦭�
        mov     statusPit, dl   ; / �������� ?

        cmp     Alarm_IP, al            ; ��ࠢ�﫨 ⠪�� ���������?
        je      @@exit                  ; �� -> 㩤�� �� �...
        mov     Alarm_IP, al            ; ����⨬ �� ��ࠢ���

        mov     ah, _Alarm_IP_Group     ; ��ࢠ� ��㯯�
        call    AlarmProccessSend
@@exit:
        ret
AlarmProccessIP ENDP


; ----=> AlarmProccessTF <=-----------------------
; Created by Smaller on 02/08/05 11:57
; WHAT DO?
;�室�� �����:
;�� ��室�:
AlarmProccessTF proc
        public  AlarmProccessTF

        test    FG_PROGRAM, _b_00
        jz      @@exit

        xor     al, al

        test    fg_tf, _b_05
        jz      @@no_peregruz
        or      al, _b_02
@@no_peregruz:

        cmp     Alarm_TF, al            ; ��ࠢ�﫨 ⠪�� ���������?
        je      @@exit                  ; �� -> 㩤�� �� �...
        mov     Alarm_TF, al            ; ����⨬ �� ��ࠢ���

        mov     ah, _Alarm_TF_Group     ; ��ࢠ� ��㯯�
        call    AlarmProccessSend
@@exit:
        ret
AlarmProccessTF endp
; ----=> AlarmProccessSend <=-----------------------
; Created by Smaller on 11/12/04 18:55
; ���뫠�� ���਩��� ����� �� ��
;�室�� �����: � al - ����஢�� ���ਨ
;                � ah - ����� ��㯯� ���ਨ
;�� ��室�: CR  - �� ��
;           CR  - �� 㤠���� ��ࠢ���
AlarmProccessSend       PROC
        push    es
        push    ax
        ; ���뫪� ���਩ �����
        @POLUCH_ADR_MO                  ; ����砥� 㪠��⥫� �� ���� ��
        OR      DL,DL                   ; ���� ����������� ��᫠��?
        JNZ     @@exit_error            ; ��� -> �ய��⨬

        MOV     AL,94                   ; ��31 - ����� �����
        STOSB                           ; ����訬
        pop     ax                      ; ���⠭���� ��㯯� � �訡��
        ror     ax, 8
        stosw                           ; ����訬
        @ZAPIS_OK_MO                    ; ��諥�
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
; ��ࠡ�⪠ �訡�� � ���뫪� ��
AlarmManager    PROC
        push    gs
        mov     gs, ss:seg_gs
        ; ��ࠡ�⠥� ���筨�� ��⠭��
        mov     dl, gs:[_reg_statusPit] ; ���쬥� ����� ���筨��� ��⠭��
;-------��஥ �� ������ ���� 㤠����--------------------------------
;        MOV     DH,DL                   ; �㡫��㥬
;        SHR     DL,4                    ; ��⠢�� ��ன � ���� ��ࢮ��
;        AND     DH,1                    ; ���⨬ ���� �� ��ண�
;        MOV     WORD PTR I_P,DX         ; ��࠭�� ��� ���쭥�襩 ��ࠡ�⪨
;        mov     dl ,statusPit           ; ����訬 ��� ���쭥�襣� �ᯮ�짮�����
;---------------------------------------
        and     dl, _b_00 OR _b_04      ; 㡥६ ��譥�
        mov     statusPit, dl           ; ����訬 ��� ���쭥�襣� �ᯮ�짮�����
        cmp     statusPitOld, dl
        je      @@noSendStatusPit
        ;mov     si, offset moSendBuf
        ;mov     [si], byte ptr 26       ; ��।�� ���ﭨ� ���筨��� ��⠭��
        ;mov     [si + 1], dl            ; ॣ���� ��⠭��
        ;mov     [si + 2], byte ptr 7    ; �ᥣ�� 7
        ;mov     cx, 3
        ;call    mo_Send
        ;jnc     @@noSendStatusPit

	; NEW EDITION _ STABLE
if 0;;;1 - �� �� �ਭ����� 26-�� ������� �� ��㣮�� �� (� �&� �ய��뢠���� ��� ��) !!!!!!!
        push    dx
        ; ���뫪� ���਩ �����
        @POLUCH_ADR_MO                  ; ����砥� 㪠��⥫� �� ���� ��
        mov     al, dl
        pop	dx
        or      al, al
        JNZ     @@noSendStatusPit       ; ��� -> �ய��⨬

        MOV     AL, 26                  ; ��26 - ���ﭨ� ���筨��� ��⠭��
        STOSB                           ; ����訬
        mov     al, dl
        stosb
        mov	al, 7
        stosb
        @ZAPIS_OK_MO                    ; ��諥�

        mov     statusPitOld, dl
endif
@@noSendStatusPit:
        ; ���������� �訡��
        xor     ax, ax
        mov     di, mo_idx
        mov     al, ala_mo[di]
        ; ���筨�� ��⠭��
        mov     dh, statusPit
        mov     dl, dh
        shr     dl, 4
        or      dl, dh
        and     dl, 1
        shl     dl, 1
        or      al, dl
        ; ����
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
        ; ���뫪� ���਩ �����
        @POLUCH_ADR_MO                  ; ����砥� 㪠��⥫� �� ���� ��
        OR      DL,DL                   ; ���� ����������� ��᫠��?
        pop	dx
        JNZ     @@no_send_alarm         ; ��� -> �ய��⨬

        mov     al, 32

        or      dl, dl
        jz      @@no_alarm

        mov     al, 31

@@no_alarm:
        stosb

        mov     al, 34
        stosb

        @ZAPIS_OK_MO                    ; ��諥�
endif

@@no_send_alarm:
        pop     ax

        or      al, al
        jnz     @@view_alarm
        mov     al, 4
@@view_alarm:

        mov     gs, ss:seg_st
        mov     ah, gs:[_reg_stativ]
        and     ah, 0C0h                ; �뤥��� WatchDog
        or      al, ah                  ; ᬨ���㥬 WatchDog � ��⨢��� ᨣ������樥�
        and     al, NOT _b_06           ; �� ��直� ��砩 ����稬 WatchDog
        xor     al, _b_07               ; �������㥬 WatchDog !!!! �⮡ �� ��ᥫ�!
        mov     gs:[_reg_stativ],al
        pop     gs
        ret
AlarmManager    ENDP

