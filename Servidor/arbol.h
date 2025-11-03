#ifndef ARBOL_H_INCLUDED
#define ARBOL_H_INCLUDED

typedef struct sNodoArbol{
    void *dato;
    unsigned tamDato;
    struct sNodoArbol *izq, *der;
}tNodoArbol;

typedef tNodoArbol *tArbolBinBusq;

void crearArbol(tArbolBinBusq *p);
int insertarEnArbolOrdenado(tArbolBinBusq *p, void *dato, unsigned tam, int (*cmp)(const void *, const void *));
tNodoArbol **buscarEnArbol(tArbolBinBusq *p, const void *dato, int (*comp) (const void *, const void *));
tNodoArbol **mayorNodoArbolBinBusq(tArbolBinBusq *p);
tNodoArbol **buscarEnArbolNoClave(tArbolBinBusq *a, const void *dato, int (*cmp)(const void*, const void*));
int eliminarRaizArbolBinBusq(tArbolBinBusq *p);
int alturaArbolBin(tArbolBinBusq *p);
void destruirArbol(tArbolBinBusq *p);
int contarNodosArbol(const tArbolBinBusq *a);

#endif

