INCLUDE _DEFS.ASM
TITLE   SORM
;.386P
PAGE	60,130
;
;╔══════════════════════════════════════════════════════════════════════╗
;║     ==            Программа управления блоком СОРМ             ==    ║ █
;╚══════════════════════════════════════════════════════════════════════╝ █
;  ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
; - - - - - - - - - - - - - S T A R T - P T R - - - - - - - - - - - - - - - - -

CODE_AXS        SEGMENT PARA USE16 PUBLIC "CODE"
ASSUME  DS:DATA_AXS,CS:CODE_AXS
START:
                jmp     StartNew
CODE_AXS        ends
IF _MSP NE TRUE
; - - - - - - - - - - - - - - -  H D D - - - - - - - - - - - - - - - - - - - -
;
HDD_CODE	SEGMENT PARA PUBLIC USE16 'CODE'
                extrn   HDD_SIZE:ABS
                extrn   InitHDD
                extrn   FileOpen
                extrn   FileErase
                extrn   FileEraseName
                extrn   FileRename
                extrn   FileCreate
                extrn   FileCreateSize
                extrn   FileCreateEmpty
                extrn   FileClose
                extrn   FileSize
                extrn   FileSeek
                extrn   FilePos
                extrn   FileRead
                extrn   FileWrite
HDD_CODE	ENDS

; - - - - - - - - - - - - - - -  Ф А П Ч - - - - - - - - - - - - - - - - - - -
;
                INCLUDE pllext.asm
                PUBLIC  SEG_PLL_DATA
                PUBLIC  BASIC0
ENDIF

; - - - - - - - - - - - - - - - S T A C K - - - - - - - - - - - - - - - - - - -
STACK_AXS       SEGMENT PARA USE16 STACK "STACK"
STACK_GO        label   word

Curr_T          DD      0

include lib\exc.dat
IF _MSP EQ TRUE
SEG_DOP         DW      0
SEG_C111        DW      294     DUP(0)          ;НАЧАЛО СЕГМЕНТА КОММУТАТОРА
SEG_FLASH_OLD   DW      0
SEG_SYS         DW      0
SEG_SORM        DW      328     ;
SEG_BUNCH       DW      336     ;
                ; ┌── PUMA ───────────┐
                ; │                   │
SEG_PUMA_DRV    DW      344     ;     │
SEG_PUMA_SCR    DW      352     ;     │
SEG_PUMA        DW      360     ;     │
                ; │                   │
                ; └── PUMA ───────────┘

ELSE
SEG_FL          DW      280     ; HDLC на МСП-М !!!
SEG_SORM        DW      368     ;
SEG_MAX_II      DW      376     ;
                ; ┌── PUMA ───────────┐
                ; │                   │
SEG_PUMA_DRV    DW      384     ;     │
SEG_PUMA_SCR    DW      392     ;     │
SEG_PUMA        DW      400     ;     │
                ; │                   │
                ; └── PUMA ───────────┘
SEG_BUNCH       DW      408     ;
ENDIF
SEG_ULOG        DW      0
SEG_COM         DW      0
SEG_AXD         DW      0



SEG_NLOG        DW      0
IF _MSP EQ TRUE
SEG_VIZ         DW      0
SEG_FLASH       DW      280;;;0D000h  ;Сегмент FLASH
SEG_MARSH_S     DW      0
SEG_C000        DW      0                       ;НАЧАЛО СЕГМЕНТА КОММУТАТОРА
SEG_TER         DW      0
ELSE
SEG_FLASH       DW      0D000h  ;Сегмент FLASH
ENDIF
SEG_BABA        DW      0D800h  ;Сегмент BABA
SEG_ADR_SOED    DW      0
SEG_ZAG         DW      0                       ;НАЧАЛО СЕГМЕНТА КОММУТАТОРА
SEG_PCM         DW      0
SEG_NULL        DW      0
SEG_BUS         DW      0
SEG_SOED        DW      0
SEG_KONF        DW      0
SEG_TF          DW      0
SEG_OBJ         DW      0
SEG_GS8         DW      0
SEG_GS          DW      0D000H
SEG_ST          DW      0C800H          ;НАЧАЛО СЕГМЕНТА КОММУТАТОРА
SEG_FS          DW      0B800H
SEG_AX          DW      0               ;СЕГМЕНТ ДАННЫХ
SEG_BLK_LST     DW      _BLCK_LST1_SEG   ;Текущий сегмент черного списка
IF _MSP EQ TRUE
SEG_ST2         LABEL   WORD
        public  SEG_ST2
SEG_ST_1        DW      32;0D000H          ;НАЧАЛО СЕГМЕНТА КОММУТАТОРА
SEG_VIDEO       LABEL WORD
        PUBLIC  SEG_VIDEO
SEG_SCR                 DW      0;168;0B800H    ;\ \
        public  SEG_SCR                         ;/ /
SEG_PROC                DW      0;120           ;\ \
        public  SEG_PROC                        ;/ /
SEG_DC                  DW      0:WORD;128;SEG_AX /  |-\ | | |\/| /-\
        public  SEG_DC                          ;/ / | | | | |\/| | |
SEG_FL                  DW      0:WORD;280      ;\ \ |-/ | | |  | |-|
SEG_BUF                 DW      0:WORD;192      ; \  |   \-/ |  | | |
        public  SEG_BUF                         ;\ \

NEW_ADR=1                                       ;\ \
ELAN=NEW_ADR                                    ; \
UNI_BL=1                                        ;\ \
ELSE
SEG_ANI_LST     DW      _ANI_LST_SEG    ;Текущий сегмент черного списка
;;;SEG_BUNCH       DW      _ANI_LST_SEG    ;сегмент с описанием пучков ПОКА ИСПОЛЬЗУЮ СЕГМЕНТ АОН !!!!

; - - - - - - - - - - - - - - -  H D D - - - - - - - - - - - - - - - - - - - -
;
                PUBLIC  SEG_HDD_HEAP
SEG_HDD_HEAP    DW      328

; - - - - - - - - - - - - - - -  Ф А П Ч - - - - - - - - - - - - - - - - - - -
;
SEG_PLL_CS      DW      352     ;
SEG_PLL_DATA    DW      360     ;
LABEL	BASIC0


SEG_ALL         DW      _SEG_ALL;
ENDIF
_BLOCK_TF_SORM  = 4                             ;/ /
_TYPE_BLOCK     = _BLOCK_TF_SORM                ; /
        public _TYPE_BLOCK                      ;/ /
        public EXC_1

_SS_DOP_SIZE = 10h - ($-STACK_GO AND 0Fh)       ; расчитаем выравнивание до FFFFF0h
        db      _SS_DOP_SIZE    DUP('S')        ; вставим недостающее
        DB      1000h           DUP('S')        ; здесь СТЕК
Stack_start     label   byte

SS_SIZE = $-STACK_GO
STACK_AXS       ENDS


;   ----------=========******** DATA ********==========---------

DATA_AXS  SEGMENT PARA USE16 PUBLIC 'DATA';Сегмент кофигурации 100-ка
NACH_AXS        LABEL   WORD
                ; --- работа с пучками
                INCLUDE Bunch.DAT                      ; здесь только определения
;;;LEN_FILE        DW      8000H                       ; не используется
                ; подключаем данные
                INCLUDE TF_TDAT.INC

                ; --- визуализация принятых команд МО ---------------------------------
