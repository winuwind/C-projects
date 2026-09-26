#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>


#define SIZE 256


char FLAG;


typedef struct tree_t{
    int data;
    unsigned int symbol;
    struct tree_t *left, *right, *root;
}tree;


tree* Creat_tree(int x){
    tree* Tree = (tree*) malloc(sizeof(tree));
    Tree->data = x;
    Tree->right = NULL;
    Tree->left = NULL;
    Tree->root = NULL;
    return Tree;
}


void Destroy_tree(tree **Tree){
    if((*Tree)->left != NULL){
        Destroy_tree(&((*Tree)->left));
        (*Tree)->left = NULL;
    }
    if((*Tree)->right != NULL){
        Destroy_tree(&((*Tree)->right));
        (*Tree)->right = NULL;
    }
    free((*Tree));
}


void Dict_to_tree(tree* Tree, char Code_Symbol[SIZE + 1][SIZE]){
    tree* help = Tree;
    for(unsigned int i = 0; i < SIZE; i++){
        for(unsigned int j = 0; j < strlen(Code_Symbol[i]) + 1; j++){
            if(Code_Symbol[i][j] == '\0'){
                if(j != 0){
                    help->symbol = i;
                    help = Tree;
                }
            }
            else if(Code_Symbol[i][j] == '0'){
                if(help->left == NULL) {
                    help->left = (tree*) malloc(sizeof(tree));
                    help->left->left = NULL;
                    help->left->right = NULL;
                    help->left->data = 0;
                    help->left->root = help;
                }
                help->symbol = SIZE;
                help = help->left;
            }
            else if(Code_Symbol[i][j] == '1'){
                if(help->right == NULL) {
                    help->right = (tree*) malloc(sizeof(tree));
                    help->right->left = NULL;
                    help->right->right = NULL;
                    help->right->data = 0;
                    help->right->root = help;
                }
                help->symbol = SIZE;
                help = help->right;
            }
        }
    }
}


void From_dec(char* str, size_t x){
    for(unsigned int i = 0, weight = (unsigned int) INT_MAX + 1; i < 32; i++, weight >>= 1) {
        str[i] = (char) (((x & weight) > 0) + '0');
    }
    str[32] = '\0';
}


unsigned int To_dec(unsigned char* str, int start){
    unsigned int x = 0;
    for(unsigned int weight = 128; weight > 0; start++, weight >>= 1){
        if(str[start] == '1'){
            x += weight;
        }
    }
    return x;
}


void Quick_Sort(unsigned int n_Start, unsigned int n_End, int array[], unsigned int Index[]){
    if(n_Start >= n_End){
        return;
    }
    unsigned int left = n_Start, right = n_End;
    int x = array[left + ((right - left) / 2)], c, ii = 0, jj = 0;
    unsigned int c1;
    while(left <= right){
        while(left < n_End && array[left] < x){
            left++;
        }
        while(right > 0 && array[right] > x){
            right--;
        }
        if(left <= right){
            if(left != right) {
                c = array[left];
                array[left] = array[right];
                array[right] = c;
                for (int i = 0; i < SIZE; i++) {
                    if (Index[i] == left) {
                        ii = i;
                    } else if (Index[i] == right) {
                        jj = i;
                    }
                }
                c1 = Index[ii];
                Index[ii] = Index[jj];
                Index[jj] = c1;
            }
            if(right > 0){
                right--;
            }
            if(left < SIZE){
                left++;
            }
        }
    }
    Quick_Sort(n_Start, right, array, Index);
    Quick_Sort(left, n_End, array, Index);
}


unsigned int get_symbol(unsigned char* str, unsigned int len, unsigned int start, char Code_Symbol[SIZE][SIZE], unsigned int Len_Code[SIZE]){
    for(unsigned int i = 0, j; i < SIZE; i++){
        if(len - start < Len_Code[i]){
            continue;
        }
        for(j = 0; j < Len_Code[i] && str[start + j] == (unsigned char) Code_Symbol[i][j]; j++);
        if(j == Len_Code[i] && j != 0){
            return i;
        }
    }
    return SIZE;
}


unsigned char* bits_to_bytes(char* str){
    unsigned int size = strlen(str) / 8;
    unsigned char* answer = (unsigned char*) malloc(sizeof(unsigned char) * (size + 1));
    for(unsigned int i = 0; i < size; i++){
        unsigned char copy_str[8];
        for(int ii = 0; ii < 8; ii++){
            copy_str[ii] = (unsigned char) str[i * 8 + ii];
        }
        answer[i] = (unsigned char) To_dec(copy_str, 0);
    }
    answer[size] = '\0';
    return answer;
}


unsigned char* bytes_to_bits(unsigned char* str, size_t size, unsigned char *answer){
    char* str_help = (char*) malloc(sizeof(char) * 33);
    for(unsigned int i = 0; i < size; i++){
        From_dec(str_help, (size_t) str[i]);
        for(int j = 0; j < 8; j++){
            answer[i * 8 + j] = (unsigned char) str_help[j + 24];
        }
    }
    free(str_help);
    return answer;
}


