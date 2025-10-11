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
char **crear_matriz(int filas, int columnas);
void destruir_matriz(void **matriz, int filas);
void mostrar_matriz(char **matriz, int filas, int col);
int inicializarTablero(Tablero *laberinto);
int generarLaberinto(Tablero *laberinto, int fil, int col);
int agujerearLaberinto(Tablero *laberinto);
void mezclarDirecciones(int dir[4][2]);
int generarSalida(Tablero *laberinto);
int guardarLaberinto(Tablero *laberinto);
int cargarLaberinto(Tablero *laberinto, Fantasma *fantasmas, Configuracion config);
void dibujarTablero(SDL_Renderer *renderer, Tablero *laberinto, Jugador *j, TTF_Font *fuente, int tamCelda, int offsetX, float escalaTexto);

//FANTASMAS//
int generarFantasmas(Tablero *laberinto, Fantasma *fantasmas, int maxFantasmas);
int moverFantasmas(Tablero *laberinto, Fantasma *fantasmas, Jugador *jugador, int maxFantasmas);
int encontrarFantasma(Fantasma *fantasmas, int maxFantasmas, int x, int y);

//PREMIOS Y VIDAS EXTRA//
int colocarVidasExtra(Tablero *laberinto, int vidasExtra);
int colocarPremios(Tablero *laberinto, int maxPremios);

//PARTIDA//
int menu(SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto);
int Jugar(Tablero *laberinto, Jugador *jugador, Fantasma *fantasmas, int maxFantasmas, SDL_Window *ventana,
            SDL_Renderer *renderer, TTF_Font **fuentePtr, TTF_Font **fuenteHudPtr, int tamFuenteBase,
            int tamFuenteHudBase, int ancho, int alto, tCola *ColaMovimientos);
int realizarMovimiento(Tablero *laberinto, Jugador *jugador, Fantasma *fantasmas, int maxFantasmas, char direccion);
void victoria(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto, int puntaje);
void derrota(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto);


