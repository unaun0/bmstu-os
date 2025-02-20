#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#define CHILD_COUNT 5

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345

#define SHM_BUFFER_SIZE 26
#define STR_BUFFER_SIZE 255

#define READER 'r'
#define WRITER 'w'

int loop_flag = 1;

void handler(int s_num);

char ops[] = { READER, WRITER };

int find_first_availiable_index(const char *buffer) {
    char *pos = strpbrk(buffer, "abcdefghijklmnopqrstuvwxyz");
    if (pos)
        return (int)(pos - buffer);
    return -1;
}
char first_availiable_idx = 0;

void client_process() {
    pid_t pid = getpid();
    srand(pid);
    int sock_fd;
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr); 
    while (loop_flag) { 
        sleep(rand() % 6 + 3);
        if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }
        if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            perror("connect");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        char op = ops[rand() % 2];
        if (send(sock_fd, &op, sizeof(op), 0) == -1) {
            perror("send operation");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        if (op == 'r') {
            char buffer[SHM_BUFFER_SIZE + 1];
            memset(buffer, '\0', sizeof(char) * (SHM_BUFFER_SIZE + 1));
            if (recv(sock_fd, buffer, sizeof(char) * (SHM_BUFFER_SIZE + 1), 0) == -1) {
                perror("recv buf");
                close(sock_fd);
                exit(EXIT_FAILURE);
            }
            buffer[SHM_BUFFER_SIZE] = '\0';
            first_availiable_idx = (char)find_first_availiable_index(buffer);
            printf("CLIENT [PID %d]: buff: %s\n", pid, buffer);

        }
        else if (op == 'w') {
            if (send(sock_fd, &first_availiable_idx, sizeof(first_availiable_idx), 0) == -1) {
                perror("send idx");
                close(sock_fd);
                exit(EXIT_FAILURE);
            }
            sleep(rand() % 6 + 1);
            char result_buffer[STR_BUFFER_SIZE];
            if (recv(sock_fd, result_buffer, STR_BUFFER_SIZE, 0) == -1) {
                perror("recv buf");
                close(sock_fd);
                exit(EXIT_FAILURE);
            }
            printf("CLIENT [PID %d]: %s (idx: %d)\n", pid, result_buffer, (int)first_availiable_idx);
            close(sock_fd);
        }
    }
    exit(EXIT_SUCCESS);
}

int main() {
    srand(time(NULL)); 
    signal(SIGINT, handler); 
    pid_t child_pid[CHILD_COUNT];
    for (int i = 0; i < CHILD_COUNT; i++) {
        if ((child_pid[i] = fork()) == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (child_pid[i] == 0) {
            sleep(rand() % 6 + 1);
            client_process();
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

void handler(int s_num) {
    loop_flag = 0;
}
