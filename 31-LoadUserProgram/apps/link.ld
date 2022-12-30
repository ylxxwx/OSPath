ENTRY(start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
  .text 0xA0000000:
  {
    code = .; _code = .; __code = .;
    *(.text)
    . = ALIGN(4096);
  }

  .data :
  {
     data = .; _data = .; __data = .;
     *(.data)
     *(.rodata)
     . = ALIGN(1024);
  }

  .bss :
  {
    bss = .; _bss = .; __bss = .;
    *(.bss)
    . = ALIGN(1024);
  }

  end = .; _end = .; __end = .;
}