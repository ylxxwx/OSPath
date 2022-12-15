# $@ = target file
# $< = first dependency
# $^ = all dependencies
ifeq (0,1)
C_SOURCES = $(wildcard kernel/*.c)

# Nice syntax for file extension replacement
OBJS = ${C_SOURCES:.c=.o}

# Change this if your cross-compiler is somewhere else
CC = gcc
GDB = gdb
CFLAGS = -g -fno-pie -m32 -ffreestanding
LDFLAGS = -m elf_i386 -Ttext 0x1000 --oformat binary

all: run

bochs: floppya.img
	bochs -f ../mkfile/bochsenv/bochs.bochsrc

run: floppya.img
	qemu-system-x86_64 -drive format=raw,file=$<,index=0,if=floppy
#	qemu-system-i386 -fda $<

floppya.img: bootsect.bin kernel.bin
	cat $^ > $@

bootsect.bin: boot/bootsect.asm
	nasm $< -f bin -o $@

kernel.bin: kernel_entry.o ${OBJS}
	echo "${OBJS}"
	ld $(LDFLAGS) -o $@  $^ 

kernel_entry.o: boot/kernel_entry.asm
	nasm $< -f elf -o $@

kernel.o: 

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -f *.bin *.img *.o kernel/*.o boot/*.bin boot/*.o bochsout.txt
else
C_SOURCES = $(wildcard kernel/*.c )
OBJS = ${C_SOURCES:.c=.o}

KERNEL.BIN=kernel.bin
BOOTSECT.ASM=boot/bootsect.asm
KERNELENTRY.ASM=boot/kernel_entry.asm

all: run

include ../mkfile/floppyMakefile
include ../mkfile/kernelMakefile
include ../mkfile/commonMakefile

endif