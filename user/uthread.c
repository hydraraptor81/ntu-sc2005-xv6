#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/* Possible states of a thread: */
#define FREE        0x0
#define RUNNING     0x1
#define RUNNABLE    0x2

#define STACK_SIZE  8192
#define MAX_THREAD  4


struct context {
  uint64 ra;      // Return address (x1)
  uint64 sp;      // Stack pointer (x2)
  uint64 s0;      // Saved registers / Frame pointer (x8)
  uint64 s1;      // (x9)
  uint64 s2;      // (x18)
  uint64 s3;      // (x19)
  uint64 s4;      // (x20)
  uint64 s5;      // (x21)
  uint64 s6;      // (x22)
  uint64 s7;      // (x23)
  uint64 s8;      // (x24)
  uint64 s9;      // (x25)
  uint64 s10;     // (x26)
  uint64 s11;     // (x27)
};

struct thread {
  char       stack[STACK_SIZE]; /* the thread's stack */
  int        state;             /* FREE, RUNNING, RUNNABLE */
  // TODO: include context of thread
  struct context context;
};
struct thread all_thread[MAX_THREAD];
struct thread *current_thread;
extern void thread_switch(struct context *old, struct context *new);

void
thread_init(void)
{
  // main() is thread 0, which will make the first invocation to
  // thread_schedule(). It needs a stack so that the first thread_switch() can
  // save thread 0's state.
  current_thread = &all_thread[0];
  current_thread->state = RUNNING;
}

void
thread_schedule(void)
{
  struct thread *t, *next_thread;

  /* Find another runnable thread. */
  next_thread = 0;
  t = current_thread + 1;
  for(int i = 0; i < MAX_THREAD; i++){
    if(t >= all_thread + MAX_THREAD)
      t = all_thread;
    if(t->state == RUNNABLE) {
      next_thread = t;
      break;
    }
    t = t + 1;
  }

  if (next_thread == 0) {
    printf("thread_schedule: no runnable threads\n");
    exit(-1);
  }

  if (current_thread != next_thread) {         /* switch threads?  */
    next_thread->state = RUNNING;
    t = current_thread;
    current_thread = next_thread;
    // TODO: invoke thread_switch to switch from t to next_thread:
    // thread_switch(??, ??);
    thread_switch(&t->context, &current_thread->context);
  } else
    next_thread = 0;
}

void
thread_create(void (*func)())
{
  struct thread *t;

  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == FREE) break;
  }
  if (t >= all_thread + MAX_THREAD) {
    printf("thread_create: no free threads\n");
    exit(-1);
  }

  t->state = RUNNABLE;

  // TODO: ensure `func` will be executed on its own stack
  // ...
  // Initialize context: zero out all saved registers
  memset(&t->context, 0, sizeof(t->context));

  // Set RA to the thread function so thread_switch will "return" to it
  t->context.ra = (uint64)func;

  // Set SP to the top of the stack (highest address)
  t->context.sp = (uint64)(t->stack + STACK_SIZE);
}

void
thread_yield(void)
{
  current_thread->state = RUNNABLE;
  thread_schedule();
}

void thread_exit(void) {
  current_thread->state = FREE;
  thread_schedule();
}
