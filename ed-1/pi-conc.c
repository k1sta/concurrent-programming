#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

long int nthreads = 0;
long int n = 0;

void* calculation(void* arg)
{
	long int id = (long int) arg;
	double* pi;
	pi = (double*) malloc(sizeof(double));
	if(!pi) pthread_exit(NULL);
	*pi = 0.0;	
	for(int i = id; i < n; i+=nthreads)
	{
		*pi += (4.0/(8*i+1) - 2.0/(8*i+4) - 1.0/(8*i+5) - 1.0/(8*i+6)) * (1.0/pow(16, i));
	}
	pthread_exit((void*) pi);
}

int main (int argc, char** argv)
{
	pthread_t *tid; //identificadores das threads no sistema
	double pi = 0;
	void *result;

	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s <num-threads> <iteration-number>\n", argv[0]);
		return 1;
	}

	nthreads = atoi(argv[1]);
	n = atoi(argv[2]);
	//--aloca as estruturas
	tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
	if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   	//--cria as threads
   	for(long int t=0; t<nthreads; t++) {
     		if (pthread_create(&tid[t], NULL, calculation, (void *) t)) {
       			printf("--ERRO: pthread_create()\n"); exit(-1);
    		}
   	}

	//--espera todas as threads terminarem
	for (int t=0; t<nthreads; t++) {
		if (pthread_join(tid[t], &result)) {
				printf("--ERRO: pthread_join() \n"); exit(-1); 
		} 
		pi += *((double*) result);
	} 
	printf("Valor de pi em %ld iterações com %ld threads = %lf\n", n, nthreads, pi);

	return 0;
}
