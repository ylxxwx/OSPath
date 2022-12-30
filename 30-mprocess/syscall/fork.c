#include "syscall.h"
#include "isr.h"
#include "panic.h"
#include "screen.h"
#include "process.h"
#include "task.h"

s32 ksys_fork(registers_t *regs)
{
    kprintf("sys calll: fork:\n");
    // Create a new process, with page directory cloned from this process.
    pcb_t *process = create_process("new", /* is_kernel_process = */ false);

    pcb_t *parent_process = get_cur_thread()->process;
    process->parent = parent_process;
    // add_child_process(parent_process, process);
    tcb_t *thread = fork_crt_thread();
    if (thread == 0)
        panic("clone thread failed.");

    thread->process = process;

    thread->status = TASK_READY;

    return 1;
}
