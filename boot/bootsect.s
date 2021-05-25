[org 0x7c00] ; tell the assembler that the offset is bootsector code
KERNEL_OFFSET equ 0x1000 ; The same one we used when linking the kernel

    mov [BOOT_DRIVE], dl ; Remember that the BIOS sets us the boot drive in 'dl' on boot
    mov bp, 0x9000
    mov sp, bp

    mov bx, MSG_REAL_MODE
    call print ; This will be written after the BIOS messages
    call print_nl

    call load_kernel   	
    call switch_to_protected_mode

    ; this should never get executed
    jmp $

%include "./boot/print.s"
%include "./boot/print_hex.s"
%include "./boot/disk.s"
%include "./boot/32bit-switch.s"

%include "./boot/32_real/gdt.s"
%include "./boot/32_real/print.s"



[bits 16]
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print
    call print_nl

    mov bx, KERNEL_OFFSET ; Read from disk and store in 0x1000
    mov dh, 16 ; kernel size
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 32]
BEGIN_PM: ; after the switch we will get here
    mov ebx, MSG_PROT_MODE
    call print_string_pm ; Note that this will be written at the top left corner
    
    	
    call KERNEL_OFFSET ; give control to kernel
    jmp $


BOOT_DRIVE db 0 ; It is a good idea to store it in memory because 'dl' may get overwritten
MSG_REAL_MODE db "Started in 16-bit real mode", 0
MSG_PROT_MODE db "Loaded 32-bit protected mode", 0
MSG_LOAD_KERNEL db "Loading kernel into memory", 0

; padding and magic number
times 510 - ($-$$) db 0
dw 0xaa55
