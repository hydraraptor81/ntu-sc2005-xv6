#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int vp_count;

  vp_count = countvp();

  printf("Current process virtual page count: %d\n", vp_count);

  exit(0);
}
