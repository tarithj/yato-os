ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
    /* The kernel will live at 3GB + 1MB in the virtual
       address space, which will be mapped to 1MB in the
       physical address space. */
    . = 0xC0000000;
  
  /* Place the multiboot header at the very beginning */
  .multiboot ALIGN(4K) : AT(ADDR(.multiboot) - 0xC0000000)
  {
    KEEP(*(.multiboot))
  }

  /* Code section */
  .text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000)
  {
    *(.text)
  }

	/* Read-only data. */
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
	{
       _sbss = .;
		*(COMMON)
		*(.bss)
       _ebss = .;
	}
}
