#ifndef LAB2_4_MY_SYNC_H
#define LAB2_4_MY_SYNC_H

typedef struct my_spinlock my_spinlock_t;

typedef struct my_mutex my_mutex_t;

typedef enum {
    MY_SYNC_NORMAL,
    MY_SYNC_ERROR_CHECK,
    MY_SYNC_RECURSIVE
}type_sync_t;

typedef enum {
    MY_SYNC_PRIVATE,
    MY_SYNC_SHARED
}type_access_t;

typedef enum {
    MY_SUCCESS,
    MY_ERROR_DEADLOCK,
    MY_ERROR_OPERATION_NOT_PERMITTED
}my_error_t;

int my_spinlock_init(my_spinlock_t **spin, type_sync_t type, type_access_t type_access);

int my_spinlock_destroy(my_spinlock_t *spin);

my_error_t my_spinlock_lock(my_spinlock_t *spin);

my_error_t my_spinlock_unlock(my_spinlock_t *spin);

int my_mutex_init(my_mutex_t **mutex, type_sync_t type, type_access_t type_access);

int my_mutex_destroy(my_mutex_t *mutex);

my_error_t my_mutex_lock(my_mutex_t *mutex);

my_error_t my_mutex_unlock(my_mutex_t *mutex);

#endif //LAB2_4_MY_SYNC_H
