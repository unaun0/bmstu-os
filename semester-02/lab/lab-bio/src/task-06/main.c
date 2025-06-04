#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FILE_OUT "output.txt"

void fileInfo(FILE *f) {
	struct stat statbuf;
	stat(FILE_OUT, &statbuf);
	printf("st_ino: %llu | ", statbuf.st_ino);
	printf("st_size: %d | ", statbuf.st_blksize);
    printf("pos: %ld\n", ftell(f));
}

int main() {
    FILE *f1 = fopen(FILE_OUT, "w");
    fileInfo(f1);
    FILE *f2 = fopen(FILE_OUT, "w");
    fileInfo(f2);
    for (char c = 'A'; c <= 'Z'; c++) {
        if (c % 2) {
            fprintf(f1, "%c", c);
            fileInfo(f1);
        } else {
            fprintf(f2, "%c", c);
            fileInfo(f2);
        }
    }
    fclose(f1);
    fileInfo(f1);
    fclose(f2);
    fileInfo(f2);
    exit(EXIT_SUCCESS);
}