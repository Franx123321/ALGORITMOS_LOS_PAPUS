#define SDL_MAIN_HANDLED
#include "Macros.h"

//CONFIGURACION//
int leerConfig(Configuracion *config);

//SDL//
int inicializarSDL(SDL_Window **ventana, SDL_Renderer **renderer, int ancho, int alto);
void destruirSDL(SDL_Window **ventana, SDL_Renderer **renderer, TTF_Font **fuente);
int renderizarTexto(SDL_Renderer *renderer, TTF_Font *fuente, const char *mensaje, SDL_Color color, int ancho, int alto, int tiempo);

//TABLERO//

void **crear_matriz(int filas, int columnas, unsigned tamElem);
void destruir_matriz(void **matriz, int filas);
void mostrar_matriz(char **matriz, int filas, int col);
void inicializarTablero(Tablero laberinto);
void generarLaberinto(Tablero laberinto, int fil, int col);
void agujerearLaberinto(Tablero laberinto);
void mezclarDirecciones(int dir[4][2]);
void generarSalida(Tablero *laberinto);
int guardarLaberinto(Tablero *laberinto);
void cargarLaberinto(Tablero *laberinto);
void dibujarTablero(SDL_Renderer *renderer, Tablero *laberinto);

//PARTIDA//
void Jugar(Tablero *laberinto, Jugador *jugador, SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto);
int realizarMovimiento(Tablero *laberinto, Jugador *jugador, char direccion);
void victoria(SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto);


int main(int argc, char *argv[])
{
    Configuracion config;
    Tablero laberinto;
    Jugador jugador;
    SDL_Window *ventana = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *fuente;
    int ancho,
        alto;

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
    jugador.posY = 1;
    jugador.posX = 0;
    laberinto.celdas[jugador.posY][jugador.posX] = 'J';

    guardarLaberinto(&laberinto);

    //SDL//
    ancho = laberinto.columnas * TAM_CELDA;
    alto = laberinto.filas * TAM_CELDA;
    inicializarSDL(&ventana, &renderer, ancho, alto);
    fuente = TTF_OpenFont("assets/Sora-Bold.ttf", 48);
    if(!fuente)
    {
        printf("\nERROR al cargar fuente: %s", TTF_GetError());
        exit(1);
    }

    //LOGICA DE JUEGO//
    Jugar(&laberinto, &jugador, renderer, fuente, ancho, alto);

    //DESTRUCTORES//
    destruir_matriz((void **)laberinto.celdas, laberinto.filas);
    destruirSDL(&ventana, &renderer, &fuente);


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
    if(config->maxFantasmas < 1 || config->maxPremios < 1) {
        printf("\nNo puede haber 0 fantasmas o premios, revise la configuración.");
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


//FUNCIONES SDL//
int inicializarSDL(SDL_Window **ventana, SDL_Renderer **renderer, int ancho, int alto)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("\nERROR al inicializar la ventana: %s", SDL_GetError());
        return ERROR_SDL;
    }
    if(TTF_Init() < 0)
    {
        printf("\nERROR al inicializar la ventana: %s", TTF_GetError());
        SDL_Quit();
        return ERROR_SDL;
    }

    *ventana = SDL_CreateWindow("Laberintos y Fantasmas", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ancho, alto, SDL_WINDOW_SHOWN);
    if(!*ventana)
    {
        printf("\nERROR al crear la ventana: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return ERROR_SDL;
    }

    *renderer = SDL_CreateRenderer(*ventana, -1, SDL_RENDERER_ACCELERATED);
    if(!*renderer)
    {
        printf("\nERROR al renderizar: %s", SDL_GetError());
        SDL_DestroyWindow(*ventana);
        TTF_Quit();
        SDL_Quit();
        return ERROR_SDL;
    }

    return TODO_BIEN;
}

int renderizarTexto(SDL_Renderer *renderer, TTF_Font *fuente, const char *mensaje, SDL_Color color, int ancho, int alto, int tiempo)
{
    SDL_Rect destino;
    SDL_Surface *superficie;
    SDL_Texture *texto;

    superficie = TTF_RenderText_Solid(fuente, mensaje, color);
    if(!superficie)
    {
        printf("\nERROR al renderizar un texto: %s", SDL_GetError());
        return ERROR_SDL;
    }
    texto = SDL_CreateTextureFromSurface(renderer, superficie);
    if(!texto)
    {
        printf("\nERROR al crear una textura: %s", SDL_GetError());
        SDL_FreeSurface(superficie);
        return ERROR_SDL;
    }


    destino.w = superficie->w;
    destino.h = superficie->h;
    destino.x = (ancho - destino.w) / 2;
    destino.y = (alto - destino.h) / 2;

    SDL_FreeSurface(superficie);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texto, NULL, &destino);
    SDL_RenderPresent(renderer);

    if(tiempo > 0)
        SDL_Delay(tiempo);

    SDL_DestroyTexture(texto);
    return TODO_BIEN;
}

