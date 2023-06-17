#include "udplib/ClientSocket.h"
#include <signal.h>
#include <stdbool.h>

ClientSocket socket1;

void ctrlCHandler(int signal) {
    puts("Normally stop program");
    closeClientSocket(&socket1);
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    signal(SIGINT, ctrlCHandler);
    char *ip;
    int port;
    char *post;
    if (argc == 3) {
        port = atoi(argv[1]);
        post = argv[2];
        ip = "127.0.0.1";
    } else if (argc == 4) {
        port = atoi(argv[1]);
        post = argv[2];
        ip = argv[3];
    } else {
        printf("Usage: %s *port* *post* [*ip*=127.0.0.1]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    initClientSocket(&socket1, ip, port);
    sendToServer(&socket1, post);
    printf("Send initializing message\n");
    char received_buffer[100];
    while (true) {
        int size = receiveFromServer(&socket1, received_buffer);
        received_buffer[size] = '\0';
        printf("Receive %s from server\n", received_buffer);
        sleep(1 + rand() % 5);
        printf("Heal %s\n", received_buffer);
        sendToServer(&socket1, received_buffer);
    }
}