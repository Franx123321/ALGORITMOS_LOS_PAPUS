#include "lista.h"

typedef struct{
    int codigo;
    char descripcion[20];
    float precio;
    int stock;
}Producto;

int cmpInt(const void *a, const void *b);
int cmpProd(const void *a, const void *b);

void mostrarInt(const void *elemento);
void mostrarProducto(const void *elemento);

void acumularProducto(void **datoLista, unsigned *tamDatoLista, const void *dato, unsigned);

void generarProductos();
void generarTxt();


int main()
{
    //DESCOMENTAR PARA GENERAR LOS ARCHIVOS; SINO NO SIRVE
    //generarTxt();
    //generarProductos();

    tLista ListaSimple, ListaStruct;
    FILE *texto, *binario;
    int op, auxDup, auxAcum, aux;
    Producto prod;

    crearLista(&ListaSimple);
    crearLista(&ListaStruct);

    texto = fopen("pruebaSimple.txt", "rt");
    binario = fopen("pruebaStruct.dat", "rb");
    if(!texto || !binario)
    {
        printf("\nERROR al abrir un archivo.");
        exit(1);
    }

    do{
        do{
            printf("\n------MENU------\n");
            printf("1. Lista simple.\n2. Lista de estructura.\n3. Salir.\nSu opción: ");
            scanf("%d", &op);
        }while(op < 1 || op > 3);
        switch(op)
        {
            case 1: vaciarLista(&ListaSimple);
                    do{
                        printf("\n\n1. Con duplicado.\n2. Sin duplicado.\nSu opción: ");
                        scanf("%d",&auxDup);
                    }while(auxDup < 1 || auxDup > 2);

                    if(auxDup == 1)
                    {
                        while(fscanf(texto, "%d", &aux) == 1)
                            insertarOrdenadoConDup(&ListaSimple, &aux, sizeof(int), cmpInt);
                    }
                    else if(auxDup == 2)
                    {
                        while(fscanf(texto, "%d", &aux) == 1) 
                            insertarOrdenadoSinDup(&ListaSimple, &aux, sizeof(int), cmpInt, NULL);
                    }
                    recorrerLista(&ListaSimple,sizeof(int),mostrarInt);

                    break;

            case 2: rewind(binario);
                    do{
                        printf("\n\n1. Con duplicado.\n2. Sin duplicado.\nSu opción: ");
                        scanf("%d",&auxDup);
                    }while(auxDup < 1 || auxDup > 2);
                    if(auxDup == 1)
                    {
                        while(fread(&prod,sizeof(Producto),1,binario))
                        {
                            insertarOrdenadoConDup(&ListaStruct,&prod,sizeof(Producto),cmpProd);
                            printf("%-10d | %-20s | %-10.2f | %-10d\n", prod.codigo, prod.descripcion, prod.precio, prod.stock);
                        }
                        printf("\n\n\n");
                    }
                    else if(auxDup == 2)
                    {
                        do{
                            printf("\n\n1. Permitir acumular duplicados (por stock).\n2. No permitir acumular duplicados.\nSu opción: ");
                            scanf("%d",&auxAcum);
                        }while(auxAcum < 1 || auxAcum > 2);

                        if(auxAcum == 1)
                        {
                            while(fread(&prod,sizeof(Producto),1,binario))
                            {
                                insertarOrdenadoSinDup(&ListaStruct,&prod,sizeof(Producto),cmpProd,acumularProducto);
                                printf("%-10d | %-20s | %-10.2f | %-10d\n", prod.codigo, prod.descripcion, prod.precio, prod.stock);
                            }
                            printf("\n\n\n");
                        }
                        else if(auxAcum == 2)
                        {
                            while(fread(&prod,sizeof(Producto),1,binario))
                            {
                                insertarOrdenadoSinDup(&ListaStruct,&prod,sizeof(Producto),cmpProd,NULL);
                                printf("%-10d | %-20s | %-10.2f | %-10d\n", prod.codigo, prod.descripcion, prod.precio, prod.stock);
                            }
                            printf("\n\n\n");
                        }
                    }
                    recorrerLista(&ListaStruct, sizeof(Producto), mostrarProducto);

                    break;

            case 3: break;
        }
    }while(op < 3);

    vaciarLista(&ListaSimple);
    vaciarLista(&ListaStruct);

    fclose(binario);
    fclose(texto);

    printf("\n\n\n");
    return 0;
}

int cmpInt(const void *a, const void *b)
{
    return (*(const int*)a - *(const int*)b);
}

int cmpProd(const void *a, const void *b)
{
    Producto *p1 = (Producto *)a;
    Producto *p2 = (Producto *)b;
    return p1->codigo - p2->codigo;
}

void mostrarInt(const void *elemento)
{
    int el = *(int *)elemento;
    printf("%d\t", el);
}

void mostrarProducto(const void *elemento)
{
    Producto *p = (Producto *)elemento;
    printf("%-10d | %-20s | %-10.2f | %-10d\n", p->codigo, p->descripcion, p->precio, p->stock);
}

void acumularProducto(void **datoLista, unsigned *tamDatoLista, const void *dato, unsigned tamDato)
{
    Producto *existente = *(Producto **)datoLista;
    const Producto *nuevo = (const Producto *)dato;

    existente->stock += nuevo->stock;
}




void generarTxt() 
{
    FILE *fp = fopen("pruebaSimple.txt", "w");
    if (!fp) 
    {
        printf("Error al abrir archivo");
        return;
    }

    // Inicializar semilla para números aleatorios
    srand((unsigned)time(NULL));

    for (int i = 0; i < 20; i++) {
        int num = rand() % 101; // número entre 0 y 100
        fprintf(fp, "%d\n", num);
    }

    fclose(fp);
}

void generarProductos() 
{
    FILE *fp = fopen("pruebaStruct.dat", "wb");
    if (!fp) 
    {
        printf("Error al abrir archivo");
        return;
    }

    // Lista de 10 productos iniciales
    Producto base[10] = {
        {1, "Producto1", 10.5f, 5},
        {2, "Producto2", 12.0f, 10},
        {3, "Producto3", 8.75f, 20},
        {4, "Producto4", 15.0f, 7},
        {5, "Producto5", 9.99f, 12},
        {6, "Producto6", 20.0f, 3},
        {7, "Producto7", 5.5f, 25},
        {8, "Producto8", 18.0f, 8},
        {9, "Producto9", 11.0f, 15},
        {10,"Producto10",14.25f, 9}
    };

    // Arreglo de 20 registros, incluyendo duplicados
    Producto lista[20] = {
        base[0], base[1], base[2], base[3], base[4],
        base[5], base[6], base[7], base[8], base[9],
        base[0], base[1], base[2], base[3], base[4],
        base[5], base[6], base[7], base[8], base[9]
    };

    for (int i = 0; i < 20; i++) {
        fwrite(&lista[i], sizeof(Producto), 1, fp);
    }

    fclose(fp);
}
