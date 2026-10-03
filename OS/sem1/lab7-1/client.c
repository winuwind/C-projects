#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>

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
    char buffer[BUFFER_SIZE];

    // Создание UDP сокета
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    memset(&server_addr, 0, sizeof(server_addr));

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_addr, &server_addr.sin_addr) <= 0) {
        perror("invalid address");
        exit(EXIT_FAILURE);
    }

    printf("UDP client has connected to server %s:%d\n", ip_addr, port);

    while (1) {
        printf("Enter message (or 'exit' for exit): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Удаление символа новой строки

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // Отправка сообщения серверу
        if(sendto(sockfd, (const char *)buffer, strlen(buffer), 0,
               (const struct sockaddr *)&server_addr, sizeof(server_addr) < 0)){
            close(sockfd);
            perror("Error when send");
            exit(EXIT_FAILURE);
        }

        int start = clock();
        int end = clock();
        // Получение ответа от сервера

        ssize_t len = 0;

        //MSG_DONTWAIT

        while(len <= 0 && end - start < 5 * CLOCKS_PER_SEC){
            len = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, NULL, NULL);
            end = clock();
        }

        if(len <= 0){
            printf("Connection timed out\n");
            continue;
        }

        buffer[len] = '\0';

        printf("Server answer: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}