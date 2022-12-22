#include "schedule.h"
#include "task.h"

extern void context_switch(tcb_t* crt, tcb_t* next);

void do_context_switch() {
  tcb_t* old_thread = &tcb[cur_task_id];
  int next_id = get_next_ready_task();
  //kprintf("do cs, old id:%d, next:%d\n", cur_task_id, next_id);
  
  if (next_id == 0 || next_id == cur_task_id)
    return;
    
  tcb_t* next_thread = &tcb[next_id];

  // Switch out current running thread.
  if (old_thread->status == TASK_RUNNING) {
    old_thread->status = TASK_READY;
  }
  old_thread->ticks = 0;
  old_thread->need_reschedule = false;

  // Switch in next thread.
  next_thread->status = TASK_RUNNING;
  cur_task_id = next_id;

  update_tss_esp(next_thread->kernel_stack + KERNEL_STACK_SIZE);

  //monitor_printf("thread %u switch to thread %u\n", old_thread->id, next_thread->id);
  context_switch(old_thread, next_thread);
}

void schedule_thread_yield() {
  disable_interrupt();
  do_context_switch();
}
