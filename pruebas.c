#include <string.h>
#include <stdlib.h>

#include "pa2m.h"
#include "src/hash.h"

typedef struct consola {
    char* modelo;
    char* descripcion;
} consola_t;


bool mostrar_consola(const char* clave, void* valor, void* aux){
    if(!valor || !clave) return false;
    char* descripcion = valor;
    printf("Clave: %s = %s \n", clave, descripcion);
    return true;
}

void destruir_string(void* string){
    if(string)
        //char* string_destruir = string;
        free(string);
}

bool iterar_hasta_2(const char* clave, void* valor, void* aux){
    if(!valor || !clave) return false;
    size_t* cantidad = aux;
    *cantidad += 1;
    if(*cantidad==2) return false;
    return true;
}

void puedoCrearUnHash(){
    hash_t* h = NULL;

    pa2m_afirmar((h=hash_crear(5))!=NULL, "Crear un hash con tamaño 5 devuelve un hash");
    pa2m_afirmar(hash_cantidad(h)==0, "Hash tiene 0 elementos");

    hash_destruir(h);

    pa2m_afirmar((h=hash_crear(5))!=NULL, "Crear un hash con tamaño 100 devuelve un hash");
    pa2m_afirmar(hash_cantidad(h)==0, "Hash tiene 0 elementos");

    hash_destruir(h);

    pa2m_afirmar((h=hash_crear(2))!=NULL, "Crear un hash con tamaño 2 devuelve un hash");
    pa2m_afirmar(hash_cantidad(h)==0, "Hash tiene 0 elementos");

    hash_destruir(h);
}

void puedoInsertarHash(){
    hash_t* h = NULL;
    h=hash_crear(5);

    pa2m_afirmar(hash_insertar(h, "1002", "Premium 10-Input 2-Bus Mixer", NULL)!=NULL, "Inserto clave 1002 exitosamente");
    pa2m_afirmar(hash_cantidad(h)==1, "Hash tiene 1 elemento");
    pa2m_afirmar(strcmp(hash_obtener(h, "1002"), "Premium 10-Input 2-Bus Mixer")==0, "Busco y encuentro clave 1002");

    pa2m_afirmar(hash_insertar(h, "1002", "Premium 10-Input 2-Bus Mixer USB", NULL)!=NULL, "Inserto clave 1002 repetida exitosamente");
    pa2m_afirmar(hash_cantidad(h)==1, "Hash tiene 1 elemento");
    pa2m_afirmar(strcmp(hash_obtener(h, "1002"), "Premium 10-Input 2-Bus Mixer USB")==0, "Busco y encuentro clave 1002");

    pa2m_afirmar(hash_insertar(h, "1204USB", "Premium 12-Input 2/2-Bus Mixer", NULL)!=NULL, "Inserto clave 1204USB exitosamente");
    pa2m_afirmar(hash_cantidad(h)==2, "Hash tiene 2 elementos");

    pa2m_afirmar(hash_insertar(h, "302USB", "Premium 5-Input Mixer", NULL)!=NULL, "Inserto clave 302USB exitosamente");
    pa2m_afirmar(hash_cantidad(h)==3, "Hash tiene 3 elementos");

    pa2m_afirmar(hash_insertar(h, "502", "Premium 5-Input 2-Bus Mixer", NULL)!=NULL, "Inserto clave 502 exitosamente");
    pa2m_afirmar(hash_cantidad(h)==4, "Hash tiene 4 elementos");

    pa2m_afirmar(hash_insertar(h, "802", "Premium 8-Input 2-Bus Mixer", NULL)!=NULL, "Inserto clave 802 exitosamente");
    pa2m_afirmar(hash_cantidad(h)==5, "Hash tiene 5 elementos");

    pa2m_afirmar(hash_insertar(h, "FLOW 8", "8-Input Digital Mixer", NULL)!=NULL, "Inserto clave FLOW 8 exitosamente");
    pa2m_afirmar(hash_cantidad(h)==6, "Hash tiene 6 elementos");

    pa2m_afirmar(strcmp(hash_obtener(h, "502"), "Premium 5-Input 2-Bus Mixer")==0, "Busco y encuentro clave 502");

    pa2m_afirmar(strcmp(hash_obtener(h, "1204USB"), "Premium 12-Input 2/2-Bus Mixer")==0, "Busco y encuentro clave 1204USB");

    pa2m_afirmar(strcmp(hash_obtener(h, "502"), "Premium 5-Input 2-Bus Mixer")==0, "Busco y encuentro clave 502");

    pa2m_afirmar(strcmp(hash_obtener(h, "302USB"), "Premium 5-Input Mixer")==0, "Busco y encuentro clave 302USB");

    pa2m_afirmar(strcmp(hash_obtener(h, "FLOW 8"), "8-Input Digital Mixer")==0, "Busco y encuentro clave FLOW 8");

    pa2m_afirmar(strcmp(hash_obtener(h, "802"), "Premium 8-Input 2-Bus Mixer")==0, "Busco y encuentro clave 802");
    
    pa2m_afirmar(!hash_obtener(h, "2000"), "Busco clave 2000 inexistente y encuentro NULL");

    hash_destruir(h);
}

