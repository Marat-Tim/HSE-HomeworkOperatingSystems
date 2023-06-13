#ifndef IHW4_UDPLIB_CLIENTSOCKET_H_
#define IHW4_UDPLIB_CLIENTSOCKET_H_

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"

typedef struct ClientSocketStruct {
    int _fd;
    struct sockaddr_in _server_addr;
} ClientSocket;

void initClientSocket(ClientSocket *client_socket, char *ip, uint16_t host) {
    if ((client_socket->_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("client socket() failed");
        exit(EXIT_FAILURE);
    }
    memset(&client_socket->_server_addr, 0, sizeof(client_socket->_server_addr));
    client_socket->_server_addr.sin_family = AF_INET;
    client_socket->_server_addr.sin_addr.s_addr = inet_addr(ip);
    client_socket->_server_addr.sin_port = htons(host);
}

ssize_t sendToServer(ClientSocket *client_socket, char *buffer) {
    size_t buffer_size = strlen(buffer);
    if (buffer_size > MAX_BUFFER_SIZE) {
        perror("buffer larger than max");
        exit(EXIT_FAILURE);
    }
    ssize_t send_char_count = sendto(client_socket->_fd, buffer, buffer_size, 0,
                                     (struct sockaddr *)&client_socket->_server_addr,
                                     sizeof(client_socket->_server_addr));
    if (send_char_count == -1) {
        perror("client sendto() failed, return -1");
        exit(EXIT_FAILURE);
    }
    if (send_char_count != buffer_size) {
        perror("client sendto() failed");
        fprintf(stderr, "send_char_count=%zd, your buffer_size=%zu", send_char_count, buffer_size);
        exit(EXIT_FAILURE);
    }
    return send_char_count;
}

ssize_t receiveFromServer(ClientSocket *client_socket, char *buffer) {
    socklen_t _;
    ssize_t receive_char_count = recvfrom(client_socket->_fd, buffer, MAX_BUFFER_SIZE, 0,
                                          (struct sockaddr *)&client_socket->_server_addr, &_);
    if (receive_char_count == -1) {
        perror("client recvfrom() failed, return -1");
        exit(EXIT_FAILURE);
    }
    return receive_char_count;
}

void closeClientSocket(ClientSocket *client_socket) {
    close(client_socket->_fd);
}

#endif//IHW4_UDPLIB_CLIENTSOCKET_H_
