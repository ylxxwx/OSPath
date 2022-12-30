#include "syscall.h"
#include "isr.h"
#include "panic.h"
#include "screen.h"
#include "process.h"
#include "task.h"
#include "schedule.h"

s32 ksys_exit(registers_t *regs)
{
    kprintf("ksys_exit.\n");
    mov_cur_task_dead();
    do_context_switch();
    kprintf("will not reach here ksys_exit.\n");
}

s32 ksys_fork(registers_t *regs)
{
    kprintf("sys calll: fork:\n");

    tcb_t *thread = fork_crt_thread();
    if (thread == 0)
        panic("clone thread failed.");

    // Create a new process, with page directory cloned from this process.
    pcb_t *process = create_process("new", /* is_kernel_process = */ false);

    pcb_t *parent_process = get_cur_thread()->process;
    process->parent = parent_process;
    // add_child_process(parent_process, process);

    thread->process = process;

    thread->status = TASK_READY;

    return 1;
}
