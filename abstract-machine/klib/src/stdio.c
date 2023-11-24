#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static bool is_formed, is_filed_width, is_precision, is_show_sign, is_capital,
    space_or_zero, right_or_left, long_number;
static int base, precision, field_width;
static int exit_flag;
static char *buf_pt;
#define BUF_SIZE 100
static char buf[BUF_SIZE];
#define TYPE(x) TYPE##x
#define TYPE0 int
#define TYPE1 long
#define TYPE2 long long

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define WRITE(str, c, state)                                                   \
  do {                                                                         \
    if (state == 0) {                                                          \
      assert(str != NULL);                                                     \
      *(str++) = c;                                                            \
    } else                                                                     \
      putch(c);                                                                \
  } while (0)

#define CHEKE_BOUND(st, pt, bound)                                             \
  if (pt - st >= bound)                                                        \
    assert(0);
#define IS_NUM(c) ((c <= '9') && (c >= '0'))

#define GET_NUM(x, str, ap)                                                    \
  do {                                                                         \
    if (*str == '*')                                                           \
      x = va_arg(ap, int), str++;                                              \
    else {                                                                     \
      while (IS_NUM(*str)) {                                                   \
        x = x * 10 + *str - '0';                                               \
        str++;                                                                 \
      }                                                                        \
    }                                                                          \
  } while (0)

#define ITOA(x, cap)                                                           \
  (char)((x) >= 0 && x <= 9) ? (x) + '0'                                       \
                             : ((cap) ? ((x)-10) + 'A' : ((x)-10) + 'a')

#define CHECK_FLAG(fmt, ap)                                                    \
  do {                                                                         \
    exit_flag = 0;                                                             \
    switch (*fmt) {                                                            \
    case '#':                                                                  \
      is_formed = 1;                                                           \
      fmt++;                                                                   \
      break;                                                                   \
    case '0':                                                                  \
      space_or_zero = 1;                                                       \
      fmt++;                                                                   \
      break;                                                                   \
    case '-':                                                                  \
      right_or_left = 1;                                                       \
      fmt++;                                                                   \
      break;                                                                   \
    case '+':                                                                  \
      is_show_sign = 1;                                                        \
      fmt++;                                                                   \
      break;                                                                   \
    default:                                                                   \
      exit_flag = 1;                                                           \
      break;                                                                   \
    }                                                                          \
  } while (!exit_flag)

#define CHECK_FIELD_WIDTH(fmt, ap)                                             \
  do {                                                                         \
    if (IS_NUM(*fmt) || (*fmt) == '*')                                         \
      is_filed_width = 1;                                                      \
    GET_NUM(field_width, fmt, ap);                                             \
  } while (0)

#define CHECK_PRECISION(fmt, ap)                                               \
  do {                                                                         \
    switch (*fmt) {                                                            \
    case '.':                                                                  \
      fmt++;                                                                   \
      GET_NUM(precision, fmt, ap);                                             \
      is_precision = 1;                                                        \
      space_or_zero = 1;                                                       \
      break;                                                                   \
    default:                                                                   \
      break;                                                                   \
    }                                                                          \
  } while (0)

#define CHECK_LENGTH_MODIFIER(fmt, ap)                                         \
  do {                                                                         \
    while (*fmt == 'l') {                                                      \
      fmt++;                                                                   \
    }                                                                          \
  } while (0)

#define PARSE_ARGS(fmt, ap)                                                    \
  do {                                                                         \
    fmt++;                                                                     \
    CHECK_FLAG(fmt, ap);                                                       \
    CHECK_FIELD_WIDTH(fmt, ap);                                                \
    CHECK_PRECISION(fmt, ap);                                                  \
    CHECK_LENGTH_MODIFIER(fmt, ap);                                            \
  } while (0)

#define PROCESS_CHAR(out, ap, state)                                           \
  do {                                                                         \
    char c = (char)va_arg(ap, int);                                            \
    WRITE(out, c, state);                                                      \
  } while (0)

