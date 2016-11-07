#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

#define TAM_FECHA 11
#define MAX_SMALL_BUF_LEN 21
#define POS_INICIAL 0
#define TAM_RELOJ 21
#define UNO 1
#define DOS 2

void server_create(server_t *self, unsigned short port) {
	socket_create(&self->skt_server);
	socket_bind_and_listen(&self->skt_server, port);
	lista_create(&self->listaInforme);
	memset(&self->fechaInforme[0], 0, TAM_FECHA);
}

void server_destroy(server_t *self) {
	socket_destroy(&self->skt_server);
}

void server_shutdown(server_t *self) {
	socket_shutdown(&self->skt_server);
}

void server_informarID(char* id) {
	fprintf(stderr, "Recibiendo termostato. ID=%s\n", id);
}

double server_calcularMediana(lista_t* lista) {
	int pos_medio = (lista_getTamanio(lista)-1) / DOS;
	int pos_medioMasUno = ((lista_getTamanio(lista)) + UNO) / DOS;
	if(lista_getTamanio(lista)%2 == 0) {
		double medio = lista_getDato(lista, pos_medio - 1);
		double medio_mas = lista_getDato(lista, pos_medio + UNO - 1);
		return (medio + medio_mas) / DOS;
	} else {
		return (lista_getDato(lista, pos_medioMasUno - 1));
	}
}

void server_mostrarInforme(server_t* self, char** id) {
	lista_ordenar(&self->listaInforme);
	int tamanio = lista_getTamanio(&self->listaInforme);
	double max = lista_getDato(&self->listaInforme, tamanio - 1);
	double min = lista_getDato(&self->listaInforme, POS_INICIAL);
	double mediana = server_calcularMediana(&self->listaInforme);
	fprintf(stdout, "%s %s Max=%0.1f Min=%0.1f Mediana=%0.1f Muestras=%d\n",
			self->fechaInforme, *id, max, min, mediana, tamanio);
	lista_destroy(&self->listaInforme, tamanio);
	lista_create(&self->listaInforme);
}

void server_informarTemp(server_t* self, char* paquete, char** id) {
	char* save_ptr;
	lista_t lista;
	lista_create(&lista);
	double temp;
	char *tmp = strtok_r(paquete," ", &save_ptr);
	char reloj[TAM_RELOJ];
	memset(&reloj[0], 0, TAM_RELOJ);
	strncat(reloj, tmp, strlen(tmp));
	while (tmp != NULL) {
		tmp = strtok_r(NULL, " ", &save_ptr);
		if (tmp != NULL) {
			temp = atof(tmp);
			lista_insertar(&lista, temp);
		}
	}
	int tamanio = lista_getTamanio(&lista);
	fprintf(stderr, "%s - Datos recibidos: %d\n", reloj, tamanio);
	//Informacion para salida std
	char* fecha = strtok_r(reloj, "-", &save_ptr);
	if(strcmp((char*)self->fechaInforme, "\0") == 0) {
		strncpy((char*)self->fechaInforme, fecha, TAM_FECHA);
	}
	if(strcmp(fecha, self->fechaInforme) != 0) {
		server_mostrarInforme(self, id);
		strncpy(self->fechaInforme, fecha, TAM_FECHA);
		for(int i=0; i<tamanio; i++) {
			lista_insertar(&self->listaInforme, lista_getDato(&lista, i));
		}
	} else {
		for(int i=0; i<tamanio; i++) {
			lista_insertar(&self->listaInforme, lista_getDato(&lista, i));
		}
	}
	lista_destroy(&lista, lista_getTamanio(&lista));
}

void server_procesarPaquete(server_t* self, char* paquete, char** id) {
	if(*id == NULL) {
		char* temp = (char*)malloc(strlen(paquete) + 1);
		memset(&temp[0], 0, strlen(paquete) + 1);
		strncpy(temp, paquete, strlen(paquete));
		*id = temp;
		server_informarID(paquete);
	} else {
		server_informarTemp(self, paquete, id);
	}
}

