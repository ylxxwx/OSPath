#include "syscall.h"
#include "isr.h"
//#include "harddisk.h"
#include "panic.h"
#include "schedule.h"
#include "screen.h"
#include "keyboard.h"
#include "vfs.h"

typedef s32 (*sys_func) (registers_t *regs);

static s32 syscall_handler(registers_t *regs);
s32 ksys_pwd(registers_t *regs) {
    vfs_pwd();
    return 9;
}

s32 ksys_read_file(registers_t *regs) {
    char *fn = (char *)regs->ebx;
    void *buf = (void *)regs->ecx;
    int sz = vfs_read_file(fn, buf);
    return sz;
}

s32 ksys_cd_dir(registers_t *regs) {
    u8 * dir = (u8 *)regs->ebx;
    cd_dir(dir);
    return 7;
}

s32 sys_kstd_input(registers_t *regs) {
    u8 * buf = (u8 *)regs->ebx;
    return kb_read(buf);
}

s32 sys_kprint(registers_t *regs) {
    char *fmt = (char *)regs->ebx;
    void *args = (void *)regs->ecx;
    kprintf_args(fmt, args);
    return 3;
}

s32 ksys_sleep(registers_t *regs) {
    //kprintf("sys call sleep...\n");
    do_context_switch();
    return 2;
}

s32 sys_kstd_clear_screen(registers_t *regs) {
    //kprintf("sys call sleep...\n");
    clear_screen();
    return 3;
}

s32 mount_hd(registers_t *regs) {
    kprintf("sys mount maj:%d, min:%d, part:%d\n", regs->ebx, regs->ecx, regs->edx);
    
    //init_hd();
    disk_t t;
    t.major = 1;
    t.minor = 0;
    t.partition = 0;
    mount_root(&t);
    return 9;
}

s32 ksys_ls(registers_t *regs) {
    char *dir = (char *)(regs->ebx);
    ls_dir(dir);
    return 10;
}

static void *syscalls[] =
{
   &ksys_sleep,
   &ksys_sleep,
   &mount_hd,
   &sys_kprint,
   &sys_kstd_input,
   &sys_kstd_clear_screen,
   &ksys_ls,
   &ksys_cd_dir,
   &ksys_read_file,
   &ksys_pwd,
};

u32 num_syscalls = 3;

s32 sys_handler(registers_t *r) {
    //kprintf("system call 128:%d\n", r->err_code);
    if (r->err_code < sizeof(syscalls)) {
        sys_func func = syscalls[r->err_code];
        s32 ret = func(r);
        //kprintf("sys handle return :%d\n", ret);
        return ret;
    } else {
        return ksys_sleep(r);
    }
    return -1;
}

void init_syscalls()
{
   // Register our syscall handler.
   // register_interrupt_handler (0x80, &sys_handler);
}
