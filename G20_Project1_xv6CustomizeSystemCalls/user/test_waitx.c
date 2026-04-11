#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
  int rtime, wtime;

  int pid = fork();

  if(pid == 0){
    // Child process (CPU work)
    for(int i = 0; i < 100000000; i++);
    exit(0);
  } 
  else {
    // Parent process waits
    waitx(&rtime, &wtime);

    printf("Runtime: %d\n", rtime);
    printf("Waiting Time: %d\n", wtime);
  }

  exit(0);
}