#include <stdio.h>
#include <assert.h>
#include <fixedptc.h>
#define CHECK(x,y) assert((int)(x) == (int)(y));
#define PRINT(x,y) printf("get = %d, want = %d\n", (int)x, (int)(y));
#define INIT(x,y) \
  fixedpt a = fixedpt_rconst(x);\
  fixedpt b = fixedpt_fromint(y);

int main() {
  INIT(3.2, -3.2)
  int ia;
  ia =  fixedpt_toint(fixedpt_muli(a, 3));
  assert(ia == (int)3.2 * 3);
  ia =  fixedpt_toint(fixedpt_mul(a, b));
  PRINT(ia, 3.2 * -3.2)
  assert(ia == (int)(3.2 * -3.2));
  ia =  fixedpt_toint(fixedpt_divi(a, 2));
  assert(ia == (int)3.2 / 2);
  ia =  fixedpt_toint(fixedpt_div(a, b));
  assert(ia == (int)(3.2 / -3.2));
  printf("PASS\n");
  return 0;
}

