Tracer_Step     PROC
                and     _ss_code, 0FFFF0000h
                or      _ss_code, eax
if 0
                push    es
                push    di
                mov     es, ss:seg_baba
                mov     di, 512 * _BABA_FOR_CP
                mov     es:[di], al
                mov     es:[di + 1], ah
                mov     BYTE PTR es:[di + 2], 0cdh
                mov     BYTE PTR es:[di + 3], 0abh
                mov     al, GOD_
                mov     es:[di + 4], al
                mov     al, MES
                mov     es:[di + 5], al
                mov     al, DEN_MES
                mov     es:[di + 6], al
                mov     al, SECUNDA
                mov     es:[di + 7], al
                mov     al, MIN
                mov     es:[di + 8], al
                mov     al, CHAS
                mov     es:[di + 9], al
                pop     di
                pop     es
endif
                ret
Tracer_Step     ENDP

@Tracer_Step    MACRO   Step
                push    eax
                mov     eax, Step
                call    Tracer_Step
                pop     eax
                ENDM

Tracer_Init     PROC
if 0
                push    es
                push    ax  di
                mov     es, ss:seg_baba
                mov     cx, 16
                mov     di, 512 * _BABA_FOR_CP
@@loop:         mov     al, es:[di]
                mov     es:[di + 16], al
                inc     di
                loop    @@loop
                @Tracer_Step    0
                pop     di ax
                pop     es
endif
                @Tracer_Step    0
                ret
Tracer_Init     ENDP

