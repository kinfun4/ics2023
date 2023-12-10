#include "syscall.h"
#include "am.h"
#include "debug.h"
#include <common.h>

static size_t sys_write(int fd, void *buf, size_t cnt) {
  char *_buf = buf;
  size_t ret;
  switch (fd) {
  case 1:
    for (ret = 0; ret < cnt; ret++)
      putch(*(_buf + ret));
    break;
  case 2:
    for (ret = 0; ret < cnt; ret++)
      putch(*(_buf + ret));
    break;
  default: panic("Invalid fd = %d\n",fd);
  }
  printf("\nret = %d\n",ret);
  return ret;
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
    c->GPRx = 0;
    break;
  case SYS_write:
    c->GPRx = sys_write(a[1], (void *)a[2], a[3]);
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }

  // printf("After Syscall: ID = %d, ret = %d\n", a[0], c->GPRx );
}
