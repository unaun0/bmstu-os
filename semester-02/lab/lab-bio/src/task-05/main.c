#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define FILE_NAME "../alphabet.txt"
#define CHAR_SIZE sizeof(char)
#define BUFF_SIZE 32 * CHAR_SIZE

void *fileReader(void *args) {
    int fd = *(int *)args;
    int rc = 1;
    char c;
    while (rc == 1) {
        rc = read(fd, &c, CHAR_SIZE);
        if (rc == 1) {
            write(STDOUT_FILENO, &c, CHAR_SIZE);
        }
    }
    return NULL;
}

int main() {
	int fd1 = open(FILE_NAME, O_RDONLY);
    if (fd1 == -1) {
		write(STDERR_FILENO, "error: open fd1\n", BUFF_SIZE);
		exit(EXIT_FAILURE);
	}
	int fd2 = open(FILE_NAME, O_RDONLY);
    if (fd2 == -1) {
		write(STDERR_FILENO, "error: open fd2\n", BUFF_SIZE);
        close(fd1);
		exit(EXIT_FAILURE);
	}
    pthread_t thread;
    int prc;
    if ((prc = pthread_create(&thread, NULL, fileReader, &fd1)) != 0) {
        close(fd1);
        close(fd2);
        write(STDERR_FILENO, "error: pthread1 create\n", BUFF_SIZE);
        exit(EXIT_FAILURE);
    }
    fileReader(&fd2);
    if (pthread_join(thread, NULL)) {
        close(fd1);
        close(fd2);
        write(STDERR_FILENO, "error: pthread join\n", BUFF_SIZE);
        exit(EXIT_FAILURE);
    }
    close(fd1);
    close(fd2);
	exit(EXIT_SUCCESS);
}