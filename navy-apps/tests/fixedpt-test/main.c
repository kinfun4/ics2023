#include <stdio.h>
#include <assert.h>
#include <fixedptc.h>
#define CHECK(x, y)\
  printf("get = %d, want = %d\n", (int)x, (int)(y));\
  assert((int)(x) == (int)(y));
#define INT(x) fixedpt_toint(x)
#define INIT(x, y) \
  fixedpt a = fixedpt_rconst(x);\
  fixedpt b = fixedpt_rconst(y);\
  int ia

int main() {
  INIT(3.2, -3.2);

  ia =  INT(fixedpt_muli(a, 3));
  CHEKC(ia , 3.2 * 3);

  ia =  INT(fixedpt_mul(a, b));
  CHEKC(ia , 3.2 * -3.2);

  ia =  INT(fixedpt_divi(a, 2));
  CHEKC(ia , 3.2 / 2);

  ia =  INT(fixedpt_div(a, b));
  CHEKC(ia , 3.2 / -3.2);

  ia =  INT(fixedpt_floor(a));
  CHEKC(ia , 3);

  ia =  INT(fixedpt_floor(b));
  CHEKC(ia , -4);

  ia =  INT(fixedpt_ceil(a));
  CHEKC(ia , 4);

  ia =  INT(fixedpt_ceil(b));
  CHEKC(ia , -3);

  printf("PASS\n");
  return 0;
}