void destruirSDL(SDL_Window **ventana, SDL_Renderer **renderer, TTF_Font **fuente)
{
    if(*fuente)
    {
        TTF_CloseFont(*fuente);
        *fuente = NULL;
    }
    if(*renderer)
    {
        SDL_DestroyRenderer(*renderer);
        *renderer = NULL;
    }
    if(*ventana)
    {
        SDL_DestroyWindow(*ventana);
        *ventana = NULL;
    }
    TTF_Quit();
    SDL_Quit();
}


//FUNCIONES DE TABLERO//
void cargarLaberinto(Tablero *laberinto)
{
    laberinto->celdas = (char **)crear_matriz(laberinto->filas, laberinto->columnas, sizeof(char));
    inicializarTablero(*laberinto);

    laberinto->celdas[1][1] = ' ';
    generarLaberinto(*laberinto, 1, 1);
    agujerearLaberinto(*laberinto);
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

void agujerearLaberinto(Tablero laberinto)
{
    int I, J;

    for(I = 1; I < laberinto.filas - 1; I += 2)
    {
        for(J = 1; J < laberinto.columnas - 1; J += 2)
        {
            // Si a la derecha hay pared y no es borde, tirar dado
            if (laberinto.celdas[I][J + 1] == '#' && J < laberinto.columnas - 2)
            {
                if (rand() % 10 == 0)
                {
                    laberinto.celdas[I][J + 1] = ' ';
                }
            }

            // Si abajo hay pared y no es borde, tirar dado
            if (laberinto.celdas[I + 1][J] == '#' && I < laberinto.filas - 2)
            {
                if (rand() % 10 == 0)
                {
                    laberinto.celdas[I+ 1][J] = ' ';
                }
            }
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
    //int i, j;

    //Busca la fila desde la que va a colocar la salida
    int filaSalida = 1 + rand() % ((filas - 1) / 2) * 2;

    // Pone la salida
    laberinto->celdas[filaSalida][columnas - 1] = 'S';

    if (columnas % 2 == 0) {
        laberinto->celdas[filaSalida][columnas - 2] = ' ';
    }
}

int guardarLaberinto(Tablero *laberinto)
{
    int I, J;
    FILE *archLaberinto = fopen("laberinto.txt", "wt");
    if (!archLaberinto) {
        perror("Error al abrir archivo de laberinto");
        return ERROR_ARCH;
    }

    for(I = 0; I < laberinto->filas; I++)
    {
        for(J = 0; J < laberinto->columnas; J++)
        {
            fputc(laberinto->celdas[I][J], archLaberinto);
        }
        fputc('\n', archLaberinto);
    }

    fclose(archLaberinto);

    return TODO_BIEN;
}

void dibujarTablero(SDL_Renderer *renderer, Tablero *laberinto)
{
    int I, J;

    char celda;
    SDL_Rect recta;

    for(I = 0; I < laberinto->filas; I++)
    {
        for(J = 0; J < laberinto->columnas; J++)
        {
            recta.x = J * TAM_CELDA;
            recta.y = I * TAM_CELDA;
            recta.w = TAM_CELDA;
            recta.h = TAM_CELDA;

            celda = laberinto->celdas[I][J];

            //Colores para cada cosa, se pueden cambiar cambiando los parametros 2, 3 y 4
            if(celda == '#')
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            else if(celda == 'J')
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            else if(celda == 'S')
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            else
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            SDL_RenderFillRect(renderer, &recta);
        }
    }
}


//FUNCIONES DE PARTIDA//
void Jugar(Tablero *laberinto, Jugador *jugador, SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto)
{
    char movimiento = 0;
    int estado = 1, jugando = 1;
    SDL_Event evento;

    while(jugando && estado != VICTORIA)
    {
        while(SDL_PollEvent(&evento))
        {
            if(evento.type == SDL_QUIT)
                jugando = 0;
            else if(evento.type == SDL_KEYDOWN)
            {
                switch(evento.key.keysym.sym)
                {
                    case SDLK_w: movimiento = 'w';
                                 break;
                    case SDLK_s: movimiento = 's';
                                 break;
                    case SDLK_a: movimiento = 'a';
                                 break;
                    case SDLK_d: movimiento = 'd';
                                 break;
                    default: movimiento = 0;
                             break;
                }
                if(movimiento)
                    estado = realizarMovimiento(laberinto, jugador, movimiento);
            }
        }

        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderClear(renderer);

        dibujarTablero(renderer, laberinto);

        SDL_RenderPresent(renderer);

        SDL_Delay(1000 / 60);
    }

    if(estado == VICTORIA)
        victoria(renderer, fuente, ancho, alto);
}

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

void victoria(SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto)
{
    SDL_Event evento;
    int salir = 0;

    renderizarTexto(renderer, fuente, "!Victoria!", COLOR_BLANCO, ancho, alto, 0);

    while(!salir)
    {
        while(SDL_PollEvent(&evento))
        {
            if(evento.type == SDL_QUIT)
                salir = 1;
            else if(evento.type == SDL_KEYDOWN && evento.key.keysym.sym == SDLK_ESCAPE)
                salir = 1;
        }
        SDL_Delay(50);
    }
}
