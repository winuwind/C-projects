#include <stdio.h>
#include <string.h>
#include <malloc.h>


void FuncPrefix(int* prefix, char* pattern){
    prefix[0] = -1;
    for(int j = 1; j <= (int) strlen(pattern); j++) {
        for (int x = j - 1; x >= 0; x--) {
            char flag = 0;
            for (int i = 0; i < x; i++) {
                if (pattern[i] != pattern[i + j - x]) {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) {
                prefix[j] = x;
                break;
            }
        }
    }
}


void Search(char* str, char* pattern, int* arr_prefix, int start, char* copy, int len_p, int len_s){
    int i = 0, len_c = (int) strlen(copy);
    for(int ii = 0; ii < len_c; ii++){
        str[ii] = copy[ii];
    }
    for(int j = 0; i < len_s - len_p + 1;){
        for(int k = i + j; j < len_p; j++, k++){
            if(str[k] != pattern[j]){
                break;
            }
        }
        if(j != 0){
            printf("%d %d ", start + i + 1, j);
        }
        i += j - arr_prefix[j];
        if(j != 0) {
            j = arr_prefix[j];
        }
    }
    copy[0] = 0;
    for(int jj = 0; i < len_s; i++){
        copy[jj++] = str[i];
        copy[jj] = 0;
    }
}


int main() {
    char pattern[18], str[1060], copy[18] = {0};
    char* pointer = fgets(pattern, 18, stdin);
    if(pointer == NULL){
        return 0;
    }
    int len_p = (int) strlen(pattern), start = 0;
    if(len_p == 1){
        return 0;
    }
    int* arr_prefix = (int*) malloc(sizeof(int) * (len_p));
    pattern[--len_p] = 0;
    FuncPrefix(arr_prefix, pattern);
    for(int i = 1; i <= len_p; i++){
        printf("%d ", arr_prefix[i]);
    }
    printf("\n");
    while(fgets(&(str[strlen(copy)]), 1024, stdin) != NULL){
        int len_s = (int) strlen(str);
        Search(str, pattern, arr_prefix, start, copy, len_p, len_s);
        start += len_s - (int) strlen(copy);
    }
    free(arr_prefix);
    return 0;
}