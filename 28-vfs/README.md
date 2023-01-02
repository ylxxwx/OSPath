*Concepts you may want to Google beforehand: user space, TSS*

**Goal: Move the thread to user space and run a Int 80 to call an interrupt**
TSS in the switch beetween kernel space and user space. From this task, the kernel size comes to 36k which can't be loaded by loader, but GRUB.


1. Move to user space.
2. Implement a service interrupt to print some information.
3. copy a piece code to user space to run.

comments
1. Trap gate is for system call, the previledge is different from interrupt.
2. EFLAG should enable interrupt before iret to user space.
3. In this the pages(frame) should be accessible by both kernel and user.  