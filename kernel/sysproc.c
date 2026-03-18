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
  printf("sys_fork: called from pid %d\n", myproc()->pid);
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

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
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
sys_ps(void)
{
  struct proc *p;
  struct procinfo info[NPROC];
  uint64 addr;
  int max, n = 0;

  argaddr(0, &addr);
  argint(1, &max);

  if(max > NPROC)
    max = NPROC;

  for(p = proc; p < &proc[NPROC] && n < max; p++){
    acquire(&p->lock);
    if(p->state != UNUSED){
      info[n].pid = p->pid;
      info[n].state = p->state;
      info[n].ppid = (p->parent) ? p->parent->pid : -1;
      safestrcpy(info[n].name, p->name, sizeof(info[n].name));
      n++;
    }
    release(&p->lock);
  }

  if(copyout(myproc()->pagetable, addr, (char*)info, n*sizeof(struct procinfo)) < 0)
    return -1;

  return n;
}
