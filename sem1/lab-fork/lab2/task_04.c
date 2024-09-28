#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *msg[2][100] = {"BBBBBBBBBBBBBBBBBBBBBBBBBB", "YYYY"};

int main(void)
{
    pid_t cpid[2];
    int fd[2];
    int wstatus;
    int w;
    char *buf = malloc(100);
    printf("parent pid: %d\n", getpid());
    if (pipe(fd) == -1)
    {
        perror("cant pipe\n");
        exit(1);
    }
    for (size_t i = 0; i < 2; ++i)
    {
        cpid[i] = fork();
        if (cpid[i] == -1)
        {
            perror("cant fork\n");
            exit(1);
        }
        if (cpid[i] == 0)
        {
            close(fd[0]);
            write(fd[1], (*msg)[i], strlen((*msg)[i]));
            printf("msg from child (pid = %d) %s sent to parent\n", getpid(), (*msg)[i]);
            exit(0);
        }
        else
        {   memset(buf, 0, 100);
            read(fd[0], buf, sizeof(*msg));
            printf("parent (pid: %d) recieved msg: %s\n", getpid(), buf);
        }
    }
    
    for (size_t i = 0; i < 2; ++i)
    {
        w = wait(&wstatus);
        if (w == -1) {
            perror("wait error");
            return 1;
        }
        if (WIFEXITED(wstatus)) {
            printf("wait pid: %d, exited, wstatus=%d\n", w, WEXITSTATUS(wstatus));
        } else if (WIFSIGNALED(wstatus)) {
            printf("wait pid: %d, killed by signal %d\n", w, WTERMSIG(wstatus));
        } else if (WIFSTOPPED(wstatus)) {
            printf("wait pid: %d, stopped by signal %d\n", w, WSTOPSIG(wstatus));
        }
    }
    memset(buf, 0, 100);
    close(fd[1]);
    read(fd[0], buf, sizeof(*msg));
    free(buf);
    return 0;
}
