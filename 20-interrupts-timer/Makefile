# $@ = target file
# $< = first dependency
# $^ = all dependencies
ifeq (0, 0)
C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h)
INC = kernel drivers cpu libc mm
# Nice syntax for file extension replacement
OBJS = ${C_SOURCES:.c=.o cpu/interrupt.o}

# Change this if your cross-compiler is somewhere else
CC = gcc
GDB = gdb
CFLAGS = -g -fno-pie -m32 -ffreestanding
LDFLAGS = -m elf_i386 -T../mkfile/link.ld

all: run

bochs: floppya.img
	bochs -f ../mkfile/bochsenv/bochs.bochsrc

run: floppya.img
	qemu-system-x86_64 -drive format=raw,file=$<,index=0,if=floppy
#	qemu-system-i386 -fda $<

floppya.img: boot/bootsect.bin kernel.bin
	cat $^ > $@

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: boot/kernel_entry.o ${OBJS}
	ld $(LDFLAGS) -o $@ $^ --oformat binary

kernel.elf: boot/kernel_entry.o ${OBJS}
	ld $(LDFLAGS) -z noexecstack -o $@ $^

kernel_entry.o: boot/kernel_entry.asm
	nasm $< -f elf -o $@

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf *.img
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o

else

C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h)
INC = kernel drivers cpu libc mm
# Nice syntax for file extension replacement
OBJS = ${C_SOURCES:.c=.o cpu/interrupt.o}

KERNEL.BIN=kernel.bin
BOOTSECT.ASM=boot/bootsect.asm
KERNELENTRY.ASM=boot/kernel_entry.asm

all: run

include ../mkfile/floppyMakefile
include ../mkfile/isoMakefile
include ../mkfile/kernelMakefile
include ../mkfile/commonMakefile

endif