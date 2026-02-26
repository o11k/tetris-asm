prng_state: dw 0

; void prng_seed(word seed)
prng_seed:
    push bp
    mov  bp, sp

    mov ax, word [bp+4]
    or  ax, 1  ; avoid seed=0
    mov word [prng_state], ax

    pop bp
    ret


; word prng_next_word(void)
prng_next_word:
    mov ax, word [prng_state]

    ; xs ^= xs << 7
    mov cx, ax
    shl cx, 7
    xor ax, cx

    ; xs ^= xs >> 9
    mov cx, ax
    shr cx, 9
    xor ax, cx

    ; xs ^= xs << 8
    mov cx, ax
    shl cx, 8
    xor ax, cx

    mov word [prng_state], ax

    ret


; void prng_shuffle_bytes(byte *bytes, word length)
prng_shuffle_bytes:
    push bp
    mov  bp, sp
    push si
    push di
    push bx

    mov bx, word [bp+4]

    ; for si in [0, length) in reverse
    mov si, word [bp+6]
.shuffle_loop:
    dec si
    jl  .shuffle_loop_end

    ; di = random number from [0, si]
    call prng_next_word
    mov  cx, si
    inc  cx
    xor  dx, dx
    mul  cx
    mov  di, dx

    ; swap bytes[si] <-> bytes[di]
    mov al, byte [bx+si]
    mov dl, byte [bx+di]
    mov byte [bx+si], dl
    mov byte [bx+di], al

    jmp .shuffle_loop
.shuffle_loop_end:

    pop bx
    pop di
    pop si
    pop bp
    ret