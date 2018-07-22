; --- рабочие переменные СОРМ -----------------------------------------------------------
                ; --- вспомогательные перменные для работы с КСЛ
Oper_Do_Flag    DB      0               ; признак выполнения команды
SORM_KSL_Clear  DW      0ffffh          ; №№ которые @KSL_USE требует освободить или 0ffffh
SORM_CK         DB      0               ; для запоминания № ЦК
SORM_Work_Num   DW      0               ; для запоминания рабочего №
; --- <Yu> [14.04.2010] - контроль СОРМовской коммутации
SORM_Mod_Nums   DW      0               ; №№ тайм-слотов модема, запрошенные в МО_120

                ; --- вспомогательные переменные для формирования сообщений
                ; параметры связи
                ;        0     1     2     3     4     5     6     7     вызывающий
                ;       ---   АТС   мест  зона  МГ    МН    СС    ---    вызываемый
SORM_Link_Prms  DB      000h, 011h, 000h, 000h, 000h, 000h, 000h, 000h  ; 0 - неиспользуется
                DB      000h, 01ch, 012h, 012h, 012h, 012h, 000h, 000h  ; 1 - абонент АТС
                DB      000h, 011h, 018h, 018h, 018h, 014h, 000h, 000h  ; 2 - местный абонент
                DB      000h, 011h, 018h, 018h, 018h, 014h, 000h, 000h  ; 3 - зоновый абонент
                DB      000h, 011h, 018h, 018h, 018h, 014h, 000h, 000h  ; 4 - междугородный абонет
                DB      000h, 011h, 018h, 018h, 018h, 014h, 000h, 000h  ; 5 - международный абонет
                DB      000h, 011h, 018h, 018h, 018h, 014h, 000h, 000h  ; 6 - спецслужба
                DB      000h, 011h, 000h, 000h, 000h, 000h, 000h, 000h  ; 7 - неиспользуется

		; --- перекодировка признака отбора номера для CW, CC, HOLD, HOLE
                ;        0     1     2     3     4     5     6     7
SORM_DVO_Get_Num DB      0,   004h, 008h, 00ch, 001h,  0,    0,    0    ; 0
                DB      002h,  0,    0,    0,   003h,  0,    0,    0    ; 8
                DB      010h,  0,    0,    0,    0,    0,    0,    0    ; 16
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 24
                DB      020h,  0,    0,    0,    0,    0,    0,    0    ; 32
                DB      030h,  0,    0,    0,    0,    0,    0,    0    ; 48
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 56
                DB      192 DUP(0)
                ;
SORM_DVO_Flag 	DW	0	; флаги ДВО для для CW, CC, HOLD, HOLE
SORM_3PTY_CT    DB      0       ; флаг прохождения одной из веток проги для 3PTY или CT

                ; --- перекодировка признака отбора номера для сообщения ДВО CF
                ;        0     1     2     3     4     5     6     7
SORM_RA_DVO_Get_Num DB   0,   040h, 080h, 0c0h, 001h,  0,    0,    0    ; 0
                DB      002h,  0,    0,    0,   003h,  0,    0,    0    ; 8
                DB      010h,  0,    0,    0,    0,    0,    0,    0    ; 16
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 24
                DB      020h,  0,    0,    0,    0,    0,    0,    0    ; 32
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 40
                DB      030h,  0,    0,    0,    0,    0,    0,    0    ; 48
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 56
                DB      192 DUP(0)

                ; --- перекодировка признака отбора номера для сообщений после ДВО CF
                ;        0     1     2     3     4     5     6     7
SORM_RA_Mes_Get_Num DB   0,   001h, 002h, 003h, 040h,  0,    0,    0    ; 0
                DB      080h,  0,    0,    0,   0c0h,  0,    0,    0    ; 8
                DB      010h,  0,    0,    0,    0,    0,    0,    0    ; 16
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 24
                DB      020h,  0,    0,    0,    0,    0,    0,    0    ; 32
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 40
                DB      030h,  0,    0,    0,    0,    0,    0,    0    ; 48
                DB       0,    0,    0,    0,    0,    0,    0,    0    ; 56
                DB      192 DUP(0)

                ; --- нумерация вызовов СОРМ
SORM_Call_Num   DW      0
SORM_Curr_Call  DW      0

                ; --- структура, описывающая контролируемый объект
of_AK_STATE     EQU     1               ; флаг сотояния АК

