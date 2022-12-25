#include "type.h"
#include "disk.h"
#include "ports.h"
#include "task.h"
#include "mem.h"
#include "inter.h"
#include "screen.h"
#include "schedule.h"

typedef struct {
    int task_id;
    int tart_sector;
    int num;
    u8 *buf;
    int status;
    int response;
} hd_req_t;

hd_req_t *cur = 0;

void free_cur_req() {
    if (cur) {
        kfree(cur->buf);
    }
    cur = 0;
}

u32 get_hd_cur_words_to_read() {
    if (cur) {
       return cur->num * 256;
    }
    return 0;
}

u8 *get_hd_cur_buffer() {
    if (cur)
       return cur->buf;
    return 0;
}

void hd_read_done(u8 status) {
    if (cur) {
        cur->status = status;
        cur->response = 1;
        mov_task_ready(cur->task_id);
    }
}

void read_disk(int sec_id, u8 num) {
    // Read one sector.
    port_byte_out(HD_REG_CMD_1, num);
    // LBA low
    port_byte_out(HD_REG_CMD_2, (u8)(sec_id & 0xFF));
    // LBA mid
    port_byte_out(HD_REG_CMD_3, (u8)(sec_id>>8 & 0xFF));
    // LBA high
    port_byte_out(HD_REG_CMD_4, (u8)(sec_id>>16 & 0xFF));
    // device reg: LBA[24:28]
    port_byte_out(HD_REG_CMD_5, (u8)((sec_id>>24 & 0x0F) | 0xe0));
    // start to read.
    // command reg: 0x2 read, start reading
    port_byte_out(HD_REG_CMD_6, 0x20);
}

void hd_read_req_one_sector(int sec_id, u8 * buf) {
    u8 status = port_byte_in(HD_REG_CMD_6);
    while(status & 0x80) {
        kprintf("hd_read-req, disk is busy:%x\n", status);
        do_context_switch();
    }
    disable_interrupt();
    free_cur_req();
    hd_req_t *req = (hd_req_t *)kmalloc(sizeof(hd_req_t));
    req->buf = buf;
    req->task_id = cur_task_id;
    req->tart_sector = sec_id;
    req->num = 1;
    req->response = 0;
    
    cur = req;
    read_disk(sec_id, 1);
    enable_interrupt();
    while(cur->response == 0) {
        do_context_switch();
    }
}

int read_sector(disk_t *disk, int sec_id, int num, u8* buf) {
    for (int idx = 0; idx < num; idx++) {
        hd_read_req_one_sector(sec_id + idx, buf += (idx * 512));
    }
}
