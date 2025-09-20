#include "Macros.h"

int leerConfig(Configuracion *config);

//TABLERO//
void cargarLaberinto(Tablero *laberinto);
void **crear_matriz(int filas, int columnas, unsigned tamElem);
void destruir_matriz(void **matriz, int filas);
void generarLaberinto(Tablero laberinto, int fil, int col);
void mostrar_matriz(char **matriz, int filas, int col);
void inicializarTablero(Tablero laberinto);
void mezclarDirecciones(int dir[4][2]);
void generarSalida(Tablero *laberinto);
int realizarMovimiento(Tablero *laberinto, Jugador *jugador, char direccion);


int main()
{
    Configuracion config;
    Tablero laberinto;
    Jugador jugador;
    int i;
    
    srand(time(NULL));

    if(leerConfig(&config) == 0)
    {
        printf("\nERROR al leer el archivo de configuración.");
        exit(1);
    }

    laberinto.filas = config.filas;
    laberinto.columnas = config.columnas;
    cargarLaberinto(&laberinto);    

    jugador.vidas = config.vidasIniciales;

    //SDL//
    SDL_Window *ventana;
    SDL_Renderer *renderer;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("\nERROR al inicializar la ventana: %s", SDL_GetError());
        exit(1);
    }
    if(TTF_Init() < 0)
    {
        printf("\nERROR al inicializar la ventana: %s", TTF_GetError());
        exit(1);
    }
    
    ventana = SDL_CreateWindow("Laberintos y Fantasmas", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if(!ventana)
    {
        printf("\nERROR al crear la ventana: %s", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    if(!ventana)
        {
            printf("\nERROR al renderizar: %s", SDL_GetError());
            exit(1);
        }



    //LOGICA DE MOVIMIENTO//

    char movimiento;
    int estado = 1;

    jugador.posY = 1;
    jugador.posX = 0;
    laberinto.celdas[jugador.posY][jugador.posX] = 'J';

    while(estado != VICTORIA)
    {
        printf("\n%d vidas.\n", jugador.vidas);
        mostrar_matriz(laberinto.celdas, laberinto.filas, laberinto.columnas);
        printf("\n\nMover (WASD): ");
        scanf(" %c", &movimiento);
        #ifdef WIN_32
            system("cls");
        #else
            system("clear");
        #endif
        
        estado = realizarMovimiento(&laberinto, &jugador, movimiento);

        if(estado == MOV_INVALIDO)
            printf("\nMovimiento inválido.\n");
    }

    printf("\n\nVictoria!");

    destruir_matriz((void **)laberinto.celdas, laberinto.filas);

    printf("\n\n\n\n\n\n");
    return 0;
}

//CONFIGURACION//
int leerConfig(Configuracion *config)
{
    FILE *pf = fopen("config.txt","rt");
    if(!pf)
        return 0;

    fscanf(pf, "%d", &config->columnas);
    fscanf(pf, "%d", &config->filas);
    if(config->filas < MIN_FILAS || config->columnas < MIN_COLUMNAS)
    {
        printf("\nTamaño del laberinto inferior al mínimo posible, revise la configuración.");
        exit(1);
    }
    if(config->filas > MAX_FILAS || config->columnas > MAX_COLUMNAS)
    {
        printf("\nTamaño del laberinto superior al máximo posible, revise la configuración.");
        exit(1);
    }
    fscanf(pf, "%d", &config->maxFantasmas);
    fscanf(pf, "%d", &config->maxPremios);
    if(config->maxFantasmas < 1 || config->maxPremios < 1)
    {
        printf("\nNo puede haber 0 fantasmas o premios, revise la configuración.");
        exit(1);
    }
    fscanf(pf, "%d", &config->maxVidasExtra);
    if(config->maxVidasExtra < 0)
    {
        printf("\nEl máximo número de vidas extra no puede ser un número negativo, revise la configuración.");
        exit(1);
    }
    fscanf(pf, "%d", &config->vidasIniciales);
    if(config->vidasIniciales < 1)
    {
        printf("\nNo puede empezar el juego con menos de 1 vida, revise la configuración.");
        exit(1);
    }
    fclose(pf);

    return 1;
}

//FUNCIONES DE TABLERO//
void cargarLaberinto(Tablero *laberinto)
{
    laberinto->celdas = (char **)crear_matriz(laberinto->filas, laberinto->columnas, sizeof(char));
    inicializarTablero(*laberinto);

    laberinto->celdas[1][1] = ' ';
    generarLaberinto(*laberinto, 1, 1);
    laberinto->celdas[1][0] = 'E';
    generarSalida(laberinto);
}

void **crear_matriz(int filas, int columnas, unsigned tamElem) 
{
    int I, J;
    void **matriz = malloc(filas * sizeof(void *));
    if (!matriz)
        return NULL;

    for ( I = 0; I < filas; I++)
    {
        matriz[I] = malloc(columnas * tamElem);
        if (!matriz[I])
        {
            for (J = 0; J < I; J++)
            {
                free(matriz[J]);
            }
            free(matriz);
            return NULL;
        }
    }
    return matriz;
}

void destruir_matriz(void **matriz, int filas)
{
    int I;

    for (I = 0; I < filas; I++)
    {
        free(matriz[I]);
    }
    free(matriz);
}

void mostrar_matriz(char **matriz, int filas, int col)
{
    int I, J;

    for(I = 0; I < filas; I++)
    {
        for(J = 0; J < col; J++)
        {
            printf("%c", matriz[I][J]);
        }
        printf("\n");
    }
}

void inicializarTablero(Tablero laberinto)
{
    int I, J;

    for(I = 0; I < laberinto.filas; I++)
    {
        for(J = 0; J < laberinto.columnas; J++)
        {
            laberinto.celdas[I][J] = '#';
        }
    }
}

void generarLaberinto(Tablero laberinto, int fil, int col)
{
    int direccion[4][2] = {{-2, 0},{2, 0},{0, -2},{0, 2}}; //Posibles direcciones
    int I, J, nfila, ncol;
    mezclarDirecciones(direccion);

    for(I = 0; I < 4; I++)
    {
        nfila = fil + direccion[I][0];
        ncol = col + direccion[I][1];

        if(nfila > 0 && nfila < laberinto.filas-1 && ncol > 0 && ncol < laberinto.columnas-1 && laberinto.celdas[nfila][ncol] == '#')
        {
            laberinto.celdas[(fil+nfila)/2][(col+ncol)/2] = ' ';
            laberinto.celdas[nfila][ncol] = ' ';
            generarLaberinto(laberinto, nfila, ncol);
        }
    }
}

void mezclarDirecciones(int dir[4][2])
{
    int I, J, temp, temp2;

    for(I = 3; I > 0; I--)
    {
        J = rand() % (I+1);
        temp = dir[I][0];
        temp2 = dir[I][1];
        dir[I][0] = dir[J][0];
        dir[I][1] = dir[J][1];
        dir[J][0] = temp;
        dir[J][1] = temp2;
    }
}

void generarSalida(Tablero *laberinto)
{
    int filas = laberinto->filas;
    int columnas = laberinto->columnas;
    int i, j;

    //Busca la fila desde la que va a colocar la salida
    int filaSalida = 1 + rand() % (filas - 2);

    // Verifica si hay camino hasta la penúltima columna
    if(laberinto->celdas[filaSalida][columnas - 2] != ' ')
    {
        //Si no hay camino, "fuerza" uno desde la última celda libre a la izquierda
        for(j = columnas - 2; j > 0; j--)
        {
            laberinto->celdas[filaSalida][j] = ' ';
        }
    }

    // Pone la salida
    laberinto->celdas[filaSalida][columnas - 1] = 'S';
}




//FUNCIONES DE MOVIMIENTO//

int realizarMovimiento(Tablero *laberinto, Jugador *jugador, char direccion)
{
    int nuevaFila = jugador->posY;
    int nuevaColumna = jugador->posX;

    switch(direccion = tolower(direccion))
    {
        case 'w': nuevaFila--;
                  break;
        case 's': nuevaFila++;
                  break;
        case 'a': nuevaColumna--;
                  break;
        case 'd': nuevaColumna++;
                  break;
        default: return MOV_INVALIDO;
    }

    if(nuevaFila < 0 || nuevaFila >= laberinto->filas || nuevaColumna < 0 || nuevaColumna >= laberinto->columnas)
        return MOV_INVALIDO;

    if(laberinto->celdas[nuevaFila][nuevaColumna] == '#')
        return MOV_INVALIDO;

    if(laberinto->celdas[nuevaFila][nuevaColumna] == 'S')
        return VICTORIA;

    laberinto->celdas[jugador->posY][jugador->posX] = ' ';
    jugador->posY = nuevaFila;
    jugador->posX = nuevaColumna;
    laberinto->celdas[jugador->posY][jugador->posX] = 'J';

    return MOV_VALIDO;
}
