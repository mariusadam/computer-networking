//
// Created by marius on 10/4/16.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>

#define LEVEL_DEBUG    1
#define LEVEL_INFO     2
#define LEVEL_CRITICAL 3

void reverseString(char *normal, char *reversed) {
    if (normal == NULL || reversed == NULL) {
        return;
    }

    int len = strlen(normal), i;
    for (i = len - 1; i >= 0; i--) {
        reversed[len - i - 1] = normal[i];
    }
}

void __log(const char* message, int level) {
    char file[] = "server.log";
    FILE *fp = fopen(file, "a");
    time_t currentTime;
    time(&currentTime);
    char timeStr[30];
    strcpy(timeStr, ctime(&currentTime));
    timeStr[strlen(timeStr) - 2] = '\0';

    if (level == LEVEL_INFO) {
        fprintf(fp, "[%s].INFO : %s\n", timeStr, message);
    }
    if (level == LEVEL_DEBUG) {
        fprintf(fp, "[%s].DEBUG : %s\n", timeStr, message);
    }
    if (level == LEVEL_CRITICAL) {
        fprintf(fp, "[%s].CRITICAL : %s\n", timeStr, message);
    }
    fclose(fp);
}

void debug(const char* message) {
    __log(message, LEVEL_DEBUG);
}

void info(const char* message) {
    __log(message, LEVEL_INFO);
}

void critical(const char* message) {
    __log(message, LEVEL_CRITICAL);
}

void serve_client(int c) {
	uint16_t stringLength;
    char *received, *reversed;
    char msg[65535];
        
	recv(c, &stringLength, sizeof(stringLength), MSG_WAITALL);
    stringLength = ntohs(stringLength);

    sprintf(msg, "Content length: %d", stringLength);
    info(msg);

    received = (char*) malloc((stringLength + 1) * sizeof(char));
    reversed = (char*) malloc((stringLength + 1) * sizeof(char));
    memset(received, '\0', (stringLength + 1) * sizeof(char));
    memset(reversed, '\0', (stringLength + 1) * sizeof(char));
    recv(c, received, stringLength, MSG_WAITALL);
    received[stringLength] = '\0';

    reverseString(received, reversed);
    stringLength = strlen(reversed);
    stringLength = htons(stringLength);
    send(c, &stringLength, sizeof(stringLength), 0);

    sprintf(msg, "Sent string size %s(%d) ", reversed, (int)strlen(reversed));
    info(msg);

    send(c, reversed, strlen(reversed) + 1, 0);

    sprintf(msg, "Sent string \'%s\'", reversed);
    info(msg);

    free(received);
    free(reversed);
    close(c);
    info("Connection closed with client.");

}

int main(int argc, char *argv[]) {
    int s, c, len;
    char msg[65535];
    struct sockaddr_in server, client;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        sprintf(msg, "Could not create the server socket: %s", strerror(errno));
        critical(msg);
        return 1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_port        = htons(6666);
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (struct sockaddr *) &server, sizeof(server))) {
        sprintf(msg, "Error when trying to bind: %s", strerror(errno));
        critical(msg);
        return 1;
    }

    listen(s, 5);

    len = sizeof(client);
    memset(&client, 0, sizeof(client));
    while (1) {

        c = accept(s, (struct sockaddr *) &client, (socklen_t *)&len);
        if (c < 0) {
            critical("Could not accept connection");
        }
        sprintf(msg, "Connection accepted by server from %s.", inet_ntoa(client.sin_addr));
        debug(msg);
	
	if (fork() == 0) {
		serve_client(c);
		return 0;
	}
    }
}
