#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define FILE_NAME "../alphabet.txt"
#define CHAR_SIZE sizeof(char)
#define BUFF_SIZE 32 * CHAR_SIZE

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
    int rc1 = 1, rc2 = 1;
    char c;
    while (rc1 == 1 || rc2 == 1) {
        rc1 = read(fd1, &c, CHAR_SIZE);
        if (rc1 == 1) {
            write(STDOUT_FILENO, &c, CHAR_SIZE);
        }
        rc2 = read(fd2, &c, 1);
        if (rc2 == 1) {
            write(STDOUT_FILENO, &c, CHAR_SIZE);
        }
    }
	write(STDOUT_FILENO, "\n", 1);
    close(fd1);
    close(fd2);
	exit(EXIT_SUCCESS);
}