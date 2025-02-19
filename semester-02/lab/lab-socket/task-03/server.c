#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>

#define SOCKET_PATH "server.sc"
#define BACKLOG 5

int flag = 1;
int server_sock_fd;

void handler(int s_num);

int main() {
    signal(SIGALRM, handler);
    alarm(20);
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    unlink(SOCKET_PATH);
    if ((server_sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    if (bind(server_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_sock_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_sock_fd, BACKLOG) == -1) {
        perror("listen");
        close(server_sock_fd);
        exit(EXIT_FAILURE);
    }
    printf("server started and waiting for clients...\n");  
    while (flag) {
        int client_sock_fd = accept(server_sock_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock_fd == -1) {
            perror("accept");
            continue;
        } 
        double num1, num2, result;
        pid_t client_pid;
        char op;
        if (recv(client_sock_fd, &num1, sizeof(num1), 0) <= 0 ||
            recv(client_sock_fd, &num2, sizeof(num2), 0) <= 0 ||
            recv(client_sock_fd, &op, sizeof(op), 0) <= 0 || 
            recv(client_sock_fd, &client_pid, sizeof(client_pid), 0) <= 0) {
            perror("recv");
            close(client_sock_fd);
            continue;
        }
        printf("server: сlient (%d]) connected, processing request...\n", client_pid); 
        printf("server (%d): %.2lf %c %.2lf = ?\n", client_pid, num1, op, num2);
        if (op == '+') {
            result = num1 + num2;
        } else if (op == '-') {
            result = num1 - num2;
        } else if (op == '*') {
            result = num1 * num2;
        } else if (op == '/' && num2 != 0.0) { // Доабвить сообщение error
            result = num1 / num2;
        } else {
            result = 0.0;
        }
        printf("server (%d): %.2lf %c %.2lf = %.2lf\n", client_pid, num1, op, num2, result);
        if (send(client_sock_fd, &result, sizeof(result), 0) == -1) {
            perror("send");
        }
        close(client_sock_fd);
    }
    if (flag)
        close(server_sock_fd);
    alarm(0);
    printf("server shutting down...\n");
    exit(EXIT_SUCCESS);
}

void handler(int s_num) {
    printf("server - signal resived %d\n", s_num);
    flag = 0;
    if (flag)
        close(server_sock_fd);
}
