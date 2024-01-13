#include <SDL.h>
#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>

char handle_key(SDL_Event *ev);
static void sh_printf(const char *format, ...);

void cmd_q(int status) { exit(status); }

void cmd_r(const char *buf) {
  char filename[20];
  char *argv[10];
  int argc = 1;
  int i = 0;
  while (*buf == ' ')
    buf++;
  while (*buf != '\0' && *buf != ' ')
    filename[i++] = *(buf++);
  filename[i] = '\0';
  while (*buf != '\0') {
    while (*buf == ' ') {
      buf++;
    }
    if (*buf == '\0') {
      break;
    }
    argv[argc++] = (char *)buf;
    while (*buf != '\0' && *buf != ' ')
      buf++;
  }
  printf("%s\n", filename);
  argv[0] = filename;
  argv[argc] = NULL;
  for (int i=0 ; i<argc;i++) {
  printf("%s\n", argv[i]);
  }
  char *empty[] = {NULL};
  int ret = execvp(filename, argv);
  if (ret == -1)
    sh_printf("filename error!\n");
  else sh_printf("exec succeed!\n");
}

struct handler {
  char name[10];
  void *handler;
} cmd_handler[] = {{"q", (void *)cmd_q}, {"r", (void *)cmd_r}};

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() { sh_printf("sh> "); }

static void sh_handle_cmd(const char *cmd) {
  switch (cmd[0]) {
  default:
    cmd_r(cmd);
    break;
  }
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
