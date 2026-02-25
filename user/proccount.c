#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int count;

  count = getproccount();
  printf("Active processes: %d\n", count);

  exit(0);
}

