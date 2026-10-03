#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pidB, pidC;
    pidB = fork();
    if (pidB == 0) {
        pidC = fork();
        if (pidC == 0) {
            sleep(10);
            printf("Process C: PID = %d, PPID = %d\n", getpid(), getppid());
            sleep(10);
            exit(0);
        } else {
            printf("Process B (zombie): PID = %d, child C = %d\n", getpid(), pidC);
            exit(0);
        }
    } else {
        sleep(30);
    }
    return 0;
}
