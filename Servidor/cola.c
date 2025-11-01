#include "cola.h"

void crearCola(tCola *c)
{
    c->pri = NULL;
    c->ult = NULL;
}

int encolar(tCola *c, const void *dato, unsigned tamDato)
{
    tNodo *new;

    if((new = (tNodo *)malloc(sizeof(tNodo))) == NULL || (new->dato = malloc(tamDato)) == NULL)
    {
        free(new);
        return 0;
    }

    memcpy(new->dato, dato, tamDato);
    new->tamDato = tamDato;

    new->sig = NULL;

    if(c->ult)
        c->ult->sig = new;
    else
        c->pri = new;

    c->ult = new;

    return 1;
}

int colaVacia(const tCola *c)
{
    return c->pri == NULL;
}

int verPrimero(const tCola *c, void *dato, unsigned tamDato)
{
    if(!c->pri)
        return 0;

    memcpy(dato, c->pri->dato, MIN(tamDato, c->pri->tamDato));
    return 1;
}

int desencolar(tCola *c, void *dato, unsigned tamDato)
{
    tNodo *aux = c->pri;
    if(c == NULL)
        return 0;

    c->pri = aux->sig;
    memcpy(dato, aux->dato, MIN(tamDato, aux->tamDato));

    free(aux->dato);
    free(aux);

    if(c->pri == NULL)
        c->ult = NULL;

    return 1;
}

void vaciarCola(tCola *c)
{
    tNodo *aux;
    while(c->pri)
    {
        aux = c->pri;
        c->pri = aux->sig;
        free(aux->dato);
        free(aux);
    }
    c->ult = NULL;
}