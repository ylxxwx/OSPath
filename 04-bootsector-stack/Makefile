# $@ = target file
# $< = first dependency
# $^ = all dependencies
ifeq (0,1)
all: run

bochs: floppya.img
	bochs -f ../mkfile/bochsenv/bochs.bochsrc

run: floppya.img
	qemu-system-x86_64 -drive format=raw,file=$<,index=0,if=floppy

floppya.img: boot_sect_stack.bin
	cat $^ > $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -f *.bin bochsout.txt  floppya.img

else 

BOOTSECT.ASM=boot_sect_stack.asm

all: run

include ../mkfile/floppyMakefile
include ../mkfile/commonMakefile

endif