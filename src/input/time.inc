; word[3] get_accurate_time(void)
; Clock frequency 1.193182 MHz
; Returns:
; CX = BIOS tick high word
; DX = BIOS tick low word
; AX = PIT count
get_accurate_time:
    push bp
    mov  bp, sp
    push si
    push di
    push bx

    cli
.read_loop:

    ; read BIOS tick into si:di
    mov ah, 00h
    int 1Ah
    mov si, cx
    mov di, dx

    ; read PIT count into bx
    mov al, 00h
    out 43h, al
    in  al, 40h
    mov al, bl
    in  al, 40h
    mov bh, al

    ; read BIOS tick again and detect wrap
    mov ah, 00h
    int 1Ah
    cmp si, cx
    jne .read_loop
    cmp di, dx
    jne .read_loop

.read_loop_end:
    sti

    mov ax, bx
    neg ax      ; Make PIT count up instead of down

.done:
    pop bx
    pop di
    pop si
    pop bp
    ret
