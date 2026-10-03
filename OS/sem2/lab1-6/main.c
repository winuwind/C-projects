#include <stdio.h>
#include <unistd.h>

#include "mythread.h"

void *foo(void *_) {
    printf("new thread: (TID) %d\n", gettid());
    return NULL;
}

int main(void) {
    printf("main: (TID) %d\n", gettid());
    if (mythread_init()) {
        printf("Error when init mythread\n");
        return 0;
    }
    int x = 100;
    while (x--) {
        mythread_t *t;
        if (mythread_create(&t, foo, NULL)) {
            printf("Error in mythread_init\n");
            return 0;
        }
        usleep(200);
    }
    if (mythread_finalize()) {
        printf("Error when final mythread\n");
        return 0;
    }
    return 0;
}
