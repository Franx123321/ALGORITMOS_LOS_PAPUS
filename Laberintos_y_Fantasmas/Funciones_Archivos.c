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

    cargarIndiceDesdeArchivo(&arbol, uf);
    iuser = buscarEnArbol(&arbol, j->nombre, comparacionArbol);
    cargarDatosEnArch(uf, j, (*iuser)->dato);

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

        fseek(pf, 0, SEEK_END);
        fwrite(&user, sizeof(Usuario), 1, pf);
        return TODO_BIEN;
    }

    fseek(pf, cdato->offset - sizeof(Usuario), SEEK_SET);
    fread(&user, sizeof(Usuario), 1, pf);

    user.p_total += j->puntaje;

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
    FILE *pf = fopen("partidas.dat", "a+b");
    FILE *fi = fopen("indpartidas.idx", "a+b");
    if(!pf || !fi){
        return ERROR_APERTURA;
    }

    Partida datos;
    datos.cantidad_movimientos = cantmovimientos;
    datos.id_usuario = j->id;
    strcpy(datos.nombre, j->nombre);

    Ind idx;

    fseek(pf, 0, SEEK_END);
    long registros = ftell(pf);

    if(registros == 0){
        datos.id_partida = 0;

        idx.clave = datos.id_partida;
        idx.offset = ftell(fi);

        fwrite(&idx, sizeof(Ind), 1, fi);
        fwrite(&datos, sizeof(Partida), 1, pf);
        fclose(fi);
        fclose(pf);
        return TODO_BIEN;
    }

    fseek(fi, -sizeof(Ind), SEEK_END);
    fread(&idx, sizeof(Ind), 1, fi);
    fseek(fi, 0, SEEK_END);

    idx.clave++;
    idx.offset = ftell(pf);

    datos.id_partida = idx.clave;

    fwrite(&idx, sizeof(Ind), 1, fi);
    fwrite(&datos, sizeof(Partida), 1, pf);
    fclose(pf);
    fclose(fi);
    return TODO_BIEN;
}

