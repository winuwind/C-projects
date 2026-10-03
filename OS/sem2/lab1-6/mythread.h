#ifndef LAB1_6_MYTHREAD_H
#define LAB1_6_MYTHREAD_H

#include <pthread.h>

typedef struct mythread mythread_t;

int mythread_init();

int mythread_create(mythread_t **thread, void *(*start_routine)(void *), void *arg);

int mythread_join(mythread_t *thread, void** res);

int mythread_finalize();

#endif //LAB1_6_MYTHREAD_H
