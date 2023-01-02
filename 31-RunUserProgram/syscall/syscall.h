#ifndef SYSCALL_H
#define SYSCALL_H

#include "type.h"
#include "isr.h"

typedef s32 (*sys_func)(registers_t *regs);

s32 ksys_pwd(registers_t *regs);
s32 ksys_read_file(registers_t *regs);
s32 ksys_cd_dir(registers_t *regs);
s32 ksys_ls(registers_t *regs);
s32 ksys_mount(registers_t *regs);
s32 ksys_filesize(registers_t *regs);
s32 ksys_input(registers_t *regs);
s32 ksys_print(registers_t *regs);
s32 ksys_fork(registers_t *regs);
s32 ksys_exit(registers_t *regs);
s32 ksys_show_task(registers_t *regs);
s32 ksys_clean_task(registers_t *regs);
s32 ksys_waitforpid(registers_t *regs);
s32 ksys_exec(registers_t *regs);

void init_syscalls();

#endif