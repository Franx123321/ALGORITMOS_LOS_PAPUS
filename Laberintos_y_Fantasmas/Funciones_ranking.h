#ifndef FUNCIONES_RANKING_H_INCLUDED
#define FUNCIONES_RANKING_H_INCLUDED

#include "Macros.h"
#include "lista.h"


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>




typedef struct{
    char nombre[50];
    int id_jugador;
    int p_total;
    int partidas_jugadas;
}Usuario;


void printUsuario(const void *dato);
int cmpPuntajeDesc(const void *a, const void *b);

void verRanking(tLista*, SOCKET* sock);

void mostrarRankingSDL(ContextoSDL *sdl, TTF_Font *fuente, tLista *ranking);
#endif // FUNCIONES_RANKING_H_INCLUDED
