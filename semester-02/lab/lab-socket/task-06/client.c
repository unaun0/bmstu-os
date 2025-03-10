#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#define THREAD_COUNT 5
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9877
#define SHM_BUFFER_SIZE 52
#define STR_BUFFER_SIZE 255
#define READER 'r'
#define WRITER 'w'

int loop_flag = 1;

int find_first_unoccupied_index(const char *buffer) {
    char *pos = strpbrk(buffer, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (pos)
        return (int)(pos - buffer);
    return -1;
}
char first_unoccupied_idx = 0;

#define FILE_TIME "../analyze/select_client.txt"
FILE *log_file = NULL;
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

void *client_thread(void *arg) {
    srand(time(NULL));
    int sock_fd;
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    int op_idx = 0;
    while (loop_flag) 
    {
        // sleep(rand() % 3 + 1);
        if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            pthread_exit(NULL);
        }
        if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            perror("connect");
            close(sock_fd);
            pthread_exit(NULL);
        }
        clock_t start_time = clock();
        char op = READER;
        if (send(sock_fd, &op, sizeof(op), 0) == -1) {
            perror("send operation");
            close(sock_fd);
            pthread_exit(NULL);
        }
        char buffer[SHM_BUFFER_SIZE + 1] = {0};
        if (recv(sock_fd, buffer, SHM_BUFFER_SIZE, 0) == -1) {
            perror("recv buf");
            close(sock_fd);
            pthread_exit(NULL);
        }
        first_unoccupied_idx = (char)find_first_unoccupied_index(buffer);
        printf("CLIENT [Thread %lu]: buff: %s\n", (unsigned long)pthread_self(), buffer);
        close(sock_fd);
        op = WRITER;
        if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            pthread_exit(NULL);
        }
        if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            perror("connect");
            close(sock_fd);
            pthread_exit(NULL);
        }
        if (send(sock_fd, &op, sizeof(op), 0) == -1) {
            perror("send operation");
            close(sock_fd);
            pthread_exit(NULL);
        }
        if (send(sock_fd, &first_unoccupied_idx, sizeof(first_unoccupied_idx), 0) == -1) {
            perror("send idx");
            close(sock_fd);
            pthread_exit(NULL);
        }
        // sleep(rand() % 2);
        char result_buffer[STR_BUFFER_SIZE];
        if (recv(sock_fd, result_buffer, STR_BUFFER_SIZE, 0) == -1) {
            perror("recv buf");
            close(sock_fd);
            pthread_exit(NULL);
        }
        close(sock_fd);
        clock_t total_time = clock() - start_time;
        printf("CLIENT [Thread %lu]: %s (idx: %d); time: %ld\n",(unsigned long)pthread_self(), result_buffer, (int)first_unoccupied_idx, total_time);
        if ((first_unoccupied_idx >= 0) && (strlen(result_buffer) == 1)) {
            pthread_mutex_lock(&file_mutex);
            fprintf(log_file, "%ld\n", (long)total_time);
            pthread_mutex_unlock(&file_mutex); 
        }
    }
    pthread_exit(NULL);
}

void sig_handler(int s_num) {
    printf("received signal %d\n", s_num);
    fclose(log_file);
    loop_flag = 0;
}

int main() {
    srand(time(NULL));
    signal(SIGINT, sig_handler); 
    log_file = fopen(FILE_TIME, "w");
    if (!log_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    pthread_t threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        sleep(rand() % 6 + 3);
        if (pthread_create(&threads[i], NULL, client_thread, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        pthread_detach(threads[i]);
    }
    while (loop_flag);
    exit(EXIT_SUCCESS);
}
