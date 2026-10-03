#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
int port = 11234;
char ip_addr[INET_ADDRSTRLEN] = "127.0.0.1";

int main(int argc, char *argv[]) {
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-p") == 0){
            port = strtol(argv[++i], NULL, 10);
        }
        else if(strcmp(argv[i], "-a") == 0){
            if(strlen(argv[++i]) > INET_ADDRSTRLEN){
                printf("Incorrect format ip address\n");
                exit(EXIT_FAILURE);
            }
            strcpy(ip_addr, argv[i]);
        }
        else if((strcmp(argv[i], "-h") == 0)){
            printf("\"-p <port>\" - for enter server port\n"
                   "\"-a <ip>\" - for enter server ip address\n");
            exit(EXIT_FAILURE);
        }
    }

    int sockfd;
    struct sockaddr_in server_addr;
    char buffer_send[BUFFER_SIZE];
    char buffer_recv[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_addr, &server_addr.sin_addr) <= 0) {
        perror("invalid address");
        exit(EXIT_FAILURE);
    }


    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect error");
        close(sockfd);
        return 1;
    }


    printf("TCP client has connected to server %s:%d\n", ip_addr, port);

    while (1) {
        memset(buffer_send, 0, BUFFER_SIZE);
        memset(buffer_recv, 0, BUFFER_SIZE);
        printf("Enter message (or 'exit' for exit): ");
        fgets(buffer_send, BUFFER_SIZE, stdin);
        buffer_send[strcspn(buffer_send, "\n")] = '\0';

        if (strcmp(buffer_send, "exit") == 0 || buffer_send[0] == 0) {
            break;
        }

        if(send(sockfd, buffer_send, strlen(buffer_send), 0) == -1){
            close(sockfd);
            perror("Error when send");
            exit(EXIT_FAILURE);
        }

        ssize_t len = recv(sockfd, buffer_recv, BUFFER_SIZE, 0);
        if(len < 0){
            printf("Connection timed out\n");
            break;
        }
        else if(len == 0){
            printf("Server closed\n");
            break;
        }

        buffer_send[len] = '\0';
        printf("Server answer: %s\n", buffer_send);
    }
    printf("Connection closed\n");
    if(send(sockfd, buffer_send, strlen(buffer_send), 0) == -1){
        close(sockfd);
        perror("Error when send");
        exit(EXIT_FAILURE);
    }
    close(sockfd);
    return 0;
}