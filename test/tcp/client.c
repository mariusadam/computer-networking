#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <stdlib.h>

void handleFirstOption(int c) {

	uint16_t numbers[100];
	int x, len = 0;
	
	do {
		printf("Enter element %d: ", len);
		scanf("%d", &x);
		numbers[len++] = htons(x);
	} while (x != 0);

	send(c, numbers, len * sizeof(uint16_t), 0);
}

void handleSecondOption(int c) {
	uint16_t numbers[100];
	int len;

	printf("Enter the length: ");
	scanf("%d", &len);
	
	for(int i = 0; i < len; i++) {
		printf("Enter element %d : ", i);
		scanf("%d", (int *) (numbers + i));
		numbers[i] = htons(numbers[i]);
	}
	printf("Read %d numbers\n", len);
	
	uint16_t shortLen = htons(len);

	send(c, &shortLen, sizeof(uint16_t), 0);
	send(c, numbers, len * sizeof(uint16_t), 0);
}

int main(int argc, char* argv[]) {
    	int c;
    	struct sockaddr_in server;

    	uint16_t port = 6666;
    	char serverIp[] = "127.0.0.1";
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
        	printf("Error creating client socket");
		return -1;
    	}

    	memset(&server, 0, sizeof(server));
    	server.sin_port        = htons(port);
    	server.sin_family      = AF_INET;
    	server.sin_addr.s_addr = inet_addr(serverIp);

    	if (connect(c, (struct sockaddr *) &server, sizeof(server)) < 0) {
        	printf("Could not connect to %s", serverIp);
    		return -1;
	}


	printf("How do you want to send the array? \n");
	printf("1 : Terminating with 0\n");
	printf("2 : Giving the number of elements first\n");

	int option;
	scanf("%d", &option);

	if (option == 1) {
		handleFirstOption(c);
	} else {
		handleSecondOption(c);
	}
	
	uint32_t result;

    	recv(c, &result, sizeof(result), MSG_WAITALL);
	printf("The result is %d\n", htonl(result));

    	close(c);

    	return 0;
}
