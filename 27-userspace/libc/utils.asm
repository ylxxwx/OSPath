[GLOBAL get_ebp]
[GLOBAL atomic_exchange]
[GLOBAL refresh_tss]
[GLOBAL load_gdt]
get_ebp:
  mov eax, ebp
  ret

atomic_exchange:
  mov ecx, [esp + 4]
  mov eax, [esp + 8]
  xchg [ecx], eax
  ret

refresh_tss:
  mov ax, 0x30
  ltr ax
  ret

load_gdt:
  mov eax, [esp + 4]
  lgdt [eax]

  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov ss, ax
  
  mov ax, 0x18
  mov gs, ax

  jmp 0x08:.flush
.flush:
   ret