unsigned int Code_tree(char* str, unsigned int index, tree* Tree, unsigned int index_reserve){
    unsigned int index_x = index;
    if(Tree->left != NULL){
        str[index] = '1';
        index_x = Code_tree(str, index + 1, Tree->left, index_reserve);
    }
    if(Tree->right != NULL){
        str[index_x] = '1';
        index_x = Code_tree(str, index_x + 1, Tree->right, index_reserve);
    }
    if(index_x != index_reserve){
        if(index_x == index) {
            str[index_x - 1] = '0';
            char *symbol_in_bits = (char *) malloc(sizeof(char) * 33);
            From_dec(symbol_in_bits, (size_t) Tree->symbol);
            for (unsigned int i = 0; i < 8; i++) {
                str[index_x + i] = symbol_in_bits[i + 24];
            }
            free(symbol_in_bits);
            index_x += 8;
        }
    }
    return index_x;
}


unsigned int Size_tree(unsigned char* str, unsigned int index){
    unsigned int count_0 = 0, count_1 = 0;
    while(count_1 + 2 > count_0){
        if(str[index] == '1'){
            count_1++;
        }
        else if(str[index] == '0'){
            count_0++;
            index += 8;
        }
        index++;
    }
    return index;
}


unsigned int Decode_tree(tree* Tree, unsigned char* str, unsigned int size, unsigned int index, char Code_Symbol[SIZE + 1][SIZE], unsigned int Len_Code[SIZE], char* str_help, unsigned int size_str){
    if(index == size){
        return index;
    }
    if(str[index++] == '1'){
        if(Tree->left == NULL) {
            Tree->left = (tree*) malloc(sizeof(tree));
            Tree->left->left = NULL;
            Tree->left->right = NULL;
            Tree->left->symbol = SIZE;
            Tree->left->root = Tree;
            str_help[size_str] = '0';
            index = Decode_tree(Tree->left, str, size, index, Code_Symbol, Len_Code, str_help, size_str + 1);
        }
        else if(Tree->right == NULL){
            Tree->right = (tree*) malloc(sizeof(tree));
            Tree->right->left = NULL;
            Tree->right->right = NULL;
            Tree->right->symbol = SIZE;
            Tree->right->root = Tree;
            str_help[size_str] = '1';
            index = Decode_tree(Tree->right, str, size, index, Code_Symbol, Len_Code, str_help, size_str + 1);
        }
        else{
            index = Decode_tree(Tree->root, str, size, index - 1, Code_Symbol, Len_Code, str_help, size_str - 1);
        }
    }
    else{
        if(Tree->left == NULL) {
            Tree->left = (tree*) malloc(sizeof(tree));
            Tree->left->left = NULL;
            Tree->left->right = NULL;
            Tree->left->root = Tree;
            Tree->left->symbol = To_dec(str, (int) index);
            index += 8;
            str_help[size_str++] = '0';
            Len_Code[Tree->left->symbol] = size_str;
            for(unsigned int i = 0; i < size_str; i++){
                Code_Symbol[Tree->left->symbol][i] = str_help[i];
            }
            Code_Symbol[Tree->left->symbol][size_str] = '\0';
            index = Decode_tree(Tree, str, size, index, Code_Symbol, Len_Code, str_help, size_str - 1);
        }
        else if(Tree->right == NULL){
            Tree->right = (tree*) malloc(sizeof(tree));
            Tree->right->left = NULL;
            Tree->right->right = NULL;
            Tree->right->root = Tree;
            Tree->right->symbol = (unsigned int) To_dec(str, (int) index);
            index += 8;
            str_help[size_str++] = '1';
            Len_Code[Tree->right->symbol] = size_str;
            for(unsigned int i = 0; i < size_str; i++){
                Code_Symbol[Tree->right->symbol][i] = str_help[i];
            }
            Code_Symbol[Tree->right->symbol][size_str] = '\0';
            index = Decode_tree(Tree, str, size, index, Code_Symbol, Len_Code, str_help, size_str - 1);
        }
        else{
            index = Decode_tree(Tree->root, str, size, index - 1, Code_Symbol, Len_Code, str_help, size_str - 1);
        }
    }
    return index;
}


void Compression_create_codes(unsigned int* Index, int* Summa, size_t count, char Code_symbol[SIZE + 1][SIZE]){
    unsigned int* Index1 = (unsigned int*) malloc(sizeof(unsigned int) * SIZE);
    int* Summa1 = (int*) malloc(sizeof(int) * count), c;
    for(unsigned int i = 0; i < count; i++){
        Summa1[i] = Summa[i + 1];
        if(i == 0){
            Summa1[0] += Summa[0];
        }
    }
    for(int i = 0; i < SIZE; i++){
        Code_symbol[i][0] = '\0';
        if(Index[i] == 0){
            Index1[i] = 0;
        }
        else{
            Index1[i] = Index[i] - 1;
        }
    }
    for(unsigned int i = 1, ii = 0; i < count; i++){
        if(Summa1[ii] > Summa1[i]){
            c = Summa1[ii]; Summa1[ii] = Summa1[i]; Summa1[i] = c;
            for(int j = 0; j < SIZE; j++){
                if(Index1[j] == i){
                    Index1[j] = ii;
                }
                else if(Index1[j] == ii){
                    Index1[j] = i;
                }
            }
            ii = i;
        }
        else{
            break;
        }
    }
    if(count > 2) {
        Compression_create_codes(Index1, Summa1, count - 1, Code_symbol);
    }
    else{
        for(int i = 0; i < SIZE; i++){
            if(Index1[i] == 0){
                Code_symbol[i][0] = '0';
                Code_symbol[i][1] = '\0';
            }
            else if(Index1[i] == 1){
                Code_symbol[i][0] = '1';
                Code_symbol[i][1] = '\0';
            }
        }
        free(Index1);
        free(Summa1);
        return;
    }
    for(int i = 0; i < SIZE; i++){
        unsigned int len = strlen(Code_symbol[i]);
        if(Index1[i] == 0){

            Code_symbol[i][len] = '0';
            Code_symbol[i][len + 1] = '\0';
        }
        else if(Index1[i] == 1){
            Code_symbol[i][len] = '1';
            Code_symbol[i][len + 1] = '\0';
        }
    }
    free(Index1);
    free(Summa1);
}


