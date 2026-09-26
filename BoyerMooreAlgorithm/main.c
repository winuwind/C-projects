#include <stdio.h>
#include <string.h>


void seek_substring_BM(unsigned char* s, unsigned char* q, int N, int M, int start) {
    int d[256];
    int i;
    for (i = 0; i < 256; i++) {
        d[i] = M;
    }
    for (i = 0; i < M - 1; i++) {
        d[(unsigned char) q[i]] = M - i - 1;
    }
    i = M - 1;
    do {
        int j = M - 1;
        int k = i;
        while ((j >= 0) && (q[j] == s[k])) {
            printf("%d ", k + 1 + start);
            k--;
            j--;
        }
        if(j >= 0){
            printf("%d ", k + 1 + start);
        }
        i += d[s[i]];
    }while (i < N);
}

int main() {
    unsigned char pattern[18], str[1026];
    if(fgets((char*) pattern, 18, stdin) == NULL){
        return 0;
    }
    int start = 0, len_p = (int) strlen((char*) pattern) - 1;
    while(fgets((char*) str, 1025, stdin) != NULL) {
        int len_s = (int) strlen((char*) str);
        if(len_s < len_p){
            break;
        }
        seek_substring_BM(str, pattern, len_s, len_p, start);
        start += len_s;
    }
    return 0;
}