#define PROCESS_STRING(out, ap, state)                                         \
  do {                                                                         \
    char *s = va_arg(ap, char *);                                              \
    while (*s != '\0') {                                                       \
      WRITE(out, *(s++), state);                                               \
    }                                                                          \
  } while (0)

#define PROCESS_SIGNED(out, ap, state)                                         \
  do {                                                                         \
    assert(is_formed != 1);                                                    \
    assert(base == 10);                                                        \
    signed TYPE2 x = va_arg(ap, signed TYPE2);                               \
    signed TYPE2 y;                                                            \
    bool is_negtive = x < 0;                                                   \
    int space = is_precision ? precision : (is_filed_width ? field_width : 1); \
    do {                                                                       \
      y = x % base;                                                            \
      WRITE(buf_pt, ITOA(ABS(y), is_capital), 0);                              \
      assert(buf_pt - buf < BUF_SIZE);                                         \
      x /= base;                                                               \
    } while (x != 0);                                                          \
    if (is_negtive)                                                            \
      space--;                                                                 \
    else if (is_show_sign)                                                     \
      space--;                                                                 \
    if (space_or_zero) {                                                       \
      while (buf_pt - buf < space)                                             \
        WRITE(buf_pt, '0', 0);                                                 \
    }                                                                          \
    if (is_negtive)                                                            \
      WRITE(buf_pt, '-', 0);                                                   \
    else if (is_show_sign)                                                     \
      WRITE(buf_pt, '+', 0);                                                   \
    int siz = buf_pt - buf;                                                    \
    if (right_or_left) {                                                       \
      while (buf_pt > buf) {                                                   \
        WRITE(out, *(buf_pt - 1), state);                                      \
        buf_pt--;                                                              \
      }                                                                        \
      while (siz < space) {                                                    \
        WRITE(out, ' ', state);                                                \
        space--;                                                               \
      }                                                                        \
    } else {                                                                   \
      while (siz < space) {                                                    \
        WRITE(out, ' ', state);                                                \
        space--;                                                               \
      }                                                                        \
      while (buf_pt > buf) {                                                   \
        WRITE(out, *(buf_pt - 1), state);                                      \
        buf_pt--;                                                              \
      }                                                                        \
    }                                                                          \
  } while (0)

#define PROCESS_UNSIGNED(out, ap, state)                                       \
  do {                                                                         \
    assert(is_show_sign != 1);                                                 \
    assert(!(right_or_left && space_or_zero));                                 \
    unsigned TYPE2 x = va_arg(ap, unsigned TYPE2);                             \
    unsigned TYPE2 y;                                                          \
    int space = is_precision ? precision : (is_filed_width ? field_width : 1); \
    do {                                                                       \
      y = x % base;                                                            \
      WRITE(buf_pt, ITOA(y, is_capital), 0);                                   \
      assert(buf_pt - buf < BUF_SIZE);                                         \
      x /= base;                                                               \
    } while (x != 0);                                                          \
    if (is_formed && base == 8)                                                \
      space--;                                                                 \
    if (is_formed && base == 16)                                               \
      space -= 2;                                                              \
    if (space_or_zero) {                                                       \
      while (buf_pt - buf < space)                                             \
        WRITE(buf_pt, '0', 0);                                                 \
    }                                                                          \
    if (is_formed && base == 8)                                                \
      WRITE(buf_pt, '0', 0);                                                   \
    if (is_formed && base == 16) {                                             \
      WRITE(buf_pt, 'x', 0);                                                   \
      WRITE(buf_pt, '0', 0);                                                   \
    }                                                                          \
    int siz = buf_pt - buf;                                                    \
    if (right_or_left) {                                                       \
      while (buf_pt > buf) {                                                   \
        WRITE(out, *(buf_pt - 1), state);                                      \
        buf_pt--;                                                              \
      }                                                                        \
      while (siz < space) {                                                    \
        WRITE(out, ' ', state);                                                \
        space--;                                                               \
      }                                                                        \
    } else {                                                                   \
      while (siz < space) {                                                    \
        WRITE(out, ' ', state);                                                \
        space--;                                                               \
      }                                                                        \
      while (buf_pt > buf) {                                                   \
        WRITE(out, *(buf_pt - 1), state);                                      \
        buf_pt--;                                                              \
      }                                                                        \
    }                                                                          \
  } while (0)

