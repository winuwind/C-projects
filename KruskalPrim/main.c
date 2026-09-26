#include <stdio.h>
#include <limits.h>
#include <malloc.h>


typedef struct Edges_weight{
    int peak_1, peak_2, weight;
}Edges_weight_t;



typedef struct Edges{
    int peak_1, peak_2;
}Edges_t;


int Find(int* p, int x){
    if(p[x] == x){
        return x;
    }
    p[x] = Find(p, p[x]);
    return p[x];
}


void Unite(int* p, int* rank, int x, int y){
    x = Find(p, x);
    y = Find(p, y);
    if (rank[x] < rank[y])
        p[x] = y;
    else
    {
        p[y] = x;
        if (rank[x] == rank[y])
            ++rank[x];
    }
}


void Swap(Edges_weight_t* a, Edges_weight_t* b){
    int peak_1 = a->peak_1, peak_2 = a->peak_2, weight = a->weight;
    a->peak_1 = b->peak_1; a->peak_2 = b->peak_2; a->weight = b->weight;
    b->peak_1 = peak_1; b->peak_2 = peak_2; b->weight = weight;
}


Edges_weight_t* Min(Edges_weight_t* a, Edges_weight_t* b){
    if(a->weight < b->weight){
        return a;
    }
    else{
        return b;
    }
}


void Down(Edges_weight_t* array, int len, int index){
    if(index * 2 + 1 < len){
        if(index * 2 + 2 < len){
            Edges_weight_t* min = Min(&array[index * 2 + 1], &array[index * 2 + 2]);
            if(array[index].weight > min->weight){
                Swap(&array[index], min);
                Down(array, len, (int) (min - array)); //самый маленький в самом коце массива (низу дерева) и не поднимется наверх
            }
        }
        else{
            if(array[index].weight > array[index * 2 + 1].weight){
                Swap(&array[index], &array[index * 2 + 1]);
                Down(array, len, index * 2 + 1);
            }
        }
    }
}


void Up(Edges_weight_t* array, int index){
    if(array[index].weight < array[index / 2].weight){
        Swap(&array[index], &array[index / 2]);
        Up(array, index / 2);
    }
}


