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

    free(arg_->str);
    free(arg);
    return NULL;
}

int main() {
    pthread_t tid;
    pthread_attr_t attr;

    struct arguments *arguments = (struct arguments *) malloc(sizeof(struct arguments));
    int err;

    arguments->num = 33232;
    arguments->str = (char *) malloc(sizeof(char) * 100);
    sprintf(arguments->str, "Hello!!! I'm THREAD!\n");


    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    err = pthread_create(&tid, &attr, mythread, arguments);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    pthread_exit(0);
    return 0;
}

