#include "Macros.h"
#include "arbol.h"

typedef struct{
    int id_partida;
    char nombre[50];
    int id_usuario;
    int cantidad_movimientos;
}Partida;

typedef struct{
    int clave;
    long offset;
}Ind;

typedef struct{
    int id_jugador;
    char nombre[50];
    int p_total;
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