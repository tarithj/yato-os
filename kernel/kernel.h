#ifndef KERNEL_H
#define KERNEL_H
extern uint32_t __base;
void user_input(char *input);

// Declare the external variable from assembly code
extern uint32_t *mb_info;

#include "../drivers/screen.h"

typedef struct {
    framebuffer_info_t framebuffer_info;
} KernelState;

#endif