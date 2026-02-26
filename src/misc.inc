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


; void __cdecl dword_to_decimal(dword in, char (*out)[11])
; Use double-dabble
DTD_DIGITS EQU 10
_dtd_bcd_buffer: times DTD_DIGITS db 0
dword_to_decimal:
    push bp
    mov  bp, sp
    push si
    push bx

    push DTD_DIGITS
    push 0
    push _dtd_bcd_buffer
    call memset
    add  sp, 6

    push word [bp+4]    ; [bp-6] = low 16 bits
    push word [bp+6]    ; [bp-8] = high 16 bits

    mov cx, 32  ; process 32 bits
.bit_loop:

    xor ax, ax
    shl  word [bp-6], 1
    rcl  word [bp-8], 1
    rcl  ax, 1

    push cx
    mov cx, DTD_DIGITS
    mov si, _dtd_bcd_buffer
.bcd_loop:

    ; Add 3 if >=5
    cmp byte [si], 5
    jl  .skip_add3
    add byte [si], 3
.skip_add3:

    ; Shift
    shl byte [si], 1
    or  byte [si], al
    mov al, byte [si]
    shr al, 4
    and byte [si], 0Fh

    inc si
    loop .bcd_loop
.bcd_loop_end:
    pop cx

    loop .bit_loop
.bit_loop_end:

    mov bx, word [bp+8] ; out buffer
    add bx, DTD_DIGITS
    mov byte [bx], 0
    dec bx

    mov cx, DTD_DIGITS
    mov si, _dtd_bcd_buffer
.loop_digits:
    mov al, byte [si]
    add al, 30h ; '0'
    mov byte [bx], al

    dec bx
    inc si
    loop .loop_digits
.loop_digits_end:

.done:
    add sp, 4

    pop bx
    pop si
    pop bp
    ret

; word handle_leading_zeroes(char *str)
; Replace leading zeroes with spaces (keep the last one if the entire number is 0). Return number of characters replaced
handle_leading_zeroes:
    push bp
    mov  bp, sp
    push bx

    mov bx, word [bp+4]
    xor ax, ax

    cmp byte [bx], 0
    je  .done

.loop_chars:
    cmp byte [bx], 0
    jne .not_end_of_string
    mov byte [bx-1], '0'
    dec ax
    jmp .loop_chars_end

.not_end_of_string:
    cmp byte [bx], '0'
    jne .loop_chars_end

    mov byte [bx], ' '
    inc ax

    inc bx
    jmp .loop_chars
.loop_chars_end:

.done:
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
