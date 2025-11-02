#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>



#ifdef _MSC_VER
    #pragma comment(lib, "ws2_32.lib")
#endif

#include "cola.h" //Incluyo la cola para manejar mensajes de clientes

#define PUERTO 7777
#define EN_ESPERA 5 //Cantidad de clientes en espera
#define MAX_CLIENTES 5 //Maximo de clientes simultaneos
#define TAM_BUFFER 256
#define MAX_NOMBRE 50

typedef struct{
    SOCKET sock;
    int *clientesConectados;
    HANDLE mutexClientes;
    tCola cola;  // Cola para las peticiones del cliente
}DatosCliente;




#endif // MAIN_H_INCLUDED
