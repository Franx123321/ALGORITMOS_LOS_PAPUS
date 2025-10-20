#include "Funciones_Tablero.h"

//FUNCIONES DE TABLERO//
int cargarLaberinto(Tablero *laberinto, Fantasma *fantasmas, Jugador *jugador, Configuracion config)
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
    laberinto->celdas[jugador->posY][jugador->posX] = 'J';

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

int generarSalida(Tablero *laberinto)
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
    char celda, mensajeHUD[100];
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
    renderizarHUD(renderer, fuente, mensajeHUD, COLOR_AZUL, 10, 3, escalaTexto);
    sprintf(mensajeHUD, "Puntaje: %d", j->puntaje);
    renderizarHUD(renderer, fuente, mensajeHUD, COLOR_AZUL, 198, 3, escalaTexto);
    sprintf(mensajeHUD, "Jugador: %s", j->nombre);
    renderizarHUD(renderer, fuente, mensajeHUD, COLOR_AZUL, 423, 3, escalaTexto);
}
