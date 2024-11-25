#include "timer.h"
#include "isr.h"
#include "port.h"
#include "../libc/function.h"
#include "../drivers/screen.h"
uint32_t tick = 0;

static void timer_callback(registers_t *regs)
{
    tick++;
    UNUSED(regs);
}

void get_tick(uint32_t *dest)
{
    *dest = tick;
}

void init_timer(uint32_t freq)
{
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    uint32_t divisor = 1193180 / freq;
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
    /* Send the command */
    port_byte_out(0x43, 0x36); /* Command port */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}

void sleep(uint64_t TICKS)
{
    asm("cli");
    uint64_t STOPTICK = tick + TICKS;
    asm("sti"); // Don't want this part to be interrupted.
    while (tick < STOPTICK)
        ;
}