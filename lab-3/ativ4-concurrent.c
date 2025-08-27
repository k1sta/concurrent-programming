#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

// Structure to hold data for each thread
typedef struct {
    int thread_id;
    int num_threads;
    long int N;
    float *v1;
    float *v2;
} ThreadData;

double final_dot_product = 0;
pthread_mutex_t result_mutex;

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void* concurrent_dot_product(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    long int N = data->N;
    int num_threads = data->num_threads;
    int thread_id = data->thread_id;
    double local_sum = 0.0;

    long int block_size = N / num_threads;
    long int start_index = thread_id * block_size;
    long int end_index = (thread_id == num_threads - 1) ? N : start_index + block_size;

    for (long int i = start_index; i < end_index; i++) {
        local_sum += data->v1[i] * data->v2[i];
    }


    final_dot_product += local_sum;

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int num_threads;
    char *input_file;
    long int N;
    float *v1, *v2;
    pthread_t *threads;
    ThreadData *thread_data;
    double start_time, end_time, r;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <num_threads>\n", argv[0]);
        return 1;
    }

    input_file = argv[1];
    num_threads = atoi(argv[2]);

    if (num_threads <= 0) {
        fprintf(stderr, "ERROR: Number of threads must be positive\n");
        return 1;
    }

    FILE *file = fopen(input_file, "rb");
    if (file == NULL) {
        fprintf(stderr, "ERROR: Could not open input file %s\n", input_file);
        return 1;
    }

    if (fread(&N, sizeof(long int), 1, file) != 1) {
        fprintf(stderr, "ERROR: Could not read vector dimension from file\n");
        fclose(file);
        return 1;
    }

    v1 = (float*)malloc(N * sizeof(float));
    v2 = (float*)malloc(N * sizeof(float));
    if (v1 == NULL || v2 == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed for vectors\n");
        fclose(file);
        free(v1);
        free(v2);
        return 2;
    }

    if (fread(v1, sizeof(float), N, file) != N || fread(v2, sizeof(float), N, file) != N) {
        fprintf(stderr, "ERROR: Could not read vectors from file\n");
        fclose(file);
        free(v1);
        free(v2);
        return 1;
    }

    if (fread(&r, sizeof(double), 1, file) != 1) {
        fprintf(stderr, "ERROR: Could not read result from file\n");
        fclose(file);
        free(v1);
        free(v2);
        return 1;
    }

    fclose(file);

    start_time = get_time();

    threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    thread_data = (ThreadData*)malloc(num_threads * sizeof(ThreadData));
    if (threads == NULL || thread_data == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed for threads\n");
        free(v1);
        free(v2);
        return 2;
    }

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].num_threads = num_threads;
        thread_data[i].N = N;
        thread_data[i].v1 = v1;
        thread_data[i].v2 = v2;

        if (pthread_create(&threads[i], NULL, concurrent_dot_product, &thread_data[i]) != 0) {
            fprintf(stderr, "ERROR: Failed to create thread %d\n", i);
            return 3;
        }
    }

    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "ERROR: Failed to join thread %d\n", i);
            return 4;
        }
    }

    end_time = get_time();



    printf("Dot product result: \t%f\n", final_dot_product);
    printf("Expected result: \t%f\n", r);
    printf("Relative Variation: \t%f\n", fabs(final_dot_product - r) / fabs(r));
    printf("Number of threads: \t%d\n", num_threads);
    printf("Vector dimension: \t%ld\n", N);
    printf("Total execution time: \t%.6f seconds\n", end_time - start_time);

    free(v1);
    free(v2);
    free(threads);
    free(thread_data);

    return 0;
}