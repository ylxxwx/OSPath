#include "memory.h"
#include "nofreemem.h"
#include "type.h"
#include "isr.h"
#include "screen.h"
#include "mem.h"
#include "frame.h"
#include "kheap.h"
#include "panic.h"

page_directory_t *kernel_directory = 0;

u32 vaddr_to_paddr(u32 address)
{
  u32 pageDirIdx = ((address >> 22) & 0x3FF);
  page_table_t *pageTable = current_directory->tables[pageDirIdx];
  if (pageTable == 0)
  {
    kprintf(" 1.1 page table is nil\n");
  }
  u32 pageIdx = (address >> 12) & 0x3FF;
  page_t *page = &(pageTable->pages[pageIdx]);
  if (page == 0)
  {
    kprintf(" 1.2 page is nil\n");
  }
  u32 phyAddr = (u32)(page->frame * 0x1000) + (u32)(address & 0x00000FFF);
  return phyAddr;
}

void switch_page_directory(page_directory_t *dir)
{
  current_directory = dir;
  // kprintf("page dir:v:%x, p:%x\n", &dir->tablesPhysical, vaddr_to_paddr(&dir->tablesPhysical));
  // panic("stop");
  asm("mov %0, %%cr3" ::"r"(vaddr_to_paddr((u32)&dir->tablesPhysical)));
  u32 cr0;
  asm("mov %%cr0, %0"
      : "=r"(cr0));
  cr0 |= 0x80000000;
  asm("mov %0, %%cr0" ::"r"(cr0));
}

/*
   virtual address <--> page
*/
page_t *get_page(u32 address, int make, page_directory_t *dir)
{
  address /= 0x1000;
  u32 table_idx = address / 1024;
  if (dir->tables[table_idx])
  {
    return &dir->tables[table_idx]->pages[address % 1024];
  }
  else if (make)
  {
    u32 tmp = 0;
    dir->tables[table_idx] = (page_table_t *)kmalloc_ap_nofree(sizeof(page_table_t), 1, &tmp);
    if (dir->tables[table_idx] == 0 && kheap_ready())
    {
      dir->tables[table_idx] = (page_table_t *)kmalloc_a(sizeof(page_table_t), 1);
      tmp = vaddr_to_paddr((u32)dir->tables[table_idx]);
    }
    if (dir->tables[table_idx] == 0)
      return 0;
    kmemset((u8 *)dir->tables[table_idx], 0, sizeof(page_table_t));
    // kprintf("Page alloc mem, vaddr: %x, phyaddr: %x\n", dir->tables[table_idx], tmp);
    dir->tablesPhysical[table_idx] = tmp | 0x7; // 0x7;
    return &dir->tables[table_idx]->pages[address % 1024];
  }
  else
  {
    return 0;
  }
}

page_directory_t *clone_crt_page_dir(page_directory_t *src)
{
  // Map copied page tables (including the new page dir) to some virtual space so that
  // we can access them.
  //
  // First, map the new page dir.
  page_directory_t *copied_page_dir = (page_directory_t *)kmalloc_a(sizeof(page_directory_t), 1);
  if (copied_page_dir == 0)
    panic("clone crt failed. no memory.");

  u32 new_pd_frame = vaddr_to_paddr((u32)copied_page_dir);
  kmemset((u8 *)copied_page_dir, 0, sizeof(page_directory_t));

  // First page dir entry is shared - the first 4MB virtual space is reserved.
  copied_page_dir->tablesPhysical[0] = src->tablesPhysical[0];
  copied_page_dir->tables[0] = src->tables[0];

  // Share all 256 kernel pdes - except pde 769, which should points to new page dir itself.
  // 768 * 4M = 0xC0000000 3G.
  for (uint32 i = 768; i < 1024; i++)
  {
    copied_page_dir->tablesPhysical[i] = src->tablesPhysical[i];
    copied_page_dir->tables[i] = src->tables[i];
  }

  // Copy user space page tables.
  for (uint32 i = 1; i < 768; i++)
  {
    page_table_t *src_table = src->tables[i];
    if (src_table == 0)
    {
      continue;
    }
    // kprintf("clone page table. found a user table. %d\n", i);
    //  Alloc a new frame for copied page table.
    u32 vaddr = (u32)kmalloc_a(PAGE_SIZE, 1);
    if (vaddr == 0)
    {
      panic("allocate page table failed.");
    }
    // Copy page table and set ptes copy-on-write.
    copied_page_dir->tablesPhysical[i] = vaddr_to_paddr(vaddr) | 0x07;
    ;
    copied_page_dir->tables[i] = (page_table_t *)vaddr;

    // kprintf("clone page table. found a user table. idx:%d va:%x, pa:%x\n", i, vaddr, copied_page_dir->tablesPhysical[i]);

    page_table_t *dst_table = (page_table_t *)vaddr;
    kmemset((u8 *)vaddr, 0, PAGE_SIZE);
    //*dst_table = *src_table;
    for (int j = 0; j < 1024; j++)
    {
      page_t *src_page = &src_table->pages[j];
      page_t *dst_page = &dst_table->pages[j];
      if (!src_page->present)
      {
        continue;
      }
      // kprintf("clone page table. found a user page. %d. paddr:%x\n", j, src_page->frame);
      *dst_page = *src_page;
      // Mark copy-on-write: increase copy-on-write ref count.
      src_page->rw = 0;
      dst_page->rw = 0;
      dst_page->present = 1;
      dst_page->user = 1;
      // dst_page->ker
      set_frame(src_page->frame);
      // kprintf("page frame:%d count:%d\n", src_page->frame, get_frame_user_count(src_page->frame));
    }
  }
  return copied_page_dir;
}

void free_page_dir(page_directory_t *src)
{
  // Copy user space page tables.
  for (uint32 i = 1; i < 768; i++)
  {
    page_table_t *src_table = src->tables[i];
    if (src_table == 0)
    {
      continue;
    }
    // kprintf("clone page table. found a user table. %d\n", i);
    //  Alloc a new frame for copied page table.
    kfree((u8 *)src->tables[i]);
  }
  kfree(src);
}
