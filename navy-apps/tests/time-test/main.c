#include <stdio.h>
#include "../../libs/libndl/include/NDL.h"
int main() {
  uint32_t rec = NDL_GetTicks(); 
  printf("1\n");
  while(1){
    uint32_t new = NDL_GetTicks();
    if(new - rec > 500000){
      rec = new;
      printf("Hello!\n");
    }
  }
  return 0;
}