void insercionesDuplicadas_HASH(){

}

void insercionesMUCHAS_HASH(){
	hash_t* h = NULL;
	h=hash_crear(5);
	bool insercion_correcta = true;
	char clave[10];
	for(int i = 0; insercion_correcta && i<10000; i++){
		sprintf(clave, "%d", i);;
		if(hash_insertar(h, clave, "Prueba", NULL)==NULL){
			insercion_correcta = false;
		}
	}

    pa2m_afirmar(insercion_correcta, "inserto 10000 claves ok");

    hash_destruir(h);
	
}


void puedoBorrarHAsh(){
    hash_t* h = NULL;
    h=hash_crear(5);

    pa2m_afirmar(hash_quitar(h, "1002")==NULL, "Intento quitar clave de hash vacio devuelve NULL");

    char** consolas = malloc(4*sizeof(char*));
    if(!consolas){
        hash_destruir(h);
        return;
    }

    consolas[0] = malloc(strlen("Premium 10-Input 2-Bus Mixer")+1);
    if(!consolas[0]){
        free(consolas);
        hash_destruir(h);
        return;
    }
    strcpy(consolas[0], "Premium 10-Input 2-Bus Mixer");

    consolas[1] = malloc(strlen("Premium 12-Input 2/2-Bus Mixer")+1);
    if(!consolas[1]){
        free(consolas[0]);
        free(consolas);
        hash_destruir(h);
        return;
    }
    strcpy(consolas[1], "Premium 12-Input 2/2-Bus Mixer");

    consolas[2] = malloc(strlen("Premium 5-Input Mixer")+1);
    if(!consolas[2]){
        free(consolas[0]);
        free(consolas[1]);
        free(consolas);
        hash_destruir(h);
        return;
    }
    strcpy(consolas[2], "Premium 5-Input Mixer");

    consolas[3] = malloc(strlen("Premium 5-Input 2-Bus Mixer")+1);
    if(!consolas[3]){
        free(consolas[0]);
        free(consolas[1]);
        free(consolas[2]);
        free(consolas);
        hash_destruir(h);
        return;
    }
    strcpy(consolas[3], "Premium 5-Input 2-Bus Mixer");

    pa2m_afirmar(hash_insertar(h, "1002", consolas[0], NULL)!=NULL, "Inserto clave 1002 exitosamente");
    pa2m_afirmar(hash_insertar(h, "1204USB", consolas[1], NULL)!=NULL, "Inserto clave 1204USB exitosamente");
    pa2m_afirmar(hash_insertar(h, "302USB", consolas[2], NULL)!=NULL, "Inserto clave 302USB exitosamente");
    pa2m_afirmar(hash_insertar(h, "502", consolas[3], NULL)!=NULL, "Inserto clave 502 exitosamente");

    pa2m_afirmar(hash_cantidad(h)==4, "Hash tiene 4 elementos");

    pa2m_afirmar(hash_quitar(h, "1002")!=NULL, "Intento quitar clave 1002 de hash exitosamente");
    pa2m_afirmar(hash_quitar(h, "1002")==NULL, "Intento quitar clave 1002 nuevamente de hash devuelve NULL");
    pa2m_afirmar(hash_quitar(h, "1002")==NULL, "Intento quitar clave 1002 nuevamente de hash devuelve NULL");

    pa2m_afirmar(hash_cantidad(h)==3, "Hash tiene 3 elemento");

    pa2m_afirmar(hash_insertar(h, "302USB", consolas[2], NULL)!=NULL, "Inserto clave 302USB exitosamente");
    pa2m_afirmar(hash_insertar(h, "502", consolas[3], NULL)!=NULL, "Inserto clave 502 exitosamente");

    pa2m_afirmar(hash_cantidad(h)==3, "Hash tiene 3 elementos");

    pa2m_afirmar(!hash_obtener(h, "1002"), "Busco clave 1002 borrada y encuentro NULL");

    pa2m_afirmar(hash_quitar(h, "1204USB")!=NULL, "Intento quitar clave 1204USB de hash exitosamente");
    pa2m_afirmar(hash_quitar(h, "302USB")!=NULL, "Intento quitar clave 302USB de hash exitosamente");
    pa2m_afirmar(hash_quitar(h, "502")!=NULL, "Intento quitar clave 502 de hash exitosamente");

    pa2m_afirmar(hash_cantidad(h)==0, "Hash tiene 0 elementos");
    
	free(consolas[0]);
	free(consolas[1]);
    free(consolas[2]);
    free(consolas[3]);

    free(consolas);
    hash_destruir_todo(h, destruir_string);
}


