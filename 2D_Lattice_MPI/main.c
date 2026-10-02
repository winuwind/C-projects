#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

void matrix_multiply(int* C, int n1, int n2, int n3, int p1, int p2) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int dims[2] = {p1, p2};
    int periods[2] = {0, 0};
    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart_comm);

    int coords[2];
    MPI_Cart_coords(cart_comm, rank, 2, coords);

    int row_id = coords[0];
    MPI_Comm row_comm;
    MPI_Comm_split(cart_comm, row_id, rank, &row_comm);

    int column_id = coords[1];
    MPI_Comm column_comm;
    MPI_Comm_split(cart_comm, column_id, rank, &column_comm);

    int new_rank, new_size;
    MPI_Comm_rank(column_comm, &new_rank);
    MPI_Comm_size(column_comm, &new_size);

    int *A = NULL;
    int *B = NULL;

    int block_n1 = n1 / p1;
    int block_n3 = n3 / p2;

    int *local_A = (int *) malloc(block_n1 * n2 * sizeof(int));
    int *local_B = (int *) malloc(n2 * block_n3 * sizeof(int));
    int *local_C = (int *) malloc(block_n1 * block_n3 * sizeof(int));

    if (rank == 0) {
        A = (int *) malloc(n1 * n2 * sizeof(int));
        B = (int *) malloc(n2 * n3 * sizeof(int));
        for (int i = 0; i < n1; i++) {
            for (int j = 0; j < n2; j++) {
                A[i * n2 + j] = i + j;
            }
        }
        //При передаче матрицу B надо транспонировать чтобы можно было легко отдать столбцы данных
//        for (int i = 0; i < n3; i++) {
//            for (int j = 0; j < n2; j++) {
//                B[i * n2 + j] = j - i;
//            }
//        }

        for (int i = 0; i < n2; i++) {
            for (int j = 0; j < n3; j++) {
                B[i * n3 + j] = i - j;
            }
        }

    }
    MPI_Datatype column_type, column_type_resized;
    MPI_Type_vector(n2, block_n3, n3, MPI_INT, &column_type);
    MPI_Type_create_resized(column_type, 0, block_n3 * sizeof(int), &column_type_resized);
    MPI_Type_free(&column_type);
    MPI_Type_commit(&column_type_resized);

    if (column_id == 0) {
        MPI_Scatter(A, block_n1 * n2, MPI_INT, local_A, block_n1 * n2, MPI_INT, 0, column_comm);
    }

    if (row_id == 0) {
//        MPI_Scatter(B, n2 * block_n3, MPI_INT, local_B, n2 * block_n3, MPI_INT, 0, row_comm);
        MPI_Scatter(B, 1, column_type_resized, local_B, n2 * block_n3, MPI_INT, 0, row_comm);
    }

    MPI_Type_free(&column_type_resized);

    MPI_Bcast(local_A, block_n1 * n2, MPI_INT, 0, row_comm);
    MPI_Bcast(local_B, n2 * block_n3, MPI_INT, 0, column_comm);

    for (int i = 0; i < block_n1; i++) {
        for (int j = 0; j < block_n3; j++) {
            local_C[i * block_n3 + j] = 0;
            for (int k = 0; k < n2; k++) {
//                local_C[i * block_n3 + j] += local_A[i * n2 + k] * local_B[j * n2 + k];
                local_C[i * block_n3 + j] += local_A[i * n2 + k] * local_B[k * block_n3 + j];
            }
        }
    }
    int* C_ = NULL;
    if (column_id == 0) {
        C_ = (int *) malloc(block_n1 * n3 * sizeof(int));
    }
    for (int j = 0; j < block_n1; j++) {
        MPI_Gather(&local_C[block_n3 * j], block_n3, MPI_INT, &C_[n3 * j], block_n3, MPI_INT, 0, row_comm);
    }
    if (column_id == 0) {
        MPI_Gather(C_, block_n1 * n3, MPI_INT, C, block_n1 * n3, MPI_INT, 0, column_comm);
        free(C_);
    }

    free(local_A);
    free(local_B);
    free(local_C);

    if (rank == 0) {
        free(A);
        free(B);
    }
}

int main(int argc, char** argv) {
    int n1 = 4, n2 = 4, n3 = 4;
    int p1 = 2, p2 = 2;

    for (int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-n1") == 0){
            n1 = strtoull(argv[++i], NULL, 10);
        }
        else if(strcmp(argv[i], "-n2") == 0){
            n2 = strtoull(argv[++i], NULL, 10);
        }
        else if(strcmp(argv[i], "-n3") == 0){
            n3 = strtoull(argv[++i], NULL, 10);
        }
        else if(strcmp(argv[i], "-p1") == 0){
            p1 = strtoull(argv[++i], NULL, 10);
        }
        else if(strcmp(argv[i], "-p2") == 0){
            p2 = strtoull(argv[++i], NULL, 10);
        }
    }

    MPI_Init(&argc, &argv);

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(p1 * p2 > size){
        if(rank == 0) {
            printf("p1 * p2 must be not bigger than world_size");
        }
        MPI_Finalize();
        return 0;
    }

    if(n1 % p1 != 0 || n3 % p2 != 0){
        if(rank == 0) {
            printf("n1 %% p1 and n3 %% p2 must be 0");
        }
        MPI_Finalize();
        return 0;
    }


    int *C = NULL;

    if(rank == 0) {
        C = (int *) malloc(n1 * n3 * sizeof(int));
    }
    double start_time = MPI_Wtime();
    // Запуск умножения матриц
    matrix_multiply(C, n1, n2, n3, p1, p2);
    double end_time = MPI_Wtime();

    // Результат на процессе 0
    if (rank == 0) {
//        printf("Result matrix C:\n");
//        for (int i = 0; i < n1; i++) {
//            for (int j = 0; j < n3; j++) {
//                printf("%d ", C[i * n3 + j]);
//            }
//            printf("\n");
//        }
        printf("Elapsed time = %F\n", end_time - start_time);
        // Очистка памяти
        free(C);
    }

    MPI_Finalize();
    return 0;
}
