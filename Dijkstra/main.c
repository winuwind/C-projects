#include <stdio.h>
#include <malloc.h>
#include <limits.h>


int Find_min_peak(long long int N, long long int* Arr_flag){
    int ii = 0;
    long long int value = LLONG_MAX;
    for(int i = 0; i < N; i++){
        if(Arr_flag[3 * i] == 0 && Arr_flag[3 * i + 1] < value){
            ii = i;
            value = Arr_flag[3 * i + 1];
        }
    }
    if(value == LLONG_MAX){
        return -1;
    }
    return ii;
}


long long int Print_help(long long int N, long long int index, long long int* Arr_edge, long long int* Arr_flag){
    for(long long int i = 0; i < index; i++){
        if(Arr_edge[index * (index - 1) / 2 + i] < LLONG_MAX && Arr_flag[index * 3 + 1] == Arr_flag[i * 3 + 1] + Arr_edge[index * (index - 1) / 2 + i]){
            return i;
        }
    }
    for(long long int i = index + 1; i < N; i++){
        if(Arr_edge[i * (i - 1) / 2 + index] < LLONG_MAX && Arr_flag[index * 3 + 1] == Arr_flag[i * 3 + 1] + Arr_edge[i * (i - 1) / 2 + index]){
            return i;
        }
    }
    return -1;
}


void Print_path(long long int N, long long int index, long long int* Arr_edge, long long int* Arr_flag, int* Arr_Path, int* size){
    printf("\n");
    while(Arr_flag[3 * index + 1] > 0){
        printf("%lld ", index + 1);
        Arr_Path[(*size)++] = (int) index;
        index = Print_help(N, index, Arr_edge, Arr_flag);
        if(index == -1){
            printf("\nindex = -1");
            return;
        }
    }
    printf("%lld", index + 1);
    Arr_Path[(*size)++] = (int) index;
}


void Find_neighbour(long long int N, int index, long long int* Arr_edge, long long int* Arr_flag){
    for(int i = 0; i < index; i++){
        if(Arr_edge[index * (index - 1) / 2 + i] < LLONG_MAX){
            Arr_flag[3 * i + 2] += Arr_flag[3 * index + 2];
            if(Arr_flag[3 * i] == 0 && Arr_edge[index * (index - 1) / 2 + i] + Arr_flag[3 * index + 1] < Arr_flag[3 * i + 1]){
                Arr_flag[3 * i + 1] = Arr_edge[index * (index - 1) / 2 + i] + Arr_flag[3 * index + 1];
            }
        }
    }
    for(int i = index + 1; i < N; i++){
        if(Arr_edge[i * (i - 1) / 2 + index] < LLONG_MAX){
            Arr_flag[3 * i + 2] += Arr_flag[3 * index + 2];
            if(Arr_flag[3 * i] == 0 && Arr_edge[i * (i - 1) / 2 + index] + Arr_flag[3 * index + 1] < Arr_flag[3 * i + 1]){
                Arr_flag[3 * i + 1] = Arr_edge[i * (i - 1) / 2 + index] + Arr_flag[3 * index + 1];
            }
        }
    }
}


void Dijkstra(long long int N, long long int start, long long int* Arr_edges, long long int* Arr_flag){
    int index = (int) start;
    do{
        Find_neighbour(N, index, Arr_edges, Arr_flag);
        Arr_flag[3 * index] = 1;
        index = Find_min_peak(N, Arr_flag);
    }while(index >= 0);
}


void Reverse(char* str, int size){
    for(int i = 0; i < size / 2; i++){
        char c;
        c = str[i];
        str[i] = str[size - 1 - i];
        str[size - 1 - i] = c;
    }
}


