; ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
; บ  ” ฉซ:         CTO_Drv.ASM                                                 บ
; บ  เฎฃเ ฌฌจแโ:                                                              บ
; บ  ฅเแจ๏:                                                                   บ
; วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
; บ  ’เฅกใฅโ:      CTO_Drv.DAT                                                 บ
; วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
; บ   งญ ็ฅญจฅ:   เ ฉขฅเ ช ญ ซ  –’                                          บ
; วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
; บ  ‘ฎคฅเฆจโ:                                                                 บ
; บ                                                                            บ
; บ                                                                            บ
; บ                                                                            บ
; ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

;ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
;บ     ==    ข๋งฎข แจญๅเฎญจงจเใ๎้จฉ ใช ง โฅซ์ ญ  €ใ             ==    บ Û
;ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ Û
;  ฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿
; --- คซ๏ 40 ฌแ ฏเฅเ๋ข ญจ๏
PUMA_CTO_40ms   PROC
                push    es
                mov     es, ss:SEG_PUMA_SCR
IF _MSP EQ TRUE
                mov     es:cto_curr_pos, 0
ELSE
                mov     es:cto_curr_pos, 16
ENDIF
                pop     es
                ret
PUMA_CTO_40ms   ENDP


;ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
;บ     ==    แฎกแโขฅญญฎ คเ ฉขฅเ                                   ==    บ Û
;ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ Û
;  ฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿฿
; --- คซ๏ 2 ฌแ ฏเฅเ๋ข ญจ๏
PUMA_CTO_2ms    PROC
                push    ds
                push    es
                push    fs
                pushad
                mov     ds, ss:SEG_PUMA_SCR
                ASSUME  ds:SCREEN_SEG_PUMA
                mov     fs, ss:SEG_PUMA
                ; --- ฏฎจแช เ กฎโ ๎้ฅฃฎ ช ญ ซ  –’
                xor     edx, edx
                mov     edi, _PUMA_TRAN_OFFS
@@find:         cmp     fs:[edi].L3_state, _PL3_OFF ; ฎโชซ๎็ฅญฎ !
                je      @@next
                cmp     fs:[edi].pt_type, _PT_CTO   ;
                je      @@found
@@next:         add     edi, PUMA_Trans_Size
                add     edx, 2
                cmp     edx, _PUMA_CHNLS_CNT * 2 ; จญไฎเฌ ๆจฎญญ๋ๅ ช ญ ซฎข ข กซฎชฅ * 2
                jb      @@find
                jmp     @@exit

@@found:        ; --- เ แ็๑โ ใช ง โฅซ๏
                add     cto_curr_pos, 16
                cmp     cto_curr_pos, 320
                jb      @@RxD
                mov     cto_curr_pos, 0

@@RxD:          ; --- ฏเจ๑ฌ ค ญญ๋ๅ
                mov     ecx, 16
                push    ds
                push    edx
                movzx   esi, fs:[edi].pt_cto_in         ; \
                shl     esi, 9                          ;  \ ds:esi - ญ 
                add     esi, cto_curr_pos               ;  /  ฏฏ เ โใเใ
                mov     ds, ss:SEG_BABA                 ; /
                mov     es, ss:SEG_PUMA_DRV             ; \ es จ edx - แฅฃฌฅญโ
                inc     edx                             ; / จ ü กใไฅเ  ข es
                call    RB_BPut
                pop     edx
                pop     ds

@@TxD:          ; ---  ฏฅเฅค ็ 
                mov     es, ss:SEG_BABA                 ; \
                movzx   edi, fs:[edi].pt_cto_out        ;  \ es:edi - ญ 
                shl     edi, 9                          ;  /  ฏฏ เ โใเใ
                add     edi, cto_curr_pos               ; /
                mov     fs, ss:SEG_PUMA_DRV             ; fs จ edx - แฅฃฌฅญโ จ ü ญใฆญฎฃฎ กใไฅเ  ข fs
                mov     ecx, 16                         ;
                call    RB_BGet                         ;
                sub     ecx, edx                        ; ฏฅเฅค ซจ ขแฅ 16 ก ฉโ ?
                jecxz   @@exit                          ; € -
                mov     al, 80h                         ; \ คฎก์๑ฌ
                rep     stosb; BYTE PTR [edi]             ; / ชฎคฎฌ โจ่จญ๋

@@exit:         ; ---
                popad
                pop     fs
                pop     es
                pop     ds
                ASSUME  ds:DATA_AXS
                ret
PUMA_CTO_2ms    ENDP

