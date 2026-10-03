#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>


void* foo(void* arg){
    sleep(1);
    return NULL;
}

int main(){
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, foo, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_join(tid, NULL);
    if(err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    return 0;
}