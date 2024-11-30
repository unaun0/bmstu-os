#include "calculator.h"
#include <stdio.h>
#include <stdlib.h>

void calculator_prog_1(char *host) {
    CLIENT *clnt;
    struct CALCULATOR *result_1;
    struct CALCULATOR calculator_proc_1_arg;

#ifndef DEBUG
    clnt = clnt_create(host, CALCULATOR_PROG, CALCULATOR_VER, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror(host);
        exit(1);
    }
#endif /* DEBUG */

    // Выбор операции
    char c;
    printf("Choose the operation:\n\t0---ADD\n\t1---SUB\n\t2---MUL\n\t3---DIV\n");
    c = getchar();
    if (c > '3' || c < '0') {
        printf("Error: invalid operation\n");
        exit(1);
    }
    calculator_proc_1_arg.op = c - '0';

    // Ввод чисел
    printf("Input the first number: ");
    scanf("%f", &calculator_proc_1_arg.arg1);
    printf("Input the second number: ");
    scanf("%f", &calculator_proc_1_arg.arg2);

    // Вызов удалённой процедуры
    result_1 = calculator_proc_1(&calculator_proc_1_arg, clnt);
    if (result_1 == (struct CALCULATOR *)NULL) {
        clnt_perror(clnt, "call failed");
        exit(1);
    }

    // Вывод результата
    printf("The Result is %.3f\n", result_1->result);

#ifndef DEBUG
    clnt_destroy(clnt);
#endif /* DEBUG */
}

int main(int argc, char *argv[]) {
    char *host;
    if (argc < 2) {
        printf("Usage: %s server_host\n", argv[0]);
        exit(1);
    }
    host = argv[1];
    calculator_prog_1(host); // Вызов клиента
    exit(0);
}