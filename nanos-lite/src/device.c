#include <common.h>
#include <stdio.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
#define MULTIPROGRAM_YIELD() yield()
#else
#define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) [AM_KEY_##key] = #key,

static int width, height;

static const char *keyname[256]
    __attribute__((used)) = {[AM_KEY_NONE] = "NONE", AM_KEYS(NAME)};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *_buf = (char *)buf;
  size_t ret = 0;
  while (ret < len) {
    putch(_buf[ret]);
    ret++;
  }
  return ret;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  char *_buf = buf;
  int ret = 0;
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE)
    return 0;
  if (ev.keydown)
    sprintf(buf, "kd %s\n", keyname[ev.keycode]);
  else
    sprintf(buf, "ku %s\n", keyname[ev.keycode]);
  while (_buf[ret] != '\0')
    ret++;
  return ret + 1;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) { 
  char *_buf = buf;
  int ret = 0;
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  width = config.width;
  height = config.height;
  printf("w = %d, h = %d\n", width, height);
  sprintf(_buf, "WIDTH:%d\nHEIGHT:%d", width, height);
  while (_buf[ret] != '\0')
    ret++;
  return ret + 1;
}

size_t fb_write(void *buf, size_t offset, size_t len) {
  int x = offset % width;
  int y = offset / width;
  io_write(AM_GPU_FBDRAW, x, y, buf, len, 1, true);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
