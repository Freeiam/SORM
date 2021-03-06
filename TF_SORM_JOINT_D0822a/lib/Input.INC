;
; === ��⮬���᪮� ���� ����� - ��� ===============================
; --- �⮡ࠦ���� ���� � ���������� ���� �����
; �室;         si - 㪠��⥫� �� ����⥫� ���� �����: ⨯, �����, ��⪠
; ��室:        ���
Input_init_win  PROC
                mov     ah, _color_INPUT
                ; ����ࠨ���� ���� ��� ⥪�騩 ����
Input_init_win_color:
                push    ds
                pop     es
                mov     al, [si]
                mov     Input_Type, al
                mov     al, [si + 1]
                mov     Input_Len, al
                mov     Input_Data, si
                add     Input_Data, 3
                mov     Input_Count, 0       ; ��஫� �ᥣ�� ������ �� �����
                cmp     al, _PASS
                je      @@draw
                mov     al, [si + 2]
                mov     Input_Count, al
                movzx   cx, al
                jcxz    @@draw
                push    di
                push    si
                lea     di, Input_Buff
                add     si, 3
                cld
                rep     movsb
                pop     si
                pop     di
                ; �����塞 ����� ������
@@draw:         xor     al, al
                mov     cx, 100
                lea     di, Input_Footer
                cmp     Input_Type, _RD_ONLY
                jne     @@scan
                lea     di, Output_Footer
@@scan:         repne   scasb

                mov     ch, 100
                sub     ch, cl
                dec     ch
                mov     Input_Foot_Len, ch
                ; �����塞 ����� ������
                mov     cx, 100
                mov     di, si
                movzx   bx, [si + 1]
                add     di, bx
                add     di, 3
                repne   scasb
                mov     ch, 100
                sub     ch, cl
                dec     ch
                mov     Input_Label_Len, ch
                ; 䨪��㥬 ����� ������
                mov     ch, [si + 1]
                mov     Input_Data_Len, ch
                ; �롨ࠥ� ���������襣�
                movzx   dx, Input_Foot_Len
                cmp     dl, Input_Data_Len
                jae     @@cmp_2
                mov     dl, Input_Data_Len
@@cmp_2:        cmp     dl, Input_Label_Len
                jae     @@set_win_size
                mov     dl, Input_Label_Len
                ; ��⠭���� ࠧ��� ����
@@set_win_size: add     dl, 4
                ; ����塞 ��砫� ����
                mov     di, 80
                sub     di, dx
                shr     di, 1
                shl     di, 1
                add     di, _pos_Y
                ; --- ��㥬 ������� ࠬ�� � ����������
                ; ���� ���孨� 㣮�
                push    di
                mov     al, Input_LU
                mov     cx, 1
                call    printCdi
                ; ����᪠ �� ������
                movzx   cx, dl
                sub     cl, 2
                sub     cl, Input_Label_Len
                shr     cx, 1
                push    cx
                mov     al, Input_H
                call    printCdi
                ; �������
                push    si
                movzx   cx, [si + 1]
                add     cx, 3
                add     si, cx
                call    printSdi
                pop     si
                ; ����᪠ ��᫥ ������
                movzx   cx, dl
                sub     cl, 2
                sub     cl, Input_Label_Len
                pop     bx
                sub     cx, bx
                mov     al, Input_H
                call    printCdi
                ; �ࠢ� ���孨� 㣮�
                mov     al, Input_RU
                mov     cx, 1
                call    printCdi
                pop     di
                ; --- ��㥬 �।��� ����� � ����� �����
                add     di, 160
                push    di
                ; ����� �࠭��
                mov     al, Input_V
                mov     cx, 1
                call    printCdi
                ; �஡��� �� ���� �����
                movzx   cx, dl
                sub     cl, 2
                sub     cl, Input_Data_Len
                shr     cx, 1
                push    cx
                mov     al, ' '
                call    printCdi
                ; ���� �����
                mov     Input_Ptr, di
                call    Input_input
                ; �஡��� ��᫥ ���� �����
                movzx   cx, dl
                sub     cl, 2
                sub     cl, Input_Data_Len
                pop     bx
                sub     cx, bx
                mov     al, ' '
                call    printCdi
                ; �ࠢ�� �࠭��
                mov     al, Input_V
                mov     cx, 1
                call    printCdi
                pop     di
                ; --- ��㥬 ������ ����� � ��������
                add     di, 160
                ; ���� ������ 㣮�
                mov     al, Input_LD
                mov     cx, 1
                call    printCdi
                ; ����᪠ �� ������
                movzx   cx, dl
                sub     cl, 2
                sub     cl, Input_Foot_Len
                shr     cx, 1
                push    cx
                mov     al, Input_H
                call    printCdi
                ; �������
                push    si
                lea     si, Input_Footer
                cmp     Input_Type, _RD_ONLY
                jne     @@print
                lea     si, Output_Footer
