#ifndef ARBOL_H_INCLUDED
#define ARBOL_H_INCLUDED
#include "Macros.h"

typedef tNodoArbol *tArbolBinBusq;

void crearArbol(tArbolBinBusq *p);
int insertarEnArbolOrdenado(tArbolBinBusq *p, void *dato, unsigned tam, int (*cmp)(const void *, const void *));
tNodoArbol **buscarEnArbol(tArbolBinBusq *p, const char *nombre, int (*comp) (const void *, const void *)) ;

#endif

