#define _GNU_SOURCE

#include "sync.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct Node {
    char value[100];
    struct Node *next;
    void *sync;
} Node_t;

typedef struct Storage {
    Node_t *first;
} Storage_t;

typedef struct Arguments {
    Storage_t *storage;
    int *count_check;
    int *count_iter;

    int (*comp)(Node_t *node_1, Node_t *node_2);

    void (*fn)(Storage_t *storage, int *count_check, int *count_iter, int (*comp)(Node_t *node_1, Node_t *node_2));
} Args_t;

int count_check_first = 0;
int count_check_second = 0;
int count_check_third = 0;
int count_iter_first = 0;
int count_iter_second = 0;
int count_iter_third = 0;

volatile int count_swap = 0;
void *sync_count;

volatile char running = 1;

int compareFirst(Node_t *node_1, Node_t *node_2) {
    return strlen(node_1->value) < strlen(node_2->value);
}

int compareSecond(Node_t *node_1, Node_t *node_2) {
    return strlen(node_1->value) > strlen(node_2->value);
}

int compareThird(Node_t *node_1, Node_t *node_2) {
    return strlen(node_1->value) == strlen(node_2->value);
}

int needToSwap() {
    return (rand() + rand() + rand()) % 2;
}

void threadFunc(Storage_t *storage, int *count_check, int *count_iter, int (*comp)(Node_t *node_1, Node_t *node_2)) {
    while (running) {
        int count = 0;
        Node_t *node = storage->first;

        thread_sync_lock_read(node->sync);
        Node_t *next_node = node->next;

        while (next_node != NULL) {
            thread_sync_lock_read(next_node->sync);
            if (comp(node, next_node)) {
                count++;
            }
            thread_sync_unlock(node->sync);
            node = next_node;
            next_node = next_node->next;
        }
        thread_sync_unlock(node->sync);
        (*count_check) = count;
        (*count_iter)++;
    }
}

void swapAndUnlockPrev(Storage_t *storage, Node_t **prev_node, Node_t **node, Node_t **next_node){
    if (*node == storage->first) {
        storage->first = *next_node;
    }
    (*node)->next = (*next_node)->next;
    (*next_node)->next = *node;
    (*prev_node)->next = (*next_node);
    thread_sync_unlock((*prev_node)->sync);
    *prev_node = *next_node;
    atomic_fetch_add(&count_swap, 1);
}

void doFirstIter(Storage_t *storage, Node_t **prev_node, Node_t **node, Node_t **next_node){
    thread_sync_lock_write((*next_node)->sync);
    if (needToSwap()) {
        if (*node == storage->first) {
            storage->first = *next_node;
        }
        (*node)->next = (*next_node)->next;
        (*next_node)->next = *node;
        *prev_node = *next_node;
        atomic_fetch_add(&count_swap, 1);
    } else {
        *node = *next_node;
    }
    *next_node = (*node)->next;
}

void threadSwapDoListTraversal(Storage_t *storage, Node_t **prev_node, Node_t **node, Node_t **next_node){
    if(*next_node == NULL){
        return;
    }
    doFirstIter(storage, prev_node, node, next_node);
    while (*next_node != NULL) {
        thread_sync_lock_write((*next_node)->sync);
        if (needToSwap()) {
            swapAndUnlockPrev(storage, prev_node, node, next_node);
        } else {
            thread_sync_unlock((*prev_node)->sync);
            *prev_node = *node;
            *node = *next_node;
        }
        *next_node = (*node)->next;
    }
}

void *threadSwap(void *arg) {
    Storage_t *storage = (Storage_t *) arg;
    while (running) {
        Node_t *node = storage->first;
        thread_sync_lock_write(node->sync);
        Node_t *prev_node = node;
        Node_t *next_node = node->next;

        threadSwapDoListTraversal(storage, &prev_node, &node, &next_node);

        thread_sync_unlock(prev_node->sync);
        if (prev_node != node) {
            thread_sync_unlock(node->sync);
        }
    }
    return NULL;
}

void *startThread(void *args) {
    Args_t *args_c = (Args_t *) args;
    args_c->fn(args_c->storage, args_c->count_check, args_c->count_iter, args_c->comp);
    return NULL;
}


