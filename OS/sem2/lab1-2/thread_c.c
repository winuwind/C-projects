#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>


void* foo(void* arg){
    char *x = "hello world";
    return x;
}

int main(){
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, foo, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    void* res;

    err = pthread_join(tid, &res);
    if(err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    printf("main: returned value from thread: %s\n", (char *) res);
    free(res);
    return 0;
}