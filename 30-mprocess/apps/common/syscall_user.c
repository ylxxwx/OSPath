#include "syscall_user.h"

#define SYS_SLEEP 0x00
#define SYS_INPUT 0x04
#define SYS_CLS 0x05
#define SYS_LS 0x06
#define SYS_PWD 0x09
#define SYS_EXIT 0x0A
#define SYS_FORK 0x0C

s32 sys_general(u32 sys_id, u32 param1, u32 param2)
{
    s32 ret = 0;
    __asm__ __volatile__(
        "push %%ebx;"
        "int $0x80;"
        "pop %%ebx;"
        "mov %%eax, %0;"
        : "=m"(ret)
        : "a"(sys_id), "b"(param1), "c"(param2));
    return ret;
}

#define SYS_PARAM_ZERO(func_name, sys_id) \
    s32 func_name()                       \
    {                                     \
        return sys_general(sys_id, 0, 0); \
    }
#define SYS_PARAM_ONE(func_name, sys_id)      \
    s32 func_name(u32 param)                  \
    {                                         \
        return sys_general(sys_id, param, 0); \
    }
#define SYS_PARAM_TWO(func_name, sys_id)            \
    s32 func_name(u32 param1, u32 param2)           \
    {                                               \
        return sys_general(sys_id, param1, param2); \
    }

SYS_PARAM_ZERO(sys_sleep, SYS_SLEEP)
SYS_PARAM_ZERO(sys_cls, SYS_CLS)
SYS_PARAM_ZERO(sys_pwd, SYS_PWD)
SYS_PARAM_ZERO(sys_exit, SYS_EXIT)
SYS_PARAM_ZERO(sys_fork, SYS_FORK)
SYS_PARAM_ONE(sys_input, SYS_INPUT)
SYS_PARAM_ONE(sys_ls, SYS_LS)

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
