#include "screen.h"
#include "task.h"
#include "mem.h"
#include "gdt.h"
#include "frame.h"
#include "schedule.h"
#include "vfs.h"
#include "process.h"

#define MAX_TASK_ID 16
extern tss_entry_t tss_entry;
extern void switch_to_user_mode();
extern void syscall_fork_exit();
int cur_avail_id = 0;

int cur_task_id = 0;
tcb_t tcb[MAX_TASK_ID];

extern void resume_thread();

tcb_t *fork_crt_thread()
{
  tcb_t *cur_thread = get_cur_thread();

  int id = cur_avail_id++;
  tcb_t *thread = &tcb[id];
  if (thread == 0)
  {
    return 0;
  }
  kmemcpy((void *)thread, (void *)get_cur_thread(), sizeof(struct task_struct));

  thread->id = id;

  // char buf[32];
  // sprintf(buf, "thread-%u", thread->id);
  // strcpy(thread->name, buf);

  thread->ticks = 0;

  // allocate kernel stack
  uint32 kernel_stack = (uint32)kmalloc_a(KERNEL_STACK_SIZE, 1);
  thread->kernel_stack = kernel_stack;
  kmemcpy((void *)kernel_stack, (void *)cur_thread->kernel_stack, KERNEL_STACK_SIZE);

  thread->kernel_esp =
      kernel_stack + KERNEL_STACK_SIZE - (sizeof(interrupt_stack_t) + sizeof(switch_stack_t));
  // monitor_printf("fork kernel_esp = %x\n", thread->kernel_esp);

  interrupt_stack_t *interrupt_stack =
      (interrupt_stack_t *)(thread->kernel_esp + sizeof(switch_stack_t));
  // monitor_printf("user eip = %x\n", interrupt_stack->eip);

  switch_stack_t *switch_stack = (switch_stack_t *)thread->kernel_esp;
  switch_stack->thread_entry_eip = (uint32)syscall_fork_exit;

  return thread;
}

tcb_t *get_cur_thread()
{
  return &tcb[cur_task_id];
}

void mov_task_ready(int task_id)
{
  tcb[task_id].status = TASK_READY;
}

void mov_task_wait(int task_id)
{
  tcb[task_id].status = TASK_WAITING;
}

static void kernel_main_thread()
{
  prepare_first_user_program();

  pcb_t *init_process = create_process("init_process", false);
  tcb_t *user_thread = create_user_thread(init_process, "main", (void *)/*user_func*/ 0xA0000000);
  user_thread->status = TASK_READY;

  int count = 0;
  while (true)
  {
    count++;
    if (count % 100 == 99)
    {
      // kprintf("CPU IDLE\n");
      // kprintf("task state: %d-%d-%d-%d\n", tcb[0].status, tcb[1].status, tcb[2].status, tcb[3].status);
      count = 0;
    }
    asm("hlt");
  }
}

void prepare_first_user_program()
{
  u8 *program_addr = (u8 *)0xA0000000;
  int ret = vfs_read_file("/bin/shell", program_addr);
  kprintf("Load shell OK(ret).\n");
}

void init_task()
{
  cur_task_id = 0;
  for (int idx = 0; idx < MAX_TASK_ID; idx++)
  {
    tcb[idx].id = idx;
    tcb[idx].status = TASK_DEAD;
  }
  cur_avail_id = 0;

  pcb_t *main_process = create_process("kernel_main_process", /* is_kernel_process = */ true);

  tcb_t *main_thread = create_kernel_thread(main_process, "main", (void *)kernel_main_thread);
  main_thread->status = TASK_READY;

  switch_page_directory(main_process->page_dir);
  panic("init task go to main thread.");
  // kprintf("task Create state: %d-%d-%d-%d\n", tcb[0].status, tcb[1].status, tcb[2].status, tcb[3].status);
  asm volatile(
      "movl %0, %%esp; \
    jmp resume_thread"
      :
      : "g"(main_thread->kernel_esp)
      : "memory");
}

int get_next_ready_task()
{
  for (int idx = 1; idx < MAX_TASK_ID; idx++)
  {
    tcb_t *task = &tcb[idx];
    if (task->status == TASK_READY)
    {
      return idx;
    }
  }
  return 0;
}

void schedule_thread_exit()
{
}

static void kernel_thread(thread_func *function)
{
  function();
  schedule_thread_exit();
}

