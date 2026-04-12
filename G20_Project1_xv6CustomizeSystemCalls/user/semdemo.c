#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int pid;

  if(sem_init(0, 1) < 0 || sem_init(1, 0) < 0){
    printf("semdemo: semaphore initialization failed\n");
    exit(1);
  }

  pid = fork();
  if(pid < 0){
    printf("semdemo: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    for(int i = 0; i < 3; i++){
      sem_wait(1);
      printf("semdemo: child iteration %d\n", i);
      sem_post(0);
    }
    exit(0);
  }

  for(int i = 0; i < 3; i++){
    sem_wait(0);
    printf("semdemo: parent iteration %d\n", i);
    sem_post(1);
  }

  wait(0);
  printf("semdemo: synchronization sequence complete\n");
  exit(0);
}
