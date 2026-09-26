#include <stdio.h>
#include <malloc.h>


void Quick_Sort(int n_Start, int n_End, int array[]){
    if(n_Start >= n_End){
        return;
    }
    int left = n_Start, right = n_End;
    int x = array[left + ((right - left) / 2)], c;
    while(left <= right){
        while(array[left] < x){
            left++;
        }
        while(array[right] > x){
            right--;
        }
        if(left <= right){
            c = array[left];
            array[left] = array[right];
            array[right] = c;
            left++;
            right--;
        }
    }
    Quick_Sort(n_Start, right, array);
    Quick_Sort(left, n_End, array);
}


int main(){
    int N;
    if(scanf("%d", &N) != 1){
        return 0;
    }
    int* array = (int*) malloc(sizeof(int) * N);
    for(int i = 0; i < N; i++){
        if(scanf("%d", &array[i]) != 1){
            free(array);
            return 0;
        }
    }
    Quick_Sort(0, N - 1, array);
    for(int i = 0; i < N; i++){
        printf("%d ", array[i]);
    }
    free(array);
    return 0;
}