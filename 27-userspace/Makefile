# $@ = target file
# $< = first dependency
# $^ = all dependencies
C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c mm/*.c task/*.c sync/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h mm/*.h task/*.h sync/*.h)
INC = kernel drivers cpu libc mm task sync
# Nice syntax for file extension replacement
OBJS = ${C_SOURCES:.c=.o cpu/interrupt.o task/task_switch.o libc/utils.o}

KERNEL.BIN=kernel.bin
BOOTSECT.ASM=boot/bootsect.asm
KERNELENTRY.ASM=boot/kernel_entry.asm

all: run

include ../mkfile/floppyMakefile
include ../mkfile/isoMakefile
include ../mkfile/kernelMakefile

include ../mkfile/commonMakefile