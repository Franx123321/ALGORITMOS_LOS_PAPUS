#ifndef MACROS_H_INCLUDED
#define MACROS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
    // Windows (MinGW / MSVC)
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
#elif __APPLE__
    // macOS (Frameworks)
    #include <SDL.h>
    #include <SDL_ttf.h>
#else
    // Linux (paquetes SDL2)
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
#endif

#ifdef WIN_32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#define RESERVAR_MEM(X,Y,Z,W) (((X) = (typeof(X)) malloc(Y)) == NULL ||((Z) = malloc(W)) == NULL ? free(X), 0 : 1 )

#define MIN_FILAS 8
#define MIN_COLUMNAS 8
#define MAX_FILAS 50
#define MAX_COLUMNAS 50

#define MOV_INVALIDO 0
#define MOV_VALIDO 1

#define VICTORIA 2
#define DERROTA -1

#define TAM_CELDA 30 //Tamaño en pixeles

#define ERROR_SDL 0
#define ERROR_CONFIG 0
#define TODO_BIEN 1

#define COLOR_BLANCO (SDL_Color){255, 255, 255, 255}


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