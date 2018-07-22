; данные находятся в TF_TDAT.INC


COMIN1  PROC
;
; ********************************************  ЗАКРЫТИЕ ПРЕРЫВАНИЙ
        CLI
; ********************************************  ИНИЦИАЛИЗАЦИЯ СОМ-порта

        MOV     DX,N_PORT                       ; DX - адрес СОМ-порта
        MOV     COM_TX,DX                       ;Запоминаем адрес СОМ-порта
        ADD     DX,3                            ; kod = inportb( port + 3 );
        IN      AL,DX
        AND     AL,7Fh                          ; sb = kod & 0x7F;
        OUT     DX,AL                           ; outportb( port + 3, sb );
        JMP     SHORT   $+2
        MOV     AL,1

        SUB     DX,2
        OUT     DX,AL                           ; outportb( port + 1, 3 );
        JMP     SHORT   $+2
        MOV     AL,80h
        ADD     DX,2
        OUT     DX,AL                           ; outportb( port + 3, 0x80 );
        JMP     SHORT   $+2
        MOV     AL,DIV_SPEED
        MOV     DX,COM_TX
        OUT     DX,AL                           ; outportb( port, speed );
        JMP     SHORT   $+2
        MOV     AL,0
        INC     DX
        OUT     DX,AL                           ; outportb( port + 1, 0 );
        JMP     SHORT   $+2
;;;;
        MOV     AL,3
        ADD     DX,2
        OUT     DX,AL                           ; outportb( port + 3, 3 );
        JMP     SHORT   $+2
        MOV     AL,8
        INC     DX
        OUT     DX,AL                           ; outportb( port + 4, 8 );
        JMP     SHORT   $+2
        MOV     AL,0C7h
        SUB     DX,2
        MOV     COM_TX2,DX
        OUT     DX,AL                           ; outportb( port + 2, 0xC7 );
        JMP     SHORT   $+2
        ADD     DX,3
        MOV     COM_TX5,DX
        INC     DX
        MOV     COM_TX6,DX

; *****************************************    ОПРЕДЕЛЕНИЕ РАЗМЕРА КОНФИГУРАЦИИ
        MOV     DX,COM_TX
        IN      AL,DX                           ; kod = inportb( port );
IF _MSP EQ TRUE
        MOV     AL,55h
ENDIF
        OUT     DX,AL                           ; outportb( port, kod );
        MOV     DX,COM_TX2
        IN      AL,DX                           ;Чтение рег-ра идентиф-ии прер.

        RET
        ENDP




