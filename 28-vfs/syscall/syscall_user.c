#include "syscall_user.h"

s32 sys_sleep() {
    s32 ret = 0;
    __asm__ __volatile__(
        "movl $0, %%eax;"
        "int $0x80;"
        "mov %%eax, %0"
        : "=m"(ret)
        :);
    return ret;
}

s32 sys_mount_root(u32 major, u32 min, u32 partition) {
    s32 ret = 0;
    __asm__ __volatile__(
        "push %%ebx;"
        "movl $2, %%eax;"
        "int $0x80;"
        "pop %%ebx;"
        "mov %%eax, %0;"
        : "=m"(ret)
        : "b"(major),"c"(min),"d"(partition));
    return ret;
}

s32 sys_print(char* fmt, void *arg) {
    s32 ret = 0;
    __asm__ __volatile__(
        "push %%ebx;"
        "movl $3, %%eax;"
        "int $0x80;"
        "pop %%ebx;"
        "mov %%eax, %0;"
        : "=m"(ret)
        : "b"(fmt),"c"(arg));
    return ret;
}

s32 sys_std_input(char *buf) {
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

s32 sys_clear_screen() {
    s32 ret = 0;
    __asm__ __volatile__(
        "movl $5, %%eax;"
        "int $0x80;"
        "mov %%eax, %0;"
        : "=m"(ret)
        :);
    return ret;
}

s32 sys_ls() {
    s32 ret = 0;
    __asm__ __volatile__(
        "movl $6, %%eax;"
        "int $0x80;"
        "mov %%eax, %0;"
        : "=m"(ret)
        :);
    return ret;
}

s32 sys_cd_dir(char *dir) {
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

s32 sys_read_file(char *fn, char *buf) {
    s32 ret = 0;
    __asm__ __volatile__(
        "push %%ebx;"
        "movl $8, %%eax;"
        "int $0x80;"
        "pop %%ebx;"
        "mov %%eax, %0;"
        : "=m"(ret)
        : "b"(fn),"c"(buf));
    return ret;
}
