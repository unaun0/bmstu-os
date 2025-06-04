#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define FILE_NAME 	"../alphabet.txt"
#define BUFF_SIZE 	20
#define RED     	"\x1b[31m"
#define GREEN   	"\x1b[32m"
#define RESET   	"\x1b[0m"

int main() {
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
	int rc1 = 1, rc2 = 1;
	while (rc1 == 1 || rc2 == 1) {
		char c;
		rc1 = fscanf(fs1, "%c", &c);
		if (rc1 == 1) {
			fprintf(stdout, RED "%c" RESET, c);
		}
		rc2 = fscanf(fs2, "%c", &c);
		if (rc2 == 1) {
			fprintf(stdout, GREEN "%c" RESET, c);
		}
	}
	fprintf(stdout, "\n");
	fclose(fs1);
	fclose(fs2);
	exit(EXIT_SUCCESS);
}