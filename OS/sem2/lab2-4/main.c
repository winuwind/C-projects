#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "my_sync.h"

//void *foo_1(void *arg){
//    my_spinlock_t *spin = arg;
//    while(1){
//        break;
//        my_spinlock_lock(spin);
//        printf("foo_1: thread %d owns spin\n", syscall(SYS_gettid));
//        sleep(2);
//        my_spinlock_unlock(spin);
//    }
//}

//void *foo_2(void *arg){
//    my_mutex_t *mutex = arg;
//    while(1){
//        my_mutex_lock(mutex);
//        printf("foo_1: thread %d owns mutex\n", syscall(SYS_gettid));
//        sleep(2);
//        my_mutex_unlock(mutex);
//        usleep(50);
//    }
//    return NULL;
//}

long x = 0;

void *foo_2(void *arg){
    my_spinlock_t *spin = arg;
    for(int i = 0; i < 10000000; i++){
        my_spinlock_lock(spin);
        x++;
        my_spinlock_unlock(spin);
    }
    return NULL;
}

int main(void) {
    pthread_t t3, t4;
//    my_mutex_t *mutex;
//    if(my_mutex_init(&mutex, MY_SYNC_NORMAL, MY_SYNC_PRIVATE)){
//        printf("error init mutex\n");
//        return 0;
//    }

    my_spinlock_t *spin;
    if(my_spinlock_init(&spin, MY_SYNC_NORMAL, MY_SYNC_PRIVATE)){
        printf("error init spin\n");
        return 0;
    }

    if(pthread_create(&t3, NULL, foo_2, spin)){
        printf("error create thread\n");
//        if(my_mutex_destroy(spin)){
//            printf("error destroy mutex\n");
//        }
        return 0;
    }

    if(pthread_create(&t4, NULL, foo_2, spin)){
        printf("error create thread\n");

//        if(my_mutex_destroy(mutex)){
//            printf("error destroy mutex\n");
//        }
        return 0;
    }

    if(pthread_join(t3, NULL)){
        printf("error join thread\n");

//        if(my_mutex_destroy(mutex)){
//            printf("error destroy mutex\n");
//        }
        return 0;
    }

    if(pthread_join(t4, NULL)){
        printf("error join thread\n");

//        if(my_mutex_destroy(mutex)){
//            printf("error destroy mutex\n");
//        }
        return 0;
    }

    printf("%ld\n", x);
//    if(my_mutex_destroy(mutex)){
//        printf("error destroy mutex\n");
//        return 0;
//    }

    if(my_spinlock_destroy(spin)){
        printf("error destroy spin\n");
        return 0;
    }
    return 0;
}