void Make_tables(long long* Arr_edges, long long* Arr_flag, int *Arr_Path, int N, int size){
    FILE *fp = fopen("solution.html", "wb");
    long long help_len;
    char new_tr[5] = "<tr>\n", end_tr[6] = "</tr>\n", str_for_table_title[24] = "<table>\n<tr>\n<th>\n</th>\n", new_td_yellow[22] = "<td bgcolor=\"yellow\">\n";
    char end_table[9] = "</table>\n", new_th[5] = "<th>\n", end_th[6] = "</th>\n", new_td[5] = "<td>\n", end_td[6] = "</td>\n", integer[30], minus[4] = "---\n";
    fwrite(str_for_table_title, sizeof(char), 24, fp);
    for(int i = 0, ii = 1, j; i < N; ii = ++i + 1){
        fwrite(new_th, sizeof(char), 5, fp);
        for(j = 0; j < 4; j++){
            integer[j] = (char) (ii % 10 + (int) '0');
            ii /= 10;
            if(ii == 0){
                break;
            }
        }
        Reverse(integer, j + 1);
        integer[j + 1] = '\n';
        fwrite(integer, sizeof(char), j + 2, fp);
        fwrite(end_th, sizeof(char), 6, fp);
    }
    fwrite(end_tr, sizeof(char), 6, fp);
    for(int i = 0, ii = 1, j, jj, index; i < N; ii = ++i + 1){
        fwrite(new_tr, sizeof(char), 5, fp);
        for(j = 0; j < 4; j++){
            integer[j] = (char) (ii % 10 + (int) '0');
            ii /= 10;
            if(ii == 0){
                break;
            }
        }
        Reverse(integer, j + 1);
        integer[j + 1] = '\n';
        fwrite(new_th, sizeof(char), 5, fp);
        fwrite(integer, sizeof(char), j + 2, fp);
        fwrite(end_th, sizeof(char), 6, fp);
        for(index = 1; index < size; index++){
            if(Arr_Path[index] == i){
                break;
            }
        }
        for(j = 0; j < N; j++){
            if(Arr_Path[index - 1] == j && index < size){
                fwrite(new_td_yellow, sizeof(char), 22, fp);
            }
            else {
                fwrite(new_td, sizeof(char), 5, fp);
            }
            if(j < i){
                help_len = Arr_edges[i * (i - 1) / 2 + j];
            }
            else if(j == i){
                help_len = LLONG_MAX;
            }
            else{
                help_len = Arr_edges[j * (j - 1) / 2 + i];
            }
            if(help_len == LLONG_MAX){
                fwrite(minus, sizeof(char), 4, fp);
            }
            else{
                for(jj = 0; jj < 5; jj++){
                    integer[jj] = (char) ((int) help_len % 10 + (int) '0');
                    help_len /= 10;
                    if(help_len == 0){
                        break;
                    }
                }
                Reverse(integer, jj + 1);
                integer[jj + 1] = '\n';
                fwrite(integer, sizeof(char), jj + 2, fp);
            }
            fwrite(end_td, sizeof(char), 6, fp);
        }
        fwrite(end_tr, sizeof(char), 6, fp);
    }
    char style[200] = "<style>\ntable{\nborder: 3px solid red;\nborder-collapse: collapse;\nwidth: 20%;\n}\nth{\nborder: 2px solid black;\ncolor: green;\n}\ntd{\nborder: 1px solid grey;\n}\n</style>\n";
    fwrite(style, sizeof(char), 163, fp);
    fwrite(end_table, sizeof(char), 8, fp);
    char new_table[9] = "\n<table>\n";
    fwrite(new_table, sizeof(char), 9, fp);
    fwrite(style, sizeof(char), 163, fp);
    for(int i = 0; i < 2; i++){
        fwrite(new_tr, sizeof(char), 5, fp);
        if(i == 0) {
            for (int ii = 1, jj = 1, j; ii <= N; jj = ++ii) {
                for (j = 0; j < 4; j++) {
                    integer[j] = (char) (jj % 10 + (int) '0');
                    jj /= 10;
                    if (jj == 0) {
                        break;
                    }
                }
                Reverse(integer, j + 1);
                integer[j + 1] = '\n';
                fwrite(new_th, sizeof(char), 5, fp);
                fwrite(integer, sizeof(char), j + 2, fp);
                fwrite(end_th, sizeof(char), 6, fp);
            }
        }
        else{
            for (long long int ii = 0, jj = Arr_flag[ii * 3 + 1], j; ii < N; jj = Arr_flag[3 * (++ii) + 1]) {
                for (j = 0; j < 4; j++) {
                    integer[j] = (char) ((int) jj % 10 + (int) '0');
                    jj /= 10;
                    if (jj == 0) {
                        break;
                    }
                }
                Reverse(integer, (int) j + 1);
                integer[j + 1] = '\n';
                fwrite(new_td, sizeof(char), 5, fp);
                fwrite(integer, sizeof(char), j + 2, fp);
                fwrite(end_td, sizeof(char), 6, fp);
            }
        }
        fwrite(end_tr, sizeof(char), 6, fp);
    }
    fwrite(end_table, sizeof(char), 8, fp);
    fclose(fp);
}

