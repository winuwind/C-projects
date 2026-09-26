#include <stdio.h>
#include <limits.h>
#include <malloc.h>
#include <string.h>
#include <math.h>


#define M_PI 3.14159265358979323846


typedef struct Edges_weight{
    int peak_1, peak_2, weight;
}Edges_weight_t;



typedef struct Edges{
    int peak_1, peak_2;
}Edges_t;


typedef struct Coords{
    int x, y;
}Coords_t;


void Quick_Sort(int n_Start, int n_End, Edges_weight_t array[]){
    if(n_Start >= n_End){
        return;
    }
    int left = n_Start, right = n_End;
    int x = array[left + ((right - left) / 2)].weight, c;
    while(left <= right){
        while(array[left].weight < x){
            left++;
        }
        while(array[right].weight > x){
            right--;
        }
        if(left <= right){
            c = array[left].weight; array[left].weight = array[right].weight; array[right].weight = c;
            c = array[left].peak_1; array[left].peak_1 = array[right].peak_1; array[right].peak_1 = c;
            c = array[left].peak_2; array[left].peak_2 = array[right].peak_2; array[right].peak_2 = c;
            left++;
            right--;
        }
    }
    Quick_Sort(n_Start, right, array);
    Quick_Sort(left, n_End, array);
}


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


int Kruskal(Edges_weight_t Arr_edges_weight[], Edges_t* Arr_edges, int N, int M){
    int size = 0, count_bunch = N;
    int* p = (int*) malloc(sizeof(int) * (N + 1)), *rank = (int*) malloc(sizeof(int) * (N + 1));
    for(int i = 1; i < N + 1; i++){
        p[i] = i;
        rank[i] = 1;
    }
    for(int i = 0; i < M && count_bunch > 1; i++){
        if(Find(p, Arr_edges_weight[i].peak_1) != Find(p, Arr_edges_weight[i].peak_2)){
            Arr_edges[size].peak_1 = Arr_edges_weight[i].peak_1;
            Arr_edges[size++].peak_2 = Arr_edges_weight[i].peak_2;
            Unite(p, rank, Arr_edges_weight[i].peak_1, Arr_edges_weight[i].peak_2);
            count_bunch--;
        }
    }
    if(count_bunch != 1){
        printf("no spanning tree");
    }
    else {
        for (int i = 0; i < size; i++) {
            printf("%d %d\n", Arr_edges[i].peak_1, Arr_edges[i].peak_2);
        }
    }
    free(p);
    free(rank);
    return size;
}


