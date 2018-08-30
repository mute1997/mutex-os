#include <asm/types.h>
#include <std/printk.h>
#include <std/string.h>
#include <std/hlt.h>
#include <trap.h>
#include <archtypes.h>
#include <mm.h>

#define load_idt(dtr) __asm__ __volatile__("lidt %0"::"m" (*dtr))
#define store_idt(dtr) __asm__ __volatile__("sidt %0":"=m" (*dtr))
#define store_gdt(dtr) __asm__ __volatile__("sgdt %0":"=m" (*dtr))

extern tss_flush();
extern void load_gdt(u32 desc_struct);
extern void syshandler();
extern char kernel_stack;

struct desctableptr gdt_desc, idt_desc;
struct desc_struct gdt[GDT_ENTRIES]; /* GDT */
struct gatedesc idt[IDT_SIZE]; /* IDT */
struct tss_entry tss; /* TSS */

static struct gate_table gate_table_pic[] = {
	{ hwint00, VECTOR( 0), INTR_PRIVILEGE },
	{ hwint01, VECTOR( 1), INTR_PRIVILEGE },
	{ hwint02, VECTOR( 2), INTR_PRIVILEGE },
	{ hwint03, VECTOR( 3), INTR_PRIVILEGE },
	{ hwint04, VECTOR( 4), INTR_PRIVILEGE },
	{ hwint05, VECTOR( 5), INTR_PRIVILEGE },
	{ hwint06, VECTOR( 6), INTR_PRIVILEGE },
	{ hwint07, VECTOR( 7), INTR_PRIVILEGE },
	{ hwint08, VECTOR( 8), INTR_PRIVILEGE },
	{ hwint09, VECTOR( 9), INTR_PRIVILEGE },
	{ hwint10, VECTOR(10), INTR_PRIVILEGE },
	{ hwint11, VECTOR(11), INTR_PRIVILEGE },
	{ hwint12, VECTOR(12), INTR_PRIVILEGE },
	{ hwint13, VECTOR(13), INTR_PRIVILEGE },
	{ hwint14, VECTOR(14), INTR_PRIVILEGE },
	{ hwint15, VECTOR(15), INTR_PRIVILEGE },
	{ NULL, 0, 0}
};

static struct gate_table gate_table_exceptions[] = {
	{ divide_error, DIVIDE_VECTOR, INTR_PRIVILEGE },
	{ single_step_exception, DEBUG_VECTOR, INTR_PRIVILEGE },
	{ nmi, NMI_VECTOR, INTR_PRIVILEGE },
	{ breakpoint_exception, BREAKPOINT_VECTOR, USER_PRIVILEGE },
	{ overflow, OVERFLOW_VECTOR, USER_PRIVILEGE },
	{ bounds_check, BOUNDS_VECTOR, INTR_PRIVILEGE },
	{ inval_opcode, INVAL_OP_VECTOR, INTR_PRIVILEGE },
	{ copr_not_available, COPROC_NOT_VECTOR, INTR_PRIVILEGE },
	{ double_fault, DOUBLE_FAULT_VECTOR, INTR_PRIVILEGE },
	{ copr_seg_overrun, COPROC_SEG_VECTOR, INTR_PRIVILEGE },
	{ inval_tss, INVAL_TSS_VECTOR, INTR_PRIVILEGE },
	{ segment_not_present, SEG_NOT_VECTOR, INTR_PRIVILEGE },
	{ stack_exception, STACK_FAULT_VECTOR, INTR_PRIVILEGE },
	{ general_protection, PROTECTION_VECTOR, INTR_PRIVILEGE },
	{ page_fault, PAGE_FAULT_VECTOR, INTR_PRIVILEGE },
	{ copr_error, COPROC_ERR_VECTOR, INTR_PRIVILEGE },
	{ alignment_check, ALIGNMENT_CHECK_VECTOR, INTR_PRIVILEGE },
	{ machine_check, MACHINE_CHECK_VECTOR, INTR_PRIVILEGE },
	{ simd_exception, SIMD_EXCEPTION_VECTOR, INTR_PRIVILEGE },
	{ NULL, 0, 0}
};

void int_gate(struct gatedesc *desc, unsigned vec_nr, u32 offset, unsigned dp1_type) {
  register struct gatedesc *idp;
  idp = &desc[vec_nr];
  idp->offset_low = offset;
  idp->selector = KERN_CS_SELECTOR;
  idp->p_dp1_type = dp1_type;
  idp->offset_high = offset >> 16;
}

