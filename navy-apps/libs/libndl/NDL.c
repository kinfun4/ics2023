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
static int fb_fd, event_fd, dispinfo_fd, sb_fd, sbctl_fd;
static uint32_t start_time;
static struct timeval t;

uint32_t NDL_GetTicks() {
  gettimeofday(&t, NULL);
  uint32_t ticks = t.tv_sec * 1000 + t.tv_usec / 1000 - start_time;
  return ticks;
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

void NDL_OpenAudio(int freq, int channels, int samples) {
  int buf[3];
  buf[0] = freq;
  buf[1] = channels;
  buf[2] = samples;
  write(sbctl_fd, buf, sizeof(int) * 3);
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return write(sb_fd, buf, len);
}

int NDL_QueryAudio() {
  char buf[8];
  int count;
  read(sbctl_fd, buf, sizeof(buf));
  sscanf(buf, "%d", &count);
  return count;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  // init file
    printf("%d\n", __LINE__);
  fb_fd = open("/dev/fb", O_WRONLY);
  event_fd = open("/dev/events", O_RDONLY);
  dispinfo_fd = open("/proc/dispinfo", O_RDWR);
  sbctl_fd = open("/dev/sbctl", O_RDWR);
  sb_fd = open("/dev/sb", O_WRONLY);
  // init time
  gettimeofday(&t, NULL);
  start_time = t.tv_sec * 1000 + t.tv_usec / 1000;
  // init fb
  char buf[64];
  assert(read(dispinfo_fd, buf, sizeof(buf)));
  sscanf(buf, "WIDTH:%d\nHEIGHT:%d", &width, &height);
  uint32_t *buffer = malloc(sizeof(uint32_t) * width * height);
  memset(buffer, 0, sizeof(uint32_t) * width * height);
  NDL_DrawRect(buffer, 0, 0, width, height);
  free(buffer);

  return 0;
}

void NDL_Quit() {
  close(fb_fd);
  close(event_fd);
  close(dispinfo_fd);
  close(sbctl_fd);
  close(sb_fd);
}