Node_t *createNode(int length) {
    Node_t *node = (Node_t *) malloc(sizeof(Node_t));
    if (node == NULL) {
        printf("create_node: malloc can't allocate memory\n");
        abort();
    }
    thread_sync_init(&node->sync);
    node->next = NULL;
    for (int i = 0; i < length; i++) {
        node->value[i] = 'a';
    }
    node->value[length] = 0;
    return node;
}

void destroyNode(Node_t *node) {
    thread_sync_destroy(node->sync);
    free(node);
}

Storage_t *createList(int length) {
    Storage_t *storage = (Storage_t *) malloc(sizeof(Storage_t));
    if (storage == NULL) {
        printf("create_node: malloc can't allocate memory\n");
        abort();
    }
    storage->first = createNode(rand() % 99);
    Node_t *node = storage->first;
    for (int i = 0; i < length - 1; i++) {
        node->next = createNode(rand() % 99);
        node = node->next;
    }
    return storage;
}

void destroyList(Storage_t *storage) {
    Node_t *node = storage->first;
    Node_t *node_next = node->next;
    while (node_next != NULL) {
        destroyNode(node);
        node = node_next;
        node_next = node->next;
    }
    destroyNode(node);
    free(storage);
}

void *threadController(void *_) {
    while (running) {
        sleep(1);
        printf("CHECKED: first: %d, sec: %d, third: %d\nITER: first: %d, sec: %d, third: %d\nSWAPPED: %d\n",
               count_check_first, count_check_second, count_check_third, count_iter_first, count_iter_second,
               count_iter_third, count_swap);
    }
    return NULL;
}

int main(void) {
    char command[100];
    int N = 1000;
    printf("Enter N (if not a number then N = 1000): ");
    if (fgets(command, 100, stdin)) {
        int len = strlen(command) - 1;
        command[len] = 0;
        if (len != 0) {
            char flag = 1;
            for (int i = 0; i < len; i++) {
                if (command[i] < '0' || command[i] > '9') {
                    flag = 0;
                    break;
                }
            }
            if (flag) {
                N = strtol(command, NULL, 10);
                printf("N = %d\n", N);
            }
        }
    } else {
        printf("main: fgets() failed\n");
        abort();
    }

    int err;
    Storage_t *storage = createList(N);

    thread_sync_init(&sync_count);

    pthread_t thread_first;
    pthread_t thread_second;
    pthread_t thread_third;
    pthread_t thread_swapper_first;
    pthread_t thread_swapper_second;
    pthread_t thread_swapper_third;
    pthread_t thread_printer;

    Args_t *args_first = (Args_t *) malloc(sizeof(Args_t));
    args_first->count_check = &count_check_first;
    args_first->count_iter = &count_iter_first;
    args_first->storage = storage;
    args_first->comp = compareFirst;
    args_first->fn = threadFunc;

    Args_t *args_second = (Args_t *) malloc(sizeof(Args_t));
    args_second->count_check = &count_check_second;
    args_second->count_iter = &count_iter_second;
    args_second->storage = storage;
    args_second->comp = compareSecond;
    args_second->fn = threadFunc;

    Args_t *args_third = (Args_t *) malloc(sizeof(Args_t));
    args_third->count_check = &count_check_third;
    args_third->count_iter = &count_iter_third;
    args_third->storage = storage;
    args_third->comp = compareThird;
    args_third->fn = threadFunc;

    err = pthread_create(&thread_first, NULL, startThread, args_first);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_create(&thread_second, NULL, startThread, args_second);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_create(&thread_third, NULL, startThread, args_third);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_create(&thread_swapper_first, NULL, threadSwap, storage);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_create(&thread_swapper_second, NULL, threadSwap, storage);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_create(&thread_swapper_third, NULL, threadSwap, storage);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_create(&thread_printer, NULL, threadController, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        abort();
    }

    while (running) {
        char *buf = fgets(command, 100, stdin);
        if (strcmp(command, "exit\n") == 0 || strlen(command) == 0 || buf == NULL) {
            running = 0;
        }
    }

    err = pthread_join(thread_first, NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_join(thread_second, NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_join(thread_third, NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_join(thread_swapper_first, NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_join(thread_swapper_second, NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_join(thread_swapper_third, NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        abort();
    }
    err = pthread_join(thread_printer, NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        abort();
    }

    thread_sync_destroy(sync_count);

    free(args_first);
    free(args_second);
    free(args_third);
    destroyList(storage);
    return 0;
}