#ifndef FUNCIONES_ARCHIVOS_H_INCLUDED
#define FUNCIONES_ARCHIVOS_H_INCLUDED

#include "main.h"
#include "arbol_bin_busq.h"

typedef struct {
    int id;
    char nombre[MAX_NOMBRE];
    int puntuacion;
} DatosJugador;

typedef struct {
    int id;
    char nombre[MAX_NOMBRE];
    int puntuacion;
    int movimientos;
} DatosPartida;

int procesarYGuardarDatos(const char* buffer, HANDLE* hArchivoMutex);


#endif // FUNCIONES_ARCHIVOS_H_INCLUDED
