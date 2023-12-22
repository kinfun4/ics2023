#include <am.h>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>

Area heap;

void putch(char ch) {
  putchar(ch);
}

void halt(int code) {
  exit(code);
}
