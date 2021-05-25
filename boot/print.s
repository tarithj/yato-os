print:
    pusha

start:
    mov al, [bx] ; bx is the base address for the string
    cmp al, 0
    je done

    ; the part were we print with the bios help
    mov ah, 0x0e
    int 0x10; al already contains the char

    ; inc the pointer and do next loop
    add bx, 1
    jmp start

done:
    popa
    ret

print_nl:
    pusha
    
    mov ah, 0x0e
    mov al, 0x0a ; new line char
    int 0x10

    mov al, 0x0d ; carriage return
    int 0x10

    popa
    ret
