#include <mm.h>
#include <std/printk.h>
#include <std/hlt.h>

extern pd_entry page_directory[1024];
pt_entry page_table[1024][1024] __attribute__((aligned(4096)));

void setup_page_table() {
  memset(page_table, 0, sizeof(page_table));
}

u32 virt_to_phys(u32 *virt) {
  u32 pd_index = pde_index((u32)virt);
  u32 pt_index = pte_index((u32)virt);

  if ((page_directory[pd_index] & VM_PS) != 0) {
    /* 4MB paging */
    return (page_directory[pd_index] & 0xFFC00000) | ((u32)virt & 0x3FFFFF);
  } else {
    /* 4KB paging */
    return (page_table[pd_index][pt_index] & 0xFFFFF000) | ((u32)virt & 0x00000FFF);
  }
  return 0;
}

/* map 4KB page */
void map_one_page(u32 *phys_addr, u32 *virt_addr) {
  u32 pd_index = pde_index((u32)virt_addr);
  u32 pt_index = pte_index((u32)virt_addr);

  /* setting up page table entry */
  page_table[pd_index][pt_index] = ((u32)phys_addr & 0xFFFFF000) |  VM_PRESENT | VM_RW | VM_USER;

  /* setting up page directory entry */
  u32 pt_phys = virt_to_phys(&page_table[pd_index][pt_index]);
  page_directory[pd_index] = ((pt_phys) & 0xFFFFF000) | VM_PRESENT | VM_RW | VM_USER;
}

/* map each 4KB page */
u32 *map_page(u32 *phys_addr, u32 *virt_addr, u32 size) {
  u32 base = (u32)phys_addr & 0xFFFFF000;
  u32 pa=base;
  u32 va=(u32)virt_addr;
  while (pa < base+(0x1000*size)) {
    map_one_page((u32*)pa, (u32*)va);
    pa+=0x1000;
    va+=0x1000;
  }

  return (u32*)(((u32)virt_addr & 0xFFFFF000)| ((u32)phys_addr & 0x00000FFF));
}

/* unmap page */
void unmap_page(u32 *virt_addr, u32 size) {
}
