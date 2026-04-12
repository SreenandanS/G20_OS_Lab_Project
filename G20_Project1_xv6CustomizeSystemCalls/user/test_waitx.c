#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int rtime;
  int wtime;
  int pid = fork();

  if(pid < 0){
    printf("test_waitx: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    for(int i = 0; i < 100000000; i++)
      ;
    exit(0);
  }

  if(waitx(&rtime, &wtime) < 0){
    printf("test_waitx: waitx failed\n");
    exit(1);
  }

  printf("test_waitx: runtime=%d waittime=%d\n", rtime, wtime);
  exit(0);
}