int Compression_call_create_codes(int Count_symbol[], unsigned int Index[], char Code_symbol[SIZE + 1][SIZE]){
    unsigned int count = 0;
    int f = 0;
    Quick_Sort(0, SIZE - 1, Count_symbol, Index);
    while(count < SIZE && Count_symbol[count] == 0){
        count++;
    }
    int* summa = (int*) malloc(sizeof(int) * (256 - count));
    for(unsigned int i = 0; i < SIZE - count; i++){
        summa[i] = Count_symbol[i + count];
    }
    for(int i = 0; i < SIZE; i++){
        if(Index[i] >= count){
            Index[i] = Index[i] - count;
        }
        else{
            Index[i] = SIZE;
        }
    }
    if(SIZE - count > 2) {
        Compression_create_codes(Index, summa, SIZE - count - 1, Code_symbol);
    }
    else if(SIZE - 1 == count){
        f = 1;
    }
    free(summa);
    for(int i = 0; i < SIZE; i++){
        unsigned int len = strlen(Code_symbol[i]);
        if(Index[i] == 0){
            Code_symbol[i][len] = '0';
            Code_symbol[i][len + 1] = '\0';
        }
        else if(Index[i] == 1){
            Code_symbol[i][len] = '1';
            Code_symbol[i][len + 1] = '\0';
        }
    }
    return f;
}


unsigned int Get_size(FILE *fp, char Code_symbol[SIZE + 1][SIZE], char* answer){
    rewind(fp);
    unsigned int index = 5, size;
    if(FLAG == '0'){
        size = fgetc(fp);
    }
    tree* Tree = Creat_tree(-1);
    Dict_to_tree(Tree, Code_symbol);
    index = Code_tree(answer, index, Tree, index);
    Destroy_tree(&Tree);
    answer[index] = '\0';
    unsigned char* Str = (unsigned char*) malloc(sizeof(char) * 100000);
    do{
        size = fread(Str, sizeof(char), 100000, fp);
        for(unsigned int i = 0; i < size; i++){
            index += strlen(Code_symbol[Str[i]]);
        }
    }while(size > 0);
    free(Str);
    return index;
}


void Print_answer(FILE *fp, FILE *fp2, char Code_symbol[SIZE + 1][SIZE], char* answer){
    unsigned char *str = (unsigned char*) malloc(sizeof(unsigned char) * 50000), *result;
    unsigned int size, index = strlen(answer);
    do{
        size = fread(str, sizeof(char), 50000, fp);
        for(unsigned int i = 0; i < size; i++){
            for(unsigned int j = 0; j < strlen(Code_symbol[str[i]]); index++, j++){
                if(index % 8 == 0){
                    answer[index] = '\0';
                    result = bits_to_bytes(answer);
                    fwrite(result, sizeof(char), index / 8, fp2);
                    free(result);
                    index = 0;
                }
                answer[index] = Code_symbol[str[i]][j];
            }
        }
    }while(size != 0);
    while(index % 8 != 0){
        answer[index++] = '0';
    }
    answer[index] = '\0';
    result = bits_to_bytes(answer);
    fwrite(result, sizeof(char), index / 8, fp2);
    free(result);
    free(answer);
    free(str);
}


void Compression(FILE *fp, FILE *fp2, char Code_symbol[SIZE + 1][SIZE], int Count_symbol[SIZE], unsigned int Index[SIZE]){
    unsigned char* Str = (unsigned char*) malloc(sizeof(unsigned char) * 50000);
    unsigned int size;
    do{
        size = fread(Str, sizeof(unsigned char), 50000, fp);
        for(unsigned int i = 0; i < size; i++){
            Count_symbol[Str[i]]++;
        }
    }while(size > 0);
    int flag =  Compression_call_create_codes(Count_symbol, Index, Code_symbol);
    char* answer = (char*) malloc(sizeof(char) * 1000000);
    unsigned int count_zero, index;
    answer[0] = '0';
    if(flag){
        answer[4] = '1';
    }
    else{
        answer[4] = '0';
    }
    index = Get_size(fp, Code_symbol, answer);
    count_zero = 8 - index % 8;
    char *code = (char *) malloc(sizeof(char) * 33);
    From_dec(code, (size_t) count_zero);
    for (int i = 1; i < 4; i++) {
        answer[i] = code[i + 28];
    }
    free(code);
    rewind(fp);
    free(Str);
    if(FLAG == '0') {
        if (fgetc(fp) == EOF) {
            free(answer);
            return;
        }
    }
    Print_answer(fp, fp2, Code_symbol, answer);
}


void Decode_code_elements(unsigned char* Str_in_bits, char Code_symbol[SIZE + 1][SIZE], unsigned int Len_code[SIZE + 1], unsigned int* start, unsigned int* flag){
    tree *Tree = Creat_tree(-1);
    char *str_help = (char *) malloc(sizeof(char) * 256);
    if(Str_in_bits[0] == '0') {
        if (Str_in_bits[4] == '1') {
            *start = 14;
            *flag = 1;
        } else {
            *start = Size_tree(Str_in_bits, 5);
        }
        *start = Decode_tree(Tree, Str_in_bits, *start, 5, Code_symbol, Len_code, str_help, 0);
    }
    else{
        if (Str_in_bits[1] == '1') {
            *start = 11;
            *flag = 1;
        } else {
            *start = Size_tree(Str_in_bits, 2);
        }
        *start = Decode_tree(Tree, Str_in_bits, *start, 2, Code_symbol, Len_code, str_help, 0);
    }
    Destroy_tree(&Tree);
    free(str_help);
}


