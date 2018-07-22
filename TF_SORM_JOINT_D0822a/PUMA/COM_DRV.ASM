; …ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕª
; ∫  î†©´:         Com_drv.ASM                                                 ∫
; ∫  è‡Æ£‡†¨¨®·‚:  íÎÁ™Æ¢ äÆ≠·‚†≠‚®≠ Ç®™‚Æ‡Æ¢®Á +7 903 381 64 67               ∫
; ∫  Ç•‡·®Ô:       15.06.2004                                                  ∫
; «ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ∂
; ∫  í‡•°„•‚:      Com_Drv.DAT                                                 ∫
; «ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ∂
; ∫  ç†ß≠†Á•≠®•:   ‡†°Æ‚† · COM ØÆ‡‚Æ¨                                         ∫
; «ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ∂
; ∫  ëÆ§•‡¶®‚:                                                                 ∫
; ∫                                                                            ∫
; ∫                                                                            ∫
; ∫                                                                            ∫
; »ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕº

; …ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ∏
; ∫ àçàñàÄãàáÄñàü ëéå-ØÆ‡‚†                                     ≥
; ∫ ¢ EBX ≠Æ¨•‡ ØÆ‡‚†!! (3F8h or 2F8h)                          ≥
; ”ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ

PUMA_COM1_INIT  PROC
                PUSH    EBX
                MOV     EBX, _ADR_COM_1
                CALL    COM_INIT
                POP     EBX
                ret
PUMA_COM1_INIT  ENDP

PUMA_COM2_INIT  PROC
                PUSH    EBX
                MOV     EBX, _ADR_COM_2
                CALL    COM_INIT
                POP     EBX
                ret
PUMA_COM2_INIT  ENDP

COM_INIT        PROC
                ; ¢ EBX ≠Æ¨•‡ ØÆ‡‚†!! (3F8h or 2F8h)
                PUSH    EAX
                PUSH    EDX
                ; ëóàíõÇÄÖå ÑÄççõÖ ëÅêÄëõÇÄÖå Åàí
                ; ìëíÄçéÇäà ÑÖãàíÖãü !!!
                MOV     EDX, EBX
                ADD     EDX, 3                          ; kod = inportb( port + 3 );
                IN      AL, DX
                AND     AL, 7Fh                         ; sb = kod & 0x7F;
                OUT     DX, AL                          ; outportb( port + 3, sb );

                ; éíäêõÇÄÖå êÄáêÖòÖçàÖ èé ÉéíéÇçéëíà
                ; èêà èÖêÖÑÄóà à èêàÖåÖ _01_, _10_
                MOV     AL, 3
                SUB     EDX, 2
                OUT     DX, AL                          ; outportb( port + 1, 3 );

                ; ìëíÄçÄÇãàÇÄÖå Åàí ìëíÄçéÇäà
                ; ÑÖãàíÖãü
                MOV     AL, 80h                         ;
                ADD     EDX, 2
                OUT     DX, AL                          ; outportb( port + 3, 0x80 );

                ; áÄèàëõÇÄÖå ÑÖãàíÖãú
                MOV     EAX, _COM_SPEED
                MOV     EDX, EBX
                OUT     DX, AL                          ; outportb( port, speed );

                ; èêéÑéãÜÄÖå áÄèàëõÇÄíú ÑÖãàíÖãú
                MOV     AL, 0
                INC     EDX
                OUT     DX, AL                          ; outportb( port + 1, 0 );

                ; §´®≠† Ø•‡•§†Á® 8 °®‚
                ; Æ§®≠ ·‚ÆØ °®‚
                ; Ø‡Æ¢•‡™† ≠† Á•‚≠Æ·‚Ï ≠• ®·ØÆ´Ïß„•‚·Ô
                ; ØÆ‡‚ 3(2)F8h Ø‡®¢•§•≠ ¢ ‡†°Æ‚„
                MOV     AL, 3
                ADD     EDX, 2
                OUT     DX, AL                          ; outportb( port + 3, 3 );

                ; ¢™´ÓÁ†•¨ OUT2 (ß†Ø†·≠†Ô ´®≠®Ô) ???
                MOV     AL, 8
                INC     EDX
                OUT     DX, AL                          ; outportb( port + 4, 8 );

                ; ß†Á•¨ ‚Æ Â‡•≠Ï Ø®Ë•¨...?
                ; FIFO!!!!
                MOV     AL, 0C7h
                SUB     EDX, 2
                OUT     DX, AL                          ; outportb( port + 2, 0xC7 );

                IN      AL, 21h                         ; ß†°•‡•¨ ¨†·™„ Ø‡•‡Î¢†≠®©
                MOV     AH, _MASK_PORT_1
                CMP     EBX, _ADR_COM_1
                JE      @@STEP
                MOV     AH, _MASK_PORT_2
