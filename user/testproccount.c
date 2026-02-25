#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int count1, count2;

  count1 = getproccount();
  printf("Before fork: %d\n", count1);

  if(fork() == 0) {
    // Child
    count2 = getproccount();
    printf("In child: %d\n", count2);
    exit(0);
  }

  // Parent
  wait(0);
  count2 = getproccount();
  printf("After child exits: %d\n", count2);

  exit(0);
}
