#include "arbol_bin_busq.h"

#define MINIMO(X,Y) ((X)<(Y)?(X):(Y))
#define MAXIMO(X,Y) ((X)>(Y)?(X):(Y))

tNodoArbol **mayorNodoArbolBinBusq(const tArbolBinBusq *p);
tNodoArbol **menorNodoArbolBinBusq(const tArbolBinBusq *p);
tNodoArbol **mayorRecNodoArbolBinBusq(tArbolBinBusq *p);
tNodoArbol **menorRecNodoArbolBinBusq(tArbolBinBusq *p);
tNodoArbol **buscarNodoArbolBinBusq(const tArbolBinBusq *p, const void *d, int (*cmp)(const void*, const void*));
tNodoArbol **buscarRecNodoArbolBinBusq(const tArbolBinBusq *p, const void *d, int (*cmp)(const void*, const void*));
int esCompletoHastaNivelArbolBin(const tArbolBinBusq *p, int n);
int esAVL2CalculoArbolBin(const tArbolBinBusq *p);
const tArbolBinBusq * mayorNodoNoClaveArbolBinBusq(const tArbolBinBusq *p, const tArbolBinBusq *mayor, int (*cmp)(const void*, const void*));
const tArbolBinBusq * menorNodoNoClaveArbolBinBusq(const tArbolBinBusq *p, const tArbolBinBusq *menor, int (*cmp)(const void*, const void*));  
const tArbolBinBusq * buscarNodoNoClaveArbolBinBusq(const tArbolBinBusq *p, const void *d, int (*cmp)(const void*, const void*));   

void crearArbolBinBusq(tArbolBinBusq *p)
{
    *p = NULL;
}

#define reservarMemoriaNodo(X,Y,Z,W)(\
    ((X) = (typeof(X))malloc(Y))==NULL || \
    ((Z)=malloc(W))==NULL ? \
    free(X), 0: 1)

int insertarArbolBinBusq(tArbolBinBusq *p, const void *d, unsigned tam, int (*cmp)(const void*, const void*))
{
    tNodoArbol *nue;
    int rc;

    while(*p)
    {
        if((rc=cmp(d,(*p)->info)) < 0)
            p=&(*p)->izq;
        else if(rc > 0)
            p=&(*p)->der;
        else
            return CLA_DUP;
    }
    if(!reservarMemoriaNodo(nue,sizeof(tNodoArbol),nue->info,tam))
        return SIN_MEM;

    nue->tamInfo = tam;
    memcpy(nue->info, d, tam);
    nue->der=nue->izq=NULL;
    *p=nue;
    return TODO_BIEN;
}

int insertarRecArbolBinBusq(tArbolBinBusq *p, const void *d, unsigned tam, int (*cmp)(const void*, const void*))
{
    tNodoArbol *nue;
    int rc;

    if(!*p)
    {
        if((rc=cmp(d,(*p)->info)) < 0)
            return insertarRecArbolBinBusq(&(*p)->izq, d, tam, cmp);
        if(rc > 0)
            return insertarRecArbolBinBusq(&(*p)->der, d, tam, cmp);
        else
            return CLA_DUP;
    }
    if(!reservarMemoriaNodo(nue,sizeof(tNodoArbol),nue->info,tam))
        return SIN_MEM;

    nue->tamInfo = tam;
    memcpy(nue->info, d, tam);
    nue->der=nue->izq=NULL;
    *p=nue;
    return TODO_BIEN;
}

void recorrerEnOrdenRecArbolBinBusq(const tArbolBinBusq *p, unsigned n, void * params, void (*accion)(void*, unsigned, unsigned, void*))
{
    if(!*p)
        return;
    
    recorrerEnOrdenRecArbolBinBusq(&(*p)->izq, n+1, params, accion);
    accion((*p)->info, (*p)->tamInfo, n, params);
    recorrerEnOrdenRecArbolBinBusq(&(*p)->der, n+1, params, accion);
}

void recorrerEnOrdenArbolBinBusq(const tArbolBinBusq *p, void * params, void (*accion)(void*, unsigned, unsigned, void*))
{
    recorrerEnOrdenRecArbolBinBusq(p, 0, params, accion);
}

