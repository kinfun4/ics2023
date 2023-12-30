#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void __libc_init_array (void);
int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc = args[0];
  char **argv = (char **)(args + 1);
  char **envp = (char **)(args + argc + 2);
  printf("%p\n", argv);
  printf("%p\n", argv[0]);
  char *empty[] =  {NULL };
  environ = empty;
  __libc_init_array();
  exit(main(0, empty, empty));
  assert(0);
}
