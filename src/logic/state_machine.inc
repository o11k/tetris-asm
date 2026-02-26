; Not a pure state machine - uses randomness

; Assumes: bx=game_state, ax can be overridden
macro _state_machine_try_move row, col {
    push col
    push row
    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push word [bx+GAME_STATE_OFF_MATRIX]
    call tetrimino_try_move
    add sp, 8
}

; Assumes: bx=game_state, ax can be overridden
macro _state_machine_try_rotate direction {
    push direction
    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push word [bx+GAME_STATE_OFF_MATRIX]
    call tetrimino_try_rotate
    add sp, 6
}

; Wrap try_move and try_rotate with these macros count them towards extended placement
; Assumes: bx=game_state, ax and di can be overridden
macro _state_machine_extended_placement_before {
    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push word [bx+GAME_STATE_OFF_MATRIX]
    call tetrimino_is_on_ground
    add sp, 4
    mov di, ax
}
macro _state_machine_extended_placement_after {
    local .extended_placement
    local .no_extended_placement
    test di, di
    jz   .no_extended_placement
    test ax, ax
    jz   .no_extended_placement
    cmp  byte [bx+GAME_STATE_OFF_EXTENDED_PLACEMENT], 0
    jle  .no_extended_placement
.extended_placement:
    dec byte [bx+GAME_STATE_OFF_EXTENDED_PLACEMENT]
    mov word [bx+GAME_STATE_OFF_TIMER_LOCK], CONSTANT_LOCK_TIME
.no_extended_placement:
}

; Generate new mino and advance the next queue. Mino popped from queue in ax
; Expects: bx=game_state, ax, cx and di can be overridden
macro _state_machine_next_mino {
    xor ax, ax
    mov al, byte [bx+GAME_STATE_OFF_NEXT_QUEUE+0]
    mov di, ax

    mov cl, byte [bx+GAME_STATE_OFF_NEXT_QUEUE+1]
    mov     byte [bx+GAME_STATE_OFF_NEXT_QUEUE+0], cl
    mov cl, byte [bx+GAME_STATE_OFF_NEXT_QUEUE+2]
    mov     byte [bx+GAME_STATE_OFF_NEXT_QUEUE+1], cl
    mov cl, byte [bx+GAME_STATE_OFF_NEXT_QUEUE+3]
    mov     byte [bx+GAME_STATE_OFF_NEXT_QUEUE+2], cl
    mov cl, byte [bx+GAME_STATE_OFF_NEXT_QUEUE+4]
    mov     byte [bx+GAME_STATE_OFF_NEXT_QUEUE+3], cl
    mov cl, byte [bx+GAME_STATE_OFF_NEXT_QUEUE+5]
    mov     byte [bx+GAME_STATE_OFF_NEXT_QUEUE+4], cl

    call next_mino
    mov byte [bx+GAME_STATE_OFF_NEXT_QUEUE+5], al
    mov ax, di
}

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
    _state_machine_try_move -1, 0

    call next_mino
    mov byte [bx+GAME_STATE_OFF_NEXT_QUEUE+0], al
    call next_mino
    mov byte [bx+GAME_STATE_OFF_NEXT_QUEUE+1], al
    call next_mino
    mov byte [bx+GAME_STATE_OFF_NEXT_QUEUE+2], al
    call next_mino
    mov byte [bx+GAME_STATE_OFF_NEXT_QUEUE+3], al
    call next_mino
    mov byte [bx+GAME_STATE_OFF_NEXT_QUEUE+4], al
    call next_mino
    mov byte [bx+GAME_STATE_OFF_NEXT_QUEUE+5], al

    mov word [bx+GAME_STATE_OFF_TIMER_GEN], 0
    mov word [bx+GAME_STATE_OFF_TIMER_REPEAT], 0
    mov word [bx+GAME_STATE_OFF_TIMER_REPEAT_KICK_IN], 0

    mov word [bx+GAME_STATE_OFF_TIMER_FALL], CONSTANT_FALL_TIME
    mov word [bx+GAME_STATE_OFF_TIMER_LOCK], CONSTANT_LOCK_TIME

    mov byte [bx+GAME_STATE_OFF_LOWEST_ROW], 0xFF
    mov byte [bx+GAME_STATE_OFF_EXTENDED_PLACEMENT], CONSTANT_EXTENDED_PLACEMENT

    mov byte [bx+GAME_STATE_OFF_HOLD], MINO_NONE

    mov word [bx+GAME_STATE_OFF_MARATHON_LINES], CONSTANT_MARATHON_LINES

    mov word [bx+GAME_STATE_OFF_TIMER+0], 0
    mov word [bx+GAME_STATE_OFF_TIMER+1], 0

    pop bx
    pop bp
    ret

