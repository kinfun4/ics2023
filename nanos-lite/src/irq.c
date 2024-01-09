#include <common.h>

void do_syscall(Context *c);
Context *schedule(Context *prev);

static Context *do_event(Event e, Context *c) {
printf("%d\n", __LINE__);
  switch (e.event) {
  case EVENT_YIELD:
    printf("EVENT_YIELD\n");
    c = schedule(c);
    break;
  case EVENT_SYSCALL:
    do_syscall(c);
    break;
  case EVENT_ERROR:
    panic("EVENT_ERROR\n");
    break;
  case EVENT_IRQ_TIMER:
    break;
  default:
    panic("Unhandled event ID = %d", e.event);
    break;
  }
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