Obj_Struc       STRUC
  ; --- копия полей 5-ой команды
  obj_num       DW      0       ; 00 - 01 - условный номер объекта
  obj_type      DB      0       ; 02      - тип объекта или 0FFh
  obj_attr      DB      0       ; 03      - признак номера телефона
  obj_tel_len   DB      0       ; 04      - количество знаков в номере телефона
  obj_tel_num   DB    9 DUP(0ffh);05 - 13 - цифры номера телефона
  obj_bunch     DW      0ffffh  ; 14 - 15 - условный номер пучка
  obj_cat_ctrl  DB      0       ; 16      - категория контроля
  obj_gr_ksl    DB      0       ; 17      - номер группы КСЛ
  obj_prior     DB      0       ; 18      - приоритет контроля
  obj_ak_state  DB      0       ; 19      - состояние абонентского комплекта
  ; --- флаги
  obj_flags     DB      0       ; 20      - флаг состояния АК
  ; --- сормирование
  obj_get_num   DB      0       ; 21      - признак отбора номера
  ; --- резерв
  obj_reserve   DB     10 DUP(0); 22 - 31 - резерв
Obj_Struc       ENDS

                ; константы
_OBJ_SIZE       = size Obj_Struc        ; размер записи о контролируемом объекте
_OBJ_SHL        EQU     5               ; тоже самое как степень 2 (количество сдвигов)


                ; --- структура, описывающая сессию СОРМ
SORM_Struc      STRUC
                ; ---
sorm_attr_a     DB      0               ; 00      - признак номера A
sorm_len_a      DB      0               ; 01      - количество цифр в номере А
sorm_num_a      DB      9 DUP(0)        ; 02 - 10 - номер А
                ; ---
sorm_attr_b     DB      0               ; 11      - признак номера Б
sorm_len_b      DB      0               ; 12      - количество цифр в номере Б на данный момент
sorm_num_b      DB      9 DUP(0)        ; 13 - 21 - сами цифры в номере Б на данный момент
                ; ---
sorm_attr_c     DB      0               ; 22      - признак номера C
sorm_len_c      DB      0               ; 23      - количество цифр в номере C
sorm_num_c      DB      9 DUP(0)        ; 24 - 32 - номер C
                ; ---
sorm_ksl_a      DB      0               ; 33      -
sorm_ksl_b      DB      0               ; 34      -
sorm_ck_a       DB      0               ; 35      -
sorm_work_a     DW      0               ; 36 - 37 -
sorm_phys_a     DW      0               ; 38 - 39 -
sorm_ck_b       DB      0               ; 40      -
sorm_work_b     DW      0               ; 41 - 42 -
                ; ---
sorm_call       DW      0               ; 43 - 44 -
sorm_opcode     DB      0               ; 45      -
sorm_get_num    DB      0               ; 46      -
sorm_prior      DB      0               ; 47      -
                ; ---
sorm_obj        DW      0ffffh          ; 48 - 49 - ссылка на объект, создавший сессию
sorm_soed       DD      0ffffffffh      ; 50 - 53 - ссылка на соединение, создавшее сессию
                ; ---
sorm_flags      DW      0               ; 54 - 55 -
sorm_time       DD      0               ; 56 - 59 - время создания сессии
                ; ---
sorm_dvo_code   DB      0               ; 60      -
sorm_dvo_phase  DB      0               ; 61      -
                ; ---
sorm_chnl_a     DB      0               ; 62      -
sorm_chnl_b     DB      0               ; 63      -
                ;
SORM_Struc      ENDS

                ; константы
_SORM_SIZE      = size SORM_Struc
                ; флаги
ssf_NUM_FULL    =       0001h           ;
ssf_ANS         =       0002h           ;
ssf_REL         =       0004h           ;
		;
ssf_SWAP_AB     =       0008h           ;
ssf_SWAP_AC     =       0010h           ;
ssf_SWAP_BA     =       0020h           ;
ssf_SWAP_BC     =       8000h;;;0040h           ;
		;
ssf_HOLD        =       0080h           ;
ssf_HOLE        =       0100h           ;
ssf_CW          =       0200h           ;
ssf_CC          =       0400h           ;
ssf_RA          =       0800h           ;
ssf_3PTY        =       1000h           ;
ssf_CT          =       2000h
ssf_DVO         =       ssf_HOLD + ssf_HOLE + ssf_CW + ssf_RA + ssf_3PTY + ssf_CT ;
ssf_TCOP_UPATS  =       4000h           ;
ssf_CMND_8      =       0040h;;;8000h           ;
		;

                ; --- работа с пучками --------------------------------------------------
                ; константы
