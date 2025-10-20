#include "lista.h"


void crearLista(tLista *l)
{
    *l = NULL;
}

int listaVacia(const tLista *l)
{
    return *l == NULL;
}

void vaciarLista(tLista *l)
{
    while(*l)
    {
        tNodo *aux = *l;

        *l = aux->sig;
        free(aux->dato);
        free(aux);
    }
}

int ponerAlComienzo(tLista *l, const void *d, unsigned tamDato)
{
    tNodo *new;

    if((new = (tNodo *)malloc(sizeof(tNodo))) == NULL ||
       (new->dato = malloc(tamDato)) == NULL)
    {
        free(new);
        return 0;
    }
    memcpy(new->dato, d, tamDato);
    new->tamDato = tamDato;
    new->sig = *l;
    *l = new;
    return 1;
}

int sacarPrimeroLista(tLista *l, void *d, unsigned tamDato)
{
    tNodo *aux = *l;

    if(aux == NULL)
        return 0;
    *l = aux->sig;
    memcpy(d, aux->dato, MIN(tamDato, aux->tamDato));
    free(aux->dato);
    free(aux);
    return 1;
}

int verPrimeroLista(const tLista *l, void *d, unsigned tamDato)
{
    if(*l == NULL)
        return 0;
    memcpy(d, (*l)->dato, MIN(tamDato, (*l)->tamDato));
    return 1;
}

int ponerAlFinal(tLista *l, const void *d, unsigned tamDato)
{
    tNodo *new;

    while(*l)
        l = &(*l)->sig;

    if((new = (tNodo *)malloc(sizeof(tNodo))) == NULL ||
       (new->dato = malloc(tamDato)) == NULL)
    {
        free(new);
        return 0;
    }
    memcpy(new->dato, d, tamDato);
    new->tamDato = tamDato;
    new->sig = NULL;
    *l = new;
    return 1;
}

int sacarUltimoLista(tLista *l, void *d, unsigned tamDato)
{
    if(*l == NULL)
        return 0;

    while((*l)->sig)
        l = &(*l)->sig;
    memcpy(d, (*l)->dato, MIN(tamDato, (*l)->tamDato));
    free((*l)->dato);
    free(*l);
    *l = NULL;
    return 1;
}

int verUltimoLista(const tLista *l, void *d, unsigned tamDato)
{
    if(*l == NULL)
        return 0;
    while((*l)->sig)
        l = &(*l)->sig;
    memcpy(d, (*l)->dato, MIN(tamDato, (*l)->tamDato));
    return 1;
}

void recorrerLista(const tLista *l, unsigned tamDato, void (*accion)(const void *))
{
    tNodo *actual = *l;
    void *temp;
    unsigned min;

    while(actual != NULL)
    {
        min = MIN(tamDato, actual->tamDato);
        temp = malloc(min);
        if(temp != NULL)
        {
            memcpy(temp, actual->dato, min);
            accion(temp);
            free(temp);
        }
        actual = actual->sig;
    }
}

int insertarOrdenadoSinDup(tLista *l, const void *dato, unsigned tamDato, 
                           int (*cmp)(const void *, const void*), 
                           void (*acumular)(void **, unsigned *,const void *, unsigned))
{
    tNodo *new;
    tNodo **actual = l;

    while(*actual && cmp((*actual)->dato, dato) < 0)
        actual = &(*actual)->sig;

    if(*actual && cmp((*actual)->dato, dato) == 0)
    {
        if(acumular)
        {
            acumular(&(*actual)->dato, &(*actual)->tamDato, dato, tamDato);
            return 1;
        }
        return 0;
    }

    if((new = (tNodo *)malloc(sizeof(tNodo))) == NULL || (new->dato = malloc(tamDato)) == NULL)
    {
        free(new);
        return 0;
    }

    memcpy(new->dato, dato, tamDato);
    new->tamDato = tamDato;
    new->sig = *actual;
    *actual = new;

    return 1;
}

int insertarOrdenadoConDup(tLista *l, const void *dato, unsigned tamDato, 
                            int (*cmp)(const void *, const void*))
{
    tNodo *new;
    tNodo **actual = l;

    while(*actual && cmp((*actual)->dato, dato) < 0)
        actual = &(*actual)->sig;

    if((new = (tNodo *)malloc(sizeof(tNodo))) == NULL || (new->dato = malloc(tamDato)) == NULL)
    {
        free(new);
        return 0;
    }

    memcpy(new->dato, dato, tamDato);
    new->tamDato = tamDato;
    new->sig = *actual;
    *actual = new;

    return 1;
}