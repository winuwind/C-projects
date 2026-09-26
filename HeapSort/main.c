#include <stdio.h>
#include <malloc.h>


void Swap(int* a, int* b){
    int c = *a;
    *a = *b;
    *b = c;
}


int* Max(int* a, int* b){
    if(*a >= *b){
        return a;
    }
    else{
        return b;
    }
}


void Print_list(int* array, int len){
    for(int i = 0; i < len; i++){
        printf("%d ", array[i]);
    }
}


void Down(int* array, int len, int index){
    if(index * 2 + 1 < len){
        if(index * 2 + 2 < len){
            int* max = Max(&array[index * 2 + 1], &array[index * 2 + 2]);
            if(array[index] < *max){
                Swap(&array[index], max);
                Down(array, len, (int) (max - array));
            }
        }
        else{
            if(array[index] < array[index * 2 + 1]){
                Swap(&array[index], &array[index * 2 + 1]);
                Down(array, len, index * 2 + 1);
            }
        }
    }
}


void CreateHeap(int* array, int len){
    for(int i = len / 2; i >= 1; i--){
        if(i * 2 < len){
            int* max = Max(&array[i * 2 - 1], &array[i * 2]);
            if(array[i - 1] < *max){
                Swap(&array[i - 1], max);
                Down(array, len, (int) (max - array));
            }
        }
        else{
            if(array[i - 1] < array[i * 2 - 1]){
                Swap(&array[i - 1], &array[i * 2 - 1]);
                Down(array, len, i * 2 - 1);
            }
        }
    }
}


void HeapSort(int* array, int len) {
    CreateHeap(array, len);
    for(; len > 1; len--) {
        Swap(&array[0], &array[len - 1]);
        Down(array, len - 1, 0);
    }
}


int main(){
    int N, count1 = 0;
    if(scanf("%d", &N) != 1){
        return 0;
    }
    if(N == 0){
        return 0;
    }
    int* array = (int*) malloc(sizeof(int) * N);
    for(int i = 0; i < N; i++){
        if(scanf("%d", &array[i]) != 1){
            return 0;
        }
        if(i > 0){
            if(array[i] >= array[i - 1]){
                count1++;
            }
        }
    }
    if(count1 == N - 1){
        Print_list(array, N);
        free(array);
        return 0;
    }
    HeapSort(array, N);
    Print_list(array, N);
    free(array);
    return 0;
}