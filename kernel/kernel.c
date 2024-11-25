#include "../cpu/port.h"
#include "../drivers/screen.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "./kernel.h"
#include "../libc/string.h"
#include "../ksh/ksh.h"
#include "../libc/convert.h"
#include "../drivers/bios.h"
#include "../libc/security.h"
#include "../drivers/multiboot.h"
#include "../libc/function.h"
#include "../libc/mem.h"
#include "paging.h"
#include "gdt.h"
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
#define KERNEL_SWITCHED_MESSAGE "switched to kernel\n"
#define KERNEL_DONE_MESSAGE "Wellcome to YATO\n"
#define KERNEL_INFO_MESSAGE "Type \"help\" to get a list of all KSH commands avaliable\nThis OS is licenced under MIT and is created for educational purposes\nCreated by Tarith Jayasoriya\n"
#include "../drivers/screen.h"

uint32_t *mb_info;

KernelState kernel_state;




extern void load_gdt(void);  // Declare the external assembly function

#include <stdint.h>
#include "../drivers/keyboard.h"
#define PAGE_SIZE 0x1000
#define KERNEL_VIRTUAL_BASE 0xC0000000 // Example base for virtual addresses
#define PAGE_DIR_ADDR 0x100000 // Example address for Page Directory
#define PAGE_TABLE_ADDR 0x200000 // Example address for Page Table


void native_tlb_flush(uint32_t addr) {
	asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}


// Function to map a physical address to a virtual address
void map_physical_to_virtual(uint32_t physical_addr, uint32_t virtual_addr, uint32_t *boot_page_dir) {
    // Virtual address of the Page Directory and Page Tables after recursive mapping
    uint32_t *page_table;

    // Step 1: Get the page table address for the given virtual address
    uint32_t page_dir_index = (virtual_addr >> 22) & 0x3FF; // Top 10 bits (Page Directory index)
    uint32_t page_table_index = (virtual_addr >> 12) & 0x3FF; // Next 10 bits (Page Table index)

    if (boot_page_dir[page_dir_index] & 0x1) {
        // Page table already exists, calculate its virtual address
        page_table = (uint32_t *)(0xFFC00000 + (page_dir_index * 0x1000));
    } else {
        // Page table does not exist, allocate a new one
        uint32_t new_page_table_phys = (uint32_t)alloc_page(); // Replace with your allocator
        boot_page_dir[page_dir_index] = (new_page_table_phys & 0xFFFFF000) | 0x03; // Present, Writable

        // Calculate the virtual address of the new page table
        page_table = (uint32_t *)(0xFFC00000 + (page_dir_index * 0x1000));

        // Clear the new page table (mark all entries as not present)
        for (int i = 0; i < 1024; i++) {
            page_table[i] = 0;
        }
    }

    // Step 2: Map the physical address to the virtual address in the page table
    page_table[page_table_index] = (physical_addr & 0xFFFFF000) | 0x03; // Present, Writable

    // Step 3: Flush the TLB for the updated virtual address
    native_tlb_flush(virtual_addr); // Implement or use `invlpg` instruction
}


void *load_page_table() {
    void *raw_ptr;
    __asm__ volatile("mov %%cr3, %0" : "=a"(raw_ptr));
    return (uint32_t *) ((uintptr_t) raw_ptr + KERNEL_VIRTUAL_BASE);
}

void page_table_set(uintptr_t address, uintptr_t page_addr, uint16_t flags) {
    uint32_t *page_table = load_page_table();
    uint16_t i = (uint16_t) (page_addr >> 22);
    page_table[i] = (address & 0xFFC00000) | flags;
    __asm__ volatile("invlpg %0" : : "m"(i));
}

// Function to map a range of physical addresses to virtual addresses
void map_physical_to_virtual_range(uint32_t start_addr, uint32_t virtual_addr, uint32_t size, uint32_t *boot_page_dir) {
    uint32_t current_physical_addr = start_addr;
    uint32_t current_virtual_addr = virtual_addr;
    uint32_t end_addr = start_addr + size;

    while (current_physical_addr < end_addr) {
        // Call the single-page mapping function
        map_physical_to_virtual(current_physical_addr, current_virtual_addr, boot_page_dir);

        // Move to the next page
        current_physical_addr += 0x400000; // Increment physical address by 4 MB
        current_virtual_addr += 0x400000;  // Increment virtual address by 4 MB 
    }
}


// Function to read from physical memory
void *read_from_physical(uint32_t physical_addr) {
    // Define a virtual address to map to
    static uint32_t virtual_addr = KERNEL_VIRTUAL_BASE;

    // Map physical address to virtual address
    map_physical_to_virtual(physical_addr, virtual_addr, NULL);

    // Access the data structure through the mapped virtual address
    return (void *)virtual_addr;
}


int is_page_all_zeroes(void *page, size_t size) {
    uint8_t *byte_ptr = (uint8_t *)page; // Cast the page to a byte pointer

    for (size_t i = 0; i < size; i++) {
        if (byte_ptr[i] != 0) {
            return 0;  // Return 0 if any byte is not zero
        }
    }

    return 1;  // Return 1 if all bytes are zero
}


