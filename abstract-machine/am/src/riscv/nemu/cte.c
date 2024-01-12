#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;
void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

#define IRQ_TIMER 0x80000007
#define IRQ_ECALL 0x0000000b

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case IRQ_ECALL: ev.event = c->GPR1 == -1 ? EVENT_YIELD : EVENT_SYSCALL; c->mepc +=4; break; // Enviroment call from M-mode
      case IRQ_TIMER: ev.event = EVENT_IRQ_TIMER; break;

      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));
  asm volatile("csrw mstatus, %0" : : "r"(0x1800));

  // register event handler
  user_handler = handler;

  return true;
}

#define SP 2
#define MIE 0x8
#define MIPE 0x80

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *c = (Context *)kstack.end - 1;
  c->gpr[SP] = (uintptr_t)kstack.end;
  c->GPRx = (uintptr_t)arg;
  c->mepc = (uintptr_t)entry;
  c->mcause = 0x0;
  c->mstatus = 0x1800 | MIE | MIPE;
  c->pdir = NULL;
  c->np = 0;
  return c;
}

void yield() {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
