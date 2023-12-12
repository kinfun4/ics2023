#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

size_t *file_offset;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t std_write(const void *buf, size_t offset, size_t len) {
  char *_buf = (char *)buf;
  size_t ret = 0;
  while(ret < len){
    putch(_buf[ret]);
    ret++;
  }
  return ret;
}

/* This is the information about all files in disk. */
static int file_cnt;
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, std_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, std_write},
#include "files.h"
};

void init_fs() {
  file_cnt = LENGTH(file_table); 
  file_offset = malloc(file_cnt);
  memset(file_offset, 0, file_cnt);
  for(int i = FD_FB; i < file_cnt; i++){
      file_table[i].read = ramdisk_read;
      file_table[i].write = ramdisk_write;
    }
}

int fs_open(const char *pathname, int flags, int mode) {
  for(int i = FD_FB; i < file_cnt; i++){
    if(strcmp(pathname, file_table[i].name) == 0){
      return i;
    }
  }
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len){
  size_t offset = file_table[fd].disk_offset + file_offset[fd];
  if(fd >= FD_FB)file_offset[fd] += len;
  assert(file_offset[fd] <= file_table[fd].size);
  return file_table[fd].read((char *)buf, offset, len);
}


size_t fs_write(int fd, void *buf, size_t len) {
  size_t offset = file_table[fd].disk_offset + file_offset[fd];
  if(fd >= FD_FB)file_offset[fd] += len;
  assert(file_offset[fd] <= file_table[fd].size);
  return file_table[fd].write((char *)buf, offset, len);
}

int fs_close(int fd){
  return 0;
}

size_t fs_lseek(int fd, size_t offset, int whence){
  switch (whence) {
    case SEEK_SET:
      file_offset[fd] = offset;
      break;
    case SEEK_CUR:
      file_offset[fd] += offset;
      break;
    case SEEK_END:
      file_offset[fd] += file_table[fd].size + offset;
      break;
  }
  assert(file_offset[fd] >= 0 && file_offset[fd] <= file_table[fd].size);
  return file_offset[fd];
}