__mo_in_str     DB      '>', 0
__mo_out_str    DB      '<', 0
IF _MSP EQ TRUE
__sorm_2_str    DB      's', 0
__INF_STR_LEN   EQU     40      ; из каждой принятой команды МО выведем столько первых байт
__INF_STR_COUNT EQU     34      ; номер последней строки куда ещё можно выводить очередную команду
ELSE
__sorm_2_str    DB      'm', 0
__INF_STR_LEN   EQU     39      ; из каждой принятой команды МО выведем столько первых байт
__INF_STR_COUNT EQU     34;;; + 15     ; номер последней строки куда ещё можно выводить очередную команду
ENDIF
__sorm_1_in_str  DB     'с', 0
__sorm_1_out_str DB     'a', 0
__sorm_empty_str DB     ' ', 0
; --- debug MO ---
_ss_code        DD      0ABCD0000h       ;
_ss_1           DD      0
_ss_2           DD      0
_ss_3           DD      0
_ss_4           DD      0
; ------  ------

__inf_clear     DB      __INF_STR_LEN DUP('  '), 0 ; строка зачищающая место под новый вывод
__inf_y         DW      0       ; № строки для вывода очередной команды МО
__mo_in_lens    DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 0
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 10
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 20
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 30
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 40
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 50
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 60
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 70
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 80
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 90
                DB      6,  5,  12, 5,  4,  4,  4,  4,  8,  4   ; 100
                DB      6,  4,  9,  5,  5,  4,  4,  4,  16, 4   ; 110
IF _MSP EQ TRUE
                DB      6,  6,  5,  5,  2,  1,  1,  1,  1,  7   ; 120
                DB      1,  1, 22,  2,  2,  2,  1,  1,  1,  1   ; 130
                DB      1, 25,  2,  1,  1,  1,  1,  1,  1,  1   ; 140
                DB      1,  3,  1,  1,  1,  7,  6,  1,  1,  1   ; 150
ELSE
                DB      6,  6,  5,  5,  2,  2,  4,  4,  2,  7   ; 120
                DB      1,  1, 22,  2,  1,  8,  1,  1,  1,  1   ; 130
                DB      1, 25,  1,  1,  1,  1,  1,  1,  1,  1   ; 140
                DB      1,  3,  1,  1,  1,  7,  6,  1,  1,  1   ; 150
ENDIF
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 160
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 170
                DB      1,  1,  5,  1,  1,  1,  1,  1,  1,  1   ; 180
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 190
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 200
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 210
                DB      1,  1,  1,  1,  52, 1,  1,  1,  1,  1   ; 220
IF _MSP EQ TRUE
                DB      1,  1,  1,  1,  1, 27,  1,  1,  1,  1   ; 230
ELSE
                DB      1,  1,  1,  1,  1, 27,  1,  1,  1,  1   ; 230
ENDIF
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 240
                DB      1,  1,  1,  1,  1,  1,  1,  1,  1,  1   ; 250

                ;;;<Yu>
IF _MSP EQ TRUE
PEREM_MO156     DW      0
PEREM_          DW      0
PEREM_1         DW      0
PEREM_2         DW      0
ENDIF
PEREM_AL        DB      0    ;  \
PEREM_AH        DB      0    ;  |
PEREM_AX        DW      0    ;  |
PEREM_EAX       DD      0    ;  |
PEREM_DL        DB      0    ;  |
IF _MSP NE TRUE
PEREM_DH        DB      0    ;  |
ENDIF
PEREM_DX        DW      0    ;  |
PEREM_BX        DW      0    ;  |
PEREM_EBX       DD      0    ;  |
PEREM_BP        DW      0    ;  |
PEREM_EBP       DD      0    ;  |
PEREM_CL        DB      0    ;  |
PEREM_CX        DW      0    ;  |    переменные регистров
PEREM_DI        DW      0    ;  |    для времменого хранения внутри процедур
PEREM_EDI       DD      0    ;  |
PEREM_SI        DW      0    ;  |
PEREM_ESI       DD      0    ;  |
PEREM_SOED_PTR  DD      0    ;  |  хранение ук-ля на сессию в соед.
IF _MSP NE TRUE
PEREM_NLG       DD      0    ;  |
PEREM_NFZ       DD      0    ;  /
ENDIF
ASCII           DB      '0123456789ABCDEF'

;;; kill it !!!
TEMP_VAR        DW      40 dup (0)  ; поле данных для временного хранения

TIME_2MC        DW      0
DTIME_2MC	DD	0
Sys_Time        DD      0

EmptyStr2       DB      '  ', 0
                ; --- проверка прихода тестовых сообщений СОРМ
                DD      0
SORM_Test_Time  DD      0
_SORM_MAX_TIME  EQU     10 * 60 * 500   ; 10 минут !
SORM_Stoped	DB	0

                ; --- управление трансляцией конфигурации СИДС"у
Cfg_Send        DB      0       ; 0 - конфигурация не выдавалась, можно выдать

                ; Includes for AX data segment
                INCLUDE COPMDAT.INC                 ; загрузка данных СОРМ
IF _MSP EQ TRUE
                INCLUDE UPR\TO_COM.DAT              ; загрузка данных СОМ порта
                INCLUDE UPR\ALTERA.DAT              ; данные ALTERA
ENDIF
                INCLUDE UPATS.DAT
                INCLUDE MACR_SOR.INC                ; инициализация макросов
                INCLUDE MO\MAC_MO.INC               ; инициализация макросов МО
                INCLUDE MO\MO.DAT                   ; нициализация данных для МО и модемов
                INCLUDE WIN_VIU.VAR                 ; данные для вывода в видеообласть !!!!!!!
                INCLUDE PEREH_DN.INC                ; определение дискрипторов и прерываний

                INCLUDE X_25\MAC_LAPD.INC
                INCLUDE EQU.ASM                     ; определение ссылок на области !!!
                INCLUDE X_25\LAPB.VAR
                INCLUDE X_25\HW_C101D.INC
                INCLUDE X_25\X_25_2.VAR

;;;                INCLUDE P5ZTIS.INC                  ; для отладки ????
                INCLUDE KLADATA.INC                 ; ???? переменные клавиатуры?
;;;                INCLUDE W5ZDATA.INC
                include lib\alarm.dat
                ; --- новая конфигурация
                INCLUDE Config.DAT
                ; --- поле ввода
                INCLUDE Lib\Input.DAT
                ; --- эмулятор СОРМа
                INCLUDE Lib\SORMEmul.DAT

DOS_Flag        DB      0       ; 0 - перезагрузка, 1 - выход в DOS
DOS_mask_0      DB      0       ; досовская маска прерываний первого контроллера
DOS_mask_1      DB      0       ;                            второго
No_Debug_Flag	DB	0
;;;!!!!!!!!!!!!!
IF _MSP NE TRUE
not_REL_SORM    DD      0
ENDIF
not_REL_SOED    DD      0
not_REL_Pass    DW      0
_NOT_REL_PASS_MAX =     100

