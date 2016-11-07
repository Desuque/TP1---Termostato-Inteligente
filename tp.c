#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "server.h"
#include "client.h"

#define MIN_ARG 1
#define MAX_ARG 9
#define ERROR 2

int main(int argc, char *argv[]) {
	if (argc > MIN_ARG && argc < MAX_ARG) {
		if (strcmp(argv[1], "server") == 0) {
			int port = strtol(argv[2], NULL, 10);
			server_t server;
			server_create(&server, (unsigned short)port);
			server_procesarInformacion(&server);
			server_shutdown(&server);
			server_destroy(&server);
		} else if (strcmp(argv[1], "client") == 0) {
			int port = strtol(argv[3], NULL, 10);
			int timeStep = strtol(argv[5], NULL, 10);
			client_t client;
			client_create(&client, argv[2], (unsigned short)port);
			client_procesarInformacion(&client, argv[4], timeStep, argv[6], argv[7]);
			client_shutdown(&client);
			client_destroy(&client);
		} else {
			return ERROR;
		}
	} else {
		return ERROR;
	}
	return 0;
}