void recorrerEnOrdenInversoArbolBinBusq(const tArbolBinBusq *p, void * params, void (*accion)(void*, unsigned, unsigned, void*))
{
    if(!*p)
        return;
    
    recorrerEnOrdenInversoArbolBinBusq(&(*p)->der, params, accion);
    accion((*p)->info, (*p)->tamInfo, 0, params);
    recorrerEnOrdenInversoArbolBinBusq(&(*p)->izq, params, accion);
}

void recorrerPreOrdenRecArbolBinBusq(const tArbolBinBusq *p,unsigned n, void * params, void (*accion)(void*, unsigned, unsigned, void*))
{
    if(!*p)
        return;
    
    accion((*p)->info, (*p)->tamInfo, 0, params);
    recorrerPreOrdenRecArbolBinBusq(&(*p)->izq,n+1, params, accion);
    recorrerPreOrdenRecArbolBinBusq(&(*p)->der,n+1, params, accion);
}

void recorrerPreOrdenArbolBinBusq(const tArbolBinBusq *p, void * params, void (*accion)(void*, unsigned, unsigned, void*))
{
    recorrerPreOrdenRecArbolBinBusq(p, 0, params, accion);
}

void recorrerPosOrdenRecArbolBinBusq(const tArbolBinBusq *p, unsigned n, void * params, void (*accion)(void*, unsigned, unsigned, void*))
{
    if(!*p)
        return;
    
    recorrerPosOrdenRecArbolBinBusq(&(*p)->izq, n+1, params, accion);
    recorrerPosOrdenRecArbolBinBusq(&(*p)->der, n+1, params, accion);
    accion((*p)->info, (*p)->tamInfo, n, params);
}

void recorrerPosOrdenArbolBinBusq(const tArbolBinBusq *p, void * params, void (*accion)(void*, unsigned, unsigned, void*))
{
    recorrerPosOrdenRecArbolBinBusq(p, 0, params, accion);
}

void recorrerEnOrdenSimpleArbolBinBusq(const tArbolBinBusq *p, void * params, void (*accion)(void*, unsigned, unsigned, void*))
{
    if(!*p)
        return;
    
    recorrerEnOrdenSimpleArbolBinBusq(&(*p)->izq, params, accion);
    accion((*p)->info, (*p)->tamInfo, 0, params);
    recorrerEnOrdenSimpleArbolBinBusq(&(*p)->der, params, accion);
}

void recorrerPreOrdenSimpleArbolBinBusq(const tArbolBinBusq *p, void * params, void (*accion)(void*, unsigned, unsigned, void*))
{
    if(!*p)
        return;
    
    accion((*p)->info, (*p)->tamInfo, 0, params);
    recorrerPreOrdenSimpleArbolBinBusq(&(*p)->izq, params, accion);
    recorrerPreOrdenSimpleArbolBinBusq(&(*p)->der, params, accion);
}

void recorrerPosOrdenSimpleArbolBinBusq(const tArbolBinBusq *p, void * params, void (*accion)(void*, unsigned, unsigned, void*))
{
    if(!*p)
        return;
    
    recorrerPosOrdenSimpleArbolBinBusq(&(*p)->izq, params, accion);
    recorrerPosOrdenSimpleArbolBinBusq(&(*p)->der, params, accion);
    accion((*p)->info, (*p)->tamInfo, 0, params);
}

int eliminarRaizArbolBinBusq(tArbolBinBusq *p)
{
    tNodoArbol **remp,
                *elim;

    if(!p)
        return SIN_INICIALIZAR;;

    free((*p)->info);
    if(!(*p)->izq && !(*p)->der)
    {
        free(*p);
        *p=NULL;
        return 1; // Eliminacion exitosa
    }
    
    remp=alturaArbolBin(&(*p)->izq) >= alturaArbolBin(&(*p)->der) ?
          mayorNodoArbolBinBusq(&(*p)->izq) :
          menorNodoArbolBinBusq(&(*p)->der);

    elim=*remp;
    (*p)->info=elim->info;
    (*p)->tamInfo=elim->tamInfo;

    *remp=elim->izq ? elim->izq : elim->der;
    free(elim);

    return 1; // Eliminacion exitosa
}

