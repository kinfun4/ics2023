#include "syscall.h"
#include <proc.h>
#include <stdio.h>
#include <sys/time.h>

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static void get_time(struct timeval *t){
  t->tv_usec = io_read(AM_TIMER_UPTIME).us;
  t->tv_sec = t->tv_usec / 1000000;
  t->tv_usec %= 1000000;
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
    assert(a[1] == 0);
      // halt(0);
    char *empty[] = {NULL};
    execve("/bin/nterm", empty, empty);
    naive_uload(NULL, "/bin/nterm");
    break;
  case SYS_yield:
    yield();
    c->GPRx = 0;
    break;
  case SYS_open:
    c->GPRx = fs_open((char *)a[1], a[2], a[3]);
    break;
  case SYS_read:
    c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
    break;
  case SYS_write:
    c->GPRx = fs_write(a[1], (void *)a[2], a[3]);
    break;
  case SYS_close:
    c->GPRx = fs_close(a[1]);
    break;
  case SYS_lseek:
    c->GPRx = fs_lseek(a[1], a[2], a[3]);
    break;
  case SYS_brk:
    c->GPRx = 0;
    break;
  case SYS_execve:
    c->GPRx = execve((char *)a[1], (char **)a[2], (char **)a[3]);
    break;
  case SYS_gettimeofday:
    c->GPRx = 0;
    get_time((void *)a[1]); 
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }

  // printf("After Syscall: ID = %d, ret = %d\n", a[0], c->GPRx );
}
