#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void init_timer(uint32_t freq);

// copies tick to tick dest
void get_tick(uint32_t *dest);

void sleep(uint64_t TICKS);

#endif