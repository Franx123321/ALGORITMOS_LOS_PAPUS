#ifndef FUNCIONES_RANKING_H_INCLUDED
#define FUNCIONES_RANKING_H_INCLUDED

#include "macros.h"
#include "lista.h"

typedef struct{
    char nombre[50];
    int id_jugador;
    int p_total;
}Usuario;

void verRanking(SOCKET* sock);

#endif // FUNCIONES_RANKING_H_INCLUDED
