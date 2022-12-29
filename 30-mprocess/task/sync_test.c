#include "yieldlock.h"
#include "task.h"
#include "screen.h"

yieldlock_t count_lock;

int count = 0;
int lock = 0;
void set_lock() {
  if (lock) {
    lock = 0;
    yieldlock_unlock(&count_lock);
  }
  else 
    lock = 1;
  kprintf("Lock:%d\n", lock);
}

void first_thread() {
  int result = 0;
      
  while (1) {    
      if (count < 0xFFFFFF) {
        if (lock)
          yieldlock_lock(&count_lock);
        int tmp = count;
        asm("hlt");
        count++;
        if (tmp + 1 !=  count) {
          kprintf("first thread running switch out in process: %d:%d\n", tmp, count);  
        }
        if (lock)
           yieldlock_unlock(&count_lock);
      } else {
        asm("hlt");
      }
  }
}

void second_thread() {
  int result = 0;
  while (1) {    
      if (count > 0) {
        if (lock)
          yieldlock_lock(&count_lock);
        int tmp = count;
        asm("hlt");
        count--;
        if (tmp !=  count+1) {
          kprintf("second thread running switch out in process: %d:%d\n", tmp, count);  
        }
        if (lock)
          yieldlock_unlock(&count_lock);
      } else {
        asm("hlt");
      }
  }
}

bool sync_test = false;
void start_sync_test() {
    if (sync_test)
        return;
    sync_test = true;

    yieldlock_init(&count_lock);
    
    pcb_t *test_process = create_process("test_process", false);

    tcb_t* thread = create_thread( test_process,
      "test", first_thread, THREAD_DEFAULT_PRIORITY, false);

    tcb_t* secthread = create_thread( test_process,
      "test", second_thread, THREAD_DEFAULT_PRIORITY, false);

    thread->status = TASK_READY;
    secthread->status = TASK_READY;
    
    kprintf("kernel esp: %x\n", thread->kernel_esp);
}
