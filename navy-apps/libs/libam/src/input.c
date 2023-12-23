#include <am.h>
#include <SDL.h>

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  SDL_Event ev;
  if(SDL_PollEvent(&ev)){
    kbd->keycode = ev.key.keysym.sym;
    kbd->keydown = (ev.key.type == SDL_KEYDOWN);
  }
  else kbd->keycode = 0;
}
