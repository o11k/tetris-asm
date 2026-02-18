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
    push bx
    push si

    ; si = scan code (without prefixes)
    in  al, 60h
    mov si, ax
    and si, 07fh
    
    ; bl = is key down (1=down, 0=up)
    mov bl, al
    and bl, 80h
    shr bl, 7
    xor bl, 1

    ; al = old key state
    mov al, byte [keyboard_state_system+si]

    ; set al&2 to 1 if newly-pressed
    test bl, 1
    jz   .not_newly_pressed
    test al, 1
    jnz  .not_newly_pressed
    or   al, 2
.not_newly_pressed:

    ; set al&1 to 1=down 0=up
    and ax, 0FFFEh
    or al, bl

    ; store updated state
    mov byte [keyboard_state_system+si], al

.done:
    pop si
    pop bx
    pop ax
    jmp far [old_int_09h]  ; chain old handler


; byte* __cdecl get_keyboard_state(void)
; returns a ptr to a byte[128], where each byte represents the state of one key.
; the indexing is based on IRQ1 scan codes (see keyboard_tables.inc).
; result[i] & 1 is whether key i is currently down.
; result[i] & 2 is whether key i was pressed (went from up to down) since the last call to this function.
get_keyboard_state:
    push si

    cli
    ; for (si=127; si!=0; si--)
    mov si, 128
.loop_key:
    dec  si
    test si, si
    jz   .loop_key_end

    mov al, byte [keyboard_state_system+si]
    mov byte [keyboard_state_user+si], al
    and byte [keyboard_state_system+si], 0FDh  ; unset "pressed" flag

    jmp .loop_key
.loop_key_end:
    sti

    mov ax, keyboard_state_user
.done:
    pop si
    ret

keyboard_state_user:   db 0 dup 128
keyboard_state_system: db 0 dup 128

include "keyboard_tables.inc"
