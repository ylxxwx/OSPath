// syscall.c -- Defines the implementation of a system call system.
// Written for JamesM's kernel development tutorials.

#include "syscall.h"
#include "isr.h"
#include "harddisk.h"
#include "panic.h"
#include "schedule.h"
#include "screen.h"
#include "keyboard.h"

typedef s32 (*sys_func) (registers_t *regs);

static s32 syscall_handler(registers_t *regs);

s32 sys_kstd_input(registers_t *regs) {
    u8 * buf = regs->ebx;
    return kb_read(buf);
}

s32 sys_kprint(registers_t *regs) {
    char *fmt = (char *)regs->ebx;
    void *args = (void *)regs->ecx;
    kprintf_args(fmt, args);
    return 3;
}

s32 sys_sleep(registers_t *regs) {
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
    
    init_hd();
    return 9;
}

static void *syscalls[] =
{
   &sys_sleep,
   &sys_sleep,
   &mount_hd,
   &sys_kprint,
   &sys_kstd_input,
   &sys_kstd_clear_screen,
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
        return sys_sleep(r);
    }
    return -1;
}

void init_syscalls()
{
   // Register our syscall handler.
   register_interrupt_handler (0x80, &sys_handler);
}
