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

    mov word [bx+GAME_STATE_OFF_TIMER_FALL+0], CONSTANT_FALL_TIME_LO
    mov word [bx+GAME_STATE_OFF_TIMER_FALL+1], CONSTANT_FALL_TIME_HI
    mov word [bx+GAME_STATE_OFF_TIMER_LOCK+0], CONSTANT_LOCK_TIME_LO
    mov word [bx+GAME_STATE_OFF_TIMER_LOCK+1], CONSTANT_LOCK_TIME_HI

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

    pop bx
    pop bp
    ret
