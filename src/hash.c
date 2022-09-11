#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lista.h"
#include "hash.h"


#define ERROR -1
#define EXITO 0
#define FACTOR_CARGA 0.9
#define CANTIDAD_MINIMA 3

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
	if (capacidad < CANTIDAD_MINIMA) capacidad = CANTIDAD_MINIMA;
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

/*Recibe una clave no nula.

Devuelve el numero que luego se usará para calcular la posicion en
la tabla de hash en la que debe ir el elemento o 0 en caso de error.*/
size_t hash_maker(const char* clave)
{
	if (!clave) return 0;

	size_t numero_magico = 0;
	for (int i = 0; clave[i] != '\0'; i++) {
		numero_magico += (size_t)clave[i] * (size_t)(i);
	}
	return numero_magico;
}

/*Recibe un hash no nulo.

Devuelve 0 (EXITO) en caso de poder insertar los elementos en
el nuevo hash y -1 (ERROR) si falla en alguno de los pasos.*/
int rehash(hash_t* hash)
{
	if (!hash) return ERROR;

	size_t nuevo_tamanio = hash->capacidad * 2;

	hash_t* hash_aux = hash_crear(nuevo_tamanio);
	if (!hash_aux) return ERROR;

	par_t* par_viejo;
	lista_iterador_t* iterador = NULL;

	for (int i = 0; i < hash->capacidad; i++) {
		iterador = lista_iterador_crear(hash->tabla[i]);

		while (lista_iterador_tiene_siguiente(iterador)) {

			par_viejo = lista_iterador_elemento_actual(iterador);

			if (!hash_insertar(hash_aux, par_viejo->clave, par_viejo->elemento, NULL)) {
				lista_iterador_destruir(iterador);
				hash_destruir(hash_aux);
				return ERROR;
			}

			lista_iterador_avanzar(iterador);
		}

		lista_iterador_destruir(iterador);

	}

	hash_t hash_pointer = *hash;
	*hash = *hash_aux;
	*hash_aux = hash_pointer;
	hash_destruir(hash_aux);
	return EXITO;
}


