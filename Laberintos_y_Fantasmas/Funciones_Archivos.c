#include <stdio.h>
#include <string.h>
#include "Macros.h"
#include "Funciones_Archivos.h"
#include "arbol.h"

#define ERROR_APERTURA 0
#define TODO_BIEN 1

//Usuarios

int almacenarJugador(Jugador *j){
    FILE *uf = fopen("usuarios.dat", "r+b");
    if(!uf)
        uf = fopen("usuarios.dat", "w+b");
    if(!uf)
        return ERROR_APERTURA;

    Usuario user;
    tArbolBinBusq arbol;
    crearArbol(&arbol);
    tNodoArbol **iuser;

    fseek(uf,0,SEEK_END);
    long registros = ftell(uf); //verifico si existen registros
    long ubicacion;

    if(registros == 0){ //si no hay registros cargo lo actual al archivo
        user.id_jugador = 0;
        user.p_total = j->puntaje;
        strcpy(user.nombre, j->nombre);

        fwrite(&user, sizeof(Usuario), 1, uf);

        fclose(uf);
        return TODO_BIEN;
    }

    cargarIndiceDesdeArchivo(&arbol, uf); //si hay registros creo un ABB donde ordeno los datos por nombre
    iuser = buscarEnArbol(&arbol, j->nombre, comparacionArbol); //busco el nombre del jugador actual en el arbol
    cargarDatosEnArch(uf, j, (*iuser)->dato); //cargo los datos en el archivo, si lo encontro sobreescribo, sino creo nuevo registro

    fclose(uf);
    return TODO_BIEN;
}

int cargarDatosEnArch(FILE *pf, Jugador *j, const void *dato){
    Induser *cdato = (Induser *)dato;
    Usuario user;
    if(!cdato){ //si no encontro al user lo coloco al final del archivo, como un nuevo registro
        fseek(pf, -sizeof(Usuario), SEEK_END);
        fread(&user, sizeof(Usuario), 1, pf);

        user.p_total = j->puntaje;
        user.id_jugador++;
        j->id = user.id_jugador;

        fseek(pf, 0, SEEK_END);
        fwrite(&user, sizeof(Usuario), 1, pf);
        return TODO_BIEN;
    }

    fseek(pf, cdato->offset - sizeof(Usuario), SEEK_SET);
    fread(&user, sizeof(Usuario), 1, pf);

    user.p_total += j->puntaje;
    j->id = user.id_jugador;

    fseek(pf, cdato->offset - sizeof(Usuario), SEEK_SET);
    fwrite(&user, sizeof(Usuario), 1, pf);

    return TODO_BIEN;
}

void cargarIndiceDesdeArchivo(tArbolBinBusq *p, FILE *pf){
    Usuario user;
    Induser idx;

    rewind(pf);

    while(fread(&user, sizeof(Usuario), 1, pf)){
        strcpy(idx.nombre, user.nombre);
        idx.offset = ftell(pf);
        insertarEnArbolOrdenado(p, &idx, sizeof(Induser), comparacionIndexes);
    }
}

int comparacionArbol(const void *a, const void *b){
    Induser *ca = (Induser *)a;
    char *cb = (char*)b;

    return strcmp(ca->nombre, cb);
}

int comparacionIndexes(const void *a, const void *b){
    const Induser *ca = (const Induser*)a;
    const Induser *cb = (const Induser*)b;

    return strcmp(ca->nombre, cb->nombre);
}

// Partidas

int almacenarPartida(Jugador *j, int cantmovimientos){
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
        datos.id_partida = 0; //le asigno id 0 y le copio los datos del jugador actual
        datos.cantidad_movimientos = cantmovimientos;
        datos.id_usuario = j->id;
        strcpy(datos.nombre, j->nombre);

        fwrite(&datos, sizeof(Partida), 1, pf); //lo escribo como nuevo registro
        fclose(pf);
        return TODO_BIEN;
    }

    fseek(pf, -sizeof(Partida), SEEK_END); //miro el ultimo registro para calcular el nuevo id
    fread(&datos, sizeof(Partida),1, pf); //lo almaceno en la variable datos

    datos.id_partida++; //le creo el nuevo id y le coloco los datos del jugador actual
    datos.id_usuario = j->id;
    datos.cantidad_movimientos = cantmovimientos;
    strcpy(datos.nombre, j->nombre);

    fwrite(&datos, sizeof(Partida), 1, pf); // lo coloco al final
    fclose(pf);
    return TODO_BIEN;
}

