#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NMAX 1024
#define N 10

int max(const int a, const int b)
{
	if (a > b)
		return a;
	return b;
}

int calculate_expression(const int *pb, const int *pe)
{
	pe--;
	int result = *pb + *pe;
	while (pb <= pe) {
		result = max(result, *pb + *pe);
		pb++;
		pe--;
	}
	return result;
}

bool array_input(int *pb, int *pe)
{
	for (int *pcur = pb; pcur < pe; pcur++)
		if (scanf("%d", pcur) != 1)
			return false;
	return true;
}

int main(void)
{
	int array[NMAX];
	size_t length;
	
	printf("%d - Enter the size of massive: ", getpid());
	if (scanf("%zu", &length) != 1 || length > N || length < 1) {
		printf("Error: wrong size\n");
		return EXIT_FAILURE;
	}
	
	printf("%d - Enter array elements: ", getpid());
	if (!array_input(array, array + length)) {
		printf("Error: incorrect data.\n");
		return EXIT_FAILURE;
	}
	printf("%d - Result: %d\n",  getpid(), calculate_expression(array, array + length));
    
	return EXIT_SUCCESS;
}
