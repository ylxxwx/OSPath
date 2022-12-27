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
