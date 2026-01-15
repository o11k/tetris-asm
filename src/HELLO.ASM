.MODEL SMALL
.STACK 100h
.DATA
hello_msg DB 'Hello, World!$'
.CODE
.STARTUP
mov ah, 09h
lea dx, hello_msg
int 21h
mov ah, 4ch
int 21h
END
