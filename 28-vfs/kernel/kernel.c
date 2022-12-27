#include "isr.h"
#include "screen.h"
#include "kernel.h"
#include "string.h"
#include "gdt.h"
#include "nofreemem.h"
#include "memory.h"
#include "mem.h"
#include "task.h"
#include "hd_irs.h"
#include "syscall.h"
#include "sync_test.h"

u32 *addr = (u32 *)0xD0000000;
u32 size = 0x800000;

void main()
{
    init_gdt();
    isr_install();
    irq_install();
    init_memory();
    init_syscalls();
    init_hds();
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
        addr = (void *)kmalloc(1024 * 4);
        while (tries >= 0)
        {
            // use memory all: 1024*1024-20
            // last part: 1024 and 1023*1024-40
            addr = (void *)kmalloc(1024 * 4);
            if (addr == 0)
            {
                kprintf("Alloc memory failed.\n");
                return;
            }
            kprintf("allocated: %x\n", addr);

            u32 *addr1 = (u32*)kmalloc(1024 * 4);
            if (addr1 == 0)
            {
                kprintf("Alloc memory failed.\n");
                return;
            }

            kprintf("allocated: %x\n", addr);

            kfree((u8*)addr);
            kfree((u8*)addr1);
            tries--;
        }
    }
    else if (strcmp(input, "TEST") == 0)
    {
        u32 a1 = (u32)kmalloc_a(1024, 1);
        kprintf("allocated: %x\n", a1);

        u32 a2 = (u32)kmalloc_a(1024, 1);
        kprintf("allocated: %x\n", a2);
        kfree((u8*)a1);
        kfree((u8*)a2);

        u32 a3 = (u32)kmalloc_a(1024, 1);
        kprintf("allocated: %x\n", a3);
        kfree((u8*)a3);
    }
    else if (strcmp(input, "INT") == 0)
    {
        for (int index = 0; index < 100; index++)
        {
            kprintf("trigger a page interruption.\n");
            // int *addr = (int *)(0xEFFFFF00);
            *addr = 1;
            addr = (u32*)((u32)addr + size);
        }
    }
    kprintf("You said: %s\n", input);
}
