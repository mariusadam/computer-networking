#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "util.h"

int main(int argc, char **argv) {
	
	uint16_t port = 6666;
	char* serverIp = (char *) malloc(sizeof(char) * 10);
	strcpy(serverIp, "127.0.0.1");

	if (argc > 1) {
		free(serverIp);
		serverIp = (char *) malloc(sizeof(char) * (strlen(argv[1]) + 1));
		strcpy(serverIp, argv[1]);
		
		if (argc > 2) {
			uint16_t newPort = atoi(argv[2]);
			if (newPort > 0) {
				port = newPort;
			}
		}
	}

	int c = socket(AF_INET, SOCK_DGRAM, 0);
	if (c < 0) {
		perror("Error creating client socket.\n");
		return 1;
	}

	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_port = htons(port);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr  = inet_addr(serverIp);


	uint16_t numbers[2], result[2];

	printf("Insert the first number: ");
	scanf("%d", (int *) numbers);

	printf("Insert the second number: ");
	scanf("%d", (int *) (numbers + 1));

	numbers[0] = htons(numbers[0]);
	numbers[1] = htons(numbers[1]);

	socklen_t len = sizeof(server);

	sendto(c, numbers, sizeof(numbers), 0, (struct sockaddr *) &server, len);

	printf("Sent %d, %d to %s\n", ntohs(numbers[0]), ntohs(numbers[1]), inet_ntoa(server.sin_addr));

	recvfrom(c, result, sizeof(result), MSG_WAITALL, (struct sockaddr *) &server, &len);

	result[0] = ntohs(result[0]);
	result[1] = ntohs(result[1]);
	numbers[0] = ntohs(numbers[0]);
	numbers[1] = ntohs(numbers[1]);

	printf("Received %d, %d from %s\n", result[0], result[1], inet_ntoa(server.sin_addr));
	printf("Correct: %d, %d\n", gcd(numbers[0], numbers[1]), lcm(numbers[0], numbers[1]));
	return main(argc, argv);
}
