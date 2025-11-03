#include <stdio.h>
#include <string.h>
#include "Funciones_Archivos.h"
#include "arbol.h"

#define ERROR_APERTURA 0
#define TODO_BIEN 1

//Usuarios

int almacenarJugador(Jugador *j) {
    FILE *uf = fopen("usuarios.dat", "r+b");
    if(!uf)
        uf = fopen("usuarios.dat", "w+b");
    if(!uf)
        return ERROR_APERTURA;

    Usuario user;
    tArbolBinBusq arbol;
    tNodoArbol **iuser;

    fseek(uf, 0, SEEK_END);
    long registros = ftell(uf) / sizeof(Usuario);

    if(registros == 0) {
        user.id_jugador = 1; // Primer usuario
        user.p_total = j->puntaje;
        strcpy(user.nombre, j->nombre);
        j->id = user.id_jugador;
        user.partidas_jugadas=1;

        fseek(uf, 0, SEEK_SET);
        fwrite(&user, sizeof(Usuario), 1, uf);
        fclose(uf);
        return TODO_BIEN;
    }

    crearArbol(&arbol); //creo el arbol
    cargarIndiceDesdeArchivo(&arbol, uf); // Cargar el árbol con los usuarios existentes


    iuser = buscarEnArbol(&arbol, j->nombre, comparacionArbol); // Buscar si el jugador ya existe

    if (!iuser) {// Nuevo jugador si no existia ya
        fseek(uf, -((long)sizeof(Usuario)), SEEK_END); //me muevo al ultimo registro y obtengo sus datos
        fread(&user, sizeof(Usuario), 1, uf);
        user.id_jugador++;  // El nuevo ID va a ser el siguiente del anterior

        Usuario nuevo = user;
        nuevo.p_total = j->puntaje;
        strncpy(nuevo.nombre, j->nombre, sizeof(nuevo.nombre) - 1);
        nuevo.nombre[sizeof(nuevo.nombre) - 1] = '\0';
        j->id = nuevo.id_jugador;
        nuevo.partidas_jugadas=1;

        fseek(uf, 0, SEEK_END);
        fwrite(&nuevo, sizeof(Usuario), 1, uf);
    } else {
        cargarDatosEnArch(uf, j, (*iuser)->dato); // Si ya existia voy a cargardatosenarch
    }

    destruirArbol(&arbol); //borro el arbol despues de usarlo y cierro el archivo
    fclose(uf);
    return TODO_BIEN;
}

int cargarDatosEnArch(FILE *pf, Jugador *j, const void *dato) {
    Induser *cdato = (Induser *)dato;
    Usuario user;

    fseek(pf, cdato->offset - sizeof(Usuario), SEEK_SET);
    fread(&user, sizeof(Usuario), 1, pf);

    user.p_total += j->puntaje; //le sumo el puntaje que obtuvo en la ultima partida
    j->id = user.id_jugador;
    user.partidas_jugadas++;

    fseek(pf, -(long)sizeof(Usuario), SEEK_CUR);
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
    const char *nombreClave = (const char *)b;
    const Induser *usuario = (const Induser *)a;

    return strcmp(usuario->nombre, nombreClave);
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
        datos.id_partida = 1; //le asigno id 0 y le copio los datos del jugador actual
        datos.cantidad_movimientos = cantmovimientos;
        datos.id_usuario = j->id;
        strcpy(datos.nombre, j->nombre);
        datos.puntaje=j->puntaje;

        fwrite(&datos, sizeof(Partida), 1, pf); //lo escribo como nuevo registro
        fclose(pf);
        return TODO_BIEN;
    }

    fseek(pf, -((long)sizeof(Partida)), SEEK_END); //miro el ultimo registro para calcular el nuevo id
    fread(&datos, sizeof(Partida),1, pf); //lo almaceno en la variable datos
    datos.id_partida++; //le creo el nuevo id y le coloco los datos del jugador actual

    Partida nueva = datos;
    nueva.id_usuario = j->id;
    nueva.cantidad_movimientos = cantmovimientos;
    strncpy(nueva.nombre, j->nombre, sizeof(nueva.nombre) - 1);
    nueva.nombre[sizeof(nueva.nombre) - 1] = '\0';
    nueva.puntaje=j->puntaje;

    fseek(pf, 0, SEEK_END);
    fflush(pf);
    fwrite(&nueva, sizeof(Partida), 1, pf); // lo coloco al final
    fclose(pf);
    return TODO_BIEN;
}

