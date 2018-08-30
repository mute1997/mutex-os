#include <asm/types.h>

#define LDT 2
#define GDT_ENTRIES 7
#define IDT_SIZE 256

#define GRANULARITY_SHIFT 16
#define PAGE_GRAN_SHIFT 12
#define DEFAULT 0x40

#define SEG_SELECTOR(i) ((i)*8)
#define KERN_CS_SELECTOR SEG_SELECTOR(KERN_CS_INDEX)
#define KERN_DS_SELECTOR SEG_SELECTOR(KERN_DS_INDEX)

/* GDT Layout */
#define KERN_CS_INDEX 1
#define KERN_DS_INDEX 2
#define USER_CS_INDEX 3
#define USER_DS_INDEX 4
#define TSS_INDEX 5
#define LDT_INDEX 6

/* GDT access byte */
#define PRESENT 0x80
#define SEGMENT 0x10
#define EXECUTABLE 0x08
#define DIRECTION 0x04
#define READABLE 0x02
#define WRITABLE 0x02
#define ACCESS 0x01

/* GDT Granularity byre */
#define GRANULAR 0x80

#define INT_286_GATE 6
#define DESC_386_BIT 0x08
#define AVL_286_TSS 1
#define INT_GATE_TYPE (INT_286_GATE | DESC_386_BIT)
#define TSS_TYPE	(AVL_286_TSS  | DESC_386_BIT)

#define X86_STACK_TOP_RESERVED (2*sizeof(u32))

/* Segment descriptor */
struct desc_struct {
  u16 limit_low;
  u16 base_low;
  u8 base_middle;
  u8 access;
  u8 granularity;
  u8 base_high;
} __attribute__((packed));

/* Gate Descriptor  */
struct gatedesc {
  u16 offset_low;
  u16 selector;
  u8 pad;
  u8 p_dp1_type;
  u16 offset_high;
} __attribute__((packed));

/* Descriptor table pointer */
struct desctableptr {
  u16 limit;
  u32 base;
} __attribute__((packed));

struct gate_table {
  void(*gate) (struct interrupt_frame *frame);
  u8 vec_nr;
  u8 privilege;
};

struct tss_entry {
  u32 prev_tss;
  u32 esp0;
  u32 ss0;
  u32 esp1;
  u32 ss1;
  u32 esp2;
  u32 ss2;
  u32 cr3;
  u32 eip;
  u32 eflags;
  u32 eax;
  u32 ecx;
  u32 edx;
  u32 ebx;
  u32 esp;
  u32 ebp;
  u32 esi;
  u32 edi;
  u32 es;
  u32 cs;
  u32 ss;
  u32 ds;
  u32 fs;
  u32 gs;
  u32 ldt;
  u16 trap;
  u16 iomap_base;
} __attribute__((packed));
