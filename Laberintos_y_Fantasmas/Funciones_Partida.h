#ifndef FUNCIONES_PARTIDA_H_INCLUDED
#define FUNCIONES_PARTIDA_H_INCLUDED

#include "Macros.h"
#include "cola.h"
#include "lista.h"
#include "Funciones_SDL.h"
#include "Funciones_Tablero.h"
#include "Funciones_Generacion.h"

int menu(ContextoSDL *sdl);
int pantallaIngresarNombre(ContextoSDL *sdl, TTF_Font *fuente, Jugador *jugador);
int Jugar(Tablero *laberinto, Jugador *jugador, Fantasma *fantasmas, int maxFantasmas,
            ContextoSDL *sdl, tCola *ColaMovimientos, int *cantmovimientos);
int moverFantasmas(Tablero *laberinto, Fantasma *fantasmas, Jugador *jugador, int maxFantasmas);
int encontrarFantasma(Fantasma *fantasmas, int maxFantasmas, int x, int y);
int realizarMovimiento(Tablero *laberinto, Jugador *jugador, Fantasma *fantasmas, int maxFantasmas, char direccion);
void victoria(ContextoSDL *sdl, TTF_Font *fuente, int puntaje, tLista *movimientos);
void derrota(ContextoSDL *sdl, TTF_Font *fuente, tLista *movimientos);
int enviarDatosAlServidor(SOCKET sock, const char* nombre, int puntuacion, int cantMovimientos);

#endif // FUNCIONES_PARTIDA_H_INCLUDED
