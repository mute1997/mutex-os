#include <std/syscall.h>

int sys_hlt() {
  __asm__ __volatile__ ("hlt");
}
