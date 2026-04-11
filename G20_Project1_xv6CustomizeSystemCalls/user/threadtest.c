// threadtest.c — demo for clone() and join() system calls
//
// This program creates two lightweight threads that share the same
// address space as the parent.  Each thread increments a shared counter
// and writes a message to stdout, demonstrating that:
//
//   1. clone() gives the new thread its own kernel control block (PCB)
//      and its own stack, but the same user pagetable as the parent.
//   2. join() blocks the parent until a specific thread exits (by tid).
//   3. Mutations made by the thread to global memory are visible to the
//      parent because they share address space (no copy-on-write).
//
// Build: add $U/_threadtest to UPROGS in Makefile (done separately).

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Shared counter – both threads increment this.  Since xv6 has no
// atomic primitives in user space we keep things simple: each thread
// writes its own slot and the parent reads both after join().
static volatile int results[2];

// Stack storage for the two threads.
// RISC-V stacks grow downward; we pass the *top* of the page to clone().
#define STACK_SIZE 4096
static char stack0[STACK_SIZE];
static char stack1[STACK_SIZE];

// Thread body – receives its slot index as the argument.
static void
thread_fn(void *arg)
{
  int slot = (int)(uint64)arg;

  printf("thread[%d]: started (tid=%d), writing result\n", slot, getpid());
  results[slot] = (slot + 1) * 100;   // write something recognisable
  printf("thread[%d]: results[%d] = %d, exiting\n", slot, slot, results[slot]);

  // Threads must call exit() – they must NOT return (there is no return
  // address on the stack above thread_fn in our simple setup).
  exit(0);
}

int
main(void)
{
  printf("threadtest: parent pid=%d\n", getpid());

  // Launch thread 0
  // Pass top-of-stack (stack grows down on RISC-V).
  int tid0 = clone(thread_fn, stack0 + STACK_SIZE, (void *)0);
  if(tid0 < 0){
    printf("threadtest: clone 0 failed\n");
    exit(1);
  }
  printf("threadtest: started thread 0 with tid=%d\n", tid0);

  // Launch thread 1
  int tid1 = clone(thread_fn, stack1 + STACK_SIZE, (void *)1);
  if(tid1 < 0){
    printf("threadtest: clone 1 failed\n");
    // Still need to wait for tid0 so the parent doesn't vanish first.
    join(tid0);
    exit(1);
  }
  printf("threadtest: started thread 1 with tid=%d\n", tid1);

  // Wait for both threads to finish.
  int r0 = join(tid0);
  int r1 = join(tid1);

  printf("threadtest: joined tid0=%d (ret=%d) tid1=%d (ret=%d)\n",
         tid0, r0, tid1, r1);
  printf("threadtest: results[0]=%d results[1]=%d\n",
         results[0], results[1]);

  // Verify both threads wrote correctly.
  if(results[0] == 100 && results[1] == 200){
    printf("threadtest: PASS - shared memory updated by threads\n");
  } else {
    printf("threadtest: FAIL - unexpected values\n");
    exit(1);
  }

  exit(0);
}
