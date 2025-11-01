#include <stdio.h>
#include <string.h>
#include "arbol.h"

void crearArbol(tArbolBinBusq *p){
    *p = NULL;
}

int insertarEnArbolOrdenado(tArbolBinBusq *p, void *dato, unsigned tam, int (*cmp)(const void *, const void *)){
    tNodoArbol *nue;
    int comp;

    while(*p){
        comp = cmp((*p)->dato, dato); // < 0 el arbol es menor
        if(comp < 0){
            p = &(*p)->der;
        }
        else if(comp > 0)
            p = &(*p)->izq;
        else
            return 0;
    }

    nue = malloc(sizeof(tNodoArbol));
    if(!nue)
        return 0;

    nue->dato = malloc(tam);
    if(!nue->dato){
        free(nue);
        return 0;
    }

    memcpy(nue->dato, dato, tam);
    nue->tamDato = tam;
    nue->der = nue->izq = NULL;
    *p = nue;
    return 1;
}

tNodoArbol **buscarEnArbol(tArbolBinBusq *p, const char *nombre, int (*comp) (const void *, const void *)) {
    if (!*p)
        return NULL;

    int cmp = comp((*p)->dato, nombre);

    if (cmp == 0)
        return p;
    else if (cmp < 0)
        return buscarEnArbol(&(*p)->der, nombre, comp);
    else
        return buscarEnArbol(&(*p)->izq, nombre, comp);
}

