#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

int size = 5;
int a = -1;

struct arguments{
    int thread_number;
};

void *mythread(void *arg) {
    struct arguments *arg_ = (struct arguments *) arg;

    int x = 1;
    static int y = 0;
    const int z = 3;

    printf("\n"
           "mythread [%d %d %d]: Hello from thread %d!\n"
           "id from function pthread_self(): %lu\n"
           "&global_var = %p\n"
           "&local_var = %p\n"
           "&local_static_var = %p\n"
           "&const_var = %p\n\n", getpid(), getppid(), gettid(), arg_->thread_number, pthread_self(), &size, &x, &y, &z);


    printf("\nthread %d: a = %d; y = %d\n\n", arg_->thread_number, a, y);

    a = arg_->thread_number;
    y = arg_->thread_number;

    printf("\nNEW thread %d: a = %d; y = %d\n\n", arg_->thread_number, a, y);

    sleep(20);

    return NULL;
}

int main() {
    pthread_t *tid = (pthread_t *) malloc(sizeof(pthread_t) * size);
    struct arguments *arguments = (struct arguments *) malloc(sizeof(struct arguments) * size);
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    for(int i = 0; i < size; i++){
        arguments[i].thread_number = i;
        err = pthread_create(tid + i, NULL, mythread, arguments + i);
        if (err) {
            printf("main: pthread_create() with number %d failed: %s\n", i, strerror(err));
            return -1;
        }
        else{
            printf("main: thread with number %d and tid number %lu created successfully\n", i, tid[i]);
        }
    }

    for(int i = 0; i < size; i++){
        if(pthread_join(tid[i], NULL)) {
            printf("Error join\n");
            return -1;
        }
    }

    free(tid);
    free(arguments);

    return 0;
}

