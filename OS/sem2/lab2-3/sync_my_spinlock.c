#include "sync.h"
#include "../lab2-4/my_sync.h"

#include <stdlib.h>
#include <stdio.h>

void thread_sync_init(void **sync){
    if(my_spinlock_init((my_spinlock_t **) sync, MY_SYNC_NORMAL, MY_SYNC_PRIVATE)){
        perror("my_spinlock_init: ");
        abort();
    }
}

void thread_sync_lock_read(void *sync){
    my_spinlock_lock(sync);
}

void thread_sync_lock_write(void *sync){
    my_spinlock_lock(sync);
}

void thread_sync_unlock(void *sync){
    my_spinlock_unlock(sync);
}

void thread_sync_destroy(void *sync){
    if(my_spinlock_destroy((my_spinlock_t *) sync)){
        perror("my_spinlock_destroy: ");
        abort();
    }
    free(sync);
}

