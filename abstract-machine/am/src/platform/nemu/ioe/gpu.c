#include <am.h>
#include <nemu.h>
#include <stdint.h>
#include <sys/types.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

static int width, height;

void __am_gpu_init() {
  uint32_t data = inl(VGACTL_ADDR);
  width = data >> 16;
  height = data & ((1 << 16) - 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t data = inl(VGACTL_ADDR);
  width = data >> 16;
  height = data & ((1 << 16) - 1);
  *cfg = (AM_GPU_CONFIG_T){.present = true,
                           .has_accel = false,
                           .width = width,
                           .height = height,
                           .vmemsz = 0};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x = ctl->x, y = ctl->y;
  int w = ctl->w, h = ctl->h;
  int i, j;
  int offset_addr;
  for (i = 0; i < h; i++)
    for (j = 0; j < w; j++) {
      offset_addr = ((y + i) * width + (x + j)) * 4;
      outl(FB_ADDR + offset_addr, *(uint32_t *)(ctl->pixels + (i * w + j) * 4));
    }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) { status->ready = true; }
