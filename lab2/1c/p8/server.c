#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"

int main(int argc, char **argv) {
	int s;
	struct sockaddr_in server;
	socklen_t l;
	uint16_t numbers[2], result[2];

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		perror("Could not create server socket\n");
		return 1;
	}

	memset(&server, 0, sizeof(server));

	uint16_t port = 6666;
	if (argc > 1) {
		uint16_t newPort = atoi(argv[1]);
		if (newPort != 0) {
			port = newPort;
		}
	}

	server.sin_port = htons(port);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;

	if (bind(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("Error when trying to bind");
		return 1;
	}

	while(1) {
		struct sockaddr_in client;
		l = sizeof(client);
		memset(&client, 0, sizeof(client));

		recvfrom(s, numbers, sizeof(numbers), MSG_WAITALL, (struct sockaddr *) &client, &l);

		numbers[0] = ntohs(numbers[0]);
		numbers[1] = ntohs(numbers[1]);

		printf("Received %d, %d from %s\n", numbers[0], numbers[1], inet_ntoa(client.sin_addr));


		result[0] = htons(gcd(numbers[0], numbers[1]));
		result[1] = htons(lcm(numbers[0], numbers[1]));

		sendto(s, result, sizeof(result), MSG_WAITALL, (struct sockaddr *) &client, l);

		printf("Sent %d, %d, to %s\n", ntohs(result[0]), ntohs(result[1]), inet_ntoa(client.sin_addr));

	}

	close(s);

	return 0;
}
