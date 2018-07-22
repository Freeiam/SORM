_Cfg_Start      LABEL BYTE
    ;**************************************
    ; число тысячников в АТС
KL_TIS          EQU     64
    ; начало сохраняемой конфигурации на флеш


    ; 16 * количество тысчяников, каждый байт соответствует номеру модема
TIS_ROAD        DB      16 * KL_TIS DUP(0FFh)

    ; длина номера в станции
LENGTH_NUM      DB      6, 0

    ; флаги настроек программы
FG_PROGRAM      DW      0

_fg_prg_debug           EQU     _b_00   ; отладка - вывод __ DD xx
_fg_prg_sorm_alarm      EQU     _b_01   ; выдавать аварии СОРМ
_fg_prg_ck0             EQU     _b_02   ; работа с ЦК0
_fg_prg_in_bunch        EQU     _b_03   ; работа с исходящими пучками
_fg_prg_cats            EQU     _b_04   ; комбинированная АТС
_fg_prg_mi              EQU     _b_05   ; отладка входящего МО
_fg_prg_mo              EQU     _b_06   ; отладка исходящего МО
_fg_prg_s1              EQU     _b_07   ; отладка канала 1 СОРМ
_fg_prg_s2              EQU     _b_08   ; отладка канала 2 СОРМ
_fg_prg_multy_pref      EQU     _b_09   ; работа с мультипрефиксной конфигурацией
_fg_prg_rel_no_aon      EQU     _b_10   ; отбой соединений без АОН
_fg_prg_bl_new          EQU     _b_11   ; новый квитированный чёрный список
_fg_prg_bl_out          EQU     _b_12   ; исходящий чёрный список
_fg_prg_bl_in           EQU     _b_13   ; входящий чёрный список
_fg_prg_check_sorm      EQU     _b_14   ; проверка зависания каналов СОРМ
_fg_prg_log_sorm        EQU     _b_15   ; логирование СОРМ

; --- храним описаня пучков
;
Tranks          Trank       _MAX_TRANK DUP(<>)
; --- префиксы выхода на спецнаправления:  МГ, МН и СС
;;;IF _MSP EQ TRUE
Prefixes        DW      _PREF_MAX DUP(0ffffh)
; --- список номеров спецслужб расположенных на АТС
SS_Nums         DW      _SS_MAX   DUP(0ffffh)
; --- для сохранения необходимых флагов выборочного отслеживания команд МО
FG_Curr_MO_Conf DW              0                       ; флаги окна выбора команд МО
; --- флаги выбора команд МО ---
_spc_press         EQU   0000000000000001b              ; нажат пробел
_page_down         EQU   0000000000000010b              ; нажат page_down
_page_up           EQU   0000000000000100b              ; нажат page_up
_arrows            EQU   0000000000001000b              ; нажата какая-то стрелка: , , , 
_235_btn           EQU   0000000000010000b              ; нажат выбор 235-ой
_235_cmd           EQU   0000000000100000b              ; работаем по 235-ой
_find_cmd          EQU   0000000001000000b              ; для вывода помеченных команд
_find_numb         EQU   0000000010000000b              ; нашли отмеченную команду
_clear_MO          EQU   0000000100000000b              ; убрать все команды с контроля данного типа
_set_all_MO        EQU   0000001000000000b              ; поставить все команды на контроль
_reset_all_MO      EQU   0000010000000000b              ; убрать все команды с контроля
_set_sorm_MO       EQU   0000100000000000b              ; поставить команды СОРМ на контроль
_set_black_MO      EQU   0001000000000000b              ; поставить команды "черного" списка на контроль
_set_tarif_MO      EQU   0010000000000000b              ; поставить тарификационные команды на контроль
IF _MSP NE TRUE
    ; переменные для преобразования типов разговоров МО в типы СИДС
                ; --- все префиксы от 1-й до 3-х цифр
__PREFIX_MG       DW      080h            ; префикс выхода на АМТС
__PREFIX_ZN       DW      082h            ; внутризоновый МГ префикс
__PREFIX_MN       DW      00081h          ; префикс для выхода на МН
__PREFIX_ATS      DW      02081h          ; собственный префикс АТС
                ; --- маски для выделения префиксов