; ************  Таблица кодировки / декодировки модема MVT перенес 
KOD             DB      0,1,2,3,4,5,6,7,8,9,10
		DB	11,12,13,14,15,16,17,18,19,20
		DB	21,22,23,24,25,26,27,28,29,30
		DB	31,32,33,34,35,36,37,38,39,40
		DB	41,42,43,44,45,46,47,48,49,50
		DB	51,52,53,54,55,56,57,58,59,60
		DB	61,62,63,64,65,66,67,68,69,70
		DB	71,72,73,74,75,76,77,78,79,80
		DB	81,82,83,84,85,86,87,88,89,90
		DB	91,92,93,94,95,96,97,98,99,100
		DB	101,102,103,104,105,106,107,108,109,110
		DB	111,112,113,114,115,116,117,118,119,120
		DB	121,122,123,124,125,126,127,128,129,130
		DB	131,132,133,134,135,136,137,138,139,140
		DB	141,142,143,144,145,146,147,148,149,150
		DB	151,152,153,154,155,156,157,158,159,160
		DB	161,162,163,164,165,166,167,168,169,170
		DB	171,172,173,174,175,176,177,178,179,180
		DB	181,182,183,184,185,186,187,188,189,190
		DB	191,192,193,194,195,196,197,198,199,200
		DB	201,202,203,204,205,206,207,208,209,210
		DB	211,212,213,214,215,216,217,218,219,220
		DB	221,222,223,224,225,226,227,228,229,230
		DB	231,232,233,234,235,236,237,238,239,240
		DB	241,242,243,244,245,246,247,248,249,250
                DB      251,252,253,254,255

AXS_SIZE=$-NACH_AXS
DATA_AXS  ENDS

DATA_AXD        SEGMENT PARA USE16 PUBLIC 'DATA';Сегмент кофигурации 100-ка
NACH_AXD        LABEL   WORD
                INCLUDE TIS_DATA.INC
AXD_SIZE = $ - NACH_AXD
DATA_AXD        ENDS

DATA_BUS        SEGMENT PAGE USE16 PUBLIC 'DATA'
NACH_BUS        LABEL   WORD
                INCLUDE COPMBAS.INC             ; таблица логарифма и анти ...для СОРМа
BUS_SIZE = $ - NACH_BUS
DATA_BUS        ENDS

DATA_TELEF      SEGMENT PARA USE16 PUBLIC 'DATA';
                ; --- список объектов контроля типа пучок
Bunches         Obj_Struc       _BUNCH_MAX DUP(<>)
                ; --- список объектов контроля типа телефон
Objects         Obj_Struc       _OBJ_MAX DUP(<>)
                ; --- фиктивный объект для подключения КСЛ
Obj_for_Connect Obj_Struc       <>

DATA_TELEF      ENDS
;
DATA_SORM       SEGMENT PARA USE16 PUBLIC 'DATA';
                ; --- список сессий
Sessions        SORM_Struc      _SESSION_MAX DUP(<>)
DATA_SORM       ENDS
;
SCREEN_SEG_PCM  SEGMENT PARA USE16 PUBLIC 'DATA';Сегмент дополнительного экрана
                DB      16000 dup (80) ; экран СОРМА
                INCLUDE WINDOW.INC
SCREEN_SEG_PCM  ENDS
;
                ; ┌── ПУМА ───────────────────────┐
                ; │                               │
SCREEN_SEG_PUMA SEGMENT PARA USE16 PUBLIC 'DATA'; │
SCREEN_PUMA     LABEL   WORD                    ; │
                ; только определения
                INCLUDE PUMA\COM_Drv.DAT        ; │
                INCLUDE PUMA\PUMAProt.DAT       ; │
                INCLUDE PUMA\RingBuff.DAT       ; │
                ; а здесь есть данные
                INCLUDE PUMA\PUMA_Scr.DAT       ; │
                INCLUDE PUMA\CTO_Drv.DAT        ; │
                ; --- описания буферов ---------; │
                ; --- драйвера                  ; │
PUMA_Drv_List   DD      _PUMA_CHNLS_CNT * 2 DUP( 256 )
                ; --- сама ПУМА                 ; │
PUMA_Buf_List   DD      _PUMA_CHNLS_CNT * 2 DUP( _RBM + _PUMA_BUFF_SIZE )
SEG_PUMA_SIZE   = $ - SCREEN_PUMA               ; │
SCREEN_SEG_PUMA ENDS                            ; │
                ; │                               │
                ; └── ПУМА -──────────────────────┘


CODE_AXS        SEGMENT PARA USE16 PUBLIC "CODE"
ASSUME  DS:DATA_AXS,CS:CODE_AXS
StartNew:
                CLI
                CLD
                mov     sp, offset Stack_start
                MOV     AX,DATA_AXS             ; основной DATA сегмент
                MOV     DS,AX                   ; настроимся
                MOV     SS:SEG_AX,AX            ; сохраним

IF _MSP EQ TRUE
                MOV     AX,SCREEN_SEG_PCM       ; настроимся на виртуальный экран
ELSE
                ; --- определение наличия видеоадаптера и настройка видеорежима ---------------
                mov     ax, 1202h
                mov     bl, 30h
                int     10h                     ; 400 строк развертки
                mov     ax, 3
                int     10h
                mov     ax, 1112h               ; фонт из ПЗУ для латиницы
                int     10h

                mov     ax, 1003h
                xor     bl, bl
                int     10h                     ; выключить мерцание (яркий фон)

                mov     ah, 1                   ; гашение курсора
                mov     ch, 020h
                int     10h

		; !!!!!!!!!!!!!!!!!!!!!
                call    InitPCI_4
                call    InitPCI_8
                ; !!!!!!!!!!!!!!!!!!!!!

                MOV     AX,0B800H               ; экран
ENDIF
                MOV     SS:SEG_FS,AX            ; сохраним
                MOV     AX,0C800H               ; сегмент станции - 1
                MOV     SS:SEG_ST,AX            ; сохраним
                MOV     AX,0D000H               ; сегмент станции - 2
                MOV     SS:SEG_GS,AX            ; сохраним
                MOV     AX,SEG DATA_BUS         ;
                MOV     SS:SEG_BUS,AX           ; сохраним
                XOR     AX,AX                   ; нулевой сегмент
                MOV     SS:SEG_NULL,AX          ; сохраним
IF _MSP EQ TRUE
                ; --- остановка коммутатора
                MOV     ES,SS:SEG_ST
                mov     BYTE PTR es:[COM_MODE], 00h

                ; --- проверяем версию ИКМ 0-старое, 1-новое
                MOV     ES,ss:seg_gs            ; ATS1
                mov     ah,es:[5001h]           ; Первая цифра версии
                mov     al,es:[5002h]           ; Вторая цифра версии
                mov     versiy, al              ; записали версию
                not     ax                      ; перевернули биты
                mov     es:[5001h],ah           ;\ запишим гадость чтобы проверить
                mov     es:[5002h],al           ;/ не обычная ли это память
                mov     dh,es:[5001h]           ;\ прочитаем правильную версию
                mov     dl,es:[5002h]           ;/

                cmp     ax,dx                   ; проверим исказилось ли?
                je      old_ikm1                ; исказилось, значит есть ОЗУ т.е. старый
                mov     HardVers_hi, dh
                mov     HardVers_lo, dl
                mov     fg_prog,3               ;
                CMP     DH,2                    ; проверим, моноблок?
                JE      SHORT cont_init1        ; да -> уйдем
                ; здесь не моноблок
                CMP     DL,3                    ; старый икм, до 1.3 версии
                JA      OLD_IKM1                ; JB ??? да -> уйдем на старое железо
                MOV     VERSIY,DL               ; иначе новый ИКМ
                mov     fg_prog,1               ;
                jmp     cont_init1              ;
