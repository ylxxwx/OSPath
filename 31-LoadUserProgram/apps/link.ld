ENTRY(start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
  .text 0xA0000000:
  {
    code = .; _code = .; __code = .;
    *(.text)
    . = ALIGN(64);
  }

  .data :
  {
     data = .; _data = .; __data = .;
     *(.data)
     *(.rodata)
     . = ALIGN(64);
  }

  .bss :
  {
    bss = .; _bss = .; __bss = .;
    *(.bss)
    . = ALIGN(64);
  }

  end = .; _end = .; __end = .;
}