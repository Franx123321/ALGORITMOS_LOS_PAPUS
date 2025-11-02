#include "Funciones_Archivos.h"


int procesarYGuardarDatos(const char* buffer, HANDLE* hArchivoMutex) {
    char nombre[MAX_NOMBRE];
    int puntuacion, movimientos;
    DatosPartida partida;
    DatosJugador jugador, jugadorExistente;
    FILE *archPartida, *archJugadores;
    long pos;
    int resultado = 0, encontrado = 0;

    if(sscanf(buffer, "%[^|]|%d|%d", nombre, &puntuacion, &movimientos) != 3) {
        printf("Error: formato de mensaje invalido\n");
        return 0;
    }
    if(nombre == NULL) {
        printf("Error: nombre nulo\n");
        return 0;
    }
    if(puntuacion < 0 || movimientos < 0) {
        printf("Error: puntuacion o movimientos invalidos\n");
        return 0;
    }

    // Preparar estructura DatosPartida
    strncpy(partida.nombre, nombre, MAX_NOMBRE - 1);
    partida.nombre[MAX_NOMBRE - 1] = '\0';
    partida.puntuacion = puntuacion;
    partida.movimientos = movimientos;

    // Proteger acceso al archivo
    WaitForSingleObject(hArchivoMutex, INFINITE);

    archJugadores = fopen("Jugadores.dat", "r+b");
    if (!archJugadores) {
        archJugadores = fopen("Jugadores.dat", "w+b");
    }
    if(archJugadores) {
        while(fread(&jugadorExistente, sizeof(DatosJugador), 1, archJugadores) == 1) { //Leoo, 10, 20
            if(strcmp(jugadorExistente.nombre, nombre) == 0) {
                encontrado = 1;
                break;
            }
        }
        if (encontrado==0) {
            // Agregar nuevo jugador
            strncpy(jugador.nombre, nombre, MAX_NOMBRE - 1);
            jugador.nombre[MAX_NOMBRE - 1] = '\0';
            jugador.id = (int)(ftell(archJugadores) / sizeof(DatosJugador)) + 1;
            jugador.puntuacion = puntuacion;
            fseek(archJugadores, 0, SEEK_END);
            fwrite(&jugador, sizeof(DatosJugador), 1, archJugadores);
        }
        else if(encontrado==1){
            jugadorExistente.puntuacion+=puntuacion;
            fseek(archJugadores,(int)((-1)*sizeof(DatosJugador)), SEEK_CUR);
            fwrite(&jugadorExistente, sizeof(DatosJugador), 1, archJugadores);
        }
        fflush(archJugadores);
        fclose(archJugadores);
    } else {
        printf("Error al abrir jugadores.dat\n");
    }

    archPartida = fopen("Partidas.dat", "ab");
    if(archPartida) {
        fseek(archPartida, 0, SEEK_END);
        pos = ftell(archPartida);
        partida.id = (int)(pos / sizeof(DatosPartida)) + 1;

        if(fwrite(&partida, sizeof(DatosPartida), 1, archPartida) == 1) {
            printf("Datos guardados: Jugador=%s, ID=%d, Puntuacion=%d, Movimientos=%d\n",
                   partida.nombre, partida.id, partida.puntuacion, partida.movimientos);
            resultado = 1;
        }

        fflush(archPartida);
        fclose(archPartida);
    } else {
        printf("Error al abrir partidas.dat\n");
    }

    ReleaseMutex(hArchivoMutex);
    return resultado;
}