#include <lib/printf.h>
#include <std/hlt.h>

int hello_main(int argc, char *arg[]) {
  const char *str = "Hello world";
  printf("%s\n ", str);
  return 0;
}
