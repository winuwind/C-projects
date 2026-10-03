#include <stdio.h>
#include <stdlib.h>

int init = 5;
int notInit;
const int constanta = 10;

int* getAddressLocalVariable(int** pointer){
    int x = 5;
    printf("&local_in_func = %p\n", &x);
    *pointer = &x;
    return &x;
}

int main(void) {
    int value;
    static int value_st;
    const int c = 15;
    int** pointer = (int**) malloc(sizeof(int*));
    int* pointer_to_local = getAddressLocalVariable(pointer);
    printf("&init = %p\n&notInit = %p\n&constanta = %p\n&value = %p\n&value_st = %p\n&c = %p\n&local_variable = %p\npointer = %p, *pointer = %p\n", &init, &notInit, &constanta, &value, &value_st, &c, pointer_to_local, pointer, *pointer);
    FILE *maps = fopen("/proc/self/maps", "r");
    if(maps == NULL){
        fprintf(stderr, "Error when opening file /proc/self/maps\n");
        return -1;
    }
    char line[1025];
    while(fgets(line, 1024, maps)){
        printf("%s", line);
    }
    fclose(maps);
    free(pointer);
    return 0;
}
