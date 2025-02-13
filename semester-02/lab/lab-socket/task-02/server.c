#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
// #include <sys/error.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define SOCK_NAME "./server.soc"
#define BUFF_SIZE 1024

int flag = 1;

void handler(int s_num) {
    printf("signal resived %d\n", s_num);
    flag = 0;
}

int main() {
    int sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    uint addrlen;
    struct sockaddr addr, clnt_addr;
    addr.sa_family = AF_UNIX;
    strncpy(addr.sa_data, SOCK_NAME, strlen(SOCK_NAME));
    if (bind(sock_fd, &addr, sizeof(addr)) < 0) {
        perror("can't bind");
        exit(1);
    }
    char buff[BUFF_SIZE];
    signal(SIGALRM, handler);
    alarm(10);
    while (flag) {
        int bytes = recvfrom(sock_fd, buff, BUFF_SIZE, 0, &clnt_addr, &addrlen);
        if (bytes < 0) {
            perror("recvfrom error");
            exit(1);
        }
        buff[bytes] = '\0';
        printf("read message: %s\n", buff);
    }
    alarm(0);
    unlink(SOCK_NAME);

    exit(0);
}