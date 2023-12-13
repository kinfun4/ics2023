#include <stdio.h>
#include <NDL.h>

char buf[64];
int main() {
  NDL_Init(0);
  while (1) {
    if (NDL_PollEvent(buf, sizeof(buf))) {
      printf("receive event: %s\n", buf);
    }
  }
  return 0;
}
