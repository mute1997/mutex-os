global tss_flush
global jmp_ring3

extern shell_main

tss_flush:
  mov ax, 0x2B
  ltr ax
  ret

jmp_ring3:
  cli
  mov ax, 0x23
  mov dx, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  mov eax, esp
  push 0x23
  push eax
  pushf
  push 0x1B
  push shell_main
  iret