void idt_copy_vectors(struct gate_table* first) {
  struct gate_table *gtp;
  for (gtp=first;gtp->gate;gtp++) {
    int_gate(idt, gtp->vec_nr, (u32)gtp->gate, PRESENT | INT_GATE_TYPE | (gtp->privilege << 5));
  }
}

void syscall_vector() {
  int_gate(idt, SYSCALL_VECTOR, (u32)syshandler, PRESENT | INT_GATE_TYPE | (USER_PRIVILEGE << 5));
}

void idt_copy_vectors_pic() {
	idt_copy_vectors(gate_table_pic);
}

void idt_init() {
  /* Dummy interrupt handler */
  for (int i=0;i<256;i++) {
    int_gate(idt, i, (u32)interrupt_stub, 0x8E);
  }

  syscall_vector();
  idt_copy_vectors_pic();
  idt_copy_vectors(gate_table_exceptions);
}


/* Init Segment descriptor */
void init_segment(struct desc_struct *desc, u32 base, u32 size) {
  desc->base_low = base;
  desc->base_middle = base >> 16;
  desc->base_high = base >> 24;
  desc->limit_low = size >> 12;
  desc->granularity =  GRANULAR | (size >> (PAGE_GRAN_SHIFT + GRANULARITY_SHIFT)) | DEFAULT;
}

void init_data_segment(struct desc_struct *desc, u32 base, u32 size, int privilege) {
  init_segment(desc, base, size);
  desc->access = (privilege << 5) | (PRESENT | SEGMENT | WRITABLE);
}

void init_code_segment(struct desc_struct *desc, u32 base, u32 size, int privilege) {
  init_segment(desc, base, size);
  desc->access = (privilege << 5) | (PRESENT | SEGMENT | EXECUTABLE | READABLE);
}

void init_tss() {
  struct tss_entry *t = &tss;
  int index = TSS_INDEX;
  struct desc_struct *tssgdt;

  /* set GDT */
  tssgdt = &gdt[index];
  init_data_segment(tssgdt, (u32)t, sizeof(struct tss_entry), INTR_PRIVILEGE);
  tssgdt->access = PRESENT | (INTR_PRIVILEGE << 5) | TSS_TYPE;

  memset(t, 0, sizeof(struct tss_entry)); /* initialize tss */

  /* set TSS */
	t->esp0 = ((unsigned)&kernel_stack) - X86_STACK_TOP_RESERVED;
	t->ss0 = 0x10;
	t->cs = 0x0b;
  t->ss = t->ds = t->es = t->fs = t->gs = 0x13;
  t->iomap_base = sizeof(struct tss_entry);
	t->cr3 = read_cr3();

  /* DEBUG */
  /* TODO ユーザーランドのスタックはどこになっているのか確認する */
}

void switch_to_new_idt() {
  idt_init();
  load_idt(&idt_desc);
}

void switch_to_new_gdt() {
  load_gdt((u32)&gdt_desc);
}

void switch_to_new_tss() {
  init_tss();
  tss_flush();
}

void prot_load_selectors() {
  /* set new GDT */
  switch_to_new_gdt();
  printk("Setup gdt...  [OK]\n");

  /* set new IDT */
  switch_to_new_idt();
  printk("Setup idt...  [OK]\n");

  switch_to_new_tss();
  printk("Setup TSS...  [OK]\n");
}

void prot_init() {
  /* Init GDT */
  memset(gdt, 0, sizeof(gdt));
  memset(idt, 0, sizeof(idt));

  /* Init *_desc*/
  memset(&gdt_desc, 0, sizeof(gdt_desc));
  memset(&idt_desc, 0, sizeof(idt_desc));
  memset(&tss, 0, sizeof(tss));

  /* set GDT pointer */
  gdt_desc.base = (u32)gdt;
  gdt_desc.limit = sizeof(gdt)-1;
  idt_desc.base = (u32)idt;
  idt_desc.limit = sizeof(idt)-1;

  /* Init GDT segments */
  init_code_segment(&gdt[KERN_CS_INDEX], 0, 0xFFFFFFFF, KERNEL_PRIVILEGE); /* kernel code */
  init_data_segment(&gdt[KERN_DS_INDEX], 0, 0xFFFFFFFF, KERNEL_PRIVILEGE); /* kernel data */
  init_code_segment(&gdt[USER_CS_INDEX], 0, 0xFFFFFFFF, USER_PRIVILEGE); /* user code */
  init_data_segment(&gdt[USER_DS_INDEX], 0, 0xFFFFFFFF, USER_PRIVILEGE);  /* user data */
  init_segment(&gdt[LDT_INDEX], 0x0, 0); /* unusable LDT */
  gdt[LDT_INDEX].access = PRESENT | LDT;

  prot_load_selectors();
}
