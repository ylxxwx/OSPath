#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "kernel.h"
#include "../libc/string.h"
//#include "../libc/mem.h"
#include "nofreemem.h"
#include "memory.h"

u32 *addr = 0xD0000000;
u32 size =   0x800000;

void main() {
    isr_install();
    irq_install();

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
    }
}

void user_input(char *input) {
    if (strcmp(input, "END") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    } else if (strcmp(input, "PAGE") == 0) {
        void *addr = 0;
        int tries = 3;
        addr = kmalloc(1024*4);
        while(tries >= 0) {
            // use memory all: 1024*1024-20
            // last part: 1024 and 1023*1024-40
            addr = kmalloc(1024*4);
            if (addr == 0) {
                kprint("Alloc memory failed.\n");
                return;
            }
            kprint("allocated: ");
            kprint_hex(addr);
            kprint("\n");
            
            u32 *addr1 = kmalloc(1024*4);
            if (addr1 == 0) {
                kprint("Alloc memory failed.\n");
                return;
            }
            
            kprint("allocated: ");
            kprint_hex(addr);
            kprint("\n");
            kfree(addr);
            kfree(addr1);
            tries--;
        }
        /* Lesson 22: Code to test kmalloc, the rest is unchanged */
        /*
        u32 phys_addr;
        u32 page = kmalloc_ap_nofree(1000, 1, &phys_addr);
        char page_str[16] = "";
    
        hex_to_ascii(page, page_str);
        char phys_str[16] = "";
        hex_to_ascii(phys_addr, phys_str);
        kprint("Page: ");
        kprint(page_str);
        kprint(", physical address: ");
        kprint(phys_str);
        kprint("\n");
        */
    } else if (strcmp(input, "TEST") == 0) {
        u32 a1 = kmalloc_a(1024, 1);
        kprint("allocated: ");
        kprint_hex(a1);
        kprint("\n");
        
        u32 a2 = kmalloc_a(1024, 1);
        kprint("allocated: ");
        kprint_hex(a2);
        kprint("\n");
        kfree(a1);
        kfree(a2);

        u32 a3 = kmalloc_a(1024, 1);
        kprint("allocated: ");
        kprint_hex(a3);
        kprint("\n");
        kfree(a3);
    } else if (strcmp(input, "INT") == 0) {
        for (int index = 0; index < 100; index++) {
        kprint("trigger a page interruption.\n");
        //int *addr = (int *)(0xEFFFFF00);
        *addr = 1;
        addr = (u32)addr + size;
        }
    }
    kprint("You said: ");
    kprint(input);
    kprint("\n> ");
}
