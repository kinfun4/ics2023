#include <common.h>
#include <stdio.h>

static Context* do_event(Event e, Context* c) {
  printf("event ID = %d\n", e.event);
  switch (e.event) {
    case EVENT_YIELD: printf("EVENT_YIELD\n");break;
    default: panic("Unhandled event ID = %d", e.event);break;
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
