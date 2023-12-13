#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

size_t *file_offset;

enum { FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT, FD_DISPINFO, FD_FB };

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static int file_cnt;
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
    [FD_EVENT] = {"/dev/events", 0, 0, events_read, invalid_write},
    [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

void init_fs() {
  file_cnt = LENGTH(file_table);
  file_offset = malloc(file_cnt);
  memset(file_offset, 0, file_cnt * sizeof(size_t));
  for (int i = FD_FB; i < file_cnt; i++) {
    file_table[i].read = ramdisk_read;
    file_table[i].write = ramdisk_write;
  }
}

int fs_open(const char *pathname, int flags, int mode) {
  for (int i = FD_STDIN; i < file_cnt; i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      return i;
    }
  }
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
  size_t offset = file_table[fd].disk_offset + file_offset[fd];
  if (fd >= FD_FB){
    if(len > file_table[fd].size - file_offset[fd])
        len = file_table[fd].size - file_offset[fd];
    file_offset[fd] += len;
  }
  int ret = file_table[fd].read(buf, offset, len);
  return ret;
}

size_t fs_write(int fd, void *buf, size_t len) {
  size_t offset = file_table[fd].disk_offset + file_offset[fd];
  if (fd >= FD_FB){
    if(len > file_table[fd].size - file_offset[fd])
        len = file_table[fd].size - file_offset[fd];
    file_offset[fd] += len;
  }
  return file_table[fd].write((char *)buf, offset, len);
}

int fs_close(int fd) { return 0; }

size_t fs_lseek(int fd, size_t offset, int whence) {
  printf("offset = %d\n", offset);
  switch (whence) {
  case SEEK_SET:
    file_offset[fd] = offset;
    break;
  case SEEK_CUR:
    file_offset[fd] += offset;
    break;
  case SEEK_END:
    file_offset[fd] = file_table[fd].size + offset;
    break;
  default: assert(0);
  }
  assert(file_offset[fd] >= 0 && file_offset[fd] <= file_table[fd].size);
  return file_offset[fd];
}
