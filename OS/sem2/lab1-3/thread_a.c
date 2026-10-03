#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

struct arguments {
    int num;
    char *str;
};

void *mythread(void *arg) {
    struct arguments *arg_ = (struct arguments *) arg;
    printf("mythread [%d %d %d]: Hello from thread!\n", getpid(), getppid(), gettid());
    printf("struct arguments arg{\n\tint num = %d\n\tchar *str = %s\n}\n", arg_->num, arg_->str);
    return NULL;
}

int main() {
    pthread_t tid;
    struct arguments arguments;
    int err;

    arguments.num = 33232;
    arguments.str = (char *) malloc(sizeof(char) * 100);
    sprintf(arguments.str, "Hello!!! I'm THREAD!\n");

    err = pthread_create(&tid, NULL, mythread, &arguments);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    if (pthread_join(tid, NULL)) {
        printf("Error join\n");
        return -1;
    }

    free(arguments.str);

    return 0;
}

