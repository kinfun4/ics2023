#include <stdio.h>
#include <NDL.h>
int main() {
  uint32_t rec = NDL_GetTicks(); 
  while(1){
    uint32_t new = NDL_GetTicks();
    printf("time = %d\n", new);
    if(new - rec > 500){
      rec = new;
      printf("Hello!\n");
    }
  }
  return 0;
}
