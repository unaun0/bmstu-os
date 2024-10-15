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

    //printf("PARENT - pid: %d, gpid: %d, ppid: %d\n", getpid(), getpgrp(), getppid());

    for (int i = 0; i < 2; i++) {
        cpid[i] = fork();
        if (cpid[i] == -1) {
            perror("can't fork\n");
            exit(1);
        }
        if (cpid[i] == 0) {
            //printf("(pid = %d) executed %s\n", getpid(), exec_params[i]);
            rc = execl(exec_params[i], NULL);
            if (rc == -1) {
                perror("cant exec\n");
                exit(1);
            }
            exit(0);
        }
        else {
            printf("pid: %d, child pid: %d, group pid: %d\n", getpid(), cpid[i], getpgrp());
        }
    }
    for (int i = 0; i < 2; i++) {
        child = wait(&status);
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