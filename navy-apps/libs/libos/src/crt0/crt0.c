#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

void __libc_init_array (void);
int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc = args[0];
  char **argv = (char **)(args + 1);
  char **envp = (char **)(args + argc + 2);
  // char *empty[] =  {NULL };
  putchar('A');
  #define N 0x10000
  char *p = malloc(N);
  for (int i = 0; i < N; i++, p++)
    putchar(*p);
  exit(0);
  environ = envp;
  __libc_init_array();
  exit(main(argc, argv, envp));
  assert(0);
}
