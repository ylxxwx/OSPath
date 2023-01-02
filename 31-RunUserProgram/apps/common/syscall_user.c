#include "syscall_user.h"

#define SYS_ID_SLEEP 0x01
#define SYS_ID_TOP 0x0D
#define SYS_ID_FSIZE 0x0E
#define SYS_ID_CLS_TASK 0x0F
#define SYS_ID_WAIT4PID 0x10
#define SYS_ID_EXECUTE 0x11

s32 sys_call(u32 sys_id, u32 p1, u32 p2, u32 p3)
{
    s32 ret = 0;
    __asm__ __volatile__(
        "push %%eax;"
        "push %%ebx;"
        "push %%ecx;"
        "push %%edx;" ::);
    __asm__ __volatile__(
        "int $0x80;"
        "mov %%eax, %0;"
        : "=m"(ret)
        : "a"(sys_id), "b"(p1), "c"(p2), "d"(p3));
    __asm__ __volatile__(
        "pop %%edx;"
        "pop %%ecx;"
        "pop %%ebx;"
        "pop %%eax;" ::);
    return ret;
}

s32 sys_top()
{
    return sys_call(SYS_ID_TOP, 0, 0, 0);
}

s32 sys_sleep()
{
    return sys_call(SYS_ID_SLEEP, 0, 0, 0);
}

s32 sys_fsize(char *path_addr)
{
    return sys_call(SYS_ID_FSIZE, (u32)path_addr, 0, 0);
}

s32 sys_cls_task()
{
    return sys_call(SYS_ID_CLS_TASK, 0, 0, 0);
}

s32 sys_waitforpid(u32 pid)
{
    return sys_call(SYS_ID_WAIT4PID, pid, 0, 0);
}

s32 sys_exec(char *path, cmd_t *cmd)
{
    return sys_call(SYS_ID_EXECUTE, (u32)path, (u32)cmd, 0);
}

s32 sys_mount_root(u32 major, u32 min, u32 partition)
{
    s32 ret = 0;
    __asm__ __volatile__(
        "push %%ebx;"
        "movl $2, %%eax;"
        "int $0x80;"
        "pop %%ebx;"
        "mov %%eax, %0;"
        : "=m"(ret)
        : "b"(major), "c"(min), "d"(partition));
    return ret;
}

s32 sys_print(char *fmt, void *arg)
{
    s32 ret = 0;
    __asm__ __volatile__(
        "push %%ebx;"
        "movl $3, %%eax;"
        "int $0x80;"
        "pop %%ebx;"
        "mov %%eax, %0;"
        : "=m"(ret)
        : "b"(fmt), "c"(arg));
    return ret;
}

s32 sys_std_input(char *buf)
{
    s32 ret = 0;
    __asm__ __volatile__(
        "push %%ebx;"
        "movl $4, %%eax;"
        "int $0x80;"
        "pop %%ebx;"
        "mov %%eax, %0;"
        : "=m"(ret)
        : "b"(buf));
    return ret;
}

s32 sys_clear_screen()
{
    s32 ret = 0;
    __asm__ __volatile__(
        "movl $5, %%eax;"
        "int $0x80;"
        "mov %%eax, %0;"
        : "=m"(ret)
        :);
    return ret;
}

s32 sys_ls(char *dir)
{
    s32 ret = 0;
    __asm__ __volatile__(
        "movl $6, %%eax;"
        "int $0x80;"
        "mov %%eax, %0;"
        : "=m"(ret)
        : "b"(dir));
    return ret;
}

s32 sys_cd_dir(char *dir)
{
    s32 ret = 0;
    __asm__ __volatile__(
        "push %%ebx;"
        "movl $7, %%eax;"
        "int $0x80;"
        "pop %%ebx;"
        "mov %%eax, %0;"
        : "=m"(ret)
        : "b"(dir));
    return ret;
}

s32 sys_read_file(char *fn, char *buf)
{
    s32 ret = 0;
    __asm__ __volatile__(
        "push %%ebx;"
        "movl $8, %%eax;"
        "int $0x80;"
        "pop %%ebx;"
        "mov %%eax, %0;"
        : "=m"(ret)
        : "b"(fn), "c"(buf));
    return ret;
}

s32 sys_pwd()
{
    s32 ret = 0;
    __asm__ __volatile__(
        "movl $9, %%eax;"
        "int $0x80;"
        "mov %%eax, %0;"
        : "=m"(ret)
        :);
    return ret;
}

s32 sys_printval(u32 val)
{
    s32 ret = 0;
    __asm__ __volatile__(
        "push %%ebx;"
        "movl $0xA, %%eax;"
        "int $0x80;"
        "pop %%ebx;"
        "mov %%eax, %0;"
        : "=m"(ret)
        : "b"(val));
    return ret;
}

s32 sys_printstr(char *val)
{
    s32 ret = 0;
    __asm__ __volatile__(
        "push %%ebx;"
        "movl $0xB, %%eax;"
        "int $0x80;"
        "pop %%ebx;"
        "mov %%eax, %0;"
        : "=m"(ret)
        : "b"(val));
    return ret;
}

s32 sys_fork()
{
    s32 ret = 0;
    __asm__ __volatile__(
        "movl $0xC, %%eax;"
        "int $0x80;"
        "mov %%eax, %0;"
        : "=m"(ret));
    return ret;
}

s32 sys_exit()
{
    return sys_call(0xA, 0, 0, 0);
    /*
    s32 ret = 0;
    __asm__ __volatile__(
        "movl $0xA, %%eax;"
        "int $0x80;"
        "mov %%eax, %0;"
        : "=m"(ret));
    return ret;
    */
}
