#include <lib/printf.h>
#include <std/stdio.h>
#include <std/stdarg.h>

extern int vsnprintf(char *buf, unsigned long size, const char *fmt, va_list args);

int printf(const char *fmt, ...) {
  int ret, buf_len;
  char buf[256], *bufp;
  va_list args;

  bufp = &buf;
  va_start(args, fmt);
  memset(buf, NULL, sizeof(buf));
  vsnprintf(buf, sizeof(buf), fmt, args);

  buf_len = strlen(buf);
  syscall(SYS_WRITE, STDOUT, bufp, buf_len, ret);
}
