#include <lib/scanf.h>
#include <std/syscall.h>
#include <std/stdio.h>
#include <lib/hlt.h>

extern int is_pressed_enter;

int scanf(char *p) {
  int ret, count = sizeof(char)*256, stdin = STDIN;

  /* TODO fix busy wait */
  /* TODO use syscall return value */
  while (!is_pressed_enter) hlt();

  syscall(SYS_READ, stdin, p, count, ret);

  while (is_pressed_enter) hlt();
}
