#define SDL_MAIN_HANDLED
#include "Macros.h"
#include "cola.h"
#include "Funciones_SDL.h"
#include "Funciones_Tablero.h"
#include "Funciones_Generacion.h"
#include "Funciones_Partida.h"

//CONFIGURACION//
int leerConfig(Configuracion *config); //Esta se queda en main porque no se donde mandarla


int main()
{
    Configuracion config;
    Tablero laberinto;
    Jugador jugador;
    Fantasma *fantasmas;
    ContextoSDL sdl;
    WSADATA wsaData;
    SOCKET sock; //No le puedo llamar socket porque asi se llama la funcion
    struct sockaddr_in dirServidor;
    char buffer[256];
    int opMenu, bytesLeidos, cantMovimientos=0, EstadoJuego;

    system("chcp 65001 > nul");

    srand(time(NULL));

    //CONFIGURACION//
    if(leerConfig(&config) == 0)
    {
        printf("\nERROR al leer el archivo de configuración.");
        exit(1);
    }
    
    fantasmas = malloc(config.maxFantasmas * sizeof(Fantasma));
    if(!fantasmas)
    {
        printf("\nERROR al reservar memoria para los fantasmas.");
        exit(1);
    }

    //CONEXION AL SERVER//
    /* Explico algo: Si falla algo en la creacion de variables relacionadas con la conexion,
       termino el programa, porque esto significaria que fallo algo en la memoria, prefiero
       no arriesgarme a tener errores de memoria o codigo. El modo contingencia se activa solo
       si falla la conexion, no si falla algo interno del propio juego */
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("\nERROR al inicializar Winsock.\n");
        free(fantasmas);
        exit(1);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET)
    {
        printf("\nERROR al crear socket.\n");
        free(fantasmas);
        WSACleanup();
        exit(1);
    }

    dirServidor.sin_family = AF_INET;
    dirServidor.sin_port = htons(PUERTO);
    inet_pton(AF_INET, "127.0.0.1", &dirServidor.sin_addr);

    //Intento conectarme
    if(connect(sock, (struct sockaddr*)&dirServidor, sizeof(dirServidor)) == SOCKET_ERROR)
    {
            printf("\nERROR al conectar al servidor,se jugara en modo contingencia.\n");
            closesocket(sock);
            WSACleanup();
            /*
            BORRADO
            */
    }
    else
    {
        printf("\nConectado con exito al servidor...\n");

        bytesLeidos = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if(bytesLeidos > 0)
        {
            buffer[bytesLeidos] = '\0';
            printf("\nServidor: %s\n", buffer);
        }
        else
            printf("\nNo se recibio ningun mensaje del servidor.\n");
    }

    //JUGADOR//
    jugador.vidas = config.vidasIniciales;
    jugador.posY = 1;
    jugador.posX = 0;
    jugador.puntaje = 0;

    //TABLERO//
    //Aca se carga el contenido del laberinto: paredes, caminos, salida, fantasmas, premios y vidas extra
    laberinto.celdas = NULL;
    laberinto.filas = config.filas;
    laberinto.columnas = config.columnas;
    if(cargarLaberinto(&laberinto, fantasmas, &jugador, config) != TODO_BIEN)
    {
        printf("\nError al cargar el laberinto.");
        closesocket(sock);
        WSACleanup();
        free(fantasmas);
        exit(1);
    }

    //Aca se guarda la disposicion inicial del laberinto en un archivo de texto
    guardarLaberinto(&laberinto);

    //SDL//
    if(inicializarSDL(&sdl, &config) != TODO_BIEN)
    {
        closesocket(sock);
        WSACleanup();
        free(fantasmas);
        destruir_matriz((void **)laberinto.celdas, laberinto.filas);
        exit(1);
    }

    //LOGICA DE JUEGO//
    opMenu = menu(sdl.renderer, sdl.fuente, sdl.ancho, sdl.alto);
    if(opMenu == 3)
        printf("\nSe selecciono salir");
    else if(opMenu == 2) //TEMPORAL
        destruir_matriz((void **)laberinto.celdas, laberinto.filas);
    else if(opMenu == 1)
        if (pantallaIngresarNombre(&sdl, sdl.fuente, &jugador) != SALIR)
            if((EstadoJuego=Jugar(&laberinto, &jugador, fantasmas, config.maxFantasmas, &sdl, &cantMovimientos)) == ERROR_MEMORIA)
                printf("\nSe produjo un error durante el juego.");

    if(EstadoJuego == VICTORIA)
        enviarDatosAlServidor(sock, jugador.nombre, jugador.puntaje, cantMovimientos);

    send(sock, "FIN", 3, 0);

    //DESTRUCTORES Y LIBERACIONES//
    closesocket(sock);
    WSACleanup();
    destruir_matriz((void **)laberinto.celdas, laberinto.filas);
    destruirSDL(&sdl);
    free(fantasmas);

    printf("\nConexion cerrada y recursos liberados.\n");

    return 0;
}