; void advance_game_state(State *state, dword delta_time, KeyboardState *kb_state)
advance_game_state:
    push bp
    mov  bp, sp
    push bx
    push si
    push di

    mov bx, word [bp+4]  ; state

    mov ax, word [bp+6]  ; delta_time
    add word [bx+GAME_STATE_OFF_TIMER+0], ax
    adc word [bx+GAME_STATE_OFF_TIMER+2], 0

; ========== GENERATION PHASE ==========
    test word [bx+GAME_STATE_OFF_TIMER_GEN], 0xFFFF
    jz   .no_generation_phase

    mov ax, word [bp+6]  ; delta_time
    sub word [bx+GAME_STATE_OFF_TIMER_GEN], ax
    ja  .no_generation_yet

    mov word [bx+GAME_STATE_OFF_TIMER_GEN], 0
    mov word [bx+GAME_STATE_OFF_TIMER_LOCK], CONSTANT_LOCK_TIME
    mov word [bx+GAME_STATE_OFF_TIMER_FALL], CONSTANT_FALL_TIME
    mov byte [bx+GAME_STATE_OFF_EXTENDED_PLACEMENT], CONSTANT_EXTENDED_PLACEMENT

    _state_machine_next_mino

.initialize_tetrimino:

    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push ax
    call initialize_tetrimino
    add sp, 4

    _state_machine_try_move -1, 0

    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push word [bx+GAME_STATE_OFF_MATRIX]
    call tetrimino_detect_collision
    add sp, 4

    test ax, ax
    jz   .no_block_out
    or   word [bx+GAME_STATE_OFF_FLAGS], GAME_STATE_FLAGS_MASK_GAME_OVER

.no_block_out:
.no_generation_yet:
    jmp .cleanup
.no_generation_phase:

; ========== HARD DROP ==========

    mov si, word [bp+8]  ; kb_state
    test byte [si+CONTROLS_STATE_OFF_HARD_DROP], KEYBOARD_STATE_MASK_WAS_PRESSED
    jz  .no_hard_drop

.loop_hard_drop:
    _state_machine_try_move -1, 0

    test ax, ax
    jnz  .loop_hard_drop
.loop_hard_drop_end:
    jmp .lockdown

.no_hard_drop:

; ========== HOLD ==========

    mov si, word [bp+8]  ; kb_state
    test byte [si+CONTROLS_STATE_OFF_HOLD], KEYBOARD_STATE_MASK_WAS_PRESSED
    jz  .no_hold
    
    test byte [bx+GAME_STATE_OFF_FLAGS], GAME_STATE_FLAGS_MASK_HOLD
    jnz  .no_hold

    cmp byte [bx+GAME_STATE_OFF_HOLD], MINO_NONE
    jne .no_generate_hold

    _state_machine_next_mino
    mov byte [bx+GAME_STATE_OFF_HOLD], al
.no_generate_hold:

    mov si, word [bx+GAME_STATE_OFF_TETRIMINO]
    xor ax, ax
    mov cl, byte [si+TETRIMINO_OFF_TYPE]  ; current mino
    mov al, byte [bx+GAME_STATE_OFF_HOLD] ; held mino
    mov byte [bx+GAME_STATE_OFF_HOLD], cl

    or byte [bx+GAME_STATE_OFF_FLAGS], GAME_STATE_FLAGS_MASK_HOLD
    
    jmp .initialize_tetrimino

.no_hold:

