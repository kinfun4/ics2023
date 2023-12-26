#include <SDL.h>
#include <NDL.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static SDL_AudioCallback callback;
static void *userdata;
static int stat;
static int interval;
static int samples, channels, byte_per_data;
static uint32_t last_time;
static int frame_size;

void CheckCallback(){
  if(!stat)return;
  uint32_t cur_time = SDL_GetTicks();
  if(cur_time - last_time > interval){
    int free_size = NDL_QueryAudio();
    if(free_size < frame_size)return;
    last_time = cur_time;
    uint8_t *buf = malloc(frame_size);
    assert(buf);
    callback(userdata, buf, frame_size);
    int count = 0;
    while(count< frame_size){
      int ret = NDL_PlayAudio(buf + count, frame_size - count);
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
  frame_size = samples * byte_per_data * channels;
  NDL_OpenAudio(desired->freq, desired->channels, desired->samples);
  return 0;
}

void SDL_CloseAudio() {
}

void SDL_PauseAudio(int pause_on) {
  stat = (pause_on==0);
}

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
  assert(!(len&1));
  len = len/2;
  int16_t *_dst = (int16_t *)dst;
  int16_t *_src = (int16_t *)src;
  int tmp;
  for (int i=0; i<len; i++) {
    tmp = (int)_dst[i] + _src[i] * volume / SDL_MIX_MAXVOLUME;
    if(tmp > INT16_MAX) tmp = INT16_MAX;
    if(tmp < INT16_MIN) tmp = INT16_MIN;
    _dst[i] = (int16_t) tmp;
  }
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
  FILE* fp = fopen(file, "r");
  assert(fp);
  RIFF_header header;
  assert(fread(&header, sizeof(RIFF_header), 1, fp) == 1);
  assert(header.ChunkID == 0x46464952);
  assert(header.Format == 0x45564157);
  WAV_fmt fmt;
  assert(fread(&fmt, sizeof(WAV_fmt), 1, fp) == 1);
  assert(fmt.SubchunkID == 0x20746d66);
  assert(fmt.SubchunkSize == 16);
  assert(fmt.AduioFormat == 1);
  WAV_data data;
  assert(fread(&data, sizeof(WAV_data), 1, fp) == 1);
  assert(data.SubchunkID == 0x61746164);
  assert(fmt.BitsPerSample == AUDIO_S16SYS);
  spec->freq = fmt.SampleRate;
  spec->format = fmt.BitsPerSample;
  spec->channels = fmt.NumChannels;
  spec->samples = 4096;
  *audio_buf = malloc(data.SubchunkSize);
  assert(*audio_buf);
  assert(fread(*audio_buf, data.SubchunkSize, 1, fp) == 1);
  *audio_len = data.SubchunkSize;
  fclose(fp);
  return spec;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
  free(audio_buf);
}

void SDL_LockAudio() {
}

void SDL_UnlockAudio() {
}
