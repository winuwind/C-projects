#include <stdio.h>
#include <malloc.h>


char Top_sort(char* arr_edges, int* arr_ver, int N, int* size, int ver){
    if(arr_edges[ver * N + ver] == 2){
        return 0;
    }
    else if(arr_edges[ver * N + ver] == 1){
        return 1;
    }
    else{
        arr_edges[ver * N + ver] = 1;
        for(int i = 1; i <= N; i++){
            if(i == ver){
                continue;
            }
            if(arr_edges[ver * N + i] == 1){
                if(Top_sort(arr_edges, arr_ver, N, size, i) == 1){
                    return 1;
                }
            }
        }
        arr_edges[ver * N + ver] = 2;
        arr_ver[(*size)++] = ver;
    }
    return 0;
}


int main() {
    int N, M, peak1, peak2, number = 1;
    if(scanf("%d", &N) != 1){
        printf("bad number of lines");
        return 0;
    }
    if (N < 0 || N > 2000){
        printf("bad number of vertices");
        return 0;
    }
    if(scanf("%d", &M) != 1){
        printf("bad number of lines");
        return 0;
    }
    if (M < 0 || M > N * (N + 1) / 2){
        printf("bad number of edges");
        return 0;
    }
    char* arr_edges = (char*) malloc(sizeof(char) * (N + 1) * (N + 1));
    for(int i = 0; i < (N + 1) * (N + 1); i++){
        arr_edges[i] = 0;
    }
    for(int i = 0; i < M; i++){
        if(scanf("%d %d", &peak1, &peak2) != 2){
            printf("bad number of lines");
            free(arr_edges);
            return 0;
        }
        if(peak1 < 1 || peak1 > N || peak2 < 1 || peak2 > N){
            printf("bad vertex");
            free(arr_edges);
            return 0;
        }
        if(peak1 == peak2){
            continue;
        }
        arr_edges[N * peak1 + peak2] = 1;
    }
    int* arr_ver = (int*) malloc(sizeof(int) * N), size = 0;
    while(size != N){
        if(arr_edges[number * N + number] == 0) {
            if (Top_sort(arr_edges, arr_ver, N, &size, number) == 1) {
                printf("impossible to sort");
                free(arr_edges);
                free(arr_ver);
                return 0;
            }
        }
        number++;
    }
    for(int i = size - 1; i >= 0; i--){
        printf("%d ", arr_ver[i]);
    }
    free(arr_edges);
    free(arr_ver);
    return 0;
}
