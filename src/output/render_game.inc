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

HOLD_MATRIX_ORIGIN_X EQU 70
HOLD_MATRIX_ORIGIN_Y EQU 50

GRID_DOT_COLOR EQU 014h

BORDER_WIDTH EQU 1
BORDER_COLOR EQU 0Fh  ; white

MARATHON_LINES_TOP    EQU 130
MARATHON_LINES_MIDDLE EQU 70

SECONDS_TOP    EQU 90
SECONDS_MIDDLE EQU 70

; void draw_rectangle(byte color, word left_x, word top_y, word width, word height)
draw_rectangle:
    push bp
    mov  bp, sp
    push bx
    push si
    push di

    ; setup registers for interrupt
    mov al, byte [bp+4]  ; color
    mov cx, word [bp+6]  ; x
    mov dx, word [bp+8]  ; y
    mov bx, 0            ; page
    mov ah, 0Ch          ; function

    mov di, word [bp+12]  ; height
.loop_rows:
    cmp di, 0
    jle .loop_rows_end

    mov si, word [bp+10]  ; width
.loop_cols:
    cmp si, 0
    jle .loop_cols_end

    int 10h  ; draw graphics pixel

    inc cx
    dec si
    jmp .loop_cols
.loop_cols_end:

    mov cx, word [bp+6]
    inc dx
    dec di
    jmp .loop_rows
.loop_rows_end:

    pop di
    pop si
    pop bx
    pop bp
    ret

;              NONE L    J    S    Z    T    O    I    GHOST DEAD
mino_colors db 00h, 2Ah, 37h, 2Fh, 28h, 23h, 0Eh, 0Bh, 14h,  17h

; void draw_mino(Mino mino, word row, word col, word matrix_left_x, word matrix_bottom_y, bool draw_dot)
draw_mino:
    push bp
    mov  bp, sp
    push bx
    push si
    push di

    push BLOCK_SIZE  ; height
    push BLOCK_SIZE  ; width

    ; top y
    mov ax, word [bp+6]  ; row
    inc ax
    mov cx, BLOCK_SIZE
    mul cx
    neg ax
    add ax, word [bp+12]  ; matrix_bottom_y
    push ax
    mov si, ax

    ; left x
    mov ax, word [bp+8]  ; col
    mov cx, BLOCK_SIZE
    mul cx
    add ax, word [bp+10]  ; matrix_left_x
    push ax
    mov di, ax

    ; color
    mov bx, word [bp+4]  ; mino
    xor ax, ax
    mov al, byte [mino_colors+bx]
    push ax

    call draw_rectangle
    add sp, 10

    ; draw dot
    test word [bp+14], 0FFFFh
    jz   .no_dot
    mov cx, di
    mov dx, si
    mov bx, 0
    mov al, GRID_DOT_COLOR
    mov ah, 0Ch
    int 10h

.no_dot:

    pop di
    pop si
    pop bx
    pop bp
    ret

; void draw_matrix_mino(Mino mino, word row, word col)
draw_matrix_mino:
    push bp
    mov  bp, sp
    push bx
    push si
    push di

    cmp word [bp+4], MINO_NONE
    jne .no_grid_dot
    cmp word [bp+6], VISIBLE_MATRIX_H-1
    je  .no_grid_dot
    cmp word [bp+8], 0
    je  .no_grid_dot
.grid_dot:
    push 1
    jmp .endif_grid_dot
.no_grid_dot:
    push 0
.endif_grid_dot:

    push MATRIX_ORIGIN_Y
    push MATRIX_ORIGIN_X
    push word [bp+8]
    push word [bp+6]
    push word [bp+4]
    call draw_mino
    add sp, 12

.cleanup:
    pop di
    pop si
    pop bx
    pop bp
    ret


_dt_coords_buffer: times 2*4 dw 0
; void draw_tetrimino(Tetrimino *t, word matrix_left_x, word matrix_bottom_y)
draw_tetrimino:
    push bp
    mov  bp, sp
    push si

    ; load absolute coords into buffer
    push _dt_coords_buffer
    push word [bp+4]
    call tetrimino_get_coords
    add  sp, 4

    ; cx = type
    xor cx, cx
    mov si, [bp+4]
    mov cl, byte [si+TETRIMINO_OFF_TYPE]

    ; push initial
    push 0               ; [bp-4]  = draw dot
    push word [bp+8]     ; [bp-6]  = matrix_bottom_y
    push word [bp+6]     ; [bp-8]  = matrix_left_x
    push 0               ; [bp-10] = col
    push 0               ; [bp-12] = row
    push cx              ; [bp-14] = type

