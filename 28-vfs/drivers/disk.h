#ifndef _HD_H_
#define _HD_H_

#define HD_REG_DATA 0x1f0

#define HD_REG_CMD_1 0x01f2
#define HD_REG_CMD_2 0x01f3
#define HD_REG_CMD_3 0x01f4
#define HD_REG_CMD_4 0x01f5
#define HD_REG_CMD_5 0x01f6
#define HD_REG_CMD_6 0x01f7


typedef struct {
  u8 major;
  u8 minor;  
  u8 partition;
} disk_t;

void show_disk(char *prefix, disk_t *disk);
int read_sector(disk_t *disk, int sector_id, int num, u8 *buf);

u32 get_hd_cur_words_to_read();
u8 *get_hd_cur_buffer();
void hd_read_done(u8 status);

#endif //_HD_H_
