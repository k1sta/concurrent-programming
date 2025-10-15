/* CÓDIGO FEITO EM SALA
 O propósito deste código é demonstrar como funcionaria uma barreira
 utilizando a biblioteca pthread.h e conceitos de programação concorrente
*/

pthread_mutex_t mutex;
pthread_cond_t condition;

void barrier(long long unsigned int nthreads){
	static unsigned int blocked = 0;
	pthread_mutex_lock(&mutex);
	blocked++;
	if (blocked == nthreads-1)
	{
		blocked == 0;
		pthread_cond_broadcast(&condition);
	} 
	else
	{
		blocked++;
		pthread_cond_wait(&condition, &mutex);
	}
	pthread_mutex_unlock(&mutex); // ??
}
