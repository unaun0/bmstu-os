#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <dispatch/dispatch.h>
#include <mach/mach_time.h>

#define CHILD_COUNT 10

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9877

#define SHM_BUFFER_SIZE 52
#define STR_BUFFER_SIZE 255

#define FILE_TIME "../analyze/fork_client.txt"

#define READER 'r'
#define WRITER 'w'

int loop_flag = 1;

void sig_handler(int s_num);

int find_first_unoccupied_index(const char *buffer) {
    char *pos = strpbrk(buffer, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (pos)
        return (int)(pos - buffer);
    return -1;
}
char first_unoccupied_idx = 0;

dispatch_semaphore_t file_sem;
FILE *log_file = NULL;

void client_process() {
    pid_t pid = getpid();
    srand(pid);
    int sock_fd;
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr); 
    uint64_t start, end;
    while (loop_flag) { 
        sleep(rand() % 2 + 1);
        if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }
        if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            perror("connect");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        start = mach_absolute_time();
        char op = READER;
        if (send(sock_fd, &op, sizeof(op), 0) == -1) {
            perror("send operation");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        char buffer[SHM_BUFFER_SIZE + 1];
        memset(buffer, '\0', sizeof(char) * (SHM_BUFFER_SIZE + 1));
        if (recv(sock_fd, buffer, sizeof(char) * (SHM_BUFFER_SIZE + 1), 0) == -1) {
            perror("recv buf");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        buffer[SHM_BUFFER_SIZE] = '\0';
        close(sock_fd);
        first_unoccupied_idx = (char)find_first_unoccupied_index(buffer);
        printf("CLIENT [PID %d]: buff: %s\n", pid, buffer);
        if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }
        if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            perror("connect");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        op = WRITER;
        if (send(sock_fd, &op, sizeof(op), 0) == -1) {
            perror("send operation");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        if (send(sock_fd, &first_unoccupied_idx, sizeof(first_unoccupied_idx), 0) == -1) {
            perror("send idx");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        // sleep(rand() % 3 + 1);
        char result_buffer[STR_BUFFER_SIZE];
        if (recv(sock_fd, result_buffer, STR_BUFFER_SIZE, 0) == -1) {
            perror("recv buf");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        end = mach_absolute_time() - start;
        printf("CLIENT [PID %d]: %s (idx: %d); time: %llu\n", pid, result_buffer, (int)first_unoccupied_idx, end);
        if ((first_unoccupied_idx >= 0) && (strlen(result_buffer) == 1)) {
            dispatch_semaphore_wait(file_sem, DISPATCH_TIME_FOREVER);
            fprintf(log_file, "%llu\n", (long long)end);
            dispatch_semaphore_signal(file_sem);
        }
    }
    exit(EXIT_SUCCESS);
}

int main() {
    srand(time(NULL)); 
    signal(SIGINT, sig_handler); 
    pid_t child_pid[CHILD_COUNT];
    file_sem = dispatch_semaphore_create(1);
    log_file = fopen(FILE_TIME, "w");
    if (!log_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < CHILD_COUNT; i++) {
        if ((child_pid[i] = fork()) == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (child_pid[i] == 0) {
            // sleep(rand() % 3 + 1);
            client_process();
        }
    }
    for (size_t i = 0; i < CHILD_COUNT; i++) {
        int status;
        waitpid(child_pid[i], &status, 0);
        /*if (WIFEXITED(status)) {
            printf("%d exited with code %d\n",
                   child_pid[i], WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("%d received signal %d\n",
                   child_pid[i], WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("%d received signal %d\n",
                   child_pid[i], WSTOPSIG(status));
        }*/
    }
    exit(EXIT_SUCCESS);
}

void sig_handler(int s_num) {
    printf("received signal %d\n", s_num);
    fclose(log_file);
    dispatch_release(file_sem);
    loop_flag = 0;
}
