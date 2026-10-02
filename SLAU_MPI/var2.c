#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void AddVector(double *vector_a, double scalar_a, double *vector_b, double scalar_b, double *result, int N) {
    for (int i = 0; i < N; i++) {
        result[i] = vector_a[i] * scalar_a + vector_b[i] * scalar_b;
    }
}

double MultipleVector(double *vector_a, double *vector_b, int N) {
    double k = 0;
    for (int i = 0; i < N; i++) {
        k += vector_a[i] * vector_b[i];
    }
    return k;
}

void MultipleMatrix(double *matrix, double *vector, double *result, int N, int size, int len) {
    for (int i = 0; i < size; i++) {
        result[i] = MultipleVector(&matrix[i * len], vector, N);
    }
}

void MethodGrad(double *matrix, double *result, double *solution, int N) {
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int size = (N + world_size - 1) / world_size;
    double *r = (double *) malloc(sizeof(double) * size);
    double *z = (double *) malloc(sizeof(double) * size);
    double *helper = (double *) malloc(sizeof(double) * size);
    double *helper_ = (double *) malloc(sizeof(double) * size);
    double alpha, beta, gamma;
    double E = 10e-5;
    double alpha_arr[world_size];
    double beta_arr[world_size];
    double gamma_arr[world_size];
    double b_norm = MultipleVector(result, result, size);
    double b_norm_arr[size];
    for (int i = 1; i < world_size; i++) {
        if (i == world_rank) {
            MPI_Send(&b_norm, 1, MPI_DOUBLE, 0, i, MPI_COMM_WORLD);
        }
    }
    if (world_rank == 0) {
        for (int i = 1; i < world_size; i++) {
            MPI_Recv(&b_norm_arr[i], 1, MPI_DOUBLE, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            b_norm += b_norm_arr[i];
        }
    }
    MPI_Bcast(&b_norm, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    for (int i = 0; i < size; i++) {
        r[i] = 0.0;
    }
    AddVector(r, -1, result, 1, r, size);
    for (int i = 0; i < size; i++) {
        z[i] = r[i];
    }
    size_t count = 1000000000000000000;
    do {
        MPI_Barrier(MPI_COMM_WORLD);
        for (int j = 0; j < size; j++) {
            helper_[j] = 0;
        }
        for (int i = 0; i < world_size; i++) {
            MultipleMatrix(&matrix[((world_rank - i + world_size) % world_size) * size], z, helper, size, size, N);
            for (int j = 0; j < size; j++) {
                helper_[j] += helper[j];
            }
            MPI_Send(z, size, MPI_DOUBLE, (world_rank + 1) % world_size, (world_rank + 1) % world_size, MPI_COMM_WORLD);
            MPI_Recv(z, size, MPI_DOUBLE, (world_rank  + world_size - 1) % world_size, world_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        alpha = MultipleVector(r, r, size);
        gamma = MultipleVector(helper_, z, size);
        for (int i = 1; i < world_size; i++) {
            if (i == world_rank) {
                MPI_Send(&alpha, 1, MPI_DOUBLE, 0, i, MPI_COMM_WORLD);
                MPI_Send(&gamma, 1, MPI_DOUBLE, 0, i + world_size, MPI_COMM_WORLD);
            }
        }
        if (world_rank == 0) {
            for (int i = 1; i < world_size; i++) {
                MPI_Recv(&alpha_arr[i], 1, MPI_DOUBLE, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&gamma_arr[i], 1, MPI_DOUBLE, i, i + world_size, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                alpha += alpha_arr[i];
                gamma += gamma_arr[i];
            }
        }
        MPI_Bcast(&alpha, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&gamma, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        AddVector(solution, 1, z, alpha / gamma, solution, size);
        AddVector(r, 1, helper_, -1.0 * alpha / gamma, r, size);
        beta = MultipleVector(r, r, size);
        MPI_Barrier(MPI_COMM_WORLD);
        for (int i = 1; i < world_size; i++) {
            if (i == world_rank) {
                MPI_Send(&beta, 1, MPI_DOUBLE, 0, i, MPI_COMM_WORLD);
            }
        }
        if (world_rank == 0) {
            for (int i = 1; i < world_size; i++) {
                MPI_Recv(&beta_arr[i], 1, MPI_DOUBLE, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                beta += beta_arr[i];
            }
        }
        MPI_Bcast(&beta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        AddVector(r, 1, z, beta / alpha, z, size);
        if (alpha == beta) {
            break;
        }
    } while (beta / b_norm >= E && count--);

    free(helper_);
    free(helper);
    free(z);
    free(r);
}


int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int Nx;
    int Ny;
    if (argc == 3) {
        Nx = atoi(argv[1]);
        Ny = atoi(argv[2]);
    } else {
        Nx = 100;
        Ny = 100;
    }
    int N = Nx * Ny;
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int size = (N + world_size - 1) / world_size;
    double *matrix = (double *) malloc(sizeof(double) * (size * world_size) * size);
    double *b = (double *) malloc(sizeof(double) * size);
    double *x = (double *) malloc(sizeof(double) * size);
    for (int i = 0; i < world_rank * size; i++) {
        int z = rand();
    }
    if (world_rank + 1 != world_size) {
        for (int i = 0; i < size; i++) {
            b[i] = rand() % 101 - 50;
            x[i] = 0;
            for (int j = 0; j < N; j++) {
                if (i + world_rank * size == j) {
                    matrix[i * world_size * size + j] = -4;
                } else if (abs(i + world_rank * size - j) == 1 || abs(i + world_rank * size - j) == Nx) {
                    matrix[i * world_size * size + j] = 1;
                } else {
                    matrix[i * world_size * size + j] = 0;
                }
            }
            for(int j = N; j < size * world_size; j++){
                matrix[i * world_size * size + j] = 0;
            }
        }
    } else {
        for (int i = 0; i < size; i++) {
            if (i < N - world_rank * size) {
                b[i] = rand() % 101 - 50;
                x[i] = 0;
                for (int j = 0; j < N; j++) {
                    if (i + world_rank * size == j) {
                        matrix[i * world_size * size + j] = -4;
                    } else if (abs(i + world_rank * size - j) == 1 || abs(i + world_rank * size - j) == Nx) {
                        matrix[i * world_size * size + j] = 1;
                    } else {
                        matrix[i * world_size * size + j] = 0;
                    }
                }
                for(int j = N; j < size * world_size; j++){
                    matrix[i * world_size * size + j] = 0;
                }
            } else {
                b[i] = 0;
                x[i] = 0;
                for (int j = 0; j < size * world_size; j++) {
                    matrix[i * world_size * size + j] = 0;
                }
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    clock_t start = clock();
    MethodGrad(matrix, b, x, N);
    clock_t end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    for (int i = 0; i < world_size; i++) {
        if (world_rank == i) {
            FILE *fp;
            if (i == 0) {
                fp = fopen("result.dat", "wb");
            } else {
                fp = fopen("result.dat", "ab");
            }
            if (world_rank + 1 == world_size) {
                fwrite(x, sizeof(double), N - world_rank * size, fp);
            } else {
                fwrite(x, sizeof(double), size, fp);
            }
            fclose(fp);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    if(world_rank != 0){
        MPI_Send(&seconds, 1, MPI_FLOAT, 0, world_rank, MPI_COMM_WORLD);
    }
    else{
        float help_sec[world_size];
        for(int i = 1; i < world_size; i++){
            MPI_Recv(&help_sec[i], 1, MPI_FLOAT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            seconds += help_sec[i];
        }
        char name[256];
        sprintf(name, "time_var2_%d.txt", world_size);
        FILE *fp = fopen(name, "w");
        fprintf(fp, "Time in methodGrad(): %f\n", seconds / (double) world_size);
        fclose(fp);
        printf("Time in methodGrad(): %f\n", seconds / (double) world_size);
    }
    free(x);
    free(b);
    free(matrix);
    MPI_Finalize();
    return 0;
}