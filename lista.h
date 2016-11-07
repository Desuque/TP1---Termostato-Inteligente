#ifndef __LISTA_INCLUDED__
#define __LISTA_INCLUDED__

typedef struct nodo {
	double valor;
	struct nodo* siguiente;
} nodo_t;

typedef struct lista {
	nodo_t *inicio;
	nodo_t *fin;
	int tamanio;
} lista_t;

void lista_create(lista_t *self);
void lista_destroy(lista_t *self, int tamanio);
int lista_getTamanio(lista_t *self);
double lista_getDato(lista_t *self, int posicion);
void lista_insertar(lista_t *self, double valor);
void lista_ordenar(lista_t *self);

#endif
