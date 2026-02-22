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
    push cx
    push si

    ; si = scan code (without prefixes)
    in  al, 60h
    mov si, ax
    and si, 07fh
    
    ; cl will be the final result
    xor cl, cl

    ; cl & KEYBOARD_STATE_MASK_IS_DOWN =1 only if key down event
    test al, KEYBOARD_SCAN_CODE_MASK_KEY_UP
    jnz  .not_key_down
    or   cl, KEYBOARD_STATE_MASK_IS_DOWN
.not_key_down:

    ; cl & KEYBOARD_STATE_MASK_WAS_PRESSED =1 only if already pressed or newly pressed
    ; already was pressed
    mov al, byte [keyboard_state_system+si]
    and al, KEYBOARD_STATE_MASK_WAS_PRESSED
    or  cl, al
    ; newly pressed
    mov  al, byte [keyboard_state_system+si]
    test cl, KEYBOARD_STATE_MASK_IS_DOWN
    jz   .not_newly_pressed
    test al, KEYBOARD_STATE_MASK_IS_DOWN
    jnz  .not_newly_pressed
    or   cl, KEYBOARD_STATE_MASK_WAS_PRESSED
.not_newly_pressed:

    ; cl & KEYBOARD_STATE_MASK_WAS_RELEASED =1 only if already released or newly released
    ; already was released
    mov al, byte [keyboard_state_system+si]
    and al, KEYBOARD_STATE_MASK_WAS_RELEASED
    or  cl, al
    ; newly released
    mov  al, byte [keyboard_state_system+si]
    test cl, KEYBOARD_STATE_MASK_IS_DOWN
    jnz  .not_newly_released
    test al, KEYBOARD_STATE_MASK_IS_DOWN
    jz   .not_newly_released
    or   cl, KEYBOARD_STATE_MASK_WAS_RELEASED
.not_newly_released:

    ; store updated state
    mov byte [keyboard_state_system+si], cl

.done:
    pop si
    pop cx
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
    ; for (si=0; si<128; si++)
    xor si, si
.loop_key:
    cmp  si, 128
    jge  .loop_key_end

    ; copy from system buffer to user buffer
    mov al, byte [keyboard_state_system+si]
    mov byte [keyboard_state_user+si], al
    ; unset "pressed" and "released" flags in system buffer
    mov cl, KEYBOARD_STATE_MASK_WAS_PRESSED
    or  cl, KEYBOARD_STATE_MASK_WAS_RELEASED
    not cl
    and byte [keyboard_state_system+si], cl

    inc si
    jmp .loop_key
.loop_key_end:
    sti

    mov ax, keyboard_state_user
.done:
    pop si
    ret

keyboard_state_user:   times 128 db 0
keyboard_state_system: times 128 db 0

include "keyboard_tables.inc"
