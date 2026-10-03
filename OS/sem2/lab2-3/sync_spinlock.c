#include "sync.h"

#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>

void thread_sync_init(void **sync){
    *sync = malloc(sizeof(pthread_spinlock_t));
    if(*sync == NULL){
        perror("malloc: ");
        abort();
    }
    if(pthread_spin_init(*sync, PTHREAD_PROCESS_PRIVATE)){
        perror("pthread_spin_init: ");
        abort();
    }
}

void thread_sync_lock_read(void *sync){
    pthread_spin_lock(sync);
}

void thread_sync_lock_write(void *sync){
    pthread_spin_lock(sync);
}

void thread_sync_unlock(void *sync){
    pthread_spin_unlock(sync);
}

void thread_sync_destroy(void *sync){
    if(pthread_spin_destroy(sync)){
        perror("pthread_spin_destroy: ");
        abort();
    }
    free(sync);
}