bool server_concatenarStr(char* buffer, char* fin_paquete, char* resto) {
	char* save_ptr;
	memset(&fin_paquete[0], 0, MAX_SMALL_BUF_LEN);
	memset(&resto[0], 0, MAX_SMALL_BUF_LEN);
	char* auxiliar = strstr(buffer, "\n");
	if(auxiliar != NULL) {
		if(buffer[0] != '\n') {
			char *tmp = strtok_r(buffer, "\n", &save_ptr);
			strncat(fin_paquete, tmp, strlen(tmp));
			tmp = strtok_r(NULL, "", &save_ptr);
			if(tmp != NULL) {
				strncat(resto, tmp, strlen(tmp));
			}
		} else {
			char *tmp = strtok_r(buffer,"\n", &save_ptr);
			strncat(resto, tmp, strlen(tmp));
		}
		return true;
	} else {
		strncat(resto, buffer, MAX_SMALL_BUF_LEN);
		memset(&buffer[0], 0, MAX_SMALL_BUF_LEN);
		memset(&fin_paquete[0], 0, MAX_SMALL_BUF_LEN);
		return false;
	}
}

void server_procesarInformacion(server_t *self) {
	bool continue_running = true;
	int peerskt = 0;
	char buf_fin_paquete[MAX_SMALL_BUF_LEN];
	char buf_resto[MAX_SMALL_BUF_LEN];
	char* resto_aux = (char*)malloc((sizeof(char) * MAX_SMALL_BUF_LEN) + 1);
	char* id = NULL;
	memset(&resto_aux[0], 0, MAX_SMALL_BUF_LEN);
	char buffer[MAX_SMALL_BUF_LEN];
	peerskt = socket_accept(&self->skt_server, NULL);
	int flag = 0;
	while (continue_running) {
		if (peerskt == -1) {
			printf("Error\n");
			continue_running = false;
		} else {
			memset(buffer, 0, MAX_SMALL_BUF_LEN);
			flag = socket_receive(&self->skt_server, buffer, MAX_SMALL_BUF_LEN-1);
			if (server_concatenarStr(buffer, buf_fin_paquete, buf_resto) == true) {
				size_t length = strlen(buf_fin_paquete) + strlen(resto_aux);
				char* paquete = (char*)malloc((sizeof(char) * length) + 1);
				memset(&paquete[0], 0, length + 1);
				strncat(paquete, resto_aux, strlen(resto_aux));
				strncat(paquete, buf_fin_paquete, strlen(buf_fin_paquete));
				memset(&resto_aux[0], 0, strlen(resto_aux));
				char* auxiliar = strstr(paquete, "\n");
				if(auxiliar == NULL) {
					strncat(resto_aux, buf_resto, strlen(buf_resto));
					server_procesarPaquete(self, paquete, &id);
				}
				free(paquete);
			} else {
				/**
				 * Aumento el tamaño del resto sin perder
				 * la informacion anterior
				 **/
				size_t length = strlen(buf_resto) + strlen(resto_aux);
				char* auxiliar = (char*)malloc((sizeof(char)*length) + 1);
				memset(&auxiliar[0], 0, length + 1);
				strncat(auxiliar, resto_aux, strlen(resto_aux));
				strncat(auxiliar, buf_resto, strlen(buf_resto));
				free(resto_aux);
				resto_aux = (char*)malloc((sizeof(char)*length) + 1);
				memset(&resto_aux[0], 0, length + 1);
				strncat(resto_aux, auxiliar, strlen(auxiliar));
				free(auxiliar);
			}
			if (flag == -1) {
				fprintf(stderr, "Termostato desconectado. ID=%s\n", id);
				continue_running = false;
			}
		}
	}
	if(lista_getTamanio(&self->listaInforme) != 0) {
			server_mostrarInforme(self, &id);
	}
	free(resto_aux);
	if (id != NULL) {
		free(id);
	}
}
