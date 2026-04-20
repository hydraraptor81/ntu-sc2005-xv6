#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pages_to_alloc = 30;

  // number of physical pages before
  int before = countpp();
  printf("Physical pages before sbrk() :%d\n", before);

  // Request for 30 pages of memory
  char *heap = sbrk(PGSIZE * pages_to_alloc);
  if(heap == (char*)-1){
    printf("sbrk failed\n");
    exit(1);
  }

  int between = countpp();
  printf("Physical pages after sbrk() before physically allocating: %d\n", between);

  if(between > before + 2){
    printf("Kernel is still using eager allocation\n");
  } else {
    printf("Kernel is using lazy allocation\n");
  }

  printf("\nTouching memory to force page faults\n");
  for(int i = 0; i < pages_to_alloc; i++){
    heap[i *PGSIZE] = '8';
  }

  int after = countpp();;
  printf("Physical pages after touching all memory :%d\n", after);

  if(after >= before + pages_to_alloc){
    printf("Physical pages increased by %d\n", after - before);
  } else {
    printf("Physical pages were not properly allocated on demand");
  }

  exit(0);

}
