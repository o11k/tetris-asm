macro _tetrimino_get_coords_single index {
    mov cl, byte [bx+TETRIMINO_OFF_ROOT +0]
    add cl, byte [bx+TETRIMINO_OFF_MINOS+0+(index*2)]
    mov ch, byte [bx+TETRIMINO_OFF_ROOT +1]
    add ch, byte [bx+TETRIMINO_OFF_MINOS+1+(index*2)]

    xor ax, ax
    mov al, cl
    cbw
    mov word [si+0], ax

    xor ax, ax
    mov al, ch
    cbw
    mov word [si+2], ax
}
; void tetrimino_get_coords(Tetrimino *t, word[2][4] *buffer)
tetrimino_get_coords:
    push bp
    mov  bp, sp
    push bx
    push si

    mov bx, word [bp+4]
    mov si, word [bp+6]

    _tetrimino_get_coords_single 0
    add si, 2*2
    _tetrimino_get_coords_single 1
    add si, 2*2
    _tetrimino_get_coords_single 2
    add si, 2*2
    _tetrimino_get_coords_single 3

    pop si
    pop bx
    pop bp
    ret


_tdc_coords_buffer: times 2*4 dw 0
; bool tetrimino_detect_collision(Matrix *m, Tetrimino *t)
tetrimino_detect_collision:
    push bp
    mov  bp, sp

    push _tdc_coords_buffer
    push word [bp+6]
    call tetrimino_get_coords
    add  sp, 4

    push 0  ; [bp-2] = col
    push 0  ; [bp-4] = row
    push word [bp+4]  ; matrix

macro _tdc_single index {
    mov ax, word [_tdc_coords_buffer+(index*4)+0]
    mov word [bp-4], ax
    mov ax, word [_tdc_coords_buffer+(index*4)+2]
    mov word [bp-2], ax
    call mino_detect_collision
    test ax, ax
    jnz .found
}
    _tdc_single 0
    _tdc_single 1
    _tdc_single 2
    _tdc_single 3

.not_found:
    mov  ax, 0
    jmp .done
.found:
    mov ax, 1
.done:
    add sp, 6
    pop bp
    ret

; bool mino_detect_collision(Matrix *m, word row, word col)
mino_detect_collision:
    push bp
    mov  bp, sp
    push si

    ; out of bounds (excluding row too big)
    cmp word [bp+6], 0
    jl  .found
    cmp word [bp+8], 0
    jl  .found
    cmp word [bp+8], MATRIX_W
    jge .found

    ; collision with matrix
    mov ax, MATRIX_W
    mul word [bp+6]
    add ax, word [bp+8]
    add ax, word [bp+4]
    mov si, ax

    cmp byte [si], MINO_NONE
    jne .found

.not_found:
    mov  ax, 0
    jmp .done
.found:
    mov ax, 1
.done:
    pop si
    pop bp
    ret


; bool tetrimino_try_move(Matrix *m, Tetrimino *t, word delta_row, word delta_col)
tetrimino_try_move:
    push bp
    mov  bp, sp
    push bx

    ; Move tetrimino
    mov cx, word [bp+8]  ; row
    mov dx, word [bp+10] ; col
    mov bx, word [bp+6]
    add byte [bx+TETRIMINO_OFF_ROOT+0], cl
    add byte [bx+TETRIMINO_OFF_ROOT+1], dl

    ; Detect collision
    push bx
    push word [bp+4]
    call tetrimino_detect_collision
    add sp, 4

    xor  ax, 1  ; collision found = this function is unsuccessful
    test ax, ax
    jnz  .done

    ; If collides, undo move
    mov cx, word [bp+8]  ; row
    mov dx, word [bp+10] ; col
    sub byte [bx+TETRIMINO_OFF_ROOT+0], cl
    sub byte [bx+TETRIMINO_OFF_ROOT+1], dl

.done:
    pop bx
    pop bp
    ret

; bool tetrimino_is_on_ground(Matrix *m, Tetrimino *t)
tetrimino_is_on_ground:
    push bp
    mov  bp, sp
    push bx

    mov bx, word [bp+6]
    dec byte [bx+TETRIMINO_OFF_ROOT+0]

    push bx
    push word [bp+4]
    call tetrimino_detect_collision
    add sp, 4

    inc byte [bx+TETRIMINO_OFF_ROOT+0]

    pop bx
    pop bp
    ret

; TODO: DRY with draw_game_state_diff?
_tld_minos: times 2*4 dw 0
; void tetrimino_lock_down(Matrix *m, Tetrimino *t)
tetrimino_lock_down:
    push bp
    mov  bp, sp
    push bx
    push si

    ; Tetrimino to concrete (row,col)s
    push _tld_minos
    push word [bp+6]  ; tetrimino
    call tetrimino_get_coords
    add sp, 4

    ; dl = mino type
    mov si, word [bp+6]  ; tetrimino
    mov dl, byte [si+TETRIMINO_OFF_TYPE]


    ; Copy tetrimino to matrix
    mov bx, word [bp+4]  ; matrix
    mov cl, MATRIX_W
macro _tld_mino_to_frame index {
    mov ax, word [_tld_minos+(index*4)+0]
    mul cl
    add ax, word [_tld_minos+(index*4)+2]
    mov si, ax
    mov byte [bx+si], dl
}
    _tld_mino_to_frame 0
    _tld_mino_to_frame 1
    _tld_mino_to_frame 2
    _tld_mino_to_frame 3

    pop si
    pop bx
    pop bp
    ret