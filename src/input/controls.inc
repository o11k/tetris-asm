controls_state: times CONTROLS_STATE_SIZE db 0

get_controls_state:
    push si

    call get_keyboard_state
    mov si, ax

    ; left
    xor al, al
    or  al, byte [si+KEYBOARD_SCAN_CODE_LEFT_ARROW]
    mov byte [controls_state+CONTROLS_STATE_OFF_LEFT], al

    ; right
    xor al, al
    or  al, byte [si+KEYBOARD_SCAN_CODE_RIGHT_ARROW]
    mov byte [controls_state+CONTROLS_STATE_OFF_RIGHT], al

    ; rotate right
    xor al, al
    or  al, byte [si+KEYBOARD_SCAN_CODE_UP_ARROW]
    or  al, byte [si+KEYBOARD_SCAN_CODE_END]
    or  al, byte [si+KEYBOARD_SCAN_CODE_KEYPAD_5]
    or  al, byte [si+KEYBOARD_SCAN_CODE_PGUP]
    mov byte [controls_state+CONTROLS_STATE_OFF_ROT_RIGHT], al

    ; rotate left
    xor al, al
    or  al, byte [si+KEYBOARD_SCAN_CODE_CTRL]
    or  al, byte [si+KEYBOARD_SCAN_CODE_Z]
    or  al, byte [si+KEYBOARD_SCAN_CODE_PGDN]
    or  al, byte [si+KEYBOARD_SCAN_CODE_HOME]
    mov byte [controls_state+CONTROLS_STATE_OFF_ROT_LEFT], al

    ; rotate 180 degrees
    xor al, al
    or  al, byte [si+KEYBOARD_SCAN_CODE_A]
    mov byte [controls_state+CONTROLS_STATE_OFF_ROT_180], al

    ; soft drop
    xor al, al
    or  al, byte [si+KEYBOARD_SCAN_CODE_DOWN_ARROW]
    mov byte [controls_state+CONTROLS_STATE_OFF_SOFT_DROP], al

    ; hard drop
    xor al, al
    or  al, byte [si+KEYBOARD_SCAN_CODE_SPACEBAR]
    mov byte [controls_state+CONTROLS_STATE_OFF_HARD_DROP], al

    ; hold
    xor al, al
    or  al, byte [si+KEYBOARD_SCAN_CODE_LEFT_SHIFT]
    or  al, byte [si+KEYBOARD_SCAN_CODE_RIGHT_SHIFT]
    mov byte [controls_state+CONTROLS_STATE_OFF_HOLD], al

    ; pause
    xor al, al
    or  al, byte [si+KEYBOARD_SCAN_CODE_ESC]
    or  al, byte [si+KEYBOARD_SCAN_CODE_F1]
    mov byte [controls_state+CONTROLS_STATE_OFF_PAUSE], al

    ; menu: up
    xor al, al
    or  al, byte [si+KEYBOARD_SCAN_CODE_UP_ARROW]
    mov byte [controls_state+CONTROLS_STATE_OFF_MENU_UP], al

    ; menu: down
    xor al, al
    or  al, byte [si+KEYBOARD_SCAN_CODE_DOWN_ARROW]
    mov byte [controls_state+CONTROLS_STATE_OFF_MENU_DOWN], al

    ; menu: select
    xor al, al
    or  al, byte [si+KEYBOARD_SCAN_CODE_SPACEBAR]
    or  al, byte [si+KEYBOARD_SCAN_CODE_ENTER]
    mov byte [controls_state+CONTROLS_STATE_OFF_MENU_SELECT], al

    mov ax, controls_state
.done:
    pop si
    ret
