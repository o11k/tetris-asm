MATRIX_H EQU 40
VISIBLE_MATRIX_H EQU 20
MATRIX_W EQU 10

SCREEN_W EQU 320
SCREEN_H EQU 200
BLOCK_SIZE EQU 9
MATRIX_ORIGIN_X EQU (SCREEN_W-(BLOCK_SIZE*MATRIX_W))/2
MATRIX_ORIGIN_Y EQU SCREEN_H - (SCREEN_H-(BLOCK_SIZE*VISIBLE_MATRIX_H))/2

mino_colors db 00h, 2Ah, 37h, 2Fh, 28h, 23h, 0Eh, 0Bh
; void draw_block(Mino mino, word left_x, word top_y)
draw_block:
    push bp
    mov  bp, sp
    push bx
    push si
    push di

    ; setup registers for write-pixel interrupt
    mov si, [bp+4]
    mov al, [mino_colors+si] ; color
    mov cx, [bp+6]           ; x
    mov dx, [bp+8]           ; y
    mov bx, 0                ; page
    mov ah, 0Ch

    xor si, si
.loop1:
    cmp si, BLOCK_SIZE
    jnl .loop1_end
    xor di, di
.loop2:
    cmp di, BLOCK_SIZE
    jge .loop2_end
    inc cx  ; x++
    int 10h
    inc di
    jmp .loop2
.loop2_end:
    sub cx, BLOCK_SIZE
    inc dx  ; y++
    inc si
    jmp .loop1
.loop1_end:

    pop di
    pop si
    pop bx
    pop bp
    ret

; void draw_matrix_block(Mino mino, word row, word col)
draw_matrix_block:
    push bp
    mov  bp, sp

    cmp word [bp+4], MINO_NONE
    je  .cleanup

    ;y = origin_y - block_size - block_size*row
    mov ax, BLOCK_SIZE
    mul word [bp+6]
    add ax, BLOCK_SIZE
    neg ax
    add ax, MATRIX_ORIGIN_Y
    push ax
    ;x = origin_x + block_size*col
    mov ax, BLOCK_SIZE
    mul word [bp+8]
    add ax, MATRIX_ORIGIN_X
    push ax
    ;mino
    push word [bp+4]

    call draw_block
    add sp, 6

.cleanup:
    pop bp
    ret

; void draw_matrix(void)
draw_matrix:
    push bp
    mov  bp, sp
    push bx
    push si
    push di

    xor si, si
.loop1:
    cmp si, VISIBLE_MATRIX_H
    jge .loop1_end
    xor di, di
.loop2:
    cmp di, MATRIX_W
    jge .loop2_end

    push di  ; col
    push si  ; row
    ; mino
    mov ax, MATRIX_W
    mul si
    add ax, di
    mov bx, ax
    xor ax, ax
    mov al, [bx+matrix]
    push ax
    call draw_matrix_block
    add sp, 6

    inc di
    jmp .loop2
.loop2_end:
    inc si
    jmp .loop1
.loop1_end:

    pop bx
    pop di
    pop si
    pop bp
    ret
