#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<stdbool.h>

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
	
	printf("Enter the length of massive: ");
	if (scanf("%zu", &length) != 1 || length > N || length < 1) {
		printf("Error: wrong length\n");
		return EXIT_FAILURE;
	}
	
	printf("Enter array elements: ");
	if (!array_input(array, array + length)) {
		printf("Error: incorrect data.\n");
		return EXIT_FAILURE;
	}
	printf("Result: %d\n", calculate_expression(array, array + length));
    
	return EXIT_SUCCESS;
}
