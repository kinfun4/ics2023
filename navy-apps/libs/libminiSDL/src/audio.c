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
    int free_size = NDL_QueryAudio();
    printf("%d\n", free_size);
    len = len > free_size ? free_size : len;
    uint8_t *buf = malloc(len);
    assert(buf);
    callback(userdata, buf, len);
    int count = 0;
    while(count< len){
      int ret = NDL_PlayAudio(buf + count, len - count);
      if(ret == -1) ret = 0;
      count += ret;
    }
    free(buf);
  }
}

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
  assert(desired);
  assert(desired->format == AUDIO_S16SYS);
  if(obtained != NULL){
    obtained->format = desired->format;
    obtained->callback = desired->callback;
    obtained->samples = desired->samples;
    obtained->channels = desired->channels;
    obtained->userdata = desired->userdata;
    obtained->freq = desired->freq;
  }
  byte_per_data = desired->format / 8;
  callback = desired->callback;
  samples = desired->samples;
  channels = desired->channels;
  userdata = desired->userdata;
  interval = desired->samples * 1000 / desired->freq / 3 ;
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
