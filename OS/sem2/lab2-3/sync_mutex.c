#include "sync.h"

#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>

void thread_sync_init(void **sync){
    *sync = malloc(sizeof(pthread_mutex_t));
    if(*sync == NULL){
        perror("malloc: ");
        abort();
    }
    if(pthread_mutex_init(*sync, NULL)){
        perror("pthread_mutex_init: ");
        abort();
    }
}

void thread_sync_lock_read(void *sync){
    pthread_mutex_lock(sync);
}

void thread_sync_lock_write(void *sync){
    pthread_mutex_lock(sync);
}

void thread_sync_unlock(void *sync){
    pthread_mutex_unlock(sync);
}

void thread_sync_destroy(void *sync){
    if(pthread_spin_destroy(sync)){
        perror("pthread_mutex_destroy: ");
        abort();
    }
    free(sync);
}

