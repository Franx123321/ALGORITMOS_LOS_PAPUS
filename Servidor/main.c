/* Este codigo tiene muchos mas comentarios que el juego porque aca hay muchos conceptos nuevos
que esta bueno explicar para que se entiendan .
Nota para el profe: Pido disculpas si algun comentario queda fuera de lugar, 
son evidencias de mi lento descenso a la locura que me olvide de borrar */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "cola.h" //Incluyo la cola para manejar mensajes de clientes

#ifdef _MSC_VER
    #pragma comment(lib, "ws2_32.lib")
#endif

#define PUERTO 7777
#define EN_ESPERA 5 //Cantidad de clientes en espera
#define MAX_CLIENTES 5 //Maximo de clientes simultaneos
#define TAM_BUFFER 256
#define MAX_NOMBRE 50

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

typedef struct{
    SOCKET sock;
    int *clientesConectados;
    HANDLE mutexClientes;
    tCola cola;  // Cola para las peticiones del cliente
}DatosCliente;

// Mutex global para sincronizar acceso al archivo partidas.dat
HANDLE hArchivoMutex = NULL;

DWORD WINAPI atenderCliente(LPVOID arg);

/* 
    1. DWORD es el valor de retorno que pide la API de threads de Windows, se usa en createThread
    2. WINAPI es una convencion de llamadas de Windows
    3. LPVOID es un typedef para void*, se usa en su lugar porque algunas estructuras de 
       threads estan definidas con LPVOID y en casos raros C se enoja si el tipo no coincide 
// Función para procesar y guardar datos en archivo
int procesarYGuardarDatos(const char* buffer);

       exactamente en nombre
*/


