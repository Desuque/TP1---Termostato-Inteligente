#include "client.h"
#include <math.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define CALIB_SENSOR 10.0
#define TEMP_MIN -17.0
#define TEMP_MAX 59.7
#define TEMP_CERO 0.0
#define TEMP_INVALIDA 9999
#define SEGS_EN_UN_MIN 60.0
#define SEG_CERO 0
#define DOS 2

void client_create(client_t *self, char* host_name, unsigned short port) {
	socket_create(&self->skt_client);
	socket_connect(&self->skt_client, (const char*)host_name, port);
	self->tempAnterior = TEMP_INVALIDA;
}

void client_destroy(client_t *self) {
	socket_destroy(&self->skt_client);
	reloj_destroy(&self->reloj);
}

void client_shutdown(client_t *self) {
	socket_shutdown(&self->skt_client);
}

void client_procesarInformacion(client_t *self, char* idTermostato,
		int timeStep, char* timeNow, char* file) {
	reloj_create(&self->reloj, timeNow, timeStep);
	client_enviarID(self, idTermostato);
	client_procesarFichero(self, (const char*)file);
}

int client_calcularEnvios(client_t *self, int segActual) {
	int timeStep = reloj_getTimeStepEnSegs(&self->reloj);
	double resultadoReal = (SEGS_EN_UN_MIN-segActual) / timeStep;
	int resultado = (SEGS_EN_UN_MIN-segActual) / timeStep;
	double fract;
	double fractpart = modf(resultadoReal, &fract);
	if (fractpart > 0.0) {
		return resultado+1;
	}
	return resultado;
}

double client_validarTemperatura(client_t *self, double temperatura) {
	if(temperatura < TEMP_MIN || temperatura > TEMP_MAX) {
		if(self->tempAnterior == TEMP_INVALIDA) {
			self->tempAnterior = TEMP_CERO;
		}
		return self->tempAnterior;
	} else {
		self->tempAnterior = temperatura;
	}
	return temperatura;
}

int client_procesarFichero(client_t *self, const char* nameFile) {
	FILE* file = fopen(nameFile, "rb");
	if (file == NULL) {
		perror("Error al abrir el archivo.");
		return -1;
	}
	int cantidadDeEnvios = client_calcularEnvios(self, reloj_getSeg(&self->reloj));
	int cantidadDeEnviados = 0;
	char* tmpReloj = 0;
	char* tmpSend = 0;
	short s;
	bool finDeArchivo = false;
	while(fread(&s, sizeof(short), 1, file) != 0) {
		s = ntohs(s);
		double tempLeida = (s/CALIB_SENSOR) + TEMP_MIN;
		double tempReal = client_validarTemperatura(self, tempLeida);
		char* temperatura = (char*)malloc(sizeof(double) + 1);
		memset(&temperatura[0], 0, sizeof(double) + 1);
		snprintf(temperatura, sizeof(double), "%0.1lf", tempReal);
		tmpSend = client_stradd(temperatura, " ");
		if(cantidadDeEnviados < (cantidadDeEnvios-1)) {
			if(cantidadDeEnviados == 0) {
				tmpReloj = client_stradd(reloj_getReloj(&self->reloj), " ");
				client_send(self, tmpReloj, strlen(tmpReloj));
			}
			client_send(self, tmpSend, strlen(tmpSend));
			reloj_avanzarStepTime(&self->reloj);
			finDeArchivo = false;
			cantidadDeEnviados++;
		} else {
			char* temperaturaFinal = client_stradd(tmpSend, "\n");
			client_send(self, temperaturaFinal, strlen(temperaturaFinal));
			free(temperaturaFinal);
			reloj_avanzarStepTime(&self->reloj);
			fprintf(stderr, "%s- Enviando %d muestras\n",
					tmpReloj, cantidadDeEnviados + 1);
			if (tmpReloj != NULL) {
				free(tmpReloj);
				finDeArchivo = true;
			}
			cantidadDeEnviados = 0;
			cantidadDeEnvios = client_calcularEnvios(self, SEG_CERO);
		}
		free(tmpSend);
		free(temperatura);
	}
	fclose(file);
	if (finDeArchivo != true) {
		char* fin = client_stradd(" ", "\n");
		client_send(self, fin, strlen(fin));
		fprintf(stderr, "%s - Enviando %i muestras\n",
				reloj_getReloj(&self->reloj), cantidadDeEnviados);
		free(fin);
		free(tmpReloj);
	}
	return 0;
}

void client_enviarID(client_t *self, char* idTermostato) {
	char* id = client_stradd(idTermostato, "\n");
	client_send(self, id, strlen(id));
	free(id);
}

char* client_stradd(char* a, char* b){
	size_t length = strlen(a) + strlen(b);
	char *concat = (char*)malloc((sizeof(char) * length) + 1);
	memset(&concat[0], 0, length + 1);
	return strncat(strncat(concat, a, strlen(a)) , b, strlen(b));
}

void client_send(client_t *self, char* mensaje, int length) {
	socket_send(&self->skt_client, mensaje, length);
}
