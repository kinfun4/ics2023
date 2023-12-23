#include <am.h>
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
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
}
