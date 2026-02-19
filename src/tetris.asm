    org 100h
    use16

include 'constants.inc'

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
    call draw_matrix

.main_loop:
    call get_controls_state
    mov  si, ax

    ; Esc = exit
    mov  al, byte [si+CONTROLS_STATE_OFF_PAUSE]
    test al, KEYBOARD_STATE_MASK_WAS_PRESSED
    jnz  .main_loop_end

    ; Space = generate
    mov  al, byte [si+CONTROLS_STATE_OFF_HARD_DROP]
    test al, KEYBOARD_STATE_MASK_WAS_PRESSED
    jnz  .generate

    jmp .skip_generate
.generate:
    call next_mino
    push tetrimino_buffer
    push ax
    call initialize_tetrimino
    add sp, 4
    push tetrimino_buffer
    call draw_matrix_tetrimino
    add sp, 2

.skip_generate:
    jmp .main_loop
.main_loop_end:

.teardown:
    call restore_int_09h
    ; exit 0
    mov ax, 4C00h
    int 21h


tetrimino_buffer: times TETRIMINO_SIZE db 0

include 'input/time.inc'
include 'input/keyboard_input.inc'
include 'input/controls.inc'
include 'output/render.inc'
include 'output/render_text.inc'
include 'misc.inc'
include 'logic/prng.inc'
include 'logic/tetrimino_generation.inc'

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
