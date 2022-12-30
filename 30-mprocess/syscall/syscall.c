#include "syscall.h"
#include "isr.h"
#include "panic.h"
#include "schedule.h"
#include "screen.h"
#include "keyboard.h"
#include "vfs.h"

s32 ksys_nil(registers_t *regs)
{
    kprintf("sys call nil serice.\n");
    return 0;
}

s32 ksys_sleep(registers_t *regs)
{
    kprintf("sys call sleep...\n");
    do_context_switch();
    return 2;
}

s32 sys_kstd_clear_screen(registers_t *regs)
{
    clear_screen();
    return 3;
}

static void *syscalls[] = {
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
    &ksys_nil,
    &ksys_nil,
    &ksys_fork,
};

s32 sys_handler(registers_t *r)
{
    if (r->err_code < sizeof(syscalls))
    {
        sys_func func = syscalls[r->err_code];
        s32 ret = func(r);
        return ret;
    }
    else
    {
        return ksys_sleep(r);
    }
    return -1;
}

void init_syscalls()
{
}
