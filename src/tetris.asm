    org 100h
    use16

MINO_NONE EQU 00h
MINO_L    EQU 01h
MINO_J    EQU 02h
MINO_S    EQU 03h
MINO_Z    EQU 04h
MINO_T    EQU 05h
MINO_O    EQU 06h
MINO_I    EQU 07h

start:
    push bp
    mov bp, sp

    call register_int_09h

    ; video mode 13h
    mov ax, 0013h
    int 10h

    call draw_matrix

.loop_main:
    mov di, word [int_09h_scan_code]

    ; dx = color = keyup ? fuchsia : green
    test di, 80h
    jz .if_keydown
.if_keyup:
    mov dx, 24h
    jmp .if_end
.if_keydown:
    mov dx, 2Fh
.if_end:

    ; si = keyboard_table_names[scancode & 0x7F]
    mov si, di
    and si, 7Fh
    shl si, 1
    mov si, [keyboard_table_names+si]

    ; render
    push 20
    push 20
    push dx
    push si
    call render_text
    add sp, 8

.wait_for_new:
    cmp di, word [int_09h_scan_code]
    jz .wait_for_new

    ; erase
    push 20
    push 20
    push 0
    push si
    call render_text
    add sp, 8

    jmp .loop_main
.loop_main_end:


    call restore_int_09h
    ; exit 0
    mov ax, 4C00h
    int 21h

message_keyup:   db "UPUP", 0
message_keydown: db "DOWN", 0


include 'render.inc'
include 'render_text.inc'
include 'keyboard_input.inc'
include 'misc.inc'

; appears upside-down
matrix db \
    1,1,1,1,1,1,1,1,1,1, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    3,3,3,3,3,3,3,3,3,3, \
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
