#include <stdio.h>
#include <pthread.h>

int main(void) {
    printf("Hello, World!\n");
    return 0;
}




typedef struct my_semaphore{
    int count_res;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}my_semaphore_t;

void my_sem_wait(my_semaphore_t *sem){
    pthread_mutex_lock(&sem->mutex);
    while(sem->count_res == 0){
        pthread_cond_wait(&sem->cond, &sem->mutex);
    }
    sem->count_res--;
    pthread_mutex_unlock(&sem->mutex);
}

void my_sem_post(my_semaphore_t *sem){
    pthread_mutex_lock(&sem->mutex);
    sem->count_res++;
    pthread_cond_broadcast(&sem->cond);
    pthread_mutex_unlock(&sem->mutex);
}