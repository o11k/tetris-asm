; word __cdecl get_bios_tick(void)
get_bios_tick:
    push ds
    push bx

    mov ax, 0000h
    mov ds, ax
    mov bx, 046Ch
    mov ax, [bx]

.done:
    pop bx
    pop ds
    ret


; void __cdecl word_to_hex(word w, byte *buffer)
word_to_hex_table: db "0123456789ABCDEF", 0
word_to_hex:
    push bp
    mov bp, sp
    push bx
    push si

    mov ax, [bp+4]
    mov bx, [bp+6]

    mov si, ax
    and si, 0fh
    mov cl, byte [word_to_hex_table+si]
    mov byte [bx+3], cl
    shr ax, 4

    mov si, ax
    and si, 0fh
    mov cl, byte [word_to_hex_table+si]
    mov byte [bx+2], cl
    shr ax, 4

    mov si, ax
    and si, 0fh
    mov cl, byte [word_to_hex_table+si]
    mov byte [bx+1], cl
    shr ax, 4

    mov si, ax
    and si, 0fh
    mov cl, byte [word_to_hex_table+si]
    mov byte [bx+0], cl
    shr ax, 4

    mov byte [bx+4], 0

.done:
    pop si
    pop bx
    pop bp
    ret
