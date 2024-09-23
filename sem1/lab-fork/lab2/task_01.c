#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define SLEEP_FOR 2

int main(void) {
    pid_t cpid[2];

    printf("PARENT - pid: %d, gpid: %d, ppid: %d\n", getpid(), getpgrp(), getppid());

    for (size_t i = 0; i < 2; ++i) {
        cpid[i] = fork();
        if (cpid[i] == -1) {
            perror("can't fork\n");
            exit(1);
        } else if (cpid[i] == 0) {
            printf("1 CHILD - pid: %d, gpid: %d, ppid: %d\n", getpid(), getpgrp(), getppid());
            sleep(SLEEP_FOR);
            printf("2 CHILD - pid: %d, gpid: %d, ppid: %d\n", getpid(), getpgrp(), getppid());
            exit(0);
        } else {
            printf("PARENT pid: %d, CHILD pid: %d, GROUP pid: %d\n", getpid(), cpid[i], getpgrp());
        }
    }
    exit(0);
}
