#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define THREAD_COUNT 20
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9877
#define SHM_BUFFER_SIZE 26
#define STR_BUFFER_SIZE 255
#define READER 'r'
#define WRITER 'w'

int loop_flag = 1;

char ops[] = { READER, WRITER };

int find_first_availiable_index(const char *buffer) {
    char *pos = strpbrk(buffer, "abcdefghijklmnopqrstuvwxyz");
    if (pos)
        return (int)(pos - buffer);
    return -1;
}
char first_availiable_idx = 0;

void *client_thread(void *arg) {
    srand(time(NULL));
    int sock_fd;
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    
    while (loop_flag) 
    {
        sleep(rand() % 6 + 3);
        
        if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            pthread_exit(NULL);
        }
        if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            perror("connect");
            close(sock_fd);
            pthread_exit(NULL);
        }
        char op = ops[rand() % 2];
        if (send(sock_fd, &op, sizeof(op), 0) == -1) {
            perror("send operation");
            close(sock_fd);
            pthread_exit(NULL);
        }
        if (op == READER) {
            char buffer[SHM_BUFFER_SIZE + 1] = {0};
            if (recv(sock_fd, buffer, SHM_BUFFER_SIZE, 0) == -1) {
                perror("recv buf");
                close(sock_fd);
                pthread_exit(NULL);
            }
            first_availiable_idx = (char)find_first_availiable_index(buffer);
            printf("CLIENT [Thread %lu]: buff: %s\n", (unsigned long)pthread_self(), buffer);
        } else if (op == WRITER) {
            if (send(sock_fd, &first_availiable_idx, sizeof(first_availiable_idx), 0) == -1) {
                perror("send idx");
                close(sock_fd);
                pthread_exit(NULL);
            }
            sleep(rand() % 2);
            char result_buffer[STR_BUFFER_SIZE];
            if (recv(sock_fd, result_buffer, STR_BUFFER_SIZE, 0) == -1) {
                perror("recv buf");
                close(sock_fd);
                pthread_exit(NULL);
            }
            printf("CLIENT [Thread %lu]: %s (idx: %d)\n",(unsigned long)pthread_self(), result_buffer, (int)first_availiable_idx);
        }
        close(sock_fd);
    }
    pthread_exit(NULL);
}

void handler(int s_num) {
    loop_flag = 0;
}

int main() {
    srand(time(NULL));
    signal(SIGINT, handler); 
    pthread_t threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        if (pthread_create(&threads[i], NULL, client_thread, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        pthread_detach(threads[i]);
    }
    while (loop_flag);
    exit(EXIT_SUCCESS);
}
