    org 100h
    use16

MINO_L EQU 00h
MINO_J EQU 01h
MINO_S EQU 02h
MINO_Z EQU 03h
MINO_T EQU 04h
MINO_O EQU 05h
MINO_I EQU 06h

start:
    int3

    ; screen mode 13h
    mov ax, 0013h
    int 10h

    ; draw block
    push 0      ; y
    push 0      ; x
    push MINO_L ; mino
    call draw_block
    add sp, 6

    ; exit 0
    mov ax, 4C00h
    int 21h

BLOCK_SIZE EQU 13
BLOCK_ORIGIN_X EQU 10
BLOCK_ORIGIN_Y EQU 50

mino_colors db 2Ah, 37h, 2Fh, 28h, 23h, 0Eh, 0Bh
draw_block:
    push bp
    mov  bp, sp
    push bx
    push si

    mov si, [bp+4]
    mov al, [mino_colors+si]
    mov cx, [bp+6]
    add cx, BLOCK_ORIGIN_X
    mov dx, [bp+8]
    add dx, BLOCK_ORIGIN_Y
    mov bx, 0
    mov ah, 0Ch

    xor si, si
    xor di, di
.loop1:
    cmp si, BLOCK_SIZE
    jge .loop1_end
    inc si
    sub cx, di
    xor di, di
.loop2:
    cmp di, BLOCK_SIZE
    jge .loop2_end
    inc di
    inc cx
    int 10h
    jmp .loop2
.loop2_end:
    inc dx
    jmp .loop1
.loop1_end:

    pop si
    pop bx
    pop bp
    ret
