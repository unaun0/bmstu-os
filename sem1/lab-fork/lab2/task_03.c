#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SLEEP_FOR 10

char *exec_params[2] = {"more/app1", "more/app2"};

int main(void) {
    pid_t cpid[2];
    int rc;
    int child;
    int status;

    printf("PARENT - pid: %d, gpid: %d, ppid: %d\n", getpid(), getpgrp(), getppid());

    for (int i = 0; i < 2; i++) {
        cpid[i] = fork();
        if (cpid[i] == -1) {
            perror("can't fork\n");
            exit(1);
        }
        if (cpid[i] == 0) {
            printf("CHILD - pid: %d, gpid: %d, ppid: %d\n", getpid(), getpgrp(), getppid());
            printf("CHILD (pid = %d) executed %s\n", getpid(), exec_params[i]);
            rc = execl(exec_params[i], NULL);
            if (rc == -1) {
                perror("cant exec\n");
                exit(1);
            }
            exit(0);
        }
        else {
            printf("PARENT pid: %d, CHILD pid: %d, GROUP pid: %d\n", getpid(), cpid[i], getpgrp());
        }
    }
    for (int i = 0; i < 2; i++) {
        child = wait(&status);
        printf("CHILD - pid: %d, ppid: %d, status: %d, group %d\n", child, getpid(), status, getpgrp());
        if (WIFEXITED(status)) {
            printf("CHILD exited with code %d, child pid: %d\n", WEXITSTATUS(status), child);
        } else if (WIFSIGNALED(status)) {
            printf("CHILD terminated with un-intercepted signal number %d, child pid: %d\n", WTERMSIG(status), child);
        } else if (WIFSTOPPED(status)) {
            printf("CHILD stopped with signal number %d, child pid: %d\n", WSTOPSIG(status), child);
        }
    }
    exit(0);
}