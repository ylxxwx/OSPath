# $@ = target file
# $< = first dependency
# $^ = all dependencies
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

bootsect.bin: bootsect.asm
	nasm $< -f bin -o $@

kernel.bin: kernel_entry.o kernel.o
	ld $(LDFLAGS) -o $@  $^ 

kernel_entry.o: kernel_entry.asm
	nasm $< -f elf -o $@

kernel.o: kernel.c
	gcc ${CFLAGS} -c $< -o $@

# Rule to disassemble the kernel - may be useful to debug
kernel.dis: kernel.bin
	ndisasm -b 32 $< > $@

clean:
	rm -f *.bin *.o *.dis bochsout.txt
