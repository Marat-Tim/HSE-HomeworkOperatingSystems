#include "udplib/ClientSocket.h"

int main(int argc, char** argv) {
    ClientSocket socket;
    initClientSocket(&socket, "127.0.0.1", atoi(argv[1]));
    sendToServer(&socket, argv[2]);
    char buffer[100];
    int size = receiveFromServer(&socket, buffer);
    buffer[size] = '\0';
    printf("server return %s", buffer);
    closeClientSocket(&socket);
}