macro _dt_single index {
    mov ax, word [_dt_coords_buffer+(index*4)+0]
    mov word [bp-12], ax
    mov ax, word [_dt_coords_buffer+(index*4)+2]
    mov word [bp-10], ax
    call draw_mino
}
    _dt_single 0
    _dt_single 1
    _dt_single 2
    _dt_single 3

    add sp, 12

.done:
    pop si
    pop bp
    ret


; void delete_tetrimino(Tetrimino *t, word matrix_left_x, word matrix_bottom_y)
delete_tetrimino:
    push bp
    mov  bp, sp
    push bx
    push si

    mov si, [bp+4]
    mov bl, byte [si+TETRIMINO_OFF_TYPE]
    mov byte [si+TETRIMINO_OFF_TYPE], MINO_NONE

    push word [bp+8]
    push word [bp+6]
    push word [bp+4]
    call draw_tetrimino
    add  sp, 6

    mov byte [si+TETRIMINO_OFF_TYPE], bl

    pop si
    pop bx
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
    push bx
    push si

    ; === TOP ===
    ; height
    push word [bp+6]      ; border_width
    ; width
    mov ax, word [bp+12]  ; obj_right
    sub ax, word [bp+14]  ; obj_left
    add ax, word [bp+6]   ; border_width
    add ax, word [bp+6]
    push ax
    ; top y
    mov ax, word [bp+8]   ; obj_top
    sub ax, word [bp+6]   ; border_width
    push ax
    ; left x
    mov ax, word [bp+14]  ; obj_left
    sub ax, word [bp+6]   ; border_width
    push ax
    ; color
    push word [bp+4]

    call draw_rectangle
    add sp, 10

    ; === BOTTOM ===
    ; height
    push word [bp+6]      ; border_width
    ; width
    mov ax, word [bp+12]  ; obj_right
    sub ax, word [bp+14]  ; obj_left
    add ax, word [bp+6]   ; border_width
    add ax, word [bp+6]
    push ax
    ; top y
    push word [bp+10]     ; obj_bottom
    ; left x
    mov ax, word [bp+14]  ; obj_left
    sub ax, word [bp+6]   ; border_width
    push ax
    ; color
    push word [bp+4]

    call draw_rectangle
    add sp, 10

    ; === LEFT ===
    ; height
    mov ax, word [bp+10]  ; obj_bottom
    sub ax, word [bp+8]   ; obj_top
    add ax, word [bp+6]   ; border_width
    add ax, word [bp+6]
    push ax
    ; width
    push word [bp+6]      ; border_width
    ; top y
    mov ax, word [bp+8]   ; obj_top
    sub ax, word [bp+6]   ; border_width
    push ax
    ; left x
    mov ax, word [bp+14]  ; obj_left
    sub ax, word [bp+6]   ; border_width
    push ax
    ; color
    push word [bp+4]

    call draw_rectangle
    add sp, 10

    ; === RIGHT ===
    ; height
    mov ax, word [bp+10]  ; obj_bottom
    sub ax, word [bp+8]   ; obj_top
    add ax, word [bp+6]   ; border_width
    add ax, word [bp+6]
    push ax
    ; width
    push word [bp+6]      ; border_width
    ; top y
    mov ax, word [bp+8]   ; obj_top
    sub ax, word [bp+6]   ; border_width
    push ax
    ; left x
    push word [bp+12]     ; obj_right
    ; color
    push word [bp+4]

    call draw_rectangle
    add sp, 10

    pop si
    pop bx
    pop bp
    ret

marathon_lines_text_1: db "  lines  ", 0
marathon_lines_text_2: db "remaining", 0
seconds_text: db "seconds", 0
draw_game_skeleton:
    push word MATRIX_LEFT
    push word MATRIX_RIGHT
    push word MATRIX_BOTTOM
    push word MATRIX_TOP
    push word BORDER_WIDTH
    push word BORDER_COLOR
    call draw_border
    add sp, 12

    push MARATHON_LINES_TOP+10
    push MARATHON_LINES_MIDDLE-(8*9/2)
    push 18h
    push marathon_lines_text_1
    call render_text
    add sp, 8
    push MARATHON_LINES_TOP+20
    push MARATHON_LINES_MIDDLE-(8*9/2)
    push 18h
    push marathon_lines_text_2
    call render_text
    add sp, 8

    push SECONDS_TOP+10
    push SECONDS_MIDDLE-(8*7/2)
    push 18h
    push seconds_text
    call render_text
    add sp, 8

    ret

