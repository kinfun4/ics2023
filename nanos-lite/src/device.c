#include "klib-macros.h"
#include <common.h>
#include <stdio.h>
#include <string.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

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
  memset(buf, 0, sizeof(buf));
  int ret = 0;
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD); 
  if(ev.keycode == AM_KEY_NONE)return 0;
  if(ev.keydown) strcpy(_buf, "kd ");
  else strcpy(_buf, "ku ");
  ret = 3;
  for(int i = 0; keyname[ev.keycode][i] != '\0'; i++, ret++)
    _buf[ret] = keyname[ev.keycode][i];
  _buf[ret++] = '\n';
  _buf[ret] = '\0';
  return ret;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
