#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

volatile int alarm_hits = 0;
volatile int user_hits = 0;

void
handle_signal(int signum)
{
  if(signum == SIGALRM){
    alarm_hits++;
    printf("signaldemo: alarm hit %d\n", alarm_hits);
  } else {
    user_hits++;
    printf("signaldemo: received signal %d\n", signum);
  }

  if(alarm_hits >= 2 && user_hits >= 1){
    printf("signaldemo: handled both periodic and user signals\n");
    exit(0);
  }
  sigreturn();
}

int
main(void)
{
  int pid;
  volatile int spin = 0;

  pid = fork();
  if(pid < 0){
    printf("signaldemo: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    if(sigalarm(5, handle_signal) < 0){
      printf("signaldemo: sigalarm registration failed\n");
      exit(1);
    }

    while(1){
      spin++;
      if((spin & 0x3fffff) == 0)
        write(1, ".", 1);
    }
  }

  pause(20);
  if(sigsend(pid, SIGUSR1) < 0)
    printf("signaldemo: sigsend failed\n");
  wait(0);
  printf("signaldemo: parent observed child exit\n");
  exit(0);
}
