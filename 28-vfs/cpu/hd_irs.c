#include "timer.h"
#include "isr.h"
#include "ports.h"
#include "function.h"
#include "ports.h"
#include "task.h"
//#include "harddisk.h"
#include "disk.h"
#include "screen.h"

u8 *get_hd_cur_buffer();
void hd_read_done(u8 status);


static void hd_callback(registers_t *regs) {    
    u8 status = port_byte_in(HD_REG_CMD_6);
    if (status & 0x8) {
        u16 *buf = (u16 *)get_hd_cur_buffer();
        
        u32 words = get_hd_cur_words_to_read();
        if (buf == 0 || words == 0)
            return;
        u16 data = 0;
        int idx = 0;
        for (int idx = 0; idx < words; idx++) {
            data = port_word_in(HD_REG_DATA);
            buf[idx] = data;
        }
        hd_read_done(status);
    }
    status = port_byte_in(HD_REG_CMD_6);
    hd_read_done(status);
}

void init_hds() {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ14, hd_callback);
    register_interrupt_handler(IRQ15, hd_callback);
}

