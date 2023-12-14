#include <stdio.h>
#include <assert.h>
#include <fixedptc.h>
int main() {
  fixedpt a = fixedpt_rconst(3.2);
  fixedpt b = fixedpt_fromint(-3.2);
  int ia;
  ia =  fixedpt_toint(fixedpt_muli(a, 3));
  assert(ia == (int)3.2 * 3);
  ia =  fixedpt_toint(fixedpt_mul(a, b));
  printf("get = %d, want = %d\n", ia, (int)3.2 * (int)-3.2);
  assert(ia == (int)3.2 * (int)-3.2);
  ia =  fixedpt_toint(fixedpt_divi(a, 2));
  assert(ia == (int)3.2 / 2);
  printf("PASS\n");
  return 0;
}
