#include <am.h>
#include <NDL.h>
#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {

  NDL_PollEvent(char *buf, int len)
  kbd->keycode = ;
  kbd->keydown = (kbd->keycode & KEYDOWN_MASK) ? 1 : 0;
  kbd->keycode = kbd->keycode & ~KEYDOWN_MASK;
}
