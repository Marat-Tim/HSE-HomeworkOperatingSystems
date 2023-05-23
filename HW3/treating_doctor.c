#include "utils.h"
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <unistd.h>     /* for close() */

#define RCVBUFSIZE 32 /* Size of receive buffer */

int main(int argc, char *argv[]) {
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */

    if (argc != 4) /* Test for correct number of arguments */
    {
        dieWithError("Arguments: <Server IP> <Echo Port> <number>\n");
    }

    servIP = argv[1]; /* First arg: server IP address (dotted quad) */

    echoServPort = atoi(argv[2]); /* Use given port, if any */

    int number = atoi(argv[3]) + 3;

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        dieWithError("socket() failed");

    puts("create socket");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
    echoServAddr.sin_port = htons(echoServPort);      /* Server port */

    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        dieWithError("connect() failed");

    puts("connect to server");

    char msg[2] = {number + '0', '\0'};

    if (send(sock, msg, 2, 0) != 2)
        dieWithError("send() sent a different number of bytes than expected");

    puts("send message to server");

    char echoBuffer[32];
    int recvMsgSize;
    if ((recvMsgSize = recv(sock, echoBuffer, 32, 0)) < 0)
        dieWithError("recv() failed");
    echoBuffer[recvMsgSize] = '\0';
    while (recvMsgSize > 0) {

        sprintf(echoBuffer, "Hello world");

        if (send(sock, echoBuffer, strlen(echoBuffer), 0) != strlen(echoBuffer))
            dieWithError("send() sent a different number of bytes than expected");
        if ((recvMsgSize = recv(sock, echoBuffer, 32, 0)) < 0)
            dieWithError("recv() failed");
        echoBuffer[recvMsgSize] = '\0';
    }
    close(sock);
}