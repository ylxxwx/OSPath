[GLOBAL get_ebp]
[GLOBAL atomic_exchange]
get_ebp:
  mov eax, ebp
  ret

atomic_exchange:
  mov ecx, [esp + 4]
  mov eax, [esp + 8]
  xchg [ecx], eax
  ret