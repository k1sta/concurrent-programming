#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

int* vetor; // vetor global para acesso das threads

typedef struct{
	short int id; // id da thread na aplicacao (diferente do id do sistema)
	short int nthreads; // qtde de threads
	long int dim; // dimensao do vetor
} t_args;

// função task da thread
void* f (void* args){
	t_args *arg = (t_args*) args;
	
	long int fatia, ini, fim;
	fatia = arg->dim / arg->nthreads; //qtde de elementos que a thread vai processar
	ini = arg->id * fatia;
	fim = ini + fatia;
	
	if(arg->id == arg->nthreads-1) fim = arg->dim;
	
	for(long int i = ini; i<fim; i++)
		*(vetor+i) += 1;

	free(args);
	pthread_exit(NULL);
}

// função para alocar espaço e inicializar o vetor
short int inicializa_vetor(int** vet, long int dim){
	*vet = (int*) malloc (sizeof(int) * dim);
	if(!vet){
		fprintf(stderr, "ERRO de alocação de memória\n");
		return 1;
	}
	
	for(long int i = 0; i < dim; i++){
		*(*vet+i) = (int) i;
	}

	return 0;
}

// função que exibe o vetor
void checa_vetor(int *vet, long int dim){
	for(long int i = 0; i < dim; i++){
		if(*(vet+i) != i+1){
			puts("VETOR ERRADO");
			return;
		}
	}
	puts("tudo ok!");
	return;
}

int main(int argc, char** argv){
	short int nthreads; // num de threads
	long int dim; // dimensao do vetor
	
	struct timeval tv;
	double inicio_t, fim_t;

	// validacao dos argumentos do programa
	if(argc<3) {
		fprintf(stderr, "ERRO de entrada, digite: %s <dimensão do vetor> <qntde de threads>\n", argv[0]);
		return 1;
	}	

	// coleta as variáveis inputadas
	dim = atoi(argv[1]);
	nthreads = atoi(argv[2]);
	pthread_t tid[nthreads];
	
	if(inicializa_vetor(&vetor, dim)){
		fprintf(stderr, "ERRO de inicialização\n");
		return 2;
	}
	
	gettimeofday(&tv, NULL);
	inicio_t = tv.tv_sec + tv.tv_usec/1000000.0;

	for(short int i = 0; i < nthreads; i++) {
		t_args* args = (t_args*) malloc(sizeof(t_args));
		args->id = i;
		args->nthreads = nthreads;
		args->dim = dim;
		if(!args){
			fprintf(stderr, "ERRO na criação do t_args\n");
			return 3;
		}
		if(pthread_create(&tid[i], NULL, f, (void *) args)){
			fprintf(stderr, "ERRO na criação da thread %hd\n", i);
			return 4;
		}
	}

	for(short int i = 0; i<nthreads; i++){
		if(pthread_join(tid[i], NULL)){
			fprintf(stderr, "ERRO na coleta do retorno da thread %hd", i);
			return 5;
		}
	}

	gettimeofday(&tv, NULL);
        fim_t = tv.tv_sec + tv.tv_usec/1000000.0;

	checa_vetor(vetor, dim);

	printf("Tempo de operação no vetor: %lf\n", fim_t - inicio_t);
	return 0;
}