void Decompression(FILE *fp, FILE *fp2, char Code_symbol[SIZE + 1][SIZE]){
    unsigned char symbol, *Str = (unsigned char*) malloc(sizeof(unsigned char) * 50000), str_out[1024], *str_bits_help, *Str_in_bits = (unsigned char*) malloc(sizeof(unsigned char) * 50000 * 8);
    unsigned int Len_code[SIZE + 1], max_len = 0, start, count_zero = 0, size, symbol_max_len = 0, flag = 0, count_out = 0;
    size = fread(Str, sizeof(unsigned char), 50000, fp);
    Str_in_bits = bytes_to_bits(Str, size, Str_in_bits);
    size *= 8;
    for(unsigned int i = 0; i <= SIZE; i++){
        Len_code[i] = 0;
    }
    for (int i = 3; i >= 1; i--) {
        if (Str_in_bits[i] == '1') {
            count_zero += (unsigned int) pow(2, 3 - i);
        }
    }
    Decode_code_elements(Str_in_bits, Code_symbol, Len_code, &start, &flag);
    for (int i = 0; i < SIZE; i++) {
        if (max_len < Len_code[i]) {
            max_len = Len_code[i];
            symbol_max_len = i;
        }
    }
    str_bits_help = (unsigned char *) malloc(sizeof(unsigned char) * (max_len + count_zero));
    if(flag){
        symbol = (unsigned char) symbol_max_len;
        while(size != 0){
            if(start == 0){
                for(unsigned int i = 0; i < count_zero; i++){
                    str_out[count_out++] = symbol;
                    if(count_out == 1024) {
                        fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                        count_out = 0;
                    }
                }
            }
            for(; start < size - count_zero; start++){
                str_out[count_out++] = symbol;
                if(count_out == 1024) {
                    fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                    count_out = 0;
                }
            }
            size = fread(Str, sizeof(unsigned char), 50000, fp) * 8;
            start = 0;
            Str_in_bits = bytes_to_bits(Str, size / 8, Str_in_bits);
        }
    }
    else {
        unsigned int index_help, len_help, symbol_number;
        for (; start + max_len + count_zero < size;) {
            symbol_number = get_symbol(Str_in_bits, size, start, Code_symbol, Len_code);
            start += Len_code[symbol_number];
            if ((unsigned int) symbol_number < SIZE) {
                symbol = (unsigned char) symbol_number;
                str_out[count_out++] = symbol;
                if(count_out == 1024) {
                    fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                    count_out = 0;
                }
            }
        }
        for (int i = 0; i + start < size; i++) {
            str_bits_help[i] = Str_in_bits[i + start];
        }
        len_help = size - start;
        do {
            size = fread(Str, sizeof(unsigned char), 50000, fp);
            start = 0;
            if (size == 0) {
                while (start + count_zero < len_help) {
                    symbol_number = get_symbol(str_bits_help, len_help, start, Code_symbol, Len_code);
                    start += Len_code[symbol_number];
                    if (symbol_number < SIZE) {
                        symbol = (unsigned char) symbol_number;
                        str_out[count_out++] = symbol;
                        if(count_out == 1024) {
                            fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                            count_out = 0;
                        }
                    } else {
                        return;
                    }
                }
                break;
            }
            Str_in_bits = bytes_to_bits(Str, size, Str_in_bits);
            size *= 8;
            for (unsigned int i = 0; i + len_help < max_len + count_zero && i < size; i++) {
                str_bits_help[i + len_help] = Str_in_bits[i];
            }
            if(size >= max_len + count_zero - len_help) {
                index_help = max_len + count_zero - len_help;
            }
            else{
                index_help = size;
            }
            len_help = max_len + count_zero;
            while (start < len_help) {
                symbol_number = get_symbol(str_bits_help, len_help, start, Code_symbol, Len_code);
                start += Len_code[symbol_number];
                if (symbol_number < SIZE) {
                    symbol = (unsigned char) symbol_number;
                    str_out[count_out++] = symbol;
                    if(count_out == 1024) {
                        fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                        count_out = 0;
                    }
                } else {
                    for (unsigned int i = 0; i + start < len_help; i++) {
                        str_bits_help[i] = str_bits_help[i + start];
                    }
                    for (unsigned int i = len_help - start; i < len_help && i + start - len_help + index_help + count_zero < size; i++) {
                        str_bits_help[i] = Str_in_bits[i + start - len_help + index_help];
                    }
                    if(start - len_help + index_help + count_zero + len_help > size){
                        str_bits_help[-start + len_help - index_help - count_zero + size] = '\0';
                        len_help = -start + len_help - index_help - count_zero + size;
                    }
                    symbol_number = get_symbol(str_bits_help, len_help, 0, Code_symbol, Len_code);
                    if (symbol_number < SIZE) {
                        symbol = (unsigned char) symbol_number;
                        str_out[count_out++] = symbol;
                        if(count_out == 1024) {
                            fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                            count_out = 0;
                        }
                    }
                    start += Len_code[symbol_number] + index_help - len_help;
                    break;
                }
            }
            if(start == len_help){
                start = index_help;
            }
            for (; start + max_len + count_zero < size;) {
                symbol_number = get_symbol(Str_in_bits, size, start, Code_symbol, Len_code);
                start += Len_code[symbol_number];
                if (symbol_number < SIZE) {
                    symbol = (unsigned char) symbol_number;
                    str_out[count_out++] = symbol;
                    if(count_out == 1024) {
                        fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                        count_out = 0;
                    }
                }
            }
            for (int i = 0; i + start < size; i++) {
                str_bits_help[i] = (unsigned char) Str_in_bits[i + start];
            }
            len_help = size - start;
        } while (size > 0);
    }
    fwrite(str_out, sizeof(unsigned char), count_out, fp2);
    free(str_bits_help);
    free(Str_in_bits);
    free(Str);
}