; ========== LOCK DOWN TIMER ==========

    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push word [bx+GAME_STATE_OFF_MATRIX]
    call tetrimino_is_on_ground
    add sp, 4
    test ax, ax
    jz  .not_initially_on_ground

    mov ax, word [bp+6]  ; delta_time
    sub word [bx+GAME_STATE_OFF_TIMER_LOCK], ax
.not_initially_on_ground:

; ========== SIDE-TO-SIDE MOVEMENT ==========

    push word [bp+8]
    push word [bp+6]
    push word [bp+4]
    call advance_game_state__side_to_side
    add sp, 6

; ========== ROTATION ==========

    mov si, word [bp+8]  ; kb_state

    test byte [si+CONTROLS_STATE_OFF_ROT_LEFT], KEYBOARD_STATE_MASK_WAS_PRESSED
    jz   .no_rotate_left
    _state_machine_extended_placement_before
    _state_machine_try_rotate 3
    _state_machine_extended_placement_after
.no_rotate_left:

    test byte [si+CONTROLS_STATE_OFF_ROT_RIGHT], KEYBOARD_STATE_MASK_WAS_PRESSED
    jz   .no_rotate_right
    _state_machine_extended_placement_before
    _state_machine_try_rotate 1
    _state_machine_extended_placement_after
.no_rotate_right:

    test byte [si+CONTROLS_STATE_OFF_ROT_180], KEYBOARD_STATE_MASK_WAS_PRESSED
    jz   .no_rotate_180
    _state_machine_extended_placement_before
    _state_machine_try_rotate 2
    _state_machine_extended_placement_after
.no_rotate_180:

; ========== FALLING / SOFT DROP ==========

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
    _state_machine_try_move -1, 0

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
    mov byte [bx+GAME_STATE_OFF_EXTENDED_PLACEMENT], CONSTANT_EXTENDED_PLACEMENT
    mov word [bx+GAME_STATE_OFF_TIMER_LOCK], CONSTANT_LOCK_TIME
.not_lowest_row:
    
; ========== LOCKDOWN ==========

    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push word [bx+GAME_STATE_OFF_MATRIX]
    call tetrimino_is_on_ground
    add sp, 4
    test ax, ax
    jz  .not_on_ground

    cmp word [bx+GAME_STATE_OFF_TIMER_LOCK], 0
    jg  .no_lockdown

.lockdown:
    push word [bx+GAME_STATE_OFF_TETRIMINO]
    call advance_game_state__detect_lock_out
    add sp, 2
    test ax, ax
    jz   .no_lock_out

    or  word [bx+GAME_STATE_OFF_FLAGS], GAME_STATE_FLAGS_MASK_GAME_OVER
    jmp .cleanup

.no_lock_out:
    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push word [bx+GAME_STATE_OFF_MATRIX]
    call tetrimino_lock_down
    add sp, 4

    mov word [bx+GAME_STATE_OFF_TIMER_GEN],  CONSTANT_GEN_TIME
    
    ; unset hold flag
    mov al, GAME_STATE_FLAGS_MASK_HOLD
    not al
    and byte [bx+GAME_STATE_OFF_FLAGS], al

    push word [bx+GAME_STATE_OFF_MATRIX]
    call advance_game_state__line_clears
    add sp, 2

    sub word [bx+GAME_STATE_OFF_MARATHON_LINES], ax
    ja  .no_zero_marathon

    mov word [bx+GAME_STATE_OFF_MARATHON_LINES], 0
    or  word [bx+GAME_STATE_OFF_FLAGS], GAME_STATE_FLAGS_MASK_GAME_OVER
.no_zero_marathon:

.no_lockdown:

.not_on_ground:

.cleanup:

    pop di
    pop si
    pop bx
    pop bp
    ret


