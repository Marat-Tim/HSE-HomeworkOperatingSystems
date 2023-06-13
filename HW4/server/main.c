#include "../udplib/ServerSocket.h"

int main(int argc, char** argv) {
    ServerSocket socket;
    initServerSocket(&socket, atoi(argv[1]));
    char buffer[100];
    while (1) {
        struct sockaddr client_addr;
        int size = receiveFrom(&socket, buffer, &client_addr);
        buffer[size] = '\0';
        printf("received message = %s\n", buffer);
        sendTo(&socket, buffer, &client_addr);
    }
}