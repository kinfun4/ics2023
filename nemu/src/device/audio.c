/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 *PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 *KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 *NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_stdinc.h>
#include <assert.h>
#include <common.h>
#include <device/map.h>
#include <device/mmio.h>
#include <sys/types.h>

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;
static uint8_t silence = 0;
static uint32_t offset_addr = 0;

static void audio_play(void *userdata, uint8_t *stream, int len) {
  int nread = 0;
  int count = audio_base[reg_count];
  while (nread < len && count > 0) {
    *(stream + nread) = *(sbuf + offset_addr);
    offset_addr = (offset_addr + 1) % CONFIG_SB_SIZE;
    nread++;
    count--;
  }
  audio_base[reg_count] = count;
  if (nread < len)
    memset(stream + nread, silence, len - nread);
};

static void init_audio_ctrl() {
  SDL_AudioSpec s;
  SDL_zero(s);
  s.format = AUDIO_S16SYS;
  s.userdata = NULL;
  s.freq = audio_base[reg_freq];
  s.channels = audio_base[reg_channels];
  s.samples = audio_base[reg_samples];
  s.callback = audio_play;
  assert(SDL_InitSubSystem(SDL_INIT_AUDIO) == 0);
  assert(SDL_OpenAudio(&s, NULL) == 0);
  silence = s.silence;
  SDL_PauseAudio(0);
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  if (audio_base[reg_init] == 1) {
    init_audio_ctrl();
    audio_base[reg_init] = 0;
  }
  if (offset == reg_count * 4 && len == 4 && !is_write) {
    SDL_LockAudio();
  }
  if (offset == reg_count * 4 && len == 4 && is_write) {
    SDL_UnlockAudio();
  }
}

void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (uint32_t *)new_space(space_size);
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size,
              audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size,
               audio_io_handler);
#endif
  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, NULL);
  audio_base[reg_sbuf_size] = CONFIG_SB_SIZE;
  offset_addr = 0;
}
