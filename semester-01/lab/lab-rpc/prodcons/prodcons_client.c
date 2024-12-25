#include "prodcons.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define COUNT_CLIENTS 100

typedef struct {
    char *host;
    REQUEST service_1_arg;
} thread_data_t;

void* process_request(void *arg) {
    sleep(rand() % 5 + 2);
    thread_data_t *data = (thread_data_t*)arg; 
    CLIENT *clnt;
    REQUEST *result_1;
#ifndef DEBUG
    clnt = clnt_create(data->host, PRODCONS_PROG, PRODCONS_VERS, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror(data->host);
        free(data);
        return NULL;
    }
#endif
    result_1 = service_1(&data->service_1_arg, clnt);
    if (result_1 == (REQUEST*)NULL) {
        clnt_perror(clnt, "call failed");
    } else {
        int idx = result_1->idx;
        time_t start_time = time(NULL);
        while (!((result_1->result >= 'a') && (result_1->result <= 'z'))) {
            data->service_1_arg.idx = idx;
            result_1 = service_1(&data->service_1_arg, clnt);
            if (result_1 == (REQUEST*)NULL) {
                clnt_perror(clnt, "call failed");
                break;
            }
            if (difftime(time(NULL), start_time) > 120) {
                printf("%d - type = %c - timeout\n", data->service_1_arg.idx, data->service_1_arg.type);
                break;
            }
        }
        data->service_1_arg.idx = idx;
        data->service_1_arg.result = result_1->result == 0 ? '-' : result_1->result;
        printf("%d - type = %c, result = %c\n", data->service_1_arg.idx, data->service_1_arg.type, data->service_1_arg.result);
    }
#ifndef DEBUG
    clnt_destroy(clnt);
#endif /* DEBUG */
    free(data);
    return NULL;
}

void prodcons_prog_1(char *host) {
    printf("inside prodcons_prog_1 with host: %s\n", host);
    pthread_t threads[COUNT_CLIENTS];
    for (int i = 0; i < COUNT_CLIENTS; i++) {
        sleep(rand() % 5 + 2);
        thread_data_t *data = (thread_data_t*)malloc(sizeof(thread_data_t));
        if (!data) {
            perror("error malloc");
            exit(1);
        }
        data->host = host;
        data->service_1_arg.type = rand() % 2 == 0 ? 'p' : 'c';
        data->service_1_arg.idx = -1;
        data->service_1_arg.result = '-';
        if (pthread_create(&threads[i], NULL, process_request, data) != 0) {
            perror("error pthread create");
            free(data);
        }
    }
    for (int i = 0; i < COUNT_CLIENTS; i++) pthread_join(threads[i], NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: %s server_host\n", argv[0]);
        exit(1);
    }
    printf("argument: %s\n", argv[1]);
    srand(time(NULL));
    prodcons_prog_1(argv[1]);
    return 0;
}