IF _MSP EQ TRUE
_BUNCH_MAX      EQU     25                 ; max количество объектов - пучков
        ELSE
_BUNCH_MAX      EQU     100                ; max количество объектов - пучков
ENDIF
_BUNCH_SIZE     EQU     _OBJ_SIZE
_BUNCH_SHL      EQU     _OBJ_SHL
                ; константы, описывающие размещение инфы в файле с описанием пучков
_BUNCH_PRM_OFFS  EQU    100000h         ; смещение параметров пучков
                                        ;       байт  0   - если 0, пучок неопределён
                                        ;       байты 1-2 - условный номер пучка
                                        ;       байт  3   - резерв 0
_BUNCH_PRM_SIZE  EQU    4               ; параметры занимают 4 байта
_BUNCH_PRM_SHL   EQU    2               ; то же самое в сдвигах
_BUNCH_NAME_OFFS EQU    _BUNCH_PRM_OFFS + _BUNCH_MAX * 4 ; смещение имён пучков
_BUNCH_NAME_SIZE EQU    43              ; реальный размер имени в байтах

                ; --- работа с объектами (телефонами) -----------------------------------
                ; переменные
Obj_Count       DW      0               ; количество телефонных номеров на контроле
                ; константы
_OBJ_MAX        EQU     1536            ; max количество объектов - телефонов

                ; --- работа с сессиями -------------------------------------------------
                ; константы
_SESSION_MAX    EQU     1023;;;4            ; max количество сессий

                ; --- работа с КСЛ ------------------------------------------------------
_MAX_E1         EQU     1               ; max количество потоков Е1 в сторону ФСБ
_MAX_KSL        EQU     _MAX_E1 * 28    ; max количество КСЛ во всех Е1
_MAX_GROUP      EQU     _MAX_KSL        ; max количество групп в СОРМ
IF _MSP EQ TRUE
_MAX_MODEM_TS   EQU     _MAX_KSL / 2    ; max количество ТЧ
        else
_MAX_MODEM_TS   EQU     _MAX_KSL        ; max количество ТЧ
ENDIF
                ; СОРМовская коммутация
_MODEM_1ST_TS   EQU     60              ; номер начального тайм-слота на модеме
IF _MSP EQ TRUE
_COMM_CHNLS     EQU     14              ; сколько каналов коммутируем с модема
_COMM_AB_SHIFT  EQU     16              ; смещение коммутации для каналов А и Б
_COMM_SUM_SHIFT EQU     32              ; смещение коммутации для сумированных каналов А и Б

                ; ЦТОшная коммутация
_COMM_TO_CTO    EQU     15              ;
_COMM_FROM_CTO  EQU     14              ;
ELSE
;;;<mmm>
_COMM_1ST_BABA  EQU     16              ; сколько каналов коммутируем с модема
_COMM_CHNLS     EQU     _MAX_MODEM_TS   ; сколько каналов коммутируем с модема
_COMM_AB_SHIFT  EQU     _MAX_MODEM_TS   ; смещение коммутации для каналов А и Б
_COMM_SUM_SHIFT EQU     _MAX_MODEM_TS*2 ; смещение коммутации для сумированных каналов А и Б
ENDIF
                ; массивы, индексируемые по № группы
KSL_Group       DB      _MAX_GROUP DUP(0ffh)    ; ff - группы нет
KSL_Group_Type  DB      _MAX_GROUP DUP(0ffh)    ; ff или тип группы КСЛ
KSL_Last        DB      _MAX_GROUP DUP(0ffh)    ; № КСЛ в группе выбранной в предыдущий раз
KSL_Count       DB      _MAX_GROUP DUP(0ffh)    ; количество КСЛ закреплённых за группой
                ; массивы, индексируемые по № КСЛ
KSLs            DB      256 DUP(0ffh)           ; ff или № группы
KSL_A           DB      256 DUP(0ffh)           ; ff или 0 если эта КСЛ А
KSL_Pair        DB      256 DUP(0ffh)           ; ff или № КСЛ парной текущей КСЛ из массива KSL
KSL_Busy        DB      256 DUP(0ffh)           ; ff или если КСЛ занята - приоритет её занятия
KSL_Session     DW      256 DUP(0ffffh)         ; ffff - если КСЛ свободна или ук. на сессию

