#include <stdlib.h>
#include <stdio.h>

int main(int argc, char*argv[])
{
	int n;

	if(sscanf(argv[1], "%d", &n)!=1) return EXIT_FAILURE;
	printf("%d", n*n);

	return EXIT_SUCCESS;
}