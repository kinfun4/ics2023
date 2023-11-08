#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <stddef.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void int2str(int num, char *str,int* p) {
    int i = *p;

    if (num < 0) {
        num = -num;
        str[i++] = '-';
    }

    do {
        str[i++] = num % 10 + '0';
        num /= 10;
    } while (num);

    int j = *p;

    if (str[j] == '-') {
        j++;
    }

    for ( int k=j; k < (i+j) / 2; k++) {
        str[k] = str[k] + str[i - 1 - (k-j)];
        str[i - 1 - (k-j)] = str[k] - str[i - 1 - (k-j)];
        str[k] = str[k] - str[i - 1 - (k-j)];
    }
    *p=i;
}

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  size_t i=0;
  int ret=0;
  char *s;size_t j;
  int d;
  va_start(ap, fmt);
  while(fmt[i]!='\0'){
    if(fmt[i]!='%'){
      out[ret++]=fmt[i++];
    }
    else {
      switch (fmt[i+1]) {
        case 's': 
          s=va_arg(ap, char*);
          j=0;
          i=i+2;
          while(s[j]!='\0'){
            out[ret++]=s[j++];
          }
          break;
        case 'd':
          d=va_arg(ap, int);
          i=i+2;
          int2str(d, out,&ret);
          break;
        default:i=i+2;break;
      }
    }
  }
  out[ret]='\0';
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
