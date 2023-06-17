#include "udplib/ClientSocket.h"
#include <signal.h>
#include <stdbool.h>

ClientSocket socket1;

void ctrlCHandler(int signal) {
    puts("Normally stop program");
    closeClientSocket(&socket1);
    exit(EXIT_SUCCESS);
}

char *doctors[] = {"dentist", "surgeon", "therapist"};

int main(int argc, char **argv) {
    signal(SIGINT, ctrlCHandler);
    char *ip;
    int port;
    if (argc == 2) {
        ip = "127.0.0.1";
        port = atoi(argv[1]);
    } else if (argc == 3) {
        ip = argv[2];
        port = atoi(argv[1]);
    } else {
        printf("Usage: %s *port* [*ip*=127.0.0.1]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    initClientSocket(&socket1, ip, port);
    sendToServer(&socket1, "duty");
    printf("Send initializing message\n");
    char received_buffer[100];
    char buffer_to_send[200];
    while (true) {
        int size = receiveFromServer(&socket1, received_buffer);
        received_buffer[size] = '\0';
        printf("Receive %s from server\n", received_buffer);
        char* name = received_buffer;
        char* to_doctor = doctors[rand() % 3];
        sprintf(buffer_to_send, "%s|%s", name, to_doctor);
        printf("Process %s, send to %s doctor\n", name, to_doctor);
        sendToServer(&socket1, buffer_to_send);
    }
}