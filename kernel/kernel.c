#include "../cpu/port.h"
#include "../drivers/screen.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "../drivers/keyboard.h"
#include "./kernel.h"
#include "../libc/string.h"
#include "../ksh/ksh.h"

#define KERNEL_SWITCHED_MESSAGE "switched to kernel\n"
#define KERNEL_DONE_MESSAGE "Wellcome to YATO\n"

void main()
{
	kprint_at(KERNEL_SWITCHED_MESSAGE, -1, -1);

	isr_install();
	asm volatile("sti");
	init_keyboard();

	kprint_at(KERNEL_DONE_MESSAGE, -1, -1);
}

void user_input(char *input)
{
	ksh_handle(input);
}