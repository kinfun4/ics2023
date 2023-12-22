#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int fb_fd, event_fd, dispinfo_fd;

uint32_t NDL_GetTicks() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec * 1000 + t.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int ret = read(event_fd, buf, len);
  return ret;
}

static int width, height;

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w;
    screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    assert(write(fbctl, buf, len) == len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0)
        continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0)
        break;
    }
    close(fbctl);
  }
  char buf[64];
  assert(read(dispinfo_fd, buf, sizeof(buf)));
  sscanf(buf, "WIDTH:%d\nHEIGHT:%d", &width, &height);
  if (*w == 0 && *h == 0) {
    *w = width;
    *h = height;
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  for (int i = 0; i < h; i++) {
    lseek(fb_fd, ((y + i) * width + x) * sizeof(uint32_t), SEEK_SET);
    assert(write(fb_fd, pixels + i * w, sizeof(uint32_t) * w) == sizeof(uint32_t) * w);
  }
}

void NDL_OpenAudio(int freq, int channels, int samples) {}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) { return 0; }

int NDL_QueryAudio() { return 0; }

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  fb_fd = open("/dev/fb", O_WRONLY);
  event_fd = open("/dev/events", O_RDONLY);
  dispinfo_fd = open("/proc/dispinfo", O_RDWR);
  return 0;
}

void NDL_Quit() {
  close(fb_fd);
  close(event_fd);
  close(dispinfo_fd);
}
