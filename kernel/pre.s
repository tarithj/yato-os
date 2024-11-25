; pre.s
section .data
    ; Null descriptor (first entry, 8 bytes)
    gdt_start:
        dd 0x0            ; Base low 32 bits (null descriptor)
        dd 0x0            ; Base high 32 bits

        ; Code Segment Descriptor
        dw 0xffff         ; Segment limit (lower 16 bits)
        dw 0x0            ; Base address (lower 16 bits)
        db 0x00           ; Base address (middle 8 bits)
        db 10011010b      ; Flags: 10011010 (type: code, 32-bit, etc.)
        db 11001111b      ; Flags: 11001111 (other flags)
        db 0x00           ; Base address (higher 8 bits)

        ; Data Segment Descriptor
        dw 0xffff         ; Segment limit (lower 16 bits)
        dw 0x0            ; Base address (lower 16 bits)
        db 0x00           ; Base address (middle 8 bits)
        db 10010010b      ; Flags: 10010010 (type: data, read/write, etc.)
        db 11001111b      ; Flags: 11001111 (other flags)
        db 0x00           ; Base address (higher 8 bits)

    gdt_end:

section .text
    global load_gdt

; Function to load the GDT
load_gdt:
    ; Load the GDT (using lgdt instruction)
    lgdt [gdt_descriptor]         ; Load GDT using lgdt

    ; Set up the segment registers to point to valid segments
    mov ax, 0x08  ; Code segment selector (offset 8, points to the code segment in GDT)
    mov ds, ax     ; Data segment
    mov es, ax     ; Extra segment
    mov fs, ax     ; FS segment
    mov gs, ax     ; GS segment
    mov ss, ax     ; Stack segment

    ; Return from the function
    ret

section .data
    ; GDT descriptor
    gdt_descriptor:
        dw gdt_end - gdt_start - 1   ; Size (16 bits) = gdt_end - gdt_start - 1
        dd gdt_start                 ; Base address of the GDT (32 bits)


;
; gdt.s - contains global descriptor table and interrupt descriptor table
;	setup code.
;

[GLOBAL gdt_flush]		; Allows the C code to call gdt_flush().
gdt_flush:
	mov	eax, [esp+4]	; Get the pointer to the GDT, passed as a parameter.
	lgdt	[eax]		; Load the new GDT pointer.

	mov	ax, 0x10	; 0x10 is the offset in the GDT to our data segment
	mov	ds, ax		; Load all data segment selectors
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax
	jmp	0x08:.flush	; 0x08 is the offset to our code segment: Far jump!
.flush:
	ret