void Make_svg_file(Edges_weight_t Arr_edges_weight[], Edges_t* Arr_edges, int N, int M, int size){
    FILE *fp = fopen("solution.svg", "w");
    fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<svg width=\"3000pt\" height=\"3000pt\"\n viewBox = \"0 0 3000 3000\"\n xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n");
    Coords_t *Arr_coords = (Coords_t *) malloc(sizeof(Coords_t) * N);
    double cx0, cy0, cx1, cy1, l, dx, dy;


    for(int i = 0; i < N; i++){
        double radius = 1000, x, y;
        int Ox = 1200, Oy = 1200;
        x = radius * cos(2 * M_PI / ((double) N) * i);
        y = radius * sin(2 * M_PI / ((double) N) * i);
        Arr_coords[i].x = (int) x + Ox;
        Arr_coords[i].y = (int) y + Oy;
        fprintf(fp, "<g id=\"node%d\" class=\"node\">\n<ellipse style=\"fill:none;stroke:black;\" cx=\"%d\" cy=\"%d\" rx=\"17\" ry=\"17\"/>\n<text text-anchor=\"middle\" x=\"%d\" y=\"%d\">%d</text>\n</g>\n", i + 1, Arr_coords[i].x, Arr_coords[i].y, Arr_coords[i].x, Arr_coords[i].y, i + 1);
    }


    for(int i = 0; i < M; i++){
        dx = Arr_coords[Arr_edges_weight[i].peak_1 - 1].x - Arr_coords[Arr_edges_weight[i].peak_2 - 1].x;
        dy = Arr_coords[Arr_edges_weight[i].peak_1 - 1].y - Arr_coords[Arr_edges_weight[i].peak_2 - 1].y;
        l = sqrt(pow(dx, 2) + pow(dy, 2));
        cx0 = Arr_coords[Arr_edges_weight[i].peak_1 - 1].x - (int) (17.0 * dx / l);
        cy0 = Arr_coords[Arr_edges_weight[i].peak_1 - 1].y - (int) (17.0 * dy / l);
        cx1 = Arr_coords[Arr_edges_weight[i].peak_2 - 1].x + (int) (17.0 * dx / l);
        cy1 = Arr_coords[Arr_edges_weight[i].peak_2 - 1].y + (int) (17.0 * dy / l);
        fprintf(fp, "<g id=\"edge%d\" class=\"edge\">\n<path d=\"M %d %d L %d %d z\" fill=\"red\" stroke=\"blue\" stroke-width=\"3\" />\n<text text-anchor=\"middle\" x=\"%d\" y=\"%d\">%d</text>\n</g>\n", i + 1 + N, (int) cx0, (int) cy0, (int) cx1, (int) cy1, (int) ((cx0 + cx1) / 2) - (int) (100.0 * dx / l) - 10, (int) ((cy0 + cy1) / 2) - (int) (100.0 * dy / l) - 10, Arr_edges_weight[i].weight);
    }


    for(int i = 0; i < size; i++){
        dx = Arr_coords[Arr_edges[i].peak_1 - 1].x - Arr_coords[Arr_edges[i].peak_2 - 1].x;
        dy = Arr_coords[Arr_edges[i].peak_1 - 1].y - Arr_coords[Arr_edges[i].peak_2 - 1].y;
        l = sqrt(pow(dx, 2) + pow(dy, 2));
        cx0 = Arr_coords[Arr_edges[i].peak_1 - 1].x - (int) (17.0 * dx / l);
        cy0 = Arr_coords[Arr_edges[i].peak_1 - 1].y - (int) (17.0 * dy / l);
        cx1 = Arr_coords[Arr_edges[i].peak_2 - 1].x + (int) (17.0 * dx / l);
        cy1 = Arr_coords[Arr_edges[i].peak_2 - 1].y + (int) (17.0 * dy / l);
        fprintf(fp, "<g id=\"edge%d\" class=\"edge\">\n<path d=\"M %d %d L %d %d z\" fill=\"red\" stroke=\"red\" stroke-width=\"6\" />\n</g>\n", i + 1 + N, (int) cx0, (int) cy0, (int) cx1, (int) cy1);
    }
    fprintf(fp, "</svg>");
    free(Arr_coords);
    fclose(fp);
}


int main(int count_arg, char** Arr_arg) {
    long long int N, M, peak1, peak2, weight;
    if(scanf("%lld\n%lld", &N, &M) != 2){
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
    if(N != 1 && M == 0){
        printf("no spanning tree");
        return 0;
    }
    else if(N == 1 && M == 0){
        return 0;
    }
    Edges_weight_t* Arr_edges_weight = (Edges_weight_t*) malloc(sizeof(Edges_weight_t) * M);
    for(int i = 0; i < (int) M; i++){
        if(scanf("%lld %lld %lld", &peak1, &peak2, &weight) != 3){
            printf("bad number of lines");
            free(Arr_edges_weight);
            return 0;
        }
        if(peak1 < 1 || peak1 > N || peak2 < 1 || peak2 > N){
            printf("bad vertex");
            free(Arr_edges_weight);
            return 0;
        }
        if(weight < 0 || weight > INT_MAX){
            printf("bad length");
            free(Arr_edges_weight);
            return 0;
        }
        Arr_edges_weight[i].weight = (int) weight;
        if(peak1 < peak2){
            Arr_edges_weight[i].peak_1 = (int) peak1;
            Arr_edges_weight[i].peak_2 = (int) peak2;
        }
        else{
            Arr_edges_weight[i].peak_2 = (int) peak1;
            Arr_edges_weight[i].peak_1 = (int) peak2;
        }
    }
    Quick_Sort(0, (int) M - 1, Arr_edges_weight);
    Edges_t* Arr_edges = (Edges_t*) malloc(sizeof(Edges_t) * M);
    int size = Kruskal(Arr_edges_weight, Arr_edges, (int) N, (int) M);
    if(count_arg > 1){
        if(strcmp(Arr_arg[1], "-g") == 0){
            Make_svg_file(Arr_edges_weight, Arr_edges, (int) N, (int) M, size);
        }
    }
    free(Arr_edges_weight);
    free(Arr_edges);
    return 0;
}
