#include "sync.h"
#include "../lab2-4/my_sync.h"

#include <stdlib.h>
#include <stdio.h>

void thread_sync_init(void **sync){
    if(my_mutex_init((my_mutex_t **) sync, MY_SYNC_NORMAL, MY_SYNC_PRIVATE)){
        perror("my_mutex_init: ");
        abort();
    }
}

void thread_sync_lock_read(void *sync){
    my_mutex_lock(sync);
}

void thread_sync_lock_write(void *sync){
    my_mutex_lock(sync);
}

void thread_sync_unlock(void *sync){
    my_mutex_unlock(sync);
}

void thread_sync_destroy(void *sync){
    if(my_mutex_destroy((my_mutex_t *) sync)){
        perror("my_mutex_destroy: ");
        abort();
    }
    free(sync);
}

