global start
global page_directory
global kernel_stack

section .multiboot
header_start:
  dd 0xe85250d6                ; magic number (multiboot 2)
  dd 0                         ; architecture 0 (protected mode i386)
  dd header_end - header_start ; header length
  dd -(0xe85250d6 + 0 + (header_end - header_start)) ; checksum

  ; required end tag
  dw 0    ; type
  dw 0    ; flags
  dd 8    ; size
header_end:

KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)

section .data
align 0x1000
page_directory:
  dd 0x00000087
  times (KERNEL_PAGE_NUMBER - 1) dd 0
  dd 0x00000087
  times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0

section .text
  ; kernel entry function
  extern setup

  ; stack
  STACK_SIZE equ 4096*4

start:
  mov ecx, (page_directory - KERNEL_VIRTUAL_BASE)
  mov cr3, ecx

  mov ecx, cr4
  or ecx, 0x00000010
  mov cr4, ecx

  mov ecx, cr0
  or ecx, 0x80000000
  mov cr0, ecx

  lea ecx, [start_higher_half]
  jmp ecx

start_higher_half:
  mov dword [page_directory], 0
  invlpg [0]

  mov esp, kernel_stack+STACK_SIZE

  push ebx
  push eax
  call setup

  hlt

section .bss
kernel_stack:
  resb STACK_SIZE