size_t Count_size(FILE *fp, char Code_symbol[SIZE + 1][SIZE]){
    size_t index = 0, size;
    unsigned char* Str = (unsigned char*) malloc(sizeof(char) * 100000);
    do{
        size = fread(Str, sizeof(char), 100000, fp);
        for(unsigned int i = 0; i < size; i++){
            index += strlen(Code_symbol[Str[i]]);
        }
    }while(size > 0);
    free(Str);
    rewind(fp);
    return index;
}


void Print_dict(FILE* fp2, char Code_symbol[SIZE + 1][SIZE], int flag){
    char* answer = (char*) malloc(sizeof(char) * 2569);
    unsigned index = 2;
    answer[0] = '1';
    if(flag){
        answer[1] = '1';
    }
    else{
        answer[1] = '0';
    }
    tree* Tree = Creat_tree(-1);
    Dict_to_tree(Tree, Code_symbol);
    index = Code_tree(answer, index, Tree, index);
    Destroy_tree(&Tree);
    for(; index < 2568; index++){
        answer[index] = '0';
    }
    unsigned char* result = bits_to_bytes(answer);
    fwrite(result, sizeof(unsigned char), 321, fp2);
    free(answer);
    free(result);
}


void Directory_traversal_char_counter(char* path, unsigned int len_path, FILE *fp2, int Count_symbol[SIZE]){
    FILE* fp = fopen(path, "rb");
    struct dirent* dp;
    if (fp == NULL) {
        DIR *dir = opendir(path);
        while ((dp = readdir(dir)) != NULL) {
            if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0){
                continue;
            }
            char *path_2 = (char *) malloc(sizeof(char) * (strlen(dp->d_name) + 2 + len_path));
            for (unsigned int i = 0; i < len_path; i++) {
                path_2[i] = path[i];
            }
            path_2[len_path] = '\\';
            for (unsigned int i = 0; i < strlen(dp->d_name); i++) {
                path_2[i + 1 + len_path] = dp->d_name[i];
            }
            path_2[strlen(dp->d_name) + 1 + len_path] = 0;
            Directory_traversal_char_counter(path_2, len_path + 1 + strlen(dp->d_name), fp2, Count_symbol);
            free(path_2);
        }
    }
    else{
        unsigned int size;
        unsigned char *Str = (unsigned char*) malloc(sizeof(unsigned char) * 50000);
        do{
            size = fread(Str, sizeof(unsigned char), 50000, fp);
            for(unsigned int i = 0; i < size; i++){
                Count_symbol[Str[i]]++;
            }
        }while(size > 0);
        free(Str);
        fclose(fp);
    }
}


