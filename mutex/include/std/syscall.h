#include <asm/types.h>

#define SYSCALL_COUNT 600
#define define_syscall(i, func) syscalls[i] = func
#define syscall(i, arg1, arg2, arg3, ret) __asm__ __volatile__ ("int $0x80" \
        : "+b" (arg1), \
          "+c" (arg2), \
          "+d" (arg3), \
          "=a" (ret) \
        : "a" (i) \
        : "memory", "cc");

#define SYS_X      500
#define SYS_XP     501
#define SYS_READ   502
#define SYS_WRITE  503
#define SYS_HLT    504

int (*syscalls[SYSCALL_COUNT])(u32 ebx, u32 ecx, u32 edx);

// syscall
void init_syscalls();

// sysmemory
int sys_x(u32 addr);
int sys_xp(u32 addr);

// sysio
int sys_write(int fd, char *buf, u32 count);
int sys_read(int fd, char *buf, u32 count);

// syshlt
int sys_hlt();
