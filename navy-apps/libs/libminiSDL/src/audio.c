#include <SDL.h>
#include <NDL.h>
#include <assert.h>
#include <stdlib.h>

static SDL_AudioCallback callback;
static void *userdata;
static int stat;
static int interval;
static int samples, channels, byte_per_data;
static uint32_t last_time;

void CheckCallback(){
  if(!stat)return;
  uint32_t cur_time = SDL_GetTicks();
  if(cur_time - last_time > interval){
    last_time = cur_time;
    int len = samples * byte_per_data * channels;
    int count = NDL_QueryAudio();
    len = len > count ? count : len;
    printf("%d\n", len);
    uint8_t *buf = malloc(len);
    assert(buf);
    callback(userdata, buf, len);
    NDL_PlayAudio(buf, len);
    free(buf);
  }
}

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
  assert(desired);
  assert(desired->format == AUDIO_S16SYS);
  assert(obtained == NULL);
  byte_per_data = 2;
  callback = desired->callback;
  samples = desired->samples;
  channels = desired->channels;
  userdata = desired->userdata;
  interval = desired->samples * 1000 / desired->freq /2;
  NDL_OpenAudio(desired->freq, desired->channels, desired->samples);
  return 0;
}

void SDL_CloseAudio() {
}

void SDL_PauseAudio(int pause_on) {
  stat = (pause_on==0);
}

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
  return NULL;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
}

void SDL_LockAudio() {
}

void SDL_UnlockAudio() {
}
