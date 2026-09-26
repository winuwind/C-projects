#include <stdio.h>
#include <string.h>


void Swap(char* P, int i, int j){
    char c = P[j];
    P[j] = P[i];
    P[i] = c;
}


void Reverse(char* P, int j){
    for(int i = 0; i < ((int) strlen(P) - j) / 2; i++){
        Swap(P, j + i, (int) strlen(P) - i - 1);
    }
}


char* Next_P(char* P){
    int j = -1, k = -1;
    for(int i = 0; i < (int) strlen(P) - 1; i++){
        if(i > j && P[i] < P[i + 1]){
            j = i;
            k = i + 1;
        }
        else if(j >= 0 && P[i + 1] > P[j]){
            k = i + 1;
        }
    }
    if(j >= 0){
        Swap(P, j, k);
        Reverse(P, j + 1);
    }
    else{
        return NULL;
    }
    return P;
}


int main() {
    char str[1001], str_n[1001], *P;
    int N, Arr_count[10] = {0};
    if(fgets(str, 1001, stdin) == NULL){
        return 0;
    }
    str[strlen(str) - 1] = 0;
    if(fgets(str_n, 1001, stdin) == NULL){
        return 0;
    }
    if(sscanf(str_n, "%d", &N) != 1){
        return 0;
    }
    for(int i = 0; i < (int) strlen(str); i++){
        if(str[i] < '0' || str[i] > '9'){
            printf("bad input");
            return 0;
        }
        Arr_count[str[i] - '0']++;
    }
    for(int i = 0; i < 10; i++){
        if(Arr_count[i] > 1){
            printf("bad input");
            return 0;
        }
    }
    P = str;
    while(N--){
        P = Next_P(P);
        if(P == NULL){
            break;
        }
        printf("%s\n", P);
    }
    return 0;
}