// Function to unmap the lower half of the kernel page
void unmap_lower_half_kernel_page(uint32_t *boot_page_dir) {
    // Step 1: Unmap the first page directory entry (set it to 0)
    boot_page_dir[0] = 0;

    // Step 2: Invalidate the TLB for the virtual address 0x00000000 (the first page)
    native_tlb_flush((uint32_t)0);
}

void page_fault(registers_t *regs)
{
	// A page fault has occured.
	// The faulting address is stored in the CR2 register.
	uint32_t faulting_address;
	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

	// The error code gives us detailing og what happened.
	int present	= !(regs->err_code & 0x1);	// Page not present
	int rw		= regs->err_code & 0x2;		// Write operation?
	int us		= regs->err_code & 0x4;		// Processor was in user-mode?
	int reserved	= regs->err_code & 0x8;		// Overwritten CPU-reserved bits of page entry?
	int id		= regs->err_code & 0x10;		// Caused by an instruction fetch?
    UNUSED(id);
	// Output en error message.
	kprint("Page fault! ( ");
	if (present)
		kprint("present ");
	if (rw)
		kprint("read-only ");
	if (us)
		kprint("user-mode ");
	if (reserved)
		kprint("reserved ");

	kprint(") at 0x");
    char *st[20];
    hex_to_ascii(faulting_address,(char*)st);
	kprint((char*)st);
	kprint("\n");
	kprintln("Page fault");
    while (1) {};
}

void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_info_addr, uint32_t boot_page_dir)
{
    boot_page_dir = boot_page_dir - 0xC0000000;
    // kernel has first page access identity mapped

	UNUSED(multiboot_magic);
	UNUSED(boot_page_dir);
	UNUSED(multiboot_info_addr);
    //register_interrupt_handler(IRQ14, page_fault);
    //map_physical_to_virtual(*multiboot_info_addr, *multiboot_info_addr, (uint32_t*)boot_page_dir);
	//uint32_t addr = kmalloc(sizeof(struct multiboot_info), 1, NULL);
	//is_page_all_zeroes((uint32_t*)addr, 0x8000);

	//map_physical_to_virtual_range((uint32_t)multiboot_info_addr, addr, sizeof(struct multiboot_info), &boot_page_dir);

    init_descriptor_tables();
	vbe_mode_info_structure_t* info = (vbe_mode_info_structure_t*) ((struct multiboot_info*)(multiboot_info_addr))->vbe_mode_info;
    page_table_set(info->framebuffer, info->framebuffer, 0x83);
    UNUSED(info);
	//uint32_t framebuffer_addr = kmalloc(0x8000, 1, NULL);
	//is_page_all_zeroes((uint32_t*)addr, 0x8000);

	map_physical_to_virtual_range((uint32_t)info->framebuffer, info->framebuffer, info->width*info->pitch, &boot_page_dir);
	init_framebuffer(info);
    register_interrupt_handler(14, page_fault);
	//unmap_lower_half_kernel_page((uint32_t *)boot_page_dir);
	isr_install();
	asm volatile("sti");
	init_keyboard();
	init_timer(50);


	//sleep(100);
    //map_physical_to_virtual_range((uint32_t)info->framebuffer, info->framebuffer, info->height*info->pitch, &boot_page_dir);

    
	draw_char(50,50,'A', 0xF0F);
	kprintln(KERNEL_SWITCHED_MESSAGE);

    for (int x = 0; x <1; x++) {
        for (int y = 0; y <1; y++) {
            kput_pixel(x,y,234);
        }
    }

	//sleep(100);
	//clear_screen();

	kprintln(KERNEL_DONE_MESSAGE);
	kprintln(KERNEL_INFO_MESSAGE);



    // Search for RSDP within upper memory BIOS space, 0xe0000 - 0xfffff
    //uintptr_t ebda_base = 0xE0000;
    //struct RSDPDescriptor *rsdp = find_rsdp(ebda_base, 0x20000);
	
	kprintln("OEM: ");
	//kprintln(rsdp->oem_id);
	kprintln("\n");

	kprintln("signature: ");
	//kprintln(rsdp->signature);
	kprintln("\n");


    //struct ISDTHeader* isdt = (struct ISDTHeader*) rsdp->rsdt_address;

	kprintln("isdt OEM: ");
	//kprintln(isdt->OEMID);
	kprintln("\n");

	
	//walk_sdts(rsdp->rsdt_address);
	char *v = "";
	//int value = get_current_privilege_level();
	//convert_int_to_string(value, v);
	kprintln("in ring ");
	kprintln(v);
	kprintln("\n");

	kprintln("> ");
    for(;;) {
        asm("hlt");
    }
}


// handle user input at the kernel level
void user_input(char *input)
{
	// call ksh to handle user input
	ksh_handle(input);
}
