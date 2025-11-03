#include "arbol.h"

typedef struct{
    char nombre[50];
    int puntaje;
    int id;
}Jugador;

typedef struct{
    char nombre[50];
    int id_partida;
    int id_usuario;
    int puntaje;
    int cantidad_movimientos;
}Partida;

typedef struct{
    char nombre[50];
    int id_jugador;
    int p_total;
    int partidas_jugadas;
}Usuario;

typedef struct{
    char nombre[50];
    long offset;
}Induser;

int almacenarPartida(Jugador *j, int cantmovimientos);
int almacenarJugador(Jugador *j);
void cargarIndiceDesdeArchivo(tArbolBinBusq *p, FILE *pf);
int comparacionArbol(const void *a, const void *b);
int comparacionIndexes(const void *a, const void *b);
int cargarDatosEnArch(FILE *pf, Jugador *j, const void *dato);
int procesarYGuardarDatos(const char* buffer);
