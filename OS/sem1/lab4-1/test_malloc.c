#include <stdio.h>
#include <stdlib.h>

void foo(){
    char* buf = (char*) malloc(100);
    sprintf(buf, "hello world");
    printf("buf = \"%s\"\n", buf);
    free(buf);
    printf("buf = \"%s\"\n", buf);
    buf = (char*) malloc(100);
    sprintf(buf, "hello world");
    printf("buf = \"%s\"\n", buf);
    char* new_buf = &buf[5];
    free(new_buf);
    printf("buf = \"%s\"\n", buf);
    free(buf);
}

int main(){
    foo();
    return 0;
}