#include "syscall.h"
#include "isr.h"
#include "screen.h"
#include "keyboard.h"
#include "vfs.h"

s32 ksys_pwd(registers_t *regs)
{
    vfs_pwd();
    return 9;
}

s32 ksys_read_file(registers_t *regs)
{
    char *fn = (char *)regs->ebx;
    void *buf = (void *)regs->ecx;
    int sz = vfs_read_file(fn, buf);
    return sz;
}

s32 ksys_cd_dir(registers_t *regs)
{
    u8 *dir = (u8 *)regs->ebx;
    cd_dir(dir);
    return 7;
}

s32 ksys_input(registers_t *regs)
{
    // show_cur_task("input:");
    u8 *buf = (u8 *)regs->ebx;
    return kb_read(buf);
}

s32 ksys_print(registers_t *regs)
{
    char *fmt = (char *)regs->ebx;
    void *args = (void *)regs->ecx;
    kprintf_args(fmt, args);
    return 3;
}

s32 ksys_mount(registers_t *regs)
{
    kprintf("sys mount maj:%d, min:%d, part:%d\n", regs->ebx, regs->ecx, regs->edx);

    disk_t t;
    t.major = 1;
    t.minor = 0;
    t.partition = 0;
    mount_root(&t);
    return 9;
}

s32 ksys_ls(registers_t *regs)
{
    char *dir = (char *)(regs->ebx);
    ls_dir(dir);
    return 10;
}
