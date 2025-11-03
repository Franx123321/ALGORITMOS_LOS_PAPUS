#ifndef MACROS_H_INCLUDED
#define MACROS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#ifdef _MSC_VER
    #pragma comment(lib, "ws2_32.lib")
#endif

#ifdef _WIN32
    // Windows (MinGW / MSVC)
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>
#elif __APPLE__
    // macOS (Frameworks)
    #include <SDL.h>
    #include <SDL_ttf.h>
    #include <SDL_mixer.h>

#else
    // Linux (paquetes SDL2)
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>
#endif

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define MIN(X,Y) ((X)<(Y)?(X):(Y))

#define MIN_FILAS 15
#define MIN_COLUMNAS 15
#define MAX_FILAS 30    //Es lo maximo que se ve bien en pantalla, igualmente depende del tamaño de la ventana y de las celdas
#define MAX_COLUMNAS 50 //Podria ser bastante mas, pero seria una fea relacion filas/columnas
#define MIN_PUNTOS 10
#define MAX_PUNTOS 50

#define MOV_INVALIDO 0
#define MOV_VALIDO 1

#define VICTORIA 2
#define DERROTA -1
#define SALIR 0

#define MIN_ANCHO 300
#define MIN_ALTO 300
#define INI_ANCHO 1360
#define INI_ALTO 800
#define TAM_CELDA 28 //Tamaño en pixeles
#define MARGEN 48    //Margen superior para el HUD

#define ERROR_ARCH 0
#define ERROR_SDL 0
#define ERROR_CONFIG 0
#define ERROR_MEMORIA 0
#define ERROR_GENERACION 0
#define TODO_BIEN 1

#define PUERTO 7777

#define COLOR_BLANCO (SDL_Color){255, 255, 255, 255}
#define COLOR_ROJO (SDL_Color){255, 0, 0, 255}
#define COLOR_AZUL (SDL_Color){0, 0, 255, 255}
#define COLOR_AMARILLO (SDL_Color){255, 255, 0, 255}
#define COLOR_GRIS (SDL_Color){128, 128, 128, 255}
#define COLOR_VERDE (SDL_Color){0,255,0,255}

#define DISTANCIA(X1, Y1, X2, Y2) (abs(X1 - X2) + abs(Y1 - Y2))
#define DISTANCIA_MINIMA_ENTRADA_SALIDA 3
#define DISTANCIA_MINIMA_ENTRE_OBJETOS 2


typedef struct{
    char nombre[50];
    int vidas;
    int puntaje;
    int posX;
    int posY;
    int id;
}Jugador;

typedef struct{
    int posX;
    int posY;
    int vivo;
    char quePisa;
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

typedef struct {
    SDL_Window *ventana;
    SDL_Renderer *renderer;
    TTF_Font *fuente;
    TTF_Font *fuenteHud;
    SDL_Texture *sprites;
    int ancho;
    int alto;
    int tamFuente;
    int tamFuenteHud;
} ContextoSDL;

typedef struct{
    int x,
        y;
}NodoPos;


//ESTRUCTURAS DE DATOS//
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

#endif // MACROS_H_INCLUDED
