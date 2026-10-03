#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>


void *mythread(void *arg) {
    while(1){
        printf("Hello, World!\n");
    }
    return NULL;
}

int main() {
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    sleep(1);

    if (pthread_cancel(tid)) {
        printf("Error cancel\n");
        return -1;
    }

    if (pthread_join(tid, NULL)) {
        printf("Error join\n");
        return -1;
    }

    return 0;
}

