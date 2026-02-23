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


; void memcpy(byte *dst, byte *src, word len)
memcpy:
    push bp
    mov  bp, sp
    push si
    push di

    mov di, word [bp+4]
    mov si, word [bp+6]
    mov cx, word [bp+8]

    cld
    rep movsb

    pop di
    pop si
    pop bp
    ret


; void memset(byte *dst, byte val, word len)
memset:
    push bp
    mov  bp, sp
    push di

    mov  di, word [bp+4]   ; dst
    mov  al, byte [bp+6]   ; val
    mov  cx, word [bp+8]   ; len

    cld
    rep stosb              ; fill CX bytes at ES:DI with AL

    pop di
    pop bp
    ret


; void *memchr(const void *ptr, word ch, word length)
; Returns pointer to first occurence of ch in ptr, NULL if not found
memchr:
    push bp
    mov  bp, sp
    push di

    mov di, word [bp+4]  ; ptr
    mov al, byte [bp+6]  ; ch
    mov cx, word [bp+8]  ; length

    test cx, cx
    jz   .not_found

    cld
    repne scasb
    jnz  .not_found

    dec di  ; scasb increments di after match, fix pointer
    mov ax, di
    jmp  .done

.not_found:
    xor ax, ax

.done:
    pop  di
    pop  bp
    ret