old_ikm1:
                mov     fg_prog,0               ;
                MOV     VERSIY,0                ;
cont_init1:
;;;                CALL    COMIN1                  ;
ELSE
                mov     fg_prog, 3              ; МСП-М !!!!
ENDIF
                ; --- можно переходить в защищенный режим
                JMP     PEREH

                INCLUDE PEREH_ZR.INC                ; библиотека для перехода в защищенный режим...
                include lib\exc.asm                 ; библиотека для обработки исключений

PEREH:          @PEREH_ZR                       ; перейдем в защищенный режим

               ; --- обработка предыдущей исключительной ситуации
                mov     fs, ss:SEG_BABA                 ;\
                mov     si, _BABA_FOR_EXC * 512         ;|
                cmp     byte ptr fs:[si],     'D'       ;|  проверка на...
                jne     no_last_exc                     ;|  была ли в прошлый раз
                cmp     byte ptr fs:[si + 1], 'E'       ;|  исключительная ситуация
                jne     no_last_exc                     ;|
                cmp     byte ptr fs:[si + 2], 'B'       ;|
                jne     no_last_exc                     ;|
                add     si, 3                           ;|
                mov     di, offset ex                   ;|
                mov     cx, _exception_data_l           ;|  перепись предыдущей
                push    ss                              ;|  исключительной ситуации
                pop     es                              ;|  в структуру
                rep     movs byte ptr es:[di], fs:[si]  ;/
no_last_exc:
IF _MSP EQ TRUE
                ; --- программирование АЛЬТЕРы если нужно
                cmp     fg_prog, 3              ; моноблок
                jne     PM_NoProgAltera         ; нет прыгаем дальше
                MOV     CX, 6
SLED_PROPIS_IKM:PUSH    CX
                CALL    READ_DOP_INFO
                CALL    ALTERA_IN1_1
                MOV     DL, CL
                POP     CX
                OR      DL, DL
                JZ      PM_NoProgAltera
                INC     CUR_ALTERA_CHIP
                LOOP    SLED_PROPIS_IKM
                MOV     ES, SS:SEG_GS
                MOV     ES:[440CH], BYTE PTR 0
                MOV     ES:[440DH], BYTE PTR 0
                MOV     ES:[440EH], BYTE PTR 0
                MOV     ES:[440FH], BYTE PTR 0
                MOV     ES:[5051H], BYTE PTR 0H
                MOV     ES:[5052H], BYTE PTR 0H
                XOR     AL, AL
                MOV     ES:[4409H], AL
                MOV     ES:[440AH], AL
                MOV     ES:[440BH], AL
PM_NoProgAltera:
ELSE
                ; --- снятие кодировки с тайм-слотов управления для безмодемного СОРМа
                mov     ax, _SEG_ALL
                mov     es, ax
        if _SORM_NO_MODEM eq TRUE
                mov     BYTE PTR es:[63h], 0c0h
        else
                mov     BYTE PTR es:[63h], 0
        endif

                ; --- HDD!!!
        	CALL_HDD InitHDD

                ; --- Загрузка конфигурации
                mov     edx, OFFSET _FILE_CFG_NAME
		CALL_HDD FileOpen
		jc  	Cfg_Open_2
Cfg_Read:
		xor	edx, edx
                mov     ecx, _FILE_CFG_SIZE
		CALL_HDD FileRead
		jc  	Cfg_Load_Err
		CALL_HDD FileClose
		jc  	Cfg_Load_Err
        	mov     si, offset LOG_LOADED_CFG
       	 	call    PrintLog
		jmp	no_Cfg_Load
Cfg_Load_Err:
        	mov     si, offset LOG_NO_CFG
       	 	call    PrintLogErr
		jmp	no_Cfg_Load
Cfg_Open_2:
                mov     edx, OFFSET _FILE_CFG_
		CALL_HDD FileOpen
		jnc  	Cfg_Read
no_Cfg_Load:

                ; --- Загрузка черного списка
                ; Открываем файл заново
		lea     edx, _FILE_BLST_NAME
		CALL_HDD FileOpen
		mov 	_FILE_BLST_ID, eax
		jc	BLST_READ_End
                ; Считываем из файла
		mov 	eax, _FILE_BLST_ID
		mov	ecx, _FILE_BLST_SIZE
		xor	edx, edx
		push	ds
		mov     ds, SS:SEG_BLK_LST
		CALL_HDD FileRead
		pop	ds
		jc  	BLST_READ_End
                ; Закрываем файл
		mov 	eax, _FILE_BLST_ID
		CALL_HDD FileClose
		jc 	BLST_READ_End
BLST_READ_End:

                ; --- Загрузка списка разрешённых АОН
                xor     ax, ax
                mov     ecx, 100000h
                xor     edi, edi
                mov     es, ss:SEG_ANI_LST
		cld
		db	67
                rep     stosw
                ; Открываем файл заново
                lea     edx, _FILE_ANI_NAME
		CALL_HDD FileOpen
                mov     _FILE_ANI_ID, eax
                jc      ANI_READ_Error
                ; Считываем из файла
                mov     eax, _FILE_ANI_ID
                mov     ecx, _FILE_ANI_SIZE
		xor	edx, edx
		push	ds
                mov     ds, SS:SEG_ANI_LST
		CALL_HDD FileRead
		pop	ds
                jc      ANI_READ_Error
                ; Закрываем файл
                mov     eax, _FILE_ANI_ID
		CALL_HDD FileClose
                jc      ANI_READ_Error
                jc      ANI_READ_End
ANI_READ_Error:
ANI_READ_End:

                ; --- Загрузка описаний пучков
                mov     al, 0ffh
                mov     ecx, _BUNCH_NAME_OFFS ; == мегабайт + 40 байт
                xor     edi, edi
                mov     es, ss:SEG_BUNCH
		cld
		@_RSTOSB
                ; Открываем файл заново
                lea     edx, _FILE_BUNCH_NAME
		CALL_HDD FileOpen
                mov     _FILE_BUNCH_ID, eax
                jc      Bunch_Read_Error
                ; Считываем из файла
                mov     eax, _FILE_BUNCH_ID
                mov     ecx, _FILE_BUNCH_SIZE
		xor	edx, edx
		push	ds
                mov     ds, SS:SEG_BUNCH
		CALL_HDD FileRead
		pop	ds
                jc      Bunch_Read_Error
                ; Закрываем файл
                mov     eax, _FILE_BUNCH_ID
		CALL_HDD FileClose
Bunch_Read_Error:

                ; --- инициализация
                ; железа
ENDIF
                CALL    INIT_HARD
IF _MSP EQ TRUE
                ; --- инициализируем экран
                MOV     AX, 168
                MOV     SS:SEG_FS,AX
ELSE
                MOV     ES,SS:SEG_ST
ENDIF
                DB      9AH
                DW      OFFSET INIC_W_V
                DW      224
IF _MSP EQ TRUE
                CALL    INI_COM_CTO

ENDIF
                MOV     FS, SS:SEG_SOED
                MOV     GS, SS:SEG_KONF
                CALL    MOIN_1
