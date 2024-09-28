#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define SLEEP_FOR 5

int main(void) {
    pid_t cpid[2];

    printf("PARENT - pid: %d, ppid: %d, gpid: %d\n", getpid(), getppid(), getpgrp());

    for (size_t i = 0; i < 2; ++i) {
        cpid[i] = fork();
        if (cpid[i] == -1) {
            perror("can't fork\n");
            exit(1);
        } else if (cpid[i] == 0) {
            printf("CHILD - pid: %d, ppid: %d, gpid: %d\n", getpid(), getppid(), getpgrp());
            sleep(SLEEP_FOR);
            printf("CHILD - pid: %d, ppid: %d, gpid: %d\n", getpid(), getppid(), getpgrp());
            exit(0);
        } else {
            printf("PARENT pid: %d, cpid: %d, gpid: %d\n", getpid(), cpid[1], getpgrp());
        }
    }
    exit(0);
}
