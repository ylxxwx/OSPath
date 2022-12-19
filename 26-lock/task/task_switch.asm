[global resume_thread]
[global context_switch]

context_switch:
  push eax
  push ecx
  push edx
  push ebx
  push ebp
  push esi
  push edi

  mov eax, [esp + 32]
  mov [eax], esp
  ; jump to next thread's kernel stack
  ; and recover its execution
  mov eax, [esp + 36]
  mov esp, [eax]

resume_thread:
    pop edi
    pop esi
    pop ebp
    pop ebx
    pop edx
    pop ecx
    pop eax
    sti
    ret
