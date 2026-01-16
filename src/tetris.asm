    org 100h
    use16

macro EXIT code {
    mov ah, 4Ch
    mov al, code
    int 21h
}

macro PRINT string {
    mov ah, 09h
    mov dx, string
    int 21h
}

    PRINT hello
    EXIT 0

hello db 'Hello World', '$'
