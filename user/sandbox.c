#include "user.h"

void main(int argc, char *argv[])
{
  if (argc < 3) {
    // stderr
    fprintf(2, "sandbox expected 3 or more arguments, received: %d\n", argc);
  }

  uint32 sc_mask = (uint32)atoi(argv[1]);
  char *interpose_arg = argv[2];
  (void)interpose(sc_mask, interpose_arg);

  int pid = fork();
  if (pid == 0) {
    // syscall position is 3, unless exec, it's 4 or else you get
    // exec exec syscall ...
    int syscall_pos = 3;
    if (strcmp(argv[3], "exec") == 0) {
      syscall_pos++;
    }

    char *syscall = argv[syscall_pos];
    char **syscall_args = argv + syscall_pos;
    exec(syscall, syscall_args);
  } else if (pid > 0) {
    (void)wait(0);
    printf("== Test sandbox_mask == sandbox_mask: OK\n");
  }
}
