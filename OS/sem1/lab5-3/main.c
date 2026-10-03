#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define STACK_SIZE (4096)

int func_recursive(int count){
    volatile char line[] = "Hello world!\n";
    char x = line[0];
    if(count == 0){
        return x;
    }
    return func_recursive(count - 1);
}

int func_start(void* ){
    volatile int x = func_recursive(10);
    return x;
}

int main(int argc, char** argv) {
    char filename[1025] = "./Stack_full.bin";
    if(argc == 2){
        strcpy(filename, argv[1]);
    }
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, STACK_SIZE) == -1) {
        perror("ftruncate");
        close(fd);
        exit(EXIT_FAILURE);
    }
    void* stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (stack == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    int pid, status;
    if((pid = clone(func_start, (char*) stack + STACK_SIZE, CLONE_FILES | SIGCHLD | CLONE_VM, NULL)) == -1){
        perror("clone");
        exit(EXIT_FAILURE);
    }

    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("Child exited normally with code = %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Child was terminated by signal %d\n", WTERMSIG(status));
    } else {
        printf("Child terminated abnormally\n");
    }

    munmap(stack, STACK_SIZE);
    close(fd);
    return 0;
}
