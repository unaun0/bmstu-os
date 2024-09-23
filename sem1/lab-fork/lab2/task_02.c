#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define SLEEP_FOR 10

int main(void) {
    pid_t cpid[2];
    int status;
    pid_t w;

    printf("PARENT - pid: %d, pgrpid: %d, ppid: %d\n", getpid(), getpgrp(), getppid());

    for (size_t i = 0; i < 2; ++i) {
        cpid[i] = fork();
        if (cpid[i] == -1) {
            perror("can't fork\n");
            exit(1);
        } else if (cpid[i] == 0) {
            printf("1 CHILD - pid: %d, pgrp: %d, ppid: %d\n", getpid(), getpgrp(), getppid());
            //sleep(SLEEP_FOR);
            if (i == 0)
                pause();
            exit(0);
        } else {
            printf("2 PARENT pid: %d, CHILD pid: %d, GROUP: %d\n", getpid(), cpid[i], getpgrp());
        }
    }
    printf("\n");

    for (size_t i = 0; i < 2; ++i) {
        w = waitpid(cpid[i], &status, WUNTRACED | WCONTINUED);
        if (WIFEXITED(status)) {
            printf("WAIT - pid: %d, exited with code %d\n", w, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("WAIT - pid: %d, exited with code %d\n", w, WEXITSTATUS(status));
        } else if (WIFSTOPPED(status)){
            printf("WAIT - pid: %d, exited with code %d\n", w, WEXITSTATUS(status));
        }
    }
   
    exit(0);
}
