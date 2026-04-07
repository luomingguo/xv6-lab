#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"
// #include "kernel/defs.h"

#define SIZE (8*4096)

int
main(int argc, char *argv[])
{
  // Your code here.
  char *p = sbrk(SIZE);

  char *pattern = "This may help.";

  for (int i = 0; i < SIZE - 32; i++) {
    // 找前缀
    if (strcmp(p + i, pattern) == 0) {
      // secret 在 offset +16
      char *secret = p + i + 16;

      printf("%s\n", secret);
    }
  }
  exit(0);
}
