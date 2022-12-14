#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "kernel.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "../mm/memory.h"

extern u32 free_mem_addr;
void main() {

    kprint_hex(free_mem_addr);
    isr_install();
    kprint_hex(free_mem_addr);
    irq_install();
    kprint_hex(free_mem_addr);

    kprint("Type something, it will go through the kernel\n"
        "Type END to halt the CPU or PAGE to request a kmalloc()\n> ");
    init_memory();
/*
    char* address = (char *)0xF0000000;
    char ch = *address; // read
    //*address = 'a';   // write
    *address = 'b';
    char rch = *address;
    kprint_hex(rch);
    kprint("\n");
    *address = 'c';
    rch = *address;
    kprint_hex(rch);
    kprint("\n");
    *address = 'd';
    rch = *address;
    kprint_hex(rch);
    kprint("\n");
*/
    while(1) {
        asm("hlt");
        //kprint("A");
    }
}

void user_input(char *input) {
    if (strcmp(input, "END") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (strcmp(input, "PAGE") == 0) {
        /* Lesson 22: Code to test kmalloc, the rest is unchanged */
        u32 phys_addr;
        u32 page = kmalloc_a(1000, 1, &phys_addr);
        char page_str[16] = "";
        hex_to_ascii(page, page_str);
        char phys_str[16] = "";
        hex_to_ascii(phys_addr, phys_str);
        kprint("Page: ");
        kprint(page_str);
        kprint(", physical address: ");
        kprint(phys_str);
        kprint("\n");
    }
    kprint("You said: ");
    kprint(input);
    kprint("\n> ");
}
