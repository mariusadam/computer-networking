//
// Created by marius on 10/4/16.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <stdlib.h>

#define handle_error(msg) \
    do { \
        perror(msg); \
        exit(EXIT_FAILURE); \
    } while (0)

int main(int argc, char* argv[]) {
    int c;
    uint16_t stringLength;
    struct sockaddr_in server;
    size_t maxSize  = 255;
    char *sir       = NULL;
    char *reverse   = NULL;

    uint16_t port = 6666;
    char serverIp[] = "193.226.40.130";
    if (argc > 1) {
        strcpy(serverIp, argv[1]);
        if (argc > 2) {
            uint16_t newPort = atoi(argv[2]);
            if (newPort != 0) {
                port = newPort;
            }
        }
    }

    c = socket(AF_INET, SOCK_STREAM, 0);
    if (c < 0) {
        handle_error("Error creating client socket");
    }

    memset(&server, 0, sizeof(server));
    server.sin_port        = htons(port);
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = inet_addr(serverIp);

    if (connect(c, (struct sockaddr *) &server, sizeof(server)) < 0) {
        char msg[255];
        sprintf(msg, "Could not connect to %s", serverIp);
        handle_error(msg);
    }

    printf("Please enter the string: ");
    getline(&sir, &maxSize, stdin);
    stringLength = strlen(sir) - 1;
    //remove trailing \n
    sir[stringLength] = '\0';
    printf("Client read %d bytes : \'%s\'\n", stringLength, sir);

    stringLength = htons(stringLength);
    send(c, &stringLength, sizeof(stringLength), 0);
    send(c, sir, strlen(sir) + 1, 0);

    uint16_t strlenRecv;
    recv(c, &strlenRecv, sizeof(strlenRecv), MSG_WAITALL);
    strlenRecv = ntohs(strlenRecv);
    printf("Received string size %d\n", strlenRecv);

    reverse = (char*) malloc((strlenRecv + 1) * sizeof(char));
    memset(reverse, '\0', (strlenRecv * 1) * sizeof(char));

    recv(c, reverse, strlenRecv, MSG_WAITALL);
    reverse[strlenRecv] = '\0';
    printf("The reversed string is: \'%s\' \n", reverse);


    free(sir);
    free(reverse);

    close(c);

    return 0;
}