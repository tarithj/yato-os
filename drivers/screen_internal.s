section .text
global vga_set_mode


vga_set_mode:
    ; Set up registers for video mode setting
    mov ah, 0x00        ; AH = 0x00 for set video mode function
    mov al, byte [ebp+8] ; Load mode number from parameter
    int 0x10            ; Call interrupt 0x10 (video services)
    ret