#include "types.h"
#include "stat.h"
#include "user.h"

volatile int alarm_hits = 0;
volatile int user_hits = 0;

void
handle_signal(int signum)
{
  if(signum == SIGALRM){
    alarm_hits++;
    printf(1, "signaldemo: alarm hit %d\n", alarm_hits);
  } else {
    user_hits++;
    printf(1, "signaldemo: received signal %d\n", signum);
  }

  if(alarm_hits >= 2 && user_hits >= 1){
    printf(1, "signaldemo: handled both periodic and user signals\n");
    exit();
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
    printf(1, "signaldemo: fork failed\n");
    exit();
  }

  if(pid == 0){
    if(sigalarm(5, handle_signal) < 0){
      printf(1, "signaldemo: sigalarm registration failed\n");
      exit();
    }

    while(1){
      spin++;
      if((spin & 0x3fffff) == 0)
        write(1, ".", 1);
    }
  }

  sleep(20);
  if(sigsend(pid, SIGUSR1) < 0)
    printf(1, "signaldemo: sigsend failed\n");
  wait();
  printf(1, "signaldemo: parent observed child exit\n");
  exit();
}