; void advance_game_state__side_to_side(State *state, dword delta_time, KeyboardState *kb_state)
advance_game_state__side_to_side:
    push bp
    mov  bp, sp
    push bx
    push si
    push di

    mov bx, word [bp+4]  ; state
    mov si, word [bp+8]  ; kb_state

    ; cx = last direction
    mov cx, word [bx+GAME_STATE_OFF_FLAGS]
    and cx, GAME_STATE_FLAGS_MASK_DIR

    ; === RESOLVE CURRENT DIRECTION ===

    ; if (left.is_down && right.is_down) {
    ;   if (left.was_pressed) {...}
    ;   else if (right.was_pressed) {...}
    ;   else {...}
    ;}
    ; else if (left.is_down) {...}
    ; else if (right.is_down) {...}
    ; else {...}

    test byte [si+CONTROLS_STATE_OFF_LEFT], KEYBOARD_STATE_MASK_IS_DOWN
    jz   .resolve_check_elif
    test byte [si+CONTROLS_STATE_OFF_RIGHT], KEYBOARD_STATE_MASK_IS_DOWN
    jnz  .resolve_if_both
.resolve_check_elif:
    test byte [si+CONTROLS_STATE_OFF_LEFT], KEYBOARD_STATE_MASK_IS_DOWN
    jnz  .resolve_elif_left
    test byte [si+CONTROLS_STATE_OFF_RIGHT], KEYBOARD_STATE_MASK_IS_DOWN
    jnz  .resolve_elif_right
    jmp  .resolve_else

.resolve_if_both:
    test byte [si+CONTROLS_STATE_OFF_LEFT], KEYBOARD_STATE_MASK_WAS_PRESSED
    jnz  .resolve_if_both_if_left
    test byte [si+CONTROLS_STATE_OFF_RIGHT], KEYBOARD_STATE_MASK_WAS_PRESSED
    jnz  .resolve_if_both_if_right
    jmp  .resolve_if_both_else

.resolve_if_both_if_left:
    mov ax, GAME_STATE_FLAGS_MASK_DIR_LEFT
    jmp .resolve_if_both_endif
.resolve_if_both_if_right:
    mov ax, GAME_STATE_FLAGS_MASK_DIR_RIGHT
    jmp .resolve_if_both_endif
.resolve_if_both_else:
    mov ax, cx
    jmp .resolve_if_both_endif
.resolve_if_both_endif:
    
    jmp .resolve_endif
.resolve_elif_left:
    mov ax, GAME_STATE_FLAGS_MASK_DIR_LEFT
    jmp .resolve_endif
.resolve_elif_right:
    mov ax, GAME_STATE_FLAGS_MASK_DIR_RIGHT
    jmp .resolve_endif
.resolve_else:
    mov ax, 0
    jmp .resolve_endif
.resolve_endif:

    ; state.direction = new direction
    mov dx, GAME_STATE_FLAGS_MASK_DIR
    not dx
    and word [bx+GAME_STATE_OFF_FLAGS], dx
    or  word [bx+GAME_STATE_OFF_FLAGS], ax

    ; === TAP MOVEMENT ===
    test byte [si+CONTROLS_STATE_OFF_LEFT], KEYBOARD_STATE_MASK_WAS_PRESSED
    jz   .no_tap_left

    _state_machine_extended_placement_before
    _state_machine_try_move 0, -1
    _state_machine_extended_placement_after

    mov word [bx+GAME_STATE_OFF_TIMER_REPEAT_KICK_IN], CONSTANT_AUTOREPEAT_KICK_IN_TIME
    mov word [bx+GAME_STATE_OFF_TIMER_REPEAT], 0

.no_tap_left:
    test byte [si+CONTROLS_STATE_OFF_RIGHT], KEYBOARD_STATE_MASK_WAS_PRESSED
    jz   .no_tap_right

    _state_machine_extended_placement_before
    _state_machine_try_move 0, 1
    _state_machine_extended_placement_after

    mov word [bx+GAME_STATE_OFF_TIMER_REPEAT_KICK_IN], CONSTANT_AUTOREPEAT_KICK_IN_TIME
    mov word [bx+GAME_STATE_OFF_TIMER_REPEAT], 0

.no_tap_right:

    ; === NO DIRECTION HELD ===
    test ax, ax
    jnz  .direction_held

    mov word [bx+GAME_STATE_OFF_TIMER_REPEAT_KICK_IN], 0
    mov word [bx+GAME_STATE_OFF_TIMER_REPEAT], 0
    jmp .return
