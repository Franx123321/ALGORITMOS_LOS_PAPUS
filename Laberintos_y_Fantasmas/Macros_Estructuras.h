#ifndef MACROS_ESTRUCTURAS_H_INCLUDED
#define MACROS_ESTRUCTURAS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define RESERVAR_MEM(X,Y,Z,W) (((X) = (typeof(X)) malloc(Y)) == NULL ||((Z) = malloc(W)) == NULL ? free(X), 0 : 1 )
#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define MIN(X,Y) ((X)<(Y)?(X):(Y))




#endif // MACROS_ESTRUCTURAS_H_INCLUDED
