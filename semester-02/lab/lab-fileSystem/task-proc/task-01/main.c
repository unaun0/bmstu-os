#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int sum(int a, int b) {
    printf("%d\n", getpid());
    return a + b;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Использование: %s <число1> <число2>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);
    printf("Сумма: %d\n", sum(num1, num2));
    sleep(3600);
    return EXIT_SUCCESS;
}
