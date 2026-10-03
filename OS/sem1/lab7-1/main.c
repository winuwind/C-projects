#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int main(int ac, char **av) {
    int socket_id;
    if((socket_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        perror("Can't create socket");
    }


    return (0);
}