//CONFIGURACION//
int leerConfig(Configuracion *config)
{
    int valor, seteados = 0;
    char linea[128], clave[80], *sep;

    FILE *archConfig = fopen("config.txt", "rt");
    if (!archConfig)
        return ERROR_ARCH;

    while (fgets(linea, 128, archConfig)) {
        sep = strchr(linea, '=');
        if (!sep) continue;

        strncpy(clave, linea, sep - linea);
        *(clave + (sep - linea)) = '\0';

        sscanf(sep + 1, " %d", &valor);

        if (strcmp(clave, "filas") == 0) {
            config->filas = valor;
            seteados |= 1; // Bit: |= 000001
        }
        if (strcmp(clave, "columnas") == 0) {
            config->columnas = valor;
            seteados |= 2; // Bit: |= 000010
        }
        if (strcmp(clave, "vidas_inicio") == 0) {
            config->vidasIniciales = valor;
            seteados |= 4; // Bit: |= 000100
        }
        if (strcmp(clave, "maximo_numero_fantasmas") == 0) {
            config->maxFantasmas = valor;
            seteados |= 8; // Bit: |= 001000
        }
        if (strcmp(clave, "maximo_numero_premios") == 0) {
            config->maxPremios = valor;
            seteados |= 16; // Bit: |= 010000
        }
        if (strcmp(clave, "maximo_vidas_extra") == 0) {
            config->maxVidasExtra = valor;
            seteados |= 32; // Bit: |= 100000
        }
    }

    // Se fija si todos los bits están seteados (todos los valores presentes)
    if (seteados != 63) { // Bits: 111111
        printf("Error de configuracion: faltan parametros en el archivo o estan mal escritos.\n");
        printf("    (tip: se escribe 'clave=valor', separados con signo igual)\n");
        return ERROR_CONFIG;
    }

    // Chequeo de rangos
    if(config->filas < MIN_FILAS || config->columnas < MIN_COLUMNAS) {
        printf("\nTamaño del laberinto inferior al mínimo posible, revise la configuración.");
        return ERROR_CONFIG;
    }
    if(config->filas > MAX_FILAS || config->columnas > MAX_COLUMNAS) {
        printf("\nTamaño del laberinto superior al máximo posible, revise la configuración.");
        return ERROR_CONFIG;
    }
    if(config->columnas <= (config->filas-3))
    {
        printf("\nPor razones de renderizado, no puede haber mas de 3 columnas menos que la cantidad de filas, por favor cambie la configuracion.");
        return ERROR_CONFIG;
    }
    if(config->maxFantasmas < 1 || config->maxPremios < 1) {
        printf("\nDebe haber al menos un fantasma o premio, revise la configuración.");
        return ERROR_CONFIG;
    }
    if(config->maxFantasmas > (config->filas * config->columnas) / 4) {
        printf("\nEl número de fantasmas es demasiado alto para el tamaño del laberinto, revise la configuración.");
        return ERROR_CONFIG;
    }
    if(config->maxPremios > (config->filas * config->columnas) / 4) {
        printf("\nEl número de premios es demasiado alto para el tamaño del laberinto, revise la configuración.");
        return ERROR_CONFIG;
    }
    if(config->maxVidasExtra > (config->filas * config->columnas) / 4) {
        printf("\nEl número de vidas extra es demasiado alto para el tamaño del laberinto, revise la configuración.");
        return ERROR_CONFIG;
    }
    if(config->maxPremios < 0) {
        printf("\nEl número de premios no puede ser un número negativo, revise la configuración.");
        return ERROR_CONFIG;
    }
    if(config->maxVidasExtra < 0) {
        printf("\nEl máximo número de vidas extra no puede ser un número negativo, revise la configuración.");
        return ERROR_CONFIG;
    }
    if(config->vidasIniciales < 1) {
        printf("\nNo puede empezar el juego con menos de 1 vida, revise la configuración.");
        return ERROR_CONFIG;
    }

    fclose(archConfig);

    return TODO_BIEN;
}
