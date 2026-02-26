    org 100h
    use16

include 'constants.inc'

macro show_word x, y, val, buffer {
    push y
    push x
    push 00h
    push buffer
    call render_text
    add sp, 8

    push buffer
    push val
    call word_to_hex
    add sp, 4

    push y
    push x
    push 0Fh
    push buffer
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
    call draw_game_skeleton

    push g_time_state
    call get_time_delta
    add sp, 2

    push g_tetrimino
    push g_matrix
    push g_game_state
    call initialize_game_state
    add  sp, 6

.main_loop:
    call get_controls_state
    mov  bx, ax

    ; Esc = exit
    test byte [bx+CONTROLS_STATE_OFF_PAUSE], KEYBOARD_STATE_MASK_WAS_PRESSED
    jnz  .main_loop_end

    push g_time_state
    call get_time_delta
    add sp, 2

    push bx
    push ax
    push g_game_state
    call advance_game_state
    add sp, 6

    push g_game_state
    call draw_game_state_diff
    add  sp, 2

    test word [g_game_state+GAME_STATE_OFF_FLAGS], GAME_STATE_FLAGS_MASK_GAME_OVER
    jnz  .main_loop_end

    jmp .main_loop
.main_loop_end:

.teardown:
    call restore_int_09h
    ; exit 0
    mov ax, 4C00h
    int 21h

str_buf: times 5 db 0

include 'input/time.inc'
include 'input/keyboard_input.inc'
include 'input/controls.inc'
include 'output/render_game.inc'
include 'output/render_text.inc'
include 'misc.inc'
include 'logic/prng.inc'
include 'logic/tetrimino_generation.inc'
include 'logic/tetrimino_movement.inc'
include 'logic/state_machine.inc'


g_game_state: times GAME_STATE_SIZE db 0
g_matrix: times MATRIX_H*MATRIX_W db 0
g_tetrimino: times TETRIMINO_SIZE db 0
g_time_state: dw 0,0,0,0
