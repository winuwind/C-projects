#include "my_sync.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <limits.h>

typedef struct my_spinlock {
    int state;
    type_sync_t type;
    int owner;
}my_spinlock_t;

typedef struct my_mutex {
    int state;
    type_sync_t type;
    int locked;
    int count_waiters;
    int owner;
}my_mutex_t;

int my_spinlock_init(my_spinlock_t **spin, type_sync_t type, type_access_t type_access){
    if(type_access == MY_SYNC_PRIVATE){
        *spin = mmap(NULL, sizeof(my_spinlock_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    }
    else{
        *spin = mmap(NULL, sizeof(my_spinlock_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    }
    if(*spin == MAP_FAILED){
        perror("mmap: ");
        return 1;
    }
    atomic_init(&(*spin)->state, 0);
    (*spin)->type = type;
    return 0;
}

int my_spinlock_destroy(my_spinlock_t *spin){
    if(munmap(spin, sizeof(my_spinlock_t)) == -1){
        perror("munmap: ");
        return 1;
    }
    return 0;
}

my_error_t my_spinlock_lock(my_spinlock_t *spin){
    if(atomic_load(&spin->state) != 0 && syscall(SYS_gettid) == spin->owner){
        if(spin->type == MY_SYNC_ERROR_CHECK){
            return MY_ERROR_DEADLOCK;
        }
        else if(spin->type == MY_SYNC_RECURSIVE){
            atomic_fetch_add(&spin->state, 1);
            return MY_SUCCESS;
        }
    }
    int expected = 0;
    while(!atomic_compare_exchange_strong(&spin->state, &expected, 1)){
        expected = 0;
        usleep(10);
    }
    spin->owner = syscall(SYS_gettid);
    return MY_SUCCESS;
}

my_error_t my_spinlock_unlock(my_spinlock_t *spin){
    if(atomic_load(&spin->state) == 0 || syscall(SYS_gettid) != spin->owner){
        if(spin->type == MY_SYNC_ERROR_CHECK){
            return MY_ERROR_OPERATION_NOT_PERMITTED;
        }
        return MY_SUCCESS;
    }

    atomic_fetch_sub(&spin->state, 1);
    return MY_SUCCESS;
}


int my_mutex_init(my_mutex_t **mutex, type_sync_t type, type_access_t type_access){
    if(type_access == MY_SYNC_PRIVATE){
        *mutex = mmap(NULL, sizeof(my_mutex_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    }
    else{
        *mutex = mmap(NULL, sizeof(my_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    }
    if(*mutex == MAP_FAILED){
        perror("mmap: ");
        return 1;
    }
    atomic_init(&(*mutex)->state, 0);
    (*mutex)->type = type;
    (*mutex)->locked = 0;
    (*mutex)->count_waiters = 0;
    return 0;
}

int my_mutex_destroy(my_mutex_t *mutex){
    if(munmap(mutex, sizeof(my_mutex_t)) == -1){
        perror("munmap: ");
        return 1;
    }
    return 0;
}

my_error_t my_mutex_lock(my_mutex_t *mutex){
    if(atomic_load(&mutex->locked) != 0 && syscall(SYS_gettid) == mutex->owner){
        if(mutex->type == MY_SYNC_ERROR_CHECK){
            return MY_ERROR_DEADLOCK;
        }
        else if(mutex->type == MY_SYNC_RECURSIVE){
            mutex->state++;
            return MY_SUCCESS;
        }
    }
    atomic_fetch_add(&mutex->count_waiters, 1);
    while(1){
        int expected = 0;
        if(atomic_compare_exchange_strong(&mutex->locked, &expected, 1)){
            mutex->state = 1;
            mutex->owner = syscall(SYS_gettid);
            atomic_fetch_sub(&mutex->count_waiters, 1);
            return MY_SUCCESS;
        }
        syscall(SYS_futex, &mutex->locked, FUTEX_WAIT, 1, NULL, NULL, 0);
    }
}

my_error_t my_mutex_unlock(my_mutex_t *mutex){
    if(atomic_load(&mutex->locked) == 0 || syscall(SYS_gettid) != mutex->owner){
        if(mutex->type == MY_SYNC_ERROR_CHECK){
            return MY_ERROR_OPERATION_NOT_PERMITTED;
        }
        return MY_SUCCESS;
    }

    mutex->state--;
    if(mutex->state == 0){
        atomic_store(&mutex->locked, 0);
        if(atomic_load(&mutex->count_waiters) > 0){
            syscall(SYS_futex, &mutex->locked, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);
        }
    }
    return MY_SUCCESS;
}