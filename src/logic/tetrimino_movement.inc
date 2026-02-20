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
