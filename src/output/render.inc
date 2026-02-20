MATRIX_H EQU 40
VISIBLE_MATRIX_H EQU 20
MATRIX_W EQU 10

SCREEN_W EQU 320
SCREEN_H EQU 200
BLOCK_SIZE EQU 9
MATRIX_ORIGIN_X EQU (SCREEN_W-(BLOCK_SIZE*MATRIX_W))/2
MATRIX_ORIGIN_Y EQU SCREEN_H - (SCREEN_H-(BLOCK_SIZE*VISIBLE_MATRIX_H))/2

MATRIX_BOTTOM EQU MATRIX_ORIGIN_Y
MATRIX_TOP    EQU MATRIX_ORIGIN_Y - (BLOCK_SIZE*VISIBLE_MATRIX_H)
MATRIX_LEFT   EQU MATRIX_ORIGIN_X
MATRIX_RIGHT  EQU MATRIX_ORIGIN_X + (BLOCK_SIZE*MATRIX_W)

BORDER_WIDTH EQU 1
BORDER_COLOR EQU 0Fh  ; white

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


; void draw_border(
;    word color,
;    word border_width,
;    word obj_top,     ; inclusive
;    word obj_bottom,  ; exclusive
;    word obj_right,   ; exclusive
;    word obj_left     ; inclusive
; )
draw_border:
    push bp
    mov  bp, sp
    push si
    push bx

    ; setup registers for write-pixel interrupt
    mov al, [bp+4]  ; color
    mov bx, 0       ; page
    mov ah, 0Ch

    dec word [bp+10] ; bottom
    inc word [bp+14] ; left

    xor si, si
.loop_border_width:
    cmp si, [bp+6] ; width
    jnl .loop_border_width_end

    dec word [bp+8]  ; top
    inc word [bp+10] ; bottom
    inc word [bp+12] ; right
    dec word [bp+14] ; left

    mov dx, [bp+8] ; top
.loop_vertical:
    cmp dx, [bp+10] ; bottom
    jg  .loop_vertical_end

    mov cx, [bp+12] ; right
    int 10h
    mov cx, [bp+14] ; left
    int 10h

    inc dx
    jmp .loop_vertical
.loop_vertical_end:

mov cx, [bp+14] ; left
.loop_horizontal:
    cmp cx, [bp+12] ; right
    jg  .loop_horizontal_end

    mov dx, [bp+8]  ; top
    int 10h
    mov dx, [bp+10] ; bottom
    int 10h

    inc cx
    jmp .loop_horizontal
.loop_horizontal_end:

    inc si
    jmp .loop_border_width
.loop_border_width_end:

    pop bx
    pop si
    pop bp
    ret

draw_borders:
    push word MATRIX_LEFT
    push word MATRIX_RIGHT
    push word MATRIX_BOTTOM
    push word MATRIX_TOP
    push word BORDER_WIDTH
    push word BORDER_COLOR
    call draw_border
    add sp, 12
    ret

macro _draw_matrix_tetrimino_single index {
    mov ax, word [_draw_matrix_tetrimino_coords_buffer+(index*4)+0]
    mov word [bp-6], ax
    mov ax, word [_draw_matrix_tetrimino_coords_buffer+(index*4)+2]
    mov word [bp-4], ax
    call draw_matrix_block
}
_draw_matrix_tetrimino_coords_buffer: times 2*4 dw 0
; void draw_matrix_tetrimino(Tetrimino *t)
draw_matrix_tetrimino:
    push bp
    mov  bp, sp
    push si

    ; load absolute coords into buffer
    push _draw_matrix_tetrimino_coords_buffer
    push word [bp+4]
    call tetrimino_get_coords
    add  sp, 4

    ; cx = type
    xor cx, cx
    mov si, [bp+4]
    mov cl, byte [si+TETRIMINO_OFF_TYPE]

    ; push initial
    push 0  ; [bp-4] = col
    push 0  ; [bp-6] = row
    push cx ; type

    _draw_matrix_tetrimino_single 0
    _draw_matrix_tetrimino_single 1
    _draw_matrix_tetrimino_single 2
    _draw_matrix_tetrimino_single 3
    
    add sp, 6

.done:
    ;add sp, @@@
    pop si
    pop bp
    ret


; void delete_matrix_tetrimino(Tetrimino *t)
delete_matrix_tetrimino:
    push bp
    mov  bp, sp
    push bx
    push si

    mov si, [bp+4]
    mov bl, byte [si+TETRIMINO_OFF_TYPE]
    mov byte [si+TETRIMINO_OFF_TYPE], MINO_NONE

    push word [bp+4]
    call draw_matrix_tetrimino
    add  sp, 2

    mov byte [si+TETRIMINO_OFF_TYPE], bl

    pop si
    pop bx
    pop bp
    ret


_dgsd_last_frame: times MATRIX_W*MATRIX_H db MINO_NONE
_dgsd_curr_frame: times MATRIX_W*MATRIX_H db 0
_dgsd_minos: times 2*4 dw 0

; void draw_game_state_diff(State *state)
draw_game_state_diff:
    push bp
    mov  bp, sp
    push bx
    push si

    mov bx, word [bp+4]

    ; Copy matrix to frame
    push MATRIX_W*MATRIX_H
    push word [bx+GAME_STATE_OFF_MATRIX]
    push word _dgsd_curr_frame
    call memcpy
    add sp, 6

    ; Tetrimino to concrete (row,col)s
    push _dgsd_minos
    push word [bx+GAME_STATE_OFF_TETRIMINO]
    call tetrimino_get_coords
    add sp, 4

    ; dl = mino type
    mov si, word [bx+GAME_STATE_OFF_TETRIMINO]
    mov dl, byte [si+TETRIMINO_OFF_TYPE]

    ; Copy tetrimino to matrix
    mov cl, MATRIX_W
macro _dgsd_mino_to_frame index {
    mov ax, word [_dgsd_minos+(index*4)+0]
    mul cl
    add ax, word [_dgsd_minos+(index*4)+2]
    mov si, ax
    mov byte [_dgsd_curr_frame+si], dl
}
    _dgsd_mino_to_frame 0
    _dgsd_mino_to_frame 1
    _dgsd_mino_to_frame 2
    _dgsd_mino_to_frame 3

    ; Draw diff between last and curr
    xor si, si
.loop_diff:
    cmp si, MATRIX_H*MATRIX_W
    jge .loop_diff_end

    mov al, byte [_dgsd_last_frame+si]
    cmp al, byte [_dgsd_curr_frame+si]
    je  .skip_draw

    ; ax = row, dx = col
    xor dx, dx
    mov ax, si
    mov cx, MATRIX_W
    div cx
    push dx
    push ax
    xor cx, cx
    mov cl, byte [_dgsd_curr_frame+si]
    push cx
    call draw_matrix_block
    add sp, 6
.skip_draw:

    inc si
    jmp .loop_diff
.loop_diff_end:

    ; Copy curr to last
    push MATRIX_W*MATRIX_H
    push _dgsd_curr_frame
    push _dgsd_last_frame
    call memcpy
    add sp, 6

    pop si
    pop bx
    pop bp
    ret