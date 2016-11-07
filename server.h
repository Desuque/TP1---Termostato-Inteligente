#ifndef __SERVER_INCLUDED__
#define __SERVER_INCLUDED__

#include "socket.h"
#include "lista.h"
#include <stdbool.h>

#define TAM_FECHA 11

typedef struct {
	socket_t skt_server;
	lista_t listaInforme;
	char fechaInforme[TAM_FECHA];
} server_t;

void server_create(server_t *self, unsigned short port);
void server_destroy(server_t *self);
void server_shutdown(server_t *self);
char* server_recibirDatos(server_t *self);
void server_procesarInformacion(server_t *self);
void server_informarID(char* id);
void server_informarTemp(server_t* self, char* paquete, char** id);
void server_procesarPaquete(server_t* self, char* paquete, char** id);
bool server_concatenarStr(char* buffer, char* paquete, char* resto);
#endif