__PREFIX_MG_MASK  DW      0f0h            ; маска для выделения префикса МГ из № Б
__PREFIX_ZN_MASK  DW      00fffh          ; маска для выделения внутризонового МГ префикса из № Б
__PREFIX_MN_MASK  DW      00fffh          ; маска для выделения префикса МН из № Б
__PREFIX_ATS_MASK DW      00fffh          ; маска для выделения собственного префикса из № А

__PREFIX_SS       DB      000h            ; префикс спецслужбы
__PREFIX_SS_MASK  DB      0f0h            ; и его маска
_Cfg_End	= $ - _Cfg_Start
; добивка
                   DB      199 DUP(0)

;;;KONF_TAIL       DB      _FILE_CFG_SIZE - _Cfg_End DUP(0cch) - не использовалась--{Paul}--
ELSE
; добивка
                   DB      217 DUP(0)
    ; Конец сохраняемой конфигурации
    ;**************************************
END_KONF_FLASH     DW      0                       ; метка конца конфигурации для записи на FLASH
ENDIF
; --- для подсчета количества секторов конфигурации ---
LCO                DB      0                    ; не используется

                ; --- ЦТО
CHAN_TO_CTO     DW      15      ;Канал ОЗУ БАБА передачи на ЦТО
CHAN_FROM_CTO   DW      14      ;Канал ОЗУ БАБА приема от ЦТО
IF _MSP EQ TRUE
FiltrRM         DB      040h    ; Watch Dog для старого железа + снятие "ОШ1" <-{Paul.N - 21/04/14}
ENDIF
                ; старая конфигурация - для расширения № абонента из префикса
;        LENGTH_NUM      0   1   2   3   4  5           6           7
ROL_PREF_TIS    DB      32, 28, 24, 20, 16, 12,         8,          4
M_OR_PREF_TIS   DD      -1, -1, -1, -1, -1, 0FFFFFFF0H, 0FFFFFF00H, 0FFFFF000H
IF _MSP EQ TRUE
T_F_D           DB      55h
PERV_SEG_KONF   EQU     14H

TIP_SL          EQU     1
TIP_AB          EQU     2
TIP_AB_UP       EQU     3

TAB_TIP_LIN     DB      0, 2, 2, 2, 4, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 4, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0 ;
TAB_TIP_TF_AB   DB      0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 2, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0 ;
TAB_TIP_TF_SL   DB      0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1 ;
ENDIF
                ;       БАЛ   СЛ   И30  ТС ЦК И15  PRI  ОКС  ПУ   УИ   РЕЗЕРВ
TAB_DL_BL       DW      200h, 80h, 80h, 0, 0, 80h, 80h, 80h, 80h, 80h, 6 DUP(0)
TAB_TIP_BL      DB      2,    1,   1,   0, 0, 1,   1,   1,   1,   1,   6 DUP(0)
TAB_TIP_LN      DB      6,    4,   4,   0, 0, 4,   4,   4,   4,   4,   6 DUP(0)

KL_LEV          EQU     5               ; глубина сетитысячников
IF _MSP EQ TRUE
SCH_FAPCH       DW      0

LAST_ADR        DW      OFFSET END_KONF_FLASH           ;ПОСЛЕДНИЙ АДРЕС конфигурации
NOM_CONF        DB      0


DLIN_KONFIG     DD      11H
ENDIF
DLIN_KONF       EQU     0150000H
SMESH_KONF      EQU     0A000H

; СОРМ
FG_SOST         DB      0               ;ФЛАГ СОСТОЯНИЯ
FG_SOST_STOP    DB      0               ; <Yu> - флаг остановки СОРМ
IF _MSP EQ TRUE
FG_SORM         DW      100000B         ;ФЛАГ СОРМ
                        ;0-БУФЕР ПЕРЕПОЛНЕН
                        ;1-ОЖИДАНИЕ КОНФИГУРАЦИИ
                        ;2-ФЛАГ НАЧАЛА ЦИФРЫ ПРИ ДЕЛЕНИИ
                        ;3-ФЛАГ ПРИ УМНОЖЕНИИ ПРЕФЕКСА
                        ;4-ФЛАГ НЕ ЗАНЯТИЯ ЛИНИИ
                        ;5-ФЛАГ ВЫДАЧИ КОНФ.ТЫСЯЧНИКУ
                        ;6-ФЛАГ ОШИБКИ ПО СООТВЕТСТВИЮ OBJ
                        ;7-ФЛАГ ОСВОБОЖДЕНИЯ КСЛ
                        ;8-ФЛАГ СНЯТИЯ КОММУТАЦИИ
                        ;12-ФЛАГ ПЕРЕДАЧИ СЛЕДУЮЩЕГО ПАКЕТА НА ТАРИФИКАТОР
                        ;13-УДАЛЕННАЯ СТОРОНА ГОТОВА ПРИНЯТЬ ПОКЕТ
                        ;14-ПОВТОР ВЫДАЧИ ПОКЕТА
                        ;15-ПЛОХОЙ ПАКЕТ ДАННЫЕ АБОНЕНТА

