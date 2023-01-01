# $@ = target file
# $< = first dependency
# $^ = all dependencies
CC = gcc
GDB = gdb
CFLAGS = -g -fno-pie -m32 -ffreestanding 
LDFLAGS = -m elf_i386  -T./link.ld

C_SOURCES = $(wildcard common/*.c)
HEADERS = $(wildcard common/*.h)
INC = common
# Nice syntax for file extension replacement
OBJS = ${C_SOURCES:.c=.o common/entry.o}

all: shell pwd
	./writedisk.sh

shell: common/entry.o shell.o ${OBJS}
	ld $(LDFLAGS) -o $@ $^ --oformat binary

shell.elf: common/entry.o shell.o ${OBJS}
	ld $(LDFLAGS) -o $@ $^
# --oformat binary

pwd:  common/entry.o pwd.o ${OBJS}
	ld $(LDFLAGS) -o $@ $^ --oformat binary

pwd.elf: common/entry.o pwd.o ${OBJS}
	ld $(LDFLAGS) -o $@ $^

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} $(INC:%=-I %) -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

clean:
	rm -rf shell pwd *.elf ./*.o common/*.o
