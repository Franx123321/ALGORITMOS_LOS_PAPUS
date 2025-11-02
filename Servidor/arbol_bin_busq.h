#ifndef ARBOL_BIN_BUSQ_H_INCLUDED
#define ARBOL_BIN_BUSQ_H_INCLUDED

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define CLA_DUP 0
#define SIN_MEM 0
#define SIN_INICIALIZAR 0
#define ERROR_ARCH 0
#define TODO_BIEN 1

typedef struct sNodoArbol
{
    void *info;
    unsigned tamInfo;
    struct sNodoArbol *izq,
                      *der;
}tNodoArbol;

typedef tNodoArbol* tArbolBinBusq;  

void crearArbolBinBusq(tArbolBinBusq *p);
int insertarArbolBinBusq(tArbolBinBusq *p, const void *d, unsigned tam, int (*cmp)(const void*, const void*));
int insertarRecArbolBinBusq(tArbolBinBusq *p, const void *d, unsigned tam, int (*cmp)(const void*, const void*));

void recorrerEnOrdenRecArbolBinBusq(const tArbolBinBusq *p, unsigned n, void * params, void (*accion)(void*, unsigned, unsigned, void*));
void recorrerEnOrdenArbolBinBusq(const tArbolBinBusq *p, void * params, void (*accion)(void*, unsigned, unsigned, void*));
void recorrerEnOrdenInversoArbolBinBusq(const tArbolBinBusq *p, void * params, void (*accion)(void*, unsigned, unsigned, void*));
void recorrerPreOrdenArbolBinBusq(const tArbolBinBusq *p, void * params, void (*accion)(void*, unsigned, unsigned, void*));
void recorrerPosOrdenArbolBinBusq(const tArbolBinBusq *p, void * params, void (*accion)(void*, unsigned, unsigned, void*));
void recorrerEnOrdenSimpleArbolBinBusq(const tArbolBinBusq *p, void * params, void (*accion)(void*, unsigned, unsigned, void*));
void recorrerPreOrdenSimpleArbolBinBusq(const tArbolBinBusq *p, void * params, void (*accion)(void*, unsigned, unsigned, void*));

int eliminarRaizArbolBinBusq(tArbolBinBusq *p);
int eliminarElemArbolBinBusq(tArbolBinBusq *p,void *d, unsigned tam, int (*cmp)(const void*, const void*));

int buscarElemArbolBinBusq(const tArbolBinBusq *p, void *d, unsigned tam, int (*cmp)(const void*, const void*));

int cargarArchivoBinOrdenadoAbiertoArbolBinBusq(tArbolBinBusq *p, FILE *pf, unsigned tamInfo);
int cargarArchivoBinOrdenadoArbolBinBusq(tArbolBinBusq *p, const char *path, unsigned tamInfo);
int cargarDesdeDatosOrdenadosArbolBinBusq(tArbolBinBusq *p, void *ds, unsigned cantReg, unsigned (*leer)(void**, void*, unsigned, void *params), void *params);

int mayorElemNoClaveArbolBinBusq(const tArbolBinBusq *p, void *d, unsigned tam, int (*cmp)(const void*, const void*));
int menorElemNoClaveArbolBinBusq(const tArbolBinBusq *p, void *d, unsigned tam, int (*cmp)(const void*, const void*));
int buscarElemNoClaveArbolBinBusq(const tArbolBinBusq *p, void *d, unsigned tam, int (*cmp)(const void*, const void*));


unsigned alturaArbolBin(const tArbolBinBusq *p);
unsigned cantNodosArbolBin(const tArbolBinBusq *p);
unsigned cantNodosHastaNivelArbolBin(const tArbolBinBusq *p, int n);

int mayorElemArbolBinBusq(const tArbolBinBusq *p, void *d, unsigned tam);
int menorElemArbolBinBusq(const tArbolBinBusq *p, void *d, unsigned tam);

int esCompletoArbolBin(const tArbolBinBusq *p);
int esBalanceadoArbolBin(const tArbolBinBusq *p);
int esAVLArbolBinBusq(const tArbolBinBusq *p);
int esCompleto2ArbolBin(const tArbolBinBusq *p);
int esBalanceado2ArbolBin(const tArbolBinBusq *p);
int esAVL2ArbolBinBusq(const tArbolBinBusq *p);

#endif // ARBOL_BIN_BUSQ_H_INCLUDED
