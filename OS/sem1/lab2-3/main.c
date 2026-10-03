#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/user.h>

char* get_syscall_name(unsigned long long code) {
    switch (code) {
        case 0: return "read";
        case 1: return "write";
        case 2: return "open";
        case 3: return "close";
        case 4: return "stat";
        case 5: return "fstat";
        case 9: return "mmap";
        case 10: return "mprotect";
        case 11: return "munmap";
        case 12: return "brk";
        case 17: return "pread64";
        case 21: return "access";
        case 59: return "execve";
        case 60: return "exit";
        case 231: return "exit_group";
        case 257: return "openat";
        default: return "unknown syscall";
    }
}

int main() {
    pid_t child = fork();
    if (child == -1) {
        perror("fork");
        exit(1);
    }

    if (child == 0) {
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
            perror("ptrace");
            exit(1);
        }
        execlp("wget", "wget", "kernel.org", (char*) NULL);

        perror("execlp");
        exit(1);
    }
    else {
        int status;
        struct user_regs_struct state;
        waitpid(child, &status, 0);
        while (1) {
            if (ptrace(PTRACE_SYSCALL, child, NULL, NULL) == -1) {
                perror("ptrace");
                exit(1);
            }
            waitpid(child, &status, 0);

            ptrace(PTRACE_GETREGS, child, 0, &state);
            printf("%s\n", get_syscall_name(state.orig_rax));

            if (WIFEXITED(status)) {
                break;
            }
        }
    }

    return 0;
}