int procesarYGuardarDatos(const char* buffer) {
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
            jugador.puntuacion+=puntuacion;
            fseek(archJugadores, -sizeof(DatosJugador), SEEK_CUR);
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


int main()
{
    WSADATA wsaData;  //Muy resumidamente, WSADATA es una struct de Windows que sirve para inicializar el sistema de sockets, es exclusivo de Windows

    SOCKET socketServidor = INVALID_SOCKET, 
           socketCliente = INVALID_SOCKET; //Creo que el tipo de dato explica bastante bien que es esto
    HANDLE mutexClientes = NULL, 
           threadCliente = NULL;
    struct sockaddr_in dirServidor, dirCliente; //Direcciones de sockets
    int tamDirCliente, clientesConectados = 0;
    const char *lleno = "SERVIDOR LLENO";
    DatosCliente *datos = NULL;

    tamDirCliente = sizeof(dirCliente);

    printf("Inicializando servidor TCP...\n");

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //Aca se inicializa el sistema de sockets
    {
        printf("\nERROR al inicializar sistema de Sockets. Codigo: %d\n", WSAGetLastError());
        exit(1);
    }

    socketServidor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Se abre el socket
    if(socketServidor == INVALID_SOCKET)
    {
        printf("ERROR al crear socket. Codigo: %d\n", WSAGetLastError());
        WSACleanup(); //Limpieza del sistema de sockets
        exit(1);
    }

    //Aca se configura la direccion del servidor
    dirServidor.sin_family = AF_INET;
    dirServidor.sin_addr.s_addr = INADDR_ANY; //Aceptar conexiones desde cualquier IP
    dirServidor.sin_port = htons(PUERTO); //Se le asigna el puerto definido en la macro

    //Se asocia el socket a la direccion
    if(bind(socketServidor, (struct sockaddr*)&dirServidor, sizeof(dirServidor)) == SOCKET_ERROR)
    {
        printf("\nERROR en el bind. Codigo: %d\n", WSAGetLastError());
        closesocket(socketServidor);
        WSACleanup();
        exit(1);
    }

    //Se pone el socket en modo escucha
    if(listen(socketServidor, EN_ESPERA) == SOCKET_ERROR)
    {
        printf("\nERROR al poner el socket en modo escucha. Codigo: %d\n", WSAGetLastError()); 
        closesocket(socketServidor);
        WSACleanup();
        exit(1);
    }

    printf("\nServidor escuchando en puerto %d...\n", PUERTO);

    /*Si llego bien hasta aca, creamos mutex, el mutex sirve para sincronizar acceso al 
    contador de clientes (se evita la condicion de carrera)*/
    mutexClientes = CreateMutex(NULL, FALSE, NULL);
    if(mutexClientes == NULL)
    {
        printf("\nERROR al crear el mutex.");
        closesocket(socketServidor);
        WSACleanup();
        exit(1);
    }

    // Crear mutex para proteger accesos al archivo partidas.dat
    hArchivoMutex = CreateMutex(NULL, FALSE, NULL);
    if(hArchivoMutex == NULL)
    {
        printf("\nERROR al crear el mutex de archivo.");
        CloseHandle(mutexClientes);
        closesocket(socketServidor);
        WSACleanup();
        exit(1);
    }


    //La magia, se pone en espera de clientes
    while(1)
    {
        printf("\nEsperando nueva conexion...\n");

        socketCliente = accept(socketServidor, (struct sockaddr*)&dirCliente, &tamDirCliente);
        if(socketCliente == INVALID_SOCKET)
        {
            printf("\nERROR al aceptar una conexion. Codigo: %d\n", WSAGetLastError());
            continue;
        }

        WaitForSingleObject(mutexClientes, INFINITE);
        if(clientesConectados >= MAX_CLIENTES)
        {
            ReleaseMutex(mutexClientes);
            send(socketCliente, lleno, (int)strlen(lleno), 0);
            printf("\nRechazado cliente %s: %d debido a que el servidor esta lleno.", inet_ntoa(dirCliente.sin_addr), ntohs(dirCliente.sin_port));
            closesocket(socketCliente);
            continue;
        }

        ReleaseMutex(mutexClientes);
        
        printf("\nCliente conectado desde %s: %d\n", inet_ntoa(dirCliente.sin_addr), ntohs(dirCliente.sin_port));

        datos = (DatosCliente*)malloc(sizeof(DatosCliente));
        if(!datos)
        {
            printf("\nERROR al reservar memoria para los datos del cliente.\n");
            closesocket(socketCliente);
            continue;
        }
        datos->sock = socketCliente;
        datos->clientesConectados = &clientesConectados;
        datos->mutexClientes = mutexClientes;

        //Aca se crea el thread para el cliente que acaba de llegar y con la funcion atenderCliente automaticamente se atiende
        threadCliente = CreateThread(NULL, 0, atenderCliente, datos, 0, NULL);

        //Cerrar conexion si falla
        if(threadCliente == NULL)
        {
            printf("\nERROR al crear thread para el cliente.\n");
            closesocket(socketCliente);
            free(datos);
        }
        else
            CloseHandle(threadCliente); //Ya se atendio, se cierra
    }

    CloseHandle(mutexClientes);
    CloseHandle(hArchivoMutex);
    closesocket(socketServidor);
    WSACleanup();

    return 0;
}


/* Esta maravilla atiende al cliente, le manda que se pudo conectar (bienvenida en este caso)
   y tambien espera a que termine la conexion. */
DWORD WINAPI atenderCliente(LPVOID arg)
{
    if (!arg) return 1;
    
    DatosCliente *datos = (DatosCliente *)arg;
    SOCKET cliente = datos->sock;
     char buffer[TAM_BUFFER],
         *bienvenida = "Bienvenido al servidor de Laberintos y Fantasmas!\n";
    int bytes;
    char mensaje[TAM_BUFFER];

    // Inicializar cola de mensajes
    crearCola(&datos->cola);

    WaitForSingleObject(datos->mutexClientes, INFINITE);
    (*datos->clientesConectados)++;
    printf("\nClientes conectados: %d\n", *datos->clientesConectados);
    ReleaseMutex(datos->mutexClientes); //No es lo mismo liberar que cerrar

    send(cliente, bienvenida, (int)strlen(bienvenida), 0);

    // Bucle principal: recibir mensajes y procesarlos
    while(1)
    {
        bytes = recv(cliente, buffer, sizeof(buffer) - 1, 0);
        if(bytes > 0)
        {
            buffer[bytes] = '\0';
            printf("Cliente: '%s'\n", buffer);

            if(strcmp(buffer, "FIN") == 0)
            {
                printf("\nSe recibio fin, cerrando conexion con el cliente...\n");
                break;
            }

            // Encolar el mensaje
            if(encolar(&datos->cola, buffer, strlen(buffer) + 1)) {
                // Procesar todos los mensajes en la cola
                while(!colaVacia(&datos->cola)) {
                    if(desencolar(&datos->cola, mensaje, TAM_BUFFER)) {
                        if(procesarYGuardarDatos(mensaje)) {
                            printf("Mensaje procesado exitosamente\n");
                        } else {
                            printf("Error al procesar mensaje\n");
                        }
                    }
                }
            } else {
                printf("Error al encolar mensaje\n");
            }
        }
        else if(bytes == 0)
        {
            printf("\nEl cliente cerro la conexion.\n");
            break;
        }
        else
        {
            int error_code = WSAGetLastError();
            if (error_code == WSAECONNABORTED || error_code == WSAECONNRESET) {
                printf("\nEl cliente se desconecto abruptamente.\n");
            } else {
                printf("\nError al recibir mensajes del cliente. Codigo: %d\n", error_code);
            }
            break;
        }
    }   

    closesocket(cliente);

    WaitForSingleObject(datos->mutexClientes, INFINITE);
    (*datos->clientesConectados)--;
    printf("\nCliente desconectado.\n\nQuedan conectados: %d\n", *datos->clientesConectados);
    ReleaseMutex(datos->mutexClientes); 

    // Limpiar recursos
    vaciarCola(&datos->cola);
    free(datos);
    return 0;
} 
