#include "lista.h"
#include <stdio.h>
#include <stdlib.h>

void lista_create(lista_t *self) {
	self->inicio = NULL;
	self->fin = NULL;
	self->tamanio = 0;
}

void lista_destroy(lista_t *self, int tamanio) {
	nodo_t *aux;
	if (self->inicio != NULL) {
		for(int i=0; i<self->tamanio; i++) {
			aux = (self->inicio)->siguiente;
			free(self->inicio);
			self->inicio = aux;
		}
	}
}

void lista_ordenar(lista_t *self) {
	double val;
	nodo_t *actual = self->inicio;
	while(actual->siguiente != NULL) {
		nodo_t *siguiente = actual->siguiente;
		while(siguiente!= NULL) {
			if(actual->valor > siguiente->valor) {
				val = siguiente->valor;
				siguiente->valor = actual->valor;
				actual->valor = val;
			}
			siguiente = siguiente->siguiente;
		}
		actual = actual->siguiente;
		siguiente = actual->siguiente;
	}
}

int lista_getTamanio(lista_t *self) {
	return self->tamanio;
}

double lista_getDato(lista_t *self, int posicion) {
	if(posicion > (self->tamanio)) {
		return -1;
	} else {
		nodo_t *this = self->inicio;
		for(int i=0; i<posicion; i++) {
			this = this->siguiente;
		}
		return this->valor;
	}
}

void lista_insertar(lista_t *self, double valor) {
	nodo_t *nuevo = (nodo_t*)malloc(sizeof(nodo_t));
	nuevo->valor = valor;
	nuevo->siguiente = NULL;
	if(self->inicio == NULL) {
		self->inicio = nuevo;
		self->fin = nuevo;
	} else {
		self->fin->siguiente = nuevo;
		self->fin = nuevo;
	}
	self->tamanio = self->tamanio + 1;
}