void Directory_traversal_compress(char* path, unsigned int len_path, FILE *fp2, char Code_symbol[SIZE + 1][SIZE], char* name, unsigned int name_len){
    FILE* fp = fopen(path, "rb");
    struct dirent* dp;
    char* answer = (char*) malloc(sizeof(char) * 2500);
    char* str_help = (char*) malloc(sizeof(char) * 33);
    if (fp == NULL) {
        unsigned int start = 0;
        for(unsigned i = 0; i < name_len; i++){
            if(name[i] == '\\'){
                start = i + 1;
            }
        }
        answer[0] = '1';
        From_dec(str_help, name_len - start);
        for(int i = 0; i < 9; i++){
            answer[i + 1] = str_help[i + 23];
        }
        for(unsigned int i = 0; i < 260; i++){
            if(i >= name_len - start){
                From_dec(str_help, 0);
            }
            else {
                From_dec(str_help, (size_t) name[i + start]);
            }
            for(unsigned int j = 0; j < 8; j++){
                answer[10 + i * 8 + j] = str_help[j + 24];
            }
        }
        answer[2090] = '0'; answer[2091] = '0'; answer[2092] = '0'; answer[2093] = '0'; answer[2094] = '0'; answer[2095] = '0'; answer[2096] = 0;
        unsigned char* result = bits_to_bytes(answer);
        fwrite(result, sizeof(unsigned char), 262, fp2);
        free(result);
//        "1!!!!!!!!!!!!!!!!сжатаядиректория(********name(0,1)(если 0:!!!!!!!!сжатый файл)********"                 //восклицательные знаки - за место них размер сжатой директории или файла, * - размер имени, (0,1) показатель, идет директория или файл.
        DIR *dir = opendir(path);
        while ((dp = readdir(dir)) != NULL) {
            if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0){
                continue;
            }
            char *path_2 = (char *) malloc(sizeof(char) * (strlen(dp->d_name) + 2 + len_path));             //1(тип директория)000000000(размер названия)0...0(8*260имя директории)000000(незначащие ноли перед следующим шагом)
            for (unsigned int i = 0; i < len_path; i++) {
                path_2[i] = path[i];
            }
            path_2[len_path] = '\\';
            for (unsigned int i = 0; i < strlen(dp->d_name); i++) {
                path_2[i + 1 + len_path] = dp->d_name[i];
            }
            path_2[strlen(dp->d_name) + 1 + len_path] = 0;
            From_dec(str_help, name_len);
            Directory_traversal_compress(path_2, len_path + 1 + strlen(dp->d_name), fp2, Code_symbol, dp->d_name, strlen(dp->d_name));
//            if(strcmp(dp->d_name, "diff.txt") == 0){
//                return;
//            }
            free(path_2);
        }
        char end_direct[17] = "11111111";                              //для новой рекурсии при раскодировании 1 - тип директория, 111111111 - число 512 > 260 - максмимальная длина имени - конец директории, верхней в стеке.
        result = bits_to_bytes(end_direct);
        fwrite(result, sizeof(unsigned char), 1, fp2);
        free(result);
        free(answer);
    }
    else{
        unsigned int index; //0,000000000,0...0(260*8 0и1 - имя файла),00000000000000000000000000000000(размер файла),000(кол-во нулей),0...0(закодированный файл),0...0(незначащие нули, чтобы всю строку до кратности 8 довести)
        From_dec(str_help, (size_t) name_len);
        answer[0] = '0';
        for(int i = 0; i < 9; i++){
            answer[i + 1] = str_help[i + 23];
        }
        for(unsigned int i = 0; i < 260; i++){
            if(i >= name_len){
                From_dec(str_help, 0);
            }
            else {
                From_dec(str_help, (size_t) name[i]);
            }
            for(unsigned int j = 0; j < 8; j++){
                answer[10 + i * 8 + j] = str_help[j + 24];
            }
        }
        answer[2090] = '0'; answer[2091] = '0'; answer[2092] = '0'; answer[2093] = '0'; answer[2094] = '0'; answer[2095] = '0'; answer[2096] = 0;
        unsigned char* result = bits_to_bytes(answer);
        fwrite(result, sizeof(unsigned char), 262, fp2);
        free(result);
//        if(strcmp(name, "diff.txt") == 0){
//            fclose(fp2);
//            return;
//        }
        size_t size = Count_size(fp, Code_symbol);
        index = (unsigned int) (size % 8);
        if(index != 0) {
            From_dec(str_help, (size + 8 - (size_t) index) / 8);
        }
        else{
            From_dec(str_help, size / 8);
        }
        for(unsigned int i = 0; i < 32; i++){
            answer[i] = str_help[i];
        }
        if(index != 0) {
            From_dec(str_help, 8 - index);
        }
        else{
            From_dec(str_help, 0);
        }
        for(unsigned int i = 24; i < 32; i++){
            answer[8 + i] = str_help[i];
        }
        answer[40] = 0;
        Print_answer(fp, fp2, Code_symbol, answer);
        fclose(fp);
    }
    free(str_help);
}


