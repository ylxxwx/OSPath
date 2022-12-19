#include "isr.h"
#include "type.h"
#include "screen.h"
#include "memory.h"
#include "page.h"
#include "panic.h"
#include "frame.h"

void page_fault(registers_t *regs) {
   u32 faulting_address;
   asm ("mov %%cr2, %0": "=r"(faulting_address));

   int present = regs->err_code & 0x1;
   int rw = regs->err_code & 0x2;
   int us = regs->err_code & 0x4;
   int reserved = regs->err_code & 0x8;
   int id = regs->err_code & 0x10;

   kprintf("Page fault!(%x | %s | %s | %s) @%x\n", 
      regs->err_code,
      present? "present": "not present",
      rw?"write":"read",
      us?"user mode":"kernel mode",
      faulting_address
      );

   if (!present) {
      page_t *page = get_page(faulting_address, 1, current_directory);
      if (page == 0) {
        panic("Get page failed");
        return;
      }
      alloc_frame(page, 1, 1);
      return;
   }
   panic("Page fault");
}