#define _GNU_SOURCE
#include <assert.h>

#include "queue.h"

void *qmonitor(void *arg) {
    queue_t *q = (queue_t *)arg;

    printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

    while (1) {
        queue_print_stats(q);
        sleep(1);
    }

    return NULL;
}

queue_t* queue_init(int max_count) {
    int err;

    queue_t *q = malloc(sizeof(queue_t));
    if (!q) {
        printf("Cannot allocate memory for a queue\n");
        abort();
    }

    q->first = NULL;
    q->last = NULL;
    q->max_count = max_count;
    q->count = 0;

    q->add_attempts = q->get_attempts = 0;
    q->add_count = q->get_count = 0;

    pthread_mutexattr_t attr;
    err = pthread_mutexattr_init(&attr);
    if (err) {
        printf("queue_init: pthread_mutexattr_init() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_DEFAULT);
    if (err) {
        printf("queue_init: pthread_mutexattr_settype() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
    if (err) {
        printf("queue_init: pthread_mutexattr_setrobust() failed: %s\n", strerror(err));
        abort();
    }

    err = pthread_mutex_init(&q->mutex, &attr);
    if (err) {
        printf("queue_init: pthread_mutex_init() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_mutexattr_destroy(&attr);
    if (err) {
        printf("queue_init: pthread_mutexattr_destroy() failed: %s\n", strerror(err));
        abort();
    }

    err = pthread_cond_init(&q->cond_empty, NULL);
    if (err) {
        printf("queue_init: pthread_cond_init() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_cond_init(&q->cond_full, NULL);
    if (err) {
        printf("queue_init: pthread_cond_init() failed: %s\n", strerror(err));
        abort();
    }

    err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
    if (err) {
        printf("queue_init: pthread_create() failed: %s\n", strerror(err));
        abort();
    }

    return q;
}

void queue_destroy(queue_t *q) {
    qnode_t *tmp;
    int err;

    assert(q->count >= 0);

    while(q->count--){
        tmp = q->first;
        q->first = q->first->next;

        assert(tmp != NULL);

        free(tmp);
    }

    assert(q->first == NULL);

    err = pthread_cancel(q->qmonitor_tid);
    if(err){
        printf("queue_destroy: pthread_cancel() failed: %s\n", strerror(err));
        abort();
    }

    err = pthread_join(q->qmonitor_tid, NULL);
    if(err){
        printf("queue_destroy: pthread_join() failed: %s\n", strerror(err));
        abort();
    }

    err = pthread_mutex_destroy(&q->mutex);
    if(err){
        printf("queue_destroy: pthread_mutex_destroy() failed: %s\n", strerror(err));
        abort();
    }

    err = pthread_cond_destroy(&q->cond_full);
    if(err){
        printf("queue_destroy: pthread_cond_destroy() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_cond_destroy(&q->cond_empty);
    if(err){
        printf("queue_destroy: pthread_cond_destroy() failed: %s\n", strerror(err));
        abort();
    }

    free(q);
}

int queue_add(queue_t *q, int val) {
    int err;
    err = pthread_mutex_lock(&q->mutex);
    if(err){
        printf("queue_add: pthread_mutex_lock() failed: %s\n", strerror(err));
        abort();
    }
    q->add_attempts++;

    assert(q->count <= q->max_count);

    while(q->count == q->max_count){
        err = pthread_cond_wait(&q->cond_full, &q->mutex);
        if(err){
            printf("queue_add: pthread_cond_wait() failed: %s\n", strerror(err));
            abort();
        }
    }


    qnode_t *new = malloc(sizeof(qnode_t));
    if (!new) {
        printf("Cannot allocate memory for new node\n");
        abort();
    }

    new->val = val;
    new->next = NULL;

    if (!q->first)
        q->first = q->last = new;
    else {
        q->last->next = new;
        q->last = q->last->next;
    }

    q->count++;
    q->add_count++;

    err = pthread_cond_signal(&q->cond_empty);
    if(err){
        printf("queue_add: pthread_cond_signal() failed: %s\n", strerror(err));
        abort();
    }

    err = pthread_mutex_unlock(&q->mutex);
    if(err){
        printf("queue_add: pthread_mutex_unlock() failed: %s\n", strerror(err));
        abort();
    }

    return 1;
}

int queue_get(queue_t *q, int *val) {
    int err;
    err = pthread_mutex_lock(&q->mutex);
    if(err){
        printf("queue_get: pthread_mutex_lock() failed: %s\n", strerror(err));
        abort();
    }

    q->get_attempts++;

    assert(q->count >= 0);

    while (q->count == 0){
        err = pthread_cond_wait(&q->cond_empty, &q->mutex);
        if(err){
            printf("queue_get: pthread_cond_wait() failed: %s\n", strerror(err));
            abort();
        }
    }

    qnode_t *tmp = q->first;

    *val = tmp->val;
    q->first = q->first->next;

    free(tmp);
    q->count--;
    q->get_count++;

    err = pthread_cond_signal(&q->cond_full);
    if(err){
        printf("queue_get: pthread_cond_signal() failed: %s\n", strerror(err));
        abort();
    }

    err = pthread_mutex_unlock(&q->mutex);
    if(err){
        printf("queue_get: pthread_mutex_unlock() failed: %s\n", strerror(err));
        abort();
    }

    return 1;
}

void queue_print_stats(queue_t *q) {
    int err;
    err = pthread_mutex_lock(&q->mutex);
    if(err){
        printf("queue_print_stats: pthread_mutex_lock() failed: %s\n", strerror(err));
        abort();
    }
    printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
           q->count,
           q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
           q->add_count, q->get_count, q->add_count -q->get_count);
    err = pthread_mutex_unlock(&q->mutex);
    if(err){
        printf("queue_print_stats: pthread_mutex_unlock() failed: %s\n", strerror(err));
        abort();
    }
}

