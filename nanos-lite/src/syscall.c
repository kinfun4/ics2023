#include <common.h>
#include "syscall.h"
#include "am.h"
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  printf("GPR2=%d\n",c->GPR2);

  switch (a[0]) {
    case SYS_exit: halt(c->GPR2); break;
    case SYS_yield: yield();break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
