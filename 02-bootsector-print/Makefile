# $@ = target file
# $< = first dependency
# $^ = all dependencies
ifeq (1,0)

all: run

run: qemu bochs

bochs: floppya.img
	bochs -f ../mkfile/bochsenv/bochs.bochsrc

qemu: floppya.img
	qemu-system-x86_64 -drive format=raw,file=$<,index=0,if=floppy

floppya.img: boot_sect_hello.bin
	cat $^ > $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -f *.bin *.img

else

BOOTSECT.ASM=boot_sect_hello.asm

all: run

include ../mkfile/floppyMakefile
include ../mkfile/commonMakefile

endif