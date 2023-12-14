#include <stdio.h>
#include <assert.h>
#include <fixedptc.h>
#define CHECK(a,b) assert((int)(a) == (int)(b));
#define PRINT(a,b) printf("get = %d, want = %d\n", (int)a, (int)(b));
int main() {
  fixedpt a = fixedpt_rconst(3.2);
  fixedpt b = fixedpt_fromint(-3.2);
  int ia;
  ia =  fixedpt_toint(fixedpt_muli(a, 3));
  assert(ia == (int)3.2 * 3);
  ia =  fixedpt_toint(fixedpt_mul(a, b));
  PRINT(ia, 3.2 * -3.2)
  assert(ia == (int)(3.2 * -3.2));
  ia =  fixedpt_toint(fixedpt_divi(a, 2));
  assert(ia == (int)3.2 / 2);
  printf("PASS\n");
  return 0;
}

