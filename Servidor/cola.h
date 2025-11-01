#ifndef COLA_H_INCLUDED
#define COLA_H_INCLUDED

#include <stdlib.h>
#include <string.h>


#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define MIN(X,Y) ((X)<(Y)?(X):(Y))


typedef struct sNodo{
    void *dato;
    unsigned tamDato;
    struct sNodo *sig;
}tNodo;


typedef struct{
    tNodo *pri,
          *ult;
}tCola;

void crearCola(tCola *c);
int encolar(tCola *c, const void *dato, unsigned tamDato);
int desencolar(tCola *c, void *dato, unsigned tamDato);
int verPrimero(const tCola *c, void *dato, unsigned tamDato);
int colaVacia(const tCola *c);
void vaciarCola(tCola *c);

#endif // COLA_H_INCLUDED