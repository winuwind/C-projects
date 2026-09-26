#include <stdio.h>
#include <malloc.h>
#include <limits.h>


typedef struct Product{
    int weight, cost;
}Product_t;


void Func(Product_t* arr_products, int* arr_costs, int N, int W){
    for(int i = 0; i < N; i++){
        for(int j = 0; j <= W; j++){
            arr_costs[(i + 1) * (W + 1) + j] = arr_costs[i * (W + 1) + j];
        }
        for(int j = W; j >= 0; j--){
            int value1 = arr_costs[i * (W + 1) + j], value2 = arr_products[i].weight, value3 = arr_products[i].cost;
            if(value1 != -1 && value2 + j <= W){
                if(arr_costs[(i + 1) * (W + 1) + j + arr_products[i].weight] < value1 + value3){
                    arr_costs[(i + 1) * (W + 1) + j + arr_products[i].weight] = value1 + value3;
                }
            }
        }
    }
}


void FuncReturn(Product_t* arr_products, int* arr_costs, int N, int W){
    int weight = -1, cost = 0, size = 0;
    Product_t* arr_print = (Product_t*) malloc(sizeof(Product_t) * N);
    for(int i = W; i >= 0; i--){
        if(arr_costs[N * (W + 1) + i] != INT_MAX && arr_costs[N * (W + 1) + i] > cost){
            weight = i;
            cost = arr_costs[N * (W + 1) + i];
        }
    }
    printf("%d\n", cost);
    for(int i = N - 1; i >= 0; i--){
        for(int j = 0; j <= W; j++){
            if(weight == 0 && cost == 0){
                break;
            }
            if(arr_costs[i * (W + 1) + j == -1]){
                continue;
            }
            if(arr_costs[i * (W + 1) + j] + arr_products[i].cost == cost && arr_products[i].weight + j == weight){
                cost = arr_costs[i * (W + 1) + j];
                weight = j;
                arr_print[size++] = arr_products[i];
            }
        }
    }
    for(int i = size - 1; i >= 0; i--){
        printf("%d %d\n", arr_print[i].weight, arr_print[i].cost);
    }
    free(arr_print);
}


int main() {
    int N, W, weight, cost;
    if(scanf("%d %d", &N, &W) != 2){
        return 0;
    }
    Product_t* arr_products = (Product_t*) malloc(sizeof(Product_t) * N);
    int* arr_costs = (int*) malloc(sizeof(int) * (N + 1) * (W + 1));
    for(int i = 0; i < (N + 1) * (W + 1); i++){
        if(i % (W + 1) == 0){
            arr_costs[i] = 0;
        }
        else {
            arr_costs[i] = -1;
        }
    }
    arr_costs[0] = 0;
    for(int i = 0; i < N; i++){
        if(scanf("%d %d", &weight, &cost) != 2){
            free(arr_products);
            free(arr_costs);
            return 0;
        }
        arr_products[i].weight = weight; arr_products[i].cost = cost;
    }
    Func(arr_products, arr_costs, N, W);
    FuncReturn(arr_products, arr_costs, N, W);
    free(arr_products);
    free(arr_costs);
    return 0;
}
