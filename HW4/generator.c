#include "udplib/ClientSocket.h"
#include <signal.h>
#include <stdbool.h>

ClientSocket socket1;

void ctrlCHandler(int signal) {
    puts("Normally stop program");
    closeClientSocket(&socket1);
    exit(EXIT_SUCCESS);
}

int random_names_count = 8;
char *random_names[] = {
        "Captain America",
        "Iron Man",
        "Tor",
        "Halk",
        "Hokay",
        "Warrior",
        "Spider Man",
        "Dr. Strange"};

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
    sendToServer(&socket1, "generator");
    printf("Send initializing message\n");
    while (true) {
        char *random_name = random_names[rand() % random_names_count];
        sendToServer(&socket1, random_name);
        printf("Send %s to server\n", random_name);
        sleep(5 + rand() % 10);
    }
}