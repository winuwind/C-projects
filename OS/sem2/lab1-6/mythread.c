#define _GNU_SOURCE

#include "mythread.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sched.h>
#include <sys/syscall.h>

struct mythread {
    int tid;
    void *result;
    void *stack;
    char state;
};

typedef struct alive_threads {
    struct alive_threads *next;
    mythread_t *thread;
} alive_threads_t;

typedef struct mythread_attr {
    mythread_t *thread;

    void *(*fn)(void *);

    void *arg;
} mythread_attr_t;

const int stack_size = 4 * 1024 * 1024;
volatile alive_threads_t *a_threads;

int start_thread(void *mythread_args) {
    mythread_attr_t *args = (mythread_attr_t *) mythread_args;
    args->thread->result = args->fn(args->arg);
    volatile alive_threads_t *pointer = a_threads->next;
    volatile alive_threads_t *pointer_prev = a_threads;
    while (pointer != NULL && pointer->thread->tid != args->thread->tid) {
        pointer_prev = pointer;
        pointer = pointer->next;
    }
    if (pointer != NULL) {
        pointer_prev->next = pointer->next;
    }
    args->thread->state = 1;
    free((void *) pointer);
    free(args);
    return 0;
}

int mythread_init() {
    a_threads = malloc(sizeof(alive_threads_t));
    if (a_threads == NULL) {
        return -1;
    }
    a_threads->next = NULL;;
    a_threads->thread = NULL;
    return 0;
}

int mythread_create(mythread_t **thread, void *(*start_routine)(void *), void *arg) {
    void *stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (stack == MAP_FAILED) {
        perror("mmap: ");
        return 1;
    }

    mythread_t *new_thread = malloc(sizeof(mythread_t));
    if(new_thread == NULL){
        perror("malloc: ");
        if (munmap(stack, stack_size) == -1) {
            perror("munmap: ");
            return 1;
        }
        return 1;
    }

    new_thread->tid = syscall(SYS_gettid);
    new_thread->result = NULL;
    new_thread->stack = stack;
    new_thread->state = 0;

    *thread = new_thread;

    volatile alive_threads_t *pointer = a_threads;

    while (pointer->next) {
        pointer = pointer->next;
    }

    pointer->next = malloc(sizeof(alive_threads_t));
    if(pointer->next == NULL){
        perror("malloc: ");
        free(new_thread);
        if (munmap(stack, stack_size) == -1) {
            perror("munmap: ");
            return 1;
        }
        return 1;
    }

    pointer = pointer->next;
    pointer->next = NULL;
    pointer->thread = new_thread;

    mythread_attr_t *args = malloc(sizeof(mythread_attr_t));
    if(args == NULL){
        perror("malloc: ");
        free(new_thread);
        free((void *) pointer);
        if (munmap(stack, stack_size) == -1) {
            perror("munmap: ");
            return 1;
        }
        return 1;
    }

    args->arg = arg;
    args->fn = start_routine;
    args->thread = new_thread;

    if (clone(start_thread, stack + stack_size, CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD,
              args) == -1) {
        free(new_thread);
        free((void *) pointer);
        free(args);
        if (munmap(stack, stack_size) == -1) {
            perror("munmap: ");
            return 1;
        }
        perror("clone: ");
        return 1;
    }
    return 0;
}

int mythread_join(mythread_t *thread, void **res) {
    while (thread->state == 0) {
        usleep(20);
    }
    usleep(20);
    if (munmap(thread->stack, stack_size) == -1) {
        perror("munmap: ");
        return 1;
    }
    if (res != NULL) {
        *res = thread->result;
    }
    free(thread);
    return 0;
}

int mythread_finalize() {
    volatile alive_threads_t *pointer = a_threads;
    volatile alive_threads_t *pointer_prev = a_threads;
    while (pointer->next != NULL) {
        pointer_prev = pointer;
        if (pointer->thread != NULL) {
            if (mythread_join(pointer->thread, NULL)) {
                perror("mythread_join: ");
                return 1;
            }
        }
        pointer = pointer->next;
        free((void *) pointer_prev);
    }
    free((void *) pointer_prev);
    return 0;
}