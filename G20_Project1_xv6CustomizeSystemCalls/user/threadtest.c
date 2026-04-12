#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define STACK_SIZE 4096

static char stack0[STACK_SIZE];
static char stack1[STACK_SIZE];
static char name0[] = "alpha";
static char name1[] = "beta";

static void
thread_fn(void *arg)
{
  char *name = (char *)arg;

  printf("threadtest: worker %s started with pid=%d\n", name, getpid());
  for(int i = 0; i < 1000000; i++)
    ;
  printf("threadtest: worker %s finished\n", name);
  exit(0);
}

int
main(void)
{
  int tid0;
  int tid1;

  tid0 = clone(thread_fn, stack0 + STACK_SIZE, name0);
  if(tid0 < 0){
    printf("threadtest: first clone failed\n");
    exit(1);
  }
  if(join(tid0) < 0){
    printf("threadtest: first join failed\n");
    exit(1);
  }
  printf("threadtest: worker %s joined successfully\n", name0);

  tid1 = clone(thread_fn, stack1 + STACK_SIZE, name1);
  if(tid1 < 0){
    printf("threadtest: second clone failed\n");
    exit(1);
  }
  if(join(tid1) < 0){
    printf("threadtest: second join failed\n");
    exit(1);
  }

  printf("threadtest: worker %s joined successfully\n", name1);
  printf("threadtest: clone and join completed for both workers\n");
  exit(0);
}
