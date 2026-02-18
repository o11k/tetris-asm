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
    call get_accurate_time
    push cx
    push dx
    push ax

    push 20
    push 10
    push 00h
    push word_hex_buffer
    call render_text
    add sp, 8

    push word_hex_buffer + 0
    push word [bp-2]
    call word_to_hex
    add  sp, 4
    push word_hex_buffer + 4
    push word [bp-4]
    call word_to_hex
    add  sp, 4
    push word_hex_buffer + 8
    push word [bp-6]
    call word_to_hex
    add  sp, 4

    push 20
    push 10
    push 0Fh
    push word_hex_buffer
    call render_text
    add sp, 8

    add sp, 6
    jmp .loop_main
.loop_main_end:

    call restore_int_09h
    ; exit 0
    mov ax, 4C00h
    int 21h


word_hex_buffer: db 0,0,0,0, 0,0,0,0, 0,0,0,0, 0


include 'render.inc'
include 'render_text.inc'
include 'keyboard_input.inc'
include 'misc.inc'
include 'time.inc'

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
