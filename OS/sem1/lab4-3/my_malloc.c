#include "my_malloc.h"

#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void *addr = (void*) 0;
unsigned count_pages = 1;
unsigned heap_size = 0;
char initial_flag = 0;
unsigned page_size;


int init() {
    page_size = sysconf(_SC_PAGESIZE);
    addr = mmap(NULL, page_size * count_pages, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("Error when mmap");
        return -1;
    }
    heap_size = page_size * count_pages;
    initial_flag = 1;
    memset(addr, 0, heap_size);
    *((void **) (addr + 12)) = addr + heap_size;
    return 0;
}

int finalize() {
    if (!initial_flag || munmap(addr, heap_size) == -1) {
        return -1;
    }
    return 0;
}

void *find_first_free(unsigned size, void **pointer_to_prev_chunk) {
    void *pointer = addr;
    unsigned chunk_size = 0;
    unsigned free_size;
    char free_flag;
    void *next_chunk = addr;
    do {
        *pointer_to_prev_chunk = pointer;
        pointer = next_chunk;
        chunk_size = *((unsigned int *) pointer);
        if (chunk_size == 0) {
            free_flag = 1;
        } else {
            free_flag = 0;
        }
        if(pointer + 20 < addr + heap_size) {
            next_chunk = *((void **) (pointer + 12));
        }
        //Если адрес следующего чанка не указан, то указатель на конец памяти
        if (next_chunk == NULL) {
            next_chunk = addr + heap_size;
        }
        //Рассчитываем свободное место в данном чанке, в первую ветку теоретически не попадаем, только из-за ошибки
        if (next_chunk < pointer + size) {
            free_size = 0;
        } else {
            free_size = next_chunk - pointer - chunk_size;
        }
    } while (free_size < size && next_chunk < addr + heap_size);
    if (pointer >= addr + heap_size || !free_size) {
        return NULL;
    }
    if (!free_flag) {
        *pointer_to_prev_chunk = pointer;
        pointer += chunk_size;
    }
    return pointer;
}

void print_info(){
    void *new_pointer = addr;
    printf("\nprint_info\n");
    int count = 0;
    while (new_pointer < addr + heap_size && count++ < 20) {
        printf("pointer = %p, pointer_prev = %p, pointer_next = %p\n", new_pointer, *((void **) (new_pointer + 4)), *((void **) (new_pointer + 12)));
        new_pointer = *((void **) (new_pointer + 12));
    }
}

void *my_malloc(unsigned size) {
    if (!initial_flag || size == 0) {
        return NULL;
    }
    void *pointer_to_prev_chunk;
    void *pointer = find_first_free(2 * size + 20, &pointer_to_prev_chunk);

    if (pointer == NULL) {
        return NULL;
    }
    *((unsigned *) pointer) = 2 * size + 20;
    //Записываем адрес предыдущего чанка в новый
    *((void **) (pointer + 4)) = pointer_to_prev_chunk;
    //Записываем адрес следующего чанка в новый
    if(pointer != pointer_to_prev_chunk) {
        *((void **) (pointer + 12)) = *((void **) (pointer_to_prev_chunk + 12));
    //Записываем в предыдущий чанк адрес на новый
        *((void **) (pointer_to_prev_chunk + 12)) = pointer;
    }
    //Записываем в следующий чанк адрес на новый
    if (*((void **) (pointer + 12)) < addr + heap_size) {
        *((void **) (*((void **) (pointer + 12)) + 4)) = pointer;
    }
    return pointer + 20;
}

int check_pointer(void *pointer) {
    void *new_pointer = addr;
    while (new_pointer < addr + heap_size) {
        if (new_pointer == pointer) {
            return 0;
        }
        new_pointer = *((void **) (new_pointer + 12));
    }
    return -1;
}

int my_free(void *pointer) {
    if (pointer == NULL) {
        return 0;
    }
    if (!initial_flag) {
        return -1;
    }
    pointer -= 20;
    if (check_pointer(pointer) != 0) {
        perror("Incorrect pointer for my_free()");
        return -1;
    }

    void* pointer_prev = *((void **) (pointer + 4));
    void* pointer_next = *((void **) (pointer + 12));

    if(pointer_prev == pointer){
        *((int*) pointer) = 0;
        memset(pointer + 20, 0, pointer_next - pointer - 20);
        return 0;
    }

    //Меняем адрес на следующий чанк в предыдущем чанке
    *((void **) (pointer_prev + 12)) = pointer_next;
//    Меняем адрес на предыдущий чанк в следующем чанке

    if (pointer_next < addr + heap_size) {
        *((void **) (pointer_next + 4)) = pointer_prev;
    }
    unsigned size = *((unsigned int *) pointer);
    memset(pointer, 0, size);
    return 0;
}