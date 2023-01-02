*Concepts you may want to Google beforehand: Process*

**Goal: Move the thread to user space and run a Int 80 to call an interrupt**
1. boot from iso only sice the kernel is too big (36k).

2. first step,
    a) support one thread per process.
    to do list
     1) sys_fork                   done.
     2) copy page table            done.
     3) new thread?                done.
     3) switch context             done.
     4) copy on write              done.
     5) sys_exec                   done.
     6) exit.                      done.

3. Lesson learnt.
     1) cow(copy-on-write) only triggered by user space. If kernel tries to write the shared page, there is no cow interruption.
     2) In kernel, we free the user space pages, and then write the virtual address, it will trigger no page exception.
     3) in sys_exec, create a new thread to run the new program.
