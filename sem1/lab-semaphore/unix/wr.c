#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define COUNT_WRITERS 3
#define COUNT_READERS 5
#define SLEEP 2

#define P -1
#define V 1

#define WAITING_READERS 0
#define ACTIVE_READERS 1
#define WAITING_WRITERS 2
#define BIN_WRITER 3

const int PERMS =  S_IRWXU | S_IRWXG | S_IRWXO;
int *shm_buf;
int sem_id;
struct sembuf start_read[] = {{WAITING_READERS, V, 0}, {BIN_WRITER, 0, 0}, {WAITING_WRITERS, 0, 0}, {ACTIVE_READERS , V, 0}, {WAITING_READERS, P, 0}};
struct sembuf stop_read[] = {{ACTIVE_READERS, P, 0}};
struct sembuf start_write[] = {{WAITING_WRITERS, V, 0}, {ACTIVE_READERS, 0, 0}, {BIN_WRITER, P, 0}, {WAITING_WRITERS, P, 0}};
struct sembuf stop_write[] = {{BIN_WRITER, V, 0}};
int flag = 1;

void sig_catch(int sig_num) {
    flag = 0;
    printf("signal %d caught by %d\n", sig_num, getpid());
}

void reader(int semid, int* shm) {
    srand(getpid());
    while(flag) {
        sleep(rand() % 3);
        if (semop(semid, start_read, 4) == -1) {
            perror("semop error");
            exit(1);
        }
		printf("reader (pid=%d): %d\n", getpid(), *shm);
        if (semop(semid, stop_read, 1) == -1) {
            perror("semop error");
            exit(1);
        }
        sleep(rand() % SLEEP);
    }
    exit(0);
}

void writer(int semid, int* shm) {
    srand(getpid());
    while(flag) {
        sleep(rand() % 3);
        if (semop(semid, start_write, 4) == -1) {
            perror("semop error");
            exit(1);
        }
        (*shm)++;
        printf("writer (pid=%d): %d\n", getpid(), *shm);
        sleep(rand() % SLEEP);
        if (semop(semid, stop_write, 1) == -1) {
            perror("semop error");
            exit(1);
        }
    }
    exit(0);
}

int main() {
    pid_t pid;
    int shm_id;
    if (signal(SIGINT, sig_catch) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    if ((shm_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | PERMS)) == -1) {
		perror("shmget error\n");
		exit(1);
	}
    srand(time(NULL));
    shm_buf = shmat(shm_id, NULL, 0);
    if (shm_buf == (void*) -1) {
        perror("shmat error\n");
        exit(1);
    }
    (*shm_buf) = 0;
    if ((sem_id = semget(IPC_PRIVATE, 5, IPC_CREAT | PERMS)) == -1) {
		perror("semget error\n");
		exit(1);
	}
	if ((semctl(sem_id, BIN_WRITER, SETVAL, 1)) == -1) {
		perror("semctl error\n");
		exit(1);
	}
	for (int i = 0; i < COUNT_WRITERS; i++) {
        pid_t pid;
        pid = fork();
        if (pid == -1) {
            perror("fork error\n");
            exit(1);
        }
        if (pid == 0)
            writer(sem_id, shm_buf);
    }
    for (int i = 0; i < COUNT_READERS; i++) {
        pid_t pid;
        pid = fork();
        if (pid == -1) {
            perror("fork error\n");
            exit(1);
        }
        if (pid == 0)
            reader(sem_id, shm_buf);
	}
    int wstatus;
    for (short i = 0; i < COUNT_WRITERS + COUNT_READERS; ++i) {
        pid_t w = wait(&wstatus);
        if (w == -1) {
            perror("wait error");
            exit(1);
        }
        if (WIFEXITED(wstatus))
            printf("pid=%d, exited, status=%d\n", w, WEXITSTATUS(wstatus));
        else if (WIFSIGNALED(wstatus))
            printf("pid=%d, killed by signal %d\n", w, WTERMSIG(wstatus));
        else if (WIFSTOPPED(wstatus))
            printf("pid=%d, stopped by signal %d\n", w, WSTOPSIG(wstatus));
    }
    if (shmctl(shm_id, IPC_RMID, NULL))
    {
        perror("shmctl error");
        exit(1);
    }
    if (shmdt((void *)shm_buf) == -1) {
        perror("shmdt error");
        exit(1);
    }
    if (semctl(sem_id, 3, IPC_RMID) == -1) {
        perror("semctl error");
        exit(1);
    }
    exit(0);
}