int eliminarElemArbolBinBusq(tArbolBinBusq *p, void *d, unsigned tam, int (*cmp)(const void*, const void*))
{
    if(!(p=buscarNodoArbolBinBusq(p, d, cmp)))
        return CLA_DUP; // No se encontro el elemento a eliminar
    memcpy(d, (*p)->info, MINIMO(tam, (*p)->tamInfo));
    return eliminarRaizArbolBinBusq(p);
}

int buscarElemArbolBinBusq(const tArbolBinBusq *p, void *d, unsigned tam, int (*cmp)(const void*, const void*))
{
    if(!(p=buscarNodoArbolBinBusq(p, d, cmp)))
        return 0; // No se encontro el elemento
    memcpy(d, (*p)->info, MINIMO(tam, ((*p)->tamInfo)));
    return 1; // Elemento encontrado
}

unsigned leerDesdeArchivoBin(void **d, void *pf, unsigned pos, void *params)
{
    unsigned tam=*((int*)params);
    *d = malloc(tam);
    if(!*d)
        return 0;
    fseek((FILE*)pf, pos*tam, SEEK_SET);
    return fread(*d,tam,1,(FILE*)pf);
}

int cargarDesdeDatosOrdenadosRec(tArbolBinBusq *p, void *ds, unsigned (*leer)(void **, void *, unsigned, void* params), int li, int ls, void *params)
{
    int m=(li+ls)/2,
        r;

    if(li>ls)
        return TODO_BIEN;

    (*p) = (tNodoArbol*)malloc(sizeof(tNodoArbol));
    if(!*p || !((*p)->tamInfo = leer(&(*p)->info, ds, m, params)))
    {
        free(*p);
        return SIN_MEM;
    }
    (*p)->izq = NULL;
    (*p)->der = NULL;

    if((r=cargarDesdeDatosOrdenadosRec(&(*p)->izq, ds, leer, li, m-1, params)) != TODO_BIEN)
        return r;
    return cargarDesdeDatosOrdenadosRec(&(*p)->der, ds, leer, m+1, ls, params);
}

int cargarArchivoBinOrdenadoAbiertoArbolBinBusq(tArbolBinBusq *p, FILE *pf, unsigned tamInfo)
{
    int cantReg;
    if(*p || !pf)
        return SIN_INICIALIZAR;
    fseek(pf, 0L, SEEK_END);
    cantReg = ftell(pf) / tamInfo;
    return cargarDesdeDatosOrdenadosRec(p, pf, leerDesdeArchivoBin, 0, cantReg-1, &tamInfo);
}

int cargarArchivoBinOrdenadoArbolBinBusq(tArbolBinBusq *p, const char *path, unsigned tamInfo)
{
    int cantReg, r;
    FILE *pf;

    if(*p)
        return SIN_INICIALIZAR;
    if(!(pf=fopen(path,"rb")))
        return ERROR_ARCH;
    fseek(pf,0L,SEEK_END);
    cantReg=ftell(pf)/tamInfo;
    r=cargarDesdeDatosOrdenadosRec(p,pf,leerDesdeArchivoBin,0,cantReg-1,&tamInfo);
    fclose(pf);
    return r;
}

int cargarDesdeDatosOrdenadosArbolBinBusq(tArbolBinBusq *p, void *ds, unsigned cantReg, unsigned (*leer)(void**, void*, unsigned, void *params), void *params)
{
    if(*p || !ds)
        return 0;
    return cargarDesdeDatosOrdenadosRec(p,ds,leer,0,cantReg-1,params);
}

tNodoArbol **buscarNodoArbolBinBusq(const tArbolBinBusq *p, const void *d, int (*cmp)(const void*, const void*))
{
    int rc;
    while(*p && (rc=cmp(d,(*p)->info)))
    {
        if(rc < 0)
            p=&(*p)->izq;
        else
            p=&(*p)->der;
    }
    if(!*p)
        return 0;
    return (tNodoArbol**)p;
}

tNodoArbol ** buscarRecNodoArbolBinBusq(const tArbolBinBusq *p, const void *d, int (*cmp)(const void*, const void*))
{
    int rc;
    if(!*p)
        return NULL;
    if(*p && (rc=cmp(d,(*p)->info)))
    {
        if(rc < 0)
            return buscarRecNodoArbolBinBusq(&(*p)->izq, d, cmp);
        return buscarRecNodoArbolBinBusq(&(*p)->der, d, cmp);
    }
    return (tNodoArbol**)p;
}

