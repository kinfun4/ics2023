#include <am.h>
#include <nemu.h>
#include <stdbool.h>
#include <stdint.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)

static uint32_t offset_addr=0;
static uint32_t bufsize=0;

void __am_audio_init() {
  bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
  offset_addr = 0;
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  cfg->present = true;
  cfg->bufsize = bufsize;
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
  outl(AUDIO_FREQ_ADDR, ctrl->freq);
  outl(AUDIO_CHANNELS_ADDR, ctrl->channels);
  outl(AUDIO_SAMPLES_ADDR, ctrl->samples);
  outl(AUDIO_INIT_ADDR, 1);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = inl(AUDIO_COUNT_ADDR);
  outl(AUDIO_COUNT_ADDR, stat->count);
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  int len = ctl->buf.end - ctl->buf.start;
  uint8_t *ptr = ctl->buf.start;
  int nwrite = 0;
  int count;
  while(nwrite<len){
    count = inl(AUDIO_COUNT_ADDR);
    while(count<bufsize && nwrite<len){
      outb(AUDIO_SBUF_ADDR + offset_addr, *(ptr + nwrite));
      offset_addr = (offset_addr + 1) % bufsize;
      nwrite++;
      count++;
    }
    outl(AUDIO_COUNT_ADDR, count);
  }
}
