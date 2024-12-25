#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int main(void)
{
    double function = 0.0;
    double x;
    int index = 1;

    printf("%d - Enter x: ", getpid());
    if (scanf("%lf", &x) != 1 || x < 0) {
        printf("ERROR: x is the positive number.\n");
        return EXIT_FAILURE;
    }
    while (x >= 0) {
        function += x / index;
        index++;
        printf("%d - Enter x: ", getpid());
        if (scanf("%lf", &x) != 1) {
            printf("ERROR: x is the number.\n");
            return EXIT_FAILURE;
        }
    }
    function = sqrt(function);

    printf("%d - Function g(x): %.6lf\n",  getpid(), function);

    return EXIT_SUCCESS; 
}