FG_SORM2        DW      0
                        ;0-ППД
                        ;1-ПРОВЕРКА ТЕКУЩАЯ
                        ;2-ЗАПОЛНЕН БУФЕР ДВО
                        ;3-ФЛАГ ДЛЯ ТАРИФИКАЦИИ - не получилась запись в SEG_TF
                        ;4-ВЫДАТЬ КОНФИГУРАЦИЮ НА ТАРИФИКАЦИЮ
                        ;5-НЕ ЧЕТНЫЙ ТЕЛЕФОН
ENDIF
FG_SORM3        DW      0       ;0-ПЕРЕЗАПУСК ПЕРЕДАН
				;1-ЗАВОРОТ
; --- <Paul> --- {13/02/2013}
; --> дополнительные(служебные) флаги СОРМ
FG_SORM4        DB 0
  _3_pty_sorm   EQU     00000001b      ; для фиксации 3PTY в соединении B ->C(сессия B(*)->C)
; --- Структура для работы с сессиями СОРМ - 3PTY
; -- однозначно идентифицирует сессиию СОРМ, в которой необходима перекоммутация <Recomm_3_PTY>
Sorm_3PTY       STRUC
  session_ptr         DW       0FFFFH        ;  ук-ль на сессию
  perem_ebx_3pty      DD       0FFFFFFFFH    ;  ук-ль на 2-ое соединение конференции
  sorm_numb_3pty      DD       0FFFFFFFFH    ;  № абонента на контроле, выполнившего конференцию
  perem_tms           DB       0FFH          ;  номер таймслота на модеме во 2-м соед. для перекомм.
  soed_ptr            DD       0FFFFFFFFh    ;  ук-ль во 2-м soed., где хранится ссылка на сессию.
Sorm_3PTY       ENDS
; --- следующий свободный слот
_sorm_3pty_next       DW      0
; --- размер структуры
_sorm_3pty_size = size Sorm_3PTY
; -- кол-во структур
_sorm_3pty_max  = 100
; -- конец структур
_sorm_3pty_last = _sorm_3pty_size * _sorm_3pty_max
; -- создадим 100 структур
Sess_3PTY       Sorm_3PTY      _sorm_3pty_max DUP(<>)
; --- переменная для хранения номера таймслота.
_tmslot         DB      0
; --- <Paul> --- {END}
SLED_TF         DD      0
POS_TF          DD      0
M_TF            EQU     1FFFH
IF _MSP EQ TRUE
Dlina_Packet    DB      0
Ptr_edi         DD      0

SCH_AVA1        DW      0               ;КОЛ-ВО ПРОПУЩЕННЫХ ПАКЕТОВ
DL_OKNA         EQU     64
NOM_POKETA      DB      0
OLD_PODTVER     DB      7FH
NOM_VIDAN       DB      0
M_NOM_POK       EQU     7FH
SCH_ERR_TF      DW      0
OLD_TIM_TIME    DD      0
POROG_5MIN      EQU     300000
tek_vvod_tf     db      7FH
                ;                                       FEDCBA9876543210
