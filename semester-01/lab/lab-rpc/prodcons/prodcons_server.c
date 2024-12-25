#include "prodcons.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>

#define MAX_CNT_CLNT 1024 * 1024
#define BUFFER_SIZE 26 + 3

#define CONSUMER 'c'
#define PRODUCER 'p'

#define SE 0
#define SF 1
#define SB 2

#define P -1
#define V 1

char resultBuffer[MAX_CNT_CLNT] = { 0 };
char *buffer = NULL;
int sem_id;
int idx;

struct sembuf producer_lock[2] = {{SE, P, 0}, {SB, P, 0}};
struct sembuf producer_release[2] = {{SB, V, 0}, {SF, V, 0}};
struct sembuf consumer_lock[2] = {{SF, P, 0}, {SB, P, 0}};
struct sembuf consumer_release[2] = {{SB, V, 0}, {SE, V, 0}};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void init() {
	sem_id = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);
    if (sem_id == -1) {
		perror("semget failed");
        exit(1);
    }
    if (semctl(sem_id, SE, SETVAL, BUFFER_SIZE) == -1) {
        perror("semctl failed for SE");
        exit(1);
    }
    if (semctl(sem_id, SF, SETVAL, 0) == -1) {
        perror("semctl failed for SF");
        exit(1);
    }
    if (semctl(sem_id, SB, SETVAL, 1) == -1) {
        perror("semctl failed for SB");
        exit(1);
    }
	buffer = malloc(BUFFER_SIZE * sizeof(char));
    if (buffer == NULL) {
        perror("malloc failed for buffer");
        exit(1);
    }
    memset(buffer, 0, BUFFER_SIZE * sizeof(char));
	*(buffer) = 0; *(buffer + 1) = 0; *(buffer + 2) = 'a';
	printf("initialized\n");
}

void destroy() {
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl failed to remove semaphores");
    }
	pthread_mutex_destroy(&mutex);
	if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }
	printf("destroyed\n");
}

typedef struct {
    int idx;
} thread_data_t;

void *producer(void *data) {
    thread_data_t *thread_data = (thread_data_t *)data;
    int idx = thread_data->idx;
    static char res;
    if (semop(sem_id, producer_lock, 2) == -1) {
        perror("semop failed in producer_lock");
		free(thread_data);
        return NULL;
    }
    int sid = *(buffer) % (BUFFER_SIZE - 3);
	char ch = *(buffer + 2);
	*(buffer + 3 + sid) = ch;   
    printf("producer (idx = %d): %c\n", idx, *(buffer + 3 + sid));
	resultBuffer[idx] = *(buffer + 3 + sid);
	(*(buffer + 2))++;
    *(buffer) = (*(buffer) + 1) % (BUFFER_SIZE - 3); 
	if (semop(sem_id, producer_release, 2) == -1) {
        perror("semop failed in producer_release");
		free(thread_data);
        return NULL;
    }
	free(thread_data);
	return NULL;
}

void* consumer(void *data) {
    thread_data_t *thread_data = (thread_data_t *)data;
    int idx = thread_data->idx;
    static char res;
	if (semop(sem_id, consumer_lock, 2) == -1) {
        perror("semop failed in consumer_lock");
		free(thread_data);
        return NULL;
    }
	int sid = *(buffer + 1) % (BUFFER_SIZE - 3);
	char ch = *(buffer + 3 + sid);  
    printf("consumer (idx = %d): %c\n", idx, ch);
	resultBuffer[idx] = ch;
	*(buffer + 3 + sid) = 0;
    *(buffer + 1) = (*(buffer + 1) + 1) % (BUFFER_SIZE - 3);
	if (semop(sem_id, consumer_release, 2) == -1) {
        perror("semop failed in consumer_release");
		free(thread_data);
        return NULL;
    }
	free(thread_data);
	return NULL;
}

REQUEST*
service_1_svc(REQUEST *argp, struct svc_req *rqstp)
{
	static REQUEST result;
	if (argp->idx >= 0) {
		result.idx = argp->idx;
		result.result = (argp->result == '-') ? '-' : argp->result;
		if (result.result == '-' && \
			resultBuffer[result.idx] >= 'a' && resultBuffer[result.idx] <= 'z') {
			result.result = resultBuffer[result.idx];
			printf("client %d (type = %c) disconnected.\n", result.idx, argp->type);
			return &result;
		}
		return &result;
	}
	result.result = '-';
	pthread_mutex_lock(&mutex);
	result.idx = idx; idx++;
	pthread_mutex_unlock(&mutex);
	pthread_t thread_id;
	thread_data_t *data = malloc(sizeof(thread_data_t));
	printf("client %d (type = %c) connected.\n", result.idx, argp->type);
	if (data == NULL) {
		fprintf(stderr, "failed to allocate memory for thread data\n");
		return NULL;
	}
	data->idx = result.idx;
	switch(argp->type) {
		case(CONSUMER): {
			pthread_create(&thread_id, NULL, consumer, (void *)data);
			pthread_detach(thread_id);
			break;
		}
		case(PRODUCER): {
			pthread_create(&thread_id, NULL, producer, (void *)data);
			pthread_detach(thread_id);
			break;
		}
		default: {
			printf("undefined operation\n");
			break;
		}
	}
	return &result;
}
