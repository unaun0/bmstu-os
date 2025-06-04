#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define FILE_NAME 	"../alphabet.txt"
#define BUFF_SIZE    20
#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define RESET       "\x1b[0m"

void *fileReader(void *args) {
    FILE *fs = (FILE *)args;
    int rc = 1;
    char c;
    while (rc == 1) {
        if ((rc = fscanf(fs, "%c\n", &c)) == 1) {
            fprintf(stdout, GREEN "%c" RESET, c);
            sleep(1);
        }
    }
    return NULL;
}


int main(void) {
    int fd = open(FILE_NAME, O_RDONLY);
    if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}
    FILE *fs1 = fdopen(fd, "r");
    char buf1[BUFF_SIZE];
    setvbuf(fs1, buf1, _IOFBF, BUFF_SIZE);
    FILE *fs2 = fdopen(fd, "r");
    char buf2[BUFF_SIZE];
    setvbuf(fs2, buf2, _IOFBF, BUFF_SIZE);
    pthread_t thread;
    int prc;
    if ((prc = pthread_create(&thread, NULL, fileReader, (void *)fs2)) != 0) {
        fclose(fs1);
	    fclose(fs2);
        fprintf(stderr, "pthread_create failed: %s\n", strerror(prc));
        exit(EXIT_FAILURE);
    }
    int rc = 1;
    char c;
    while (rc == 1) {
        rc = fscanf(fs1, "%c\n", &c);
        if (rc == 1) {
            fprintf(stdout, RED "%c" RESET, c);
            sleep(1);
        }
    }
    if (pthread_join(thread, NULL) != 0) {
        fclose(fs1);
	    fclose(fs2);
        fprintf(stderr, "pthread_create failed: %s\n", strerror(prc));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "\n");
    fclose(fs1);
	fclose(fs2);
    exit(EXIT_SUCCESS);
}