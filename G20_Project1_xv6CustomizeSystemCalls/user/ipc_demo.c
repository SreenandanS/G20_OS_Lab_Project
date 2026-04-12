#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int pid = fork();

  if(pid < 0){
    printf("ipc_demo: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    char buf[64];

    printf("ipc_demo: child pid=%d waiting for mailbox message\n", getpid());
    if(msgrecv(buf) < 0){
      printf("ipc_demo: child receive failed\n");
      exit(1);
    }
    printf("ipc_demo: child received \"%s\"\n", buf);
    exit(0);
  }

  pause(10);
  if(msgsend(pid, "hello-from-parent") < 0){
    printf("ipc_demo: parent send failed\n");
    exit(1);
  }
  wait(0);
  printf("ipc_demo: parent delivered message to pid=%d\n", pid);
  exit(0);
}