void Decompression_for_directory(FILE *fp, char* path_0, char Code_symbol[SIZE + 1][SIZE]){
    unsigned char *Str = (unsigned char*) malloc(sizeof(unsigned char) * 50000), *Str_in_bits = (unsigned char*) malloc(sizeof(unsigned char) * 50000 * 8);
    unsigned int len_path = strlen(path_0), Len_code[SIZE + 1], max_len = 0, start, size, symbol_max_len = 0, flag = 0, size_arr_name = 0;
    char name[261], *path = (char*) malloc(sizeof(char) * len_path + 1), Arr_name[20][261];
    for(unsigned int i = 0; i <= len_path; i++){
        path[i] = path_0[i];
    }
    FILE *fp2;
    size = fread(Str, sizeof(unsigned char), 321, fp);
    Str_in_bits = bytes_to_bits(Str, size, Str_in_bits);
    for(unsigned int i = 0; i <= SIZE; i++){
        Len_code[i] = 0;
    }
    Decode_code_elements(Str_in_bits, Code_symbol, Len_code, &start, &flag);
    for (int i = 0; i < SIZE; i++) {
        if (max_len < Len_code[i]) {
            max_len = Len_code[i];
            symbol_max_len = i;
        }
    }
    while(1) {
        unsigned char type;
        unsigned int len_name = 0;
        size = fread(Str, sizeof(unsigned char), 1, fp);
        Str_in_bits = bytes_to_bits(Str, size, Str_in_bits);
        type = Str_in_bits[0];
        for(unsigned int weight = 256, i = 1; i < 8; i++, weight >>= 1){
            if(Str_in_bits[i] == '1'){
                len_name += weight;
            }
        }
        if(type == '1' && len_name > 260){
            len_path -= (strlen(Arr_name[size_arr_name - 1]) + 1);
            path[len_path] = 0;
            Arr_name[size_arr_name--][0] = 0;
            if(size_arr_name == 0){
                break;
            }
            continue;
        }
        size = fread(Str, sizeof(unsigned char), 261, fp);
        Str_in_bits = bytes_to_bits(Str, size, Str_in_bits);
        len_name += (unsigned int) (Str_in_bits[0] - '0') * 2 + (unsigned int) (Str_in_bits[1] - '0');
        for (unsigned int i = 0; i < len_name; i++) {
            name[i] = (char) To_dec(Str_in_bits, 2 + (int) i * 8);
        }
        char* path_h = (char *) realloc(path, sizeof(char) * (len_name + 2 + len_path));
        if(path_h == NULL){
            free(Str);
            free(Str_in_bits);
            free(path);
            return;
        }
        path = path_h;
        path[len_path] = '\\';
        for (unsigned int i = 0; i < len_name; i++) {
            path[i + 1 + len_path] = name[i];
        }
        path[len_name + 1 + len_path] = 0;
        if (type == '1') {
            char* command = (char*) malloc(sizeof(char) * (7 + strlen(path)));
            command[0] = 'm'; command[1] = 'k'; command[2] = 'd'; command[3] = 'i'; command[4] = 'r'; command[5] = ' ';
            for(unsigned int i = 0; i < strlen(path) + 1; i++){
                command[i + 6] = path[i];
            }
            if(system(command) != 0){
                printf( "Problem creating directory '%s'\n", path);
                free(path);
                free(command);
                free(Str);
                free(Str_in_bits);
                return;
            }
            for(unsigned int i = 0; i < len_name; i++){
                Arr_name[size_arr_name][i] = name[i];
            }
            Arr_name[size_arr_name][len_name] = 0;
            len_path += len_name + 1;
            size_arr_name++;
            free(command);
            continue;
        }
        else{
            unsigned char symbol, str_out[1024];
            unsigned int count_out = 0, count_zero;
            fp2 = fopen(path, "wb");
            size = fread(Str, sizeof(unsigned char), 5, fp);
            Str_in_bits = bytes_to_bits(Str, size, Str_in_bits);
            size_t size_file = 0;
            for(unsigned int i = 0; i < 4; i++){
                size_file = size_file * 256 + (size_t) To_dec(Str_in_bits, 8 * (int) i);
            }
            count_zero = To_dec(Str_in_bits, 32);
            if(flag){
                symbol = (unsigned char) symbol_max_len;
                while(size_file > 50000){
                    size_file -= 50000;
                    size = fread(Str, sizeof(unsigned char), 50000, fp);
                    Str_in_bits = bytes_to_bits(Str, size, Str_in_bits);
                    size *= 8;
                    for(unsigned int i = 0; i < size; i++){
                        str_out[count_out++] = symbol;
                        if(count_out == 1024) {
                            fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                            count_out = 0;
                        }
                    }
                }
                size = fread(Str, sizeof(unsigned char), size_file, fp);
                Str_in_bits = bytes_to_bits(Str, size, Str_in_bits);
                size *= 8;
                for(unsigned int i = 0; i < size - count_zero; i++){
                    str_out[count_out++] = symbol;
                    if(count_out == 1024) {
                        fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                        count_out = 0;
                    }
                }
            }
            else {
                unsigned char *str_bits_help = (unsigned char *) malloc(sizeof(unsigned char) * (max_len + count_zero));
                unsigned int index_help, len_help = 0, symbol_number;
                start = 0;
                while (size_file > 50000) {
                    size_file -= 50000;
                    size = fread(Str, sizeof(unsigned char), 50000, fp);
                    start = 0;
                    Str_in_bits = bytes_to_bits(Str, size, Str_in_bits);
                    size *= 8;
                    if(len_help != 0) {
                        for (unsigned int i = 0; i + len_help < max_len + count_zero; i++) {
                            str_bits_help[i + len_help] = Str_in_bits[i];
                        }
                        index_help = max_len + count_zero - len_help;
                        len_help = max_len + count_zero;
                        while (start < len_help) {
                            symbol_number = get_symbol(str_bits_help, len_help, start, Code_symbol, Len_code);
                            start += Len_code[symbol_number];
                            if (symbol_number < SIZE) {
                                symbol = (unsigned char) symbol_number;
                                str_out[count_out++] = symbol;
                                if (count_out == 1024) {
                                    fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                                    count_out = 0;
                                }
                            } else {
                                for (unsigned int i = 0; i + start < len_help; i++) {
                                    str_bits_help[i] = str_bits_help[i + start];
                                }
                                for (unsigned int i = len_help - start;
                                     i < len_help && i + start - len_help + index_help + count_zero < size; i++) {
                                    str_bits_help[i] = Str_in_bits[i + start - len_help + index_help];
                                }
                                if (start - len_help + index_help + count_zero + len_help > size) {
                                    str_bits_help[-start + len_help - index_help - count_zero + size] = '\0';
                                    len_help = -start + len_help - index_help - count_zero + size;
                                }
                                symbol_number = get_symbol(str_bits_help, len_help, 0, Code_symbol, Len_code);
                                if (symbol_number < SIZE) {
                                    symbol = (unsigned char) symbol_number;
                                    str_out[count_out++] = symbol;
                                    if (count_out == 1024) {
                                        fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                                        count_out = 0;
                                    }
                                }
                                start += Len_code[symbol_number] + index_help - len_help;
                                break;
                            }
                        }
                        if (start == len_help) {
                            start = index_help;
                        }
                    }
                    for (; start + max_len + count_zero < size;) {
                        symbol_number = get_symbol(Str_in_bits, size, start, Code_symbol, Len_code);
                        start += Len_code[symbol_number];
                        if (symbol_number < SIZE) {
                            symbol = (unsigned char) symbol_number;
                            str_out[count_out++] = symbol;
                            if(count_out == 1024) {
                                fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                                count_out = 0;
                            }
                        }
                    }
                    for (int i = 0; i + start < size; i++) {
                        str_bits_help[i] = (unsigned char) Str_in_bits[i + start];
                    }
                    len_help = size - start;
                }
                size = fread(Str, sizeof(unsigned char), size_file, fp);
                start = 0;
                Str_in_bits = bytes_to_bits(Str, size, Str_in_bits);
                size *= 8;
                for (unsigned int i = 0; i + len_help < max_len + count_zero && i < size; i++) {
                    str_bits_help[i + len_help] = Str_in_bits[i];
                }
                if(size >= max_len + count_zero - len_help) {
                    index_help = max_len + count_zero - len_help;
                }
                else{
                    index_help = size;
                }
                len_help = max_len + count_zero;
                while (start < len_help) {
                    symbol_number = get_symbol(str_bits_help, len_help, start, Code_symbol, Len_code);
                    start += Len_code[symbol_number];
                    if (symbol_number < SIZE) {
                        symbol = (unsigned char) symbol_number;
                        str_out[count_out++] = symbol;
                        if(count_out == 1024) {
                            fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                            count_out = 0;
                        }
                    } else {
                        for (unsigned int i = 0; i + start < len_help; i++) {
                            str_bits_help[i] = str_bits_help[i + start];
                        }
                        for (unsigned int i = len_help - start; i < len_help && i + start - len_help + index_help + count_zero < size; i++) {
                            str_bits_help[i] = Str_in_bits[i + start - len_help + index_help];
                        }
                        if(start - len_help + index_help + count_zero + len_help > size){
                            str_bits_help[-start + len_help - index_help - count_zero + size] = '\0';
                            len_help = -start + len_help - index_help - count_zero + size;
                        }
                        symbol_number = get_symbol(str_bits_help, len_help, 0, Code_symbol, Len_code);
                        if (symbol_number < SIZE) {
                            symbol = (unsigned char) symbol_number;
                            str_out[count_out++] = symbol;
                            if(count_out == 1024) {
                                fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                                count_out = 0;
                            }
                        }
                        start += Len_code[symbol_number] + index_help - len_help;
                        break;
                    }
                }
                if(start == len_help){
                    start = index_help;
                }
                for (; start + count_zero < size;) {
                    symbol_number = get_symbol(Str_in_bits, size, start, Code_symbol, Len_code);
                    start += Len_code[symbol_number];
                    if (symbol_number < SIZE) {
                        symbol = (unsigned char) symbol_number;
                        str_out[count_out++] = symbol;
                        if(count_out == 1024) {
                            fwrite(str_out, sizeof(unsigned char), count_out, fp2);
                            count_out = 0;
                        }
                    }
                }
                free(str_bits_help);
            }
            fwrite(str_out, sizeof(unsigned char), count_out, fp2);
            fclose(fp2);
        }
    }
    free(Str_in_bits);
    free(Str);
}


