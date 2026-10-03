#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sched.h>
#include <fcntl.h>
#include <time.h>
#include <malloc.h>
#include <signal.h>

int port = 11234;
int sockfd;
volatile int flag_working = 1;
struct sockaddr_in server_addr;
volatile int count_waiting = 0;

int listener(void* ){
    count_waiting++;
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_len = sizeof(client_addr);
    int client_fd;
    if((client_fd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len)) < 0){
        flag_working = 0;
        sleep(1);
        perror("access");
    }
    count_waiting--;
    if(!flag_working){
        return 0;
    }

    printf("Client %d has connected: %s:%d\n", client_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    char buffer[1025];
    ssize_t received;

    while(flag_working){
        memset(buffer, 0, 1024);
        received = recv(client_fd, buffer, 1024, MSG_DONTWAIT);
        if (received < 0) {
            usleep(10);
            continue;
        }
        else if(received == 0){
            break;
        }

        buffer[received] = '\0';
        printf("Receive message from client %d: %s\n", client_fd, buffer);


        if(send(client_fd, buffer, received, 0) < 0){
            printf("Error when send echo-answer to client %d\n", client_fd);
        }
    }
    close(client_fd);
    printf("Connection with client %d has closed\n", client_fd);
    return 0;
}

int scanner(void* ){
    char line[1025];
    while(flag_working){
        memset(line, 0, 1025);
        fgets(line, 1024, stdin);
        line[strcspn(line, "\n")] = '\0';
        if(strcmp(line, "exit") == 0 || line[0] == 0){
            flag_working = 0;
        }
    }
    return 0;
}

void my_free(char** buf, int size){
    for(int i = 0; i < size; i++){
        free(buf[i]);
    }
    free(buf);
}

int main() {
    socklen_t client_len;
    char buffer[1025];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("TCP echo-server has started on port %d\n", port);

    char** arr_stacks = (char**) malloc(sizeof(char*) * 1024);
    arr_stacks[0] = (char*) malloc(sizeof(char) * 1024 * 1024);
    int size_arr = 1;

    char* stack = arr_stacks[0];

    if(clone(scanner, stack + 1024 * 1024, CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD, NULL) == -1){
        flag_working = 0;
        my_free(arr_stacks, size_arr);
        perror("clone failed");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 10) < 0) {
        flag_working = 0;
        my_free(arr_stacks, size_arr);
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    while (flag_working) {
        int pending;
        socklen_t len = sizeof(pending);

        if (getsockopt(sockfd, SOL_SOCKET, SO_ACCEPTCONN, &pending, &len) < 0) {
            flag_working = 0;
            my_free(arr_stacks, size_arr);
            perror("getsockopt error");
            exit(EXIT_FAILURE);
        }

        if(pending - count_waiting <= 0){
            sleep(1);
            continue;
        }

        arr_stacks[size_arr] = (char*) malloc(sizeof(char) * 1024 * 1024);
        stack = arr_stacks[size_arr++];
        if(clone(listener, stack + 1024 * 1024, CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD, NULL) == -1){
            flag_working = 0;
            sleep(2);
            my_free(arr_stacks, size_arr);
            perror("clone failed");
            exit(EXIT_FAILURE);
        }
    }

    sleep(2);
    my_free(arr_stacks, size_arr);
    close(sockfd);
    return 0;
}