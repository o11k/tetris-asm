; Not a pure state machine - uses randomness

; void initialize_game_state(State *state, Matrix *m, Tetrimino *t)
initialize_game_state:
    push bp
    mov  bp, sp
    push bx

    push MATRIX_W * MATRIX_H
    push MINO_NONE
    push word [bp+6]  ; matrix
    call memset
    add  sp, 6

    call next_mino
    push word [bp+8]  ; tetrimino
    push ax
    call initialize_tetrimino
    add sp, 4

    mov bx, word [bp+4]  ; state
    mov word [bx+GAME_STATE_OFF_FLAGS], 0
    mov ax, word [bp+6]  ; matrix
    mov word [bx+GAME_STATE_OFF_MATRIX], ax
    mov ax, word [bp+8]  ; tetrimino
    mov word [bx+GAME_STATE_OFF_TETRIMINO], ax

    mov word [bx+GAME_STATE_OFF_TIMER_FALL], CONSTANT_FALL_TIME
    mov word [bx+GAME_STATE_OFF_TIMER_LOCK], CONSTANT_LOCK_TIME

    mov byte [bx+GAME_STATE_OFF_LOWEST_ROW], 0xFF

    pop bx
    pop bp
    ret

; void advance(State *state, dword delta_time, KeyboardState *kb_state)
advance_game_state:
    push bp
    mov  bp, sp
    push bx

    mov bx, word [bp+4]  ; state

    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push word [bx+GAME_STATE_OFF_MATRIX]
    call tetrimino_is_on_ground
    add sp, 4
    test ax, ax
    jnz .on_ground

    mov ax, word [bp+6]  ; delta_time
    sub word [bx+GAME_STATE_OFF_TIMER_FALL], ax
    jnc .loop_fall_end

.loop_fall:
    ; try_fall()
    push 0
    push -1
    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push word [bx+GAME_STATE_OFF_MATRIX]
    call tetrimino_try_move
    add sp, 8

    ; timer_fall += FALL_TIME
    add word [bx+GAME_STATE_OFF_TIMER_FALL], CONSTANT_FALL_TIME
    test word [bx+GAME_STATE_OFF_TIMER_FALL], 0FFFFh
    js .loop_fall
.loop_fall_end:

.on_ground:


    pop bx
    pop bp
    ret
