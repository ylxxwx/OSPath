#include "timer.h"
#include "isr.h"
#include "ports.h"
#include "function.h"
#include "task.h"

u32 tick = 0;

static void timer_callback(registers_t *regs) {    
/*
    if (cur_task_id != 0 && cur_task_id != 1) {
        kprint("time task id error: ");
        kprint_hex(cur_task_id);
        kprint("\n");
        cur_task_id = 0;
        next_task_id = 1;
    }
    if (cur_task_id == next_task_id) {
        kprint("time task id error2: ");
        kprint_hex(cur_task_id);
        kprint("\n");
        if (cur_task_id == 0)
          next_task_id = 1;
        else
            next_task_id = 0;
    }
*/
/*
    tcb_t *cur = &tcb[cur_task_id];
    tcb_t *next = &tcb[next_task_id];
    cur->ticks++;
    //next->ticks = 0;
    if (cur->ticks > 10) {
        cur->ticks = 0;
        //kprintf("switchcontext, cur:%d, next:%d\n", cur_task_id, next_task_id);

        int tmpid = cur_task_id;
        cur_task_id = next_task_id;
        next_task_id = tmpid;

        next->ticks = 0;
        context_switch(cur, next);
    }
*/
    tcb_t *cur = &tcb[cur_task_id];
    cur->ticks++;

    if (cur->ticks > 10) {
        do_context_switch();
    }
    
    tick++;
    UNUSED(regs);
}

void init_timer(u32 freq) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    u32 divisor = 0xFFFF;//1193180 / freq;
    u8 low  = (u8)(divisor & 0xFF);
    u8 high = (u8)( (divisor >> 8) & 0xFF);
    /* Send the command */
    port_byte_out(0x43, 0x36); /* Command port */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}

