#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <sys/mman.h>

void versionA(int argc, char** argv){
    printf("pid = %d\n", getpid());
    sleep(1);

    if(argc == 1){
        execlp(argv[0], argv[0], NULL);
    }
    else{
        execlp(argv[0], argv[0], argv[1], NULL);
    }

    printf("Hello world!\n");
    perror("Error execlp");
    exit(1);
}

void sigsegv_handler(int signum) {
    printf("Получен сигнал SIGSEGV (ошибка сегментации)\n");
    exit(1);
}

void recursive_stack_function(int depth) {
    char buffer[4096];
    if (depth < 1000) {
        recursive_stack_function(depth + 1);
    }
}

void versionC(int argc, char** argv){
    signal(SIGSEGV, sigsegv_handler);
    printf("pid = %d\n", getpid());
    sleep(10);
    recursive_stack_function(0);

    char** arr_pointers = (char**) malloc(sizeof(char*) * 1000000);
    for(int i = 0; i < 1000000; i++){
        arr_pointers[i] = (char*) malloc(4096);
    }
    sleep(3);
    for(int i = 0; i < 1000000; i++){
        free(arr_pointers[i]);
    }
    free(arr_pointers);

    void *addr = mmap(NULL, 10 * sysconf(_SC_PAGESIZE), PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0);
    printf("Адрес нового региона: %p\n", addr);

    sleep(10);
    printf("Смена прав\n");
    mprotect(addr, 10 * sysconf(_SC_PAGESIZE), PROT_NONE);

    sleep(10);
    printf("Отсоединили часть\n");
    munmap(addr + 3 * sysconf(_SC_PAGESIZE), 3 * sysconf(_SC_PAGESIZE));

    sleep(10);
    munmap(addr, 3 * sysconf(_SC_PAGESIZE));
    munmap(addr + 6 * sysconf(_SC_PAGESIZE), 3 * sysconf(_SC_PAGESIZE));
    sleep(10);
}

int main(int argc, char** argv) {
    if(argc == 1){
        versionA(argc, argv);
    }
    else{
        if(strcmp(argv[1], "-c") == 0){
            versionC(argc, argv);
        }
        else {
            versionA(argc, argv);
        }
    }
//    int pid = getpid();
//
//    char filepath[1024];
//    sprintf(filepath, "/proc/%d/maps", pid);
//    FILE *maps = fopen(filepath, "rb");
//    if(maps == NULL){
//        fprintf(stderr, "Error when open %s\n", filepath);
//        return 0;
//    }
//    char data[1024];
//    while(fgets(data, 1024, maps)){
//        printf("%s", data);
//    }
//    sleep(10);
//    printf("\n\n\n\n\n");
//
//    execlp(argv[0], argv[0], NULL);
//
//    exit(1);
}
