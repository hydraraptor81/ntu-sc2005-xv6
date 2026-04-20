#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;
  struct proc *p = myproc();

  // get requested growth size from user registers
  argint(0, &n);

  // save current process size
  addr = p->sz;

  // lazy allocation, increase virtual limit without allocating physical RAM
  if(n > 0){
    // ensure allocation does not exceed max virtual address
    if(addr + n >= MAXVA)
      return -1;
    // call helper function to mark page table entries with PTE_LAZY flag
    if(uvmlazy(p->pagetable, addr, addr + n) < 0)
      return -1;
    p->sz += n;
  }
  // if n is negative, shrink heap, free memory immediately
  else if(n < 0){
    p->sz = addr;
    if(growproc(n) < 0)
      return -1;
  }
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_countvp(void)
{
  struct proc *p = myproc();
  // Process size p->sz tracks total bytes of virtual memory allocated
  // to user program (text + data + guard page + stack + heap)
  // round up to nearest page boundary and divide by page size
  uint64 num_virtual_pages = PGROUNDUP(p->sz) / PGSIZE;

  return num_virtual_pages;
}

uint64
sys_countpp(void)
{
  struct proc *p = myproc(); // ptr to process control block proc struct
  uint64 physical_page_count = 0;
  uint64 va;
  pte_t *pte;

  // iterate through virtual address space for one 4K page at a time
  for(va = 0; va < p->sz; va += PGSIZE){
    // walk the page table to find the page table entry for this va,
    // 0 argument means, do not create a page table entry if it does not exist
    pte = walk(p->pagetable, va, 0);

    // check if pte exist and whether the PTE_V flag is set
    if(pte != 0 && (*pte * PTE_V) != 0){
      physical_page_count++;
    }
  }

  return physical_page_count;

}
