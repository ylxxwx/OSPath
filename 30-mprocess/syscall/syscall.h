#ifndef SYSCALL_H
#define SYSCALL_H

#include "type.h"
#include "isr.h"

typedef s32 (*sys_func)(registers_t *regs);

s32 ksys_pwd(registers_t *regs);
s32 ksys_read_file(registers_t *regs);
s32 ksys_cd_dir(registers_t *regs);
s32 ksys_ls(registers_t *regs);
s32 mount_hd(registers_t *regs);

s32 sys_kstd_input(registers_t *regs);
s32 sys_kprint(registers_t *regs);

s32 ksys_fork(registers_t *regs);

void init_syscalls();

#endif