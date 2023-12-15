#include <stdio.h>
#include <assert.h>
#include <fixedptc.h>
#define CHECK(x, y)\
  printf("get = %8d, want = %8d\n", (int)(x), (int)(y));\
  assert((int)(x) == (int)(y))
#define INT(x) fixedpt_toint(x)
#define INIT(x, y) \
  fixedpt a = fixedpt_rconst(x);\
  fixedpt b = fixedpt_rconst(y);\
  int ia

int main() {
  INIT(3.2, -3.2);

  ia =  INT(fixedpt_muli(a, 3));
  CHECK(ia , 3.2 * 3);

  ia =  INT(fixedpt_mul(a, b));
  CHECK(ia , 3.2 * -3.2);

  ia =  INT(fixedpt_divi(a, 2));
  CHECK(ia , 3.2 / 2);

  ia =  INT(fixedpt_div(a, b));
  CHECK(ia , 3.2 / -3.2);

  ia =  INT(fixedpt_floor(a));
  CHECK(ia , 3);

  ia =  INT(fixedpt_floor(b));
  CHECK(ia , -4);

  ia =  INT(fixedpt_ceil(a));
  CHECK(ia , 4);

  ia =  INT(fixedpt_ceil(b));
  CHECK(ia , -3);

  printf("PASS\n");
  return 0;
}
