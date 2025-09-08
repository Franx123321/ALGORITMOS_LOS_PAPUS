#ifndef MACROS_H_INCLUDED
#define MACROS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#ifdef WIN_32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#define RESERVAR_MEM(X,Y,Z,W) (((X) = (typeof(X)) malloc(Y)) == NULL ||((Z) = malloc(W)) == NULL ? free(X), 0 : 1 )

typedef struct sNodo{
    void *dato;
    unsigned tamDato;
    struct sNodo *sig;
}tNodo;

typedef struct sNodoArbol{
    void *dato;
    unsigned tamDato;
    struct sNodoArbol *izq, *der;
}tNodoArbol;

typedef struct{
    char nombre[50];
    int vidas;
    int puntaje;
    int posX;
    int posY;
}Jugador;

typedef struct{
    int posX;
    int posY;
}Fantasma;

typedef struct{
    int filas;
    int columnas;
    int vidasIniciales;
    int maxFantasmas;
    int maxPremios;
    int maxVidasExtra;
}Configuracion;

typedef struct{
    int filas;
    int columnas;
    char **celdas;
}Tablero;

typedef struct{
    int clave;
    long offset;
}Indice;


#endif // MACROS_H_INCLUDED