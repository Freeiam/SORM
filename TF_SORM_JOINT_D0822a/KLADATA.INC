SEGKL           DW    0
OFSKL           DW    0
N_KLAV          DB    0
RUS_LAT         DB    0
FLG_KLA         DB    0
FG_KLA          DW      0       ;0-ALT
                                ;1-X
                        ;254-ALT
                        ;253-/ALT
                        ;252-PAGE_UP
                        ;251-PAGE_DOWN
IF _MSP EQ TRUE
                      ;  0    1    2    3    4    5    6    7    8   9   A   B    C    D   E   F
TABL_KLA        DB      0FFH, 24h,   1,   2,   3,   4,   5,   6,  7,  8,  9,  0,0FFH,0FFH,0AH,23H
                DB       51H, 57H, 45H, 52H, 54H, 59H, 55h, 49H         ;10H
                DB       4FH, 50H,0FFH,0FFH, 13H,0FFH, 41H, 53H
                DB       44H, 46H, 47H, 48H, 4AH, 4BH, 4CH,0FFH         ;20H
                DB      0FFH,0FFH,0FFH,0FFH, 5AH, 58H, 43H, 56H
                DB       42H, 4EH, 4DH,0FFH,0FFH,0FFH,0FFH,0FFH         ;30H
                DB      0FEH, 22H,0FFH, 16H, 17H, 18H, 19H, 1AH
                DB       1BH, 1CH, 1DH, 1EH, 1FH,0FFH,0FFH,0FFH         ;40H
                DB      14H, 0252,0FFH,11H,0FFH,12H,0FFH,0FFH,15H
                DB      251,5       DUP(0FFH)                           ;51H
                DB       20H, 21H,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH         ;60H
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH         ;70H
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH         ;80H
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH         ;90H
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH         ;A0H
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH         ;B0H
                DB      0253,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH         ;C0H
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH         ;D0H
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH         ;E0H
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH         ;F0H
                DB      0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
ELSE
                      ;    0     1     2     3     4     5     6     7
TABL_KLA        DB      0FFH,  24h,    1,    2,    3,    4,    5,    6
                DB         7,    8,    9,    0, 0FFH, 0FFH,  0AH,  23H
                DB       51H,  57H,  45H,  52H,  54H,  59H,  55h,  49H ;10H
                DB       4FH,  50H, 0FFH, 0FFH,  13H, 0FFH,  41H,  53H
                DB       44H,  46H,  47H,  48H,  4AH,  4BH,  4CH, 0FFH ;20H
                DB      0FFH, 0FFH, 0FFH, 0FFH,  5AH,  58H,  43H,  56H
                DB       42H,  4EH,  4DH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH ;30H
                DB      0FEH,  22H, 0FFH,  16H,  17H,  18H,  19H,  1AH
                DB       1BH,  1CH,  1DH,  1EH,  1FH, 0FFH, 0FFH, 0FFH ;40H
                DB       14H,  252, 0FFH,  11H, 0FFH,  12H, 0FFH, 0FFH
                DB       15H,  251, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH,  20H ;50H
                DB       21H, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH ;60H
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH ;70H
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH ;80H
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH ;90H
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH ;A0H
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH ;B0H
                DB       253, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH ;C0H
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH ;D0H
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH ;E0H
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH ;F0H
                DB      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH
ENDIF
;                DB      0FFH,24H,1,2,3,4,5,6,7,8,9,0,0FFH,0FFH,0AH,21H
;                DB      89H,96H,93H,8AH,85H,8DH,83h,98H                 ;10H
;                DB      99H,87H,95H,9AH,13H,0FFH,94H,9BH
;                DB      82H,80H,8FH,90H,8EH,8BH,84H,86H                 ;20H
;                DB      9DH,0FFH,0FFH,0FFH,9FH,97H,91H,8CH
;                DB      88H,92H,9CH,81H,9EH,0FFH,0FFH,0FFH              ;30H
;                DB      0FFH,20H,0FFH,16H,17H,18H,19H,1AH
;                DB      1BH,1CH,1DH,1EH,1FH,0FFH,0FFH,0FFH              ;40H
;                DB      14H,0FFH,0FFH,11H,0FFH,12H,0FFH,0FFH,15H
;                DB      6       DUP(0FFH)                               ;51H
;                DB      22H,23H
;                DB      167     DUP(0FFH)