;0 - TIME - NET_PODTVERSHDENIY  ─────────────────────────┼─┼┼┼┼┼┼┼┼┼┼┼┼┘
;1 - TIME - ЗАДЕРЖКА НА СЛЕДУЮЩИЙ ПАКЕТ  ────────────────┼─┼┼┼┼┼┼┼┼┼┼┼┘
;2 - INIC_TF - ТАКЖЕ ЗАПРЕЩАЕТ ИНКРЕМЕНТИРОВАТЬ ПОКЕТ  ──┼─┼┼┼┼┼┼┼┼┼┼┘
;3 - ДЛЯ СОМ ПОРТА  ─────────────────────────────────────┼─┼┼┼┼┼┼┼┼┼┘
;4 - ТАЙМЕР НА ПЕРЕЗАПУСК   ─────────────────────────────┼─┼┼┼┼┼┼┼┼┘
;5 - НЕТ СВЯЗИ С АПУС - ПЕРЕГРУЗКА ──────────────────────┼─┼┼┼┼┼┼┼┘
;6 - ИЗМЕНЕНИЕ NOM_POKETA  ──────────────────────────────┼─┼┼┼┼┼┼┘
;7 - НЕТ СВЯЗИ С АПУС  ──────────────────────────────────┼─┼┼┼┼┼┘
;8 - ВЫДАТЬ СТОРОНА ПЕРЕГРУЖЕНА   ───────────────────────┼─┼┼┼┼┘
;9 - ВЫДАЕТСЯ КОНФИГУРАЦИЯ    ───────────────────────────┼─┼┼┼┘
;10 - ГОТОВ ВЫДАТЬ СТО ──────────────────────────────────┼─┼┼┘
;11 - ДЛЯ НУЖД ──────────────────────────────────────────┼─┼┘
;12 - ЗАПИСЬ ПОКЕТА С ПОДТВЕРЖДЕНИЕМ ────────────────────┼─┘
;14 - ТАЙМЕР ДЛИТЕЛЬНОГО ПОДТВЕРЖДЕНИЯ ──────────────────┘

; --- параметры -=> СОМ1 <=- порта для АПУСа !!!
N_PORT          DW      3F8H    ; PortCOM1
DIV_SPEED       DB      1       ; делитель частоты
COM_TX          DW      3F8H    ; Адрес СОМ-порта
COM_TX2         DW      3FAh    ; Адрес СОМ-порта + 2
COM_TX5         DW      3FDh    ; Адрес СОМ-порта + 5
COM_TX6         DW      3FEh    ; Адрес СОМ-порта + 6

; тарификация
PREDEL_TF       EQU     1000H
OLD_TIME_TF0    DW      0
OLD_TIME_TF1    DW      0
OLD_TIME_TF4    DW      0
POROG_TF        EQU     1000
POROG_TF1       EQU     100
POROG_TF4       EQU     5000
NACH_TF_POKETA  DD      0
END_TF_POKETA   DW      0

SCH_PEREZAP     DW      0
SCH_INI_TF      DB      0
INI_PAKET       DB      40 DUP(0)
NOM_INI_P       DB      254,7FH
                DB      50 DUP(0)
DL_INI_PAK      EQU     50
KONFIG          EQU     128
SCH_TIME        DW      0

; ************************************ ПЕРЕМЕННЫЕ ИСПОЛЬЗУЕМЫЕ ДЛЯ РАБОТЫ С ЦТО
SCREEN_SEG      DW      0B800h  ;Сегмент видеообласти
POINT_SS        DW      0       ;Указатель записи в ОЗУ БАБА
ENDIF
FG_TF           DW                                      0000000000000000b
INT_5           DB      0       ;Флаг клавиатурного прерывания
IF _MSP EQ TRUE
KLAV            DB      0       ;Скан-код нажатой клавиши
ENDIF
ALA_MO          DB      0       ;Признак аварии МО при NO_MOD = 0 ( ФАЗА 0 )
                DB      2       ;Признак аварии МО при NO_MOD = 1 ( ФАЗА 1 )
                DB      2       ;Признак аварии МО при NO_MOD = 2 ( ФАЗА 2 )
                DB      2       ;Признак аварии МО при NO_MOD = 3 ( ФАЗА 3 )
                DB      2       ;Признак аварии МО при NO_MOD = 4 ( ФАЗА 4 )
                DB      2       ;Признак аварии МО при NO_MOD = 5 ( ФАЗА 5 )
                DB      2       ;Признак аварии МО при NO_MOD = 6 ( ФАЗА 6 )
FG_TIS          DB      KL_TIS DUP(0)
                                        ;0-ЗАПРОШЕНА DATA КОНФИГУРАЦИИ
                                        ;1-ПОШЛА КОНФИГУРАЦИЯ
                                        ;2-КОНФИГУРАЦИЯ ПОЛУЧЕНА
                                        ;3-НЕТА ДАТА (НАДО ЗАПРОСИТЬ КОНФИГ.)
                                        ;4-ЗАПРОШЕНА КОНФИГУРАЦИЯ
                                        ;5-DATA ПЕРЕДАНА
                                        ;6-ЕСТЬ СВЯЗЬ С TIS
                                        ;7-ВЫДАНЫ ПАРОЛИ


