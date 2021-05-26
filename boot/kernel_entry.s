[bits 32]
[extern kernel_main] ; Define calling point. Mist have some name as kernel.c 'main' function
call kernel_main ; Calls the c function. The linker will know where it is placed in memory
jmp $