section .text
global ide_init
global ide_ready_check
global ide_get_status_register

ide_get_status_register:
    mov dx, 0x1F6        ; Command/Status register
    in al, dx            ; Read the status register
    mov [bx], al         ; Store the value of AL (status register) at the memory location pointed to by BX
    ret

ide_set_command_status_register:
    mov dx, 0x1F6        ; Command/Status register
    ret

    
; checks the status registers
ide_ready_check:
    in al, dx            ; Read the status register
    test al, 0x40        ; Check if DRQ (bit 6) is set 
    jnz .drq_set         ; Jump to .drq_set if DRQ is set
    mov eax, 0           ; Clear eax register if DRQ is not set
    ret

.drq_set:
    mov eax, 1           ; Set eax register to 1 if DRQ is set
    ret

ide_init:
    ; Reset the IDE controller
    mov dx, 0x1F6        ; Command/Status register
    mov al, 0x04         ; Software reset command
    out dx, al

    ; Wait for the IDE controller to reset (wait for BSY to clear)
.reset_loop:
    in al, dx            ; Read the status register
    test al, 0x80        ; Check if BSY (bit 7) is set
    jnz .reset_loop      ; If BSY is set, loop until it clears

    ; Enable interrupts and set the drive/head register
    mov dx, 0x3F6        ; Device control register
    mov al, 0x02         ; Enable interrupts and set drive/head to master
    out dx, al

    ; Detect IDE devices
    mov dx, 0x1F7        ; Command/Status register
    mov al, 0xEC         ; Identify command
    out dx, al

    ; Wait for the IDE controller to be ready to accept commands
.ready_loop:
    in al, dx            ; Read the status register
    test al, 0x40        ; Check if DRQ (bit 6) is set
    jnz .ready_loop      ; If DRQ is set, controller is ready

    ret
