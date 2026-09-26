#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>


unsigned int Hash(unsigned char* str, unsigned int start, unsigned int len, unsigned int hash_prev){
    if(hash_prev == UINT_MAX) {
        unsigned int hash = 0;
        for (unsigned int i = start; i < start + len; i++) {
            hash += ((str[i] % 3) * (unsigned int) pow(3, i - start));
        }
        return hash;
    }
    hash_prev = hash_prev / 3 + (str[start + len - 1] % 3) * (unsigned int) pow(3, len - 1);;
    return hash_prev;
}


void Search(unsigned char* str, unsigned char* pattern, unsigned int start, unsigned int hash, unsigned int len_p, unsigned int len_s){
//    unsigned int len_p = strlen((char*) pattern) - 1, len_s = strlen((char*) str), flag = 0, hash_new = UINT_MAX;
    unsigned int hash_new;
    for(unsigned int i = 0; i < len_s - len_p + 1; i++){
        hash_new = 0;
//        hash_new = Hash(str, i, len_p, UINT_MAX);
        for (unsigned int j = 0; j < len_p; j++) {
            hash_new += ((str[j + i] % 3) * (unsigned int) pow(3, j));
        }
        if(hash_new != hash){
            continue;
        }
        int j = 0;
        for(; j < len_p; j++){
            printf("%u ", start + 1 + i + j);
            if(str[i + j] != pattern[j]){
                break;
            }
        }
    }
}


int main() {
    unsigned char pattern[18];
    unsigned char str[1025];
    unsigned int start = 0;
    if(fgets((char*) pattern, 18, stdin) == NULL){
        return 0;
    }
    unsigned int len_p = strlen((char*) pattern) - 1;
    unsigned int hash = /*Hash(pattern, 0, strlen((char*) pattern) - 1, UINT_MAX)*/0;
    for (unsigned int j = 0; j < len_p; j++) {
        hash += ((pattern[j] % 3) * (unsigned int) pow(3, j));
    }
    printf("%d ", hash);
    while(fgets((char*) str, 100, stdin) != NULL) {
        unsigned int len_s = strlen((char*) str), hash_new;
//        Search(str, pattern, start, hash, len_p, len_s);
        if(len_p > len_s){
            continue;
        }
        for(unsigned int i = 0; i < len_s - len_p + 1; i++, start++){
            hash_new = 0;
//        hash_new = Hash(str, i, len_p, UINT_MAX);
            for (unsigned int j = 0; j < len_p; j++) {
                hash_new += ((str[j + i] % 3) * (unsigned int) pow(3, j));
            }
            if(hash_new != hash) {
                continue;
            }
            for(int j = 0; j < len_p; j++){
                printf("%u ", start + 1 + j);
                if(str[i + j] != pattern[j]){
                    break;
                }
            }
        }
        start += len_p - 1;
    }
    return 0;
}