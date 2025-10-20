#include "Funciones_Generacion.h"

//GENERAR FANTASMAS//
int generarFantasmas(Tablero *laberinto, Fantasma *fantasmas, int maxFantasmas)
{
    int f, x, y, I, valido, intentos;
    const int minDistanciaES = 3,
              minDistanciaFantasmas = 4;

    if (!laberinto || !laberinto->celdas || !fantasmas)
        return ERROR_MEMORIA;

    for (f = 0; f < maxFantasmas; f++)
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


//GENERAR PREMIOS Y VIDAS//
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