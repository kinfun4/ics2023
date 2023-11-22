#include <am.h>
#include <nemu.h>
#include <stdint.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
#ifdef MODE_800x600
#define W 800
#define H 600
#else
#define W 400
#define H 300
#endif

void __am_gpu_init() {}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T){.present = true,
                           .has_accel = false,
                           .width = W,
                           .height = H,
                           .vmemsz = 0};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
    int x = ctl->x, y = ctl->y;
    int w = ctl->w, h = ctl->h;
    int i, j;
    int offset_addr;
    for (i = 0; i < h; i++)
      for (j = 0; j < w; j++) {
        offset_addr = (y + i) * W + (x + j);
        outl(FB_ADDR + offset_addr, *(uint32_t *)(ctl->pixels));
      }
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) { status->ready = true; }
