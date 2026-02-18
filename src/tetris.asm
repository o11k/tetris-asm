    org 100h
    use16

include 'constants.inc'

start:
    push bp
    mov bp, sp

    call register_int_09h

    ; video mode 13h
    mov ax, 0013h
    int 10h

    call draw_matrix

.loop_main:
    call get_controls_state
    mov  si, ax

    ; Esc = exit
    mov  al, byte [si+CONTROLS_STATE_OFF_PAUSE]
    test al, KEYBOARD_STATE_MASK_WAS_PRESSED
    jnz  .loop_main_end

    ; Rotate right
    mov  al, byte [si+CONTROLS_STATE_OFF_ROT_RIGHT]
    test al, KEYBOARD_STATE_MASK_WAS_PRESSED
    jz   .rotate_right_false
.rotate_right:
    inc word [pointer_value]
    and word [pointer_value], 3
.rotate_right_false:

    ; Rotate left
    mov  al, byte [si+CONTROLS_STATE_OFF_ROT_LEFT]
    test al, KEYBOARD_STATE_MASK_WAS_PRESSED
    jz   .rotate_left_false
.rotate_left:
    dec word [pointer_value]
    and word [pointer_value], 3
.rotate_left_false:

    ; Rotate 180 degrees
    mov  al, byte [si+CONTROLS_STATE_OFF_ROT_180]
    test al, KEYBOARD_STATE_MASK_WAS_PRESSED
    jz   .rotate_180_false
.rotate_180:
    add word [pointer_value], 2
    and word [pointer_value], 3
.rotate_180_false:

    ; Move right
    mov  al, byte [si+CONTROLS_STATE_OFF_RIGHT]
    test al, KEYBOARD_STATE_MASK_WAS_PRESSED
    jz   .right_false
.right:
    add word [pointer_pos_x], 8
.right_false:

    ; Move left
    mov  al, byte [si+CONTROLS_STATE_OFF_LEFT]
    test al, KEYBOARD_STATE_MASK_WAS_PRESSED
    jz   .left_false
.left:
    sub word [pointer_pos_x], 8
.left_false:

    ; skip if no inputs
    mov ax, word [pointer_value]
    cmp ax, word [old_pointer_value]
    jne .changed
    mov ax, word [pointer_pos_x]
    cmp ax, word [old_pointer_pos_x]
    jne .changed
    jmp .no_change

.changed:
    ; delete old
    push 20  ;y
    push word [old_pointer_pos_x]
    push 00h
    push pointer_str
    call render_text
    add sp, 8

    ; update values
    mov ax, word [pointer_value]
    mov word [old_pointer_value], ax
    mov ax, word [pointer_pos_x]
    mov word [old_pointer_pos_x], ax

    mov si, [pointer_value]
    mov al, byte [pointer_chars+si]
    mov byte [pointer_str], al

    ; render new
    push 20  ;y
    push word [pointer_pos_x]
    push 0Fh
    push pointer_str
    call render_text
    add sp, 8

.no_change:
    jmp .loop_main
.loop_main_end:

    call restore_int_09h
    ; exit 0
    mov ax, 4C00h
    int 21h

old_pointer_value: dw 1
pointer_value: dw 0
pointer_str: db "^", 0
pointer_chars: db "^>v<", 0
old_pointer_pos_x: dw 21
pointer_pos_x: dw 20

include 'render.inc'
include 'render_text.inc'
include 'keyboard_input.inc'
include 'misc.inc'
include 'time.inc'
include 'controls.inc'

; appears upside-down
matrix db \
    1,1,1,1,1,1,1,1,1,1, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    2,0,0,0,0,0,0,0,0,2, \
    3,3,3,3,3,3,3,3,3,3, \
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
