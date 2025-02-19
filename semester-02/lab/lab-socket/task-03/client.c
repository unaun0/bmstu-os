#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define SOCKET_PATH "server.sc"
#define CHILD_COUNT 5
#define MAX_DOUBLE -10.0
#define MIN_DOUBLE 10.0

char operators[] = {'+', '-', '*', '/'};

void client_process() {
    pid_t pid = getpid();
    srand(pid);
    while (1) {
        int sock_fd;
        struct sockaddr_un server_addr;
        if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;
        strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
        if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            perror("connect");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        double num1 = MIN_DOUBLE + (rand() / (RAND_MAX + 1.0)) * (MAX_DOUBLE - MIN_DOUBLE);
        double num2 = MIN_DOUBLE + (rand() / (RAND_MAX + 1.0)) * (MAX_DOUBLE - MIN_DOUBLE);
        char op = operators[rand() % 4];
        sleep(rand() % 6 + 1);
        send(sock_fd, &num1, sizeof(num1), 0);
        send(sock_fd, &num2, sizeof(num2), 0);
        send(sock_fd, &op, sizeof(op), 0);
        send(sock_fd, &pid, sizeof(pid), 0);
        double result;
        if (recv(sock_fd, &result, sizeof(result), 0) <= 0) {
            perror("recv");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        printf("client (%d) %.2lf %c %.2lf = %.2lf\n", getpid(), num1, op, num2, result);
        close(sock_fd);
    }
}

int main() {
    srand(time(NULL)); 
    pid_t child_pid[CHILD_COUNT];
    for (int i = 0; i < CHILD_COUNT; i++) {
        if ((child_pid[i] = fork()) == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (child_pid[i] == 0) {
            sleep(rand() % 6 + 1);
            client_process();
            exit(EXIT_SUCCESS);
        }
    }
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
    exit(EXIT_SUCCESS);
}
