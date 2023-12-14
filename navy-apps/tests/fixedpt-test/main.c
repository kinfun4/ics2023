#include <stdio.h>
#include <assert.h>
#include <fixedptc.h>
#define ASSERT(x, y) assert((int)(x) == (int)(y))
#define PRINT(x, y) printf("get = %d, want = %d\n", (int)x, (int)(y))
#define INT(x) fixedpt_toint(x)
#define INIT(x, y) \
  fixedpt a = fixedpt_rconst(x);\
  fixedpt b = fixedpt_rconst(y);\
  int ia

int main() {
  INIT(3.2, -3.2);
  ia =  INT(fixedpt_muli(a, 3));
  assert(ia == (int)(3.2 * 3));
  ia =  INT(fixedpt_mul(a, b));
  PRINT(ia , (int)(3.2 * -3.2));
  assert(ia == (int)(3.2 * -3.2));
  ia =  INT(fixedpt_divi(a, 2));
  assert(ia == (int)3.2 / 2);
  ia =  INT(fixedpt_div(a, b));
  assert(ia == (int)(3.2 / -3.2));
  ia =  INT(fixedpt_floor(a));
  assert(ia == 3);
  ia =  INT(fixedpt_floor(b));
  assert(ia == -4);
  ia =  INT(fixedpt_ceil(a));
  assert(ia == 4);
  ia =  INT(fixedpt_ceil(b));
  assert(ia == -3);
  printf("PASS\n");
  return 0;
}

