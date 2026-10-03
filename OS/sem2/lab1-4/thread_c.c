#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

void my_free(void *arg){
    free(arg);
    printf("Allocated memory are free\n");
}

void *mythread(void *arg) {
    char *str = (char *) malloc(100);
    sprintf(str, "Hello, World!\n");

    pthread_cleanup_push(my_free, str);

    while(1){
        printf("%s", str);
    }

    pthread_cleanup_pop(0);
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