void pruebasIteradorHash(){
    hash_t* h = NULL;
    h=hash_crear(5);

    char** consolas = malloc(4*sizeof(char*));
    if(!consolas){
        hash_destruir(h);
        return;
    }

    consolas[0] = malloc(strlen("Premium 10-Input 2-Bus Mixer")+1);
    if(!consolas[0]){
        free(consolas);
        hash_destruir(h);
        return;
    }
    strcpy(consolas[0], "Premium 10-Input 2-Bus Mixer");

    consolas[1] = malloc(strlen("Premium 12-Input 2/2-Bus Mixer")+1);
    if(!consolas[1]){
        free(consolas[0]);
        free(consolas);
        hash_destruir(h);
        return;
    }
    strcpy(consolas[1], "Premium 12-Input 2/2-Bus Mixer");

    consolas[2] = malloc(strlen("Premium 5-Input Mixer")+1);
    if(!consolas[2]){
        free(consolas[0]);
        free(consolas[1]);
        free(consolas);
        hash_destruir(h);
        return;
    }
    strcpy(consolas[2], "Premium 5-Input Mixer");

    consolas[3] = malloc(strlen("Premium 5-Input 2-Bus Mixer")+1);
    if(!consolas[3]){
        free(consolas[0]);
        free(consolas[1]);
        free(consolas[2]);
        free(consolas);
        hash_destruir(h);
        return;
    }
    strcpy(consolas[3], "Premium 5-Input 2-Bus Mixer");

    pa2m_afirmar(hash_insertar(h, "1002", consolas[0], NULL)!=NULL, "Inserto clave 1002 exitosamente");
    pa2m_afirmar(hash_insertar(h, "1204USB", consolas[1], NULL)!=NULL, "Inserto clave 1204USB exitosamente");
    pa2m_afirmar(hash_insertar(h, "302USB", consolas[2], NULL)!=NULL, "Inserto clave 302USB exitosamente");
    pa2m_afirmar(hash_insertar(h, "502", consolas[3], NULL)!=NULL, "Inserto clave 502 exitosamente");
    
    pa2m_afirmar(hash_con_cada_clave(h, mostrar_consola, NULL)==4, "Se mostraron 4 consolas, puedo iterar con aux NULL");

    size_t cantidad_iterados = 0;
    pa2m_afirmar(hash_con_cada_clave(h, iterar_hasta_2, &cantidad_iterados)==cantidad_iterados, "Se iteraron la cantidad correcta de consolas");

    hash_destruir_todo(h, destruir_string);
        free(consolas);

}

void pruebasNULLHash(){
    hash_t* h = NULL;

    pa2m_afirmar((h=hash_crear(5))!=NULL, "Crear un hash con destructor NULL devuelve un hash");
    pa2m_afirmar(hash_insertar(h, NULL, "NULL", NULL)==NULL, "Insertar con clave NULL devuelve -1");
    pa2m_afirmar(hash_cantidad(h)==0, "Hash tiene 0 elementos");

    pa2m_afirmar(hash_insertar(h, "802", "Premium 8-Input 2-Bus Mixer", NULL)!=NULL, "Inserto clave 802 exitosamente");
    pa2m_afirmar(hash_cantidad(h)==1, "Hash tiene 1 elementos");

    pa2m_afirmar(hash_quitar(h, NULL)==NULL, "Intento quitar clave NULL de hash devuelve -1");

    pa2m_afirmar(hash_insertar(h, "202", NULL, NULL)!=NULL, "Inserto clave 202 con elemento NULL exitosamente");



    hash_destruir(h);
}


int main(){
    pa2m_nuevo_grupo("Pruebas de creacion Hash");
    puedoCrearUnHash();

    pa2m_nuevo_grupo("Pruebas de insercion y busqueda");
    puedoInsertarHash();

    pa2m_nuevo_grupo("Pruebas de insercion MUCHAS");
    insercionesMUCHAS_HASH();

    pa2m_nuevo_grupo("Pruebas de borrado");
    puedoBorrarHAsh();

    pa2m_nuevo_grupo("Pruebas de iterador interno");
    pruebasIteradorHash();

    pa2m_nuevo_grupo("Pruebas con NULL");
    pruebasNULLHash();

    return pa2m_mostrar_reporte();
}