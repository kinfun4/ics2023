#include <proc.h>
#include <fs.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void context_kload(PCB *pcb, void (*entry)(void *), void * arg){
  pcb->cp = kcontext((Area) { pcb->stack, pcb + 1 }, entry, arg);
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  protect(&pcb->as);
  void *sp_vaddr = pcb->as.area.end;

  void *sp_paddr = new_page(PG_PER_STACK) + PG_PER_STACK * PGSIZE;
  assert(sp_paddr);
  char *sp = sp_paddr;

  for(int i = 1;i <= PG_PER_STACK;i++){
    map(&pcb->as, sp_vaddr - i * PGSIZE, sp - i * PGSIZE, 0x7);
  }

  assert(argv && envp);
  int envc = 0,argc = 0;
  while(*(envp + envc) != NULL)envc++;
  while(*(argv + argc) != NULL)argc++;

  char **_envp = malloc((envc + 1) * sizeof(char *));
  char **_argv = malloc((argc + 1) * sizeof(char *));
  assert(_argv && _envp);

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

  free(_argv);
  free(_envp);

  sp -= sizeof(int);
  *(int *)sp = argc;

  intptr_t entry = loader(pcb, filename);
  pcb->cp = ucontext(&pcb->as, (Area) { pcb->stack, pcb + 1 }, (void *)entry);

  pcb->cp->GPRx = (intptr_t) sp_vaddr - (sp_paddr - (void *)sp);
}

int execve(const char *filename, char *const argv[], char *const envp[]){
  PCB *p = current == &pcb[0] ? &pcb[1] : &pcb[0];
  printf("%d\n", current == &pcb[0]);
  if(fs_open(filename, 0, 0) == -1)return -2;
  context_uload(p, filename, argv, envp);
  yield();
  return 0;
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
  char *filename = "/bin/nterm";
  char *argv[] = {filename, NULL};
  char *envp[] = {NULL};
  context_uload(&pcb[0], filename, argv, envp);
  // context_uload(&pcb[1], "/bin/hello", argv, envp);
  // context_kload(&pcb[1], hello_fun, (void *)1);
  switch_boot_pcb();
  Log("Initializing processes...");
  // naive_uload(NULL, "/bin/nterm");
}

Context* schedule(Context *prev) {
  current->cp = prev;
  // current = &pcb[0];
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}
