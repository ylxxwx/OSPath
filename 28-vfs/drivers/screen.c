#include "screen.h"
#include "ports.h"
#include "mem.h"

/* Declaration of private functions */
int get_cursor_offset();
void set_cursor_offset(int offset);
int print_char(char c, int col, int row, char attr);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

/**********************************************************
 * Public Kernel API functions                            *
 **********************************************************/

/**
 * Print a message on the specified location
 * If col, row, are negative, we will use the current offset
 */
void kprint_at(char *message, int col, int row) {
    /* Set cursor if col/row are negative */
    int offset;
    if (col >= 0 && row >= 0)
        offset = get_offset(col, row);
    else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    /* Loop through message and print it */
    int i = 0;
    while (message[i] != 0) {
        offset = print_char(message[i++], col, row, WHITE_ON_BLACK);
        /* Compute row/col for next iteration */
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
}

static void kprint(char *message) {
    kprint_at(message, -1, -1);
}

void kprint_backspace() {
    int offset = get_cursor_offset()-2;
    int row = get_offset_row(offset);
    int col = get_offset_col(offset);
    print_char(0x08, col, row, WHITE_ON_BLACK);
}

void kprint_hex(unsigned int value) {
    for (int idx = 7; idx >= 0; idx--) {
        int tmp = value;
        for (int st = 0; st < idx; st++) {
            tmp = (tmp >> 4);
        }
        tmp = tmp & 0xF;
        char ch;
        if (tmp >= 0 && tmp <= 9) {
            ch = (char)tmp + '0';
        } else {
            ch = (char)(tmp - 10) + 'A';
        }
        
        print_char(ch, -1, -1, WHITE_ON_BLACK);
    }
}

void kprint_dec(int32 n) {
  if (n == 0) {
    print_char('0', -1, -1, WHITE_ON_BLACK);
    return;
  }
  if (n < 0) {
    print_char('-', -1, -1, WHITE_ON_BLACK);
    n = -n;
  }

  uint32 acc = n;
  char c[32];
  int i = 0;
  while (acc > 0) {
    c[i] = '0' + acc % 10;
    acc /= 10;
    i++;
  }
  c[i] = 0;

  char c2[32];
  c2[i--] = 0;
  int j = 0;
  while(i >= 0) {
    c2[i--] = c[j++];
  }
  kprint(c2);
}

/**********************************************************
 * Private kernel functions                               *
 **********************************************************/
int scroll_screen(int cur_offset) {
    /* Check if the offset is over screen size and scroll */

    if (cur_offset >= MAX_ROWS * MAX_COLS * 2) {
        int i;
        for (i = 1; i < MAX_ROWS; i++) 
            kmemcpy((u8*)(get_offset(0, i-1) + VIDEO_ADDRESS),
                    (u8*)(get_offset(0, i) + VIDEO_ADDRESS),
                    MAX_COLS * 2);

        /* Blank last line */
        char *last_line = (char*) (get_offset(0, MAX_ROWS-1) + (u8*) VIDEO_ADDRESS);
        for (i = 0; i < MAX_COLS * 2; i += 2) {
          //last_line[i] = 0;
          last_line[i] = ' ';
          last_line[i+1] = WHITE_ON_BLACK;
        }

        cur_offset -= 2 * MAX_COLS;
    }
    return cur_offset;
}

/**
 * Innermost print function for our kernel, directly accesses the video memory 
 *
 * If 'col' and 'row' are negative, we will print at current cursor location
 * If 'attr' is zero it will use 'white on black' as default
 * Returns the offset of the next character
 * Sets the video cursor to the returned offset
 */
int print_char(char c, int col, int row, char attr) {
    u8 *vidmem = (u8*) VIDEO_ADDRESS;
    if (!attr) attr = WHITE_ON_BLACK;

    /* Error control: print a red 'E' if the coords aren't right */
    if (col >= MAX_COLS || row >= MAX_ROWS) {
        vidmem[2*(MAX_COLS)*(MAX_ROWS)-2] = 'E';
        vidmem[2*(MAX_COLS)*(MAX_ROWS)-1] = RED_ON_WHITE;
        return get_offset(col, row);
    }

    int offset;
    if (col >= 0 && row >= 0) offset = get_offset(col, row);
    else offset = get_cursor_offset();

    if (c == '\n') {
        row = get_offset_row(offset);
        offset = get_offset(0, row+1);
    } else if (c == 0x08) { /* Backspace */
        vidmem[offset] = ' ';
        vidmem[offset+1] = attr;
    } else {
        vidmem[offset] = c;
        vidmem[offset+1] = attr;
        offset += 2;
    }

    /* Check if the offset is over screen size and scroll */
    offset = scroll_screen(offset);
    set_cursor_offset(offset);
    return offset;
}

int get_cursor_offset() {
    /* Use the VGA ports to get the current cursor position
     * 1. Ask for high byte of the cursor offset (data 14)
     * 2. Ask for low byte (data 15)
     */
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8; /* High byte: << 8 */
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2; /* Position * size of character cell */
}

void set_cursor_offset(int offset) {
    /* Similar to get_cursor_offset, but instead of reading we write data */
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (u8)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (u8)(offset & 0xff));
}

void clear_screen() {
    int screen_size = MAX_COLS * MAX_ROWS;
    int i;
    u8 *screen = (u8*) VIDEO_ADDRESS;

    for (i = 0; i < screen_size; i++) {
        screen[i*2] = ' ';
        screen[i*2+1] = WHITE_ON_BLACK;
    }
    set_cursor_offset(get_offset(0, 0));
}

int get_offset(int col, int row) { return 2 * (row * MAX_COLS + col); }
int get_offset_row(int offset) { return offset / (2 * MAX_COLS); }
int get_offset_col(int offset) { return (offset - (get_offset_row(offset)*2*MAX_COLS))/2; }

extern void *get_ebp();

void kprintf_args(char* str, void* arg_ptr) {
  int i = 0;
  while (1) {
    char c = str[i];
    if (c == '\0') {
      break;
    }

    if (c == '%') {
      i++;
      char next = str[i];
      if (c == '\0') {
        break;
      }

      if (next == 'd') {
        int32 int_arg = *((int32*)arg_ptr);
        kprint_dec(int_arg);
        arg_ptr += 4;
      } else if (next == 'u') {
        uint32 int_arg = *((uint32*)arg_ptr);
        kprint_dec(int_arg);
        arg_ptr += 4;
      } else if (next == 'x') {
        uint32 int_arg = *((uint32*)arg_ptr);
        kprint_hex(int_arg);
        arg_ptr += 4;
      } else if (next == 'c') {
        char char_arg = *((char*)arg_ptr);
        print_char(char_arg, -1, -1, WHITE_ON_BLACK);
        arg_ptr += 4;
      } else if (next == 's') {
        char* str_arg = *((char**)arg_ptr);
        kprint(str_arg);
        arg_ptr += 4;
      }
    } else {
      print_char(c, -1, -1, WHITE_ON_BLACK);
    }

    i++;
  }
}

void kprintf(char* str, ...) {
  void* ebp = get_ebp();
  void* arg_ptr = ebp + 12;
  kprintf_args(str, arg_ptr);
}

void kprintln() {
  kprintf("\n");
}
