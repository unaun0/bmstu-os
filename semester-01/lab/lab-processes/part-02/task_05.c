#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int flag = 0;

void sig_handler(int sig_num){ 
    flag = 1;
    printf("Signal %d handled\n", sig_num);
}

int main(){
    pid_t cpid[2];
    pid_t w;
    int wstatus;
    int fd[2];
    signal(SIGINT, sig_handler);
    printf("Press \'ctr + c\', to send message\n");
    printf("parent: pid = %d\n", getpid());
    if (pipe(fd) == -1)
    {
        perror("can't pipe");
        exit(1);
    }
    sleep(5);
    for (int i = 0; i < 2; ++i){
        if ((cpid[i] = fork())== -1){
	        perror("can't fork\n");
            exit(1);
        } else if (cpid[i] == 0) {
            const char* msg;
            if (i == 0)
                msg = "aaaaaaaaaaaaa\n";
            else
                msg = "bbbbb\n";
            if (flag) {
                close(fd[0]);
                write(fd[1], msg, strlen(msg));
            }
            else printf("No signal\n");
            exit(0);
        }
    }
    char c;
    close(fd[1]);
    while (read(fd[0], &c, 1) > 0)
        printf("%c", c);
    printf("\n");
    close(fd[0]);
    for (int i = 0; i < 2; ++i) {
        w = waitpid(cpid[i],&wstatus, WUNTRACED | WCONTINUED);
        if (w == -1) {
            perror("wait error");
            exit(1);
        }
        if (WIFEXITED(wstatus)) {
            printf("wait pid: %d, exited, status=%d\n", w, WEXITSTATUS(wstatus));
        } else if (WIFSIGNALED(wstatus)) {
            printf("wait pid: %d, killed by signal %d\n", w, WTERMSIG(wstatus));
        } else if (WIFSTOPPED(wstatus)) {
            printf("wait pid: %d, stopped by signal %d\n", w, WSTOPSIG(wstatus));
        }
    }
    exit(0);
}
