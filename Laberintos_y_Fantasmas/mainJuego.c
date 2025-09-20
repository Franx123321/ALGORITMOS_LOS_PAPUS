#include "Macros.h"

//TABLERO//
void **crear_matriz(int filas, int columnas, unsigned tamElem);
void destruir_matriz(void **matriz, int filas);
void generarLaberinto(Tablero laberinto, int fil, int col);
void mostrar_matriz(char **matriz, int filas, int col);
void inicializarTablero(Tablero laberinto);
void mezclarDirecciones(int dir[4][2]);
void generarSalida(Tablero *laberinto);
int leerConfiguracion(const char* nombreArch, Configuracion* config);


int main()
{
    Configuracion config;
    Tablero laberinto;
    int i;

    if (leerConfiguracion("config.txt", &config)) {
        exit(1);
    }

    srand(time(NULL));

    laberinto.filas = config.filas;
    laberinto.columnas = config.columnas;
    laberinto.celdas = (char **)crear_matriz(laberinto.filas, laberinto.columnas, sizeof(char));

    inicializarTablero(laberinto);
    laberinto.celdas[1][1] = ' ';
    generarLaberinto(laberinto, 1, 1);
    laberinto.celdas[1][0] = 'E';
    generarSalida(&laberinto);
    mostrar_matriz(laberinto.celdas, laberinto.filas, laberinto.columnas);
    destruir_matriz((void **)laberinto.celdas, laberinto.filas);





    return 0;
}


//FUNCIONES DE TABLERO//
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

int leerConfiguracion(const char* nombreArch, Configuracion* config)
{
    int valor, seteados = 0;
    char linea[128], clave[80], *sep;

    FILE *archConfig = fopen(nombreArch, "rt");
    if (!archConfig)
    {
        perror("Error al abrir archivo de configuracion");
        return 1;
    }

    while (fgets(linea, 128, archConfig)) {
        sep = strchr(linea, '=');
        if (!sep) continue;

        strncpy(clave, linea, sep - linea);
        *(clave + (sep - linea)) = '\0';

        sscanf(sep + 1, " %d", &valor);

        if (strcmp(clave, "filas") == 0) {
            config->filas = valor;
            seteados++;
        }
        if (strcmp(clave, "columnas") == 0) {
            config->columnas = valor;
            seteados++;
        }
        if (strcmp(clave, "vidas_inicio") == 0) {
            config->vidasIniciales = valor;
            seteados++;
        }
        if (strcmp(clave, "maximo_numero_fantasmas") == 0) {
            config->maxFantasmas = valor;
            seteados++;
        }
        if (strcmp(clave, "maximo_numero_premios") == 0) {
            config->maxPremios = valor;
            seteados++;
        }
        if (strcmp(clave, "maximo_vidas_extra") == 0) {
            config->maxVidasExtra = valor;
            seteados++;
        }
    }

    if (seteados < 6) {
        printf("Error de configuracion: faltan parametros en el archivo o estan mal escritos.\n");
        printf("    (tip: se escribe 'clave=valor', separados con signo igual)\n");
        return 2;
    }

    fclose(archConfig);

    return 0;
}
