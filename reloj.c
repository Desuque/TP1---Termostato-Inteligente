#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reloj.h"

#define POS_ANIO 1
#define POS_MES 2
#define POS_DIA 3
#define POS_HORA 4
#define POS_MIN 5
#define POS_SEG 6
#define MILISEC 1000
#define TAM_ANIO 5
#define TAM_NOANIO 3

void reloj_create(reloj_t *self, char* timeNow, int timeStep) {
	reloj_cargarDatos(self, timeNow);
	reloj_cargarTimeStep(self, timeStep);
}

void reloj_destroy(reloj_t *self) {
}

/**
 * Devuelve el reloj actual completo sin los segundos actuales, ej: 2016.08.20­-23:59:00
 */
char* reloj_getReloj(reloj_t *self) {
	char buffer[TAM_RELOJ];
	memset(&buffer[0], 0, TAM_RELOJ);
	char anio[TAM_ANIO];
	memset(&anio[0], 0, TAM_ANIO);
	char mes[TAM_NOANIO];
	memset(&mes[0], 0, TAM_NOANIO);
	char dia[TAM_NOANIO];
	memset(&dia[0], 0, TAM_NOANIO);
	char hora[TAM_NOANIO];
	memset(&hora[0], 0, TAM_NOANIO);
	char min[TAM_NOANIO];
	memset(&min[0], 0, TAM_NOANIO);
	snprintf(anio, TAM_ANIO, "%d", self->anio);
	snprintf(mes, TAM_NOANIO, "%02d", self->mes);
	snprintf(dia, TAM_NOANIO, "%02d", self->dia);
	snprintf(hora, TAM_NOANIO, "%02d", self->hora);
	snprintf(min, TAM_NOANIO, "%02d", self->min);
	strncat(buffer, anio, TAM_ANIO);
	strncat(buffer, ".", 1);
	strncat(buffer, mes, TAM_NOANIO);
	strncat(buffer, ".", 1);
	strncat(buffer, dia, TAM_NOANIO);
	strncat(buffer, "-", 1);
	strncat(buffer, hora, TAM_NOANIO);
	strncat(buffer, ":", 1);
	strncat(buffer, min, TAM_NOANIO);
	strncat(buffer, ":", 1);
	strncat(buffer, "00", TAM_NOANIO);
	strncpy(self->clock, buffer, TAM_RELOJ-1);
	return self->clock;
}

void reloj_avanzarStepTime(reloj_t *self){
	int segFinal = self->seg + self->timeStep;
	if(segFinal > 59) {
		self->seg = (segFinal-60);
		int minFinal = self->min + 1;
		if(minFinal > 59) {
			self->min = (minFinal-60);
			int horaFinal = self->hora + 1;
			if(horaFinal > 23) {
				self->hora = (horaFinal-24);
				int diaFinal = self->dia + 1;
				if(diaFinal > 30) {
					self->dia = (diaFinal - 30);
					int mesFinal = self->mes + 1;
					if(mesFinal > 12) {
						self->mes = (mesFinal- 12);
						self->anio = self->anio + 1;
					} else {
						self->mes = mesFinal;
					}
				} else {
					self->dia = diaFinal;
				}
			} else {
				self->hora = horaFinal;
			}
		} else {
			self->min = minFinal;
		}
	} else {
		self->seg = segFinal;
	}
}

void reloj_cargarTimeStep(reloj_t *self, int timeStep) {
	self->timeStep = timeStep/MILISEC;
}

int reloj_getTimeStepEnSegs(reloj_t *self) {
	return self->timeStep;
}

void reloj_cargarDatos(reloj_t *self, char* timeNow) {
	char* save_ptr;
	char* timeParcial = strtok_r(timeNow,".-:", &save_ptr);
	int time = atoi(timeParcial);
	reloj_setAnio(self, time);
	int i=POS_MES;
	while (timeParcial != NULL) {
		timeParcial = strtok_r(NULL, ".-:", &save_ptr);
		if (timeParcial != NULL) {
			time = atoi(timeParcial);
			switch (i) {
				case POS_MES:
					reloj_setMes(self, time);
					i++;
			    break;
				case POS_DIA:
					reloj_setDia(self, time);
					i++;
				break;
				case POS_HORA:
					reloj_setHora(self, time);
					i++;
				break;
				case POS_MIN:
					reloj_setMin(self, time);
					i++;
				break;
				case POS_SEG:
					reloj_setSeg(self, time);
					i++;
				break;
			}
		}
	}
}

void reloj_setAnio(reloj_t *self, int anio) {
	self->anio = anio;
}

void reloj_setMes(reloj_t *self, int mes) {
	self->mes = mes;
}

void reloj_setDia(reloj_t *self, int dia) {
	self->dia = dia;
}

void reloj_setHora(reloj_t *self, int hora) {
	self->hora = hora;
}

void reloj_setMin(reloj_t *self, int min) {
	self->min = min;
}

void reloj_setSeg(reloj_t *self, int seg) {
	self->seg = seg;
}

int reloj_getAnio(reloj_t *self) {
	return self->anio;
}

int reloj_getMes(reloj_t *self) {
	return self->mes;
}

int reloj_getDia(reloj_t *self) {
	return self->dia;
}

int reloj_getHora(reloj_t *self) {
	return self->hora;
}

int reloj_getMin(reloj_t *self) {
	return self->min;
}

int reloj_getSeg(reloj_t *self) {
	return self->seg;
}
