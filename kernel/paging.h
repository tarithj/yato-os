#ifndef PAGING_H
#define PAGING_H

#include "stddef.h"
#include "../libc/mem.h"


void paging_install(void);

uintptr_t phys_to_virt(uintptr_t phys);
uintptr_t virt_to_phys(uintptr_t virt);
void* alloc_page();
#endif /* DUNE_PAGING_H */