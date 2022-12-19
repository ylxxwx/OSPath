*Concepts you may want to Google beforehand: lock mutex*

**Goal: Implement a yieldlock and mutex**

To inplement a mutex, we need a list first.

yieldock depends on the atomic operator xchange.

From the shell, we use comand "SYNC" to create two user kernel threads, which try to read/write a share parameter "count". Without yieldlock, we can see the sync issues from screen print.

With yieldlock, this issue is gone.

