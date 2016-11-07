#ifndef __RELOJ_INCLUDED__
#define __RELOJ_INCLUDED__

#define TAM_RELOJ 21

typedef struct {
	char clock[TAM_RELOJ];
	int timeStep; //En segundos
	int anio;
	int mes;
	int dia;
	int hora;
	int min;
	int seg;
} reloj_t;

void reloj_create(reloj_t *self, char* timeNow, int timeStep);
void reloj_destroy(reloj_t *self);
char* reloj_getReloj(reloj_t *self);
void reloj_avanzarStepTime(reloj_t *self);
void reloj_cargarTimeStep(reloj_t *self, int timeStep);
int reloj_getTimeStepEnSegs(reloj_t *self);
void reloj_cargarDatos(reloj_t *self, char* timeNow);
void reloj_setAnio(reloj_t *self, int anio);
void reloj_setMes(reloj_t *self, int mes);
void reloj_setDia(reloj_t *self, int dia);
void reloj_setHora(reloj_t *self, int hora);
void reloj_setMin(reloj_t *self, int min);
void reloj_setSeg(reloj_t *self, int seg);
int reloj_getAnio(reloj_t *self);
int reloj_getMes(reloj_t *self);
int reloj_getDia(reloj_t *self);
int reloj_getHora(reloj_t *self);
int reloj_getMin(reloj_t *self);
int reloj_getSeg(reloj_t *self);

#endif
