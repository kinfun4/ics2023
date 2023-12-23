#include <am.h>
#include <NDL.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

static int width, height;

void __am_gpu_init() {
  NDL_OpenCanvas(&width, &height);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T){.present = true,
                           .has_accel = false,
                           .width = width,
                           .height = height,
                           .vmemsz = 0};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x = ctl->x, y = ctl->y;
  int w = ctl->w, h = ctl->h;
  NDL_DrawRect(ctl->pixels, x, y, w, h);
}

void __am_gpu_status(AM_GPU_STATUS_T *status) { status->ready = true; }
