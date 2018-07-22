; ----=> numToBCDax <=-----------------------
; Created by Smaller on 12/24/04 11:54
; трансформирование из обычного числа в BCD
;Входные данные: ax
;На выходе: ax
numToBCDax      proc
                public  numToBCDax
                push    ebx ecx edx

                xor     bx, bx

                xor     dx, dx
                mov     cx, 1000
                div     cx
                shl     ax, 12
                or      bx, ax
                mov     ax, dx

                xor     dx, dx
                mov     cx, 100
                div     cx
                shl     ax, 8
                or      bx, ax
                mov     ax, dx

                xor     dx, dx
                mov     cx, 10
                div     cx
                shl     ax, 4
                or      bx, ax

                mov     ax, dx
                or      ax, bx
                pop     edx ecx ebx
                ret
numToBCDax      endp


; перевод в BCD
_Hex_To_BCD:
                push    ebx ecx edx
                xor     edx,edx
                and     eax,0ffh
                mov     bl,al
                mov     cl,10
                div     cl
                mov     dl,al
                shl     al,1
                mov     cl,al
                shl     al,2
                add     al,cl
                sub     bl,al
                mov     al,bl
                shl     dl,4
                or      al,dl
                pop     edx ecx ebx
                ret
