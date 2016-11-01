#include <sys/wait.h>
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

char * get_ip(unsigned int val) {
	char * address = (char *) malloc(sizeof(char) * 16);
	sprintf(
		address, "%d.%d.%d.%d",
		 val & 0x000000FF,
		(val & 0x0000FF00) >> 8,
		(val & 0x00FF0000) >> 16,
		(val & 0xFF000000) >> 24
	);
	
	return address;
}

int readTerminatingZeroArray(int c, uint16_t * numbers) {
	int len = 0;
	uint16_t x;

	do {
		recv(c, &x, sizeof(x), MSG_WAITALL);
		numbers[len++] = ntohs(x);
	} while (x != 0);
	
	return len;
}

int readGivenLengthArray(int c, uint16_t * numbers, int len) {
	recv(c, numbers, len * sizeof(uint16_t), MSG_WAITALL);

	for(int i = 0; i < len; i++) {
		numbers[i] = ntohs(numbers[i]);
	}

	return len;
}

void serve_client(int c) {
    	uint16_t firstNumber;
    	uint16_t numbers[100];
        int realLen = 0;

	recv(c, &firstNumber, sizeof(firstNumber), MSG_WAITALL);
        firstNumber = ntohs(firstNumber);

	printf("Received first number: %d\n", firstNumber);

	if (firstNumber % 2 == 0) {
		realLen = readGivenLengthArray(c, numbers, firstNumber);
	} else {
		numbers[0] = firstNumber;
		realLen = readTerminatingZeroArray(c, numbers + 1);
	}

	uint32_t result = 1;
	for (int i = 0; i < realLen; i++) {
		result *= numbers[i];
	}
	result = htonl(result);


    	send(c, &result, sizeof(result), 0);

	close(c);
}

int main(int argc, char *argv[]) {
	int s, c, len;
    	struct sockaddr_in server, client;

    	s = socket(AF_INET, SOCK_STREAM, 0);
    	if (s < 0) {
        	perror("Could not create the server socket.\n");
        	return 1;
    	}	

    	uint16_t port = 6666;
    	if (argc > 1) {
        	uint16_t newPort = atoi(argv[1]);
        	if (newPort != 0) {
            		port = newPort;
        	}
    	}	

    	memset(&server, 0, sizeof(server));
    	server.sin_port        = htons(port);
    	server.sin_family      = AF_INET;
    	server.sin_addr.s_addr = INADDR_ANY;

    	if (bind(s, (struct sockaddr *) &server, sizeof(server))) {
        	perror("Error when trying to bind: %s");
        	return 1;
    	}
    	printf("Listening for connections on port %d", port);

    	listen(s, 5);

    	len = sizeof(client);
    	memset(&client, 0, sizeof(client));

    	while (1) {

        	c = accept(s, (struct sockaddr *) &client, (socklen_t *)&len);
        	if (c < 0) {
            		printf("Could not accept connection");
        		continue;
		}
        	printf("Connection accepted by server from %s.\n", get_ip(client.sin_addr.s_addr));

        	if (fork() == 0) {
         		serve_client(c);
            		exit(0);
	    	}
    	}
}