IF _MSP EQ TRUE
if _SORM_NO_MODEM ne TRUE
        if 1;_BUN ne TRUE
                ; --- включение абонентского комплекта
                mov     es, ss:SEG_GS                           ; настроимся на D000
                mov     BYTE PTR es:[5865h], 1                  ; активируем ист. питания на программирование
                ;
                MOV     ES, SS:SEG_GS
                MOV     ES:[ADR_REG_AK],  BYTE PTR 0BBh         ; подача питпния на АК
                MOV     ES:[ADR_REG_AK1], BYTE PTR 80h          ; разрешение опроса АК
                ;
                MOV     ES, SS:SEG_ST
                MOV     DI, 120                                 ; 120-й шаг коммутации
                SHL     DI, 2
                ADD     DI, ADR_OZU_K1
                MOV     ES:[DI],    BYTE PTR 0                  ; \ из 0-го канала ОЗУ данных
                MOV     ES:[DI+1],  BYTE PTR 1EH + _nom_ikm     ; / в 30-й канал ЦАП (ТС30 ИКМ)
                MOV     ES:[DI+4],  BYTE PTR 32                 ; \ из 32-го канала ОЗУ данных
                MOV     ES:[DI+5],  BYTE PTR 1FH + _nom_ikm     ; / в 31-й канал ЦАП (ТС31 ИКМ)
                MOV     ES:[DI+8],  BYTE PTR 31  + _nom_ikm     ; \ из 31-го канала ОЗУ данных
                MOV     ES:[DI+9],  BYTE PTR 32                 ; / в 32-й канал ЦАП (АК1)
                MOV     ES:[DI+12], BYTE PTR 30  + _nom_ikm     ; \ из 30-го канала ОЗУ данных
                MOV     ES:[DI+13], BYTE PTR 00H                ; / в 0-й канал ЦАП (АК0)
        else
        endif
endif ; // _SORM_NO_MODEM
ENDIF
                JMP     BAGIN_    ; Вторичная инициализация
;                INCLUDE MACR_SOR.INC                ; инициализация макросов
;                INCLUDE MO\MAC_MO.INC               ; инициализация макросов МО

                ; LOADING LIBRARY
                INCLUDE KLACODE.INC

                INCLUDE MO\MO.INC
                INCLUDE MO\MO_COM_B.INC ; заглушки и плохие команды
                INCLUDE MO\MO_COM.INC   ;
                INCLUDE MO\MO_SOED.INC  ;
                INCLUDE MO\MO_SORM.INC  ;
                INCLUDE MO\MO_SMESS.INC ;
                INCLUDE MO\MO_SESS.INC  ;
                INCLUDE MO\MO_KSL.INC   ;
IF _MSP EQ TRUE
                INCLUDE UPR\TO_COM.ASM
                include lib\confDrv.asm
ENDIF
                include lib\BlkLst.asm
                INCLUDE X_25\HW_C101C.INC
IF _MSP EQ TRUE
                INCLUDE UPR\ALTERA.ASM
                include lib\transf.asm
ENDIF
                include lib\tracer.asm
                include lib\alarm.asm

                ; --- новая конфигурация
                INCLUDE Config.ASM
                ; --- работа с пучками
                INCLUDE Bunch.ASM
                ; ---
                INCLUDE UPATS.ASM

                ; ┌── ПУМА ───────────────────────┐
                ; │                               │
                INCLUDE PUMA\COM_Drv.ASM        ; │
                INCLUDE PUMA\CTO_Drv.ASM        ; │
                INCLUDE PUMA\PUMA_Scr.ASM       ; │
                INCLUDE PUMA\PUMAProt.ASM       ; │
                INCLUDE PUMA\RingBuff.ASM       ; │
                ; │                               │
                ; └── ПУМА -──────────────────────┘
                INCLUDE PUMA\PUMA_Cmd.ASM       ; это часть программы, а не библиотеки
MOIN_1D         PROC    FAR
                CALL    MOIN_1
                RETF
                ENDP
IF _MSP EQ TRUE
SAVE_CONFD      PROC    FAR
                CALL    SAVE_CONF
                RETF
                ENDP
ENDIF

BAGIN_:         ; Вторичная инициализация
IF _MSP EQ TRUE
                cmp     FG_PROG,0               ; совсем старое железо
                JE      JUMP_OLD                ; да -> пропустим

                cmp     FG_PROG, 3              ; моноблок?
                Je      END_SAVE_IKM            ; пропустим пропустим прошивку ИКМ 1.5
                ; --- прошивка ИКМ 1.5
                CMP     VERSIY,2
                JB      END_SAVE_IKM;JUMP_OLD
                CALL    READ_DOP_INFO
                CALL    ALTERA_IN1
END_SAVE_IKM:
ELSE
                PUSH    ES
                PUSH    CX
ENDIF
                ; --- очистим сегмент тарификации
                MOV     ES,SS:SEG_TF
                MOV     AX,0FFFFh
                MOV     ECX,400000
                XOR     EDI,EDI
                @_RSTOSB
IF _MSP NE TRUE
                POP     CX
                POP     ES
ENDIF
                ; --- сбросим в ноль тарифкационные пакеты
                MOV     POS_TF,0
                MOV     SLED_TF,0

                ; --- ???
                MOV     ES,SS:SEG_GS
                MOV     ES:[5036H],BYTE PTR 00H ; отменим завоторы на ИКМ
                MOV     ES:[5035H],BYTE PTR 0FH ; РЕГ 35(ЗАВАРОТ)
                MOV     AL,0
                MOV     ES:[5036H],AL           ; отменим завоторы на ИКМ
                mov     es:[5076h],al
                mov     es:[5078h],byte ptr 16
                mov     es:[5077h],byte ptr 0ffh
                mov     es:[5079h],byte ptr 1
                MOV     ES:[500BH],BYTE PTR 0H
                MOV     ES:[5027H],BYTE PTR 0H

JUMP_OLD:       OR      FG_INIC,M_OR_1          ; пометим что железо принициализировано

                ; --- запуск коммутатора
                MOV     ES,SS:SEG_ST
IF _MSP NE TRUE
                mov     BYTE PTR es:[COM_MODE], 0F0h    ; запуск коммутатора
ELSE
                mov     BYTE PTR es:[COM_MODE], 070h    ; запуск коммутатора safe mode
ENDIF
                ; разрешение прерываний
                mov     al, 11100000B                   ;ТАЙМЕР
                out     021h, al                        ; клава и 2 контр. пр-й
                mov     AL, 01110011B
                OUT     0A1H,AL
                mov     al, 020h
                out     020h, al
                out     0a0h, al
                sti
IF _MSP EQ TRUE
                call    INIC_HARD_C101
ENDIF
                ; еще раз инициализируем ФАПЧ
                call    Fapch_Init
IF _MSP EQ TRUE
                ; --- КОММУТАЦИЯ ДЛЯ РАБОТЫ С ЦТО
                @COMM_CTO

                ; --- работа с пучками
                call    Bunch_Define            ; заглушка - в ручную заполняем Tranks
                call    Bunch_Init              ; чистим SEG_BUNCH
                call    Bunch_Calc              ; заполняем SG_BUNCH на основе информации из Tranks

                ; --- инициализация конфигурации
                call    StartConfig
