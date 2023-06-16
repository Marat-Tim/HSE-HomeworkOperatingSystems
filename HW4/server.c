#include <stdbool.h>
#include <signal.h>
#include "udplib/ServerSocket.h"
#include "OptionalAddr.h"

const int MAX_SIZE = 100;

ServerSocket socket1;

void ctrlCHandler(int signal) {
    puts("Normally stop program");
    closeServerSocket(&socket1);
    exit(EXIT_SUCCESS);
}

OptionalAddr generator = {.has_value = false};
OptionalAddr duty1 = {.has_value = false};
OptionalAddr duty2 = {.has_value = false};
OptionalAddr dentist = {.has_value = false};
OptionalAddr surgeon = {.has_value = false};
OptionalAddr therapist = {.has_value = false};

bool isInitialized() {
    return generator.has_value && duty1.has_value && duty2.has_value && dentist.has_value && surgeon.has_value
           && therapist.has_value;
}

void initialize(char *post, struct sockaddr_in addr) {
    if (strcmp(post, "generator") == 0) {
        set(&generator, addr);
        printf("Initialize generator\n");
    } else if (strcmp(post, "duty") == 0) {
        if (!duty1.has_value) {
            set(&duty1, addr);
            printf("Initialize duty1\n");
        } else {
            set(&duty2, addr);
            printf("Initialize duty2\n");
        }
    } else if (strcmp(post, "dentist") == 0) {
        set(&dentist, addr);
        printf("Initialize dentist\n");
    } else if (strcmp(post, "surgeon") == 0) {
        set(&surgeon, addr);
        printf("Initialize surgeon\n");
    } else if (strcmp(post, "therapist") == 0) {
        set(&therapist, addr);
        printf("Initialize therapist\n");
    } else {
        printf("Wrong post: %s\n", post);
    }
    if (isInitialized()) {
        printf("All initialized\n");
    }
}

void process(char *message, struct sockaddr_in addr) {
    printf("process %s from %d\n", message, addr.sin_addr.s_addr);
}

int main(int argc, char **argv) {
    signal(SIGINT, ctrlCHandler);
    if (argc != 2) {
        printf("Usage: ./%s *port*", argv[0]);
        exit(EXIT_FAILURE);
    }
    initServerSocket(&socket1, atoi(argv[1]));
    char buffer[MAX_SIZE];
    struct sockaddr_in client_addr;
    while (1) {
        int size = receive(&socket1, buffer, &client_addr);
        buffer[size] = '\0';
        if (!isInitialized()) {
            initialize(buffer, client_addr);
        } else {
            process(buffer, client_addr);
        }
    }
}