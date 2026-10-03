#ifndef LAB1_7_UTHREAD_H
#define LAB1_7_UTHREAD_H

typedef struct user_thread uthread_t;

typedef struct my_user_threads my_uthreads_t;

my_uthreads_t* uthread_init();

int uthread_create(my_uthreads_t* control, uthread_t **thread, void *(*start_routine)(void *), void *arg);

int uthread_sched_yield(my_uthreads_t* control);

int uthread_join(my_uthreads_t* control, uthread_t *thread, void **res);

int uthread_finalize(my_uthreads_t* control);

#endif //LAB1_7_UTHREAD_H
