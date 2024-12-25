#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <rpc/rpc.h>
#include <math.h>

#include "bakery.h"

#define CLNT_CNT 10
#define OP_CNT 4
#define ATTEMPTS 30

void 
create_req(struct REQUEST *argp) {
    srand((unsigned int)(time(NULL) ^ argp->pid));
    argp->arg1 = (int)((((float)rand() / RAND_MAX) * 200 - 100) * 10) / 10.0;
    argp->arg2 = (int)((((float)rand() / RAND_MAX) * 200 - 100) * 10) / 10.0;
    argp->op = rand() % 4;
}


void 
bakery_prog_1(char *host, int idx)
{
	CLIENT *clnt;
	struct REQUEST req;
	req.pid = getpid();
	req.idx = idx;
	create_req(&req);
	char op_sym;
	if (req.op == 0) op_sym = '+';
	else if (req.op == 1) op_sym = '-';
	else if (req.op == 2) op_sym = '*';
	else op_sym = '/';
	int *res_1;
	float *res_2;

#ifndef DEBUG
	clnt = clnt_create(host, BAKERY_PROG, BAKERY_VER, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror(host);
		exit(1);
	}
	struct timeval timeout;
    timeout.tv_sec = 60;
    timeout.tv_usec = 0;
    if (clnt_control(clnt, CLSET_TIMEOUT, (char *)&timeout) == FALSE) {
        fprintf(stderr, "Failed to set timeout\n");
        clnt_destroy(clnt);
        exit(1);
    }
#endif /* DEBUG */

	res_1 = get_number_1(&req, clnt);
	if (res_1 == (int *)NULL) clnt_perror(clnt, "call failed (get_number)");
	req.num = *res_1;
	printf("Client (pid = %d, no = %d): %.1lf %c %.1lf = ?\n", req.pid, req.num, req.arg1, op_sym, req.arg2);

    int at = 0;
    while (at < ATTEMPTS) {
        res_2 = bakery_service_1(&req, clnt);
		if (res_2 == (float *)NULL) {
			clnt_perror(clnt, "call failed (bakery_service)");
			break;
		}
        else if (*res_2 == INFINITY) {
            sleep(rand() % 3 + 1);
            at++;
        } else {
            break;
		}
    }
	req.res = *res_2;
	if (req.res == INFINITY)
		clnt_perror(clnt, "attempts are exhausted: failed to get valid result, returning INF.\n");
	else
		printf("Client (pid = %d, no = %d): %.1lf %c %.1lf = %.1lf\n", req.pid, req.num, req.arg1, op_sym, req.arg2, req.res);

#ifndef DEBUG
	clnt_destroy(clnt);
#endif /* DEBUG */
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("usage: %s server_host\n", argv[0]);
		exit(1);
	}
	char *host = argv[1];
	pid_t clnt_pid[CLNT_CNT];
	for (size_t i = 0; i < CLNT_CNT; ++i) {
		if ((clnt_pid[i] = fork()) == -1) {
			perror("can't fork\n");
			exit(1);
		} else if (clnt_pid[i] == 0) {
			bakery_prog_1(host, i);
			exit(0);
		}
		srand((unsigned int)time(NULL) ^ clnt_pid[i]);
		sleep(rand() % 3);
	}
	for (size_t i = 0; i < CLNT_CNT; i++) {
		int status;
		waitpid(clnt_pid[i], &status, 0);
	}
	printf("PARENT CLIENT EXITED.\n");
	exit(0);
}
