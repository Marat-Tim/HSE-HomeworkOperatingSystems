#ifndef IHW4_UDPLIB_SERVER_SOCKET_H_
#define IHW4_UDPLIB_SERVER_SOCKET_H_

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"

typedef struct ServerSocketStruct {
    int _fd;
    uint16_t port;
} ServerSocket;

void initServerSocket(ServerSocket *server_socket, uint16_t port) {
    if ((server_socket->_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("server socket() failed");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_socket->port = port;
    if (bind(server_socket->_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("server bind() failed");
        exit(EXIT_FAILURE);
    }
}

ssize_t sendTo(ServerSocket *server_socket, char *buffer, struct sockaddr_in *client_addr) {
    printf("client_addr->sin_addr.s_addr=%d\n"
           "client_addr->sin_port=%d\n"
           "client_addr->sin_zero=%s\n"
           "client_addr->sin_family=%d\n",
           client_addr->sin_addr.s_addr,
           client_addr->sin_port,
           client_addr->sin_zero,
           client_addr->sin_family);
    size_t buffer_size = strlen(buffer);
    if (buffer_size > MAX_BUFFER_SIZE) {
        perror("buffer larger than max");
        exit(EXIT_FAILURE);
    }
    ssize_t send_char_count = sendto(server_socket->_fd, buffer, buffer_size, 0,
               (const struct sockaddr *)client_addr, sizeof(*client_addr));
    if (send_char_count == -1) {
        perror("server sendto() failed, return -1");
        exit(EXIT_FAILURE);
    }
    if (send_char_count != buffer_size) {
        perror("server sendto() failed");
        fprintf(stderr, "send_char_count=%zd, your buffer_size=%zu", send_char_count, buffer_size);
        exit(EXIT_FAILURE);
    }
    return send_char_count;
}

ssize_t receiveFrom(ServerSocket *server_socket, char* buffer, struct sockaddr_in *client_addr) {
    socklen_t _;
    ssize_t receive_char_count = recvfrom(server_socket->_fd, buffer, MAX_BUFFER_SIZE, 0,
                 (struct sockaddr *)client_addr, &_);
    if (receive_char_count < 0) {
        perror("server recvfrom() failed, return -1");
        exit(EXIT_FAILURE);
    }
    return receive_char_count;
}

void closeServerSocket(ServerSocket *server_socket) {
    close(server_socket->_fd);
}

#endif//IHW4_UDPLIB_SERVER_SOCKET_H_
