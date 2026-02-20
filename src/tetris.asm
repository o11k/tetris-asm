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

    call next_mino
    push tetrimino_buffer
    push ax
    call initialize_tetrimino
    add sp, 4
    push tetrimino_buffer
    call draw_matrix_tetrimino
    add sp, 2

.main_loop:
    call get_controls_state
    mov  si, ax

    ; Esc = exit
    mov  al, byte [si+CONTROLS_STATE_OFF_PAUSE]
    test al, KEYBOARD_STATE_MASK_WAS_PRESSED
    jnz  .main_loop_end

    ; No space/arrows = next frame
    xor  al, al
    or   al, byte [si+CONTROLS_STATE_OFF_LEFT]
    or   al, byte [si+CONTROLS_STATE_OFF_RIGHT]
    or   al, byte [si+CONTROLS_STATE_OFF_ROT_RIGHT]
    or   al, byte [si+CONTROLS_STATE_OFF_SOFT_DROP]
    or   al, byte [si+CONTROLS_STATE_OFF_HARD_DROP]
    test al, KEYBOARD_STATE_MASK_WAS_PRESSED
    jz   .skip_everything

    ; Delete old tetrimino
    push tetrimino_buffer
    call delete_matrix_tetrimino
    add sp, 2

    ; Space = generate
    mov  al, byte [si+CONTROLS_STATE_OFF_HARD_DROP]
    test al, KEYBOARD_STATE_MASK_WAS_PRESSED
    jz   .skip_generate

.generate:
    call next_mino
    push tetrimino_buffer
    push ax
    call initialize_tetrimino
    add sp, 4
.skip_generate:


macro do_move name, key, row, col {
    mov  al, byte [si+CONTROLS_STATE_OFF_#key]
    test al, KEYBOARD_STATE_MASK_WAS_PRESSED
    jz   .skip_#name

    push col
    push row
    push tetrimino_buffer
    push matrix
    call tetrimino_try_move
    add sp, 8
.skip_#name:
}

    do_move left  ,LEFT      , 0 ,-1
    do_move right ,RIGHT     , 0 , 1
    do_move up    ,ROT_RIGHT , 1 , 0
    do_move down  ,SOFT_DROP ,-1 , 0

    ; draw new tetrimino
    push tetrimino_buffer
    call draw_matrix_tetrimino
    add sp, 2

.skip_everything:
    jmp .main_loop
.main_loop_end:

.teardown:
    call restore_int_09h
    ; exit 0
    mov ax, 4C00h
    int 21h


tetrimino_buffer: times TETRIMINO_SIZE db 0
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
    2,4,4,4,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    3,0,0,0,0,0,0,0,0,3, \
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
