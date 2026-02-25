tetrimino_bag: db MINO_L, MINO_J, MINO_S, MINO_Z, MINO_T, MINO_O, MINO_I
tetrimino_bag_index: dw MINO_COUNT

; Mino next_mino(void)
next_mino:
    push si

    cmp word [tetrimino_bag_index], MINO_COUNT
    jb  .skip_shuffle

    push MINO_COUNT
    push tetrimino_bag
    call prng_shuffle_bytes
    add sp, 4

    mov word [tetrimino_bag_index], 0

.skip_shuffle:
    mov si, word [tetrimino_bag_index]
    inc word [tetrimino_bag_index]
    xor ax, ax
    mov al, byte [tetrimino_bag+si]

    pop si
    ret

; void initialize_tetrimino(Mino type, Tetrimino *out)
initialize_tetrimino:
    push bp
    mov  bp, sp

    ; ax = &(tetrimino_templates[type-1])
    mov ax, word [bp+4]
    dec ax
    mov cx, TETRIMINO_SIZE
    mul cx
    add ax, tetrimino_templates

    push TETRIMINO_SIZE
    push ax
    push word [bp+6]
    call memcpy
    add sp, 6

    pop bp
    ret

; WARNING: depends on the values of MINO_X macros
tetrimino_templates:  ; | type | size | rotation | root | minos
tetrimino_templates_l: db 01h,   3,     0,         19,3,  1,0, 1,1, 1,2, 2,2
tetrimino_templates_j: db 02h,   3,     0,         19,3,  1,0, 1,1, 1,2, 2,0
tetrimino_templates_s: db 03h,   3,     0,         19,3,  1,0, 1,1, 2,1, 2,2
tetrimino_templates_z: db 04h,   3,     0,         19,3,  2,0, 2,1, 1,1, 1,2
tetrimino_templates_t: db 05h,   3,     0,         19,3,  1,0, 1,1, 1,2, 2,1
tetrimino_templates_o: db 06h,   3,     0,         19,3,  1,1, 2,1, 1,2, 2,2
tetrimino_templates_i: db 07h,   4,     0,         18,3,  2,0, 2,1, 2,2, 2,3
