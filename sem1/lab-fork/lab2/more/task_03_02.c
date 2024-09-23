#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int main(void)
{
    double function = 0.0;
    double x;
    int index = 1;

    printf("Enter x: ");
    if (scanf("%lf", &x) != 1 || x < 0) {
        printf("ERROR: x is the positive number.\n");
        return EXIT_FAILURE;
    }
    while (x >= 0) {
        function += x / index;
        index++;
        printf("Enter x: ");
        if (scanf("%lf", &x) != 1) {
            printf("ERROR: x is the number.\n");
            return EXIT_FAILURE;
        }
    }
    function = sqrt(function);

    printf("Function g(x): %.6lf\n", function);

    return EXIT_SUCCESS; 
}
