#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int childpid;
    if ((childpid = fork()) == -1) {
        perror("Can't fork\n"); 
        exit(1);
    } else if (childpid == 0) {
        while(1) printf(" %d", getpid());
    } else {
        while(1) printf(" %d", getpid());
    }

    exit(0);
}