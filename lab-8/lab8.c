#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

typedef struct {
    int id;
    int primos_encontrados;
} ConsumidorStats;

int *buffer;              
int M;                     
int N;                    
int buffer_count = 0;     
int proximo_numero = 1;   
int producao_finalizada = 0; 

sem_t mutex;              
sem_t vazio;              
sem_t itens;              

ConsumidorStats *stats;
int num_consumidores;
int total_primos = 0;
int total_primos_esperados = 0;  // Contagem de quantos primos existem de 1 a N
sem_t mutex_stats;        

int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

void* produtor(void* arg) {
    while (proximo_numero <= N) {
        sem_wait(&vazio);
        
        sem_wait(&mutex);
        
        buffer_count = 0;
        int inicio = proximo_numero;
        while (buffer_count < M && proximo_numero <= N) {
            buffer[buffer_count] = proximo_numero;
            buffer_count++;
            proximo_numero++;
        }
        
        printf("Produtor: preencheu buffer com %d elementos (de %d a %d)\n", 
               buffer_count, inicio, proximo_numero - 1);
        
        int items_produzidos = buffer_count;
        
        sem_post(&mutex);
        
        for (int i = 0; i < items_produzidos; i++) {
            sem_post(&itens);
        }
    }
    
    sem_wait(&mutex);
    producao_finalizada = 1;
    sem_post(&mutex);
    
    for (int i = 0; i < num_consumidores; i++) {
        sem_post(&itens);
    }
    
    printf("Produtor: finalizou produção\n");
    return NULL;
}

void* consumidor(void* arg) {
    int id = *(int*)arg;
    int numero;
    int primos_locais = 0;
    
    while (1) {
        sem_wait(&itens);
        
        sem_wait(&mutex);
        
        if (producao_finalizada && buffer_count == 0) {
            sem_post(&mutex);
            sem_post(&itens);
            break;
        }
        
        if (buffer_count > 0) {
            buffer_count--;
            numero = buffer[buffer_count];
            
            if (buffer_count == 0 && !producao_finalizada) {
                sem_post(&vazio);
            }
            
            sem_post(&mutex);
            
            if (ehPrimo(numero)) {
                primos_locais++;
                
                sem_wait(&mutex_stats);
                total_primos++;
                sem_post(&mutex_stats);
            }
            
        } else {
            sem_post(&mutex);
        }
    }
    
    sem_wait(&mutex_stats);
    stats[id].id = id;
    stats[id].primos_encontrados = primos_locais;
    sem_post(&mutex_stats);
    
    printf("Consumidor %d: encontrou %d primos\n", id, primos_locais);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Uso: %s <N> <M> <num_consumidores>\n", argv[0]);
        printf("N: quantidade total de números\n");
        printf("M: tamanho do buffer\n");
        printf("num_consumidores: número de threads consumidoras\n");
        return 1;
    }
    
    N = atoi(argv[1]);
    M = atoi(argv[2]);
    num_consumidores = atoi(argv[3]);
    
    if (N <= 0 || M <= 0 || M >= N || num_consumidores <= 0) {
        printf("Erro: parâmetros inválidos\n");
        printf("Condições: N > 0, M > 0, M << N, num_consumidores > 0\n");
        return 1;
    }
    
    // Calcula quantos primos existem de 1 a N (para verificar corretude)
    printf("Calculando quantos primos existem de 1 a %d...\n", N);
    for (int i = 1; i <= N; i++) {
        if (ehPrimo(i)) {
            total_primos_esperados++;
        }
    }
    printf("Número esperado de primos: %d\n\n", total_primos_esperados);
    
    buffer = (int*)malloc(sizeof(int) * M);
    stats = (ConsumidorStats*)malloc(sizeof(ConsumidorStats) * num_consumidores);
    
    sem_init(&mutex, 0, 1);
    sem_init(&vazio, 0, 1);
    sem_init(&itens, 0, 0);
    sem_init(&mutex_stats, 0, 1);
    
    pthread_t prod_thread;
    pthread_t *cons_threads = (pthread_t*)malloc(sizeof(pthread_t) * num_consumidores);
    int *cons_ids = (int*)malloc(sizeof(int) * num_consumidores);
    
    pthread_create(&prod_thread, NULL, produtor, NULL);
    
    for (int i = 0; i < num_consumidores; i++) {
        cons_ids[i] = i;
        pthread_create(&cons_threads[i], NULL, consumidor, &cons_ids[i]);
    }
    
    pthread_join(prod_thread, NULL);
    
    for (int i = 0; i < num_consumidores; i++) {
        pthread_join(cons_threads[i], NULL);
    }
    
    int vencedor_id = 0;
    int max_primos = stats[0].primos_encontrados;
    
    for (int i = 1; i < num_consumidores; i++) {
        if (stats[i].primos_encontrados > max_primos) {
            max_primos = stats[i].primos_encontrados;
            vencedor_id = i;
        }
    }
    
    printf("\n=== RESULTADOS ===\n");
    printf("Total de números processados: %d\n", N);
    printf("Total de primos encontrados: %d\n", total_primos);
    printf("Thread consumidora VENCEDORA: %d (encontrou %d primos)\n", 
           vencedor_id, max_primos);
    
    // Verificação de corretude
    printf("\n=== VERIFICAÇÃO DE CORRETUDE ===\n");
    printf("Primos esperados: %d\n", total_primos_esperados);
    printf("Primos encontrados: %d\n", total_primos);
    
    sem_destroy(&mutex);
    sem_destroy(&vazio);
    sem_destroy(&itens);
    sem_destroy(&mutex_stats);
    free(buffer);
    free(stats);
    free(cons_threads);
    free(cons_ids);
    
    return 0;
}
