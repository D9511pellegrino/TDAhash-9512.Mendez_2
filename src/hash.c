#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "lista.h"

#define ERROR -1
#define EXITO 0
#define FACTOR_CARGA 0.60
#define MAX_ELEM_LISTA 8

typedef struct par_hash
{
	char* clave;
	void* elemento;
} par_t;

struct hash
{
	size_t cantidad_elementos;
	size_t cantidad_listas;
	size_t capacidad;
	lista_t** tabla;
};

typedef struct par_reemplazo
{
	bool reemplazado;
	par_t* elemento;
} parreemplazo_t;

typedef struct par_contiene
{
	bool contiene;
	char* clave;
} parcontiene_t;

hash_t* hash_crear(size_t capacidad)
{
	if (capacidad < 3) capacidad = 3;
	hash_t* nuevo_hash = malloc(sizeof(hash_t));
	if (!nuevo_hash) return NULL;

	nuevo_hash->tabla = calloc(capacidad, sizeof(lista_t*));
	if (!nuevo_hash->tabla) {
		free(nuevo_hash);
		return NULL;
	}

	nuevo_hash->cantidad_elementos = 0;
	nuevo_hash->cantidad_listas = 0;

	nuevo_hash->capacidad = capacidad;

	return nuevo_hash;
}

size_t hash_maker(const char* clave)
{
	if (!clave) return 0;

	size_t numero_magico = 0;
	for (int i = 0; clave[i] != '\0'; i++) {
		numero_magico += (size_t)clave[i] * (size_t)(i + 1);
	}
	return numero_magico;

}

int rehash(hash_t* hash)
{
	if (!hash) return ERROR;

	hash_t* hash_aux = hash_crear(hash->capacidad + hash->capacidad / 2);
	if (!hash_aux) return ERROR;

	par_t* par_aux;
	char* clave;
	size_t posicion = 0;
	lista_iterador_t* iterador = NULL;

	for (int i = 0; i < hash->capacidad; i++) {
		if (!lista_vacia(hash->tabla[i])) {
			iterador = lista_iterador_crear(hash->tabla[i]);

			while (lista_iterador_tiene_siguiente(iterador)) {
				par_aux = lista_iterador_elemento_actual(iterador);
				clave = par_aux->clave;
				posicion = hash_maker(clave) % hash_aux->capacidad;
				if (lista_insertar(hash_aux->tabla[posicion], par_aux) == NULL) {
					lista_iterador_destruir(iterador);
					hash_destruir(hash_aux);
					return ERROR;
				}
				lista_iterador_avanzar(iterador);
			}

			lista_iterador_destruir(iterador);
		}
	}

	hash_t hash_pointer = *hash;
	*hash = *hash_aux;
	*hash_aux = hash_pointer;
	hash_destruir(hash_aux);
	return EXITO;
}

bool hash_reemplazar_duplicados(void* elemento1, void* elemento2)
{
	if (!elemento1 || !elemento2) return false;

	par_t* par1 = elemento1;
	parreemplazo_t* par_reemplazo = elemento2;
	par_t* par2 = par_reemplazo->elemento;
	void* aux;

	if (strcmp(par1->clave, par2->clave) == 0) {
		aux = par1->elemento;
		par1->elemento = par2->elemento;
		par2->elemento = aux;
		par_reemplazo->reemplazado = true;
		return false;
	}
	return true;
}

bool hash_obtener_con_lista(void* elemento1, void* elemento2)
{
	if (!elemento1 || !elemento2) return false;

	par_t* par1 = elemento1;
	par_t* par2 = elemento2;

	if (strcmp(par1->clave, par2->clave) == 0) {
		par2->elemento = par1->elemento;
		return false;
	}
	return true;
}

bool hash_contiene_con_lista(void* elemento1, void* elemento2)
{
	if (!elemento1 || !elemento2) return false;

	par_t* par1 = elemento1;
	parcontiene_t* par_contiene = elemento2;
	char* clave = par_contiene->clave;

	if (strcmp(par1->clave, clave) == 0) {
		par_contiene->contiene = true;
		return false;
	} else return true;
}

bool hay_que_rehashear(hash_t* hash)
{
	if (!hash) return false;
	double fraccion_llenado = 0;
	for (int i = 0; i < hash->capacidad; i++) {
		if (!lista_vacia(hash->tabla[i])) {
			fraccion_llenado = (double)hash->tabla[i]->cantidad / (double)MAX_ELEM_LISTA;
			if (fraccion_llenado > FACTOR_CARGA) return true;
		}
	}
	return false;
}

hash_t* hash_insertar(hash_t* hash, const char* clave, void* elemento, void** anterior)
{
	if (!clave || !hash) return NULL;

	if (hay_que_rehashear(hash)) rehash(hash);

	par_t* elemento_nuevo_hash = malloc(sizeof(par_t));
	if (!elemento_nuevo_hash) return NULL;

	elemento_nuevo_hash->elemento = elemento;

	elemento_nuevo_hash->clave = malloc(strlen(clave) + 1);
	if (!elemento_nuevo_hash->clave) {
		free(elemento_nuevo_hash);
		return NULL;
	}

	elemento_nuevo_hash->clave = strcpy(elemento_nuevo_hash->clave, clave);

	size_t posicion = hash_maker(clave) % hash->capacidad;

	if (!hash->tabla[posicion]) {
		hash->tabla[posicion] = lista_crear();
		hash->cantidad_listas++;
	}

	parreemplazo_t* par_reemplazo_nuevo = malloc(sizeof(parreemplazo_t));
	if (!par_reemplazo_nuevo) {
		free(elemento_nuevo_hash->clave);
		free(elemento_nuevo_hash);
		return NULL;
	}
	par_reemplazo_nuevo->elemento = elemento_nuevo_hash;
	par_reemplazo_nuevo->reemplazado = false;

	if (lista_con_cada_elemento(hash->tabla[posicion], hash_reemplazar_duplicados, (void*)par_reemplazo_nuevo) == hash->tabla[posicion]->cantidad
		&& par_reemplazo_nuevo->reemplazado == false) {
		lista_insertar(hash->tabla[posicion], elemento_nuevo_hash);
		if (anterior) *anterior = NULL;
		hash->cantidad_elementos++;
	} else {
		if (anterior) *anterior = elemento_nuevo_hash->elemento;
		free(elemento_nuevo_hash->clave);
		free(elemento_nuevo_hash);
	}

	free(par_reemplazo_nuevo);

	return hash;
}

