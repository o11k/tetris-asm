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
    push si

    mov bx, word [bp+4]  ; state

    mov si, word [bp+8]  ; kb_state
    test byte [si+CONTROLS_STATE_OFF_HARD_DROP], KEYBOARD_STATE_MASK_WAS_PRESSED
    jz  .no_hard_drop

.loop_hard_drop:
    push 0
    push -1
    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push word [bx+GAME_STATE_OFF_MATRIX]
    call tetrimino_try_move
    add sp, 8

    test ax, ax
    jnz  .loop_hard_drop
.loop_hard_drop_end:
    jmp .lockdown

.no_hard_drop:

    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push word [bx+GAME_STATE_OFF_MATRIX]
    call tetrimino_is_on_ground
    add sp, 4
    test ax, ax
    jnz .initially_on_ground

    ; Soft drop
    mov ax, word [bp+6]  ; delta_time
    mov si, word [bp+8]  ; kb_state
    test byte [si+CONTROLS_STATE_OFF_SOFT_DROP], KEYBOARD_STATE_MASK_IS_DOWN
    jz .no_soft_drop
    mov cl, CONSTANT_SOFT_DROP_SPEEDUP
    mul cl
.no_soft_drop:

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

.initially_on_ground:

    ; if tetrimino.row < lowest_row
    mov si, word [bx+GAME_STATE_OFF_TETRIMINO]
    mov al, byte [si+TETRIMINO_OFF_ROOT+0]
    mov ah, byte [bx+GAME_STATE_OFF_LOWEST_ROW]
    cmp al, ah
    jge .not_lowest_row

    mov byte [bx+GAME_STATE_OFF_LOWEST_ROW], al
    mov word [bx+GAME_STATE_OFF_TIMER_LOCK], CONSTANT_LOCK_TIME

.not_lowest_row:

    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push word [bx+GAME_STATE_OFF_MATRIX]
    call tetrimino_is_on_ground
    add sp, 4
    test ax, ax
    jz  .not_on_ground

    ; TODO don't do this if initially not on ground
    mov ax, word [bp+6]  ; delta_time
    sub word [bx+GAME_STATE_OFF_TIMER_LOCK], ax
    jc  .no_lockdown

.lockdown:
    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push word [bx+GAME_STATE_OFF_MATRIX]
    call tetrimino_lock_down
    add sp, 4

    ; TODO start timer to generate - not immediately
    call next_mino
    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push ax
    call initialize_tetrimino
    add sp, 4

.no_lockdown:

.not_on_ground:

    pop si
    pop bx
    pop bp
    ret
