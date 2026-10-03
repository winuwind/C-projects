#include "my_malloc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void parse_command(int* count, char** argv, char* command){
    for(int i = 0; i < 10; i++){
        argv[i][0] = 0;
    }

    int index = 0;
    unsigned last_space = 0;
    unsigned len = strlen(command);
    if(command[len - 1] == '\n'){
        command[--len] = 0;
    }
    for(unsigned i = 0; i < len; i++){
        if(command[i] == ' '){
            last_space = i + 1;
            index++;
            continue;
        }
        argv[index][i - last_space] = command[i];
        argv[index][i - last_space + 1] = 0;
    }
    *count = index + 1;
}

int find_free(char** memories){
    for(int i = 0; i < 100; i++){
        if(memories[i] == NULL){
            return i;
        }
    }
    return -1;
}

int main(int argc, char **argv) {
    char flag_runtime = 0;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-p") == 0 && i != argc - 1) {
                count_pages = strtol(argv[++i], NULL, 10);
            } else if (strcmp(argv[i], "-s") == 0) {
                printf("page_size = %li\n\n", sysconf(_SC_PAGESIZE));
            } else if (strcmp(argv[i], "-h") == 0) {
                printf("-h\t\t\t\t\tPrint help for program\n");
                printf("-s\t\t\t\t\tPrint size of page\n");
                printf("-p <count>\t\t\t\tAlloc count pages fo program\n");
                printf("-x\\t\\t\\t\\t\\tRun with keyboard input:\\n\");\n"
                       "                printf(\"1.\\\"malloc <size>\\\" - malloc memory\\n\");\n"
                       "                printf(\"2.\\\"free <id>\\\" - free memory allocated with identification number \\\"id\\\"\\n\");\n"
                       "                printf(\"3.\\\"put <id> \\\"<string>\\\"\\\" - put \\\"string\\\" to memory with identification number \\\"id\\\"\\n\");\n"
                        "                printf(\"4.\\\"print <id>\\\" - Print content in memory with identification number \"id\"\n");
                printf("5.\"exit\" - free all memory and exit\n");
                printf("6.\"all\" - Print info about all allocated memory\n\n");
            } else if (strcmp(argv[i], "-x") == 0) {
                flag_runtime = 1;
            }
        }
    }
    if (init() != 0) {
        fprintf(stderr, "Error when init\n");
        return -1;
    }
    if (!flag_runtime) {
        char *hello = (char *) my_malloc(5000);
        if (hello != NULL) {
            sprintf(hello, "Hello, World!\n");
            fprintf(stdout, "line = %s", hello);
            fprintf(stdout, "pointer = %p\n", hello);
        }
        if (my_free(hello) != 0) {
            fprintf(stderr, "Error when free\n");
        }
        if (hello != NULL) {
            printf("old pointer = %p\n", hello);
        }
        hello = (char *) my_malloc(7000);
        if (hello != NULL) {
            sprintf(hello, "Goodbye, World!\n");
            fprintf(stdout, "line = %s", hello);
        }
        if (my_free(hello) != 0) {
            fprintf(stderr, "Error when free\n");
        }
        if (hello != NULL) {
            printf("new pointer = %p\n", hello);
        }
    } else {
        printf("<");
        char command[1024];
        char* arguments[10];
        for(int i = 0; i < 10; i++){
            arguments[i] = (char *) my_malloc(256);
            if(arguments[i] == NULL){
                fprintf(stderr, "Not enough memory\n");
                finalize();
                return -1;
            }
        }
        int count_args = 0;
        char* memories[100];
        for(int i = 0; i < 100; i++){
            memories[i] = NULL;
        }
        while (fgets(command, 1024, stdin) != NULL) {
            parse_command(&count_args, arguments, command);
            if(strcmp(arguments[0], "malloc") == 0){
                int x = find_free(memories);
                if(x == -1){
                    printf(">Not enough space\n");
                    continue;
                }
                char* pointer = (char*) my_malloc(strtoul(arguments[1], NULL, 10));
                if(pointer == NULL){
                    printf(">Not enough space\n");
                    continue;
                }
                memories[x] = pointer;
                printf(">id = %d: pointer = %p\n", x, pointer);
            } else if(strcmp(arguments[0], "free") == 0) {
                int x = strtol(arguments[1], NULL, 10);
                my_free(memories[x]);
                memories[x] = NULL;
            } else if(strcmp(arguments[0], "put") == 0) {
                int x = strtol(arguments[1], NULL, 10);
                if(memories[x] == NULL){
                    printf(">memory with this id doesn't exist\n");
                    continue;
                }
                unsigned int index = 0;
                for(int i = 2; i < 10; i++){
                    strcpy(&memories[x][index], arguments[i]);
                    index += strlen(arguments[i]);
                    memories[x][index++] = ' ';
                }
            } else if(strcmp(arguments[0], "print") == 0) {
                int x = strtol(arguments[1], NULL, 10);
                if(memories[x] == NULL){
                    printf(">memory with this id doesn't exist\n");
                    continue;
                }
                printf(">%s\n", memories[x]);
            } else if(strcmp(arguments[0], "exit") == 0) {
                for(int i = 0; i < 100; i++){
                    if(memories[i] != NULL){
                        my_free(memories[i]);
                    }
                }
                break;
            } else if(strcmp(arguments[0], "all") == 0) {
                for(int i = 0; i < 100; i++){
                    if(memories[i] != NULL){
                        printf(">id = %d, pointer = %p: %s\n", i, memories[i], memories[i]);
                    }
                }
            }
            printf("<");
        }
        for(int i = 0; i < 10; i++){
            my_free(arguments[i]);
        }
    }
    finalize();
    return 0;
}
