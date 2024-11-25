C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c ksh/*.c memory/*.c)
HEADERS = $(wildcard kernel/*.h  cpu/*.h libc/*.h ksh/*.h memory/*.h drivers/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o drivers/ata_internal.o drivers/screen_internal.o kernel/pre.o } 

# Change this if your cross-compiler is somewhere else
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
LD = /usr/local/i386elfgcc/bin/i386-elf-ld

# -g: Use debugging symbols in gcc
CFLAGS = -g -m32 -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
		 -Wall -Wextra -Werror -ffreestanding
		 
boot.iso: kernel.bin
	$(grub2-mkrescue -o boot.iso iso)

kernel.bin: boot/boot.o kernel/kernel.o ${OBJ}
	${CC} -T linker.ld -o iso/boot/grub/kernel.bin -ffreestanding -O2 -nostdlib $^ -lgcc 


run: boot.iso
	qemu-system-i386 -hda boot.iso -display curses -machine q35

run_graphic: boot.iso 
	qemu-system-i386 -hda boot.iso -machine q35  -device VGA 
	

# Open the connection to qemu and load our kernel-object file with symbols
# qemu-system-i386 -machine q35 -s -fda os-image.bin -d guest_errors,int &
debug: boot.iso
	qemu-system-i386 -hda boot.iso -machine q35 -device VGA  -d int -s -S &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file iso/boot/grub/kernel.bin"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.s
	nasm $< -f elf -o $@

%.bin: %.s
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o ksh/*.o 
