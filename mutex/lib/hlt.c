#include <std/syscall.h>
#include <lib/hlt.h>

int hlt() {
  syscall(SYS_HLT, NULL, NULL, NULL, NULL);
}
