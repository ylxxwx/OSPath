# $@ = target file
# $< = first dependency
# $^ = all dependencies
ifeq (0,1)
all: run

bochs:  floppya.img
	bochs -f ../mkfile/bochsenv/bochs.bochsrc

run:  floppya.img
	qemu-system-x86_64 -drive format=raw,file=$<,index=0,if=floppy

floppya.img: 32bit-main.bin
	cat $^ > $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -f *.bin bochsout.txt *.img

else 

BOOTSECT.ASM=32bit-main.asm

all: run

include ../mkfile/floppyMakefile
include ../mkfile/commonMakefile

endif