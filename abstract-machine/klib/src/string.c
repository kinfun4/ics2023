#include <klib.h>
#include <klib-macros.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t i=0;
  while(s[i]!='\0'){
    i++;
  }
  return i;
}

char *strcpy(char *dst, const char *src) {
  size_t i=0;
  while(src[i]!='\0'){
    dst[i]=src[i];
    i++;
  }
  dst[i]='\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;
  for(i=0;i<n && src[i]!='\0';i++)
    dst[i]=src[i];
  for(;i<n;i++)
    dst[i]='\0';
  return dst;
}

char *strcat(char *dst, const char *src) {
  size_t dst_len=strlen(dst);
  size_t i=0;
  while(src[i]!='\0'){
    dst[dst_len+i]=src[i];
    i++;
  }
  dst[dst_len+i]='\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  size_t i=0;
  int ret;
  while(s1[i]!='\0'&&s2[i]!='\0'){
    ret = (unsigned char)s1[i] - (unsigned char)s2[i];
    if(ret!=0)return ret;
    i++;
  }
  ret= (unsigned char)s1[i]-(unsigned char)s2[i];
  return ret;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t i;
  int ret;
  for(i=0;i<n&&s1[i]!='\0'&&s2[i]!='\0';i++){
    ret = (unsigned char)s1[i] - (unsigned char)s2[i];
    if(ret!=0)return ret;
  }
  ret = (unsigned char)s1[i] - (unsigned char)s2[i];
  return ret;
}

void *memset(void *s, int c, size_t n) {
  size_t i;
  char* _s = (char*)s;
  for(i=0;i<n;i++){
    _s[i]=c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  size_t i;
  char* tmp = (char*) malloc(n);
  char* _dst = (char*) dst;
  char* _src = (char*) src;
  for(i=0;i<n;i++){
    tmp[i]=_src[i];
  }
  for(i=0;i<n;i++){
    _dst[i]=tmp[i];
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  size_t i;
  char* _out =out;
  char* _in = (char*)in;
  for (i=0; i<n; i++){
    _out[i]=_in[i];
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  size_t i;
  int ret;
  unsigned char* _s1=(unsigned char*)s1;
  unsigned char* _s2=(unsigned char*)s2;
  for (i=0; i<n; i++) {
    ret=_s1[i]-_s2[i];
    if(ret!=0)return ret;
  }
  return ret;
}

#endif
