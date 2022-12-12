# $@ = target file
# $< = first dependency
# $^ = all dependencies

all: run

bochs: root-image.bin
	bochs -f ../99_common/bochs.bochsrc

run: root-image.bin
	qemu-system-x86_64 -drive format=raw,file=$<,index=0,if=floppy

root-image.bin: boot_sect_hello.bin
	cat $^ > $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -f *.bin bochsout.txt
