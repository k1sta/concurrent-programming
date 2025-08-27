#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // thread library

// function executed by thread
void* task(void* arg);

int main(int argc, char* argv[]){
	// thread number
	short int nthreads;
	
	// validate input
	if(argc<2){
		printf("ERROR: %s <thread-amount>\n", argv[0]);
		return 1;
	}
	nthreads = atoi(argv[1]);
	
	// thread vector
	pthread_t tid_vec[nthreads]; //malloc nao existe ne pqp

	// creating threads
	for(long int i=0; i < nthreads; i++){
		// returns != 0 on failure
		if(pthread_create(tid_vec+i, NULL, task, (void*) i))
		{
			printf("ERROR: pthread_create failed on thread no. %ld\n", i);
			return 2;
		}
	}
	
	// sync function, waits thread exit
	for (short int i=0; i < nthreads; i++){
		void* retval;
		if(pthread_join(*(tid_vec+i), &retval))
		{
			printf("ERROR: pthread_join failed on %hd iteration\n", i);
		}
		printf("Thread no. %ld ended\n", (long int) retval);
	}

	// ending
	puts("main end");
	return 0;
}

void* task(void* arg){
	// cast works because of the variable size
	long int id = (long int) arg;
	
	// hello world!
	printf("Hello, world, from thread no. %ld\n", id);
	
	// returns the thread no. 
	pthread_exit((void*) arg);
}
