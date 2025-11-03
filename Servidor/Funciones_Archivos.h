#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <signal.h>
#ifdef _MSC_VER
    #pragma comment(lib, "ws2_32.lib")
#endif
#include "arbol.h"
#include "cola.h"

#define PUERTO 7777
#define EN_ESPERA 5 //Cantidad de clientes en espera
#define MAX_CLIENTES 5 //Maximo de clientes simultaneos
#define TAM_BUFFER 256
#define MAX_NOMBRE 50


typedef struct{
    SOCKET sock;
    int *clientesConectados;
    HANDLE mutexClientes; //El que no entiende lo que es un mutex que curse Sistemas Operativos
    HANDLE mutexArbol;  
    tArbolBinBusq *arbolIndice;
    tCola cola;
}DatosCliente;

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
    int Id; //Balanceo
    char nombre[50]; //Busqueda
    long offset;
}Induser;

int almacenarPartida(Usuario *j, int cantmovimientos);
int almacenarJugador(Usuario *j, tArbolBinBusq *arbol, HANDLE mutexArbol);
void cargarIndiceDesdeArchivo(tArbolBinBusq *p, FILE *pf);
int cargarIndiceDesdeIdx(tArbolBinBusq *p, const char *ruta);
int guardarIndiceEnArchivo(const char *rutaBin, const char *rutaIdx);
int comparacionArbol(const void *a, const void *b);
int comparacionIndexes(const void *a, const void *b);
int cargarDatosEnArch(FILE *pf, Usuario *j, const void *dato);
int procesarYGuardarDatos(const char* buffer, tArbolBinBusq *arbol, HANDLE mutexArbol);
void manejarCtrlC(int signo);