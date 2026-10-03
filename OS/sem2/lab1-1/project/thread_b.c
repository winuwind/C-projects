#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

#define SIZE 5

struct arguments{
    int thread_number;
};

void *mythread(void *arg) {
    struct arguments *arg_ = (struct arguments *) arg;
    printf("mythread [%d %d %d]: Hello from thread %d!\n", getpid(), getppid(), gettid(), arg_->thread_number);
    return NULL;
}

int main() {
    pthread_t *tid = (pthread_t *) malloc(sizeof(pthread_t) * SIZE);
    struct arguments *arguments = (struct arguments *) malloc(sizeof(struct arguments) * SIZE);
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    for(int i = 0; i < SIZE; i++){
        arguments[i].thread_number = i;
        err = pthread_create(tid + i, NULL, mythread, arguments + i);
        if (err) {
            printf("main: pthread_create() with number %d failed: %s\n", i, strerror(err));
            return -1;
        }
    }

    for(int i = 0; i < SIZE; i++){
        if(pthread_join(tid[i], NULL)) {
            printf("Error join\n");
            return -1;
        }
    }

    free(tid);
    free(arguments);

    return 0;
}

