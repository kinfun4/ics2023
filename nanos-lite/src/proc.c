#include <proc.h>

#define MAX_NR_PROC 4
#define PG_PER_STACK 8

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void context_kload(PCB *pcb, void (*entry)(void *), void * arg){
  pcb->cp = kcontext((Area) { pcb->stack, pcb + 1 }, entry, arg);
}

intptr_t uload(PCB *pcb, const char *filename);

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  intptr_t entry = uload(pcb, filename);
  pcb->cp = ucontext(&pcb->as, (Area) { pcb->stack, pcb + 1 }, (void *)entry);

  char *sp = (char *)new_page(PG_PER_STACK);

  int envc = 0,argc = 0;
  while(*(envp + envc) != NULL)envc++;
  while(*(argv + argc) != NULL)argc++;

  char **_envp = malloc((envc + 1) * sizeof(char *));
  char **_argv = malloc((argc + 1) * sizeof(char *));

  for (int i = 0; i < envc; i++) {
    int len = strlen(*(envp + i)) + 1;
    len = (len & ~3) + (len & 3 ? 4 : 0);
    sp -= len;
    _envp[i] = sp;
    strncpy(_envp[i], *(envp + i), len);
  }
  for (int i = 0; i< argc; i++){
    int len = strlen(*(argv + i)) + 1;
    len = (len & ~3) + (len & 3 ? 4 : 0);
    sp -= len;
    _argv[i] = sp;
    strncpy(_argv[i], *(argv + i), len);
  }

  _envp[envc] = NULL;
  sp -= (envc + 1) * sizeof(char *);
  memcpy(sp, _envp, (envc + 1) * sizeof(char *));

  _argv[argc] = NULL;
  sp -= (argc + 1) * sizeof(char *);
  memcpy(sp, _argv, (argc + 1) * sizeof(char *));

  sp -= sizeof(int);
  *(int *)sp = argc;

  pcb->cp->GPRx = (intptr_t) sp;
}

int fs_open(const char *pathname, int flags, int mode);
int execve(const char *filename, char *const argv[], char *const envp[]){
  PCB *p = current == &pcb[0] ? &pcb[1] : &pcb[0];
  assert(fs_open(filename, 0, 0) != -1);
  context_uload(p, filename, argv, envp);
  yield();
  return -1;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  char *filename = "/bin/menu";
  char *argv[] = {filename, NULL};
  char *envp[] = {NULL};
  context_uload(&pcb[0], filename, argv, envp);
  switch_boot_pcb();
  Log("Initializing processes...");
  // naive_uload(NULL, "/bin/nterm");
}

Context* schedule(Context *prev) {
  current->cp = prev;
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}
