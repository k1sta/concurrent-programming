#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void generate_random_vec(float *v, long unsigned int dim)
{
    for (long unsigned int i = 0; i < dim; i++) {
        v[i] = (float)rand() / RAND_MAX;
    }
    return;
}

void print_vec(float *v, long int dim)
{
    for (int i = 0; i < dim; i++){
        printf("v[%d] = %f\n", i, v[i]);
    }
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    FILE *file;
    char filename[50];
    float *v1, *v2;
    double r;
    long unsigned int N;
    short unsigned int amount;
    int verbose = 0;

    if (argc == 4 && strcmp(argv[3], "-v") == 0) {
        verbose = 1;
    } else if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s <amount> <N> [-v]\n", argv[0]);
        return 1;
    }

    amount = (short unsigned int)atoi(argv[1]);
    N = (long unsigned int)atoi(argv[2]);

    v1 = (float*)malloc(N * sizeof(float));
    v2 = (float*)malloc(N * sizeof(float));
    if (v1 == NULL || v2 == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed\n");
        return 2;
    }
    
    for(short unsigned int j = 0; j < amount; j++){
        r = 0.0;
        
        generate_random_vec(v1, N);
        generate_random_vec(v2, N);

        for (long unsigned int i = 0; i < N; i++) {
            r += v1[i] * v2[i];
        }

        printf("r = %f\n", r);
        sprintf(filename, "result_%hu.bin", j);
        
        file = fopen(filename, "wb");
        if (file == NULL) {
            fprintf(stderr, "ERROR: Could not open file %s for writing\n", filename);
            return 3;
        }
        
        fwrite(&N, sizeof(long unsigned int), 1, file);
        fwrite(v1, sizeof(float), N, file);
        fwrite(v2, sizeof(float), N, file);
        fwrite(&r, sizeof(double), 1, file);

        fclose(file);

        if (verbose) {
            print_vec(v1, N);
            print_vec(v2, N);
            printf("r = %f\n", r);
            printf("Result %hu written to %s\n", j, filename);
        }
    }

    free(v1);
    free(v2);
    return 0;
}