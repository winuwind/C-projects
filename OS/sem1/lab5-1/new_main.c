#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int global_var = 100;

int main() {
    int local_var = 200;

    printf("PARENT PROCESS\n");
    printf("pid: %d\n", getpid());
    printf("Global var: address=%p, value=%d\n", &global_var, global_var);
    printf("Local var: address=%p, value=%d\n", &local_var, local_var);


    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf("\nCHILD PROCESS\n");
        printf("Child pid: %d\n", getpid());
        printf("Parent pid: %d\n", getppid());
        printf("Global var 1: address=%p, value=%d\n", &global_var, global_var);
        printf("Local var 1: address=%p, value=%d\n", &local_var, local_var);


        sleep(30);

        printf("\nCHILD PROCESS\n");
        printf("Global var 2: %d\n", global_var);
        printf("Local var 2: %d\n", local_var);

        exit(5);
    } else {
        sleep(5);

        global_var = 300;
        local_var = 400;

        printf("\nPARENT PROCESS\n");
        printf("Global var: %d\n", global_var);
        printf("Local var: %d\n", local_var);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Child exited normally with code %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child terminated by signal %d\n", WTERMSIG(status));
        } else {
            printf("Child terminated abnormally\n");
        }
    }

    return 0;
}
