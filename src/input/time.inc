; WARNING: there is a bug in this function, where sometimes time decreases. Don't use it.
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
    mov bl, al
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

    mov ax, bx
    not ax      ; Make PIT count up instead of down

.done:
    pop bx
    pop di
    pop si
    pop bp
    ret


_TIME_DELTA_LAST_LO EQU word [bp+4]
_TIME_DELTA_LAST_HI EQU word [bp+6]
_TIME_DELTA_CURR_LO EQU word [bp+8]
_TIME_DELTA_CURR_HI EQU word [bp+10]
; word get_time_delta(word[4] *buffer)
; Gets delta from last call with same arg, in hundredths of a second.
; When called more a minute apart, result will be garbage.
get_time_delta:
    push bp
    mov  bp, sp
    push bx

    mov bx, word [bp+4]

    ; arg.last = arg.curr
    mov ax, _TIME_DELTA_CURR_LO
    mov _TIME_DELTA_LAST_LO, ax
    mov ax, _TIME_DELTA_CURR_HI
    mov _TIME_DELTA_LAST_HI, ax

    ; arg.curr = curr_time()
    mov ax, 2C00h
    int 21h
    mov _TIME_DELTA_CURR_LO, dx
    mov _TIME_DELTA_CURR_HI, cx

    ; dl = arg.curr.hs (already true)
    ; dh = arg.curr.s  (already true)
    ; cl = arg.last.hs
    ; ch = arg.last.s
    mov cx, _TIME_DELTA_LAST_LO

    ; dl = arg.curr.hs - arg.last.hs
    ; dh = arg.curr.s  - arg.last.s
    sub dl, cl
    sub dh, ch

    ; Account for overflow
    cmp dl, 0
    jge .no_hs_overflow
.hs_overflow:
    add dl, 100
    dec dh
.no_hs_overflow:

    cmp dh, 0
    jge .no_s_overflow
.s_overflow:
    add dh, 60
.no_s_overflow:

    ; return 100*s + hs
    xor ax, ax
    mov cl, 100
    mov al, dh
    mul cl
    xor cx, cx
    mov cl, dl
    add ax, cx

.done:
    ;add sp, @@@
    pop bx
    pop bp
    ret
