#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        printf("Child pid: %d\n", getpid());
        sleep(30);
        printf("Exit");
        exit(5);
    }
    else{
        printf("Spawned child with PID: %d\n", pid);
    }
    return 0;
}
