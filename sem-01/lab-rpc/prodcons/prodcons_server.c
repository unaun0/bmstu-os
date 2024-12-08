/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>

#include "prodcons.h"

#define BUFFER_SIZE 26
#define CLNT_COUNT 100

int buffer[BUFFER_SIZE];
int in = 0; int out = 0;

#define SE 0
#define SF 1
#define SB 2

#define P -1
#define V 1

int sem_id;

struct sembuf producer_lock[2] = {{SE, P, 0}, {SB, P, 0}};
struct sembuf producer_release[2] = {{SB, V, 0}, {SF, V, 0}};
struct sembuf consumer_lock[2] = {{SF, P, 0}, {SB, P, 0}};
struct sembuf consumer_release[2] = {{SB, V, 0}, {SE, V, 0}};

int result[CLNT_COUNT];
pthread_mutex_t res_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_semaphore() {
    sem_id = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget failed");
        exit(1);
    }

    // Инициализируем семафоры
    if (semctl(sem_id, SE, SETVAL, 1) == -1) {
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
	pthread_mutex_init(&res_mutex, NULL);
	for (int i = 0; i < CLNT_COUNT; i++)
		result[i] = -1;
	for (int i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = 0;
}	

void destroy_semaphores() {
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl failed to remove semaphores");
    }
}

typedef struct {
    REQUEST *argp;
    struct svc_req *rqstp;
} thread_data_t;

void *producer_thread(void *data) {
    thread_data_t *thread_data = (thread_data_t *)data;
    REQUEST *argp = thread_data->argp;
	REQUEST local_argp = *argp;
    struct svc_req *rqstp = thread_data->rqstp;
    static char res;

    if (semop(sem_id, producer_lock, 2) == -1) {
        perror("semop failed in producer_lock");
		free(thread_data);
        return NULL;
    }
    buffer[in] = in;
    res = 'a' + in % 26;
    printf("Producer (ID %d): %c at index %d\n", local_argp.idx, res, in);
    in = (in + 1) % BUFFER_SIZE;
	if (semop(sem_id, producer_release, 2) == -1) {
        perror("semop failed in producer_release");
		free(thread_data);
        return NULL;
    }
    if ((local_argp.idx < CLNT_COUNT) && (result[local_argp.idx] == -1)) {
		pthread_mutex_lock(&res_mutex);
        result[local_argp.idx] = res;
		pthread_mutex_unlock(&res_mutex);
	}
	else if ((local_argp.idx < CLNT_COUNT) && (result[local_argp.idx] != -1)) {
		time_t start_time = time(NULL); 
		while (result[local_argp.idx] != -1){
			time_t current_time = time(NULL);
			if (difftime(current_time, start_time) > 120.0) {
            	printf("Timeout reached for producer %d, exiting thread.\n", local_argp.idx);
				free(thread_data);
            	return NULL;
        	}
		}
		pthread_mutex_lock(&res_mutex);
        result[local_argp.idx] = res;
		pthread_mutex_unlock(&res_mutex);
	}
	free(thread_data);
	return NULL;
}

void* consumer_thread(void *data) {
    thread_data_t *thread_data = (thread_data_t *)data;
    REQUEST *argp = thread_data->argp;
	REQUEST local_argp = *argp;
    struct svc_req *rqstp = thread_data->rqstp;
    static char res;
	if (semop(sem_id, consumer_lock, 2) == -1) {
        perror("semop failed in consumer_lock");
		free(thread_data);
        return NULL;
    }
    res = 'a' + buffer[out];
    printf("Consumer (ID %d): %c from index %d\n", local_argp.idx, res, out);
    buffer[out] = -1;
	out = (out + 1) % BUFFER_SIZE;
	if (semop(sem_id, consumer_release, 2) == -1) {
        perror("semop failed in consumer_release");
		free(thread_data);
        return NULL;
    }
    if ((local_argp.idx < CLNT_COUNT) && (result[local_argp.idx] == -1)) {
		pthread_mutex_lock(&res_mutex);
        result[local_argp.idx] = res;
		pthread_mutex_unlock(&res_mutex);
	}
	else if ((local_argp.idx < CLNT_COUNT) && (result[local_argp.idx] != -1)) {
		time_t start_time = time(NULL); 
		while (result[local_argp.idx] != -1){
			time_t current_time = time(NULL);
			if (difftime(current_time, start_time) > 120.0) {
            	printf("Timeout reached for consumer %d, exiting thread.\n", local_argp.idx);
				free(thread_data);
            	return NULL;
        	}
		}
		pthread_mutex_lock(&res_mutex);
        result[local_argp.idx] = res;
		pthread_mutex_unlock(&res_mutex);
	}
	free(thread_data);
	return NULL;
}

int*
producer_1_svc(REQUEST *argp, struct svc_req *rqstp)
{
	static int res;
	if (argp->res == 0) {
		pthread_t thread_id;
		thread_data_t *data = malloc(sizeof(thread_data_t));
		printf("Producer (ID %d) connected.\n", argp->idx);
		if (data == NULL) {
			fprintf(stderr, "Failed to allocate memory for thread data\n");
			res = 0;
			return &res;
		}
		data->argp = argp;
		data->rqstp = rqstp;
		pthread_create(&thread_id, NULL, producer_thread, (void *)data);
		pthread_detach(thread_id);
		res = -1;
		return &res;
	}
	else if (result[argp->idx] != -1) {
		res = result[argp->idx];
		pthread_mutex_lock(&res_mutex);
        result[argp->idx] = -1;
		pthread_mutex_unlock(&res_mutex);
		printf("Producer (ID %d) disconnected.\n", argp->idx);
	}
	return &res;
}

int*
consumer_1_svc(REQUEST *argp, struct svc_req *rqstp)
{
	static int res;
	if (argp->res == 0) {
		res = -1;
		pthread_t thread_id;
		thread_data_t *data = malloc(sizeof(thread_data_t));
		printf("Consumer (ID %d) connected.\n", argp->idx);
		if (data == NULL) {
			fprintf(stderr, "Failed to allocate memory for thread data\n");
			res = 0;
			return &res;
		}
		data->argp = argp;
		data->rqstp = rqstp;
		pthread_create(&thread_id, NULL, consumer_thread, (void *)data);
		pthread_detach(thread_id);
		return &res;
	} else if (result[argp->idx] != -1) {
		res = result[argp->idx];
		pthread_mutex_lock(&res_mutex);
        result[argp->idx] = -1;
		pthread_mutex_unlock(&res_mutex);
		printf("Consumer (ID %d) disconnected.\n", argp->idx);
	}
	return &res;
}