IF _MSP EQ TRUE
FG_SEG          DB      256 DUP(0FFH)
                                        ;0-0-ZANYT/1-СВОБОДЕН
                ;ПЕРЕМЕННЫЕ FLASH
SMESH_DATA_T    EQU     4000H
SMESH_TIS_PREF  EQU     4400H
SMESH_PAROLI    EQU     5000H
SMESH_PRIOR     EQU     SMESH_PAROLI+100H
SMESH_FL_PAR    EQU     SMESH_PRIOR+100H
SMESH_PAR_BL    EQU     6000H
ADR_KONF_F      EQU     160000H         ;АДРЕС КОНФ.НА FLASH
MIN_DLIN_BLOKA  EQU     80H*16
FG_FL           DW      0
FL_STEP         DW      0
ETIC_FL         DW      0EFBDH
SLED_SEG        DW      PERV_SEG_KONF
FL_ESI          DD      0
FL_DS           DW      0
FL_ECX          DD      0
FL_EDI          DD      0
OLD_FL_ESI      DD      0
OLD_FL_DS       DW      0
OLD_FL_ECX      DD      0
OLD_FL_EDI      DD      0
FL_SEG          DB      0
ERR_FLASH       DB      0
ERASE_SEG       DB      0
SLED_FL_STEP    DB      0
KL_SEG_KONF     EQU     26
ENDIF
;**************
FG_PROG         DB      1               ; флаг железа
                                        ; 0 - старое
                                        ; 1 - новое
                                        ; 3 - моноблок
VERSIY          DB      0
HardVers_hi     DB      0
HardVers_lo     DB      0


;***** Переменные контроля подсчета ошибок ( Е-3 Е-5 )*************
FEC_Ptr         DB      4 DUP(0); указатель в накопители FEC
FEC1            DB    512 DUP(0); накопители считанных значений FEC
FEC_Sum         DW      4 DUP(0); накопленные суммы FEC

;**************

; --- переменные для индикации черного списка
BL_CurrNum_In   DD      0       ;
BL_Count_In     DW      0       ;
BL_CurrNum_Out  DD      0       ;
BL_Count_Out    DW      0       ;
IF _MSP EQ TRUE
; --- переменная для игнорирования приема времени с Тысячника
NO_TIME_OF_CK   DB      0       ; 0 - =1 - не принимать время с ЦК
                                ; 1 - передавать время на ЦК

; ---------------------- -= * ПУМА * =- -------------------------------
N_GEN_CTO_R	DW      14      ;Канал ОЗУ БАБА приема от ЦТО
N_GEN_CTO_T	DW      15      ;Канал ОЗУ БАБА передачи на ЦТО

        ;!!!! это было в hard.var может надо в другой сегмент
ST_VOICE        DD      10000h                  ;Начальный адрес блока
                                                ;голосовых сообщений
ST_PROGRAM      DD      300000h                 ;Начальный адрес основной
                                                ;программы
ST_CONFIG       DD      200000h                 ;Начальный адрес конфигурации
LEN_CONF        DD      200                     ;Длина блока конфигурации
CURR_PROG       DB      0

