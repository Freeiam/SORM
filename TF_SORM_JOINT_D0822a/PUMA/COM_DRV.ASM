; ����������������������������������������������������������������������������ͻ
; �  ����:         Com_drv.ASM                                                 �
; �  �ணࠬ����:  ��窮� ����⠭⨭ ����஢�� +7 903 381 64 67               �
; �  �����:       15.06.2004                                                  �
; ����������������������������������������������������������������������������Ķ
; �  �ॡ��:      Com_Drv.DAT                                                 �
; ����������������������������������������������������������������������������Ķ
; �  �����祭��:   ࠡ�� � COM ���⮬                                         �
; ����������������������������������������������������������������������������Ķ
; �  ����ন�:                                                                 �
; �                                                                            �
; �                                                                            �
; �                                                                            �
; ����������������������������������������������������������������������������ͼ

; �������������������������������������������������������������͸
; � ������������� ���-����                                     �
; � � EBX ����� ����!! (3F8h or 2F8h)                          �
; ���������������������������������������������������������������

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
                ; � EBX ����� ����!! (3F8h or 2F8h)
                PUSH    EAX
                PUSH    EDX
                ; ��������� ������ ���������� ���
                ; ��������� �������� !!!
                MOV     EDX, EBX
                ADD     EDX, 3                          ; kod = inportb( port + 3 );
                IN      AL, DX
                AND     AL, 7Fh                         ; sb = kod & 0x7F;
                OUT     DX, AL                          ; outportb( port + 3, sb );

                ; ��������� ���������� �� ����������
                ; ��� �������� � ������ _01_, _10_
                MOV     AL, 3
                SUB     EDX, 2
                OUT     DX, AL                          ; outportb( port + 1, 3 );

                ; ������������� ��� ���������
                ; ��������
                MOV     AL, 80h                         ;
                ADD     EDX, 2
                OUT     DX, AL                          ; outportb( port + 3, 0x80 );

                ; ���������� ��������
                MOV     EAX, _COM_SPEED
                MOV     EDX, EBX
                OUT     DX, AL                          ; outportb( port, speed );

                ; ���������� ���������� ��������
                MOV     AL, 0
                INC     EDX
                OUT     DX, AL                          ; outportb( port + 1, 0 );

                ; ����� ��।�� 8 ���
                ; ���� �⮯ ���
                ; �஢�ઠ �� �⭮��� �� �ᯮ������
                ; ���� 3(2)F8h �ਢ���� � ࠡ���
                MOV     AL, 3
                ADD     EDX, 2
                OUT     DX, AL                          ; outportb( port + 3, 3 );

                ; ����砥� OUT2 (����᭠� �����) ???
                MOV     AL, 8
                INC     EDX
                OUT     DX, AL                          ; outportb( port + 4, 8 );

                ; ��祬 � �७� ��襬...?
                ; FIFO!!!!
                MOV     AL, 0C7h
                SUB     EDX, 2
                OUT     DX, AL                          ; outportb( port + 2, 0xC7 );

                IN      AL, 21h                         ; ����६ ���� ���뢠���
                MOV     AH, _MASK_PORT_1
                CMP     EBX, _ADR_COM_1
                JE      @@STEP
                MOV     AH, _MASK_PORT_2
@@STEP:
                NOT     AH
                AND     AL, AH                          ; ����襭�� IRQ4 ��� IRQ3
                OUT     021h, AL
                MOV     EDX, EBX
                IN      AL, DX                          ; kod = inportb( port );
                MOV     AL, 55h
                OUT     DX, AL                          ; outportb( port, kod );
                MOV     EDX, EBX
                ADD     EDX, 2
                IN      AL, DX                          ; �⥭�� ॣ-� ������-�� ���.

                POP     EDX
                POP     EAX
                RET
COM_INIT        ENDP


; ---
; �室:  es     -       ������ ���� ��⠭����� � SEG_PUMA
PUMA_COM1_OUT   PROC
                PUSH    EBX
                MOV     EBX, _ADR_COM_1
                CALL    COM_OUT
                POP     EBX
                ret
PUMA_COM1_OUT   ENDP

; ---
; �室:  es     -       ������ ���� ��⠭����� � SEG_PUMA
PUMA_COM2_OUT   PROC
                PUSH    EBX
                MOV     EBX, _ADR_COM_2
                CALL    COM_OUT
                POP     EBX
                ret
PUMA_COM2_OUT   ENDP

; ---
; �室:  es     -       ������ ���� ��⠭����� � SEG_PUMA
;        ebx    -       _PT_COM1 ��� _PT_COM2
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
@@find:         cmp     es:[edi].L3_state, _PL3_OFF ; �⪫�祭� !
                je      @@next
                cmp     es:[edi].pt_type, al        ;
                je      @@found
@@next:         add     edi, PUMA_Trans_Size
                add     ch, 2
                inc     cl
                cmp     cl, _PUMA_CHNLS_CNT     ; ���ଠ樮���� ������� � �����
                jb      @@find
                jmp     @@exit
                ;
@@found:
                MOV     FS, ss:SEG_PUMA_DRV     ; ᥣ���� ���⮢�� ���஢
                mov     edx, ebx
                add     EDX, 5                  ;\ PORT_5
                IN      AL, DX                  ; > ���� �� ��।��稪?
                TEST    AL, 20h                 ;/
                JZ      SHORT @@EXIT            ; �᫨ ���, � ��室

                MOVZX   EDX, ch                 ; � EDX ����� ����
                CALL    RB_BGetAL               ; ���⠫�
                OR      EDX, EDX                ; �᫨ � ���� ��祣� ���
                JZ      SHORT @@EXIT            ; �멤�� �� ���樠����樨

                mov     EDX, ebx                ;\ ��।��� ���� �� ����
                OUT     DX, AL                  ;/
@@EXIT:         sti
                ret
COM_OUT         ENDP


; --- ��ࠡ��稪� ��� ���⮢᪨� ���뢠��� ----------
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