Modem_KSL       DB      _MAX_MODEM_TS DUP(0ffh) ; ff тайм-слот на модеме не занят или номер КСЛ
_KSL_DATA_SIZE = $ - KSL_Group

                ; флаги допустимых КСЛ
KSL_Enabled     DB      0, 28 DUP(01), 3 DUP(0)
                DB      223 DUP(0), 01

                ; --- отработка 10-ой команды -------------------------------------------
                ; общие переменные
Obj_Data_Run    DB      0               ; флаг - идёт выдача данных
Obj_Data_Time   DD      0               ; время последней передачи
_OBJ_DATA_TOUT  =       250             ; тайм-аут передачи
                ; объекты - найденные и посланные
Obj_Data_Count  DW      0               ; число найденных объектов
Obj_Data_Send   DW      0               ; число отправленных объектов
Obj_Data_Buf    DW      _BUNCH_MAX + _OBJ_MAX DUP(0) ; буфер с результатами поиска - указатели на объекты
                ; буфер под одно сообщение N 3
Obj_Data_Out_Buf DB     10 + 20 DUP(0)  ;
Obj_Data_Out_Len DW     0               ; байт в передающем буфере

                ; --- отработка 11-ой команды -------------------------------------------
KSL_Conform_Count DW    0               ; счётчик отобранных КСЛ
KSL_Conform_Send  DW    0               ; счётчик КСЛ по которым отправлена информация
KSL_Conform_Buff  DD    _MAX_KSL DUP(0) ; буфер для отобранных КСЛ

                ; --- отработка 12-ой команды -------------------------------------------
_DVO_BUFF_SIZE  =       33              ; размер буфера ДВО
DVO_Buff        DB      _DVO_BUFF_SIZE DUP(0) ; сам буфер - добивка тела 12-ой команды до 45 байт

                ; --- отработка 16-ой команды -------------------------------------------
                ; общие переменные
Bunch_Data_Run  DB      0               ; флаг - идёт выдача данных
Bunch_Data_Time DD      0               ; время последней передачи
_BUNCH_DATA_TOUT =      250             ; тайм-аут передачи
                ; объекты - найденные и посланные
Bunch_Data_Count   DW   0               ; число найденных объектов
Bunch_Data_Send    DW   0               ; число отправленных объектов
Bunch_Data_Buf     DD   _BUNCH_MAX DUP(0) ; буфер с результатами поиска - указатели на объенкты
                ; буфер под одно сообщение N 3
Bunch_Data_Out_Buf DB   10 + 43 DUP(0)  ;
Bunch_Data_Out_Len DW   0               ; байт в передающем буфере

                ; --- работа с пароля СОРМ --------------------------
OLD_PAROL_0_3   DD      31313131H       ;ПЕРВОНАЧАЛЬНЫЙ ПАРОЛЬ 0-3
OLD_PAROL_4_5   DW      3131H           ;4-5
PAROL_0_3       DD      1               ;ТЕКУЩИЙ ПАРОЛЬ 0-3
PAROL_4_5       DW      1               ;4-5

                ; --- работа с буфером 1-го канала
POS_SORM        DW      0               ;НОМЕР ПОСЛЕДНЕЙ ЗАПИСАННОЙ В БУФЕР SORM_COM
PER_SORM        DW      0               ;НОМЕР КОМАНДЫ ДЛЯ ВЫДАЧИ
M_COM           EQU     1023            ;МОДУЛЬ КОЛ-ВА КОМАНД

                ; --- работа с буфером 2-го канала
SLED_SOOB       DW      0               ;СЛЕДУЮЩЕЕ СООБЩЕНИЕ К ОБРАБОТКЕ
POS_SOOB        DW      0               ;ТЕКУЩЕЕ МЕСТО ПОД КОМАНДУ
M_SOOB          EQU     1023

                ; --- нумерация соединений для ФСБ
NOM_VIZOVA      DW      0       ;НОМЕР СЛЕДУЮЩЕГО ВЫЗОВА
TEKUSH_VIZ      DW      0       ;ТЕКУЩИЙ ВЫЗОВ

                ; --- версия СОРМ для ФСБ
BUF_PO          LABEL BYTE
                DB      'ALS SORM, version 6.01'
                DB      50 DUP(20h)

                ; --- формирование тестовых сообщений
FG_TEST         DB      2 DUP(0) ; для формирования ответа на запрос теста
BUF_TEST1       DB      0BH,0,29H,3,1,0,1,0,0FFH,0FFH,9,01,01
BUF_TEST2       DB      15,0,0CCH,1,53H,3,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,1,1,1
KPD1_2          DW      101H            ;ТЕСТОВЫЙ КОД

                ; адреса переходов по приёму команд
