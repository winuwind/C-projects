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

int main(int argc, char** argv) {
    versionA(argc, argv);
}