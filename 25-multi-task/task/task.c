#include "screen.h"
#include "task.h"
#include "mem.h"

extern void syscall_fork_exit();
extern void switch_to_user_mode();
int cur_avail_id = 0;

int cur_task_id = 1;
int next_task_id = 0;
tcb_t tcb[16];

extern void resume_thread();
//static id_pool_t thread_id_pool;

//void init_task_manager() {
//  id_pool_init(&thread_id_pool, 2048, 32768);
//}
void schedule_thread_exit() {
}

static void kernel_thread(thread_func* function) {
  function();
  schedule_thread_exit();
}

tcb_t* init_thread(char* name, thread_func function, u32 priority, uint8 user) {
  u32 id = cur_avail_id++;
  tcb_t *thread = &tcb[id];
  kmemset(thread, 0, sizeof(tcb_t));

  thread->id = id;
  if (name != 0) {
    kmemcpy(thread->name, name, 4);
  } else {
    kmemcpy(thread->name, "thrd", 4);
  }
  kprintf("create thread :%d\n", thread->id);

  thread->status = TASK_READY;
  thread->ticks = 0;
  thread->priority = priority;
  thread->user_stack_index = -1;

  // Init thread stack.
  uint32 kernel_stack = (uint32)kmalloc_a(KERNEL_STACK_SIZE, 1);
  kprintf("stack memory: %x\n", kernel_stack);
  kmemset((void*)kernel_stack, 0, KERNEL_STACK_SIZE);
  thread->kernel_stack = kernel_stack;

  thread->kernel_esp =
      kernel_stack + (u32)KERNEL_STACK_SIZE - (u32)(sizeof(interrupt_stack_t) + sizeof(switch_stack_t));
  switch_stack_t* switch_stack = (switch_stack_t*)thread->kernel_esp;

  switch_stack->edi = 0;
  switch_stack->esi = 0;
  switch_stack->ebp = 0;
  switch_stack->ebx = 0;
  switch_stack->edx = 0;
  switch_stack->ecx = 0;
  switch_stack->eax = 0;

  switch_stack->thread_entry_eip = (uint32)kernel_thread;
  switch_stack->function = function;

  // For user thread, there are some more steps to do:
  //  - Prepare the kernel interrupt stack context for later to switch to user mode.
  //  - Allocate a user space stack;
  //  - Prepare the user stack:
  //    - 1. Copy args to stack;
  //    - 2. Set the return address as thread_exit syscall so that it can exit normally;
/*
  if (user) {
    switch_stack->thread_entry_eip = (uint32)switch_to_user_mode;

    interrupt_stack_t* interrupt_stack =
        (interrupt_stack_t*)((uint32)thread->kernel_esp + sizeof(switch_stack_t));

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
    interrupt_stack->eip = (uint32)function;
    interrupt_stack->cs = SELECTOR_U_CODE;
    interrupt_stack->eflags = EFLAGS_IOPL_0 | EFLAGS_MBS | EFLAGS_IF_1;

    // user stack
    interrupt_stack->user_ss = SELECTOR_U_DATA;
  }
*/
   return thread;
}

// Copy args to user stack and set return address.
// Return the stack top for thread function to run.
uint32 prepare_user_stack(
    tcb_t* thread, uint32 stack_top, uint32 argc, char** argv, uint32 return_addr) {
  uint32 total_argv_length = 0;
  // Reserve space to copy the argv strings.
  for (int i = 0; i < argc; i++) {
    char c;
    int j = 0;
    while ((c != argv[i][j++]) != '\0') {
      total_argv_length++;
    }
    total_argv_length++;
  }
  stack_top -= total_argv_length;
  stack_top = stack_top / 4 * 4;

  char* args[argc + 1];
  args[0] = thread->name;

  char* argv_chars_addr = (char*)stack_top;
  for (int i = 0; i < argc; i++) {
    uint32 length = 0;//strcpy(argv_chars_addr, argv[i]);
    args[i + 1] = (char*)argv_chars_addr;
    argv_chars_addr += (length + 1);
  }

  // Copy args[] array to stack.
  stack_top -= ((argc + 1) * 4);
  uint32 argv_start = stack_top;;
  for (int i = 0; i < argc + 1; i++) {
    *((char**)(argv_start + i * 4)) = args[i];
  }

  stack_top -= 4;
  *((uint32*)stack_top) = argv_start;
  stack_top -= 4;
  *((uint32*)stack_top) = argc + 1;

  // Set thread return address.
  stack_top -= 4;
  *((uint32*)stack_top) = return_addr;
  //monitor_printf("%x return_addr = %x\n", stack_top, return_addr);

  //interrupt_stack_t* interrupt_stack =
  //    (interrupt_stack_t*)((uint32)thread->kernel_esp + sizeof(switch_stack_t));
  //interrupt_stack->user_esp = stack_top;
  return stack_top;
}

tcb_t* fork_crt_thread() {
/*
  tcb_t* crt_thread = get_crt_thread();

  tcb_t* thread = (tcb_t*)kmalloc(sizeof(struct task_struct));
  if (thread == nullptr) {
    return nullptr;
  }
  memcpy((void*)thread, (void*)get_crt_thread(), sizeof(struct task_struct));

  uint32 id;
  if (!id_pool_allocate_id(&thread_id_pool, &id)) {
    return nullptr;
  }
  thread->id = id;

  char buf[32];
  sprintf(buf, "thread-%u", thread->id);
  strcpy(thread->name, buf);

  thread->ticks = 0;

  // allocate kernel stack
  uint32 kernel_stack = (uint32)kmalloc_aligned(KERNEL_STACK_SIZE);
  thread->kernel_stack = kernel_stack;
  memcpy((void*)kernel_stack, (void*)crt_thread->kernel_stack, KERNEL_STACK_SIZE);

  thread->kernel_esp =
      kernel_stack + KERNEL_STACK_SIZE - (sizeof(interrupt_stack_t) + sizeof(switch_stack_t));
  //monitor_printf("fork kernel_esp = %x\n", thread->kernel_esp);

  interrupt_stack_t* interrupt_stack =
      (interrupt_stack_t*)(thread->kernel_esp + sizeof(switch_stack_t));
  //monitor_printf("user eip = %x\n", interrupt_stack->eip);

  switch_stack_t* switch_stack = (switch_stack_t*)thread->kernel_esp;
  switch_stack->thread_entry_eip = (uint32)syscall_fork_exit;

  return thread;
*/
  return 0;
}

void destroy_thread(tcb_t* thread) {
  //id_pool_free_id(&thread_id_pool, thread->id);
  kfree((void*)thread->kernel_stack);
  //kfree(thread);
}
