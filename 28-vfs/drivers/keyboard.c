#include "keyboard.h"
#include "ports.h"
#include "isr.h"
#include "screen.h"
#include "string.h"
#include "function.h"
#include "kernel.h"
#include "task.h"
#include "schedule.h"
#include "mem.h"
#include "inter.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C

static char key_buffer[256];

typedef struct kb_req {
  int task_id;
  int done;
  u8* buf;
} kb_req_t;

kb_req_t kb_cur;

s32 kb_read(u8 *buf) {
    disable_interrupt();
    kb_cur.done = 0;
    kb_cur.buf = buf;
    kb_cur.task_id = cur_task_id;
    mov_task_wait(cur_task_id);
    while(kb_cur.done == 0) {
      do_context_switch();
    }
    //kmemcpy(buf, key_buffer, 256);
    enable_interrupt();
    return 256;
}

void kb_task_done() {
    kmemcpy(kb_cur.buf, key_buffer, 256);
    kmemset(key_buffer, 8, 256);
    kb_cur.done = 1;
    mov_task_ready(kb_cur.task_id);
}


#define SC_MAX 57
const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};

static void keyboard_callback(registers_t *regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    u8 scancode = port_byte_in(0x60);
    
    if (scancode > SC_MAX) return;
    if (scancode == BACKSPACE) {
        if (backspace(key_buffer) >= 0)
            kprint_backspace();
    } else if (scancode == ENTER) {
        kprintf("\n");
        kb_task_done();
        //user_input(key_buffer); /* kernel-controlled function */
        key_buffer[0] = '\0';
    } else {
        char letter = sc_ascii[(int)scancode];
        /* Remember that kprint only accepts char[] */
        char str[2] = {letter, '\0'};
        append(key_buffer, letter);
        kprintf(str);
    }
    UNUSED(regs);
}

void init_keyboard() {
   register_interrupt_handler(IRQ1, keyboard_callback); 
}