tNodoArbol ** mayorNodoArbolBinBusq(const tArbolBinBusq *p)
{
    if(!*p)
        return NULL;
    while((*p)->der)
        p=&(*p)->der;
    return (tNodoArbol**)p;
}   

tNodoArbol ** menorNodoArbolBinBusq(const tArbolBinBusq *p)
{
    if(!*p)
        return NULL;
    while((*p)->izq)
        p=&(*p)->izq;
    return (tNodoArbol**)p;
}

tNodoArbol ** mayorRecNodoArbolBinBusq(tArbolBinBusq *p)
{
    if(!*p)
        return NULL;
    if(!(*p)->der)
        return (tNodoArbol **)p;
    return mayorRecNodoArbolBinBusq(&(*p)->der);
}

tNodoArbol ** menorRecNodoArbolBinBusq(tArbolBinBusq *p)
{
    if(!*p)
        return NULL;
    if(!(*p)->izq)
        return (tNodoArbol **)p;
    return menorRecNodoArbolBinBusq(&(*p)->izq);
}

int mayorElemNoClaveArbolBinBusq(const tArbolBinBusq *p, void *d, unsigned tam, int (*cmp)(const void*, const void*))
{
    const tArbolBinBusq *mayor=p;
    if(!*p)
        return 0;
    mayor=mayorNodoNoClaveArbolBinBusq(&(*p)->izq, mayor, cmp);
    mayor=mayorNodoNoClaveArbolBinBusq(&(*p)->der, mayor, cmp);
    memcpy(d, (*mayor)->info, MINIMO(tam, (*mayor)->tamInfo));
    return 1; // Elemento encontrado
}

const tArbolBinBusq * mayorNodoNoClaveArbolBinBusq(const tArbolBinBusq *p, const tArbolBinBusq *mayor, int (*cmp)(const void*, const void*))
{
    if(!*p)
        return mayor;
    if(cmp((*p)->info, (*mayor)->info) > 0)
        mayor=p;
    mayor=mayorNodoNoClaveArbolBinBusq(&(*p)->izq, mayor, cmp);
    mayor=mayorNodoNoClaveArbolBinBusq(&(*p)->der, mayor, cmp);
    return mayor;
}

int menorElemNoClaveArbolBinBusq(const tArbolBinBusq *p, void *d, unsigned tam, int (*cmp)(const void*, const void*))
{
    const tArbolBinBusq * menor=p;
    if(!*p)
        return 0;

    menor=menorNodoNoClaveArbolBinBusq(&(*p)->izq, menor, cmp);
    menor=menorNodoNoClaveArbolBinBusq(&(*p)->der, menor, cmp);
    memcpy(d, (*menor)->info, MINIMO(tam, (*menor)->tamInfo));
    return 1; // Elemento encontrado
}

const tArbolBinBusq * menorNodoNoClaveArbolBinBusq(const tArbolBinBusq *p, const tArbolBinBusq *menor, int (*cmp)(const void*, const void*))
{
    if(!*p)
        return menor;
    if(cmp((*p)->info, (*menor)->info) < 0)
        menor=p;
    menor=menorNodoNoClaveArbolBinBusq(&(*p)->izq, menor, cmp);
    menor=menorNodoNoClaveArbolBinBusq(&(*p)->der, menor, cmp);
    return menor;
}

int buscarElemNoClaveArbolBinBusq(const tArbolBinBusq *p, void *d, unsigned tam, int (*cmp)(const void*, const void*))
{
    const tArbolBinBusq * busq = NULL;
    if(!*p)
        return 0;

    if(cmp((*p)->info, d) == 0)
        busq=p;
    else if ((busq = buscarNodoNoClaveArbolBinBusq(&(*p)->izq, d, cmp)) == NULL)
    {
        busq = buscarNodoNoClaveArbolBinBusq(&(*p)->der, d, cmp);
    }
        

    if(busq==NULL)  
        return 0;

    memcpy(d, (*busq)->info, MINIMO(tam, (*busq)->tamInfo));

    return 1; // Elemento encontrado    
}

