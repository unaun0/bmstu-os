#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define CHILD_COUNT 3

char *messages[] = {
    "aaaa", 
    "bb",
    "ccccc", 
    "dddddddd",
    "eee", 
    "ffffffffffff"
};

int main(void) {
    char buf[64];
    pid_t child_pid[4];
    int sockfd[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockfd) == -1) {
        perror("socketpair error");
        exit(1);
    }
    for (size_t i = 0; i < CHILD_COUNT; i++) {
        if ((child_pid[i] = fork()) == -1) {
            perror("сan't fork\n");
            exit(1);
        }
        if (child_pid[i] == 0) {
            close(sockfd[1]);
            write(sockfd[0], messages[i + 3], strlen(messages[i + 3]) + 1);
            printf("child send - %s\n", messages[i + 3]);
            sleep(1);
            read(sockfd[0], buf, strlen(messages[i]) + 1);
            printf("child received - %s\n", buf);
            close(sockfd[0]);
            exit(0);
        }
        sleep(1);
        read(sockfd[1], buf, strlen(messages[i + 3]) + 1);
        printf("parent receive - %s\n", buf);
        write(sockfd[1], messages[i], strlen(messages[i]) + 1);
        printf("parent send - %s\n", messages[i]);
    }
    close(sockfd[0]);
    close(sockfd[1]);
    for (size_t i = 0; i < CHILD_COUNT; i++) {
        int status;
        waitpid(child_pid[i], &status, 0);
        if (WIFEXITED(status)) {
            printf("%d exited with code %d\n",
                   child_pid[i], WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("%d received signal %d\n",
                   child_pid[i], WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("%d received signal %d\n",
                   child_pid[i], WSTOPSIG(status));
        }
    }
    exit(0);
}