int main()
{
    Configuracion config;
    Tablero laberinto;
    Jugador jugador;
    Fantasma *fantasmas;
    SDL_Window *ventana = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *fuente = NULL, *fuenteHud = NULL;
    int ancho, alto, tamFuente, tamFuenteHud, opMenu;
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
    laberinto.celdas = NULL;
    laberinto.filas = config.filas;
    laberinto.columnas = config.columnas;
    if(cargarLaberinto(&laberinto, fantasmas, config) != TODO_BIEN)
    {
        printf("\nError al cargar el laberinto.");
        free(fantasmas);
        exit(1);
    }


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
    opMenu = menu(renderer, fuente, ancho, alto);
    if(opMenu == 3)
    {
        printf("\nSe selecciono salir");
        destruir_matriz((void **)laberinto.celdas, laberinto.filas);
        destruirSDL(&ventana, &renderer, &fuente, &fuenteHud);
        free(fantasmas);
        exit(1);
    }
    else if(opMenu == 2) //TEMPORAL
    {
        destruir_matriz((void **)laberinto.celdas, laberinto.filas);
        destruirSDL(&ventana, &renderer, &fuente, &fuenteHud);
        free(fantasmas);
        exit(1);
    }
    else if(opMenu == 1)
    {
        if(Jugar(&laberinto, &jugador, fantasmas, config.maxFantasmas, ventana, renderer, &fuente, &fuenteHud,
                tamFuente, tamFuenteHud, ancho, alto, &ColaMovimientos) != TODO_BIEN)
        {
            printf("\nSe produjo un error durante el juego.");
            vaciarCola(&ColaMovimientos);
            destruir_matriz((void **)laberinto.celdas, laberinto.filas);
            destruirSDL(&ventana, &renderer, &fuente, &fuenteHud);
            free(fantasmas);
            exit(1);
        }
    }


    //DESTRUCTORES//
    vaciarCola(&ColaMovimientos);
    destruir_matriz((void **)laberinto.celdas, laberinto.filas);
    destruirSDL(&ventana, &renderer, &fuente, &fuenteHud);
    free(fantasmas);



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



//FUNCIONES SDL//
int inicializarSDL(SDL_Window **ventana, SDL_Renderer **renderer, TTF_Font **fuente, TTF_Font **fuenteHud,
                    const Configuracion *config, int *ancho, int *alto, int *tamFuente, int *tamFuenteHud)
{
    float escalaX, escalaY, escala;
    int base, tamMedio;

    if(!config || !ancho || !alto || !tamFuente || !tamFuenteHud)
    {
        printf("\nArgumentos invalidos.");
        return ERROR_SDL;
    }

    *ancho = config->columnas * TAM_CELDA;
    *alto = config->filas * TAM_CELDA + MARGEN;

    if(*ancho < MIN_ANCHO || *alto < MIN_ALTO)
    {
        escalaX = (float)MIN_ANCHO / (float)*ancho;
        escalaY = (float)MIN_ALTO / (float)*alto;
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

    if(*ancho > 0 && *alto > 0)
        SDL_RenderSetLogicalSize(*renderer, *ancho, *alto);    

    SDL_RenderSetIntegerScale(*renderer, SDL_FALSE);
    SDL_SetWindowMinimumSize(*ventana, MIN_ANCHO, MIN_ALTO);

    base = (*ancho < *alto ? *ancho : *alto);
    tamMedio = (config->columnas + config->filas) / 2;
    if(tamMedio <= 0)
        tamMedio = 1;

    *tamFuente = (int)((base / (tamMedio * 0.2)));
    *tamFuenteHud = (int)((base / (tamMedio * 1.8)));

    if(*tamFuente < 12)
        *tamFuente = 12; //Tamaño minimo de fuente para que se lea bien
    if(*tamFuente > 48)
        *tamFuente = 48; //Tamaño maximo de fuente para que no sea gigante
    if(*tamFuenteHud < 8)
        *tamFuenteHud = 8; //Lo mismo
    if(*tamFuenteHud > 32)
        *tamFuenteHud = 32; //Lo mismisimo

    *fuente = TTF_OpenFont("assets/Sora-Bold.ttf", *tamFuente);
    if(!*fuente)
    {
        printf("\nERROR al cargar una fuente: %s", TTF_GetError());
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*ventana);
        TTF_Quit();
        SDL_Quit();
        return ERROR_SDL;
    }

    *fuenteHud = TTF_OpenFont("assets/Sora-Bold.ttf", *tamFuenteHud);
    if(!*fuenteHud)
    {
        printf("\nERROR al cargar una fuente: %s", TTF_GetError());
        TTF_CloseFont(*fuente);
        *fuente = NULL;
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
    SDL_Surface *superficie = NULL;
    SDL_Texture *texto = NULL;

    if(!renderer || !fuente || !mensaje)
        return ERROR_SDL;

    superficie = TTF_RenderText_Blended(fuente, mensaje, color);
    if(!superficie)
    {
        printf("\nERROR al renderizar un texto: %s", TTF_GetError());
        return ERROR_SDL;
    }

    texto = SDL_CreateTextureFromSurface(renderer, superficie);
    if(!texto)
    {
        printf("\nERROR al crear una textura: %s", SDL_GetError());
        SDL_FreeSurface(superficie);
        return ERROR_SDL;
    }


    destino.w = superficie->w > 0 ? superficie->w : 1;
    destino.h = superficie->h > 0 ? superficie->h : 1;
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
    SDL_Surface *superficie = NULL;
    SDL_Texture *texto = NULL;
    SDL_Rect destino;
    int ancho_texto = 0, alto_texto = 0;

    if(!fuente || !mensaje || !renderer)
        return;


    //Renderizar texto con TTF blended para mejor calidad
    if(superficie)
        SDL_FreeSurface(superficie);
    superficie = TTF_RenderText_Blended(fuente, mensaje, color);

    texto = SDL_CreateTextureFromSurface(renderer, superficie);
    SDL_FreeSurface(superficie);
    if (!texto)
    {
        printf("\nERROR al crear una textura: %s", SDL_GetError());
        return;
    }

    SDL_SetTextureBlendMode(texto, SDL_BLENDMODE_BLEND);

    SDL_QueryTexture(texto, NULL, NULL, &ancho_texto, &alto_texto);

    //Ajustar posición y tamaño según escala requerida
    if(escala <= 0)
        escala = 1;

    destino.w = (int)(ancho_texto * escala);
    destino.h = (int)(alto_texto * escala);

    if(destino.w <= 0)
        destino.w = 1;
    if(destino.h <= 0)
        destino.h = 1;

    destino.x = x;
    destino.y = y;

    SDL_RenderCopy(renderer, texto, NULL, &destino);
    SDL_DestroyTexture(texto);
}

void centrarRenderizado(SDL_Renderer *renderer, TTF_Font *fuente, const char *texto, SDL_Color color, int ancho, int alto, float escala)
{
    int ancho_texto = 0, alto_texto = 0, x, y;

    if(!fuente || !texto)
        return;

    TTF_SizeText(fuente, texto, &ancho_texto, &alto_texto);

    if(ancho_texto <= 0)
        ancho_texto = 1;

    x = (ancho - (int)(ancho_texto * escala)) / 2;
    y = (alto - (int)(alto_texto * escala)) / 2;

    renderizarHUD(renderer, fuente, texto, color, x, y, escala);
}

void destruirSDL(SDL_Window **ventana, SDL_Renderer **renderer,
                    TTF_Font **fuente, TTF_Font **fuenteHud)
{
    if(fuente && *fuente)
    {
        TTF_CloseFont(*fuente);
        *fuente = NULL;
    }
    if(fuenteHud && *fuenteHud)
    {
        TTF_CloseFont(*fuenteHud);
        *fuenteHud = NULL;
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
int cargarLaberinto(Tablero *laberinto, Fantasma *fantasmas, Configuracion config)
{
    if(!laberinto)
        return ERROR_MEMORIA;
    
    if(laberinto->filas <= 0 || laberinto->columnas <= 0)
        return ERROR_MEMORIA;
    
    laberinto->celdas = crear_matriz(laberinto->filas, laberinto->columnas);
    if(!laberinto->celdas)
        return ERROR_MEMORIA;

    if(inicializarTablero(laberinto) != TODO_BIEN)
    {
        destruir_matriz((void **)laberinto->celdas, laberinto->filas);
        return ERROR_MEMORIA;
    }

    laberinto->celdas[1][1] = ' ';
    if(generarLaberinto(laberinto, 1, 1) != TODO_BIEN)
    {
        printf("\nError al inicializar el laberinto.");
        destruir_matriz((void **)laberinto->celdas, laberinto->filas);
        return ERROR_MEMORIA;
    }

    if(agujerearLaberinto(laberinto) != TODO_BIEN)
    {
        printf("\nError al agujerear el laberinto.");
        destruir_matriz((void **)laberinto->celdas, laberinto->filas);
        return ERROR_MEMORIA;
    }

    laberinto->celdas[1][0] = 'E';

    if(generarSalida(laberinto) != TODO_BIEN)
    {
        printf("\nError al generar la salida.");
        destruir_matriz((void **)laberinto->celdas, laberinto->filas);
        return ERROR_MEMORIA;
    }

    if(generarFantasmas(laberinto, fantasmas, config.maxFantasmas) == ERROR_GENERACION)
    {
        printf("\nError generando fantasmas.");
        return ERROR_GENERACION;
    }
    
    if(colocarVidasExtra(laberinto, config.maxVidasExtra) == ERROR_GENERACION)
    {
        printf("\nError generando vidas extra.");
        return ERROR_GENERACION;
    }

    if(colocarPremios(laberinto, config.maxPremios) == ERROR_GENERACION)
    {
        printf("\nError generando premios.");
        return ERROR_GENERACION;
    }

    return TODO_BIEN;
}

char **crear_matriz(int filas, int columnas)
{
    int I, J;

    if(filas <= 0 || columnas <= 0)
        return NULL;

    char **matriz = (char **)malloc(filas * sizeof(char *));
    if (!matriz)
        return NULL;

    for ( I = 0; I < filas; I++)
    {
        matriz[I] = malloc(columnas * sizeof(char));
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

    if(!matriz)
        return;

    for (I = 0; I < filas; I++)
    {
        free(matriz[I]);
    }
    free(matriz);
}

void mostrar_matriz(char **matriz, int filas, int col)
{
    int I, J;

    if(!matriz)
        return;

    for(I = 0; I < filas; I++)
    {
        for(J = 0; J < col; J++)
        {
            printf("%c", matriz[I][J]);
        }
        printf("\n");
    }
}

int inicializarTablero(Tablero *laberinto)
{
    int I, J;

    if(!laberinto || !laberinto->celdas)
        return ERROR_MEMORIA;

    for(I = 0; I < laberinto->filas; I++)
    {
        for(J = 0; J < laberinto->columnas; J++)
        {
            laberinto->celdas[I][J] = '#';
        }
    }

    return TODO_BIEN;
}

int generarLaberinto(Tablero *laberinto, int fil, int col)
{
    int direccion[4][2] = {{-2, 0},{2, 0},{0, -2},{0, 2}}; //Posibles direcciones
    int I, nfila, ncol, resultado;

    if(!laberinto || !laberinto->celdas)
        return ERROR_MEMORIA;

    mezclarDirecciones(direccion);

    for(I = 0; I < 4; I++)
    {
        nfila = fil + direccion[I][0];
        ncol = col + direccion[I][1];

        if(nfila > 0 && nfila < laberinto->filas-1 && ncol > 0 && ncol < laberinto->columnas-1 && laberinto->celdas[nfila][ncol] == '#')
        {
            laberinto->celdas[(fil+nfila)/2][(col+ncol)/2] = ' ';
            laberinto->celdas[nfila][ncol] = ' ';
            resultado = generarLaberinto(laberinto, nfila, ncol);
            if(resultado != TODO_BIEN)
                return resultado;
        }
    }

    return TODO_BIEN;
}

int agujerearLaberinto(Tablero *laberinto)
{
    int I, J;

    if(!laberinto || !laberinto->celdas)
        return ERROR_MEMORIA;

    for(I = 1; I < laberinto->filas - 1; I += 2)
    {
        for(J = 1; J < laberinto->columnas - 1; J += 2)
        {
            // Si a la derecha hay pared y no es borde, tirar dado
            if (laberinto->celdas[I][J + 1] == '#' && J < laberinto->columnas - 2)
                if (rand() % 10 == 0)
                    laberinto->celdas[I][J + 1] = ' ';

            // Si abajo hay pared y no es borde, tirar dado
            if (laberinto->celdas[I + 1][J] == '#' && I < laberinto->filas - 2)
                if (rand() % 10 == 0)
                    laberinto->celdas[I+ 1][J] = ' ';
        }
    }

    return TODO_BIEN;
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

int  generarSalida(Tablero *laberinto)
{
    int filas = laberinto->filas,
        columnas = laberinto->columnas,
        filaSalida;

    if(!laberinto || !laberinto->celdas)
        return ERROR_MEMORIA;

    if(filas < 3 || columnas < 3)
        return ERROR_MEMORIA;

    //Busca la fila desde la que va a colocar la salida
    filaSalida = 1 + rand() % ((filas - 1) / 2) * 2;

    // Pone la salida
    laberinto->celdas[filaSalida][columnas - 1] = 'S';

    if (columnas % 2 == 0)
        laberinto->celdas[filaSalida][columnas - 2] = ' ';

    return TODO_BIEN;
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

    if(!laberinto || !laberinto->celdas)
        return ERROR_MEMORIA;

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

    if(!laberinto || !laberinto->celdas || !renderer || !fuente)
        return;

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
            switch (celda)
            {
                case '#': SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                          break;
                case 'J': SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                          break;
                case 'S': SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                          break;
                case 'F': SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                          break;
                case 'V': SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255);
                          break;
                case 'P': SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
                          break;
                default:  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                          break;
            }

            SDL_RenderFillRect(renderer, &recta);
        }
    }

    sprintf(mensajeHUD, "Vidas: %d", j->vidas);
    renderizarHUD(renderer, fuente, mensajeHUD, COLOR_AZUL, 2, 3, escalaTexto);
    sprintf(mensajeHUD, "Puntaje: %d", j->puntaje);
    renderizarHUD(renderer, fuente, mensajeHUD, COLOR_AZUL, 200, 3, escalaTexto);
}



//FUNCIONES DE FANTASMAS//
int generarFantasmas(Tablero *laberinto, Fantasma *fantasmas, int maxFantasmas)
{
    int f, x, y, I, valido, intentos;
    const int minDistanciaES = 3,
              minDistanciaFantasmas = 4;

    if (!laberinto || !laberinto->celdas || !fantasmas)
        return ERROR_MEMORIA;

    for(f = 0; f < maxFantasmas; f++)
    {
        intentos = 0;
        do{
            I = 0;
            valido = 1;

            y = rand() % laberinto->filas;
            x = rand() % laberinto->columnas;

            //evita que se genere cerca de la entrada o salida
            if(laberinto->celdas[y][x] != ' ' || DISTANCIA(x, y, 0, 1) < minDistanciaES)
                valido = 0;

            //evita que si hay muchos fantasmas, se generen muy cerca entre si
            while(I < f && valido)
            {
                if(DISTANCIA(x, y, fantasmas[I].posX, fantasmas[I].posY) < minDistanciaFantasmas)
                    valido = 0;

                I++;
            }

            intentos++;
            if(intentos > 100)
            {
                printf("Error ubicando un fantasma, se generaran menos de los indicados.");
                return ERROR_GENERACION;
            }
        }while (!valido);

        fantasmas[f].posY = y;
        fantasmas[f].posX = x;
        fantasmas[f].vivo = 1;
        laberinto->celdas[y][x] = 'F';
        fantasmas[f].quePisa = ' ';
    }

    return TODO_BIEN;
}

//Se usa un algoritmo BFS, que basicamente busca la ruta mas corta hacia el jugador. Posiblemente lo explique
//mas detallado en el readme, pero como estoy por caer en la demencia, puede ser que me haya olvidado   
int moverFantasmas(Tablero *laberinto, Fantasma *fantasmas, Jugador *jugador, int maxFantasmas)
{
    int posibleX[4] = {1, -1, 0, 0},
        posibleY[4] = {0, 0, 1, -1},
        sX, sY, gX, gY, nX, nY, cX, cY, nuevoX, nuevoY, d, f, c, encontrado, I, J, usarBFS,
        visitado[MAX_FILAS][MAX_COLUMNAS] = {0};
    NodoPos padre[MAX_FILAS][MAX_COLUMNAS],
            inicio, actual, vecino, p;
    tCola ColaFantasmas;
    crearCola(&ColaFantasmas);
    
    if (!laberinto || !laberinto->celdas || !fantasmas || !jugador)
        return ERROR_MEMORIA;

    for(I = 0; I < maxFantasmas; I++)
    {
        if(!fantasmas[I].vivo)
            continue;

        if(rand() %100 < 80)
            usarBFS = 1;
        else
            usarBFS = 0;

        sX = fantasmas[I].posX;
        sY = fantasmas[I].posY;
        gX = jugador->posX;
        gY = jugador->posY;

        if(usarBFS)
        {
            inicio.x = sX;
            inicio.y = sY;

            encolar(&ColaFantasmas, &inicio, sizeof(NodoPos));

            visitado[sY][sX] = 1;
            padre[sY][sX] = (NodoPos){-1, -1};

            encontrado = 0;

            while(!colaVacia(&ColaFantasmas))
            {
                desencolar(&ColaFantasmas, &actual, sizeof(NodoPos));

                if(actual.x == gX && actual.y == gY)
                {
                    encontrado = 1;
                    break;
                }

                for(J = 0; J < 4; J++)
                {
                    nuevoX = actual.x + posibleX[J];
                    nuevoY = actual.y + posibleY[J];

                    if(nuevoX >= 0 && nuevoX < laberinto->columnas && 
                    nuevoY >= 0 && nuevoY < laberinto->filas &&
                    !visitado[nuevoY][nuevoX] &&
                    laberinto->celdas[nuevoY][nuevoX] != '#' &&
                    laberinto->celdas[nuevoY][nuevoX] != 'S' &&
                    laberinto->celdas[nuevoY][nuevoX] != 'E' &&
                    laberinto->celdas[nuevoY][nuevoX] != 'F')
                    {
                        vecino.x = nuevoX;
                        vecino.y = nuevoY;
                        encolar(&ColaFantasmas, &vecino, sizeof(NodoPos));
                        visitado[nuevoY][nuevoX] = 1;
                        padre[nuevoY][nuevoX] = actual;
                    }
                }
            }

            nX = sX;
            nY = sY;

            if(encontrado)
            {
                cX = gX;
                cY = gY;

                while(!(cX == sX && cY == sY))
                {
                    p = padre[cY][cX];
                    if(p.x == sX && p.y == sY)
                        break; 
                    cX = p.x;
                    cY = p.y;
                }

                nX = cX;
                nY = cY;
            }
        }
        else
        {
            d = rand() % 4;
            nX = sX + posibleX[d];
            nY = sY + posibleY[d];
        }

        if(nX >= 0 && nX < laberinto->columnas && nY >= 0 && nY < laberinto->filas &&
           laberinto->celdas[nY][nX] != '#' && laberinto->celdas[nY][nX] != 'S' && 
           laberinto->celdas[nY][nX] != 'E')
        {
            laberinto->celdas[sY][sX] = fantasmas[I].quePisa;
            fantasmas[I].quePisa = laberinto->celdas[nY][nX];
            fantasmas[I].posX = nX;
            fantasmas[I].posY = nY;
            laberinto->celdas[nY][nX] = 'F';

            if(nX == gX && nY == gY)
            {
                jugador->vidas--;
                if(jugador->vidas <= 0)
                    return DERROTA;
            
                fantasmas[I].vivo = 0;
                laberinto->celdas[nY][nX] = 'J';
            }
        }

        //Se limpia todo por si acaso
        vaciarCola(&ColaFantasmas);
        memset(visitado, 0, sizeof(visitado));
        for(f = 0; f < laberinto->filas; f++)
        {
            for(c = 0; c < laberinto->columnas; c++)
            {
                padre[f][c] = (NodoPos){-1, -1};
            }
        }
    }

    return TODO_BIEN;
}

int encontrarFantasma(Fantasma *fantasmas, int maxFantasmas, int x, int y)
{
    int I;

    if(!fantasmas)
        return ERROR_MEMORIA;

    for(I = 0; I < maxFantasmas; I++)
    {
        if(fantasmas[I].posX == x && fantasmas[I].posY == y && fantasmas[I].vivo)
            return I;
    }

    return -1;
}



//FUNCIONES DE PREMIOS Y VIDAS//
int colocarVidasExtra(Tablero *laberinto, int vidasExtra)
{
    int colocadas = 0, x, y, I, valido, intentos = 0,
        (*posVidas)[2] = malloc(sizeof(int[vidasExtra][2]));
    const int minDistanciaES = 3,
              minDistanciaVidas = 3;

    if (!laberinto || !laberinto->celdas)
        return ERROR_MEMORIA;

    if(!posVidas)
        return ERROR_GENERACION;

    if(vidasExtra <= 0)
        return ERROR_GENERACION;

    while(colocadas < vidasExtra && intentos < 100)
    {
        valido = 1;
        I = 0;
        intentos++;

        y = rand() % laberinto->filas;
        x = rand() % laberinto->columnas;

        if(laberinto->celdas[y][x] != ' ' || DISTANCIA(x, y, 0, 1) < minDistanciaES)
            valido = 0;

        while(I < colocadas && valido)
        {
            if(DISTANCIA(x, y, posVidas[I][0], posVidas[I][1]) < minDistanciaVidas)
                valido = 0;

            I++;
        }

        if(valido)
        {
            posVidas[colocadas][0] = x;
            posVidas[colocadas][1] = y;
            laberinto->celdas[y][x] = 'V';
            colocadas++;
        }
    }

    free(posVidas);

    if(colocadas < vidasExtra)
        printf("\nError colocando todas las vidas extra, se colocaron menos de las indicadas");
        
    return TODO_BIEN;
}

int colocarPremios(Tablero *laberinto, int maxPremios)
{
    int colocados = 0, x, y, I, valido, intentos = 0,
        (*posPremios)[2] = malloc(sizeof(int[maxPremios][2]));
    const int minDistanciaES = 2,
              minDistanciaPremios = 2;

    if (!laberinto || !laberinto->celdas)
        return ERROR_MEMORIA;

    if(!posPremios)
        return ERROR_GENERACION;

    if(maxPremios <= 0)
        return ERROR_GENERACION;

    while(colocados < maxPremios && intentos < 100)
    {
        valido = 1;
        I = 0;
        intentos++;

        y = rand() % laberinto->filas;
        x = rand() % laberinto->columnas;

        if(laberinto->celdas[y][x] != ' ' || DISTANCIA(x, y, 0, 1) < minDistanciaES)
            valido = 0;

        while(I < colocados && valido)
        {
            if(DISTANCIA(x, y, posPremios[I][0], posPremios[I][1]) < minDistanciaPremios)
                valido = 0;

            I++;
        }

        if(valido)
        {
            posPremios[colocados][0] = x;
            posPremios[colocados][1] = y;
            laberinto->celdas[y][x] = 'P';
            colocados++;
        }
    }

    free(posPremios);

    if(colocados < maxPremios)
        printf("\nError generando todos los premios, se generaron menos de los esperados");

    return TODO_BIEN;
}



//FUNCIONES DE PARTIDA//
int menu(SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto)
{
    SDL_Event evento;
    int op, ejecutando = 1, I, x, y, hover = -1;
    const char *titulo = "LABERINTOS Y FANTASMAS",
               *jugar = "JUGAR",
               *ranking = "VER RANKING",
               *salir = "SALIR";
    const char *opciones[] = {jugar, ranking, salir};
    SDL_Surface *supTexto;
    SDL_Texture *texturaTexto;
    SDL_Rect recta, rectaOpciones[3];
    SDL_Color color;

    while(ejecutando)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        supTexto = TTF_RenderText_Blended(fuente, titulo, COLOR_ROJO);
        texturaTexto = SDL_CreateTextureFromSurface(renderer, supTexto);
        
        recta.x = ancho / 2 - supTexto->w / 2;
        recta.y = alto / 5;
        recta.w = supTexto->w;
        recta.h = supTexto->h;

        SDL_RenderCopy(renderer, texturaTexto, NULL, &recta);
        SDL_FreeSurface(supTexto);
        SDL_DestroyTexture(texturaTexto);

        for(I = 0; I < 3; I++)
        {
            color = (I == hover) ? COLOR_AMARILLO : COLOR_BLANCO;
            supTexto = TTF_RenderText_Blended(fuente, opciones[I], color);
            texturaTexto = SDL_CreateTextureFromSurface(renderer, supTexto);
        
            recta.x = ancho / 2 - supTexto->w / 2;
            recta.y = (alto / 2) + (I * (supTexto->h + 20));
            recta.w = supTexto->w;
            recta.h = supTexto->h;

            SDL_RenderCopy(renderer, texturaTexto, NULL, &recta);
            SDL_FreeSurface(supTexto);
            SDL_DestroyTexture(texturaTexto);

            rectaOpciones[I] = recta;

        }

        SDL_RenderPresent(renderer);

        while(SDL_PollEvent(&evento))
        {
            if(evento.type == SDL_QUIT)
            {
                op = 3;
                ejecutando = 0;
            }
            else if(evento.type == SDL_MOUSEMOTION)
            {
                x = evento.motion.x;
                y = evento.motion.y;

                hover = -1;

                for(I = 0; I < 3; I ++)
                {
                    if (x >= rectaOpciones[I].x && x <= rectaOpciones[I].x + rectaOpciones[I].w &&
                        y >= rectaOpciones[I].y && y <= rectaOpciones[I].y + rectaOpciones[I].h)
                    {
                        hover = I;
                        break;
                    }
                }
            }
            else if(evento.type == SDL_MOUSEBUTTONDOWN && evento.button.button == SDL_BUTTON_LEFT)
            {
                x = evento.button.x;
                y = evento.button.y;
                
                for (I = 0; I < 3; I++)
                {
                    if (x >= rectaOpciones[I].x && x <= rectaOpciones[I].x + rectaOpciones[I].w &&
                        y >= rectaOpciones[I].y && y <= rectaOpciones[I].y + rectaOpciones[I].h)
                    {
                        op = I + 1; // 1=Jugar, 2=Ranking, 3=Salir
                        ejecutando = 0;
                    }
                }
            }
        }

        SDL_Delay(16);
    }

    return op;
}

int Jugar(Tablero *laberinto, Jugador *jugador, Fantasma *fantasmas, int maxFantasmas, SDL_Window *ventana,
            SDL_Renderer *renderer, TTF_Font **fuentePtr, TTF_Font **fuenteHudPtr, int tamFuenteBase,
            int tamFuenteHudBase, int ancho, int alto, tCola *ColaMovimientos)
{
    char movimiento = 0, aseguradorMovimiento;
    int estado = 1, jugando = 1, tamCeldaReal, winW, winH,
        vpW, vpH, totalWidth, offsetX, tamFuenteHudDeseado;
    float escalaX, escalaY, escala, escalaTexto = 1.0f;
    TTF_Font *fuenteLocal = NULL;
    TTF_Font *fuenteHudOriginal = NULL;
    int tamFuenteHudActual = 0;
    TTF_Font *fuenteHudLocal = NULL;
    SDL_Event evento;

    if (!laberinto || !jugador || !fantasmas || !ventana || !renderer || !ColaMovimientos)
        return ERROR_MEMORIA;

    SDL_GetWindowSize(ventana, &winW, &winH);

    //Inicializar punteros locales a las fuentes pasadas por referencia
    if (fuentePtr)
        fuenteLocal = *fuentePtr;
    else
        fuenteLocal = NULL;

    if (fuenteHudPtr)
        fuenteHudOriginal = *fuenteHudPtr;
    else
        fuenteHudOriginal = NULL;

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
                if (escala <= 0.0f) 
                    escala = 1.0f;
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
                                 {
                                    printf("No se pudo realizar un movimiento.");
                                    break;
                                 }
                                 break;
                    case SDLK_s: movimiento = 's';
                                 if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                 {
                                    printf("No se pudo realizar un movimiento.");
                                    break;
                                 }
                                 break;
                    case SDLK_a: movimiento = 'a';
                                 if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                 {
                                    printf("No se pudo realizar un movimiento.");
                                    break;
                                 }
                                 break;
                    case SDLK_d: movimiento = 'd';
                                 if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                 {
                                    printf("No se pudo realizar un movimiento.");
                                    break;
                                 }
                                 break;
                    case SDLK_ESCAPE: jugando = 0;
                                      break;
                    default:     movimiento = 'z';
                                 if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                 {
                                    printf("No se pudo realizar un movimiento.");
                                    break;
                                 }
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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        tamCeldaReal = (ancho / laberinto->columnas);
        if (tamCeldaReal <= 0)
            tamCeldaReal = 1;

        totalWidth = tamCeldaReal * laberinto->columnas;
        offsetX = (ancho - totalWidth) / 2;
        if (offsetX < 0)
            offsetX = 0;

        tamFuenteHudDeseado = 35;
        
        if (tamFuenteHudDeseado != tamFuenteHudActual)
        {
            //Recrear la fuente HUD local
            if (fuenteHudLocal && fuenteHudLocal != fuenteHudOriginal)
            {
                TTF_CloseFont(fuenteHudLocal);
                fuenteHudLocal = NULL;
            }
            fuenteHudLocal = TTF_OpenFont("assets/Sora-Bold.ttf", tamFuenteHudDeseado);
            if (!fuenteHudLocal)
                fuenteHudLocal = fuenteHudOriginal;
            else
                tamFuenteHudActual = tamFuenteHudDeseado;
        }

        //Usar la fuente HUD local si existe, sino la pasada por parametro 
        TTF_Font *fuenteParaHUD = (fuenteHudLocal ? fuenteHudLocal : fuenteHudOriginal);

        dibujarTablero(renderer, laberinto, jugador, fuenteParaHUD, tamCeldaReal, offsetX, escalaTexto);

        SDL_RenderPresent(renderer);

        SDL_Delay(1000 / 60);
    }

    if(estado == VICTORIA)
        victoria(ventana, renderer, fuenteLocal, ancho, alto, jugador->puntaje);
    else if(estado == DERROTA)
        derrota(ventana, renderer, fuenteLocal, ancho, alto);

    //Liberar la fuente HUD local si fue creada 
    if (fuenteHudLocal && fuenteHudLocal != fuenteHudOriginal)
    {
        TTF_CloseFont(fuenteHudLocal);
        fuenteHudLocal = NULL;
    }

    return TODO_BIEN;
}

int realizarMovimiento(Tablero *laberinto, Jugador *jugador, Fantasma *fantasmas, int maxFantasmas, char direccion)
{
    int nuevaFila = jugador->posY,
        nuevaColumna = jugador->posX,
        fantasmaBuscado,
        puntosGanados;

    if (!laberinto || !jugador || !fantasmas || !laberinto->celdas || maxFantasmas <= 0)
        return ERROR_MEMORIA;

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
    centrarRenderizado(renderer, fuente, puntajeTexto, COLOR_AMARILLO, ancho, alto / 2 + 75, escalaTexto);
    centrarRenderizado(renderer, fuente, "Presiona ESC para salir...", COLOR_GRIS, ancho, alto + 250, escalaTexto);

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
    centrarRenderizado(renderer, fuente, "No se obtendran puntos.", COLOR_ROJO, ancho, alto / 2 + 75, escalaTexto);
    centrarRenderizado(renderer, fuente, "Presiona ESC para salir...", COLOR_GRIS, ancho, alto + 250, escalaTexto);

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