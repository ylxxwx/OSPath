#include "syscall.h"
#include "isr.h"
#include "panic.h"
#include "screen.h"
#include "task.h"
#include "process.h"
#include "schedule.h"
u32 debug = 0;
s32 ksys_exit(registers_t *regs)
{
    // show_cur_task("exit:");
    // kprintf("enter ksys_exit\n");
    mov_cur_task_dead();
    // mov_task_wait(get_cur_thread()->id);
    //  panic("want to panic");
    // debug = 1;
    do_context_switch();
    // kprintf("will not reach here ksys_exit.\n");
}

s32 ksys_fork(registers_t *regs)
{
    // show_cur_task("fork:");
    // Create a new process, with page directory cloned from this process.
    pcb_t *process = create_process("new", /* is_kernel_process = */ false);
    tcb_t *thread = fork_crt_thread();
    if (thread == 0)
        panic("clone thread failed.");
    add_thread_to_process(process, thread);

    pcb_t *parent_process = get_cur_thread()->process;
    process->parent = parent_process;
    // add_child_process(parent_process, process);

    thread->process = process;

    thread->status = TASK_READY;
    // kprintf("ksys_fork return 1\n");
    return process->id;
}

s32 ksys_exec(registers_t *regs)
{
    char *path = (char *)regs->ebx;
    cmd_t *cmd = (cmd_t *)regs->ecx;
    // kprintf("ksys_exec, argc:%d, argv[0]:%s\n", cmd->argc, cmd->argv[0]);
    process_exec(path, cmd);
    return 0;
}

s32 ksys_waitforpid(registers_t *regs)
{
    u32 pid = regs->ebx;
    waitforpid(pid);
    return 0;
}

s32 ksys_show_task(registers_t *regs)
{
    show_task();
    show_process();
    return 0;
}

s32 ksys_clean_task(registers_t *regs)
{
    clean_task();
    clean_process();
}