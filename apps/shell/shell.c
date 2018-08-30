#include <lib/scanf.h>
#include <app/echo.h>
#include <mm.h>
#include <lib/hlt.h>

typedef struct cmd {
  char *cmd_name;
  int (*built_in_cmds)(int, char*);
  struct cmd *next;
} cmd_t;

cmd_t first_built_in_cmd;

cmd_t *get_end_built_in_cmd() {
  struct cmd *p = &first_built_in_cmd;
  while (p->next != NULL) {
    p = p->next;
  }
  return p;
}

void define_cmd(char *cmd_name, int (*built_in_cmd)(char*)) {
  cmd_t *p = get_end_built_in_cmd();

  cmd_t *c = kmalloc(sizeof(cmd_t));
  memset(c, NULL, sizeof(c));
  c->cmd_name = cmd_name;
  c->built_in_cmds = built_in_cmd;
  c->next = NULL;

  p->next = c;
}

int find_and_run_built_in_cmd(char *cmd_name, char *arg, int argc) {
  struct cmd *p = &first_built_in_cmd;

  while (p->next != NULL) {
    p = p->next;
    if (strcmp(p->cmd_name, cmd_name) == 0) {
      p->built_in_cmds(argc, arg);
      return 0;
    }
  }
  return -1;
}

void setup_built_in_cmds() {
  memset(first_built_in_cmd, NULL, sizeof(first_built_in_cmd));

  // TODO exitコマンド
  // TODO xコマンド
  // TODO xpコマンド
  define_cmd("echo", echo_main);
}

int split(char *str, char *ret[256]) {
  int i=0;
  ret[i] = kmalloc(sizeof(char) * 256);
  ret[i][0] = *str;
  while (*str++ != NULL) {
    if (*str == ' ') {
      *str++;
      if (strlen(ret[i]) != 0) i++;
      ret[i] = kmalloc(sizeof(char) * 256);
    }
    ret[i][strlen(ret[i])] = *str;
  }
  return i;
}

// TODO コンテキストスイッチさせる
// TODO ユーザーランドで動作させる
void shell_main() {
  char cmd[256];
  int ret;

  /* reigister built-in commands */
  setup_built_in_cmds();

  /* main loop */
  while (1) {
    memset(cmd, NULL, sizeof(cmd));

    /* prompt */
    printf("mutex-os > ");
    scanf(cmd);

    char *split_cmd[256];
    memset(split_cmd, NULL, sizeof(split_cmd));
    int argc = split(cmd, split_cmd);
    ret = find_and_run_built_in_cmd(split_cmd[0], split_cmd, argc);

    // TODO split_cmdをfreeする

    if (ret != 0) printf("command not found: %s\n", cmd);
  }
}
