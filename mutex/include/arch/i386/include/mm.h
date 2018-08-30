#include <asm/types.h>

extern u32 _kernel_end;
extern u32 _text_end;

#define KERNEL_VIRTUAL_BASE 0xC0000000
#define KERNEL_VIRTUAL_START (u32)&_text_end
#define KERNEL_VIRTUAL_END (u32)&_kernel_end

#define MEMORY_HIGH_LIMIT 0x00EFFFFF

#define PAGE_SIZE (1024*4) /* 4KB */
#define PAGE_SHIFT 12
#define PGDIR_SHIFT 22
#define PTRS_PER_PGD 1024
#define PTRS_PER_PTE 1024
#define PHYS_PAGETABLE_SIZE 1048456 /* 4GB / PAGE_SIZE */
#define MMAP_TABLE_SIZE 256 /* mmap table size */

/* PTE, PDE constants */
#define VM_PRESENT        0x001
#define VM_RW             0x002
#define VM_USER           0x004 /* user | supervisor */
#define VM_PWT            0x008 /* page level write through */
#define VM_PCD            0x010 /* page level cache disable */
#define VM_ACC            0x020 /* page table accessed */
#define VM_DIRTY          0x040 /* page table accessed */
#define VM_PS             0x080 /* page table accessed */
#define VM_GLOBAL         0x100 /* page table accessed */
#define VM_UNUSED1        0x200 /* available for use */
#define VM_UNUSED2        0x400 /* available for use */
#define VM_UNUSED3        0x800 /* available for use */

#define VM_ADDR_MASK      0xFFFFF000

#define _VM_INIT_USER (VM_RW | VM_PRESENT | VM_USER)
#define _VM_INIT_KERN (VM_RW | VM_PRESENT)

#define pde_index(addr) (((addr) >> PGDIR_SHIFT))
#define pte_index(addr) (((addr) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))

#define write_cr3(x) __asm__ __volatile__ ("movl %0,%%cr3": :"r" (x))
#define write_cr0(x) __asm__ __volatile__ ("movl %0,%%cr0": :"r" (x))
#define read_cr2() ({\
    u32 __ret;\
  __asm__ __volatile__ ("mov %%cr2, %0" \
      : "=r" (__ret)\
      :);\
  __ret;})
#define read_cr3() ({\
      u32 __ret; \
      __asm__ __volatile__ ( \
        "movl %%cr3,%0\n\t" \
        :"=r" (__ret)); \
        __ret; \
        })

typedef u32 pt_entry;
typedef u32 pd_entry;

struct page {
  int count; /* TODO atomicな操作にする */
  u32 *phys_mem;
  u32 *virt_mem;
};

/* init */
void init_kern_pages();
void setup_physical_memory(u32 addr);

/* page_alloc */
int phys_to_page(u32 *addr);
u32 *alloc_page();
void free_page(u32 *addr);
u32 *kmalloc(u32 size);
void free(u32 addr);

/* mmu */
void setup_page_table();
void map_one_page(u32 *phys_addr, u32 *virt_addr);
u32 virt_to_phys(u32 *virt);
u32 *map_page(u32 *phys_addr, u32 *virt_addr, u32 size);
void unmap_page(u32 *virt_addr, u32 size);