/*Recibe un par_t y un parreemplazo_t no nulos.

Compara si las claves son iguales y en ese caso reemplaza el elemento
en el par_t.

Devuelve false para cortar la iteración si se encontró el par y
en caso de no coincidir las claves sigue devolviendo true.*/
bool hash_reemplazar_duplicados(void* p1, void* p2)
{
	if (!p1 || !p2) return false;

	par_t* par1 = p1;
	parreemplazo_t* par_reemplazo = p2;
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


/*Recibe un par_t y un char* (clave) no nulos.

Compara la clave recibida con la clave del par y devuelve el resultado
de esa comparacion. En caso de recibir nulos devuelve -1 (ERROR).*/
int comparar_claves_par(void* p1, void* p2)
{
	if (!p1 || !p2) return ERROR;

	par_t* par1 = p1;
	char* clave = p2;

	return strcmp(par1->clave, clave);
}


/*Recibe una lista y una clave no nulas.

Devuelve el resultado de buscar la clave en la lista.
En caso de error devuelve NULL.*/
par_t* hash_obtener_par(lista_t* lista, const char* clave)
{
	if (!lista || !clave) return NULL;

	return lista_buscar_elemento(lista, comparar_claves_par, (char*)clave);
}


/*Recibe un hash no nulo.
En caso de error o si no se cumple la condicion de rehasheo,
devuelve false. Caso contrario devuelve true.*/
bool hay_que_rehashear(hash_t* hash)
{
	if (!hash) return false;
	double fraccion_llenado = (double)hash->cantidad_listas / (double)hash->capacidad;
	if (fraccion_llenado >= (double)FACTOR_CARGA) return true;
	else return false;
}

hash_t* hash_insertar(hash_t* hash, const char* clave, void* elemento, void** anterior)
{
	if (!clave || !hash) return NULL;

	if (hay_que_rehashear(hash)) {
		if (rehash(hash) == -1) return NULL;
	}

	par_t* par_nuevo = malloc(sizeof(par_t));
	if (!par_nuevo) return NULL;

	par_nuevo->elemento = elemento;

	par_nuevo->clave = malloc(strlen(clave) + 1);
	if (!par_nuevo->clave) {
		free(par_nuevo);
		return NULL;
	}

	par_nuevo->clave = strcpy(par_nuevo->clave, clave);

	size_t posicion = hash_maker(clave) % hash->capacidad;

	if (!hash->tabla[posicion]) {
		hash->tabla[posicion] = lista_crear();
		hash->cantidad_listas++;
	}

	parreemplazo_t par_reemplazo_nuevo;
	par_reemplazo_nuevo.elemento = par_nuevo;
	par_reemplazo_nuevo.reemplazado = false;

	if (lista_con_cada_elemento(hash->tabla[posicion], hash_reemplazar_duplicados, (void*)&par_reemplazo_nuevo) == hash->tabla[posicion]->cantidad
		&& par_reemplazo_nuevo.reemplazado == false) {
		lista_insertar_en_posicion(hash->tabla[posicion], par_nuevo, 0);
		if (anterior) *anterior = NULL;
		hash->cantidad_elementos++;
	} else {
		if (anterior) *anterior = par_nuevo->elemento;
		free(par_nuevo->clave);
		free(par_nuevo);
	}
	return hash;
}

void* hash_quitar(hash_t* hash, const char* clave)
{
	if (!hash || !clave) return NULL;
	par_t* par_actual;
	void* elemento_encontrado;
	lista_iterador_t* iterador = NULL;
	size_t posicion = hash_maker(clave) % hash->capacidad;
	size_t i;
	if (!lista_vacia(hash->tabla[posicion])) {
		i = 0;
		iterador = lista_iterador_crear(hash->tabla[posicion]);
		while (lista_iterador_tiene_siguiente(iterador)) {
			par_actual = lista_iterador_elemento_actual(iterador);
			if (strcmp(par_actual->clave, clave) == 0) {
				par_actual = lista_quitar_de_posicion(hash->tabla[posicion], i);
				free(par_actual->clave);
				elemento_encontrado = par_actual->elemento;
				free(par_actual);
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

	size_t posicion = hash_maker(clave) % hash->capacidad;
	par_t* par_actual = hash_obtener_par(hash->tabla[posicion], clave);
	if (par_actual) return par_actual->elemento;
	else return NULL;
}

bool hash_contiene(hash_t* hash, const char* clave)
{
	if (!hash || !clave) return false;

	size_t posicion = hash_maker(clave) % hash->capacidad;

	if (hash_obtener_par(hash->tabla[posicion], clave) != NULL) return true;
	else return false;
}

size_t hash_cantidad(hash_t* hash)
{
	if (hash) return hash->cantidad_elementos;
	else return 0;
}

void hash_destruir(hash_t* hash)
{
	hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t* hash, void (*destructor)(void*))
{
	if (!hash) return;
	par_t* dato_aux;
	for (int i = 0; i < hash->capacidad; i++) {
		while (!lista_vacia(hash->tabla[i])) {
			dato_aux = lista_quitar_de_posicion(hash->tabla[i], 0);
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
	if (!hash || !f) return 0;
	size_t cantidad_iterados = 0;

	par_t* par_actual;
	lista_iterador_t* iterador = NULL;
	for (int i = 0; i < hash->capacidad; i++) {
		if (!lista_vacia(hash->tabla[i])) {
			iterador = lista_iterador_crear(hash->tabla[i]);

			while (lista_iterador_tiene_siguiente(iterador)) {
				par_actual = lista_iterador_elemento_actual(iterador);

				if (!f(par_actual->clave, par_actual->elemento, aux)) {
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