KOMANDI         DW      17      DUP(0)  ;ПРИНЯТИЯ КОМАНД
                ; адреса переходов по отправке ответов на команды
ADR_SORM_C      DW      17      DUP(0)  ;АДРЕСА ВЫДАЧИ КОМАНД

                ; ????????????????????????????????????????
FG_INIC         DW      0
TIME_PEREZ      DW      0       ; cickl.inc -> mtp_go

                ; форматы СОРМ
PREAMB_NOM_SORM DW      01CCH
DLIN_ZAGOLOVKA  EQU     10              ; LEV_L4 - смещение указателя на параметры СОРМ
                ; количество параметров в байтах у команд СОРМ
                ;           1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17
COM_LEN         DB      0,  0,  0,  6,  4,  19, 16, 6,  7,  3,  16, 4,  11, 0,  1,  5,  2,  0

; *********** КОНСТАНТЫ ****************************
                ; коды команд
COM_ZAP         EQU     0001B    ; 1 - запуск
COM_OST         EQU     0010B    ; 2 - остановка
COM_PAR         EQU     0011B    ; 3 -
COM_ZAK         EQU     0100B    ; 4 -
COM_POS         EQU     0101B    ; 5 -
COM_SNY         EQU     0110B    ; 6 -
COM_PDK         EQU     0111B    ; 7 -
COM_OSV         EQU     1000B    ; 8 - освобождение
COM_ISK         EQU     1001B    ; 9 -
COM_ZOB         EQU     1010B    ;10 -
COM_ZKS         EQU     1011B    ;11 -
COM_ZDV         EQU     1100B    ;12 -
COM_PRE         EQU     1101B    ;13 -
COM_TES_        EQU     1110B    ;14 -
COM_IZM         EQU     1111B    ;15 -
COM_ZUS         EQU    10000B    ;16 -
COM_ZPO         EQU    10001B    ;17 -
                ; коды ответов на команды
COM_AVA         EQU     21H
COM_PER         EQU     22H
COM_OBJ         EQU     23H
COM_SOO         EQU     24H
COM_DVO         EQU     25H
COM_NES         EQU     26H
COM_POD         EQU     27H
COM_VIP         EQU     28H
COM_TES         EQU     29H
COM_DSO         EQU     2AH
COM_VPO         EQU     2BH
                ; коды статистических сообщений
SO_NOM          EQU     41H
SO_OTV          EQU     42H
SO_RAZ          EQU     43H
SO_DVO          EQU     44H
SO_POD          EQU     51H
SO_OSV          EQU     52H
SO_TES          EQU     53H

                ; категория контроля
SOVMEST         EQU     1               ;СОВМЕЩЁННОЕ НАБЛЮДЕНИЕ
RAZDEL          EQU     11H             ;РАЗДЕЛЬНОЕ НАБЛЮДЕНИЕ
STATIC          EQU     2               ;СТАТИЧЕСКОЕ НАБЛЮДЕНИЕ
                ; признаки приёма / выполнения команды
IND_GUD         EQU     0
IND_BAD         EQU     1
                ; код освобождения КСЛ
OSV_KOM8        EQU     1
OSV_PRI         EQU     2
OSV_KOM7        EQU     3
OSV_BAD         EQU     4
OSV_KOM6        EQU     5
                ; параметры связи
UNDEFINED_SV    EQU     10000b
ISXOD_VIZ       EQU     10001B
VXOD_VIZ        EQU     10010B
; --- NOT USED!!!
MESHDUN         EQU     10100B  ; 100100B \ всё автоматика,
MESHGOR         EQU     11000B  ; 101000B / а не полу !
; ---
VNUT_SV         EQU     11100B
                ; признак номера телефона
PRIZN_STAN      EQU     1
PRIZN_SETI      EQU     2
PRIZN_ZONI      EQU     3
PRIZN_MESHGOR   EQU     4
PRIZN_MESHDUN   EQU     5
PRIZN_SLUSHB    EQU     6
PRIZN_UNDEF	EQU	0ffh
                ; тип объекта наблюдения
AB_STANC        EQU     1               ;АБОНЕНТ СТАНЦИИ
AB_SET          EQU     2               ;АБОНЕНТ СЕТИ
NEPOLN_NOM      EQU     12H             ;НЕПОЛНЫЙ НОМЕР
BUNCH           EQU     3               ;ТИП - ПУЧЕК ЛИНИЙ
                ; метка приоритета
