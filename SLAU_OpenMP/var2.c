#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

int size_block = 10;

void AddVector(double *vector_a, double scalar_a, double *vector_b, double scalar_b, double *result, int N) {
#pragma omp for schedule(dynamic, size_block)
    for (int i = 0; i < N; i++) {
        result[i] = vector_a[i] * scalar_a + vector_b[i] * scalar_b;
    }
}

double MultipleVector(double *vector_a, double *vector_b, int N, double* k) {
#pragma omp for schedule(dynamic, size_block)
    for (int i = 0; i < N; i++) {
        *k += vector_a[i] * vector_b[i];
    }
    return *k;
}

void MultipleMatrix(double *matrix, double *vector, double *result, int N) {
#pragma omp for schedule(dynamic, size_block)
    for (int i = 0; i < N; i++) {
        double x = 0.0;
        for (int j = 0; j < N; j++) {
            x += matrix[i * N + j] * vector[j];
        }
        result[i] = x;
    }
}

void MethodGrad(double *matrix, double *result, double *solution, int N) {
    double *r = (double *) malloc(sizeof(double) * N);
    double *z = (double *) malloc(sizeof(double) * N);
    double *helper = (double *) malloc(sizeof(double) * N);
    double E = 1e-5;
    double alpha, beta, gamma, b_norm;
    MultipleMatrix(matrix, solution, r, N);
    AddVector(r, -1, result, 1, r, N);
#pragma omp parallel for schedule(dynamic, size_block)
    for (int j = 0; j < N; j++) {
        z[j] = r[j];
    }
    double k = 0.0;
#pragma omp parallel reduction(+:k)
    {
        b_norm = MultipleVector(result, result, N, &k);
        do {
            MultipleMatrix(matrix, z, helper, N);
            k = 0.0;
            alpha = MultipleVector(r, r, N, &k);
            k = 0.0;
            gamma = MultipleVector(helper, z, N, &k);
            AddVector(solution, 1, z, alpha / gamma, solution, N);
            AddVector(r, 1, helper, -1.0 * alpha / gamma, r, N);
            k = 0.0;
            beta = MultipleVector(r, r, N, &k);
            AddVector(r, 1, z, beta / alpha, z, N);
        } while (beta / b_norm >= E);
    }
    free(helper);
    free(z);
    free(r);
}

int main(int argc, char *argv[]) {
    int Nx = 60;
    int Ny = 60;
    int num_threads = 4;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-x") == 0) {
            Nx = strtol(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "-y") == 0) {
            Ny = strtol(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "-n") == 0) {
            num_threads = strtol(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "-s") == 0) {
            size_block = strtol(argv[++i], NULL, 10);
        }
    }
    int N = Nx * Ny;
    omp_set_num_threads(num_threads);
    double *matrix = (double *) malloc(sizeof(double) * N * N);
    double *b = (double *) malloc(sizeof(double) * N);
    double *x = (double *) malloc(sizeof(double) * N);
    for (int i = 0; i < N; i++) {
        b[i] = rand() % 101 - 50;
        x[i] = 0;
        for (int j = 0; j < N; j++) {
            if (i == j) {
                matrix[i * N + j] = -4;
            } else if (abs(i - j) == 1 || abs(i - j) == Nx) {
                matrix[i * N + j] = 1;
            } else {
                matrix[i * N + j] = 0;
            }
        }
    }
    double start = omp_get_wtime();
    MethodGrad(matrix, b, x, N);
    double end = omp_get_wtime();
    printf("Time in MethodGrad (var2 with %d threads): %F; shedule(dynamic, %d)\n", num_threads, end - start, size_block);
    FILE *fp = fopen("result.dat", "wb");
    fwrite(x, sizeof(double), N, fp);
    fclose(fp);
    free(x);
    free(b);
    free(matrix);
    return 0;
}
