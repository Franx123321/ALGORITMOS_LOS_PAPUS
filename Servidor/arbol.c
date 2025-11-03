#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

tNodoArbol **buscarEnArbol(tArbolBinBusq *p, const void *dato, int (*comp) (const void *, const void *)) {
    if (!*p)
        return NULL;

    int cmp = comp((*p)->dato, dato);

    if (cmp == 0)
        return p;
    else if (cmp < 0)
        return buscarEnArbol(&(*p)->der, dato, comp);
    else
        return buscarEnArbol(&(*p)->izq, dato, comp);
}

int alturaArbolBin(tArbolBinBusq *p){
    int ai = 0, ad = 0;

    if(!*p)
        return 0;

    ai = alturaArbolBin(&(*p)->izq);
    ad = alturaArbolBin(&(*p)->der);
    return (ai > ad ? ai : ad) + 1;
}

tNodoArbol **mayorNodoArbolBinBusq(tArbolBinBusq *p){
    if(!*p)
        return NULL;

    while((*p)->der){
        p = &(*p)->der;
    }

    return (tNodoArbol **)p;
}

int eliminarRaizArbolBinBusq(tArbolBinBusq *p){
    tNodoArbol **remp, *elim;

    if(!*p)
        return 0;

    free((*p)->dato);
    if(!(*p)->izq && !(*p)->der)
    {
        free(*p);
        *p=NULL;
        return 1; // Eliminacion exitosa
    }

    remp = alturaArbolBin(&(*p)->izq) > alturaArbolBin(&(*p)->der) ? mayorNodoArbolBinBusq(&(*p)->izq) : mayorNodoArbolBinBusq(&(*p)->der);

    elim = *remp;
    (*p)->dato = elim->dato;
    (*p)->tamDato = elim->tamDato;

    *remp = elim->izq ? elim->izq : elim->der;

    free(elim);

    return 1;
}

void destruirArbol(tArbolBinBusq *p){
    if(!*p)
        return;

    destruirArbol(&(*p)->izq);
    destruirArbol(&(*p)->der);
    free((*p)->dato);
    free(*p);
    *p = NULL;
}
