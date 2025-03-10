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
#include <time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>

#define SERVER_PORT 9877
#define SERVER_BACKLOG 5

#define SHM_BUFFER_SIZE 52
#define STR_BUFFER_SIZE 255

#define OP_READER 'r'
#define OP_WRITER 'w'

#define SEM_P -1
#define SEM_V 1

#define SHM_PERMS (S_IRWXU | S_IRWXG | S_IRWXO)

#define KEY_FILE "keyf"

static int shm_id;
static char *shm_buf;
int loop_flag = 1;
int listen_sock_fd;
unsigned long long start_ticks, end_ticks;

void sig_handler(int s_num) {
    printf("Server shutting down...\n");
    loop_flag = 0;
    shutdown(listen_sock_fd, SHUT_RDWR);
    close(listen_sock_fd);
}

void writer(int connect_sock_fd, int idx) {
    char buffer[STR_BUFFER_SIZE];
    if (idx < 0 || idx >= SHM_BUFFER_SIZE) {
        raise(SIGINT);
        snprintf(buffer, SHM_BUFFER_SIZE, "buffer empty");
        send(connect_sock_fd, buffer, SHM_BUFFER_SIZE, 0);
    } else {
        char ch = shm_buf[idx];
        if (ch == '-') snprintf(buffer, STR_BUFFER_SIZE, "Symbol occupied");
        else {
            shm_buf[idx] = '-';
            snprintf(buffer, STR_BUFFER_SIZE, "%c", ch);
        }
        send(connect_sock_fd, buffer, STR_BUFFER_SIZE, 0);
        printf("client [writer]: %s (idx - %d)\n", buffer, idx);
    }
}

void reader(int connect_sock_fd) {
    char buffer[SHM_BUFFER_SIZE + 1] = { 0 };
    strncpy(buffer, shm_buf, SHM_BUFFER_SIZE);
    send(connect_sock_fd, buffer, SHM_BUFFER_SIZE, 0);
    printf("client [reader]: %s\n", buffer);
}

void client_service(int connect_sock_fd) {
    char op;
    if (recv(connect_sock_fd, &op, sizeof(op), 0) <= 0) {
        close(connect_sock_fd);
    } else {
        if (op == OP_READER) reader(connect_sock_fd);
        else if (op == OP_WRITER) {
            char idx;
            if (recv(connect_sock_fd, &idx, sizeof(idx), 0) <= 0) {
                close(connect_sock_fd);
            } else
                writer(connect_sock_fd, (int)idx);
        }
        close(connect_sock_fd);
    }
}

int main() {
    key_t key = ftok(KEY_FILE, 0);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    if ((shm_id = shmget(key, SHM_BUFFER_SIZE, IPC_CREAT | SHM_PERMS)) == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    shm_buf = shmat(shm_id, NULL, 0);
    if (shm_buf == (void*)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    size_t index = 0;
    for (char c = 'a'; c <= 'z' && index < SHM_BUFFER_SIZE; ++c) {
        shm_buf[index++] = c;
    }
    for (char c = 'A'; c <= 'Z' && index < SHM_BUFFER_SIZE; ++c) {
        shm_buf[index++] = c;
    }
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    fd_set master_set, read_fds;
    int active_fd_count, connect_sock_fd;
    if ((listen_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    setsockopt(listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    if (bind(listen_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if (listen(listen_sock_fd, SERVER_BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    signal(SIGINT, sig_handler);
    FD_ZERO(&master_set);
    FD_SET(listen_sock_fd, &master_set);
    active_fd_count = listen_sock_fd;
    printf("server started on port %d\n", SERVER_PORT);
    while (loop_flag) {
        read_fds = master_set;
        if (select(active_fd_count + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }
        for (int fd = 0; fd <= active_fd_count; fd++) {
            if (fd == listen_sock_fd) {
                connect_sock_fd = accept(listen_sock_fd, (struct sockaddr*)&client_addr, &client_len);
                if (connect_sock_fd == -1) {
                    perror("accept");
                    continue;
                }
                FD_SET(connect_sock_fd, &master_set);
                if (connect_sock_fd > active_fd_count) 
                    active_fd_count = connect_sock_fd;
            } else {
                client_service(fd);
                FD_CLR(fd, &master_set);
            }
        }
    }
    if (shm_buf != (void*)-1)
        shmdt((void*)shm_buf);
    if (shm_id != -1) 
        shmctl(shm_id, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}
