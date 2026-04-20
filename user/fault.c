#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if(argc <2) {
    printf("Usage: fault <null|bounds>\n");
    exit(1);
  }

  if(strcmp(argv[1], "null") == 0){
    int *null_ptr = (int *)0;
    printf("Writing to null pointer\n");
    *null_ptr = 81;
  }
  else if(strcmp(argv[1], "bounds") == 0){
    // return current top of allocated heap
    char *heap_top = sbrk(0);
    // create a pointer exactly one page past the boundary
    char *out_of_bounds_ptr = heap_top + 4096;
    printf("Out of bounds write at %p\n", out_of_bounds_ptr);
    *out_of_bounds_ptr = 81;
  }
  else {
    printf("Invalid option");
  }
}
