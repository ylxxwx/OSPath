#include "isr.h"
#include "type.h"
#include "screen.h"
#include "mem.h"
#include "memory.h"
#include "page.h"
#include "panic.h"
#include "frame.h"

extern int cur_task_id;

void page_fault(registers_t *regs)
{
   u32 faulting_address;

   asm("mov %%cr2, %0"
       : "=r"(faulting_address));

   int present = regs->err_code & 0x1;
   int rw = regs->err_code & 0x2;
   int us = regs->err_code & 0x4;
   int reserved = regs->err_code & 0x8;
   int id = regs->err_code & 0x10;
   /*
   kprintf("Page fault!(%x | %s | %s | %s | %d) @%x \n",
      regs->err_code,
      present? "present": "not present",
      rw?"write":"read",
      us?"user mode":"kernel mode",
      id, faulting_address
      );
   */
   page_t *page = get_page(faulting_address, 1, current_directory);
   if (page == 0)
   {
      panic("Get page failed");
      return;
   }

   if (!present)
   {
      // 0, 1, user, writable, this is temp use.
      //
      alloc_frame(page, 0, 1);
      return;
   }
   else
   {
      if (rw != 0 && page->rw == 0)
      {
         u32 frame_id = page->frame;
         // kprintf("page:%d, copy on write, count:%d\n", page->frame, get_frame_user_count(frame_id));
         if (get_frame_user_count(frame_id) == 1)
         {
            // panic("not sure if we can get here.");
            page->user = 1;
            page->rw = 1;
            // switch_page_directory(current_directory);
            return;
         }

         u8 *vAddr = (u8 *)faulting_address;
         u8 *temp = (u8 *)0; // kmalloc_a(PAGE_SIZE, 1);
         kmemcpy(temp, (u8 *)(((u32)vAddr) & 0xFFFFF000), PAGE_SIZE);
         page->frame = 0;
         alloc_frame(page, 0, 1);
         switch_page_directory(current_directory);
         kmemcpy((u8 *)(((u32)vAddr) & 0xFFFFF000), temp, PAGE_SIZE);
         page->user = 1;
         page->rw = 1;
         page->present = 1;
         switch_page_directory(current_directory);
         clear_frame(frame_id);
         return;
      }
      else
      {
         kprintf("Page fault!(%x | %s | %s | %s | %d) @%x \n",
                 regs->err_code,
                 present ? "present" : "not present",
                 rw ? "write" : "read",
                 us ? "user mode" : "kernel mode",
                 id, faulting_address);
      }
   }

   panic("Page fault");
}
