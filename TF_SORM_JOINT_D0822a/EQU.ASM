ADR_REG_AK      EQU     5860H   ;АДРЕС ТЕЛЕФОННОГО КОМПЛЕКТА НА ИКМ
ADR_REG_AK1     EQU     5864H   ;АДРЕС ВТОРОЙ 80H
ADR_REG_AC1	EQU	3422h	;Адрес 1-го регистра АЦП/ЦАП
ADR_REG_AC2	EQU	3423h	;Адрес 2-го регистра АЦП/ЦАП
ADR_OZU_K1	EQU	3000h	;Адрес 1-го ОЗУ коммутации
ADR_REG_FILT    EQU     3400H   ;АДРЕС РЕГИСТРА ФИЛЬТРА
ARSOS		EQU	3420H	;Адрес регистра состояния 100-ка
RG_REG_KOMM     EQU     3420H   ;Адрес регистра состояния 100-ка
ADR_REG_RM	EQU	3424h	;Адрес регистра режима модема
ADR_REG_DMO	EQU	3425h	;Адрес регистра длины пакета МО
ADR_IP_1        EQU     5860H   ;АДРЕС 1-ОГО ИСТОЧНИКА ПИТАНИЯ
ADR_IP_2        EQU     4323H   ;АДРЕС 2-ОГО ИСТОЧНИКА ПИТАНИЯ
;ADR_IP_1        EQU     3461H   ;АДРЕС 1-ОГО ИСТОЧНИКА ПИТАНИЯ
;ADR_IP_2        EQU     3462H   ;АДРЕС 2-ОГО ИСТОЧНИКА ПИТАНИЯ
ADR_REG_STA     EQU     3422h   ;Адрес регистра стативной сигнализации
ADR_REG_SM	EQU	3426h	;Адрес регистра состояния модема
ADR_OZU_K3	EQU	3800h	;Адрес расширения ОЗУ коммутации
ADR_OZU_COD	EQU	4200h	;Адрес ОЗУ кодировки
ADR_OZU_DEC	EQU	4300h	;Адрес ОЗУ декодировки
ADR_OZU_MOO	EQU	4800h	;Адрес ОЗУ МО модема (передатчика)
ADR_OZU_MOI	EQU	4900h	;Адрес ОЗУ МО модема (приемника)
ADR_BABA	EQU	8000h	;Адрес ОЗУ БАБА
ADR_RD0_PHA     EQU     0BF80h  ;Адрес порта A микросхемы ВВ55
ADR_RD1_PHA     EQU     0BF81h  ;Адрес порта B микросхемы ВВ55
ADR_RR_PHA      EQU     0BF82h  ;Адрес порта C микросхемы ВВ55
ADR_RPCM        EQU     5300h   ;Адрес регистра режима ИКМ
ADR_IPCM        EQU     5340h   ;Адрес регистра индикации номера ИКМ
ADR_EPCM        EQU     5380h   ;Адрес регистра индикации ошибок ИКМ
ADR_OZU_OS0     EQU     5090h   ;Адрес ОЗУ передачи СЦС ИКМ 0
ADR_OZU_OS1     EQU     50B0h   ;Адрес ОЗУ передачи СЦС ИКМ 1
ADR_RSR0        EQU     5220h   ;Адрес регистра RSR AGFA ИКМ 0
ADR_RSR1        EQU     5260h   ;Адрес регистра RSR AGFA ИКМ 1
ADR_RSR2        EQU     52A0h   ;Адрес регистра RSR AGFA ИКМ 2
ADR_RSR3        EQU     52E0h   ;Адрес регистра RSR AGFA ИКМ 3
ADR_CCR0        EQU     5220h   ;Адрес регистра CCR AGFA ИКМ 0
ADR_CCR1        EQU     5260h   ;Адрес регистра CCR AGFA ИКМ 1
ADR_CCR2        EQU     52A0h   ;Адрес регистра CCR AGFA ИКМ 2
ADR_CCR3        EQU     52E0h   ;Адрес регистра CCR AGFA ИКМ 3
ADR_FEC0        EQU     5221h   ;Адрес регистра FEC AGFA ИКМ 0
ADR_FEC1        EQU     5261h   ;Адрес регистра FEC AGFA ИКМ 1
ADR_FEC2        EQU     52A1h   ;Адрес регистра FEC AGFA ИКМ 2
ADR_FEC3        EQU     52E1h   ;Адрес регистра FEC AGFA ИКМ 3

ADR_NVP		EQU	5008H   ;Адрес регистра	НВП   XXXX0000 0-ВСЕ ЕСТЬ
ADR_SIA		EQU	501CH   ;Адрес регистра	СИА   1111ХХХХ 1-ПРИЕМ СИА
ADR_PCS		EQU	500AH   ;Адрес регистра	ПЦС   00000000 0-ВСЕ ЕСТЬ
ADR_AUS		EQU	5029H   ;Адрес регистра	АУС   00000000 0-ВСЕ ЕСТЬ