@@STEP:
                NOT     AH
                AND     AL, AH                          ; ê†ß‡•Ë•≠®• IRQ4 ®´® IRQ3
                OUT     021h, AL
                MOV     EDX, EBX
                IN      AL, DX                          ; kod = inportb( port );
                MOV     AL, 55h
                OUT     DX, AL                          ; outportb( port, kod );
                MOV     EDX, EBX
                ADD     EDX, 2
                IN      AL, DX                          ; ó‚•≠®• ‡•£-‡† ®§•≠‚®‰-®® Ø‡•‡.

                POP     EDX
                POP     EAX
                RET
COM_INIT        ENDP


; ---
; ¢ÂÆ§:  es     -       §Æ´¶•≠ °Î‚Ï „·‚†≠Æ¢´•≠ ¢ SEG_PUMA
PUMA_COM1_OUT   PROC
                PUSH    EBX
                MOV     EBX, _ADR_COM_1
                CALL    COM_OUT
                POP     EBX
                ret
PUMA_COM1_OUT   ENDP

; ---
; ¢ÂÆ§:  es     -       §Æ´¶•≠ °Î‚Ï „·‚†≠Æ¢´•≠ ¢ SEG_PUMA
PUMA_COM2_OUT   PROC
                PUSH    EBX
                MOV     EBX, _ADR_COM_2
                CALL    COM_OUT
                POP     EBX
                ret
PUMA_COM2_OUT   ENDP

; ---
; ¢ÂÆ§:  es     -       §Æ´¶•≠ °Î‚Ï „·‚†≠Æ¢´•≠ ¢ SEG_PUMA
;        ebx    -       _PT_COM1 ®´® _PT_COM2
COM_OUT         PROC
                ;
                cli
                mov     al, _PT_COM1
                cmp     ebx, _ADR_COM_1
                je      @@to_find
                mov     al, _PT_COM2
                cmp     ebx, _ADR_COM_2
                jne     @@exit
                ;
@@to_find:      xor     cx, cx
                mov     edi, _PUMA_TRAN_OFFS
@@find:         cmp     es:[edi].L3_state, _PL3_OFF ; Æ‚™´ÓÁ•≠Æ !
                je      @@next
                cmp     es:[edi].pt_type, al        ;
                je      @@found
@@next:         add     edi, PUMA_Trans_Size
                add     ch, 2
                inc     cl
                cmp     cl, _PUMA_CHNLS_CNT     ; ®≠‰Æ‡¨†Ê®Æ≠≠ÎÂ ™†≠†´Æ¢ ¢ °´Æ™•
                jb      @@find
                jmp     @@exit
                ;
@@found:
                MOV     FS, ss:SEG_PUMA_DRV     ; ·•£¨•≠‚ °†©‚Æ¢ÎÂ °„‰•‡Æ¢
                mov     edx, ebx
                add     EDX, 5                  ;\ PORT_5
                IN      AL, DX                  ; > è„·‚ ´® Ø•‡•§†‚Á®™?
                TEST    AL, 20h                 ;/
                JZ      SHORT @@EXIT            ; Ö·´® ≠•‚, ‚Æ ¢ÎÂÆ§

                MOVZX   EDX, ch                 ; ¢ EDX ≠Æ¨•‡ °„‰•‡†
                CALL    RB_BGetAL               ; Ø‡ÆÁ®‚†´®
                OR      EDX, EDX                ; •·´® ¢ °„‰•‡• ≠®Á•£Æ ≠•‚
                JZ      SHORT @@EXIT            ; ¢Î©§•¨ ®ß ®≠®Ê®†´®ß†Ê®®

                mov     EDX, ebx                ;\ Ø•‡•§†•¨ °†©‚ ®ß °„‰•‡†
                OUT     DX, AL                  ;/
@@EXIT:         sti
                ret
COM_OUT         ENDP


; --- Æ°‡†°Æ‚Á®™® ëéå ØÆ‡‚Æ¢·™®Â Ø‡•‡Î¢†≠®© ----------
;
PUMA_COM1       PROC
                cli
                pushad
                push    ds
                push    es
                push    fs
                ;
                mov     es, ss:SEG_PUMA_SCR
                cmp     es:bPUMAInited, 0
                je      PC1_Exit
                ;
                @PUMA_COM _ADR_COM_1
                ;
PC1_Exit:       mov     al, 020h
                out     020h, al
                pop     fs
                pop     es
                pop     ds
                popad
                iretd
PUMA_COM1       ENDP

PUMA_COM2       PROC
                cli
                pushad
                push    ds
                push    es
                push    fs
                ;
                mov     es, ss:SEG_PUMA_SCR
                cmp     es:bPUMAInited, 0
                je      PC2_Exit
                ;
                @PUMA_COM _ADR_COM_2
                ;
PC2_Exit:       mov     al, 020h
                out     020h, al
                pop     fs
                pop     es
                pop     ds
                popad
                iretd
PUMA_COM2       ENDP

