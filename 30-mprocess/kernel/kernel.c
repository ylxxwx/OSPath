#include "isr.h"
#include "screen.h"
#include "string.h"
#include "gdt.h"
#include "nofreemem.h"
#include "memory.h"
#include "mem.h"
#include "task.h"
#include "hd_irs.h"
#include "syscall.h"
#include "sync_test.h"
#include "disk.h"
#include "vfs.h"

void init_rootfs() {
    disk_t disk;
    disk.major = 1;
    disk.minor = 0;
    disk.partition = 0;
    mount_root(&disk);
}

void main()
{
    init_gdt();
    isr_install();
    irq_install();
    init_memory();
    init_syscalls();
    init_hds();
    init_rootfs();
    init_task();
}
