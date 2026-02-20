macro _tetrimino_get_coords_single index {
    mov al, byte [bx+TETRIMINO_OFF_ROOT +0]
    add al, byte [bx+TETRIMINO_OFF_MINOS+0+(index*2)]
    mov ah, byte [bx+TETRIMINO_OFF_ROOT +1]
    add ah, byte [bx+TETRIMINO_OFF_MINOS+1+(index*2)]

    xor  cx, cx

    mov  cl, al
    mov  word [si+0], cx
    mov  cl, ah
    mov  word [si+2], cx
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


macro _tetrimino_detect_collision_single index {
    mov ax, word [_tetrimino_detect_collision_coords_buffer+(index*4)+0]
    mov word [bp-4], ax
    mov ax, word [_tetrimino_detect_collision_coords_buffer+(index*4)+2]
    mov word [bp-2], ax
    call mino_detect_collision
    test ax, ax
    jnz .found
}
_tetrimino_detect_collision_coords_buffer: times 2*4 dw 0
; bool tetrimino_detect_collision(Matrix *m, Tetrimino *t)
tetrimino_detect_collision:
    push bp
    mov  bp, sp

    push _tetrimino_detect_collision_coords_buffer
    push word [bp+6]
    call tetrimino_get_coords
    add  sp, 4

    push 0  ; [bp-2] = col
    push 0  ; [bp-4] = row
    push word [bp+4]  ; matrix

    _tetrimino_detect_collision_single 0
    _tetrimino_detect_collision_single 1
    _tetrimino_detect_collision_single 2
    _tetrimino_detect_collision_single 3

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