ELSE
                mov     al, 0c1h
		out	21h, al
		mov	al, 0ffh
		out	0a1h, al
		xor	al, al
		out	70h, al
		sti

                mov     es, ss:SEG_GS
                mov     es:[5871h], BYTE PTR 7
                mov     es:[440ah], BYTE PTR 3

                cld                             ;
                ; --- работа с пучками
                call    Bunch_Define            ; заглушка - в ручную заполняем Tranks
                call    Bunch_Init              ; чистим SEG_BUNCH
                call    Bunch_Calc              ; заполняем SG_BUNCH на основе информации из Tranks
                call    StartConfig             ;

                @COMM_CTO                       ; установка коммутации для ЦТО
ENDIF
                ; --- инициализация ПУМы
                mov     eax,0ffffffffh          ;
IF _MSP EQ TRUE
                mov     al, _PT_COM2            ;
ELSE
                mov     al, _PT_COM1
ENDIF
                mov     ah, _PT_CTO             ;
                mov     bl, _BABA_FROM_CTO      ; 14
                mov     bh, _BABA_TO_CTO        ; 15
                call    PUMA_Init               ;
                xor     ebx, ebx                ;
                call    PUMA_Set_Tar            ;
                ; --- в лог - отметим запуск программы
                test    FG_PROGRAM, M_OR_15     ;
                jz      No_SORM_LOG_Start       ;
                call    PUMA_Start_Tar          ;
                mov     al, 8fh                 ;
                @_STOSB                         ;
                mov     al, 0ffh                ;
                @_STOSB                         ;
                call    PUMA_Send_Tar           ;
No_SORM_LOG_Start:
                ; ---- {Paul} --- (26\12\11) ---
                ; --- считаем флаги отбора команд
                push    ax
                mov     ax, FG_Curr_MO_Conf
                or      FG_Curr_MO_Num, ax
                pop     ax
                DB      9AH                     ;\
                DW      OFFSET Refresh_MO_Slct_Win_Far ;| обновим окно выбора команд МО
                DW      224                     ;/
                ; ---- {END} ------
                call    Tracer_Init

if _MSP NE TRUE
if _debugFile  ; MVT - открытие файла для вывода отладочной информации
		call	open_debug_file
