#include "calculator.h"

struct CALCULATOR *calculator_proc_1_svc(struct CALCULATOR *argp, struct svc_req *rqstp) {
    static struct CALCULATOR result;

    // Выполнение операции
    switch (argp->op) {
        case ADD:
            result.result = argp->arg1 + argp->arg2;
            break;
        case SUB:
            result.result = argp->arg1 - argp->arg2;
            break;
        case MUL:
            result.result = argp->arg1 * argp->arg2;
            break;
        case DIV:
            if (argp->arg2 != 0) {
                result.result = argp->arg1 / argp->arg2;
            } else {
                printf("Error: Division by zero\n");
                result.result = 0; // Можно обработать ошибку по-другому
            }
            break;
        default:
            printf("Error: Invalid operation\n");
            break;
    }
    return &result;
}