.direction_held:

    ; === DIRECTION CHANGED WHILE HELD ===
    cmp cx, ax
    je  .direction_not_changed

    mov word [bx+GAME_STATE_OFF_TIMER_REPEAT_KICK_IN], CONSTANT_AUTOREPEAT_KICK_IN_TIME
    mov word [bx+GAME_STATE_OFF_TIMER_REPEAT], 0

.direction_not_changed:

    ; === DELAY PHASE ===
    cmp word [bx+GAME_STATE_OFF_TIMER_REPEAT_KICK_IN], 0
    jz  .no_delay

    mov dx, word [bp+6]  ; delta_time
    sub word [bx+GAME_STATE_OFF_TIMER_REPEAT_KICK_IN], dx
    jnc .delay_not_over
    mov word [bx+GAME_STATE_OFF_TIMER_REPEAT_KICK_IN], 0

.delay_not_over:
    jmp .return

.no_delay:

    ; === REPEAT PHASE ===
    mov dx, word [bp+6]  ; delta_time
    sub word [bx+GAME_STATE_OFF_TIMER_REPEAT], dx
    jg  .repeat_timer_not_over

    add word [bx+GAME_STATE_OFF_TIMER_REPEAT], CONSTANT_AUTOREPEAT_TIME

    cmp ax, GAME_STATE_FLAGS_MASK_DIR_RIGHT
    je  .repeat_right
.repeat_left:
    _state_machine_extended_placement_before
    _state_machine_try_move 0, -1
    _state_machine_extended_placement_after
    jmp .repeat_endif
.repeat_right:
    _state_machine_extended_placement_before
    _state_machine_try_move 0, 1
    _state_machine_extended_placement_after
.repeat_endif:

.repeat_timer_not_over:

.return:

    pop di
    pop si
    pop bx
    pop bp
    ret

; word advance_game_state__line_clears(Matrix *m)
advance_game_state__line_clears:
    push bp
    mov  bp, sp
    push si
    push di
    push bx

    mov bx, word [bp+4]  ; offset in matrix
    xor si, si           ; number of cleared lines

    xor di, di           ; line number
.clears_loop:
    push MATRIX_W
    push MINO_NONE
    push bx
    call memchr
    add sp, 6
    test ax, ax
    jnz  .not_a_clear
    inc  si
    jmp  .remove_line
.not_a_clear:

    test si, si
    jz   .skip_move_down
    test di, di
    jz   .skip_move_down

    mov ax, si
    mov cl, MATRIX_W
    mul cl
    neg ax
    add ax, bx

.remove_line:
    push MATRIX_W
    push bx
    push ax
    call memcpy
    add sp, 6

    push MATRIX_W
    push MINO_NONE
    push bx
    call memset
    add sp, 6

.skip_move_down:

    add bx, MATRIX_W
    inc di
    cmp di, MATRIX_H
    jl  .clears_loop
.clears_loop_end:

.done:
    mov ax, si

    pop bx
    pop di
    pop si
    pop bp
    ret


_state_machine_minos: times 2*4 dw 0
; bool advance_game_state__detect_lock_out(Tetrimino *t)
advance_game_state__detect_lock_out:
    push bp
    mov  bp, sp
    push si
    push di
    push bx

    mov si, word [bp+4]

    push _state_machine_minos
    push si
    call tetrimino_get_coords
    add sp, 4

    xor ax, ax
    cmp word [_state_machine_minos+(4*0)+0], VISIBLE_MATRIX_H
    jl  .return
    cmp word [_state_machine_minos+(4*1)+0], VISIBLE_MATRIX_H
    jl  .return
    cmp word [_state_machine_minos+(4*2)+0], VISIBLE_MATRIX_H
    jl  .return
    cmp word [_state_machine_minos+(4*3)+0], VISIBLE_MATRIX_H
    jl  .return

    mov ax, 1

.return:
    ;add sp, @@@
    pop bx
    pop di
    pop si
    pop bp
    ret