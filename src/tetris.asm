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

    call get_bios_tick
    mov di, ax

.loop_main:
    call get_bios_tick
    cmp ax, di
    jb  .loop_main_end  ; break if overflow
    sub ax, 91  ; 5 seconds
    cmp ax, di
    jge .loop_main_end  ; break if 5 seconds passed

    cmp word [is_keyup], 0
    jz  .if_keydown

.if_keyup:
    push 20
    push 20
    push 24h
    push message_keyup
    call render_text
    add sp, 8

    jmp .if_end
.if_keydown:
    push 20
    push 20
    push 2Fh
    push message_keydown
    call render_text
    add sp, 8

.if_end:

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
