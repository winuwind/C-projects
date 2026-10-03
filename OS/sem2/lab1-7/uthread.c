#include "uthread.h"

#include <ucontext.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct user_thread {
    int id;
    void *stack;
    ucontext_t *context;
    volatile char planned;
    volatile char finished;

    void *arg;

    void *(*start_routine)(void *);

    void *res;
} uthread_t;

typedef struct alive_threads {
    struct alive_threads *next;
    uthread_t *thread;
} alive_threads_t;

typedef struct my_user_threads{
    int stack_size;
    volatile alive_threads_t *a_threads;
    volatile int uthread_last_id;

    ucontext_t context_main;
    volatile char planned;
} my_uthreads_t;

void start_thread(void *arg) {
    ucontext_t current_uc;
    if (getcontext(&current_uc) == -1) {
        perror("getcontext: ");
        return;
    }

    my_uthreads_t *control = (my_uthreads_t *) arg;

    volatile char x;
    volatile alive_threads_t *pointer = control->a_threads;
    uthread_t *thread = NULL;
    while (pointer != NULL) {
        if (pointer->thread != NULL && pointer->thread->stack <= (void *) &x &&
            (pointer->thread->stack + control->stack_size) >= (void *) &x) {
            thread = pointer->thread;
        }
        pointer = pointer->next;
    }
    if(thread == NULL){
        return;
    }

    thread->res = thread->start_routine(thread->arg);

    pointer = control->a_threads->next;
    volatile alive_threads_t *pointer_prev = control->a_threads;
    while (pointer != NULL && pointer->thread->id != thread->id) {
        pointer_prev = pointer;
        pointer = pointer->next;
    }
    if (pointer != NULL) {
        pointer_prev->next = pointer->next;
    }
    free((void *) pointer);

    thread->finished = 1;
    thread->planned = 0;
}

int uthread_scheduller(my_uthreads_t* control, uthread_t *thread) {
    if (thread == NULL) {
        control->planned = 1;
        while (1) {
            volatile alive_threads_t *pointer = control->a_threads;
            int size_a_threads = 0;
            while (pointer != NULL) {
                size_a_threads++;
                if (pointer->thread != NULL) {
                    uthread_t *_thread = pointer->thread;
                    ucontext_t context;
                    if (swapcontext(&context, _thread->context) == -1) {
                        perror("swapcontext: ");
                        return 1;
                    }
                }
                pointer = pointer->next;
            }
            if(size_a_threads == 1){
                return 2;
            }
        }
    } else {
        thread->planned = 1;
        char flag = 0; //флаг для правильной очередности
        while (1) {
            volatile alive_threads_t *pointer = control->a_threads;
            while (pointer != NULL) {
                if (pointer->thread != NULL) {
                    if (pointer->thread->id == thread->id) {
                        flag = 1;
                        pointer = pointer->next;
                        continue;
                    }
                    if (!flag) {
                        pointer = pointer->next;
                        continue;
                    }
                    uthread_t *_thread = pointer->thread;
                    ucontext_t context;
                    if (swapcontext(&context, _thread->context) == -1) {
                        perror("swapcontext: ");
                        return 1;
                    }
                }
                pointer = pointer->next;
            }
            //Если я тут, то потоки до этого не готовы к возобновлению/последний поток вызвал yield
            if (!control->planned) {
                control->planned = 1;
            } else {
                ucontext_t context;
                if (swapcontext(&context, &control->context_main)) {
                    perror("swapcontext: ");
                    return 1;
                }
            }
        }
    }
    return 0;
}

my_uthreads_t* uthread_init() {
    volatile alive_threads_t *a_threads = malloc(sizeof(alive_threads_t));
    if (a_threads == NULL) {
        return NULL;
    }
    a_threads->next = NULL;
    a_threads->thread = NULL;

    my_uthreads_t *threads_control = malloc(sizeof(my_uthreads_t));
    threads_control->planned = 1;
    threads_control->a_threads = a_threads;
    threads_control->stack_size = 4194304;
    threads_control->uthread_last_id = 0;
    return threads_control;
}