int main(int count_arg, char *arr_arg[]) {
    long long int N, M, S, F, peak1, peak2, weight;
    if(scanf("%lld\n%lld %lld\n%lld", &N, &S, &F, &M) != 4){
        return 0;
    }
    if(N < 0 || N > 5000){
        printf("bad number of vertices");
        return 0;
    }
    if(M < 0 || M > N * (N - 1) / 2){
        printf("bad number of edges");
        return 0;
    }
    if(S < 1 || S > N || F < 1 || F > N){
        printf("bad vertex");
        return 0;
    }
    long long int *Arr_edges = (long long int*) malloc(sizeof(long long int) * N * (N - 1) / 2);
    for(int i = 0; i < N * (N - 1) / 2; i++){
        Arr_edges[i] = LLONG_MAX;
    }
    for(int i = 0; i < M; i++){
        if(scanf("%lld %lld %lld", &peak1, &peak2, &weight) != 3){
            printf("bad number of lines");
            free(Arr_edges);
            return 0;
        }
        if(peak1 < 1 || peak1 > N || peak2 < 1 || peak2 > N){
            printf("bad vertex");
            free(Arr_edges);
            return 0;
        }
        if(weight < 0 || weight > INT_MAX){
            printf("bad length");
            free(Arr_edges);
            return 0;
        }
        peak1--;
        peak2--;
        if(peak1 < peak2){
            long long int c = peak1;
            peak1 = peak2;
            peak2 = c;
        }
        else if(peak1 == peak2) {
            continue;
        }
        Arr_edges[peak1 * (peak1 - 1) / 2 + peak2] = weight;
    }
    S--; F--;
    long long int *Arr_flag = (long long int*) malloc(sizeof(long long int) * N * 3);
    int *Arr_Path = (int*) malloc(sizeof(int) * N), size = 0;
    for(int i = 0; i < N; i++){
        Arr_flag[3 * i] = 0;
        Arr_flag[3 * i + 1] = LLONG_MAX;
        Arr_flag[3 * i + 2] = 0;
    }
    Arr_flag[3 * S + 1] = 0;
    Arr_flag[3 * S + 2] = 1;
    Dijkstra(N, S, Arr_edges, Arr_flag);
    for(int i = 0; i < N; i++){
        if(Arr_flag[i * 3 + 1] < LLONG_MAX){
            if(Arr_flag[i * 3 + 1] > INT_MAX){
                printf("INT_MAX+ ");
            }
            else {
                printf("%lld ", Arr_flag[i * 3 + 1]);
            }
        }
        else {
            printf("oo ");
        }
    }
    if(Arr_flag[3 * F + 1] == LLONG_MAX){
        printf("\nno path");
    }
    else if(Arr_flag[3 * F + 1] > INT_MAX && Arr_flag[3 * F + 2] > 1){
        printf("\noverflow");
    }
    else{
        Print_path(N, F, Arr_edges, Arr_flag, Arr_Path, &size);
    }
    if(count_arg > 1 && arr_arg[1][0] == '-' && arr_arg[1][1] == 't'){
        Make_tables(Arr_edges, Arr_flag, Arr_Path, (int) N, size);
    }
    free(Arr_Path);
    free(Arr_flag);
    free(Arr_edges);
    return 0;
}
