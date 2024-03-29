#ifndef __PROC_H__
#define __PROC_H__

#include <common.h>
#include <memory.h>

#define PG_PER_STACK 8
#define STACK_SIZE (PG_PER_STACK * PGSIZE)

typedef union {
  uint8_t stack[STACK_SIZE] PG_ALIGN;
  struct {
    Context *cp;
    AddrSpace as;
    // we do not free memory, so use `max_brk' to determine when to call _map()
    uintptr_t max_brk;
  };
} PCB;

uintptr_t loader(PCB *pcb, const char *filename);
void naive_uload(PCB *pcb, const char *filename);
extern PCB *current;
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);
int execve(const char *filename, char *const argv[], char *const envp[]);
void switch_fgpcb (int k);

#endif
