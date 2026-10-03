#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void* thread_block_all(void *arg) {
    printf("Thread 1: tid = %d, pid = %d\n", gettid(), getpid());

    sigset_t set;
    sigfillset(&set);
    int err = pthread_sigmask(SIG_SETMASK, &set, NULL);
    if(err){
        printf("thread_block_all: pthread_sigmask() failed: %s\n", strerror(err));
        pthread_exit(NULL);
    }
    printf("Thread 1: All signals blocked\n");
    while (1) {
        usleep(10);
    }
    return NULL;
}

void sigint_handler(int signo) {
    printf("Thread %d: SIGINT\n", gettid());
    pthread_exit(NULL);
}

void* thread_sigint(void *arg) {
    printf("Thread 2: tid = %d, pid = %d\n", gettid(), getpid());

    int err;
    int signo;

    sigset_t set;
    sigfillset(&set);

    err = pthread_sigmask(SIG_SETMASK, &set, NULL);
    if(err){
        printf("Thread 2: pthread_sigmask() failed: %s\n", strerror(err));
        pthread_exit(NULL);
    }

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    err = pthread_sigmask(SIG_UNBLOCK, &set, NULL);
    if(err){
        printf("Thread 2: pthread_sigmask() failed: %s\n", strerror(err));
        pthread_exit(NULL);
    }

    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        pthread_exit(NULL);
    }

    printf("Thread 2: Wait SIGINT\n");
    while (1) {
        usleep(10);
    }
    return NULL;
}

void* thread_sigquit(void *arg) {
    printf("Thread 3: tid = %d, pid = %d\n", gettid(), getpid());

    int err;
    int signo;

    sigset_t set;
    sigfillset(&set);

    err = pthread_sigmask(SIG_SETMASK, &set, NULL);
    if(err){
        printf("Thread 3: pthread_sigmask() failed: %s\n", strerror(err));
        pthread_exit(NULL);
    }

    printf("Thread 3: Wait SIGQUIT\n");

    while(1) {
        if (sigwait(&set, &signo) == 0) {
            if (signo == 3) {
                printf("Thread 3: SIGQUIT (%d)\n", signo);
                break;
            }
        }
    }
    return NULL;
}

void sigtstp_handler(int signo) {
    printf("Thread %d: SIGTSTP\n", gettid());
    exit(0);
}

int main() {
    pthread_t tid_1, tid_2, tid_3;

    int err;
    int signo;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGINT);

    err = pthread_sigmask(SIG_SETMASK, &set, NULL);
    if(err){
        printf("main: pthread_sigmask() failed: %s\n", strerror(err));
        return -1;
    }

    struct sigaction sa;
    sa.sa_handler = sigtstp_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        perror("sigaction");
        return -1;
    }

    err = pthread_create(&tid_1, NULL, thread_block_all, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    err = pthread_create(&tid_2, NULL, thread_sigint, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    err = pthread_create(&tid_3, NULL, thread_sigquit, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_join(tid_1, NULL);
    if(err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }
    err = pthread_join(tid_2, NULL);
    if(err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }
    err = pthread_join(tid_3, NULL);
    if(err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    return 0;
}
