#include <NDL.h>
#include <assert.h>
#include <sdl-event.h>
#include <stdio.h>
#include <string.h>

#define keyname(k) #k,

void CheckCallback();

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static uint8_t keystate[SDLK_NUM];

int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[64];
  CheckCallback();
  if(NDL_PollEvent(buf, sizeof(buf)) == 0)
    return 0;
  if(strncmp(buf, "ku", 2) == 0){
    ev->key.type = SDL_KEYUP;
    char name[10];
    sscanf(buf, "ku %s", name);
    for(int i=0;i < SDLK_NUM; i++){
      if(strcmp(name, keyname[i]) == 0){
        ev->key.keysym.sym = i;
        keystate[i] = 0;
        return 1;
      }
    }
    assert(0);
  }
  if(strncmp(buf, "kd", 2) == 0){
    ev->key.type = SDL_KEYDOWN;
    char name[10];
    sscanf(buf, "kd %s", name);
    for(int i=0;i < SDLK_NUM; i++){
      if(strcmp(name, keyname[i]) == 0){
        ev->key.keysym.sym = i;
        keystate[i] = 1;
        return 1;
      }
    }
    assert(0);
  }
  assert(0);
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  while (!SDL_PollEvent(event))
    continue;
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  if(numkeys != NULL)
    *numkeys = SDLK_NUM;
  return keystate;
}
