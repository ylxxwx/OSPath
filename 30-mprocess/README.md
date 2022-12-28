*Concepts you may want to Google beforehand: Process*

**Goal: Move the thread to user space and run a Int 80 to call an interrupt**
1. boot from iso only sice the kernel is too big 36k.

2. first step,
    a) support one thread per process.
    to do list
     1) sys_fork
     2) copy page table
     3) new thread?
     3) switch context
     4) copy on write
     5) sys_exec
