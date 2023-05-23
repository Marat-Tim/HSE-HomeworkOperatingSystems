#include "BlockingQueue.h"
#include "utils.h"
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int createSeverSocket(int port) {
    int server_socket;
    struct sockaddr_in echo_serv_addr;
    if ((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        dieWithError("socket() failed");
    }
    memset(&echo_serv_addr, 0, sizeof(echo_serv_addr));
    echo_serv_addr.sin_family = AF_INET;
    echo_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    echo_serv_addr.sin_port = htons(port);
    if (bind(server_socket, (struct sockaddr *) &echo_serv_addr, sizeof(echo_serv_addr)) < 0) {
        dieWithError("bind() failed");
    }
    if (listen(server_socket, 5) < 0) {
        dieWithError("listen() failed");
    }
    return server_socket;
}

void identifyClient(int socket, int client_sockets[]) {
    int client_socket;
    struct sockaddr_in echoClntAddr;
    unsigned int clntLen;
    clntLen = sizeof(echoClntAddr);

    if ((client_socket = accept(socket, (struct sockaddr *) &echoClntAddr,
                                &clntLen))
        < 0)
        dieWithError("accept() failed");
    char echoBuffer[32];
    if ((recv(client_socket, echoBuffer, 32, 0)) < 0)
        dieWithError("recv() failed");
    int client_type = atoi(echoBuffer);
    printf("%d\n", client_type);
    if (client_type >= 6 || client_type < 0 || client_sockets[client_type] > 0) {
        putsSync("Incorrect client");
    } else {
        putsSync("Correct client connect");
        client_sockets[client_type] = client_socket;
    }
}

struct BlockingQueue duty_doctors_queue;

void *patientGenerator(void *arg) {
    int socket = *(int *) arg;
    char echoBuffer[32];
    int recvMsgSize;
    if ((recvMsgSize = recv(socket, echoBuffer, 32, 0)) < 0)
        dieWithError("recv() failed");

    puts("Adding null to end of string");

    echoBuffer[recvMsgSize] = '\0';

    puts("receive first command from patient generator");

    while (recvMsgSize > 0) {
        puts("adding element to queue in patient generator");
        addToBlockingQueue(&duty_doctors_queue, echoBuffer);
        puts("add element to queue in patient generator");
        if ((recvMsgSize = recv(socket, echoBuffer, 32, 0)) < 0)
            dieWithError("recv() failed");
        puts("receive command from patient generator");
        echoBuffer[recvMsgSize] = '\0';
    }
    close(socket);
    return NULL;
}

struct BlockingQueue treating_doctors_queues[3];

void *dutyDoctor(void *arg) {
    int socket = *(int *) arg;
    int recvMsgSize;
    do {
        puts("trying to get element from queue in dutyDoctor");
        char *patient = getFromBlockingQueue(&duty_doctors_queue);
        puts("get element from queue in dutyDoctor");
        char echoBuffer[32];
        if (send(socket, patient, strlen(echoBuffer), 0) != strlen(echoBuffer))
            dieWithError("send() failed in dutyDoctor");
        if ((recvMsgSize = recv(socket, echoBuffer, 32, 0)) < 0)
            dieWithError("recv() failed in dutyDoctor");
        if (recvMsgSize <= 0) {
            break;
        }
        puts("start manipulation on string in dutyDoctor");
        int doctor_type = echoBuffer[recvMsgSize - 1] - '0';
        echoBuffer[recvMsgSize - 2] = '\0';
        puts("end manipulation on string in dutyDoctor");
        puts("trying to add element to queue in duty doctor");
        addToBlockingQueue(&treating_doctors_queues[doctor_type], echoBuffer);
        puts("add element to queue in duty doctor");
    } while (true);
    close(socket);
    return NULL;
}

struct Args {
    int number;
    int socket;
};

void *treatingDoctor(void *arg) {
    struct Args args = *(struct Args *) arg;
    int socket = args.socket;
    int number = args.number;
    int recvMsgSize;
    do {
        puts("trying to get element from queue in treating doctor");
        char *patient = getFromBlockingQueue(treating_doctors_queues + number);
        puts("get element from queue in treating doctor");
        char echoBuffer[32];
        ssize_t send_size = send(socket, patient, strlen(patient), 0);
        printf("send_size = %zd\n", send_size);
        printf("strlen = %zd\n", strlen(patient));
        if (send_size != strlen(patient)) {
            dieWithError("send() failed");
        }
        if ((recvMsgSize = recv(socket, echoBuffer, 32, 0)) < 0)
            dieWithError("recv() failed");
        puts("set \\0 to the end of string in treating doctor");
        echoBuffer[recvMsgSize] = '\0';
        puts("set \\0 to end success");
        puts(echoBuffer);
    } while (recvMsgSize > 0);
    close(socket);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        dieWithError("Arguments: <server port>");
    }
    int port = atoi(argv[1]);
    int server_socket = createSeverSocket(port);
    int client_sockets[6] = {-1, -1, -1, -1, -1, -1};
    while (containsMinusOne(client_sockets, 6)) {
        identifyClient(server_socket, client_sockets);
    }
    initBlockingQueue(&duty_doctors_queue);
    for (int i = 0; i < 3; ++i) {
        initBlockingQueue(&treating_doctors_queues[i]);
    }
    pthread_t client_threads[6];
    pthread_create(client_threads + 0, NULL, patientGenerator, (void *) &client_sockets[0]);
    pthread_create(client_threads + 1, NULL, dutyDoctor, (void *) &client_sockets[1]);
    pthread_create(client_threads + 2, NULL, dutyDoctor, (void *) &client_sockets[2]);
    struct Args args1;
    args1.socket = client_sockets[3];
    args1.number = 0;
    struct Args args2;
    args1.socket = client_sockets[4];
    args1.number = 1;
    struct Args args3;
    args1.socket = client_sockets[5];
    args1.number = 2;
    pthread_create(client_threads + 3, NULL, treatingDoctor, (void *) &args1);
    pthread_create(client_threads + 4, NULL, treatingDoctor, (void *) &args2);
    pthread_create(client_threads + 5, NULL, treatingDoctor, (void *) &args3);

    puts("create all threads");

    for (int i = 0; i < 6; ++i) {
        pthread_join(client_threads[i], NULL);
    }
    puts("End of the program");
}