const tArbolBinBusq * buscarNodoNoClaveArbolBinBusq(const tArbolBinBusq *p, const void *d, int (*cmp)(const void*, const void*))
{
    const tArbolBinBusq * busq = NULL;
    if(!*p)
        return NULL;

    if(cmp((*p)->info, d) < 0)
        return p;

    if ((busq = buscarNodoNoClaveArbolBinBusq(&(*p)->izq, busq, cmp)))
        return busq;
    return buscarNodoNoClaveArbolBinBusq(&(*p)->der, busq, cmp);
}

unsigned alturaArbolBin(const tArbolBinBusq *p)
{
    int hi, hd;
    if(!*p)
        return 0;
    hi=alturaArbolBin(&(*p)->izq);
    hd=alturaArbolBin(&(*p)->der);
    return 1 + MAXIMO(hi, hd);
}

unsigned cantNodosArbolBin(const tArbolBinBusq *p)
{
    if(!*p)
        return 0;
    return 1 + cantNodosArbolBin(&(*p)->izq) + cantNodosArbolBin(&(*p)->der);
}

int mayorElemArbolBinBusq(const tArbolBinBusq *p, void *d, unsigned tam)
{
    if(!(p=mayorNodoArbolBinBusq(p)))
        return 0;
    memcpy(d, (*p)->info, MINIMO(tam, (*p)->tamInfo));
    return 1; // Elemento encontrado
}

int menorElemArbolBinBusq(const tArbolBinBusq *p, void *d, unsigned tam)
{
    if(!(p=menorNodoArbolBinBusq(p)))
        return 0;
    memcpy(d, (*p)->info, MINIMO(tam, (*p)->tamInfo));
    return 1; // Elemento encontrado
}

unsigned cantNodosHastaNivelArbolBin(const tArbolBinBusq *p, int n)
{
    if(!*p)
        return 0;
    if(n==0)
        return 1;
    return cantNodosHastaNivelArbolBin(&(*p)->izq, n-1) + cantNodosHastaNivelArbolBin(&(*p)->der, n-1) +1;
}

int esCompletoHastaNivelArbolBin(const tArbolBinBusq *p, int n)
{
    if(!*p)
        return n<0;
    if(n==0)
        return 1;
    return esCompletoHastaNivelArbolBin(&(*p)->izq, n-1) && esCompletoHastaNivelArbolBin(&(*p)->der, n-1);
}

int esCompletoArbolBin(const tArbolBinBusq *p)
{
    return esCompletoHastaNivelArbolBin(p, alturaArbolBin(p)-1);
}

int esBalanceadoArbolBin(const tArbolBinBusq *p)
{
    return esCompletoHastaNivelArbolBin(p,alturaArbolBin(p)-2);
}

int esAVLArbolBinBusq(const tArbolBinBusq *p)
{
    int hi, hd;
    if(!*p)
        return 1;
    hi=alturaArbolBin(&(*p)->izq);
    hd=alturaArbolBin(&(*p)->der);
    if(abs(hi-hd) > 1)
        return 0;
    return esAVLArbolBinBusq(&(*p)->izq) && esAVLArbolBinBusq(&(*p)->der);
}

int esCompleto2ArbolBin(const tArbolBinBusq *p)
{
    int h=alturaArbolBin(p);
    return cantNodosArbolBin(p) == pow(2,h)-1;
}

int esBalanceado2ArbolBin(const tArbolBinBusq *p)
{
    int h=alturaArbolBin(p);
    return cantNodosHastaNivelArbolBin(p,h-2) == pow(2,h-1)-1;
}

int esAVL2CalculoArbolBin(const tArbolBinBusq *p)
{
    int hi, hd;
    if(!*p)
        return 1;
    hi=esAVL2CalculoArbolBin(&(*p)->izq);
    hd=esAVL2CalculoArbolBin(&(*p)->der);
    if(hi<0 || hd<0 || abs(hi-hd)>1)
        return -1;
    return (hi>hd?hi:hd)+1;
}

int esAVL2ArbolBinBusq(const tArbolBinBusq *p)
{
    return esAVL2CalculoArbolBin(p) >= 0;
}