#include <stdio.h>
#include <NDL.h>
int main() {
  NDL_Init(0);
  uint32_t rec = NDL_GetTicks(); 
  while(1){
    uint32_t new = NDL_GetTicks();
    if(new - rec > 500){
      rec = new;
      printf("Hello!\n");
    }
  }
  NDL_Quit();
  return 0;
}
