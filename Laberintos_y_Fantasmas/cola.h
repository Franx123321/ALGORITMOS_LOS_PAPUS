#ifndef COLA_H_INCLUDED
#define COLA_H_INCLUDED
#include "Macros.h"

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
