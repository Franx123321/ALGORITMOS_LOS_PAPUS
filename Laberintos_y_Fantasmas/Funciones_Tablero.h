#ifndef FUNCIONES_TABLERO_H_INCLUDED
#define FUNCIONES_TABLERO_H_INCLUDED

#include "Macros.h"
#include "Funciones_SDL.h"
#include "Funciones_Generacion.h"

char **crear_matriz(int filas, int columnas);
void destruir_matriz(void ***matriz, int filas);
void mostrar_matriz(char **matriz, int filas, int col);
int inicializarTablero(Tablero *laberinto);
int generarLaberinto(Tablero *laberinto, int fil, int col);
int agujerearLaberinto(Tablero *laberinto);
void mezclarDirecciones(int dir[4][2]);
int generarSalida(Tablero *laberinto);
int guardarLaberinto(Tablero *laberinto);
int cargarLaberinto(Tablero *laberinto, Fantasma *fantasmas, Jugador *jugador, Configuracion config);
void dibujarTablero(ContextoSDL *sdl, Tablero *laberinto, Jugador *j, TTF_Font *fuente, int tamCelda, int offsetX, float escalaTexto);


#endif // FUNCIONES_TABLERO_H_INCLUDED