tcb_t *create_thread(pcb_t *pcb, char *name, thread_func function, u32 priority, uint8 user)
{
  u32 id = cur_avail_id++;
  // kprintf("create thread avil id: %d \n", cur_avail_id);
  tcb_t *thread = &tcb[id];
  kmemset((u8 *)thread, 0, sizeof(tcb_t));

  // kprintf("create thread id: %d \n", id);
  thread->id = id;
  if (name != 0)
  {
    kmemcpy(thread->name, name, 4);
  }
  else
  {
    kmemcpy(thread->name, "thrd", 4);
  }
  // kprintf("create thread :%d\n", thread->id);

  thread->status = TASK_DEAD;
  thread->ticks = 0;
  thread->priority = priority;
  thread->user_stack_index = -1;
  thread->process = pcb;
  // Init thread stack.
  uint32 kernel_stack = (uint32)kmalloc_a(KERNEL_STACK_SIZE, 1);
  // kprintf("stack memory: %x\n", kernel_stack);
  kmemset((void *)kernel_stack, 0, KERNEL_STACK_SIZE);
  thread->kernel_stack = kernel_stack;

  thread->kernel_esp =
      kernel_stack + (u32)KERNEL_STACK_SIZE - (u32)(sizeof(interrupt_stack_t) + sizeof(switch_stack_t));
  switch_stack_t *switch_stack = (switch_stack_t *)thread->kernel_esp;
  // kprintf("thread kernel stack:%x, top:%x\n", kernel_stack, kernel_stack+(u32)KERNEL_STACK_SIZE);

  switch_stack->edi = 0;
  switch_stack->esi = 0;
  switch_stack->ebp = 0;
  switch_stack->ebx = 0;
  switch_stack->edx = 0;
  switch_stack->ecx = 0;
  switch_stack->eax = 0;

  switch_stack->thread_entry_eip = (uint32)kernel_thread;
  // kprintf("task kernel function start.\n");
  switch_stack->function = function;
  // kprintf("task kernel function end.\n");

  // For user thread, there are some more steps to do:
  //  - Prepare the kernel interrupt stack context for later to switch to user mode.
  //  - Allocate a user space stack;
  //  - Prepare the user stack:
  //    - 1. Copy args to stack;
  //    - 2. Set the return address as thread_exit syscall so that it can exit normally;

  if (user)
  {
    switch_stack->thread_entry_eip = (u32)switch_to_user_mode; //(uint32)return_usermode;//switch_to_user_mode;

    interrupt_stack_t *interrupt_stack =
        (interrupt_stack_t *)((uint32)thread->kernel_esp + sizeof(switch_stack_t));
    // data segemnts
    interrupt_stack->ds = SELECTOR_U_DATA;

    // general regs
    interrupt_stack->edi = 0;
    interrupt_stack->esi = 0;
    interrupt_stack->ebp = 0;
    interrupt_stack->esp = 0;
    interrupt_stack->ebx = 0;
    interrupt_stack->edx = 0;
    interrupt_stack->ecx = 0;
    interrupt_stack->eax = 0;

    // user-level code env
    // kprintf("task us function start.\n");
    interrupt_stack->eip = (u32)function;
    // kprintf("task us function end.\n");
    interrupt_stack->cs = SELECTOR_U_CODE;
    interrupt_stack->eflags = EFLAGS_IOPL_0 | EFLAGS_MBS | EFLAGS_IF_1;

    // user stack
    interrupt_stack->user_ss = SELECTOR_U_DATA;
  }

  return thread;
}

void destroy_thread(tcb_t *thread)
{
  // id_pool_free_id(&thread_id_pool, thread->id);
  kfree((void *)thread->kernel_stack);
  // kfree(thread);
}

tcb_t *create_kernel_thread(pcb_t *pcb, char *name, void *function)
{
  return create_thread(pcb, name, function, THREAD_DEFAULT_PRIORITY, false);
}

#define USER_STACK_TOP 0xC0000000 // 0xC0000000 - 4MB
#define USER_STACK_SIZE 65536     // 64KB

void thread_exit()
{
  kprintf("Thread exit\n");
}

uint32 prepare_user_stack(
    tcb_t *thread, uint32 stack_top, uint32 argc, char **argv, uint32 return_addr)
{
  uint32 total_argv_length = 0;
  // Reserve space to copy the argv strings.
  stack_top -= 8;
  *((uint32 *)stack_top) = return_addr;

  interrupt_stack_t *interrupt_stack =
      (interrupt_stack_t *)((uint32)thread->kernel_esp + sizeof(switch_stack_t));
  interrupt_stack->user_esp = stack_top;
  return stack_top;
}

tcb_t *create_user_thread(pcb_t *pcb, char *name, void *function)
{
  tcb_t *task = create_thread(pcb, name, function, THREAD_DEFAULT_PRIORITY, true);
  int stack_index = 1;
  task->user_stack_index = stack_index;
  uint32 thread_stack_top = USER_STACK_TOP - stack_index * USER_STACK_SIZE;
  map_address((uint32)thread_stack_top - PAGE_SIZE, 0, 1);
  prepare_user_stack(task, thread_stack_top, 0, 0, (uint32)thread_exit);
  return task;
}
