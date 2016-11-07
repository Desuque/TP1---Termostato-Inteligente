#ifndef __CLIENT_INCLUDED__
#define __CLIENT_INCLUDED__

#include "socket.h"
#include "reloj.h"

typedef struct {
	socket_t skt_client;
	reloj_t reloj;
	double tempAnterior;
} client_t;

void client_create(client_t *self, char* host_name, unsigned short port);
void client_destroy(client_t *self);
void client_shutdown(client_t *self);
void client_procesarInformacion(client_t *self, char* idTermostato,
		int timeStep, char* timeNow, char* file);
int client_calcularEnvios(client_t *self, int segActual);
double client_validarTemperatura(client_t *self, double temperatura);
int client_procesarFichero(client_t *self, const char* file);
void client_enviarID(client_t *self, char* idTermostato);
char* client_stradd(char* a, char* b);
void client_send(client_t *self, char* mensaje, int num);

#endif
