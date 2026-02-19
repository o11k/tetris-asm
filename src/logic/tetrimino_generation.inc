tetrimino_bag: db MINO_L, MINO_J, MINO_S, MINO_Z, MINO_T, MINO_O, MINO_I
tetrimino_bag_index: dw MINO_COUNT

; MINO next_tetrimino(void)
next_tetrimino:
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
