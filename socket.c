#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "socket.h"

#define MAX_CLIENTES 1
#define ERR_COD 1

void socket_create(socket_t *self) {
	self->socket = socket(AF_INET, SOCK_STREAM, 0);
}

void socket_destroy(socket_t *self) {
}

int socket_bind_and_listen(socket_t *self, unsigned short port) {
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;
	memset(address.sin_zero, 0, sizeof(address.sin_zero));
	if (bind(self->socket, (struct sockaddr*) &address,
			sizeof(struct sockaddr_in)) == -1) {
		return ERR_COD;
	}
	if (listen(self->socket, MAX_CLIENTES) == -1) {
		return ERR_COD;
	}
	return 0;
}

int socket_connect(socket_t *self, const char* host_name,
		unsigned short port) {
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = inet_addr(host_name);
	memset(address.sin_zero, 0, sizeof(address.sin_zero));
	int connected = connect(self->socket, (struct sockaddr*)
			&address, sizeof(struct sockaddr_in));
	if (connected != 0) {
		return connected;
	}
	return 0;
}

int socket_accept(socket_t *self, socket_t* accepted_socket) {
	self->socket = accept(self->socket, NULL, NULL);
	if (self->socket == -1) {
		return ERR_COD;
	}
	return self->socket;
}

int socket_send(socket_t *self, char* buffer, size_t length) {
	int sent = 0;
	int s = 0;
	bool is_the_socket_valid = true;
	while (sent < length && is_the_socket_valid) {
		s = send(self->socket, &buffer[sent], length-sent, MSG_NOSIGNAL);
		if (s == 0) {
			is_the_socket_valid = false;
		} else if (s < 0) {
			is_the_socket_valid = false;
		} else {
			sent += s;
		}
	}
	if (is_the_socket_valid) {
		return sent;
	} else {
		return -1;
	}
}

int socket_receive(socket_t *self, char* buffer, size_t length) {
	int received = 0;
	int s = 0;
	bool is_the_socket_valid = true;
	while (received < length && is_the_socket_valid) {
		s = recv(self->socket, &buffer[received], length-received, MSG_NOSIGNAL);
		if (s == 0) { // Socket cerrado
			is_the_socket_valid = false;
		} else if (s < 0) { // Error
			is_the_socket_valid = false;
		} else {
			received = received + s;
		}
	}
	if (is_the_socket_valid) {
		return received;
	} else {
		return -1;
	}
}

void socket_shutdown(socket_t *self) {
	shutdown(self->socket, SHUT_RDWR);
	close(self->socket);
}
