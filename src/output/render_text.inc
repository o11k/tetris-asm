include 'font8x8_basic.inc'

; how to set a pixel:
; ah=0Ch
; al=color
; cx=x
; dx=y
; bx=0 (page)


; void render_text(byte *text, word color, word x_left, word y_top)
; 8086 assembly, cdecl
render_text:
    push bp
    mov  bp, sp
    push si
    push di
    push bx

    ; Params: (bp+4)=text ptr, (bp+6)=color, (bp+8)=x_left, (bp+10)=y_top
    mov cx, word [bp+8]  ; x
    mov dx, word [bp+10] ; y

    sub sp, 6
    ; locals:
    ; [bp-8]  ; row
    ; [bp-10] ; col
    ; [bp-12] ; cur text ptr
    mov ax, [bp+4]
    mov [bp-12], ax

    ; while (*text)
.loop_text:
    mov si, word [bp-12]
    cmp byte [si], 0
    je .loop_text_end

    ; for (row=0; row<8; row++)
    mov word [bp-8], 0
.loop_row:
    cmp word [bp-8], 8
    je .loop_row_end

    ; di = cur_line = *((*text)*8 + row + font8x8_basic)
    xor ax, ax
    mov di, word [bp-12]
    mov al, byte [di]
    mov di, ax
    shl di, 3
    add di, word [bp-8]
    add di, word font8x8_basic
    mov al, byte [di]
    mov di, ax

    ; for (col=0; col<8; col++)
    mov word [bp-10], 0
.loop_col:
    cmp word [bp-10], 8
    je .loop_col_end

    ; if (pixel is on)
    test di, 1
    je .skip_pixel

    ; draw pixel
    mov ah, 0Ch
    mov al, byte [bp+6]  ; color
    ; mov cx, x
    ; mov dx, y
    mov bx, 0  ; page
    int 10h

.skip_pixel:
    shr di, 1
    inc cx

    inc word [bp-10]
    jmp .loop_col
.loop_col_end:

    sub cx, 8
    inc dx

    inc word [bp-8]
    jmp .loop_row
.loop_row_end:

    add cx, 8
    sub dx, 8
    inc word [bp-12]  ; text++
    jmp .loop_text
.loop_text_end:

.done:
    add sp, 6
    pop bx
    pop di
    pop si
    pop bp
    ret