void KruskalPrim(int* Arr_edges_weight/*, Edges_t* Arr_edges*/, int N, int M, FILE* fp){
    Edges_weight_t* queue = (Edges_weight_t*) malloc(sizeof(Edges_weight_t) * 100);
    int size_queue = 0, peak = 0, size = 0, size_q = 100;
    int* p = (int*) malloc(sizeof(int) * (N + 1)), *rank = (int*) malloc(sizeof(int) * (N + 1));
    for(int i = 1; i < N + 1; i++){
        p[i] = i;
        rank[i] = 1;
    }
    while(size < N - 1) {
        for (int i = 0; i < peak; i++) {
            if (Arr_edges_weight[peak * (peak - 1) / 2 + i] != -1) {
                if (Find(p, 1) != Find(p, i + 1)) {
                    if(size_queue == size_q){
                        size_q += 100;
                        Edges_weight_t* queue_new = realloc(queue, sizeof(Edges_weight_t) * size_q);
                        if(queue_new == NULL){
                            free(p); free(rank); free(queue);
                            return;
                        }
                        queue = queue_new;
                    }
                    queue[size_queue].peak_1 = peak + 1;
                    queue[size_queue].peak_2 = i + 1;
                    queue[size_queue].weight = Arr_edges_weight[peak * (peak - 1) / 2 + i];
                    Up(queue, size_queue++);
                }
            }
        }
        for(int i = peak + 1; i < N; i++){
            if (Arr_edges_weight[i * (i - 1) / 2 + peak] != -1) {
                if (Find(p, 1) != Find(p, i + 1)) {
                    if(size_queue == size_q){
                        size_q += 100;
                        Edges_weight_t* queue_new = realloc(queue, sizeof(Edges_weight_t) * size_q);
                        if(queue_new == NULL){
                            free(p); free(rank); free(queue);
                            return;
                        }
                        queue = queue_new;
                    }
                    queue[size_queue].peak_1 = peak + 1;
                    queue[size_queue].peak_2 = i + 1;
                    queue[size_queue].weight = Arr_edges_weight[i * (i - 1) / 2 + peak];
                    Up(queue, size_queue++);
                }
            }
        }
        if(size_queue == 0){
            if (size < N - 1) {
                if(freopen("out.txt", "w", fp) == NULL){
                    free(p); free(rank); free(queue);
                    return;
                }
                fprintf(fp, "no spanning tree");
                free(p); free(rank); free(queue);
                return;
            }
            else{
                break;
            }
        }
        while (Find(p, queue[0].peak_1) == Find(p, 1) && Find(p, queue[0].peak_2) == Find(p, 1) && size_queue > 0) {
            Swap(queue, &(queue[--size_queue]));
            Down(queue, size_queue, 0);
        }
        if (size_queue == 0) {
            if (size < N - 1) {
                if(freopen("out.txt", "w", fp) == NULL){
                    free(p); free(rank); free(queue);
                    return;
                }
                fprintf(fp, "no spanning tree");
                free(p); free(rank); free(queue);
                return;
            }
            else{
                break;
            }
        }
        else {
            if (Find(p, queue[0].peak_1) == Find(p, 1)) {
                peak = queue[0].peak_2 - 1;
            }
            else {
                peak = queue[0].peak_1 - 1;
            }
            Unite(p, rank, 1, queue[0].peak_1);
            Unite(p, rank, 1, queue[0].peak_2);
//            Arr_edges[size].peak_1 = queue[0].peak_1;
//            Arr_edges[size].peak_2 = queue[0].peak_2;
            fprintf(fp, "%d %d\n", queue[0].peak_1, queue[0].peak_2);
            size++;
            Swap(queue, &(queue[--size_queue]));
            Down(queue, size_queue, 0);
        }
    }
//    for (int i = 0; i < size; i++) {
//        printf("%d %d\n", Arr_edges[i].peak_1, Arr_edges[i].peak_2);
//    }
    free(p); free(rank); free(queue);
}


int main() {
    FILE *fp = stdout;
    long long int N, M, peak1, peak2, weight;
    if(scanf("%lld\n%lld", &N, &M) != 2){
        return 0;
    }
    if(N < 0 || N > 5000){
        fprintf(fp, "bad number of vertices");
        return 0;
    }
    if(M < 0 || M > N * (N - 1) / 2){
        fprintf(fp, "bad number of edges");
        return 0;
    }
    if(N != 1 && M == 0){
        fprintf(fp, "no spanning tree");
        return 0;
    }
    else if(N == 1 && M == 0){
        return 0;
    }
    int* Arr_edges_weight = (int*) malloc(sizeof(int) * (N - 1) * N / 2);
    for(int i = 0; i < (N - 1) * N / 2; i++){
        Arr_edges_weight[i] = -1;
    }
    for(int i = 0; i < (int) M; i++){
        if(scanf("%lld %lld %lld", &peak1, &peak2, &weight) != 3){
            fprintf(fp, "bad number of lines");
            free(Arr_edges_weight);
            return 0;
        }
        if(peak1 < 1 || peak1 > N || peak2 < 1 || peak2 > N){
            fprintf(fp, "bad vertex");
            free(Arr_edges_weight);
            return 0;
        }
        if(weight < 0 || weight > INT_MAX){
            fprintf(fp, "bad length");
            free(Arr_edges_weight);
            return 0;
        }
        peak1--; peak2--;
        if(peak1 == peak2){
            continue;
        }
        if(peak1 < peak2){
            long long int c = peak1;
            peak1 = peak2;
            peak2 = c;
        }
        else if(peak1 == peak2) {
            continue;
        }
        Arr_edges_weight[peak1 * (peak1 - 1) / 2 + peak2] = (int) weight;
    }
//    Edges_t* Arr_edges = (Edges_t*) malloc(sizeof(Edges_t) * (N - 1));
    KruskalPrim(Arr_edges_weight,/* Arr_edges,*/ (int) N, (int) M, fp);
    free(Arr_edges_weight);
//    free(Arr_edges);
    fclose(fp);
    return 0;
}