@@print:        call    printSdi
                pop     si
                ; ����᪠ ��᫥ ������
                movzx   cx, dl
                sub     cl, 2
                sub     cl, Input_Foot_Len
                pop     bx
                sub     cx, bx
                mov     al, Input_H
                call    printCdi
                ; �ࠢ� ������ 㣮�
                mov     al, Input_RD
                mov     cx, 1
                call    printCdi
                ; ---
                ret
Input_init_win  ENDP

; --- ���������� ���� ����� ����묨 �� ����� (१���⠬� �।��饣� �����)
; �室:         ah - 梥� �����
Input_input     PROC
                ; �ନ�㥬 ���⮥ ���� �����
                mov     al, '�'
                movzx   cx, Input_Data_Len
                mov     di, Input_Ptr
                call    printCdi
                mov     ebp, edi
                ; ��।��塞 ⨯ �������� ���ଠ樨
                cmp     Input_Count, 0
                je      @@exit
                cmp     Input_Type, _PASS
                jne     @@not_pass
                ; ��஫� - �� �����뢠��
@@pass:         mov     al, Input_PS            ; '*'
                movzx   cx, Input_Count
                mov     di, Input_Ptr
                call    printCdi
                ret
@@not_pass:     ; �� ��� ��⠫��� ����� ������塞 ���� १���⠬� �।��饣� �����
                movzx   cx, Input_Count
                mov     di, Input_Ptr
                lea     si, Input_Buff
                call    printHex
;                add     di, cx
;                add     di, cx
                mov     edi, ebp
@@exit:         ret
Input_input     ENDP

; --- ।���஢���� ���� �����
Input_kla       PROC
                cmp     Input_Type, _RD_ONLY
                je      @@exit
                ; ---
                cmp     ah, 0aH         ; [ 0..9 ��� BACKSPACE ]
                jb      @@Digits        ; -> [ 0..9 ]
                je      @@BackSpace     ; -> [ BACKSPACE ]
                cmp     ah, 13H         ; [ ENTER ]
                je      @@Enter
                cmp     ah, 41h
                jb      @@exit
                cmp     ah, 46h
                ja      @@exit
                ; --- �㪮���
@@Letters:      sub     ah, 55
                ; --- ���쪨
@@Digits:       mov     al, Input_Count
                cmp     al, Input_Len
                jae     @@exit
                movzx   bx, Input_Count
                mov     Input_Buff[bx], ah
                inc     Input_Count
                jmp     @@redraw
                ; --- �����
@@BackSpace:    cmp     Input_Count, 0
                je      @@exit
                dec     Input_Count
@@redraw:       mov     ah, _color_INPUT
                call    Input_input
@@exit:         clc
                ret
                ; --- ����
@@Enter:        push    ds
                pop     es
                movzx   cx, Input_Count
                mov     di, Input_Data
                mov     [di - 1], cl
                jcxz    @@no_data
                lea     si, Input_Buff
                cld
                rep     movsb
@@no_data:      stc
                ret
Input_kla       ENDP