int uthread_finalize(my_uthreads_t* control) {
    volatile alive_threads_t *pointer = control->a_threads;
    while (pointer->next != NULL) {
        if (pointer->thread != NULL) {
            if (uthread_join(control, pointer->thread, NULL)) {
                perror("mythread_join: ");
                return 1;
            }
        }
        pointer = pointer->next;
    }
    free((void *) control->a_threads);

    free(control);
    return 0;
}

int uthread_create(my_uthreads_t* control, uthread_t **thread, void *(*start_routine)(void *), void *arg) {
    void *stack = mmap(NULL, control->stack_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (stack == MAP_FAILED) {
        perror("mmap: ");
        return 1;
    }

    uthread_t *new_thread = malloc(sizeof(uthread_t));
    if (new_thread == NULL) {
        perror("malloc: ");
        if (munmap(stack, control->stack_size) == -1) {
            perror("munmap: ");
            return 1;
        }
        return 1;
    }

    new_thread->id = control->uthread_last_id++;
    new_thread->stack = stack;
    new_thread->finished = 0;
    new_thread->planned = 1;
    new_thread->arg = arg;
    new_thread->start_routine = start_routine;
    new_thread->res = NULL;

    new_thread->context = malloc(sizeof(ucontext_t));
    if (new_thread->context == NULL) {
        free(new_thread);
        perror("malloc: ");
        if (munmap(stack, control->stack_size) == -1) {
            perror("munmap: ");
            return 1;
        }
        return 1;
    }

    if (getcontext(new_thread->context) == -1) {
        free(new_thread->context);
        free(new_thread);
        perror("getcontext: ");
        if (munmap(stack, control->stack_size) == -1) {
            perror("munmap: ");
            return 1;
        }
        return 1;
    }
    new_thread->context->uc_stack.ss_sp = stack;
    new_thread->context->uc_stack.ss_size = control->stack_size;
    new_thread->context->uc_link = &control->context_main;

    *thread = new_thread;
    volatile alive_threads_t *pointer = control->a_threads;

    while (pointer->next != NULL) {
        pointer = pointer->next;
    }

    pointer->next = malloc(sizeof(alive_threads_t));
    if(pointer->next == NULL){
        perror("malloc: ");
        free(new_thread);
        if (munmap(stack, control->stack_size) == -1) {
            perror("munmap: ");
            return 1;
        }
        return 1;
    }

    pointer = pointer->next;
    pointer->next = NULL;
    pointer->thread = new_thread;

    makecontext(new_thread->context, (void (*)()) start_thread, 1, control);

    if (swapcontext(&control->context_main, new_thread->context) == -1) {
        free(new_thread->context);
        free(new_thread);
        perror("swapcontext: ");
        if (munmap(stack, control->stack_size) == -1) {
            perror("munmap: ");
            return 1;
        }
        return 1;
    }
    return 0;
}

int uthread_sched_yield(my_uthreads_t* control) {
    volatile char x;
    volatile alive_threads_t *pointer = control->a_threads;
    uthread_t *thread = NULL;
    while (pointer != NULL) {
        if (pointer->thread != NULL && pointer->thread->stack <= (void *) &x &&
            (pointer->thread->stack + control->stack_size) >= (void *) &x) {
            thread = pointer->thread;
        }
        pointer = pointer->next;
    }
    if (thread == NULL) {
        //main thread
        control->planned = 0;
        if (getcontext(&control->context_main) == -1) {
            perror("getcontext: ");
            return 1;
        }
        if (!control->planned) {
            int res = uthread_scheduller(control, NULL);
            return res;
        }
    } else {
        //uthread
        thread->planned = 0;
        if (getcontext(thread->context) == -1) {
            perror("getcontext: ");
            return 1;
        }
        if (!thread->planned) {
            if (uthread_scheduller(control, thread)) {
                return 1;
            }
        }
    }
    return 0;
}

int uthread_join(my_uthreads_t* control, uthread_t *thread, void **res) {
    while (!thread->finished) {
        uthread_sched_yield(control);
    }
    if (munmap(thread->stack, control->stack_size) == -1) {
        perror("munmap: ");
        return 1;
    }
    if (res != NULL) {
        *res = thread->res;
    }
    free(thread->context);
    free(thread);
    return 0;
}
