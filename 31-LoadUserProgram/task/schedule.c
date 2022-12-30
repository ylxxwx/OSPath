#include "gdt.h"
#include "inter.h"
#include "schedule.h"
#include "task.h"
#include "process.h"

extern void context_switch(tcb_t *crt, tcb_t *next);

void do_context_switch()
{
  tcb_t *old_thread = &tcb[cur_task_id];
  int next_id = get_next_ready_task();
  // kprintf("do cs, old id:%d, next:%d\n", cur_task_id, next_id);
  if (next_id == cur_task_id)
  {
    if (cur_task_id == 0)
      return;
    next_id = 0;
  }

  disable_interrupt();
  tcb_t *next_thread = &tcb[next_id];

  // Switch out current running thread.
  if (old_thread->status == TASK_RUNNING)
  {
    old_thread->status = TASK_READY;
  }
  else
  {
    // kprintf("context (%x) switch %d to %d\n", old_thread->status, old_thread->id, next_thread->id);
    // kprintf("context esp(%x) switch to esp(%x)\n", old_thread->kernel_esp, next_thread->kernel_esp);
    // kprintf("context (%x) switch %x\n", old_thread->process->id, next_thread->process->id);
  }
  old_thread->ticks = 0;
  old_thread->need_reschedule = false;

  // Switch in next thread.
  next_thread->status = TASK_RUNNING;
  cur_task_id = next_id;

  // kprintf("context switch from :%x to :%x\n", old_thread->id, next_thread->id);
  if (old_thread->process != next_thread->process)
  {
    switch_page_directory(next_thread->process->page_dir);
  }
  // kprintf("update tss to: %x\n", next_thread->kernel_stack + KERNEL_STACK_SIZE);
  update_tss_esp(next_thread->kernel_stack + KERNEL_STACK_SIZE);
  // kprintf("update done tss to: %x\n", next_thread->kernel_stack + KERNEL_STACK_SIZE);
  context_switch(old_thread, next_thread);
}

void schedule_thread_yield()
{
  disable_interrupt();
  do_context_switch();
}
