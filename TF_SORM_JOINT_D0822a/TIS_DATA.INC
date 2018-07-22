
BUF_A_TIS       DW      KL_TIS DUP(16*2 DUP(0FFFFH))    ;N_TIS0-2B,ROAD_TIS-4B
TIS_TIS         DB      KL_TIS DUP(128 DUP(0FFH))       ; буфер маршрутов для ЦК
CHECK_ENGINE    DD      0