int main(int count_arg, char *arr_arg[]) {
    FILE *fp = NULL, *fp2 = NULL;
    unsigned char *Str = (unsigned char*) malloc(sizeof(unsigned char) * 10), mode = 0;
    int Count_symbol[SIZE];
    char Code_symbol[SIZE + 1][SIZE];
    unsigned int Index[SIZE], size;
    for(unsigned int i = 0; i < SIZE; i++){
        Code_symbol[i][0] = '\0';
        Count_symbol[i] = 0;
        Index[i] = i;
    }
    Code_symbol[SIZE][0] = 0;
    if(count_arg == 1){
        FLAG = '0';
        fp = fopen("in.txt", "rb");
        fp2 = fopen("out.txt", "wb");
    }
    else {
        FLAG = '1';
        fp = fopen(arr_arg[2], "rb");
        if (fp == NULL) {
            if (strcmp(arr_arg[1], "-c") != 0) {
                printf("the directory can only be compressed");
                free(Str);
                fclose(fp2);
                return 0;
            }
            fp2 = fopen(arr_arg[3], "wb");
            Directory_traversal_char_counter(arr_arg[2], strlen(arr_arg[2]), fp2, Count_symbol);
            int flag = Compression_call_create_codes(Count_symbol, Index, Code_symbol);
            Print_dict(fp2, Code_symbol, flag);
            Directory_traversal_compress(arr_arg[2], strlen(arr_arg[2]), fp2, Code_symbol, arr_arg[2], strlen(arr_arg[2]));
            free(Str);
            fclose(fp2);
            return 0;
        }
        else {
            if (strcmp(arr_arg[1], "-c") == 0) {
                mode = 'c';
            }
            else if (strcmp(arr_arg[1], "-d") == 0) {
                mode = 'd';
                if(fgetc(fp) < 128){
                    fp2 = fopen(arr_arg[3], "wb");
                }
                else{
                    rewind(fp);
                    Decompression_for_directory(fp, arr_arg[3], Code_symbol);
                    fclose(fp);
                    fclose(fp2);
                    free(Str);
                    return 0;
                }
            }
            else {
                printf("Syntax error");
                free(Str);
                fclose(fp);
                fclose(fp2);
                return 0;
            }
        }
    }
    size = fread(Str, sizeof(unsigned char), 10, fp);
    if((FLAG == '0' && size == 1) || (FLAG == '1' && size == 0)){
        fclose(fp);
        fclose(fp2);
        free(Str);
        return 0;
    }
    rewind(fp);
    if(FLAG == '0'){
        mode = fgetc(fp);
    }
    else{
        if (strcmp(arr_arg[1], "-c") == 0) {
            mode = 'c';
        }
        else if (strcmp(arr_arg[1], "-d") == 0) {
            mode = 'd';
        }
    }
    if(mode == 'c'){
        Compression(fp, fp2, Code_symbol, Count_symbol, Index);
    }
    else{
        Decompression(fp, fp2, Code_symbol);
    }
    fclose(fp);
    fclose(fp2);
    free(Str);
    return 0;
}
