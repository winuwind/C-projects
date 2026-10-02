#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <string.h>

int Nx = 16, Ny = 16, Nz = 16;
double Dx = 0.5, Dy = 0.5, Dz = 0.5;
double X0 = 0, Y0 = 0, Z0 = 0;
double hx = 2, hy = 2, hz = 2;
double hx2 = 4, hy2 = 4, hz2 = 4;
int size_x, size_y, size_z;
int px = 2, py = 2, pz = 1;
double a = 1e5;
double e = 1e-5;

double X(int i) {
    return X0 + (double) i * hx;
}

double Y(int i) {
    return Y0 + (double) i * hy;
}

double Z(int i) {
    return Z0 + (double) i * hz;
}

int phi_index(int x, int y, int z) {
    return x * size_y * size_z + y * size_z + z;
}

double ro(double *phi, int x, int y, int z) {
    return 6 - a * phi[phi_index(x, y, z)];
}

void pullBorder(double *phi, MPI_Comm cart_comm) {
    int rank, size;
    int coords[3];
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Cart_coords(cart_comm, rank, 3, coords);

    if (coords[0] == 0) {
        for (int y = 0; y < size_y; y++) {
            for (int z = 0; z < size_z; z++) {
                phi[phi_index(0, y, z)] = 1 + Y(y + coords[1] * (size_y - 1)) * Y(y + coords[1] * (size_y - 1)) +
                                          Z(z + coords[2] * (size_z - 1)) * Z(z + coords[2] * (size_z - 1));
            }
        }
    }
    if (coords[0] == px - 1) {
        for (int y = 0; y < size_y; y++) {
            for (int z = 0; z < size_z; z++) {
                phi[phi_index(size_x - 1, y, z)] =
                        1 + Y(y + coords[1] * (size_y - 1)) * Y(y + coords[1] * (size_y - 1)) +
                        Z(z + coords[2] * (size_z - 1)) * Z(z + coords[2] * (size_z - 1));
            }
        }
    }

    if (coords[1] == 0) {
        for (int x = 0; x < size_x; x++) {
            for (int z = 0; z < size_z; z++) {
                phi[phi_index(x, 0, z)] = 1 + X(x + coords[0] * (size_x - 1)) * X(x + coords[0] * (size_x - 1)) +
                                          Z(z + coords[2] * (size_z - 1)) * Z(z + coords[2] * (size_z - 1));
            }
        }
    }
    if (coords[1] == py - 1) {
        for (int x = 0; x < size_x; x++) {
            for (int z = 0; z < size_z; z++) {
                phi[phi_index(x, size_y - 1, z)] =
                        1 + X(x + coords[0] * (size_x - 1)) * X(x + coords[0] * (size_x - 1)) +
                        Z(z + coords[2] * (size_z - 1)) * Z(z + coords[2] * (size_z - 1));
            }
        }
    }

    if (coords[2] == 0) {
        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                phi[phi_index(x, y, 0)] = 1 + Y(y + coords[1] * (size_y - 1)) * Y(y + coords[1] * (size_y - 1)) +
                                          X(x + coords[0] * (size_x - 1)) * X(x + coords[0] * (size_x - 1));
            }
        }
    }
    if (coords[2] == pz - 1) {
        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                phi[phi_index(x, y, size_z - 1)] =
                        1 + Y(y + coords[1] * (size_y - 1)) * Y(y + coords[1] * (size_y - 1)) +
                        X(x + coords[0] * (size_x - 1)) * X(x + coords[0] * (size_x - 1));
            }
        }
    }
}

void swap(double **b1, double **b2) {
    double *c = *b1;
    *b1 = *b2;
    *b2 = c;
}

