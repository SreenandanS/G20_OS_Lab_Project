#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NPROC 64

struct pinfo {
  int pid[NPROC];
  int state[NPROC];
  int runtime[NPROC];
  int waittime[NPROC];
};

int main()
{
    struct pinfo info;

    if(getpinfo(&info) < 0){
        printf("FAILED\n");
        exit(1);
    }

    printf("PID\tSTATE\tRTIME\tWTIME\n");

    for(int i = 0; i < NPROC; i++){
        if(info.pid[i] > 0){
            printf("%d\t%d\t%d\t%d\n",
                info.pid[i],
                info.state[i],
                info.runtime[i],
                info.waittime[i]);
        }
    }

    exit(0);
}