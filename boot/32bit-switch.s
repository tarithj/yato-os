[bits 16]
switch_to_protected_mode:
	; First things first: no interrupts
	cli
	; Load our GDT descriptor.
	lgdt [gdt_descriptor]
	; Set 32-bit mode in CR0 register!
	mov eax, cr0
	or eax, 0x1
	mov cr0, eax ; <==== LOAD cr0 to notify the processo
	; And jump to our code segment!
	jmp CODE_SEG:init_protected_mode ; <===== do a FAR jump in the CODE_SEG

[bits 32]
init_protected_mode:
    mov ax, DATA_SEG ; update the segment registers
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000 ; update the stack right at the top of the free space
    mov esp, ebp

    call BEGIN_PM
