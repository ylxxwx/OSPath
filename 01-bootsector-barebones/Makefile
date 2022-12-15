# $@ = target file
# $< = first dependency
# $^ = all dependencies
ifeq (0,1)

all: run

run: qemu

bochs: floppya.img
	bochs -f ../mkfile/bochsenv/bochs.bochsrc

qemu: floppya.img
	qemu-system-x86_64 -drive format=raw,file=$<,index=0,if=floppy

floppya.img: boot_sect_simple.bin
	cat $^ > $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -f *.bin floppya.img

else
BOOTSECT.ASM=boot_sect_simple.asm

all: run

include ../mkfile/floppyMakefile
include ../mkfile/commonMakefile

endif