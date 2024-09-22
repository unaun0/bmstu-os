#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t childpid;

    if ((childpid = fork()) == -1) {
        perror("fork() failed");
        return 1;
    } else if (childpid == 0) {
        while (1) {
            printf(" %d", getpid());
        }
    } else {
        while(1) {
            printf(" %d", getpid());
        }
    }
    return 0;
}