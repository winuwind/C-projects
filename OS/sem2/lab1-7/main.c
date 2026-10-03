#include <stdio.h>
#include <unistd.h>
#include <malloc.h>

#include "uthread.h"

my_uthreads_t *control;

void* foo(void* str_v){
    int x = 10;
    while(x--){
        char *str = str_v;
        printf("%s", str);
        if(uthread_sched_yield(control)){
            break;
        }
    }
    return NULL;
}

int main(void) {
    if (!(control = uthread_init())) {
        printf("Error when init uthread\n");
        return 0;
    }
    char* str_first = malloc(100);
    sprintf(str_first, "Hello, ");
    char* str_second = malloc(100);
    sprintf(str_second, "World!\n");
    uthread_t *thread_first;
    if(uthread_create(control, &thread_first, foo, str_first)){
        printf("Error create\n");
        if (uthread_finalize(control)) {
            printf("Error when final uthread\n");
            return 0;
        }
        return 0;
    }
    uthread_t *thread_second;
    if(uthread_create(control, &thread_second, foo, str_second)){
        printf("Error create\n");
        if (uthread_finalize(control)) {
            printf("Error when final uthread\n");
            return 0;
        }
        return 0;
    }

    while(1){
        sleep(1);
        int err = uthread_sched_yield(control);
        if(err){
            break;
        }
    }
    if (uthread_finalize(control)) {
        printf("Error when final uthread\n");
        return 0;
    }
    free(str_first);
    free(str_second);
    return 0;
}


