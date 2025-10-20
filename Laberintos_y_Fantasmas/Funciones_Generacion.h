#ifndef FUNCIONES_GENERACION_H_INCLUDED
#define FUNCIONES_GENERACION_H_INCLUDED

#include "Macros.h"

//FANTASMAS//
int generarFantasmas(Tablero *laberinto, Fantasma *fantasmas, int maxFantasmas);

//PREMIOS Y VIDAS EXTRA//
int colocarVidasExtra(Tablero *laberinto, int vidasExtra);
int colocarPremios(Tablero *laberinto, int maxPremios);


#endif // FUNCIONES_GENERACION_H_INCLUDED
