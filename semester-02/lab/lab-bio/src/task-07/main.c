#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

#define FILE_OUT "output.txt"

struct arg {
    FILE *f;
    int off;
};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void fileInfo(FILE *f) {
	struct stat statbuf;
	stat(FILE_OUT, &statbuf);
    pthread_mutex_lock(&mutex);
	printf("st_ino: %llu | ", statbuf.st_ino);
	printf("st_size: %d | ", statbuf.st_blksize);
    printf("pos: %ld\n", ftell(f));
    pthread_mutex_unlock(&mutex);
}

void *fileWriter(void *arg) {
    struct arg *args = (struct arg*) arg;
	for (char c = 'A' + args->off; c <= 'Z'; c+=2) {
        fprintf(args->f, "%c", c);
        fileInfo(args->f);
    }
    return NULL;
}

int main() {
    FILE *f1 = fopen(FILE_OUT, "w");
    fileInfo(f1);
    FILE *f2 = fopen(FILE_OUT, "w");
    fileInfo(f2);
    pthread_t thread1, thread2;
    struct arg arg1 = {.f = f1, .off = 0}; 
    struct arg arg2 = {.f = f2, .off = 1};
    if (pthread_create(&thread1, NULL, fileWriter, &arg1) != 0) {
        perror("error: create thread1");
        fclose(f1);
        fclose(f2);
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&thread2, NULL, fileWriter, &arg2) != 0) {
        perror("error: create thread2");
        fclose(f1);
        fclose(f2);
        exit(EXIT_FAILURE);
    }
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    fclose(f1);
    fileInfo(f1);
    fclose(f2);
    fileInfo(f2);
    exit(EXIT_SUCCESS);
}