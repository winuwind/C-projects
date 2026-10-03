#ifndef LAB2_3_SYNC_H
#define LAB2_3_SYNC_H

void thread_sync_init(void **sync);

void thread_sync_lock_read(void *sync);

void thread_sync_lock_write(void *sync);

void thread_sync_unlock(void *sync);

void thread_sync_destroy(void *sync);


#endif //LAB2_3_SYNC_H
