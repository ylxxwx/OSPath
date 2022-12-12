# $@ = target file
# $< = first dependency
# $^ = all dependencies
C_SOURCES = fucntion.c

# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o}

# Change this if your cross-compiler is somewhere else
CC = gcc
GDB = gdb
CFLAGS = -g -fno-pie -m32 -ffreestanding -I./driver -I./cpu -I./kernel
INCLUDEFLAGS = -I./driver -I./cpu -I./kernel

all: objs bins

objs: function.o functioncalls.o localvars.o pointers.o

bins: function.bin functioncalls.bin localvars.bin pointers.bin

%.o: %.c
	gcc ${CFLAGS}  -c $< -o $@

%.bin: %.o
	ld -m elf_i386 -o $@ -Ttext 0x0 --oformat binary $<

clean:
	rm -f *.bin *.o
