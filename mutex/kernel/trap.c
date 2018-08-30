#include <std/printk.h>
#include <std/hlt.h>
#include <std/syscall.h>
#include <drivers/keyboard.h>
#include <asm/io.h>
#include <asm/types.h>
#include <trap.h>
#include <i8325.h>
#include <mm.h>

__attribute__((interrupt))
void interrupt_stub(struct interrupt_frame *frame){
  printk("interrupt_stub");
}

__attribute__((interrupt))
void hwint00(struct interrupt_frame *frame){
  printk("hwint00");
  irq_8259_eoi(VECTOR(0));
}
__attribute__((interrupt))
void hwint01(struct interrupt_frame *frame){
  unsigned char scancode = inb(0x60);
  char letter = get_char_from_scancode(scancode);
  if (letter == NULL) {
    irq_8259_eoi(VECTOR(1));
    return;
  }
  printk("%c", letter);
  irq_8259_eoi(VECTOR(1));
}
__attribute__((interrupt))
void hwint02(struct interrupt_frame *frame){
  printk("hwint02");
  irq_8259_eoi(VECTOR(2));
}
__attribute__((interrupt))
void hwint03(struct interrupt_frame *frame){
  printk("hwint03");
  irq_8259_eoi(VECTOR(3));
}
__attribute__((interrupt))
void hwint04(struct interrupt_frame *frame){
  printk("hwint04");
  irq_8259_eoi(VECTOR(4));
}
__attribute__((interrupt))
void hwint05(struct interrupt_frame *frame){
  printk("hwint05");
  irq_8259_eoi(VECTOR(5));
}
__attribute__((interrupt))
void hwint06(struct interrupt_frame *frame){
  printk("hwint06");
  irq_8259_eoi(VECTOR(6));
}
__attribute__((interrupt))
void hwint07(struct interrupt_frame *frame){
  printk("hwint07");
  irq_8259_eoi(VECTOR(7));
}
__attribute__((interrupt))
void hwint08(struct interrupt_frame *frame){
  printk("hwint08");
  irq_8259_eoi(VECTOR(8));
}
__attribute__((interrupt))
void hwint09(struct interrupt_frame *frame){
  printk("hwint09");
  irq_8259_eoi(VECTOR(9));
}
__attribute__((interrupt))
void hwint10(struct interrupt_frame *frame){
  printk("hwint10");
  irq_8259_eoi(VECTOR(10));
}
__attribute__((interrupt))
void hwint11(struct interrupt_frame *frame){
  printk("hwint11");
  irq_8259_eoi(VECTOR(11));
}
__attribute__((interrupt))
void hwint12(struct interrupt_frame *frame){
  printk("hwint12");
  irq_8259_eoi(VECTOR(12));
}
__attribute__((interrupt))
void hwint13(struct interrupt_frame *frame){
  printk("hwint13");
  irq_8259_eoi(VECTOR(13));
}
__attribute__((interrupt))
void hwint14(struct interrupt_frame *frame){
  printk("hwint14");
  irq_8259_eoi(VECTOR(14));
}
__attribute__((interrupt))
void hwint15(struct interrupt_frame *frame){
  printk("hwint15");
  irq_8259_eoi(VECTOR(15));
}

__attribute__((interrupt))
void divide_error (struct interrupt_frame *frame) {
  printk("divide error");
}
__attribute__((interrupt))
void single_step_exception (struct interrupt_frame *frame){
  printk("single step exception");
}
__attribute__((interrupt))
void nmi (struct interrupt_frame *frame){
  printk("nmi");
}
__attribute__((interrupt))
void breakpoint_exception (struct interrupt_frame *frame){
  printk("breakpoint exception\n");
}
__attribute__((interrupt))
void overflow (struct interrupt_frame *frame){
  printk("overflow");
}
__attribute__((interrupt))
void bounds_check (struct interrupt_frame *frame){
  printk("bounds check");
}
__attribute__((interrupt))
void inval_opcode (struct interrupt_frame *frame){
  printk("inval opcode");
}
__attribute__((interrupt))
void copr_not_available (struct interrupt_frame *frame){
  printk("copr not available");
}
__attribute__((interrupt))
void double_fault(struct interrupt_frame *frame){
  printk("double fault");
  hlt();
}
__attribute__((interrupt))
void copr_seg_overrun(struct interrupt_frame *frame){
  printk("copr seg overrun");
}
__attribute__((interrupt))
void inval_tss(struct interrupt_frame *frame){
  printk("inval tss");
}
__attribute__((interrupt))
void segment_not_present(struct interrupt_frame *frame){
  printk("segment not present");
}
__attribute__((interrupt))
void stack_exception(struct interrupt_frame *frame){
  printk("stack exception");
}
__attribute__((interrupt))
void general_protection(struct interrupt_frame *frame){
  printk("general protection\n");
  hlt();
}
__attribute__((interrupt))
void page_fault(struct interrupt_frame *frame){
  u32 fault_address = read_cr2();

  char *errors[] = {
    "", /* not error */
    "Non present page access.", /* 0x1 present */
    "Write access error.", /* 0x10 W/R access */
    "Supervisor error.", /* 0x100 usermode, supervisor error */
    "PDE reserved error.", /* 0x1000 reserved */
    "Instruction fetch error.", /* 0x10000 instruction fetch */
  };

  printk("\nPageFault {\n");
  printk("     address: 0x%x\n", fault_address);
  printk("        flag: 0x%x\n", frame->flags);
  printk("   flag info:\n");
  for (int i=0;i<6;i++) {
    int flags = frame->flags;
    if ((flags >> i) & 1) {
      printk("     %s\n", errors[i+1]);
    }
  }
  
  printk("}\n\n");
  hlt();
}
__attribute__((interrupt))
void copr_error(struct interrupt_frame *frame){
  printk("copr error");
}
__attribute__((interrupt))
void alignment_check(struct interrupt_frame *frame){
  printk("alignment check");
}
__attribute__((interrupt))
void machine_check(struct interrupt_frame *frame){
  printk("machine check");
}
__attribute__((interrupt))
void simd_exception(struct interrupt_frame *frame){
  printk("simd exception");
}

/* software interrupt handlers */
int syscall_handler(){
  int eax, ebx, ecx, edx;
  read_registers(eax, ebx, ecx, edx);

  if (syscalls[eax] == NULL) {
    printk("Not found syscall");
    return 1;
  }

  return syscalls[eax](ebx, ecx, edx);
}
