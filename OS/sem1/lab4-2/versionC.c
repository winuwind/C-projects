#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <sys/mman.h>

void sigsegv_handler(int signum) {
    printf("Получен сигнал SIGSEGV (ошибка сегментации)\n");
    exit(1);
}

void recursive_stack_function(int depth) {
    char buffer[4096];
//    sleep(1);
//    usleep(100);
    if (depth < 1000) {
        recursive_stack_function(depth);
    }
}

void versionC(){
    signal(SIGSEGV, sigsegv_handler);
    printf("pid = %d\n", getpid());
//    sleep(10);
//    recursive_stack_function(0);

//    char** arr_pointers = (char**) malloc(sizeof(char*) * 1000000);
//    for(int i = 0; i < 1000000; i++){
//        arr_pointers[i] = (char*) malloc(256);
//        usleep(1000);
//    }
//    sleep(3);
//    for(int i = 0; i < 1000000; i++){
//        free(arr_pointers[i]);
//    }
//    free(arr_pointers);

    void *addr = mmap(NULL, 10 * sysconf(_SC_PAGESIZE), PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    printf("Адрес нового региона: %p\n", addr);
    sprintf((char*) addr, "Hello, World!\n");

//    sleep(10);
    printf("Смена прав\n");
    mprotect(addr, 10 * sysconf(_SC_PAGESIZE), PROT_WRITE);
    char x = *(char*) addr;
    printf("1\n");
    printf("%s", (char*) addr);
//    *((char*)addr) = 0;

    sleep(10);
    printf("Отсоединили часть\n");
    munmap(addr + 3 * sysconf(_SC_PAGESIZE), 3 * sysconf(_SC_PAGESIZE));

    sleep(10);
    munmap(addr, 3 * sysconf(_SC_PAGESIZE));
    munmap(addr + 6 * sysconf(_SC_PAGESIZE), 3 * sysconf(_SC_PAGESIZE));
    sleep(10);
}

int main(){
    versionC();
}