;;; kill it !!!!
NOM_BLOKA_T     DD      0
ELSE
CURRENT_TIME	DD	0			;Текущее время (в 2-х мс

; --- WatchDog
WD_Time         DD      0
_WD_TOUT        =       1000
WD_Value        DB      0
ADR_WD          =       4409h
WD_Reboot       DB      0
Reboot_Time     DB      0

                ; --- буфер для отправки ПУМовских  сообщений
buff_PUMA       DB      _PUMA_MESS_LEN DUP(0)

;;;HDD!!!
                ; --- файлы, используемые программой, и их параметры
_FILE_PRG_SIZE   =      512*1024        ;;;!!!
_FILE_CFG_SIZE   =      512 * 12         ;;;!!!
_FILE_RBF_SIZE   =      0D100Ch
_FILE_BLST_SIZE  =      80000h          ;;;!!!
_FILE_ANI_SIZE   =      20a000h         ;;;!!!
_FILE_BUNCH_SIZE =      1053276


_FILE_CFG_       DB     'tf_sorm.cfg', 5 dup(0)
_FILE_PRG_NAME   DB     'PROG.EXE', 5 dup(0)
_FILE_PRG_NAME_T DB     'PROG.TMP', 5 dup(0)
_FILE_PRG_NAME_C DB     'PROG~.EXE', 5 dup(0)
_FILE_CFG_NAME   DB     'TF_SORM.CFG', 5 dup(0)
_FILE_CFG_NAME_T DB     'TF_SORM.TMP', 5 dup(0)
_FILE_CFG_NAME_C DB     'TF_SORM~.CFG', 5 dup(0)
_FILE_BLST_NAME  DB     'TF_SORM.BLS', 5 dup(0)
_FILE_BLST_NAME_T DB     'TF_SORM.TMP', 5 dup(0)
_FILE_BLST_NAME_C DB     'TF_SORM~.BLS', 5 dup(0)
_FILE_ANI_NAME   DB     'TF_SORM.ANI', 5 dup(0)
_FILE_BUNCH_NAME DB     'TF_SORM.BNC', 5 dup(0)
_FILE_RBF_NAME   DB     'CYCL_PCM.RBF', 5 dup(0)
_FILE_RBF_NAME_T DB     'CYCL_PCM.TMP', 5 dup(0)
_FILE_RBF_NAME_C DB     'CYCL_PC~.RBF', 5 dup(0)



; Идентификаторы режимов конфигурации
_NOP_ID	        =	0FFh
_PROGR_ID	=	000h
_CONFG_ID       =	001h
_DOPIN_ID       =	002h
_ANI_ID         =       003h
_BUNCH_ID       =       004h

_FILE_PRG_ID    DD      _PROGR_ID
_FILE_GFG_ID    DD      _CONFG_ID
_FILE_BLST_ID   DD      _DOPIN_ID
_FILE_ANI_ID    DD      _ANI_ID
_FILE_BUNCH_ID  DD      _BUNCH_ID
ENDIF

__dd_num	db	18   ; MVT изменил немного отладочные переменные
_1st_dd   label byte
_1st_dword   label dword
__dd1            DD      0    ; \
__dd2            DD      0    ; |
__dd3            DD      0    ; |
__dd4            DD      0    ; |   отладочные переменные
__dd5            DD      0    ; |
__dd6            DD      0    ; |
__dd7            DD      0    ; |
__dd8            DD      0    ; |
__dd9            DD      0    ; |
__dd10           DD      0    ; /
_2nd_dd   label byte
__dd11           DD      0    ; \
__dd12           DD      0    ; |
__dd13           DD      0    ; |
__dd14           DD      0    ; |   отладочные переменные
__dd15           DD      0    ; |
_16n_dd   label byte
__dd16           DD      0    ; |
__dd17           DD      0    ; |
__dd18           DD      0    ; |
__dd19           DD      0    ; |
__dd20           DD      0    ; /
_3rd_dd   label byte
__dd21           DD      0    ; \
__dd22           DD      0    ; |
__dd23           DD      0    ; |
__dd24           DD      0    ; |   отладочные переменные
__dd25           DD      0    ; |
__dd26           DD      0    ; |
__dd27           DD      0    ; |
__dd28           DD      0    ; |
__dd29           DD      0    ; |
__dd30           DD      0    ; /
_4th_dd   label byte
__dd31           DD      0    ; \
__dd32           DD      0    ; |
__dd33           DD      0    ; |
__dd34           DD      0    ; |   отладочные переменные
__dd35           DD      0    ; |
__dd36           DD      0    ; |
__dd37           DD      0    ; |
__dd38           DD      0    ; |
__dd39           DD      0    ; |
__dd40           DD      0    ; /
_5th_dd   label byte
__dd41           DD      0    ; \
__dd42           DD      0    ; |
__dd43           DD      0    ; |
__dd44           DD      0    ; |   отладочные переменные
__dd45           DD      0    ; |
__dd46           DD      0    ; |
__dd47           DD      0    ; |
__dd48           DD      0    ; |
__dd49           DD      0    ; |
__dd50           DD      0    ; /

; MVT переменные для работы с файлом отладочной информации
if _MSP NE TRUE
DebugFile db 'DEBUG.TXT',0
DebugFileDescriptor	dd ?
DebugFileOpened	db 0  ; 0 - закрыт, 1 - открыт, 2 - идёт процесс открытия или закрытия, проверка размера отключена
DebugFileDDValue db 8 dup (?)
DebugFileHeader	db	' Debug started '
cr_lf	db 13,10
space_symb	db ' '
DebugArray db 0aah, 0bbh, 0cch
DebugFilePoint db '.'
DebugFileBottom db 13,10,'Debug stopped'
DebugFileHexBuffer	db 1024 dup(?)
ici db 'ici ',13,10 ; Строка для трассировки программы в файл макросом @String_to_file
if _isDebugBuffered
DebugFileBuffer db _DebugBufferSize dup (?)
DebugFileBufferPos dd 0
endif
BreakDebugFile db 13,10,'==out of max size==',13,10
BreakDebugFileLen db 23
DebugFileSizeInfoCol db 20
DebugFileSizeInfoRow db 33
NoDebugFileSize db 'OVERFLOW'
DebugFileSizeColor	EQU clYELLOW * 10h + clRED
CurrentDebugFileSize	dd	?
isDebugFileOverflow	db	?   ; 0 - нет переполнения лог файла, 1 - есть


string1	db 'asdf'
string1offset dd offset string1
string2 db 'asdf'
string2offset dd offset string2

if _debug_Puma
puma_RTP db 'RTP ',13,10
puma_RTW db 'RTW ',13,10
puma_INFO db 'INFO',13,10
puma_NULL db 'NULL',13,10
puma_unknown db '??? ',13,10
puma_RTP_offset dd offset puma_RTP
puma_RTW_offset dd offset puma_RTW
puma_INFO_offset dd offset puma_INFO
puma_NULL_offset dd offset puma_NULL
puma_unknown_offset dd offset puma_unknown

SendString db 'SEND>'
SendStringOffset dd offset SendString
ReceiveString db 'RECEIVE>'
ReceiveStringOffset dd offset ReceiveString

rtp_array db 98h,0E6h,0FFh,01h,00h,3Eh
rtp_array_offset dd offset rtp_array
rtw_array db 98h,0E6h,0FFh,01h,05h,25h
rtw_array_offset dd offset rtw_array
null_array db 98h,0E6h,0FFh,00h,0D7h
null_array_offset dd offset null_array
endif
endif
; /MVT

 ; ---- Black List DD's -----
_black_dd       label dword
__bl_dd1        DD      0
__bl_dd2        DD      0
__bl_dd3        DD      0
__bl_dd4        DD      0
__bl_dd5        DD      0
__bl_dd6        DD      0
__bl_dd7        DD      0
__bl_dd8        DD      0
__bl_dd9        DD      0
__bl_dd10       DD      0
__bl_dd11       DD      0
; --- Black List Debuger -------------------
_black_active           DB      0           ; ---> состояние отладчика: 0 - не актив., 255 - актив.
_numb_dds_bl            DB      0           ; счетчик прохода по исх. ч.с.
_numb_dds_bl_prev       DB      0   ; предыдущий проход по исх. ч.с.
_numb_dds_bl_in         DB      0           ; счетчик прохода по вх. ч.с.
_prev_dds_bl_in         DB      0   ; предыдущий проход по вх. ч.с.
_differ_black           DB      0   ; перем. для определения правила исх. ч.с.
; ---- квитированный ч.с.(quit)
_numb_dds_bl_q          DB      0           ; счетчик прохода по исх. ч.с.(quit)
_numb_dds_bl_prev_q     DB      0   ; предыдущий проход по исх. ч.с.(quit)
_numb_dds_bl_in_q       DB      0           ; счетчик прохода по вх. ч.с.(quit)
_prev_dds_bl_in_q       DB      0   ; предыдущий проход по вх. ч.с.(quit)
_black_differ_q         DB      0   ; перем. для определения правила исх. ч.с.(quit)
_differ_black_q         DB      0   ; перем. для определения корректности длины префикса №А(quit)
BL_dbg_q                DB      0   ; квитированный ч.с.
; -----------------------------------------------------------------
;;;_Buff_Buff       DB      512 DUP(0) --- not used
; --- {Paul.N} --- [31/05/13]
; -- буфер под хранение ссылок на сессии по №С при переадресации
Buff_Sess_RA            DW     _Sess_RA_MAX DUP(0FFFFh)

