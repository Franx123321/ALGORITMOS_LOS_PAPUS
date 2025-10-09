#define SDL_MAIN_HANDLED
#include "Macros.h"
#include "cola.h"

//CONFIGURACION//
int leerConfig(Configuracion *config);

//SDL//
int inicializarSDL(SDL_Window **ventana, SDL_Renderer **renderer, TTF_Font **fuente, TTF_Font **fuenteHud, 
                    const Configuracion *config, int *ancho, int *alto, int *tamFuente, int *tamFuenteHud);
void destruirSDL(SDL_Window **ventana, SDL_Renderer **renderer, 
                    TTF_Font **fuente, TTF_Font **fuenteHud);
int renderizarTexto(SDL_Renderer *renderer, TTF_Font *fuente, 
                    const char *mensaje, SDL_Color color, int ancho, int alto, int tiempo);
void renderizarHUD(SDL_Renderer *renderer, TTF_Font *fuente, const char *mensaje,
                    SDL_Color color, int x, int y, float escala);
void centrarRenderizado(SDL_Renderer *renderer, TTF_Font *fuente, const char *texto, 
                        SDL_Color color, int ancho, int alto, float escala);
int renderizarTexto(SDL_Renderer *renderer, TTF_Font *fuente, 
                    const char *mensaje, SDL_Color color, int ancho, int alto, int tiempo);


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
void cargarLaberinto(Tablero *laberinto, Fantasma *fantasmas, Configuracion config);
void dibujarTablero(SDL_Renderer *renderer, Tablero *laberinto, Jugador *j, TTF_Font *fuente, int tamCelda, int offsetX, float escalaTexto);

//FANTASMAS//
void generarFantasmas(Tablero *laberinto, Fantasma *fantasmas, int maxFantasmas);
int moverFantasmas(Tablero *laberinto, Fantasma *fantasmas, Jugador *jugador, int maxFantasmas);
int encontrarFantasma(Fantasma *fantasmas, int maxFantasmas, int x, int y);

//PREMIOS Y VIDAS EXTRA//
void colocarVidasExtra(Tablero *laberinto, int vidasExtra);
void colocarPremios(Tablero *laberinto, int maxPremios);

//PARTIDA//
void Jugar(Tablero *laberinto, Jugador *jugador, Fantasma *fantasmas, int maxFantasmas, SDL_Window *ventana,
            SDL_Renderer *renderer, TTF_Font **fuentePtr, TTF_Font **fuenteHudPtr, int tamFuenteBase, 
            int tamFuenteHudBase, int ancho, int alto, tCola *ColaMovimientos);
int realizarMovimiento(Tablero *laberinto, Jugador *jugador, Fantasma *fantasmas, int maxFantasmas, char direccion);
int pantallaInicio(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto);
void victoria(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto, int puntaje);
void derrota(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto);


