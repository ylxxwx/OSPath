#include "isr.h"
#include "screen.h"
#include "kernel.h"
#include "string.h"
#include "gdt.h"
#include "nofreemem.h"
#include "memory.h"
#include "task.h"

u32 *addr = 0xD0000000;
u32 size = 0x800000;

void main()
{
    cur_task_id = 0;
    init_gdt();
    isr_install();
    irq_install();
    init_memory();

    kprintf("Type something, it will go through the kernel\n"
            "Type END to halt the CPU or PAGE to request a kmalloc()\n> ");
    initialise_syscalls();
    // asm("int $0x80");

    init_task();
}

void user_input(char *input)
{
    if (strcmp(input, "LOCK") == 0)
    {
        set_lock();
    }
    if (strcmp(input, "SYNC") == 0)
    {
        start_sync_test();
    }
    else if (strcmp(input, "END") == 0)
    {
        kprintf("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    }
    else if (strcmp(input, "PAGE") == 0)
    {
        void *addr = 0;
        int tries = 3;
        addr = kmalloc(1024 * 4);
        while (tries >= 0)
        {
            // use memory all: 1024*1024-20
            // last part: 1024 and 1023*1024-40
            addr = kmalloc(1024 * 4);
            if (addr == 0)
            {
                kprintf("Alloc memory failed.\n");
                return;
            }
            kprintf("allocated: %x\n", addr);

            u32 *addr1 = kmalloc(1024 * 4);
            if (addr1 == 0)
            {
                kprintf("Alloc memory failed.\n");
                return;
            }

            kprintf("allocated: %x\n", addr);

            kfree(addr);
            kfree(addr1);
            tries--;
        }
    }
    else if (strcmp(input, "TEST") == 0)
    {
        u32 a1 = kmalloc_a(1024, 1);
        kprintf("allocated: %x\n", a1);

        u32 a2 = kmalloc_a(1024, 1);
        kprintf("allocated: %x\n", a2);
        kfree(a1);
        kfree(a2);

        u32 a3 = kmalloc_a(1024, 1);
        kprintf("allocated: %x\n", a3);
        kfree(a3);
    }
    else if (strcmp(input, "INT") == 0)
    {
        for (int index = 0; index < 100; index++)
        {
            kprintf("trigger a page interruption.\n");
            // int *addr = (int *)(0xEFFFFF00);
            *addr = 1;
            addr = (u32)addr + size;
        }
    }
    kprintf("You said: %s\n", input);
}