macro _draw_matrix_tetrimino_single index {
    mov ax, word [_draw_matrix_tetrimino_coords_buffer+(index*4)+0]
    mov word [bp-6], ax
    mov ax, word [_draw_matrix_tetrimino_coords_buffer+(index*4)+2]
    mov word [bp-4], ax
    call draw_matrix_mino
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
    pop si
    pop bp
    ret


_dgsd_last_frame: times MATRIX_W*MATRIX_H db 0FFh
_dgsd_curr_frame: times MATRIX_W*MATRIX_H db 0
_dgsd_last_hold: db MINO_NONE
_dgsd_last_marathon_lines: dw 0
_dgsd_last_seconds: dw 0FFFFh
_dgsd_last_next_queue: times 6 db MINO_NONE
_dgsd_minos: times 2*4 dw 0
_dgsd_tetrimino_buffer: times TETRIMINO_SIZE db 0

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

    test word [bx+GAME_STATE_OFF_TIMER_GEN], 0xFFFF
    jnz .skip_tetrimino

macro _dgsd_mino_to_frame index {
    mov ax, word [_dgsd_minos+(index*4)+0]
    mul cl
    add ax, word [_dgsd_minos+(index*4)+2]
    mov si, ax
    mov byte [_dgsd_curr_frame+si], dl
}

    ; Copy ghost tetrimino to matrix
    push TETRIMINO_SIZE
    push word [bx+GAME_STATE_OFF_TETRIMINO]
    push _dgsd_tetrimino_buffer
    call memcpy
    add sp, 6

    push 0
    push -1
    push _dgsd_tetrimino_buffer
    push word [bx+GAME_STATE_OFF_MATRIX]
    mov ax, 1
.ghost_loop:
    call tetrimino_try_move
    test ax, ax
    jnz  .ghost_loop
    add  sp, 8
.ghost_loop_end:

    push _dgsd_minos
    push _dgsd_tetrimino_buffer
    call tetrimino_get_coords
    add sp, 4

    mov si, _dgsd_tetrimino_buffer
    mov dl, MINO_GHOST
    mov cl, MATRIX_W

    _dgsd_mino_to_frame 0
    _dgsd_mino_to_frame 1
    _dgsd_mino_to_frame 2
    _dgsd_mino_to_frame 3

    ; Copy tetrimino to matrix
    push _dgsd_minos
    push word [bx+GAME_STATE_OFF_TETRIMINO]
    call tetrimino_get_coords
    add sp, 4

    mov si, word [bx+GAME_STATE_OFF_TETRIMINO]
    mov dl, byte [si+TETRIMINO_OFF_TYPE]
    mov cl, MATRIX_W
    _dgsd_mino_to_frame 0
    _dgsd_mino_to_frame 1
    _dgsd_mino_to_frame 2
    _dgsd_mino_to_frame 3

.skip_tetrimino:

    ; Color grey when dead
    test word [bx+GAME_STATE_OFF_FLAGS], GAME_STATE_FLAGS_MASK_GAME_OVER
    jz   .no_game_over
    xor si, si
.loop_game_over:
    cmp si, VISIBLE_MATRIX_H*MATRIX_W
    jge .loop_game_over_end

    cmp byte [_dgsd_curr_frame+si], MINO_NONE
    je  .no_game_over_mino
    mov byte [_dgsd_curr_frame+si], MINO_DEAD
.no_game_over_mino:

    inc si
    jmp .loop_game_over
.loop_game_over_end:
.no_game_over:

    ; Draw diff between last and curr
    xor si, si
.loop_diff:
    cmp si, VISIBLE_MATRIX_H*MATRIX_W
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
    call draw_matrix_mino
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

    ; Update held tetrimino
    xor ax, ax
    push HOLD_MATRIX_ORIGIN_Y
    push HOLD_MATRIX_ORIGIN_X
    mov al, byte [bx+GAME_STATE_OFF_HOLD]
    push ax
    mov al, byte [_dgsd_last_hold]
    push ax
    call draw_game_state_diff__tetrimino
    add sp, 8

    mov al, byte [bx+GAME_STATE_OFF_HOLD]
    mov byte [_dgsd_last_hold], al

    ; Update next queue
    xor ax, ax
macro _dgsd_next_queue index {
    push 50 + (BLOCK_SIZE*index*3)
    push 220
    mov al, byte [bx+GAME_STATE_OFF_NEXT_QUEUE+index]

    ; TODO dynamic when CONSTANT_NEXT_QUEUE_RENDERED is settable
    if index >= CONSTANT_NEXT_QUEUE_RENDERED
        mov al, 0
    end if

    push ax
    mov si, ax
    mov al, byte [_dgsd_last_next_queue+index]
    push ax
    call draw_game_state_diff__tetrimino
    add sp, 8
    mov ax, si
    mov byte [_dgsd_last_next_queue+index], al
}

    _dgsd_next_queue 0
    _dgsd_next_queue 1
    _dgsd_next_queue 2
    _dgsd_next_queue 3
    _dgsd_next_queue 4
    _dgsd_next_queue 5

    ; Marathon lines
    push MARATHON_LINES_TOP
    push MARATHON_LINES_MIDDLE
    push 0
    push word [bx+GAME_STATE_OFF_MARATHON_LINES]
    push 0
    push word [_dgsd_last_marathon_lines]
    call draw_game_state_diff__decimal
    add sp, 12

    mov ax, word [bx+GAME_STATE_OFF_MARATHON_LINES]
    mov word [_dgsd_last_marathon_lines], ax

    ; Seconds timer
    mov ax, word [bx+GAME_STATE_OFF_TIMER+0]
    mov dx, word [bx+GAME_STATE_OFF_TIMER+2]
    and dx, 3Fh  ; avoid divide error
    mov cx, 100
    div cx

    mov cx, word [_dgsd_last_seconds]
    mov word [_dgsd_last_seconds], ax

    push SECONDS_TOP
    push SECONDS_MIDDLE
    push 0
    push ax
    push 0
    push cx
    call draw_game_state_diff__decimal
    add sp, 12

    pop si
    pop bx
    pop bp
    ret

; void draw_game_state_diff__tetrimino(Mino old, Mino new, word matrix_left_x, word matrix_bottom_y)
draw_game_state_diff__tetrimino:
    push bp
    mov  bp, sp
    push si
    push di
    push bx

    xor ax, ax
    mov al, byte [bp+4] ; old
    cmp al, byte [bp+6] ; new
    je  .done

    cmp byte [bp+4], MINO_NONE
    je  .skip_delete

    push _dgsd_tetrimino_buffer
    push ax
    call initialize_tetrimino
    add sp, 4
    mov byte [_dgsd_tetrimino_buffer+TETRIMINO_OFF_ROOT+0], 0
    mov byte [_dgsd_tetrimino_buffer+TETRIMINO_OFF_ROOT+1], 0

    push word [bp+10]
    push word [bp+8]
    push _dgsd_tetrimino_buffer
    call delete_tetrimino
    add sp, 6
.skip_delete:

    xor ax, ax
    mov al, byte [bp+6]
    push _dgsd_tetrimino_buffer
    push ax
    call initialize_tetrimino
    add sp, 4
    mov byte [_dgsd_tetrimino_buffer+TETRIMINO_OFF_ROOT+0], 0
    mov byte [_dgsd_tetrimino_buffer+TETRIMINO_OFF_ROOT+1], 0

    push word [bp+10]
    push word [bp+8]
    push _dgsd_tetrimino_buffer
    call draw_tetrimino
    add sp, 6

    mov al, byte [bx+GAME_STATE_OFF_HOLD]
    mov byte [_dgsd_last_hold], al

.done:
    pop bx
    pop di
    pop si
    pop bp
    ret

_dgsd_str_buffer: times 11 db 0
; void draw_game_state_diff__decimal(dword old, dword new, word middle_x, word top_y)
draw_game_state_diff__decimal:
    push bp
    mov  bp, sp
    push si
    push di
    push bx

    mov ax, word [bp+4]
    cmp ax, word [bp+8]
    jne .changed
    mov ax, word [bp+6]
    cmp ax, word [bp+10]
    jne .changed

    jmp .unchanged
.changed:

    push _dgsd_str_buffer
    push word [bp+6]
    push word [bp+4]
    call dword_to_decimal
    add sp, 6
    push _dgsd_str_buffer
    call handle_leading_zeroes
    add sp, 2

    ; Calculate left_x from middle_x and leading_zeroes
    neg ax
    shl ax, 2
    sub ax, 40
    add ax, word [bp+12]

    push word [bp+14]  ; top_y
    push ax
    push 00h
    push _dgsd_str_buffer
    call render_text
    add sp, 8

    push _dgsd_str_buffer
    push word [bp+10]
    push word [bp+8]
    call dword_to_decimal
    add sp, 6
    push _dgsd_str_buffer
    call handle_leading_zeroes
    add sp, 2

    ; Calculate left_x from middle_x and leading_zeroes
    neg ax
    shl ax, 2
    sub ax, 40
    add ax, word [bp+12]

    push word [bp+14]  ; top_y
    push ax
    push 0Fh
    push _dgsd_str_buffer
    call render_text
    add sp, 8

.unchanged:

.done:
    pop bx
    pop di
    pop si
    pop bp
    ret