#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/signal.h>
#include <errno.h>

#define SE 0
#define SF 1
#define SB 2

#define P -1
#define V 1


struct sembuf producer_lock[2] = {{SE, P, 0}, {SB, P, 0}};
struct sembuf producer_release[2] = {{SB, V, 0}, {SF, V, 0}};
struct sembuf consumer_lock[2] = {{SF, P, 0}, {SB, P, 0}};
struct sembuf consumer_release[2] = {{SB, V, 0}, {SE, V, 0}};
int flag = 1;

void sig_catch(int sig_num) {
    flag = 0;
    printf("signal %d caught by %d\n", sig_num, getpid());
}

int producer(char *addr, int semid) {
    srand(getpid());
    while (flag) {
        sleep(rand() % 3);
        if (semop(semid, producer_lock, 2) == -1) {
            perror("semop error");
            printf("semop %d, errno %d\n", getpid(), errno);
            exit(1);
        }
        char **prod_cur = (char **)(addr + sizeof(char *));
        char ch = 'a' + (*prod_cur - (addr + sizeof(char *) + sizeof(char *))) % 26;
        **prod_cur = ch;
        (*prod_cur)++;
        // printf("producer (pid = %d) write: %c\n", getpid(), ch);
        if (semop(semid, producer_release, 2) == -1) {
            perror("semop error");
            printf("semop %d, errno %d\n", getpid(), errno);
            exit(1);
        }
        printf("producer (pid = %d): %c\n", getpid(), ch);
    }
    return 0;
}

int consumer(char *addr, int semid) {
    srand(getpid());
    while (flag) {
        sleep(rand() % 2);
        if (semop(semid, consumer_lock, 2) == -1) {
            perror("semop error");
            printf("semop %d, errno %d\n", getpid(), errno);
            exit(1);
        }
        char **cons_cur = (char **)addr;
        char c = **cons_cur;
        (*cons_cur)++;
       // printf("consumer (pid=%d read): %c\n", getpid(), c);
        if (semop(semid, consumer_release, 2) == -1) {
            perror("semop error");
            printf("semop %d, errno %d\n", getpid(), errno);
            exit(1);
        }
        printf("consumer (pid=%d): %c\n", getpid(), c);
    }
    return 0;
}

int main() {
    signal(SIGINT, sig_catch);
    int perms = S_IRWXU | S_IRWXG | S_IRWXO;
    int shm_fd = shmget(IPC_PRIVATE, 4096, perms);
    if (shm_fd == -1) {
        perror("shmget error");
        exit(1);
    }
    char *addr = (char *)shmat(shm_fd, 0, 0);
    if (addr == (char *)-1) {
        perror("shmat error");
        exit(1);
    }
    char **cons_cur = (char **)addr;
    *cons_cur = addr + (sizeof(char *) + sizeof(char *));
    char **prod_cur = (char **)(addr + sizeof(char *));
    *prod_cur = addr + (sizeof(char *) + sizeof(char *));
    int sem_fd = semget(IPC_PRIVATE, 3, perms);
    if (sem_fd == -1) {
        perror("semget error");
        exit(1);
    }
    if (semctl(sem_fd, SB, SETVAL, 1) == -1) {perror("semctl");}
    if (semctl(sem_fd, SE, SETVAL, 26) == -1) {perror("semctl");}
    if (semctl(sem_fd, SF, SETVAL, 0) == -1) {perror("semctl");}
    pid_t child_pid;
    for (short i = 0; i < 3; ++i) {
        if ((child_pid = fork()) == -1) {
            perror("can't fork.\n");
            exit(1);
        } else if (child_pid == 0) {
            if (producer(addr, sem_fd) == 1)
                exit(1);
            exit(0);
        }
    }
    for (short i = 0; i < 4; ++i) {
        if ((child_pid = fork()) == -1) {
            perror("can't fork.\n");
            exit(1);
        } else if (child_pid == 0) {
            if (consumer(addr, sem_fd) == 1)
                exit(1);
            exit(0);
        }
    }
    int wstatus;
    for (short i = 0; i < 7; ++i) {
        pid_t w = wait(&wstatus);
        if (w == -1) {
            perror("wait error");
            switch(errno) {
                case ECHILD:
                    printf("the calling process does not have any unwaited-for children.\n");
                    break;
                case EINTR:
                    printf("WNOHANG was not set and an unblocked signal or a SIGCHLD was caught\n");
                    break;
                default:
                    printf("unknown error\n");
                    break;
            }
            exit(1);
        }
        if (WIFEXITED(wstatus))
            printf("pid=%d, exited, status=%d\n", w, WEXITSTATUS(wstatus));
        else if (WIFSIGNALED(wstatus))
            printf("pid=%d, killed by signal %d\n", w, WTERMSIG(wstatus));
        else if (WIFSTOPPED(wstatus))
            printf("pid=%d, stopped by signal %d\n", w, WSTOPSIG(wstatus));
    }
    if (shmctl(shm_fd, IPC_RMID, NULL) == -1){
        perror("shmctl error");
        exit(1);
    }
    if (shmdt((void *)addr) == -1) {
        perror("shmdt error");
        exit(1);
    }
    if (semctl(sem_fd, 0, IPC_RMID) == -1) {
        perror("semctl error");
        exit(1);
    }
    exit(1);
}