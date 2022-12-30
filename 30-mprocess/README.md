*Concepts you may want to Google beforehand: Process*

**Goal: Move the thread to user space and run a Int 80 to call an interrupt**
1. boot from iso only sice the kernel is too big 36k.

2. first step,
    a) support one thread per process.
    to do list
     1) sys_fork                   done.
     2) copy page table            done.
     3) new thread?                
     3) switch context             done.
     4) copy on write              done.
     5) sys_exec
     6) exit.
