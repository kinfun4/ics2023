#include <proc.h>
#include <stdint.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void context_kload(PCB *p, void (*entry)(void *), void * arg){
  p->cp = kcontext((Area) { p->stack, p + 1 }, entry, arg);
}

intptr_t uload(PCB *pcb, const char *filename);
void context_uload(PCB *p, const char *filename){
  intptr_t entry = uload(p, filename);
  p->cp = ucontext(&p->as, (Area) { p->stack, p + 1 }, (void *)entry);
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
  context_kload(&pcb[0], hello_fun, (void *)0);
  context_uload(&pcb[1], "/bin/hello");
  switch_boot_pcb();
  Log("Initializing processes...");
  // naive_uload(NULL, "/bin/nterm");
  // load program here
}

Context* schedule(Context *prev) {
  current->cp = prev;
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}
