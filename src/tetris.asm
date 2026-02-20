    org 100h
    use16

include 'constants.inc'

macro show_word x, y, val {
    push word_str_buffer
    push val
    call word_to_hex
    add sp, 4

    push y
    push x
    push 0Fh
    push word_str_buffer
    call render_text
    add sp, 8
}

start:
    push bp
    mov bp, sp

.setup:
    call register_int_09h

    ; video mode 13h
    mov ax, 0013h
    int 10h

    ; seed prng
    call get_accurate_time
    xor ax, cx
    xor ax, dx
    push ax
    call prng_seed
    add sp, 2

.main:
    call draw_borders

    push g_tetrimino
    push g_matrix
    push g_game_state
    call initialize_game_state
    add  sp, 6

    mov byte [g_tetrimino+TETRIMINO_OFF_ROOT+0], 0
    mov byte [g_tetrimino+TETRIMINO_OFF_ROOT+1], 0

    push g_game_state
    call draw_game_state_diff
    add  sp, 2

    show_word 20, 20, word [_dgsd_minos+(4*0)+(2*0)]
    show_word 60, 20, word [_dgsd_minos+(4*0)+(2*1)]
    show_word 20, 30, word [_dgsd_minos+(4*1)+(2*0)]
    show_word 60, 30, word [_dgsd_minos+(4*1)+(2*1)]
    show_word 20, 40, word [_dgsd_minos+(4*2)+(2*0)]
    show_word 60, 40, word [_dgsd_minos+(4*2)+(2*1)]
    show_word 20, 50, word [_dgsd_minos+(4*3)+(2*0)]
    show_word 60, 50, word [_dgsd_minos+(4*3)+(2*1)]
    
.teardown:
    call restore_int_09h
    ; exit 0
    mov ax, 4C00h
    int 21h


word_str_buffer: db 0,0,0,0,0

include 'input/time.inc'
include 'input/keyboard_input.inc'
include 'input/controls.inc'
include 'output/render.inc'
include 'output/render_text.inc'
include 'misc.inc'
include 'logic/prng.inc'
include 'logic/tetrimino_generation.inc'
include 'logic/tetrimino_movement.inc'
include 'logic/state_machine.inc'



g_game_state: times GAME_STATE_SIZE db 0
g_matrix: times MATRIX_H*MATRIX_W db 0
g_tetrimino: times TETRIMINO_SIZE db 0
