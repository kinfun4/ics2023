#include "klib-macros.h"
#include "riscv/riscv.h"
#include <am.h>
#include <nemu.h>
#include <klib.h>
#include <stdint.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> PGWIDTH)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);
  printf("kas.ptr = %p\n", kas.ptr);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

#define GET_PPN(pte) (READ_HIGH((pte), 10) << 2)
#define GET_PTE(pa)  (READ_HIGH((pa), 12) >> 2)

void map(AddrSpace *as, void *va, void *pa, int prot) {
  uintptr_t vpn1 = (uintptr_t)va >> 22;
  uintptr_t vpn0 = READ_LEN((uintptr_t)va, 12, 22); 
  PTE *pte1 = (void *)as->ptr + vpn1 * PTESIZE;
  if(!(*pte1 & PTE_V)){
    void *ptr = pgalloc_usr(PGSIZE);
    *pte1 = GET_PTE((uintptr_t)ptr) | PTE_V;  // non-leaf PTE
  }
  PTE* pte0 = (void *)GET_PPN(*pte1) + vpn0 * PTESIZE;
  *pte0 = GET_PTE((uintptr_t)pa) | PTE_V | PTE_R | PTE_W | PTE_X | PTE_A | PTE_D; // leaf PTE
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *c = (Context *)kstack.end - 1;
  c->GPRx = (uintptr_t)heap.end;
  c->mepc = (uintptr_t)entry;
  c->mcause = 0x0;
  c->mstatus = 0x1800;
  return c;
}
