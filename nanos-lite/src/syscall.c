#include <common.h>
#include "syscall.h"
#include "am.h"
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  printf("Before Syscall: ID = %d, GPR2 = %d, GPR3 = %d, GPR4 = %d\n", a[0], a[1], a[2], a[3]);

  switch (a[0]) {
    case SYS_exit: halt(c->GPR2); break;
    case SYS_yield: yield();break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  printf("After Syscall: ID = %d, GPRX = %d\n", a[0], c->GPRx);
}
