#include "syscall.h"
#include "am.h"
#include "debug.h"
#include <common.h>

static void sys_write(int fd, void *buf, size_t cnt) {
  char *_buf = buf;
  switch (fd) {
  case 1:
    for (int i = 0; i < cnt; i++)
      putch(*(_buf + i));
    break;
  case 2:
    for (int i = 0; i < cnt; i++)
      putch(*(_buf + i));
    break;
  default: panic("Invalid fd = %d\n",fd);
  }
}
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  // printf("Before Syscall: ID = %d, GPR2 = %d, GPR3 = %d, GPR4 = %d\n", a[0],
  // a[1], a[2], a[3]);

  switch (a[0]) {
  case SYS_exit:
    halt(a[1]);
    break;
  case SYS_yield:
    yield();
    break;
  case SYS_write:
    sys_write(a[1], (void *)a[2], a[3]);
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }

  // printf("After Syscall: ID = %d, GPRX = %d\n", a[0], c->GPRx);
}
