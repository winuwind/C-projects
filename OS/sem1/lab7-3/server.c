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
#include <poll.h>


int port = 11234;
int sockfd;
volatile int flag_working = 1;
struct sockaddr_in server_addr;
volatile int count_waiting = 0;

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
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("TCP echo-server has started on port %d\n", port);

    if (listen(sockfd, 10) < 0) {
        flag_working = 0;
        perror("listen error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    struct pollfd *fds = malloc(sizeof(struct pollfd) * 100);
    int nfds = 2;
    int timeout = -1;

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    fds[1].fd = sockfd;
    fds[1].events = POLLIN;

    while (flag_working) {
        int ret = poll(fds, nfds, timeout);
        if(ret < 0){
            flag_working = 0;
            perror("poll");
            for(int i = 1; i < nfds; i++){
                close(fds[i].fd);
            }
            free(fds);
            exit(EXIT_FAILURE);
        }
        if(fds[0].revents & POLLIN){
            char line[1025];
            if(fgets(line, 1024, stdin) == NULL){
                memset(line, 0, 1025);
                flag_working = 0;
                perror("fgets");
                for(int i = 1; i < nfds; i++){
                    close(fds[i].fd);
                }
                free(fds);
                exit(EXIT_FAILURE);
            }
            line[strcspn(line, "\n")] = '\0';
            if(strcmp(line, "exit") == 0 || line[0] == 0){
                flag_working = 0;
            }
            if(strcmp(line, "count") == 0){
                printf("%d\n", nfds - 2);
            }
            fds[0].revents = 0;
        }
        if(fds[1].revents & POLLIN){
            int client_fd;
            struct sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(client_addr));
            socklen_t client_len = sizeof(client_addr);
            if((client_fd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len)) < 0){
                flag_working = 0;
                perror("accept");
                for(int i = 1; i < nfds; i++){
                    close(fds[i].fd);
                }
                free(fds);
                exit(EXIT_FAILURE);
            }
            if(nfds >= 100){
                close(client_fd);
            }
            else{
                fds[nfds].fd = client_fd;
                fds[nfds++].events = POLLIN;
                printf("Client %d has connected: %s:%d\n", client_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            }

            fds[1].revents = 0;
        }
        for(int i = 2; i < nfds; i++){
            if(fds[i].revents & POLLIN) {
                char buffer[1025];
                ssize_t received;
                memset(buffer, 0, 1024);
                received = recv(fds[i].fd, buffer, 1024, 0);
                if (received < 0) {
                    perror("recv");
                    for (int i = 1; i < nfds; i++) {
                        close(fds[i].fd);
                    }
                    free(fds);
                    exit(EXIT_FAILURE);
                } else if (received == 0) {
                    close(fds[i].fd);
                    printf("Connection with client %d has closed\n", fds[i].fd);
                    fds[i].fd = fds[--nfds].fd;
                    continue;
                }

                buffer[received] = '\0';
                printf("Receive message from client %d: %s\n", fds[i].fd, buffer);

                if (send(fds[i].fd, buffer, received, 0) < 0) {
                    printf("Error when send echo-answer to client %d\n", fds[i].fd);
                }
                fds[i].revents = 0;
            }
        }
    }

    for(int i = 1; i < nfds; i++){
        close(fds[i].fd);
    }

    free(fds);
    return 0;
}