void method_Jacoby(double *phi, double *phi_help, MPI_Comm cart_comm) {
    double max_delta;
    double d_phi_x;
    double d_phi_y;
    double d_phi_z;
    double new_value;

    double *border_x_start = (double *) malloc(sizeof(double) * size_y * size_z);
    double *border_x_end = (double *) malloc(sizeof(double) * size_y * size_z);
    double *border_y_start = (double *) malloc(sizeof(double) * size_x * size_z);
    double *border_y_end = (double *) malloc(sizeof(double) * size_x * size_z);
    double *border_z_start = (double *) malloc(sizeof(double) * size_y * size_x);
    double *border_z_end = (double *) malloc(sizeof(double) * size_y * size_x);

    double *border_x_start_own = (double *) malloc(sizeof(double) * size_y * size_z);
    double *border_x_end_own = (double *) malloc(sizeof(double) * size_y * size_z);
    double *border_y_start_own = (double *) malloc(sizeof(double) * size_x * size_z);
    double *border_y_end_own = (double *) malloc(sizeof(double) * size_x * size_z);
    double *border_z_start_own = (double *) malloc(sizeof(double) * size_y * size_x);
    double *border_z_end_own = (double *) malloc(sizeof(double) * size_y * size_x);

    int rank, size;
    int coords[3];
    int x_prev, x_next, y_prev, y_next, z_prev, z_next;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Cart_coords(cart_comm, rank, 3, coords);
    MPI_Cart_shift(cart_comm, 0, 1, &x_prev, &x_next);
    MPI_Cart_shift(cart_comm, 1, 1, &y_prev, &y_next);
    MPI_Cart_shift(cart_comm, 2, 1, &z_prev, &z_next);
    MPI_Request request_s[6], request_r[6];

    double global_max_delta;
    do {
        max_delta = 0.0;
        for (int y = 0; y < size_y; y++) {
            for (int z = 0; z < size_z; z++) {
                border_x_start_own[y * size_z + z] = phi[phi_index(0, y, z)];
                border_x_end_own[y * size_z + z] = phi[phi_index(size_x - 1, y, z)];
            }
        }
        for (int x = 0; x < size_x; x++) {
            for (int z = 0; z < size_z; z++) {
                border_y_start_own[x * size_z + z] = phi[phi_index(x, 0, z)];
                border_y_end_own[x * size_z + z] = phi[phi_index(x, size_y - 1, z)];
            }
        }
        for (int x = 0; x < size_x; x++) {
            for (int y = 0; y < size_y; y++) {
                border_z_start_own[x * size_y + y] = phi[phi_index(x, y, 0)];
                border_z_end_own[x * size_y + y] = phi[phi_index(x, y, size_z - 1)];
            }
        }

        MPI_Isend(border_x_start_own, size_y * size_z, MPI_DOUBLE, x_prev, x_prev * 6, MPI_COMM_WORLD, request_s);
        MPI_Isend(border_x_end_own, size_y * size_z, MPI_DOUBLE, x_next, x_next * 6 + 1, MPI_COMM_WORLD, &request_s[1]);
        MPI_Isend(border_y_start_own, size_x * size_z, MPI_DOUBLE, y_prev, y_prev * 6 + 2, MPI_COMM_WORLD,
                  &request_s[2]);
        MPI_Isend(border_y_end_own, size_x * size_z, MPI_DOUBLE, y_next, y_next * 6 + 3, MPI_COMM_WORLD, &request_s[3]);
        MPI_Isend(border_z_start_own, size_y * size_x, MPI_DOUBLE, z_prev, z_prev * 6 + 4, MPI_COMM_WORLD,
                  &request_s[4]);
        MPI_Isend(border_z_end_own, size_y * size_x, MPI_DOUBLE, z_next, z_next * 6 + 5, MPI_COMM_WORLD, &request_s[5]);

        MPI_Irecv(border_x_end, size_y * size_z, MPI_DOUBLE, x_next, rank * 6, MPI_COMM_WORLD, &request_r[0]);
        MPI_Irecv(border_x_start, size_y * size_z, MPI_DOUBLE, x_prev, rank * 6 + 1, MPI_COMM_WORLD, &request_r[1]);
        MPI_Irecv(border_y_end, size_x * size_z, MPI_DOUBLE, y_next, rank * 6 + 2, MPI_COMM_WORLD, &request_r[2]);
        MPI_Irecv(border_y_start, size_x * size_z, MPI_DOUBLE, y_prev, rank * 6 + 3, MPI_COMM_WORLD, &request_r[3]);
        MPI_Irecv(border_z_end, size_y * size_x, MPI_DOUBLE, z_next, rank * 6 + 4, MPI_COMM_WORLD, &request_r[4]);
        MPI_Irecv(border_z_start, size_y * size_x, MPI_DOUBLE, z_prev, rank * 6 + 5, MPI_COMM_WORLD, &request_r[5]);

        for (int x = 1; x < size_x - 1; x++) {
            for (int y = 1; y < size_y - 1; y++) {
                for (int z = 1; z < size_z - 1; z++) {
                    d_phi_x = (phi[phi_index(x + 1, y, z)] + phi[phi_index(x - 1, y, z)]) / hx2;
                    d_phi_y = (phi[phi_index(x, y + 1, z)] + phi[phi_index(x, y - 1, z)]) / hy2;
                    d_phi_z = (phi[phi_index(x, y, z + 1)] + phi[phi_index(x, y, z - 1)]) / hz2;
                    new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, x, y, z)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
                    phi_help[phi_index(x, y, z)] = new_value;
                }
            }
        }

        MPI_Wait(&request_s[0], MPI_STATUS_IGNORE);
        MPI_Wait(&request_s[1], MPI_STATUS_IGNORE);
        MPI_Wait(&request_s[2], MPI_STATUS_IGNORE);
        MPI_Wait(&request_s[3], MPI_STATUS_IGNORE);
        MPI_Wait(&request_s[4], MPI_STATUS_IGNORE);
        MPI_Wait(&request_s[5], MPI_STATUS_IGNORE);
        MPI_Wait(&request_r[0], MPI_STATUS_IGNORE);
        MPI_Wait(&request_r[1], MPI_STATUS_IGNORE);
        MPI_Wait(&request_r[2], MPI_STATUS_IGNORE);
        MPI_Wait(&request_r[3], MPI_STATUS_IGNORE);
        MPI_Wait(&request_r[4], MPI_STATUS_IGNORE);
        MPI_Wait(&request_r[5], MPI_STATUS_IGNORE);


        //0,0,0
        d_phi_x = (phi[phi_index(1, 0, 0)] + border_x_start[0]) / hx2;
        d_phi_y = (phi[phi_index(0, 1, 0)] + border_y_start[0]) / hy2;
        d_phi_z = (phi[phi_index(0, 0, 1)] + border_z_start[0]) / hz2;
        new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, 0, 0, 0)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
        phi_help[phi_index(0, 0, 0)] = new_value;
        //0,0,1
        d_phi_x = (phi[phi_index(1, 0, size_z - 1)] + border_x_start[size_z - 1]) / hx2;
        d_phi_y = (phi[phi_index(0, 1, size_z - 1)] + border_y_start[size_z - 1]) / hy2;
        d_phi_z = (phi[phi_index(0, 0, size_z - 2)] + border_z_end[0]) / hz2;
        new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, 0, 0, size_z - 1)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
        phi_help[phi_index(0, 0, size_z - 1)] = new_value;
        //0,1,0
        d_phi_x = (phi[phi_index(1, size_y - 1, 0)] + border_x_start[(size_y - 1) * size_z]) / hx2;
        d_phi_y = (phi[phi_index(0, size_y - 2, 0)] + border_y_end[0]) / hy2;
        d_phi_z = (phi[phi_index(0, size_y - 1, 1)] + border_z_start[size_y - 1]) / hz2;
        new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, 0, size_y - 1, 0)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
        phi_help[phi_index(0, size_y - 1, 0)] = new_value;
        //0,1,1
        d_phi_x =
                (phi[phi_index(1, size_y - 1, size_z - 1)] + border_x_start[(size_y - 1) * size_z + size_z - 1]) / hx2;
        d_phi_y = (phi[phi_index(0, size_y - 2, size_z - 1)] + border_y_end[size_z - 1]) / hy2;
        d_phi_z = (phi[phi_index(0, size_y - 1, size_z - 2)] + border_z_end[size_y - 1]) / hz2;
        new_value =
                (d_phi_x + d_phi_y + d_phi_z - ro(phi, 0, size_y - 1, size_z - 1)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
        phi_help[phi_index(0, size_y - 1, size_z - 1)] = new_value;
        //1,0,0
        d_phi_x = (phi[phi_index(size_x - 2, 0, 0)] + border_x_end[0]) / hx2;
        d_phi_y = (phi[phi_index(size_x - 1, 1, 0)] + border_y_start[(size_x - 1) * size_z]) / hy2;
        d_phi_z = (phi[phi_index(size_x - 1, 0, 1)] + border_z_start[(size_x - 1) * size_y]) / hz2;
        new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, size_x - 1, 0, 0)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
        phi_help[phi_index(size_x - 1, 0, 0)] = new_value;
        //1,0,1
        d_phi_x = (phi[phi_index(size_x - 2, 0, size_z - 1)] + border_x_end[size_z - 1]) / hx2;
        d_phi_y =
                (phi[phi_index(size_x - 1, 1, size_z - 1)] + border_y_start[(size_x - 1) * size_z + size_z - 1]) / hy2;
        d_phi_z = (phi[phi_index(size_x - 1, 0, size_z - 2)] + border_z_end[(size_x - 1) * size_y]) / hz2;
        new_value =
                (d_phi_x + d_phi_y + d_phi_z - ro(phi, size_x - 1, 0, size_z - 1)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
        phi_help[phi_index(size_x - 1, 0, size_z - 1)] = new_value;
        //1,1,0
        d_phi_x = (phi[phi_index(size_x - 2, size_y - 1, 0)] + border_x_end[(size_y - 1) * size_z]) / hx2;
        d_phi_y = (phi[phi_index(size_x - 1, size_y - 2, 0)] + border_y_end[(size_x - 1) * size_z]) / hy2;
        d_phi_z =
                (phi[phi_index(size_x - 1, size_y - 1, 1)] + border_z_start[(size_x - 1) * size_y + size_y - 1]) / hz2;
        new_value =
                (d_phi_x + d_phi_y + d_phi_z - ro(phi, size_x - 1, size_y - 1, 0)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
        phi_help[phi_index(size_x - 1, size_y - 1, 0)] = new_value;
        //1,1,1
        d_phi_x = (phi[phi_index(size_x - 2, size_y - 1, size_z - 1)] +
                   border_x_end[(size_y - 1) * size_z + size_z - 1]) / hx2;
        d_phi_y = (phi[phi_index(size_x - 1, size_y - 2, size_z - 1)] +
                   border_y_end[(size_x - 1) * size_z + size_z - 1]) / hy2;
        d_phi_z = (phi[phi_index(size_x - 1, size_y - 1, size_z - 2)] +
                   border_z_start[(size_x - 1) * size_y + size_y - 1]) / hz2;
        new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, size_x - 1, size_y - 1, size_z - 1)) /
                    (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
        phi_help[phi_index(size_x - 1, size_y - 1, size_z - 1)] = new_value;

        for (int x = 1; x < size_x - 1; x++) {
            d_phi_x = (phi[phi_index(x + 1, 0, 0)] + phi[phi_index(x - 1, 0, 0)]) / hx2;
            d_phi_y = (phi[phi_index(x, 1, 0)] + border_y_start[x * size_z]) / hy2;
            d_phi_z = (phi[phi_index(x, 0, 1)] + border_z_start[x * size_y]) / hz2;

            new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, x, 0, 0)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
            phi_help[phi_index(x, 0, 0)] = new_value;
        }
        for (int x = 1; x < size_x - 1; x++) {
            d_phi_x = (phi[phi_index(x + 1, 0, size_z - 1)] + phi[phi_index(x - 1, 0, size_z - 1)]) / hx2;
            d_phi_y = (phi[phi_index(x, 1, size_z - 1)] + border_y_start[x * size_z + size_z - 1]) / hy2;
            d_phi_z = (phi[phi_index(x, 0, size_z - 2)] + border_z_end[x * size_y]) / hz2;

            new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, x, 0, size_z - 1)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
            phi_help[phi_index(x, 0, size_z - 1)] = new_value;
        }
        for (int x = 1; x < size_x - 1; x++) {
            d_phi_x = (phi[phi_index(x + 1, size_y - 1, 0)] + phi[phi_index(x - 1, size_y - 1, 0)]) / hx2;
            d_phi_y = (phi[phi_index(x, size_y - 2, 0)] + border_y_end[x * size_z]) / hy2;
            d_phi_z = (phi[phi_index(x, size_y - 1, 1)] + border_z_start[x * size_y + size_y - 1]) / hz2;

            new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, x, size_y - 1, 0)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
            phi_help[phi_index(x, size_y - 1, 0)] = new_value;
        }
        for (int x = 1; x < size_x - 1; x++) {
            d_phi_x = (phi[phi_index(x + 1, size_y - 1, size_z - 1)] + phi[phi_index(x - 1, size_y - 1, size_z - 1)]) /
                      hx2;
            d_phi_y = (phi[phi_index(x, size_y - 2, size_z - 1)] + border_y_end[x * size_z + size_z - 1]) / hy2;
            d_phi_z = (phi[phi_index(x, size_y - 1, size_z - 2)] + border_z_end[x * size_y + size_y - 1]) / hz2;

            new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, x, size_y - 1, size_z - 1)) /
                        (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
            phi_help[phi_index(x, size_y - 1, size_z - 1)] = new_value;
        }

        for (int y = 1; y < size_y - 1; y++) {
            d_phi_x = (phi[phi_index(1, y, 0)] + border_x_start[y * size_z]) / hx2;
            d_phi_y = (phi[phi_index(0, y + 1, 0)] + phi[phi_index(0, y - 1, 0)]) / hy2;
            d_phi_z = (phi[phi_index(0, y, 1)] + border_z_start[y]) / hz2;

            new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, 0, y, 0)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
            phi_help[phi_index(0, y, 0)] = new_value;
        }
        for (int y = 1; y < size_y - 1; y++) {
            d_phi_x = (phi[phi_index(1, y, size_z - 1)] + border_x_start[y * size_z + size_z - 1]) / hx2;
            d_phi_y = (phi[phi_index(0, y + 1, size_z - 1)] + phi[phi_index(0, y - 1, size_z - 1)]) / hy2;
            d_phi_z = (phi[phi_index(0, y, size_z - 2)] + border_z_end[y]) / hz2;

            new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, 0, y, size_z - 1)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
            phi_help[phi_index(0, y, size_z - 1)] = new_value;
        }
        for (int y = 1; y < size_y - 1; y++) {
            d_phi_x = (phi[phi_index(size_x - 2, y, 0)] + border_x_end[y * size_z]) / hx2;
            d_phi_y = (phi[phi_index(size_x - 1, y + 1, 0)] + phi[phi_index(size_x - 1, y - 1, 0)]) / hy2;
            d_phi_z = (phi[phi_index(size_x - 1, y, 1)] + border_z_start[(size_x - 1) * size_y + y]) / hz2;

            new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, size_x - 1, y, 0)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
            phi_help[phi_index(size_x - 1, y, 0)] = new_value;
        }
        for (int y = 1; y < size_y - 1; y++) {
            d_phi_x = (phi[phi_index(size_x - 2, y, size_z - 1)] + border_x_end[y * size_z + size_z - 1]) / hx2;
            d_phi_y = (phi[phi_index(size_x - 1, y + 1, size_z - 1)] + phi[phi_index(size_x - 1, y - 1, size_z - 1)]) /
                      hy2;
            d_phi_z = (phi[phi_index(size_x - 1, y, size_z - 2)] + border_z_end[(size_x - 1) * size_y + y]) / hz2;

            new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, size_x - 1, y, size_z - 1)) /
                        (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
            phi_help[phi_index(size_x - 1, y, size_z - 1)] = new_value;
        }

        for (int z = 1; z < size_z - 1; z++) {
            d_phi_x = (phi[phi_index(1, 0, z)] + border_x_start[z]) / hx2;
            d_phi_y = (phi[phi_index(0, 1, z)] + border_y_start[z]) / hy2;
            d_phi_z = (phi[phi_index(0, 0, z + 1)] + phi[phi_index(0, 0, z - 1)]) / hz2;

            new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, 0, 0, z)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
            phi_help[phi_index(0, 0, z)] = new_value;
        }
        for (int z = 1; z < size_z - 1; z++) {
            d_phi_x = (phi[phi_index(size_x - 2, 0, z)] + border_x_end[z]) / hx2;
            d_phi_y = (phi[phi_index(size_x - 1, 1, z)] + border_y_start[(size_x - 1) * size_z + z]) / hy2;
            d_phi_z = (phi[phi_index(size_x - 1, 0, z + 1)] + phi[phi_index(size_x - 1, 0, z - 1)]) / hz2;

            new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, size_x - 1, 0, z)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
            phi_help[phi_index(size_x - 1, 0, z)] = new_value;
        }
        for (int z = 1; z < size_z - 1; z++) {
            d_phi_x = (phi[phi_index(1, size_y - 1, z)] + border_x_start[(size_y - 1) * size_z + z]) / hx2;
            d_phi_y = (phi[phi_index(0, size_y - 2, z)] + border_y_end[z]) / hy2;
            d_phi_z = (phi[phi_index(0, size_y - 1, z + 1)] + phi[phi_index(0, size_y - 1, z - 1)]) / hz2;

            new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, 0, size_y - 1, z)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
            phi_help[phi_index(0, size_y - 1, z)] = new_value;
        }
        for (int z = 1; z < size_z - 1; z++) {
            d_phi_x = (phi[phi_index(size_x - 2, size_y - 1, z)] + border_x_end[(size_y - 1) * size_z + z]) / hx2;
            d_phi_y = (phi[phi_index(size_x - 1, size_y - 2, z)] + border_y_end[(size_x - 1) * size_z + z]) / hy2;
            d_phi_z = (phi[phi_index(size_x - 1, size_y - 1, z + 1)] + phi[phi_index(size_x - 1, size_y - 1, z - 1)]) /
                      hz2;

            new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, size_x - 1, size_y - 1, z)) /
                        (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
            phi_help[phi_index(size_x - 1, size_y - 1, z)] = new_value;
        }


        for (int y = 1; y < size_y - 1; y++) {
            for (int z = 1; z < size_z - 1; z++) {
                d_phi_x = (phi[phi_index(1, y, z)] + border_x_start[y * size_z + z]) / hx2;
                d_phi_y = (phi[phi_index(0, y + 1, z)] + phi[phi_index(0, y - 1, z)]) / hy2;
                d_phi_z = (phi[phi_index(0, y, z + 1)] + phi[phi_index(0, y, z - 1)]) / hz2;

                new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, 0, y, z)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
                phi_help[phi_index(0, y, z)] = new_value;
            }
        }

        for (int y = 1; y < size_y - 1; y++) {
            for (int z = 1; z < size_z - 1; z++) {
                d_phi_x = (phi[phi_index(size_x - 2, y, z)] + border_x_end[y * size_z + z]) / hx2;
                d_phi_y = (phi[phi_index(size_x - 1, y + 1, z)] + phi[phi_index(size_x - 1, y - 1, z)]) / hy2;
                d_phi_z = (phi[phi_index(size_x - 1, y, z + 1)] + phi[phi_index(size_x - 1, y, z - 1)]) / hz2;

                new_value =
                        (d_phi_x + d_phi_y + d_phi_z - ro(phi, size_x - 1, y, z)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
                phi_help[phi_index(size_x - 1, y, z)] = new_value;
            }
        }

        for (int y = 1; y < size_y - 1; y++) {
            for (int x = 1; x < size_x - 1; x++) {
                d_phi_x = (phi[phi_index(x + 1, y, 0)] + phi[phi_index(x - 1, y, 0)]) / hx2;
                d_phi_y = (phi[phi_index(x, y + 1, 0)] + phi[phi_index(x, y - 1, 0)]) / hy2;
                d_phi_z = (phi[phi_index(x, y, 1)] + border_z_start[x * size_y + y]) / hz2;

                new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, x, y, 0)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
                phi_help[phi_index(x, y, 0)] = new_value;
            }
        }

        for (int y = 1; y < size_y - 1; y++) {
            for (int x = 1; x < size_x - 1; x++) {
                d_phi_x = (phi[phi_index(x + 1, y, size_z - 1)] + phi[phi_index(x - 1, y, size_z - 1)]) / hx2;
                d_phi_y = (phi[phi_index(x, y + 1, size_z - 1)] + phi[phi_index(x, y - 1, size_z - 1)]) / hy2;
                d_phi_z = (phi[phi_index(x, y, size_z - 2)] + border_z_end[x * size_y + y]) / hz2;

                new_value =
                        (d_phi_x + d_phi_y + d_phi_z - ro(phi, x, y, size_z - 1)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
                phi_help[phi_index(x, y, size_z - 1)] = new_value;
            }
        }

        for (int x = 1; x < size_x - 1; x++) {
            for (int z = 1; z < size_z - 1; z++) {
                d_phi_x = (phi[phi_index(x + 1, 0, z)] + phi[phi_index(x - 1, 0, z)]) / hx2;
                d_phi_y = (phi[phi_index(x, 1, z)] + border_y_start[x * size_z + z]) / hy2;
                d_phi_z = (phi[phi_index(x, 0, z + 1)] + phi[phi_index(x, 0, z - 1)]) / hz2;

                new_value = (d_phi_x + d_phi_y + d_phi_z - ro(phi, x, 0, z)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
                phi_help[phi_index(x, 0, z)] = new_value;
            }
        }

        for (int x = 1; x < size_x - 1; x++) {
            for (int z = 1; z < size_z - 1; z++) {
                d_phi_x = (phi[phi_index(x + 1, size_y - 1, z)] + phi[phi_index(x - 1, size_y - 1, z)]) / hx2;
                d_phi_y = (phi[phi_index(x, size_y - 2, z)] + border_y_end[x * size_z + z]) / hy2;
                d_phi_z = (phi[phi_index(x, size_y - 1, z + 1)] + phi[phi_index(x, size_y - 1, z - 1)]) / hz2;

                new_value =
                        (d_phi_x + d_phi_y + d_phi_z - ro(phi, x, size_y - 1, z)) / (2 / hx2 + 2 / hy2 + 2 / hz2 + a);
                phi_help[phi_index(x, size_y - 1, z)] = new_value;
            }
        }

        pullBorder(phi_help, cart_comm);
        for (int i = 0; i < size_x * size_y * size_z; i++) {
            max_delta = fmax(max_delta, fabs(phi_help[i] - phi[i]));
        }
        swap(&phi, &phi_help);

        global_max_delta = 0.0;
        MPI_Allreduce(&max_delta, &global_max_delta, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    } while (global_max_delta > e);

    free(border_x_start);
    free(border_x_end);
    free(border_y_start);
    free(border_y_end);
    free(border_z_start);
    free(border_z_end);

    free(border_x_start_own);
    free(border_x_end_own);
    free(border_y_start_own);
    free(border_y_end_own);
    free(border_z_start_own);
    free(border_z_end_own);
}

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-Nx") == 0) {
            Nx = strtol(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "-Ny") == 0) {
            Ny = strtol(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "-Nz") == 0) {
            Nz = strtol(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "-px") == 0) {
            px = strtol(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "-py") == 0) {
            py = strtol(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "-pz") == 0) {
            pz = strtol(argv[++i], NULL, 10);
        }
    }
    MPI_Init(&argc, &argv);

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (px * py * pz > size) {
        if (rank == 0) {
            printf("px * py * pz must be not bigger than world_size");
        }
        MPI_Finalize();
        return 0;
    }

    if (Nx % px || Ny % py || Nz % pz) {
        if (rank == 0) {
            printf("Nx %% px, Ny %% py and Nz %% pz must be 0");
        }
        MPI_Finalize();
        return 0;
    }

    hx = Dx / Nx;
    hx2 = hx * hx;
    hy = Dy / Ny;
    hy2 = hy * hy;
    hz = Dz / Nz;
    hz2 = hz * hz;

    size_x = Nx / px + 1;
    size_y = Ny / py + 1;
    size_z = Nz / pz + 1;

    printf("%F %F %F %F %F %F\n%F %F %F %F %F %F\n%d %d %d\n", Dx, Dy, Dz, X0, Y0, Z0, hx, hx2, hy, hy2, hz, hz2, size_x, size_y, size_z);

    int dims[3] = {px, py, pz};
    int coords[3];
    int periods[3] = {1, 1, 1};

    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 3, dims, periods, 1, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, 3, coords);

    double *phi = (double *) malloc(sizeof(double) * size_x * size_y * size_z);
    for (int i = 0; i < size_x * size_y * size_z; i++) {
        phi[i] = 0.0;
    }
    pullBorder(phi, cart_comm);
    double *phi_help = (double *) malloc(sizeof(double) * size_x * size_y * size_z);

    double start_time = MPI_Wtime();
    method_Jacoby(phi, phi_help, cart_comm);
    double end_time = MPI_Wtime();

    double max = 0.0;
    for (int x = 0; x < size_x; x++) {
        for (int y = 0; y < size_y; y++) {
            for (int z = 0; z < size_z; z++) {
                double x_n = X(x + coords[0] * (size_x - 1));
                double y_n = Y(y + coords[1] * (size_y - 1));
                double z_n = Z(z + coords[2] * (size_z - 1));
                double value = value = x_n * x_n + y_n * y_n + z_n * z_n;
                max = fmax(max, fabs(phi[phi_index(x, y, z)] - value));
            }
        }
    }
    double global_max_delta = 0.0;
    MPI_Allreduce(&max, &global_max_delta, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Elapsed time = %F, max_delta = %F\n", end_time - start_time, global_max_delta);
    }

    free(phi);
    free(phi_help);

    MPI_Finalize();
    return 0;
}
