#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>


void *foo(void *arg) {
    printf("thread: TID = %d\n", gettid());
    return NULL;
}

int main() {
    pthread_t tid;
    pthread_attr_t attr;

    int err;

    while (1) {
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        err = pthread_create(&tid, &attr, foo, NULL);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }

//        err = pthread_join(tid, NULL);
//        if (err) {
//            printf("main: pthread_join() failed: %s\n", strerror(err));
//            return -1;
//        }
    }

    return 0;
}