PRIOR_HI        EQU     1
PRIOR_LO        EQU     2
                ; код завершения соединения
PRI_TEX         EQU     1               ; по техническим причинам
PRI_NENAB       EQU     2               ; неполный набор (не для АМТС)
PRI_ZAN         EQU     3               ; вызываемый занят
PRI_NEOT        EQU     4               ; вызываемый не отвечает
PRI_RAZ         EQU     5               ; нормальное разъединение (после разговора)
                ; код неподключения КСЛ
NEPODKL_OK      EQU     0
NEPODKL_ZAN     EQU     1
NEPODKL_STAT    EQU     0ffh
NEPODKL_MO      EQU     2               ; моя придумка - не отправил МО120
NEPODKL_MODEM   EQU     3               ; моя придумка - не свободных тайм-слотов на модеме, хотя КСЛ есть
                ; код фазы ДВО
PHASE_CALL      EQU     0
PHASE_SET_DVO   EQU     1
PHASE_TEST_DVO  EQU     2
PHASE_RESET_DVO EQU     3
PHASE_USE_DVO   EQU     4

                ; коды ДВО МО
MO_HT           EQU     35h             ; горячий телефон
MO_OA           EQU     37h             ; однократный будильник
MO_MA           EQU     38h             ; многократный будильник
;MO_CFU          EQU     ??h             ; вместо команды 7b идёт команда b6
MO_CFB          EQU     62h             ; п/а при занятости
MO_CFNRY        EQU     63h             ; п/а при неответе
MO_HOLE         EQU     64h             ; завершение удержание вызова
MO_HOLD         EQU     65h             ; удержание вызова
MO_CC           EQU     66h             ; наведение справки во время разговора
MO_3PTY         EQU     67h             ; трёхсторонняя конференнция
MO_CT           EQU     68h             ; передача вызова
MO_CW           EQU     69h             ; уведомление о поступлении нового вызова
; ---------------------------------------------------------------------------------

                ; коды ДВО СОРМ
CF              EQU     20h             ; просто п/а
CFU             EQU     21h             ; безусловная п/а
CFB             EQU     29h             ; п/а при занятости
CFNRY           EQU     2ah             ; п/а при неответе
CT              EQU     31h             ; передача вызова
CC              EQU     33h             ; наведение справки во время разговора
CW              EQU     41h             ; уведомление о поступлении нового вызова
HOLD            EQU     42h             ; удержание вызова
HOLE            EQU     40h             ; завершение удержание вызова
_3PTY           EQU     52h             ; трёхсторонняя конференнция
                ; --- новые нестандартные коды ДВО
HT              EQU     MO_HT           ; горячий телефон
OA              EQU     MO_OA           ; однократный будильник
MA              EQU     MO_MA           ; многократный будильник

; --- флаги ДВО для конфигурации - бит 0 => какое-то ДВО есть
CFU_CFG         EQU     0feh            ; CFU
CFB_CFG         EQU     0fdh            ; CFB
CFNRY_CFG       EQU     0fbh            ; CFNRY
HT_CFG          EQU     0f7h            ; горячий телефон
OA_CFG          EQU     0efh            ; однократный будильник
MA_CFG          EQU     0dfh            ; многократный будильник

; --- объединённые коды ДВО для СОРМ (ст. байт) и конфигурации (мл. байт)
_CFU_           EQU     CFU   * 256 + CFU_CFG  ; CFU
_CFB_           EQU     CFB   * 256 + CFB_CFG  ; CFB
_CFNRY_         EQU     CFNRY * 256 + CFNRY_CFG; CFNRY
_HT_            EQU     HT    * 256 + HT_CFG   ; горячий телефон
_OA_            EQU     OA    * 256 + OA_CFG   ; однократный будильник
_MA_            EQU     MA    * 256 + MA_CFG   ; многократный будильник


IF _MSP EQ TRUE
BUF_R_BABA      DB      224,0,240,0,225,0,241,0,226,0,242,0,227,0,243,0,228,0,244,0,229,0
                DB      245,0,230,0,246,0,231,0,247,0,232,0,248,0,233,0,249,0
                DB      234,0,250,0,235,0,251,0,236,0,252,0,237,0,253,0
ENDIF
ADD_4_KAN       EQU     16
ADD_31_KAN      DW      124             ;<- {Paul.N - 17/10/17 - add}
