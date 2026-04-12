#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int parent_pid = getpid();
  int child_pid;

  printf("prioritydemo: parent pid=%d priority=%d\n",
         parent_pid, getpriority(parent_pid));

  child_pid = forkprio(30);
  if(child_pid < 0){
    printf("prioritydemo: forkprio failed\n");
    exit(1);
  }

  if(child_pid == 0){
    int mypid = getpid();
    printf("prioritydemo: child pid=%d initial priority=%d\n",
           mypid, getpriority(mypid));
    if(setpriority(mypid, 80) < 0){
      printf("prioritydemo: setpriority failed\n");
      exit(1);
    }
    printf("prioritydemo: child pid=%d updated priority=%d\n",
           mypid, getpriority(mypid));
    exit(0);
  }

  pause(2);
  printf("prioritydemo: parent observes child pid=%d priority=%d\n",
         child_pid, getpriority(child_pid));
  wait(0);
  exit(0);
}
