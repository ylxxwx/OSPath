#ifndef TASK_THREAD_H
#define TASK_THREAD_H

#include "type.h"
#include "process.h"
#include "isr.h"

#define KERNEL_MAIN_STACK_TOP 0xF0000000
#define THREAD_STACK_MAGIC 0x32602021
#define THREAD_DEFAULT_PRIORITY 10

#define KERNEL_STACK_SIZE 8192

#define EFLAGS_MBS (1 << 1)
#define EFLAGS_IF_0 (0 << 9)
#define EFLAGS_IF_1 (1 << 9)
#define EFLAGS_IOPL_0 (0 << 12)
#define EFLAGS_IOPL_3 (3 << 12)

typedef void thread_func();

typedef enum task_status
{
  TASK_UNINIT,
  TASK_RUNNING,
  TASK_READY,
  TASK_BLOCKED,
  TASK_WAITING,
  TASK_HANGING,
  TASK_DEAD
} task_status_t;

typedef struct task_struct
{
  // kernel stack pointer
  uint32 kernel_esp;
  uint32 kernel_stack;
  uint32 id;
  char name[32];
  uint8 priority;
  task_status_t status;
  // timer ticks this thread has been running for.
  uint32 ticks;
  // pointer to its process
  // struct process_struct* process;
  struct pcb *process;
  // user stack
  uint32 user_stack;
  int32 user_stack_index;
  // need reschedule flag
  bool need_reschedule;
  // preempt (disable) count
  uint32 preempt_count;
} tcb_t;

struct switch_stack
{
  // Switch context.
  uint32 edi;
  uint32 esi;
  uint32 ebp;
  uint32 ebx;
  uint32 edx;
  uint32 ecx;
  uint32 eax;

  // For thread first run.
  uint32 thread_entry_eip;

  void(*unused_retaddr);
  thread_func *function;
};
typedef struct switch_stack switch_stack_t;

extern int cur_task_id;
extern int next_task_id;
extern tcb_t tcb[16];
// ****************************************************************************
void init_task();

void mov_task_ready(int task_id);
void mov_task_wait(int task_id);
void mov_cur_task_dead();
void show_task();
void show_cur_task(char *);
// Create a new thread.
tcb_t *create_thread(pcb_t *, char *name, thread_func function, uint32 priority, uint8 user);
tcb_t *create_user_thread(pcb_t *, char *name, void *function);
uint32 prepare_user_stack(tcb_t *thread, uint32 stack_top, uint32 argc, char **argv, uint32 return_addr);

tcb_t *fork_crt_thread();
tcb_t *get_cur_thread();
void destroy_thread(tcb_t *thread);

tcb_t *create_kernel_thread(pcb_t *, char *name, void *function);
int get_next_ready_task();
#endif
