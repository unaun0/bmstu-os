#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>

#define SERVER_PORT 9877
#define SERVER_BACKLOG 5

#define SHM_BUFFER_SIZE 52
#define STR_BUFFER_SIZE 255

#define OP_READER 'r'
#define OP_WRITER 'w'

#define SEM_P -1
#define SEM_V 1

#define SEM_COUNT 4
#define SEM_WAITING_READERS 0
#define SEM_ACTIVE_READERS 1
#define SEM_ACTIVE_WRITER 2
#define SEM_WAITING_WRITERS 3

#define SEM_PERMS (S_IRWXU | S_IRWXG | S_IRWXO)

#define KEY_FILE "keyf"

struct sembuf start_read[] = {
    {SEM_WAITING_READERS, SEM_V, 0},
    {SEM_ACTIVE_WRITER, 0, 0}, 
    {SEM_WAITING_WRITERS, 0, 0},
    {SEM_WAITING_READERS, SEM_P, 0},
    {SEM_ACTIVE_READERS, SEM_V, 0}
};
struct sembuf stop_read[] = {
    {SEM_ACTIVE_READERS, SEM_P, 0}
};
struct sembuf start_write[] = {
    {SEM_WAITING_WRITERS, SEM_V, 0},
    {SEM_ACTIVE_READERS, 0, 0},
    {SEM_ACTIVE_WRITER, SEM_V, 0},
    {SEM_WAITING_WRITERS, SEM_P, 0}
};
struct sembuf stop_write[] = {
    {SEM_ACTIVE_WRITER, SEM_P, 0}
};

static int sem_id;
static int shm_id;
static char *shm_buf;

int loop_flag = 1;
int server_sock_fd;

void handler(int s_num);
void cleanup_port(int port);

void writer(int client_sock_fd, int idx) {
    if (semop(sem_id, start_write, 4) == -1) {
        perror("semop start_write");
        return;
    }
    char buffer[STR_BUFFER_SIZE];
    memset(buffer, '\0', STR_BUFFER_SIZE);
    if (idx < 0 || idx >= SHM_BUFFER_SIZE) {
        snprintf(buffer, sizeof(buffer), "error: the selected index is occupied.");
        if (send(client_sock_fd, buffer, sizeof(char) * STR_BUFFER_SIZE, 0) == -1) {
            perror("send error message");
            return;
        }
        return;
    }
    char ch = shm_buf[idx];
    if (ch == '-') {
        snprintf(buffer, sizeof(buffer), "error: the selected symbol is occupied.");
    } else {
        shm_buf[idx] = '-';
        memset(buffer, '\0', STR_BUFFER_SIZE);
        snprintf(buffer, sizeof(buffer), "%c", ch);
    }
    if (send(client_sock_fd, buffer, sizeof(char) * STR_BUFFER_SIZE, 0) == -1) {
        perror("send result");
        return;
    }
    if (semop(sem_id, stop_write, 1) == -1) {
        perror("semop stop_write");
        return;
    }
    printf("server [Thread %lu]: %s\n", (unsigned long)pthread_self(), buffer);
}

void reader(int client_sock_fd) {
    if (semop(sem_id, start_read, 5) == -1) {
        perror("semop start_read");
        return;
    }
    char buffer[SHM_BUFFER_SIZE + 1];
    memset(buffer, '\0', SHM_BUFFER_SIZE + 1);
    strncpy(buffer, shm_buf, SHM_BUFFER_SIZE);
    if (send(client_sock_fd, buffer, sizeof(char) * (SHM_BUFFER_SIZE + 1), 0) == -1) {
        perror("send buf");
        return;
    }
    if (semop(sem_id, stop_read, 1) == -1) {
        perror("semop stop_read");
        return;
    }
    printf("server [Thread %lu]: %s\n", (unsigned long)pthread_self(), buffer);
}

void* handle_client(void* arg) {
    int client_sock_fd = *(int*)arg;
    free(arg);
    char op;
    if (recv(client_sock_fd, &op, sizeof(op), 0) == -1) {
        perror("recv op");
        close(client_sock_fd);
        pthread_exit(NULL);
    }
    if (op == OP_READER) {
        reader(client_sock_fd);
    } else if (op == OP_WRITER) {
        char idx;
        if (recv(client_sock_fd, &idx, sizeof(idx), 0) == -1) {
            perror("recv idx");
            close(client_sock_fd);
            pthread_exit(NULL);
        }
        writer(client_sock_fd, (int)idx);
    } else {
        perror("invalid op");
    }
    close(client_sock_fd);
    pthread_exit(NULL);
}

int main() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    if ((server_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    int reuse_addr = 1;
    if (setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) == -1) {
        perror("setsockopt");
        close(server_sock_fd);
        exit(EXIT_FAILURE);
    }
    cleanup_port(SERVER_PORT);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    if (bind(server_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }
    if (listen(server_sock_fd, SERVER_BACKLOG) == -1) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }
    creat(KEY_FILE, SEM_PERMS);
    key_t key = ftok(KEY_FILE, 0);
    if (key == -1) {
        perror("ftok error");
        exit(EXIT_FAILURE);
    }
    if ((shm_id = shmget(key, SHM_BUFFER_SIZE, IPC_CREAT | SEM_PERMS)) == -1) {
        perror("shmget error");
        exit(EXIT_FAILURE);
    }
    shm_buf = shmat(shm_id, NULL, 0);
    if (shm_buf == (void*)-1) {
        perror("shmat error");
        exit(EXIT_FAILURE);
    }
    size_t index = 0;
    for (char c = 'a'; c <= 'z' && index < SHM_BUFFER_SIZE; ++c) {
        shm_buf[index++] = c;
    }
    for (char c = 'A'; c <= 'Z' && index < SHM_BUFFER_SIZE; ++c) {
        shm_buf[index++] = c;
    }
    if ((sem_id = semget(key, 4, IPC_CREAT | SEM_PERMS)) == -1) {
        perror("semget error");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < SEM_COUNT; ++i) {
        if (semctl(sem_id, i, SETVAL, 0) == -1) {
            perror("semctl error");
            exit(EXIT_FAILURE);
        }
    }
    printf("server resources initialized\n");
    signal(SIGINT, handler);
    printf("server started on port %d\n", SERVER_PORT);
    while (loop_flag) {
        int* client_sock_fd = malloc(sizeof(int));
        if (!client_sock_fd) {
            perror("malloc error");
            continue;
        }
        *client_sock_fd = accept(server_sock_fd, (struct sockaddr*)&client_addr, &client_len);
        if (*client_sock_fd == -1) {
            free(client_sock_fd);
            break;
        }
        printf("new connection\n");
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, client_sock_fd) != 0) {
            perror("pthread_create error");
            free(client_sock_fd);
            close(*client_sock_fd);
        } else {
            pthread_detach(thread);
        }
    }
    if (shm_buf != (void*)-1)
        shmdt((void*)shm_buf);
    if (shm_id != -1)
        shmctl(shm_id, IPC_RMID, NULL);
    if (sem_id != -1)
        semctl(sem_id, 0, IPC_RMID);
    printf("server resources destroyed\n");
    exit(EXIT_SUCCESS);
}

void handler(int s_num) {
    loop_flag = 0;
    shutdown(server_sock_fd, SHUT_RDWR);
    close(server_sock_fd);
}

void cleanup_port(int port) {
    char command[256];
    snprintf(command, sizeof(command), "lsof -ti :%d | xargs kill -9", port);
    system(command);
}
