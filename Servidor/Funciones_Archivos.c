#include <stdio.h>
#include <string.h>
#include "Funciones_Archivos.h"
#include "arbol.h"

#define ERROR_APERTURA 0
#define TODO_BIEN 1

//Usuarios
int almacenarJugador(Usuario *j, tArbolBinBusq *arbol, HANDLE mutexArbol) 
{
    WaitForSingleObject(mutexArbol, INFINITE);

    long offset;
    Induser nuevo;
    FILE *uf = fopen("jugadores.dat", "r+b");
    if(!uf)
        uf = fopen("jugadores.dat", "w+b");
    if(!uf)
    {
        ReleaseMutex(mutexArbol);
        return ERROR_APERTURA;
    }

    tNodoArbol **iuser = buscarEnArbol(arbol, j->nombre, comparacionArbol);

    if (!iuser) 
    {
        //Nuevo jugador si no existia ya
        fseek(uf, 0, SEEK_END); //me muevo al ultimo registro y obtengo sus datos
        offset = ftell(uf);
        j->id_jugador = contarNodosArbol(arbol) + 1;
        fwrite(j, sizeof(Usuario), 1, uf);

        
        strcpy(nuevo.nombre, j->nombre);
        nuevo.offset = offset;
        insertarEnArbolOrdenado(arbol, &nuevo, sizeof(Induser), comparacionIndexes);
    } else
        cargarDatosEnArch(uf, j, (*iuser)->dato); // Si ya existia voy a cargardatosenarch

    fclose(uf);
    return TODO_BIEN;
}


int cargarDatosEnArch(FILE *pf, Usuario *j, const void *dato) {
    Induser *cdato = (Induser *)dato;
    Usuario user;

    fseek(pf, cdato->offset, SEEK_SET);
    fread(&user, sizeof(Usuario), 1, pf);

    user.p_total += j->p_total; //le sumo el puntaje que obtuvo en la ultima partida
    j->id_jugador = user.id_jugador;
    user.partidas_jugadas++;

    fseek(pf, -(long)sizeof(Usuario), SEEK_CUR);
    fwrite(&user, sizeof(Usuario), 1, pf);

    return TODO_BIEN;
}


void cargarIndiceDesdeArchivo(tArbolBinBusq *p, FILE *pf){
    Usuario user;
    Induser idx;
    long offset;

    rewind(pf);

    while(1)
    {
        offset = ftell(pf);
        if(fread(&user, sizeof(Usuario), 1, pf) != 1)
            break;
        strcpy(idx.nombre, user.nombre);
        idx.offset = offset;
        insertarEnArbolOrdenado(p, &idx, sizeof(Induser), comparacionIndexes);
    }
}

int comparacionArbol(const void *a, const void *b){
    const char *nombreClave = (const char *)b;
    const Induser *usuario = (const Induser *)a;

    return strcmp(usuario->nombre, nombreClave);
}

int comparacionIndexes(const void *a, const void *b){
    const Induser *ca = (const Induser*)a;
    const Induser *cb = (const Induser*)b;

    return strcmp(ca->nombre, cb->nombre);
}



//Partida
int almacenarPartida(Usuario *j, int cantmovimientos){
    FILE *pf = fopen("partidas.dat", "r+b");
    if(!pf)
        pf = fopen("partidas.dat", "w+b");
    if(!pf){
        return ERROR_APERTURA;
    }

    Partida datos;

    fseek(pf, 0, SEEK_END);
    long registros = ftell(pf);

    if(registros == 0){
        datos.id_partida = 1; //le asigno id 0 y le copio los datos del jugador actual
        datos.cantidad_movimientos = cantmovimientos;
        datos.id_usuario = j->id_jugador;
        strcpy(datos.nombre, j->nombre);
        datos.puntaje=j->p_total;

        fwrite(&datos, sizeof(Partida), 1, pf); //lo escribo como nuevo registro
        fclose(pf);
        return TODO_BIEN;
    }

    fseek(pf, -((long)sizeof(Partida)), SEEK_END); //miro el ultimo registro para calcular el nuevo id
    fread(&datos, sizeof(Partida),1, pf); //lo almaceno en la variable datos
    datos.id_partida++; //le creo el nuevo id y le coloco los datos del jugador actual

    datos.id_usuario = j->id_jugador;
    datos.cantidad_movimientos = cantmovimientos;
    strncpy(datos.nombre, j->nombre, sizeof(datos.nombre) - 1);
    datos.nombre[sizeof(datos.nombre) - 1] = '\0';
    datos.puntaje = j->p_total;

    fseek(pf, 0, SEEK_END);
    fflush(pf);
    fwrite(&datos, sizeof(Partida), 1, pf); // lo coloco al final
    fclose(pf);
    return TODO_BIEN;
}

