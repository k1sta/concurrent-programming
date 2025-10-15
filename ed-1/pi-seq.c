#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double serie(int n)
{
	double pi = 0;
	for(int i = 0; i < n; i++)
	{
		pi += (4.0/(8*i+1) - 2.0/(8*i+4) - 1.0/(8*i+5) - 1.0/(8*i+6)) * (1.0	/pow(16, i));
	}
	return pi;
}

int main (int argc, char** argv)
{
	int n;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s n\n", argv[0]);
		exit(1);
	}

	n = atoi(argv[1]);

	printf("%lf\n", serie(n));

	return 0;
}
