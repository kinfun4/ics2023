#include "sys/time.h"
#include <stdio.h>
int main() {
  struct timeval time;
  gettimeofday(&time, NULL);
  long rec = time.tv_usec + time.tv_sec * 1000000;
  while(1){
    gettimeofday(&time, NULL);
    if(time.tv_sec * 1000000 + time.tv_usec - rec > 500000){
      rec = time.tv_sec * 1000000 + time.tv_usec;
      printf("Hello!\n");
    }
  }
  return 0;
}
