#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define SLEEP_FOR 10

int main(void) {
    pid_t cpid[2];
    int child;
    int status;

    printf("PARENT - pid: %d, gpid: %d, ppid: %d\n", getpid(), getpgrp(), getppid());

    for (size_t i = 0; i < 2; ++i) {
        cpid[i] = fork();
        if (cpid[i] == -1) {
            perror("can't fork\n");
            exit(1);
        } else if (cpid[i] == 0) {
            printf("CHILD - pid: %d, gpid: %d, ppid: %d\n", getpid(), getpgrp(), getppid());
            //sleep(SLEEP_FOR);
            if (i == 0)
                pause();
                
            exit(0);
        } else {
            printf("PARENT pid: %d, CHILD pid: %d, GROUP pid: %d\n", getpid(), cpid[i], getpgrp());
        }
    }
    printf("\n");

    for (int i = 0; i < 2; i++) {
        child = wait(&status);
        printf("pid: %d, ppid: %d, status: %d, group %d\n", child, getpid(), status, getpgrp());
        if (WIFEXITED(status)) {
            printf("exited with code %d, pid: %d\n", WEXITSTATUS(status), child);
        } else if (WIFSIGNALED(status)) {
            printf("terminated with un-intercepted signal number %d, pid: %d\n", WTERMSIG(status), child);
        } else if (WIFSTOPPED(status)) {
            printf("stopped with signal number %d, pid: %d\n", WSTOPSIG(status), child);
        }
    }
   
    exit(0);
}
