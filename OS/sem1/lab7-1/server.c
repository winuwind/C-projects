#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int port = 11234;

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[1025];

    // Создание UDP сокета
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Привязка сокета к адресу
    if (bind(sockfd, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP echo-server has started on port %d\n", port);

    while (1) {

        memset(buffer, 0, 1024);

        client_len = sizeof(client_addr);

        // Получение данных от клиента
        ssize_t len = recvfrom(sockfd, (char *) buffer, 1024, 0,
                               (struct sockaddr *) &client_addr, &client_len);
        if(len < 0){
            perror("recv");
            break;
        }
        if(len == 0){
            break;
        }

        printf("Receive message from %s:%d: %s\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

        // Отправка данных обратно клиенту
        if(sendto(sockfd, (const char *) buffer, len, 0,
               (const struct sockaddr *) &client_addr, client_len) < 0){
            perror("send");
            break;
        }

        printf("Send echo-answer to %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }

    close(sockfd);
    return 0;
}