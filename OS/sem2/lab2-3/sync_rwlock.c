#include "sync.h"

#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>

void thread_sync_init(void **sync){
    *sync = malloc(sizeof(pthread_rwlock_t));
    if(*sync == NULL){
        perror("malloc: ");
        abort();
    }
    if(pthread_rwlock_init(*sync, NULL)){
        perror("pthread_rwlock_init: ");
        abort();
    }
}

void thread_sync_lock_read(void *sync){
    pthread_rwlock_rdlock(sync);
}

void thread_sync_lock_write(void *sync){
    pthread_rwlock_wrlock(sync);
}

void thread_sync_unlock(void *sync){
    pthread_rwlock_unlock(sync);
}

void thread_sync_destroy(void *sync){
    if(pthread_rwlock_destroy(sync)){
        perror("pthread_rwlock_destroy: ");
        abort();
    }
    free(sync);
}

