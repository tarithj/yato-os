#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stddef.h>

void memory_copy(uint32_t *source, uint32_t *dest, int nbytes);
void memory_set(uint8_t *dest, uint8_t val, uint32_t len);
int memory_compare(const void *ptr1, const void *ptr2, size_t num);
uint32_t align_to_4MB(uint32_t address);
/* At this stage there is no 'free' implemented. */
uint32_t kmalloc(uint32_t size, int align, uint32_t *phys_addr);

#endif