int main(int argc, char *argv[])
{
    Configuracion config;
    Tablero laberinto;
    Jugador jugador;
    Fantasma *fantasmas;
    SDL_Window *ventana = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *fuente = NULL, *fuenteHud = NULL;
    int ancho, alto, tamFuente, tamFuenteHud;
    tCola ColaMovimientos;

    srand(time(NULL));

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

    //TABLERO//
    //Aca se carga el contenido del laberinto: paredes, caminos, salida, fantasmas, premios y vidas extra
    laberinto.filas = config.filas;
    laberinto.columnas = config.columnas;
    cargarLaberinto(&laberinto, fantasmas, config);

    //JUGADOR//
    jugador.vidas = config.vidasIniciales;
    jugador.posY = 1;
    jugador.posX = 0;
    jugador.puntaje = 0;
    laberinto.celdas[jugador.posY][jugador.posX] = 'J';

    //Aca se guarda la disposicion inicial del laberinto en un archivo de texto
    guardarLaberinto(&laberinto);

    //SDL//   
    if(inicializarSDL(&ventana, &renderer, &fuente, &fuenteHud, &config, &ancho, &alto, &tamFuente, &tamFuenteHud) != TODO_BIEN)
    {
        free(fantasmas);
        destruir_matriz((void **)laberinto.celdas, laberinto.filas);
        exit(1);
    }

    //LOGICA DE JUEGO//
    crearCola(&ColaMovimientos);
    Jugar(&laberinto, &jugador, fantasmas, config.maxFantasmas, ventana, renderer, &fuente, &fuenteHud,
            tamFuente, tamFuenteHud, ancho, alto, &ColaMovimientos);

    //DESTRUCTORES//
    destruir_matriz((void **)laberinto.celdas, laberinto.filas);
    destruirSDL(&ventana, &renderer, &fuente, &fuenteHud);
    free(fantasmas);
    vaciarCola(&ColaMovimientos);


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


//FUNCIONES SDL//
int inicializarSDL(SDL_Window **ventana, SDL_Renderer **renderer, TTF_Font **fuente, TTF_Font **fuenteHud, 
                    const Configuracion *config, int *ancho, int *alto, int *tamFuente, int *tamFuenteHud)
{
    float escalaX, escalaY, escala;
    int base, tamMedio;

    *ancho = config->columnas * TAM_CELDA;
    *alto = config->filas * TAM_CELDA + MARGEN;
    
    if(*ancho < MIN_ANCHO || *alto < MIN_ALTO)
    {
        escalaX = (float)MIN_ANCHO / *ancho;
        escalaY = (float)MIN_ALTO / *alto;
        escala = (escalaX > escalaY ? escalaX : escalaY);

        *ancho = (int)(*ancho * escala);
        *alto = (int)(*alto * escala);
    }
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

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    *ventana = SDL_CreateWindow("Laberintos y Fantasmas", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                *ancho, *alto, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(!*ventana)
    {
        printf("\nERROR al crear la ventana: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return ERROR_SDL;
    }


    *renderer = SDL_CreateRenderer(*ventana, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!*renderer)
    {
        printf("\nERROR al renderizar: %s", SDL_GetError());
        SDL_DestroyWindow(*ventana);
        TTF_Quit();
        SDL_Quit();
        return ERROR_SDL;
    }

    SDL_RenderSetLogicalSize(*renderer, *ancho, *alto);
    SDL_RenderSetIntegerScale(*renderer, SDL_FALSE);
    SDL_SetWindowMinimumSize(*ventana, MIN_ANCHO, MIN_ALTO);

    base = (*ancho < *alto ? *ancho : *alto);
    tamMedio = (config->columnas + config->filas) / 2;

    *tamFuente = (int)((base / (tamMedio * 0.2)));
    *tamFuenteHud = (int)((base / (tamMedio * 1.2)));

    if(*tamFuente < 12) 
        *tamFuente = 12; //Tamaño minimo de fuente para que se lea bien
    if(*tamFuente > 48)
        *tamFuente = 48; //Tamaño maximo de fuente para que no sea gigante
    if(*tamFuenteHud < 8) 
        *tamFuenteHud = 8; //Lo mismo
    if(*tamFuenteHud > 32)
        *tamFuenteHud = 32; //Lo mismisimo

    *fuente = TTF_OpenFont("assets/Sora-Bold.ttf", *tamFuente);
    *fuenteHud = TTF_OpenFont("assets/Sora-Bold.ttf", *tamFuenteHud);
    if(!*fuente || !*fuenteHud)
    {
        printf("\nERROR al cargar una fuente: %s", TTF_GetError());
        SDL_DestroyRenderer(*renderer);
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

    /* Use blended text for higher quality when scaling */
    superficie = TTF_RenderText_Blended(fuente, mensaje, color);
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
    SDL_SetTextureBlendMode(texto, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texto, NULL, &destino);
    SDL_RenderPresent(renderer);

    if(tiempo > 0)
        SDL_Delay(tiempo);

    SDL_DestroyTexture(texto);
    return TODO_BIEN;
}

void renderizarHUD(SDL_Renderer *renderer, TTF_Font *fuente, const char *mensaje, SDL_Color color, int x, int y, float escala)
{
    SDL_Surface *superficie;
    SDL_Texture *texto;
    SDL_Rect destino;
    int ancho_texto, alto_texto;

    /* Renderizar texto con TTF blended para mejor calidad */
    superficie = TTF_RenderText_Blended(fuente, mensaje, color);
    if (!superficie) 
    {
        printf("\nERROR al renderizar un texto: %s", TTF_GetError());
        return;
    }

    texto = SDL_CreateTextureFromSurface(renderer, superficie);
    SDL_FreeSurface(superficie);
    if (!texto) 
    {
        printf("\nERROR al crear una textura: %s", SDL_GetError());
        return;
    }

    SDL_SetTextureBlendMode(texto, SDL_BLENDMODE_BLEND);

    SDL_QueryTexture(texto, NULL, NULL, &ancho_texto, &alto_texto);

    /* Ajustar posición y tamaño según escala requerida */
    destino.w = (int)(ancho_texto * escala);
    destino.h = (int)(alto_texto * escala);
    destino.x = x;
    destino.y = y;

    SDL_RenderCopy(renderer, texto, NULL, &destino);
    SDL_DestroyTexture(texto);
}

void centrarRenderizado(SDL_Renderer *renderer, TTF_Font *fuente, const char *texto, SDL_Color color, int ancho, int alto, float escala)
{
    int ancho_texto, alto_texto, x;

    TTF_SizeText(fuente, texto, &ancho_texto, &alto_texto);
    x = (ancho - (int)(ancho_texto * escala)) / 2;
    renderizarHUD(renderer, fuente, texto, color, x, alto, escala);
}

void destruirSDL(SDL_Window **ventana, SDL_Renderer **renderer, 
                    TTF_Font **fuente, TTF_Font **fuenteHud)
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
void cargarLaberinto(Tablero *laberinto, Fantasma *fantasmas, Configuracion config)
{
    laberinto->celdas = (char **)crear_matriz(laberinto->filas, laberinto->columnas, sizeof(char));
    inicializarTablero(*laberinto);

    laberinto->celdas[1][1] = ' ';
    generarLaberinto(*laberinto, 1, 1);
    agujerearLaberinto(*laberinto);
    laberinto->celdas[1][0] = 'E';
    generarSalida(laberinto);

    generarFantasmas(laberinto, fantasmas, config.maxFantasmas);
    colocarVidasExtra(laberinto, config.maxVidasExtra);
    colocarPremios(laberinto, config.maxPremios);
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
    int I, nfila, ncol;
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
                if (rand() % 10 == 0)
                    laberinto.celdas[I][J + 1] = ' ';

            // Si abajo hay pared y no es borde, tirar dado
            if (laberinto.celdas[I + 1][J] == '#' && I < laberinto.filas - 2)
                if (rand() % 10 == 0)
                    laberinto.celdas[I+ 1][J] = ' ';
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

    //Busca la fila desde la que va a colocar la salida
    int filaSalida = 1 + rand() % ((filas - 1) / 2) * 2;

    // Pone la salida
    laberinto->celdas[filaSalida][columnas - 1] = 'S';

    if (columnas % 2 == 0)
        laberinto->celdas[filaSalida][columnas - 2] = ' ';
}

int guardarLaberinto(Tablero *laberinto)
{
    int I, J;
    FILE *archLaberinto = fopen("laberinto.txt", "wt");
    if (!archLaberinto) 
    {
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

void dibujarTablero(SDL_Renderer *renderer, Tablero *laberinto, Jugador *j, TTF_Font *fuente, 
                    int tamCeldaReal, int offsetX, float escalaTexto)
{
    int I, J;
    char celda, mensajeHUD[50];
    SDL_Rect recta;

    for(I = 0; I < laberinto->filas; I++)
    {
        for(J = 0; J < laberinto->columnas; J++)
        {
            recta.x = offsetX + J * tamCeldaReal;
            recta.y = I * tamCeldaReal + MARGEN;
            recta.w = tamCeldaReal;
            recta.h = tamCeldaReal;

            celda = laberinto->celdas[I][J];

            //Colores para cada cosa, se pueden cambiar cambiando los parametros 2, 3 y 4
            if(celda == '#')
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            else if(celda == 'J')
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            else if(celda == 'S')
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            else if(celda == 'F')
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            else if(celda == 'V')
                SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255); 
            else if(celda == 'P')
                SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
            else
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            SDL_RenderFillRect(renderer, &recta);
        }
    }

    sprintf(mensajeHUD, "Vidas: %d", j->vidas);
    renderizarHUD(renderer, fuente, mensajeHUD, COLOR_AZUL, 2, 3, escalaTexto);
    sprintf(mensajeHUD, "Puntaje: %d", j->puntaje);
    renderizarHUD(renderer, fuente, mensajeHUD, COLOR_AZUL, 200, 3, escalaTexto);
}

//FUNCIONES DE FANTASMAS//
void generarFantasmas(Tablero *laberinto, Fantasma *fantasmas, int maxFantasmas)
{
    int f, x, y;

    for(f = 0; f < maxFantasmas; f++)
    {
        do{
            y = rand() % laberinto->filas;
            x = rand() % laberinto->columnas;
        }while (laberinto->celdas[y][x] != ' ');

        if(laberinto->celdas[y][x] == 'E' || laberinto->celdas[y][x] == 'S')
            f--;
        else
        {
            fantasmas[f].posY = y;
            fantasmas[f].posX = x;
            fantasmas[f].vivo = 1;
        }

        laberinto->celdas[y][x] = 'F';
        fantasmas[f].quePisa = ' ';
    }
}

int moverFantasmas(Tablero *laberinto, Fantasma *fantasmas, Jugador *jugador, int maxFantasmas)
{
    int posibleX[4] = {1, -1, 0, 0},
        posibleY[4] = {0, 0, 1, -1},
        dirX, dirY,
        nuevoX, nuevoY,
        direccion, intento, I;

    for(I = 0; I < maxFantasmas; I++)
    {
        if(!fantasmas[I].vivo)
            continue;
    
        dirX = jugador->posX - fantasmas[I].posX;
        dirY = jugador->posY - fantasmas[I].posY;

        /*A continuacion, basicamente, hay un X% de probabilidad de que el fantasma
          se mueva en direccion al jugador (puede cambiarse este numero), y un (1-X)% de que se mueva random,
          de esta forma el fantasma es inteligente pero tampoco es imposible
          de ganar*/
        if(rand() % 100 < 60)
        {
            if(abs(dirX) > abs(dirY))
                direccion = (dirX > 0) ? 0 : 1; //Derecha o Izquierda
            else
                direccion = (dirY > 0) ? 2 : 3; //Abajo o Arriba
        }
        else
            direccion = rand() % 4;

        //hace 4 intentos de moverse de forma inteligente, si no puede, se mueve random
        for(intento = 0; intento < 4; intento++)
        {
            if(intento > 0) //Si ya intento moverse en la direccion calculada, intenta en otra random
                direccion = rand() % 4;

            nuevoX = fantasmas[I].posX + posibleX[direccion];
            nuevoY = fantasmas[I].posY + posibleY[direccion];

            if(nuevoX >= 0 && nuevoX < laberinto->columnas 
                && nuevoY >= 0 && nuevoY < laberinto->filas 
                && laberinto->celdas[nuevoY][nuevoX] != '#' 
                && laberinto->celdas[nuevoY][nuevoX] != 'S' 
                && laberinto->celdas[nuevoY][nuevoX] != 'E'
                && laberinto->celdas[nuevoY][nuevoX] != 'F')
            {
                laberinto->celdas[fantasmas[I].posY][fantasmas[I].posX] = fantasmas[I].quePisa; 
                fantasmas[I].quePisa = laberinto->celdas[nuevoY][nuevoX]; //Guarda lo que pisa para restaurarlo despues

                fantasmas[I].posX = nuevoX;
                fantasmas[I].posY = nuevoY;
                laberinto->celdas[nuevoY][nuevoX] = 'F';

                if(nuevoX == jugador->posX && nuevoY == jugador->posY)
                {
                    jugador->vidas--;
                    if(jugador->vidas <= 0)
                        return DERROTA;
                    fantasmas[I].vivo = 0;
                    laberinto->celdas[nuevoY][nuevoX] = 'J';
                }

                break;
            }
        }
    }

    return TODO_BIEN;
}   

int encontrarFantasma(Fantasma *fantasmas, int maxFantasmas, int x, int y)
{
    int I;

    for(I = 0; I < maxFantasmas; I++)
    {
        if(fantasmas[I].posX == x && fantasmas[I].posY == y && fantasmas[I].vivo)
            return I;
    }

    return -1;
}


//FUNCIONES DE PREMIOS Y VIDAS//
void colocarVidasExtra(Tablero *laberinto, int vidasExtra)
{
    int colocadas = 0, x, y;

    while(colocadas < vidasExtra)
    {
        y = rand() % laberinto->filas;
        x = rand() % laberinto->columnas;

        if(laberinto->celdas[y][x] == ' ' && laberinto->celdas[y][x] != 'E' && laberinto->celdas[y][x] != 'S'
            && laberinto->celdas[y][x] != 'F' && laberinto->celdas[y][x] != 'J')
        {
            laberinto->celdas[y][x] = 'V';
            colocadas++;
        }
    }
}

void colocarPremios(Tablero *laberinto, int maxPremios)
{
    int colocados = 0, x, y;

    while(colocados < maxPremios)
    {        
        y = rand() % laberinto->filas;
        x = rand() % laberinto->columnas;

        if(laberinto->celdas[y][x] == ' ' && laberinto->celdas[y][x] != 'E' 
            && laberinto->celdas[y][x] != 'S' && laberinto->celdas[y][x] != 'F' 
            && laberinto->celdas[y][x] != 'J' && laberinto->celdas[y][x] != 'V')
        {
            laberinto->celdas[y][x] = 'P';
            colocados++;
        }
    }
}


//FUNCIONES DE PARTIDA//
void Jugar(Tablero *laberinto, Jugador *jugador, Fantasma *fantasmas, int maxFantasmas, SDL_Window *ventana, 
            SDL_Renderer *renderer, TTF_Font **fuentePtr, TTF_Font **fuenteHudPtr, int tamFuenteBase, 
            int tamFuenteHudBase, int ancho, int alto, tCola *ColaMovimientos)
{
    char movimiento = 0, aseguradorMovimiento;
    int estado = 1, jugando = 1, tamCeldaReal, winW, winH, 
        vpW, vpH, totalWidth, offsetX, tamFuenteHudDeseado;
    float escalaX, escalaY, escala, escalaTexto, tamCeldaVista;
    /* Variables para manejar fuentes */
    TTF_Font *fuenteLocal = NULL;
    TTF_Font *fuenteHudOriginal = NULL;
    int tamFuenteHudActual = 0;
    TTF_Font *fuenteHudLocal = NULL;
    SDL_Event evento;

    SDL_GetWindowSize(ventana, &winW, &winH);

    /* Inicializar punteros locales a las fuentes pasadas por referencia */
    if (fuentePtr) 
        fuenteLocal = *fuentePtr; 
    else 
        fuenteLocal = NULL;

    if (fuenteHudPtr) 
        fuenteHudOriginal = *fuenteHudPtr; 
    else 
        fuenteHudOriginal = NULL;

    if(pantallaInicio(ventana, renderer, fuenteHudOriginal, ancho, alto) == SALIR)
        return;

    while(jugando && estado != VICTORIA)
    {
        while(SDL_PollEvent(&evento))
        {
            if(evento.type == SDL_QUIT)
                jugando = 0;
            else if (evento.type == SDL_WINDOWEVENT && evento.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                winH = evento.window.data2;
                escalaX = (float)winW / (float)ancho;
                escalaY = (float)winH / (float)alto;
                escala = (escalaX < escalaY) ? escalaX : escalaY;
                if (escala <= 0.0f) escala = 1.0f;
                vpW = (int)(ancho * escala);
                vpH = (int)(alto * escala);
                SDL_Rect vp = { (winW - vpW) / 2, (winH - vpH) / 2, vpW, vpH };
                SDL_RenderSetViewport(renderer, &vp);
            }
            else if(evento.type == SDL_KEYDOWN)
            {
                switch(evento.key.keysym.sym)
                {
                    case SDLK_w: movimiento = 'w';
                                 if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                    printf("No se pudo realizar un movimiento.");
                                 break;
                    case SDLK_s: movimiento = 's';
                                 if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                    printf("No se pudo realizar un movimiento.");                                 break;
                    case SDLK_a: movimiento = 'a';
                                 if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                    printf("No se pudo realizar un movimiento.");
                                 break;
                    case SDLK_d: movimiento = 'd';
                                 if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                    printf("No se pudo realizar un movimiento.");
                                 break;
                    case SDLK_ESCAPE: jugando = 0;
                                      break;
                    default: movimiento = 'z';
                             if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                    printf("No se pudo realizar un movimiento.");
                             break;
                }
                if(jugando && desencolar(ColaMovimientos, &aseguradorMovimiento, sizeof(char)))
                {
                    estado = realizarMovimiento(laberinto, jugador, fantasmas, maxFantasmas, aseguradorMovimiento);
                    if(estado == DERROTA)
                        jugando = 0;
                    if(estado != MOV_INVALIDO)
                    {
                        if(moverFantasmas(laberinto, fantasmas, jugador, maxFantasmas) == DERROTA)
                        {
                            estado = DERROTA;
                            jugando = 0;
                        }
                    }
                }
            }
        }


        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderClear(renderer);

        tamCeldaReal = (ancho / laberinto->columnas);
        if (tamCeldaReal <= 0) 
            tamCeldaReal = 1;

        totalWidth = tamCeldaReal * laberinto->columnas;
        offsetX = (ancho - totalWidth) / 2;
        if (offsetX < 0) 
            offsetX = 0;

        SDL_Rect vpActual;
        SDL_RenderGetViewport(renderer, &vpActual);
        escalaTexto = 1.0f;
        if (vpActual.w > 0 && laberinto->columnas > 0) 
        {
            tamCeldaVista = (float)vpActual.w / (float)laberinto->columnas;
            escalaTexto = tamCeldaVista / (float)TAM_CELDA;
            if (escalaTexto <= 0.0f) 
                escalaTexto = 1.0f;
        }

        /* Calcular tamaño deseado de fuente para el HUD basado en el tamano de celda visible */
        tamFuenteHudDeseado = (int)(tamCeldaVista * 0.6f);
        if (tamFuenteHudDeseado < 8) 
            tamFuenteHudDeseado = 8;

        if (tamFuenteHudDeseado != tamFuenteHudActual) 
        {
            /* Recrear la fuente HUD local */
            if (fuenteHudLocal)
            {
                TTF_CloseFont(fuenteHudLocal);
                fuenteHudLocal = NULL;
            }
            fuenteHudLocal = TTF_OpenFont("assets/Sora-Bold.ttf", tamFuenteHudDeseado);
            if (!fuenteHudLocal) 
                /* Si falla, mantenemos la fuente original */
                fuenteHudLocal = fuenteHudOriginal;
            else
                tamFuenteHudActual = tamFuenteHudDeseado;
        }

        /* Usar la fuente HUD local si existe, sino la pasada por parametro */
        TTF_Font *fuenteParaHUD = (fuenteHudLocal ? fuenteHudLocal : fuenteHudOriginal);

        dibujarTablero(renderer, laberinto, jugador, fuenteParaHUD, tamCeldaReal, offsetX, escalaTexto);

        SDL_RenderPresent(renderer);

        SDL_Delay(1000 / 60);
    }

    if(estado == VICTORIA) 
        victoria(ventana, renderer, fuenteLocal, ancho, alto, jugador->puntaje);
    else if(estado == DERROTA)
        derrota(ventana, renderer, fuenteLocal, ancho, alto);

    /* Liberar la fuente HUD local si fue creada */
    if (fuenteHudLocal && fuenteHudLocal != fuenteHudOriginal) 
    {
        TTF_CloseFont(fuenteHudLocal);
        fuenteHudLocal = NULL;
    }
}

int realizarMovimiento(Tablero *laberinto, Jugador *jugador, Fantasma *fantasmas, int maxFantasmas, char direccion)
{
    int nuevaFila = jugador->posY,
        nuevaColumna = jugador->posX,
        fantasmaBuscado,
        puntosGanados;

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

    if(nuevaFila < 0 || nuevaFila >= laberinto->filas 
        || nuevaColumna < 0 || nuevaColumna >= laberinto->columnas)
        return MOV_INVALIDO;

    if(laberinto->celdas[nuevaFila][nuevaColumna] == '#')
        return MOV_INVALIDO;

    if(laberinto->celdas[nuevaFila][nuevaColumna] == 'S')
        return VICTORIA;

    if(laberinto->celdas[nuevaFila][nuevaColumna] == 'F')
    {
        fantasmaBuscado = encontrarFantasma(fantasmas, maxFantasmas, nuevaColumna, nuevaFila);
        if(fantasmaBuscado != -1)
            fantasmas[fantasmaBuscado].vivo = 0;

        jugador->vidas--;
        if(jugador->vidas <= 0)
            return DERROTA;
    }

    if(laberinto->celdas[nuevaFila][nuevaColumna] == 'V')
        jugador->vidas++;

    if(laberinto->celdas[nuevaFila][nuevaColumna] == 'P')
    {
        puntosGanados = MIN_PUNTOS + rand() % (MAX_PUNTOS - MIN_PUNTOS + 1);
        jugador->puntaje += puntosGanados;
    }


    laberinto->celdas[jugador->posY][jugador->posX] = ' ';
    jugador->posY = nuevaFila;
    jugador->posX = nuevaColumna;
    laberinto->celdas[jugador->posY][jugador->posX] = 'J';

    return MOV_VALIDO;
}

int pantallaInicio(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto)
{
    SDL_Event evento;
    int continuar = 0,
        y = 120,
        salto = 40;
    SDL_Rect vp;
    float escalaTexto = 1.0f;

    /* Calcular escala de texto basada en el viewport actual (relacion ventana/logico) */
    SDL_RenderGetViewport(renderer, &vp);
    if (vp.w > 0 && ancho > 0) 
    {
        escalaTexto = (float)vp.w / (float)ancho;
        if (escalaTexto <= 0.0f) 
            escalaTexto = 1.0f;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 80, 255);
    SDL_RenderClear(renderer);

    centrarRenderizado(renderer, fuente, "LABERINTOS Y FANTASMAS", COLOR_AMARILLO, ancho, y, escalaTexto);
    y += 2 * salto;
    
    centrarRenderizado(renderer, fuente, "CONTROLES:", COLOR_BLANCO, ancho, y, escalaTexto);
    y += salto;
    centrarRenderizado(renderer, fuente, "WASD - Moverse", COLOR_BLANCO, ancho, y, escalaTexto);   
    y += salto;   
    centrarRenderizado(renderer, fuente, "ESC - Salir", COLOR_BLANCO, ancho, y, escalaTexto);
    y += 2 * salto;

    centrarRenderizado(renderer, fuente, "OBJETIVO:", COLOR_BLANCO, ancho, y, escalaTexto);
    y += salto;
    centrarRenderizado(renderer, fuente, "Llegar a la salida (azul) evitando a los fantasmas (rojo)", COLOR_BLANCO, ancho, y, escalaTexto);
    y += salto;
    centrarRenderizado(renderer, fuente, "y acumulando la mayor cantidad de puntos (dorado) posibles.", COLOR_BLANCO, ancho, y, escalaTexto);
    y += salto;
    centrarRenderizado(renderer, fuente, "Recoge vidas extra (violeta) para sobrevivir mas tiempo.", COLOR_BLANCO, ancho, y, escalaTexto);
    y += 2 * salto;
    centrarRenderizado(renderer, fuente, "Presiona ENTER para comenzar...", COLOR_GRIS, ancho, alto - 80, escalaTexto);

    SDL_RenderPresent(renderer);

    while(!continuar)
    {
        while(SDL_PollEvent(&evento))
        {
            if(evento.type == SDL_QUIT)
                return SALIR;
            if(evento.type == SDL_KEYDOWN)
            {
                if(evento.key.keysym.sym == SDLK_RETURN)
                    continuar = 1;
                else if(evento.key.keysym.sym == SDLK_ESCAPE)
                    return SALIR;
            }
        }
        SDL_Delay(10);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    return TODO_BIEN;
}

void victoria(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto, int puntaje)
{
    SDL_Event evento;
    int salir = 0;
    char puntajeTexto[50];
    SDL_Rect vp;
    float escalaTexto = 1.0f;

    SDL_RenderGetViewport(renderer, &vp);
    if (vp.w > 0 && ancho > 0) 
    {
        escalaTexto = (float)vp.w / (float)ancho;
        if (escalaTexto <= 0.0f) 
            escalaTexto = 1.0f;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 80, 255);
    SDL_RenderClear(renderer);

    sprintf(puntajeTexto, "Puntaje obtenido: %d", puntaje);
    centrarRenderizado(renderer, fuente, "Victoria!", COLOR_BLANCO, ancho, alto / 2 - 20, escalaTexto);
    centrarRenderizado(renderer, fuente, puntajeTexto, COLOR_AMARILLO, ancho, alto / 2 + 40, escalaTexto);
    centrarRenderizado(renderer, fuente, "Presiona ESC para salir...", COLOR_GRIS, ancho, alto - 80, escalaTexto);

    SDL_RenderPresent(renderer);

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

void derrota(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto)
{
    SDL_Event evento;
    int salir = 0;
    SDL_Rect vp;
    float escalaTexto = 1.0f;

    SDL_RenderGetViewport(renderer, &vp);
    if (vp.w > 0 && ancho > 0) 
    {
        escalaTexto = (float)vp.w / (float)ancho;
        if (escalaTexto <= 0.0f) 
            escalaTexto = 1.0f;
    }

    SDL_SetRenderDrawColor(renderer, 60, 0, 0, 255);
    SDL_RenderClear(renderer);

    centrarRenderizado(renderer, fuente, "Game over", COLOR_ROJO, ancho, alto / 2 - 20, escalaTexto);
    centrarRenderizado(renderer, fuente, "No se obtendran puntos.", COLOR_ROJO, ancho, alto / 2 + 40, escalaTexto);
    centrarRenderizado(renderer, fuente, "Presiona ESC para salir...", COLOR_GRIS, ancho, alto - 80, escalaTexto);

    SDL_RenderPresent(renderer);

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