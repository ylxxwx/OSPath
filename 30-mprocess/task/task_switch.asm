[global resume_thread]
[global context_switch]
[global switch_to_user_mode]
[global syscall_fork_exit]
[extern interrupt_exit]
[extern kprint_hex]

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

switch_to_user_mode:
  add esp, 0x08
  jmp interrupt_exit

syscall_fork_exit:
  ; move esp to interrupt stack
  add esp, 8

  push esp
  call kprint_hex
  pop esp

  ; recover the original data segment.
  pop ecx
  mov ds, cx
  mov es, cx
  mov fs, cx
  mov gs, cx

  pop edi
  pop esi
  pop ebp
  add esp, 4  ; do not pop old esp, otherwise child process will not return normally from fork()
  pop ebx
  pop edx
  pop ecx

  mov eax, 0  ; child process returns 0
  add esp, 4

  ; pop dummy values
  add esp, 8
  
  sti
  ; pop eip, cs, eflags, user_esp and user_ss by processor
  iret


  ;popa
  ; clean up the pushed error code and pushed ISR number
  ;add esp, 8

  ; make sure interrupt is enabled
  ;sti
  ; pop cs, eip, eflags, user_ss, and user_esp by processor
  ;iret
