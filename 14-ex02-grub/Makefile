# $@ = target file
# $< = first dependency
# $^ = all dependencies
C_SOURCES = $(wildcard kernel/*.c)

# Nice syntax for file extension replacement
OBJS = ${C_SOURCES:.c=.o}

# Change this if your cross-compiler is somewhere else
CC = gcc
GDB = gdb
CFLAGS = -g -fno-pie -m32 -ffreestanding
LDFLAGS = -m elf_i386 -Ttext 0x1000 --oformat binary

all: run

bochs: root-image.bin
	bochs -f ../99_common/bochs.bochsrc

run: root-image.bin
	qemu-system-x86_64 -drive format=raw,file=$<,index=0,if=floppy
#	qemu-system-i386 -fda $<

root-image.bin: bootsect.bin kernel.bin
	cat $^ > $@

bootsect.bin: boot/bootsect.asm
	nasm $< -f bin -o $@

kernel.bin: boot/kernel_entry.o ${OBJS}
	ld -m elf_i386 -TlinkLilo.ld -o $@ $^ --oformat binary

kernel.img: boot/kernel_entry.o ${OBJS}
	ld -m elf_i386 -z noexecstack -TlinkGrub.ld -o $@ $^

kernel_entry.o: boot/kernel_entry.asm
	nasm $< -f elf -o $@

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -f *.bin *.o kernel/*.o boot/*.bin boot/*.o bochsout.txt ./grub/bochsout.txt