old_int_09h: dd 0

register_int_09h:
    push ds
    push es
    push bx

    cli
    mov ax, cs
    mov ds, ax     ; ensure current page stored in ds

    mov ax, 3509h  ; get old int 09h
    int 21h
    mov word [old_int_09h+0], bx
    mov word [old_int_09h+2], es

    mov dx, new_int_09h
    mov ax, 2509h  ; set new int 09h
    int 21h
    sti

    pop bx
    pop es
    pop ds
    ret

restore_int_09h:
    push ds

    cli
    mov dx, [old_int_09h+0]
    mov ds, [old_int_09h+2]
    mov ax, 2509h  ; set old int 09h
    int 21h
    sti

    pop ds
    ret

int_09h_scan_code: dw 0

new_int_09h:
    push ax

    in  al, 60h  ; read scan code
    mov byte [int_09h_scan_code], al

.done:
    pop ax
    jmp far [old_int_09h]  ; chain old handler

include "keyboard_tables.inc"
