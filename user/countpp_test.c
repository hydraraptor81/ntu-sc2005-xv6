#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pp_count;

  pp_count = countpp();

  printf("Current process physical page count: %d\n", pp_count);

  exit(0);
}
