#include <stdio.h>

int Corp_int(void* x, void* y){
    int *a = (int*) x, *b = (int*) y;
    if(*a > *b){
        return 1;
    }
    else if(*a == *b){
        return 0;
    }
    else{
        return -1;
    }
}


void Quick_Sort(void* array, size_t len, size_t size, int(Corp)(void*, void*)){
    if(len == 0){
        return;
    }
    size_t n_Start = 0, n_End = len;
    if(n_Start >= n_End){
        return;
    }
    size_t left = n_Start, right = n_End;
    char *x = (char*) array + (left + ((right - left) / 2)) * size, c;
    while(left < right){
        while(Corp((char*) array + left * size, x) < 0){
            left++;
        }
        while(Corp((char*) array + right * size, x) > 0){
            right--;
        }
        if(left < right){
            for(int i = 0; i < size; i++){
                c = *((char*) array + left * size + i);
                *((char*) array + left * size + i) = *((char*) array + right * size + i);
                *((char*) array + right * size + i) = c;
            }
            left++;
            right--;
        }
    }
    Quick_Sort((char*) array, right, size, Corp);
    Quick_Sort((char*) array + (left + 1) * size, len - (left + 1), size, Corp);
}


int main() {
    int len;
    scanf("%d", &len);
    int array[len];
    for(int i = 0; i < len; i++){
        scanf("%d", &array[i]);
    }
    Quick_Sort(array, len - 1, sizeof(int), Corp_int);
    for(int i = 0; i < len; i++){
        printf("%d ", array[i]);
    }
    return 0;
}