void* hash_quitar(hash_t* hash, const char* clave)
{
	if (!hash || !clave) return NULL;
	par_t* par_aux;
	void* elemento_encontrado;
	lista_iterador_t* iterador = NULL;
	size_t posicion = hash_maker(clave) % hash->capacidad;
	size_t i;
	if (!lista_vacia(hash->tabla[posicion])) {
		i = 0;
		iterador = lista_iterador_crear(hash->tabla[posicion]);
		while (lista_iterador_tiene_siguiente(iterador)) {
			par_aux = lista_iterador_elemento_actual(iterador);
			if (strcmp(par_aux->clave, clave) == 0) {
				par_aux = lista_quitar_de_posicion(hash->tabla[posicion], i);
				free(par_aux->clave);
				elemento_encontrado = par_aux->elemento;
				free(par_aux);
				lista_iterador_destruir(iterador);
				hash->cantidad_elementos--;
				return elemento_encontrado;
			}
			lista_iterador_avanzar(iterador);
			i++;
		}
		lista_iterador_destruir(iterador);
	}
	return NULL;
}

void* hash_obtener(hash_t* hash, const char* clave)
{
	if (!hash || !clave) return NULL;
	par_t* par_aux = calloc(1, sizeof(par_t));
	if (!par_aux) return NULL;

	par_aux->clave = malloc(strlen(clave) + 1);
	if (!par_aux->clave) {
		free(par_aux);
		return NULL;
	}
	par_aux->clave = strcpy(par_aux->clave, clave);
	void* aux;

	size_t posicion = hash_maker(clave) % hash->capacidad;

	if (!lista_vacia(hash->tabla[posicion])) {
		lista_con_cada_elemento(hash->tabla[posicion], hash_obtener_con_lista, (void*)par_aux);
		aux = par_aux->elemento;
		free(par_aux->clave);
		free(par_aux);
		return aux;
	}
	free(par_aux->clave);
	free(par_aux);
	return NULL;
}

bool hash_contiene(hash_t* hash, const char* clave)
{
	if (!hash || !clave) return false;

	size_t posicion = hash_maker(clave) % hash->capacidad;

	parcontiene_t* par_contiene = malloc(sizeof(parcontiene_t));
	if (!par_contiene) return false;

	par_contiene->clave = strcpy(par_contiene->clave, clave);;
	par_contiene->contiene = false;

	if (!lista_vacia(hash->tabla[posicion])) {
		if (lista_con_cada_elemento(hash->tabla[posicion], hash_contiene_con_lista, (void*)clave) == hash->tabla[posicion]->cantidad
			&& par_contiene->contiene == false)
			return false;
		else return true;
	}
	return false;
}

size_t hash_cantidad(hash_t* hash)
{
	if (hash) return hash->cantidad_elementos;
	else return 0;
}

void hash_destruir(hash_t* hash)
{
	if (!hash) return;
	par_t* dato_aux;
	for (int i = 0; i < hash->capacidad; i++) {
		while (!lista_vacia(hash->tabla[i])) {
			dato_aux = lista_quitar(hash->tabla[i]);
			free(dato_aux->clave);
			free(dato_aux);
		}
		lista_destruir(hash->tabla[i]);
	}
	free(hash->tabla);
	free(hash);

}

void hash_destruir_todo(hash_t* hash, void (*destructor)(void*))
{
	if (!hash) return;
	par_t* dato_aux;
	for (int i = 0; i < hash->capacidad; i++) {
		while (!lista_vacia(hash->tabla[i])) {
			dato_aux = lista_quitar(hash->tabla[i]);
			free(dato_aux->clave);
			if (destructor) destructor(dato_aux->elemento);
			free(dato_aux);
		}
		lista_destruir(hash->tabla[i]);
	}
	free(hash->tabla);
	free(hash);
}

size_t hash_con_cada_clave(hash_t* hash, bool (*f)(const char* clave, void* valor, void* aux), void* aux)
{
	size_t cantidad_iterados = 0;
	if (!hash) return 0;
	par_t* par_aux;
	lista_iterador_t* iterador = NULL;
	for (int i = 0; i < hash->capacidad; i++) {
		if (!lista_vacia(hash->tabla[i])) {
			iterador = lista_iterador_crear(hash->tabla[i]);

			while (lista_iterador_tiene_siguiente(iterador)) {
				par_aux = lista_iterador_elemento_actual(iterador);

				if (!f(par_aux->clave, par_aux->elemento, aux)) {
					lista_iterador_destruir(iterador);
					return cantidad_iterados + 1;
				}
				lista_iterador_avanzar(iterador);
				cantidad_iterados++;
			}
			lista_iterador_destruir(iterador);
		}
	}
	return cantidad_iterados;
}
