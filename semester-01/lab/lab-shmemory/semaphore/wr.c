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
#include <errno.h>

#define COUNT_WRITERS 3
#define COUNT_READERS 5
#define SLEEP 2

#define P -1
#define V 1

#define WAITING_READERS 0
#define ACTIVE_READERS 1
#define ACTIVE_WRITER 2
#define WAITING_WRITERS 3
#define BIN_WRITER 4

const int PERMS =  S_IRWXU | S_IRWXG | S_IRWXO;

int *shm_buf;
int sem_id;
int shm_id;

// Если есть реализация монитора читатели - писатели, 
// что 1 писатель написал, а читатели читают только последее изменение -- потерянное обновление
// Потерянное обновление связано только с разделяемой переменной.
// В системе опр. как условно занятое и уже никакой другой писатель к нему обратиться не сможет
// Если често покупается, оно удаляется из пула свободных мест. То есть повторно к этому месту обратиться невохзможно
// Доп действия с базой данных.

struct sembuf reader_lock[] = {
    {WAITING_READERS, V, 0}, 
    {BIN_WRITER, 0, 0},
    {WAITING_WRITERS, 0, 0}, 
    {ACTIVE_READERS , V, 0}, 
    {WAITING_READERS, P, 0}
};
struct sembuf reader_release[] = {{ACTIVE_WRITER, 0, 0}, {ACTIVE_READERS, P, 0}};
struct sembuf writer_lock[] = {
    {WAITING_WRITERS, V, 0}, 
    {ACTIVE_READERS, 0, 0},
    {BIN_WRITER, P, 0}, 
    {ACTIVE_WRITER, V, 0},
    {WAITING_WRITERS, P, 0}
};
struct sembuf writer_release[] = {{ACTIVE_WRITER, P, 0}, {BIN_WRITER, V, 0}};

int flag = 1;

void sig_catch(int sig_num) {
    flag = 0;
    printf("signal %d caught by %d\n", sig_num, getpid());
}

int start_read(int sem_id){
    return semop(sem_id, reader_lock, 5);
}

int stop_read(int sem_id){
    return semop(sem_id, reader_release, 2);
}

int start_write(int sem_id){
    return semop(sem_id, writer_lock, 5);
}

int stop_write(int sem_id){
    return semop(sem_id, writer_release, 3);
}

int reader(int semid, int* shm)
{
    srand(getpid());
    while(flag){
        sleep(rand() % 2);
        if (start_read(semid) == -1){
            perror("semop");
            printf("semop %d, errno %d\n", getpid(), errno);
            exit(1);
        }
        char ch = 'a' + (*shm - 1) % 26;
		printf("reader %d: %c\n", getpid(), ch);
        if (stop_read(semid) == -1){
            printf("semop %d, errno %d\n", getpid(), errno);
            perror("semop");
            exit(1);
        }
    }
    return 0;
}

int writer(int semid, int* shm)
{
    srand(getpid());
    while(flag){
        sleep(rand() % 2);
        if (start_write(semid) == -1){
            perror("semop");
            printf("semop %d, errno %d\n", getpid(), errno);
            exit(1);
        }
        char ch = 'a' + *shm % 26;
        (*shm)++;
        printf("writer %d: %c\n", getpid(), ch);
        if (stop_write(semid) == -1){
            perror("semop");
            printf("semop %d, errno %d\n", getpid(), errno);
            exit(1);
        }
    }
    return 0;
}

int main()
{
    signal(SIGINT, sig_catch);
    key_t key = ftok("keyf",0);
    if (key == -1){
        printf("ftok error\n");
        exit(1);
    }
    if ((shm_id = shmget(key, sizeof(int), IPC_CREAT | PERMS)) == -1){
		perror("shmget error\n");
		exit(1);
	}
    srand(time(NULL));
    shm_buf = shmat(shm_id, NULL, 0);
    if (shm_buf == (void*) -1){
        perror("shmat error\n");
        exit(1);
    }
    (*shm_buf) = 0;
    if ((sem_id = semget(key, 5, IPC_CREAT | PERMS)) == -1){
		perror("semget error\n");
		exit(1);
	}
    if ((semctl(sem_id, WAITING_READERS, SETVAL, 0)) == -1){
        perror("semctl error\n");
		exit(1);
    }
    if ((semctl(sem_id, ACTIVE_READERS, SETVAL, 0)) == -1){
        perror("semctl error\n");
		exit(1);
    }
    if ((semctl(sem_id, WAITING_WRITERS, SETVAL, 0)) == -1){
        perror("semctl error\n");
		exit(1);
    }
    if ((semctl(sem_id, ACTIVE_WRITER, SETVAL, 0)) == -1){
        perror("semctl error\n");
		exit(1);
    }
    if ((semctl(sem_id, BIN_WRITER, SETVAL, 1)) == -1){
        perror("semctl error\n");
		exit(1);
    }
    pid_t pid;
    for (int i = 0; i < COUNT_READERS && pid != 0; i++){
        pid = fork();
        if (pid == -1){
            perror("fork error\n");
            exit(1);
        }
        if (pid == 0){
            reader(sem_id, shm_buf);
            exit(0);
        }
	}
	for (int i = 0; i < COUNT_WRITERS && pid != 0; i++) {
        pid = fork();
        if (pid == -1){
            perror("fork error\n");
            exit(1);
        }
        if (pid == 0){
            writer(sem_id, shm_buf);
            exit(0);
        }
	}
    int wstatus;
    for (short i = 0; i < COUNT_WRITERS + COUNT_READERS; ++i) {
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
    if (shmdt((void*)shm_buf) == -1 || shmctl(shm_id, IPC_RMID, NULL) == -1 || semctl(sem_id, IPC_RMID, 0) == -1)
    {
        perror("error\n");
        exit(1);
    }
    exit(0);
}