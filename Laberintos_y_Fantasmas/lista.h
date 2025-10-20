#ifndef LISTA_H_INCLUDED
#define LISTA_H_INCLUDED

#include "Macros.h"

typedef tNodo *tLista;

void crearLista(tLista *l);
int listaVacia(const tLista *l);
void vaciarLista(tLista *l);
int ponerAlComienzo(tLista *l, const void *d, unsigned tamDato);
int sacarPrimeroLista(tLista *l, void *d, unsigned tamDato);
int verPrimeroLista(const tLista *l, void *d, unsigned tamDato);
int ponerAlFinal(tLista *l, const void *d, unsigned tamDato);
int sacarUltimoLista(tLista *l, void *d, unsigned tamDato);
int verUltimoLista(const tLista *l, void *d, unsigned tamDato);
void recorrerLista(const tLista *l, unsigned tamDato, void (*accion)(const void *));
int insertarOrdenadoSinDup(tLista *l, const void *dato, unsigned tamDato, 
                     int (*cmp)(const void *, const void*), 
                     void (*acumular)(void **, unsigned *,const void *, unsigned));
int insertarOrdenadoConDup(tLista *l, const void *dato, unsigned tamDato, 
                     int (*cmp)(const void *, const void*));

#endif // LISTA_H_INCLUDED