endif
endif
;          ___________________________________________     _  _     ___________
;\        /         Let`s Blood Battle Begin          \   / \/ \   /
; \______/ _*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_ \_/\_/\_/\_/
; /      \  * * * * * * * * * * * * * * * * * * * * *  / \/ \/ \/ \
;/        \___________________________________________/   \_/\_/   \___________

BAGIN:          ; истинное начало цикла
public BAGIN
@Tracer_Step 100h
                mov     fs, ss:SEG_FS           ;
                xor     esi, esi                ;
                call    PUMA_Call               ;
                xor     ebx, ebx                ;
In_PUMA:        call    PUMA_In                 ;
                jecxz   Next_PUMA               ;
                call    PUMA_Cmd                ;
                jmp     In_PUMA                 ;
Next_PUMA:      inc     ebx                     ;
		cmp	ebx, _PUMA_CHNLS_CNT    ;
                jb      In_PUMA                 ;

@Tracer_Step 200h
                cmp     SOST_VIU5, _TC_Ctrl
                jne     @@no_tc
                DB      9AH                     ;\
                DW      OFFSET Refresh_TC_Far   ;|  контроль ТЧ СОРМа
                DW      224                     ;/
@@no_tc:
IF _MSP EQ TRUE
if 01
                cmp     SOST_VIU5, _OZUK
                jne     @@no_ozuk
                DB      9AH                     ;\
                DW      OFFSET Refresh_OZUK_Far ;|  контроль ОЗУ коммутации
                DW      224                     ;/
@@no_ozuk:
endif
ENDIF
IF _MSP EQ TRUE
                ; Watch Dog для старого железа
                push    ds ax                   ;\
                mov     ds, ss:SEG_AX           ;|
                xor     FiltrRM, 080h           ;|
                mov     al, FiltrRM             ;| WATCHDOG programm -=> old IRON <=-
                mov     ds, ss:seg_st           ;|
                mov     ds:[ADR_REG_FILT], al   ;|
                pop     ax ds                   ;/

                mov     ds, ss:seg_ax
                cmp     INT_5, 0
                je      return_x
                MOV     INT_5,0                 ;Сброс признака приема клавиши
                MOV     AL,KLAV                 ;В SI - SCAN-код
                @INT_5
RETURN_x:
ENDIF
@Tracer_Step 300h
                call    Check_SORM              ;
IF _MSP NE TRUE
                cld     ;;;!!!
		cmp	No_Debug_Flag, 0
		jne	No_Debug
		@DEBUG_PRINT
No_Debug:
                @TC_Debug

; - - - - - - - - - - - - - - -  Ф А П Ч - - - - - - - - - - - - - - - - - - -
;

                ; ФАПЧ
                CALL_PLL _CONTROL_CHANAL

                ; если включена проверка тестовых сообщений
                cmp     FG_SOST, 0
                je      No_SORM_Test_Check
                mov     eax, Current_Time
                sub     eax, SORM_Test_Time
                cmp     eax, _SORM_MAX_TIME
                jbe     No_SORM_Test_Check
                ;mov     SORM_Stoped, 255
No_SORM_Test_Check:

if _debugFile
	call display_debug_file_size
endif
ENDIF


               call    SetTime                 ;

                call    AskConfig               ;
                call    GetNewConfig            ;
                call    GetOldConfig            ;

@Tracer_Step 400h
                CALL    MOIN_2                  ;
@Tracer_Step 500h
IF _MSP EQ TRUE
                ; --- обработка ошибок - похоже где-то внутри обрабатывается mono watch-dog
                call    AlarmProccess           ; обработка ошибок по ПУМе
                call    AlarmManager            ; обработка ошибок по новому
ENDIF
@Tracer_Step 600h

                ; --- обработка основного цикла но в другом кодовом сегменте
                DB      9AH                     ;\
                DW      OFFSET WIN_SORM_GO      ;|  экран и части СОРМа
                DW      224                     ;/
@Tracer_Step 700h
IF _MSP EQ TRUE
        if _SORM_NO_MODEM eq TRUE
                call    c101_go_new             ; вызовим обработку HDLC
        else
                test    FG_PROG, _b_01          ; новое железо?
                jz      @@old_modem             ; нет, пропустим
                call    C101_GO_NEW             ; вызовим обработку HDLC
                jmp     @@c101_go_end
@@old_modem:    call    C101_GO                 ; вызовим обработку HDLC на С101
@@c101_go_end:
        endif
ELSE
                call    C101_Go_New
                ; WatchDog
                cmp     WD_Reboot, 0
                jnz     BAGIN
                mov     fs, ss:SEG_MAX_II
                mov     edx, ss:Curr_T
                sub     edx, WD_Time
                cmp     edx, _WD_TOUT
                jb      BAGIN
                mov     edx, ss:Curr_T
                mov     WD_Time, edx
                xor     WD_Value, 80h
                mov     al, WD_Value
                mov     fs:[ADR_WD], al
ENDIF

@Tracer_Step 800h

; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;
                test    FG_PROGRAM, _fg_prg_debug
                jz      @@not_dd
                mov     ah, cBLUE * 16 + clRED
		; ---
; MVT изменил форму вывода отладочных переменных
		mov	cx, 35
		lea	si, _1st_dd
		mov	di, 160 + 63 * 2
@@dd1:          push    cx
	        movzx	ebx, cx
		sub	ebx, 36
		neg	ebx
		mov	cx, 2
		call	print_D 
                add	di, 8
	        mov	ebx, [si]
		mov	cx, 8
                call    print_H
		pop	cx
		add	di, 152
		add	si, 4
		loop	@@dd1
		; ---
;		mov	cx, 20
;                lea     si, _4th_dd
;                mov     di, 160 + 71 * 2
;@@dd2:          push    cx
;	        mov	ebx, [si]
;		mov	cx, 8
;                call    print_H
;                pop     cx
;		add	di, 160
;		add	si, 4
;		loop	@@dd2
		; ---

;		mov	cx, 15
;                lea     si, _4th_dd
;                mov     di, 51 * 2
;@@dd3:          push    cx
;	        mov	ebx, [si]
;		mov	cx, 8
;                call    print_H
;		pop	cx
;		add	di, 160
;		add	si, 4
;                loop    @@dd3
@@not_dd:


                JMP     BAGIN           ; к началу цикла!!!
;               \\ ││ //         /\
;                \\││//         /││\
;                 \\//         /║││║\
;                  \/         //║││║\\
;                 ║  ║          ║││║
;                 ║││╚══════════╝││║
;                 ║└┴────────────┴┘║
;                 ╚════════════════╝
; MVT Заносимым в отладку параметром является eax
value_to_ddebug_proc proc far
		push	ds
		mov	ds,SS:SEG_AX
		;cmp	ds:__dd1, 0   ; __dd1 служит включателем
		;je	skip_debug_writing
		cmp	ds:__dd_num,35
		jae	skip_debug_writing
		push	ebx
		movzx	ebx, ds:__dd_num
		shl	ebx,2
		mov	ds:_1st_dword[ebx],eax
		inc	ds:__dd2
		inc	ds:__dd_num
		pop	ebx
skip_debug_writing:	
		pop	ds
		retf
value_to_ddebug_proc endp

if _MSP NE TRUE
if _debugFile  
; MVT Открытие отладочного файла
; Вход: нет
; Выход: в случае удачи DebugFileDescriptor содержит дескриптор, DebugFileOpened = 1
;        в случае неудачи DebugFileDescriptor неопределён, DebugFileOpened не меняется
open_debug_file	proc	
		push	ds edx eax ecx ebp
		mov	ds,SS:SEG_AX
		mov	isDebugFileOverflow, 0
		lea	edx, DebugFile
		mov	ebp,0
		CALL_HDD 	FileOpen
		jnc	@@ok_open
		mov	ebp,0
		CALL_HDD     FileCreate  ; EAX = идентификатор файла
		jc	@@end_open
@@ok_open:
		mov	DebugFileDescriptor,eax
		mov	ebp,0
		CALL_HDD 	FileSize
		mov	CurrentDebugFileSize,ecx
		jc	@@end_open
		mov	ebp,0
		CALL_HDD 	FileSeek
		jc	@@end_open
		mov	DebugFileOpened,2
		call write_cr_lf_to_debug_file
		mov	ebx,ss:Curr_T
		mov	ecx,8
		call write_ebx_to_debug_file
		lea	edx,DebugFileHeader
		mov	ecx,15
		call 	write_to_debug_file
		call	write_cr_lf_to_debug_file
		push	ds
		mov	ds,ss:SEG_PCM
		mov	edx,offset version_m
		mov	ecx,14
		call 	write_to_debug_file
		pop	ds
		call	write_cr_lf_to_debug_file
		mov	DebugFileOpened,1
@@end_open:
		pop	ebp ecx eax edx ds
		call	check_debug_file_size	
		ret
open_debug_file	endp		
		
; MVT Запись строки в отладочный файл
; Запись производится при DebugFileOpened =/= 0
; Вход: ds:edx - адрес строки
;       ecx - число записываемых байт
; Выход: нет
write_to_debug_file	proc	
		push	eax 
		push	ds
		mov	ds,SS:SEG_AX
		mov	al, DebugFileOpened
		pop	ds
		or	al,al
		jz	@@skip_write_debug
		call	file_write_buf
@@skip_write_debug:
		pop	eax 
		ret
write_to_debug_file	endp

; MVT Запись пробела в отладочный файл
; Вход: нет
; Выход: нет
write_space_to_debug_file	proc	
		push	ds edx ecx eax
		mov	ds,SS:SEG_AX
		mov	al, DebugFileOpened
		or	al,al
		jz	@@skip_write_space
		mov	ecx,1
		lea	edx,space_symb
		call	file_write_buf
@@skip_write_space:
		pop	eax ecx edx ds
		ret
write_space_to_debug_file	endp

; MVT Запись перевода строки CR LF в отладочный файл
; Вход: нет
; Выход: нет
write_cr_lf_to_debug_file	proc	
		push	ds edx ecx eax
		mov	ds,SS:SEG_AX
		mov	al, DebugFileOpened
		or	al,al
		jz	@@skip_write_cr_lf
		mov	ecx,2
		lea	edx,cr_lf
		call	file_write_buf
@@skip_write_cr_lf:
		pop	eax ecx edx ds
		ret
write_cr_lf_to_debug_file	endp

; MVT Запись трассировочной строки ici в отладочный файл
; Вход: edx - номер трассируемого места 
; Выход: нет
; Параметр не сохраняется
write_ici_to_debug_file	proc	
		push	ds ebx ecx eax
		mov	ds,SS:SEG_AX
		mov	al, DebugFileOpened
		or	al,al
		jz	@@skip_write_ici
		lea	ebx,ici
		add	edx,48
		mov	[ebx+3],dl
		mov	edx,ebx
		mov	ecx,5
		call	file_write_buf
@@skip_write_ici:
		pop	eax ecx ebx ds
		ret
write_ici_to_debug_file	endp

; MVT Запись числа в буфер в символьном виде
; Вход: ebx - число, ecx - число тетрад, eax - начальная позиция в буфере
; Выход: eax - новая позиция в буфере
ebx_to_buffer	proc
		push	ecx edi edx ds
		mov	ds,SS:SEG_AX
		lea	edi,DebugFileHexBuffer
		add	edi,eax
		dec	ecx
		and     cl, 7
@@loop:
		push    cx
		shl     cl, 2
		mov     edx, ebx
		shr     edx, cl
		and     dl, 0Fh
		cmp     dl, 9
		jbe     @@no_hex
		add     dl, 'A' - '9' - 1
@@no_hex:
		add     dl, '0'
		mov	ds:[edi],dl
		inc	edi
		inc	eax
		pop     cx
		dec     cl
		jns     @@loop
		pop	ds edx edi ecx
		ret
ebx_to_buffer	endp

; MVT Запись числа в отладочный файл
; Запись производится при DebugFileOpened =/= 0
; Вход: ebx - число, ecx - число тетрад
; Выход: нет
write_ebx_to_debug_file	proc	
		push	eax ds ecx edx edi
		mov	ds,SS:SEG_AX
		mov	al, DebugFileOpened
		or	al,al
		jz	@@skip_write_debug
		mov	eax,0
		call	ebx_to_buffer
		lea	edx,DebugFileHexBuffer
		call	file_write_buf
@@skip_write_debug:
		pop	edi edx ecx ds eax 
		ret
write_ebx_to_debug_file	endp

; MVT Запись массива байтов в отладочный файл в символьном виде
; Запись производится при DebugFileOpened =/= 0
; Вход: ds:esi - адрес массива байтов, ecx - число байтов
; Выход: нет
write_array_to_debug_file	proc
		push 	ebx ecx edx eax edi
		push	es
		push	ds
		pop	es
		push	ds
		mov	ds,SS:SEG_AX
		mov	al, DebugFileOpened
		or	al,al
		jz	@@skip_write_debug
		mov	eax,0
@@next_byte:
		push	ecx
		movzx	ebx, byte ptr es:[esi]
		mov	ecx, 2
		call	ebx_to_buffer
		lea	edi,DebugFileHexBuffer
		add	edi,eax
		mov	byte ptr [edi],'.'
		inc	eax
		pop	ecx
		inc	esi
		loop	@@next_byte
		lea	edx,DebugFileHexBuffer
		mov	ecx,eax
		call	file_write_buf
		call	write_cr_lf_to_debug_file
@@skip_write_debug:
		pop	ds
		pop	es
		pop	edi eax edx ecx ebx
		ret
write_array_to_debug_file	endp

; MVT закрытие отладочного файла
; Закрытие производится при DebugFileOpened =/= 0
; Вход: нет
; Выход: нет
close_debug_file	proc
		push	eax edx ecx ds
		mov	ds,SS:SEG_AX
		mov	al, DebugFileOpened
		or	al,al
		jz	@@skip_write_debug
                mov	DebugFileOpened, 2
		lea	edx,DebugFileBottom
		mov	ecx, 15
		call 	write_to_debug_file
if _isDebugBuffered
		call	flush_buffer_to_file
		call	write_cr_lf_to_debug_file ; Обязательно, т.к. последняя FileWrite не сохраняется
		call	flush_buffer_to_file
else
		call	write_cr_lf_to_debug_file ; Обязательно, т.к. последняя FileWrite не сохраняется
endif
		mov	DebugFileOpened, 0
		mov	eax,DebugFileDescriptor
		push	ebp
		mov	ebp,0
		CALL_HDD 	FileClose
		pop	ebp
@@skip_write_debug:
		pop	ds ecx edx eax 
		ret
close_debug_file	endp

close_debug_file_far	proc far
		call	close_debug_file
		retf
close_debug_file_far	endp

; MVT Сброс буфера отладочного файла при включённом _isDebugBuffered
; требует DebugFileOpened != 0
if _isDebugBuffered
flush_buffer_to_file	proc
		push	ds edx ecx eax ebp
		mov	ds,SS:SEG_AX
		lea	edx,DebugFileBuffer
		mov	ecx,DebugFileBufferPos
		mov	eax,DebugFileDescriptor
		mov	ebp,0
		CALL_HDD 	FileWrite 
		mov	DebugFileBufferPos, 0
		pop	ebp eax ecx edx ds
		ret
flush_buffer_to_file	endp
endif

; MVT перенос в буфер строки для буферизованного вывода и запись для небуферизованного
; Вход: ds:edx - адрес строки, ecx - количество байт
; Выход: нет
; Обновляет размер отладочного файла в переменной CurrentDebugFileSize
; Закрывает файл, если он велик
; требует DebugFileOpened != 0
file_write_buf proc
		push	ecx
if _isDebugBuffered
		push 	es edi esi
		mov	es,SS:SEG_AX
		mov	edi,es:DebugFileBufferPos
		add	edi,ecx
		cmp	edi,_DebugBufferSize
		jb	skip_flush_buffers ; если буфер переполнен, предварительно сбрасываем его
		call 	flush_buffer_to_file
skip_flush_buffers:
		lea	edi,DebugFileBuffer
		add	edi,es:DebugFileBufferPos
		add	es:DebugFileBufferPos,ecx
		mov	esi,edx
		cld
		rep movsb
		pop	esi edi es
else		
		push	ebp
		mov	ebp,0
		push	ds
		mov	ds,SS:SEG_AX
		mov	eax,DebugFileDescriptor
		pop	ds
		CALL_HDD	FileWrite 
		pop	ebp
endif
		pop	ecx
		add	CurrentDebugFileSize,ecx
		cmp	DebugFileOpened, 2
		je	@@skip_check_file_size  ; а то пойдёт по кругу
		call	check_debug_file_size	
@@skip_check_file_size:
		ret
file_write_buf endp

; MVT отображение длины отладочного файла
display_debug_file_size	proc
		push	ecx eax ds esi edi
		mov	ds,SS:SEG_AX
		cmp	isDebugFileOverflow, 0
		je	@@skip_display_overflow
		mov	ecx,8
		lea	esi,NoDebugFileSize
		mov	ah, DebugFileSizeColor
		mov	edi, 160 * 34 + 20
		call	print_S ; сообщаем на экран, что логгирование прервано
		jmp	@@end_display
@@skip_display_overflow:
		mov	al, DebugFileOpened
		or	al,al
		jz	@@end_display
		mov	ebx,CurrentDebugFileSize
		mov	ecx,8
		mov	ah, DebugFileSizeColor
		mov	edi, 160 * 34 + 20
		call	Print_H          
@@end_display:
		pop	edi esi ds eax ecx
		ret
display_debug_file_size	endp


; MVT проверка размера отладочного файла. Закрывает файл, если он велик
; требует DebugFileOpened != 0
check_debug_file_size	proc
		push	eax ds 
		mov	ds,SS:SEG_AX
		mov	eax,CurrentDebugFileSize
if _isDebugBuffered
		add	eax,_DebugBufferSize
else
		add	eax,_DebugWriteSize
endif
		cmp	eax,_DebugFileMaxSize
		jna	@@no_close
		mov	isDebugFileOverflow, 1
		push	ecx edx
                mov	DebugFileOpened, 2
		lea	edx,BreakDebugFile
		movzx	ecx, BreakDebugFileLen
		call 	write_to_debug_file ; выводим в файл, что логгирование прервано
		pop 	edx ecx 
		call	close_debug_file
@@no_close:
		pop	ds eax
		ret
check_debug_file_size	endp

; MVT очистка отладочного файла
clear_debug_file proc far
		call	close_debug_file
		push	DS EDX ebp
		mov	ds,SS:SEG_AX
		lea	edx, DebugFile
		mov	ebp,0
		CALL_HDD FileEraseName
		pop	ebp edx	ds
		call	open_debug_file
		retf
clear_debug_file endp
		
endif ; _debugFile
endif ; MSP

;;;; перезагрузка надо будет потом найти точки входа сюда и исправить логику
PEREZAGRUSKA1:  INT     3

SORM_SIZE = $ - START
CODE_AXS        ENDS

CODE_AXD        SEGMENT PARA USE16 PUBLIC "CODE"
                ASSUME  DS:DATA_AXS,CS:CODE_AXD
D_START         DB   3
                ; библиотеки СОРМа
                INCLUDE X_25\X_25_3N.INC
                INCLUDE X_25\LAPB.INC
                INCLUDE X_25\INIC.INC
                INCLUDE X_25\CIKL.INC
                INCLUDE X_25\SDL_LAPD.INC
                INCLUDE COPMCOD.INC
                ; библиотеки вывода на экран
                INCLUDE WIN_KLA.INC
                INCLUDE WIN_VIU.INC
                ; --- поле ввода
                INCLUDE Lib\Input.INC
                ; --- эмулятор СОРМа
                INCLUDE Lib\SORMEmul.INC

DOP_SIZE = $ - D_START
CODE_AXD        ENDS

        END     START