#define PROCESS(out, fmt, ap, state)                                           \
  do {                                                                         \
    switch (*fmt) {                                                            \
    case 'c':                                                                  \
      PROCESS_CHAR(out, ap, state);                                            \
      fmt++;                                                                   \
      break;                                                                   \
    case 's':                                                                  \
      PROCESS_STRING(out, ap, state);                                          \
      fmt++;                                                                   \
      break;                                                                   \
    case 'd':                                                                  \
      PROCESS_SIGNED(out, ap, state);                                          \
      fmt++;                                                                   \
      break;                                                                   \
    case 'i':                                                                  \
      PROCESS_SIGNED(out, ap, state);                                          \
      fmt++;                                                                   \
      break;                                                                   \
    case 'u':                                                                  \
      PROCESS_UNSIGNED(out, ap, state);                                        \
      fmt++;                                                                   \
      break;                                                                   \
    case 'o':                                                                  \
      base = 8;                                                                \
      PROCESS_UNSIGNED(out, ap, state);                                        \
      fmt++;                                                                   \
      break;                                                                   \
    case 'x':                                                                  \
      base = 16;                                                               \
      PROCESS_UNSIGNED(out, ap, state);                                        \
      fmt++;                                                                   \
      break;                                                                   \
    case 'X':                                                                  \
      base = 16;                                                               \
      is_capital = 1;                                                          \
      PROCESS_UNSIGNED(out, ap, state);                                        \
      fmt++;                                                                   \
      break;                                                                   \
    case 'p':                                                                  \
      is_formed = 1;                                                           \
      base = 16;                                                               \
      PROCESS_UNSIGNED(out, ap, state);                                        \
      fmt++;                                                                   \
      break;                                                                   \
    case '%':                                                                  \
      WRITE(out, '%', state);                                                  \
      fmt++;                                                                   \
      break;                                                                   \
    default:                                                                   \
      putch(*fmt);                                                             \
      putch('\n');                                                             \
    }                                                                          \
  } while (0)

static void init_flag() {
  buf_pt = buf;
  long_number = 0;
  is_filed_width = 0;
  is_precision = 0;
  is_capital = 0;
  is_formed = 0;
  is_show_sign = 0;
  space_or_zero = 0;
  right_or_left = 0;
  base = 10;
  precision = 0;
  field_width = 0;
}

// d,i,o,u,x,X,c,s,p
int printf(const char *fmt, ...) {
  int ret = 0;
  va_list ap;
  char *out = NULL;
  va_start(ap, fmt);
  while (*fmt != '\0') {
    init_flag();
    switch (*fmt) {
    case '%':
      PARSE_ARGS(fmt, ap);
      PROCESS(out, fmt, ap, 1);
      break;
    default:
      WRITE(out, *(fmt++), 1);
      break;
    }
  }
  WRITE(out, *fmt, 1);
  va_end(ap);
  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, -1, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  char *st = out;
  int ret = 0;
  while (*fmt != '\0') {
    init_flag();
    switch (*fmt) {
    case '%':
      PARSE_ARGS(fmt, ap);
      PROCESS(out, fmt, ap, 0);
      break;
    default:
      WRITE(out, *(fmt++), 0);
      break;
    }
    CHEKE_BOUND(st, out, n);
  }
  WRITE(out, *fmt, 0);
  return ret;
}

#endif
