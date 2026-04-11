#include "kernel/types.h"
#include "user/user.h"

int main() {
    int pid = fork();

    if (pid < 0) {
        printf("Fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // Child Process
        char buf[64];
        printf("Child (PID %d) waiting for message...\n", getpid());
        if (msgrecv(buf) == 0) {
            printf("Child received: %s\n", buf);
        }
        exit(0);
    } else {
        // Parent Process
        sleep(10); // Give child time to start waiting
        printf("Parent (PID %d) sending message to Child (PID %d)...\n", getpid(), pid);
        if (msgsend(pid, "Hello from Parent!") == 0) {
            printf("Parent: Message sent successfully!\n");
        }
        wait(0);
        exit(0);
    }
}