#include "am.h"
#include <memory.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *ret = pf;
  pf += nr_page * PGSIZE;
  return ret;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  assert(n == PGSIZE);
  void *ptr = new_page(1);
  memset(ptr, 0, PGSIZE);
  return ptr;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
extern PCB *current;
int mm_brk(uintptr_t brk) {
  while (current->max_brk < brk) {
    void *paddr = new_page(1);
    void *vaddr = (void *)current->max_brk;
    map(&current->as, vaddr, paddr, 0x7);
    current->max_brk += PGSIZE;
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
