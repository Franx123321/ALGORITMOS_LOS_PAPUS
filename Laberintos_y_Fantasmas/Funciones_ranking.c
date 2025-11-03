#include "Funciones_ranking.h"

int cmpPuntajeDesc(const void *a, const void *b) {
    const Usuario *ua = (const Usuario *)a;
    const Usuario *ub = (const Usuario *)b;
    return ub->p_total - ua->p_total;
}


void printUsuario(const void *dato) {
    const Usuario *user = (const Usuario *)dato;
    printf("ID: %d | Nombre: %s | Puntaje Total: %d\n", user->id_jugador, user->nombre, user->p_total);
}


void verRanking(SOCKET *sock) {
    char buffer[512], nombre[50], acumulador[4096] = "";
    int bytes, id, puntaje;

    Usuario user;
    tLista ranking;
    crearLista(&ranking);

    // Pedimos ranking al servidor
    send(*sock, "RANKING", 7, 0);

    // Recibir datos hasta FIN_RANKING
    while (1) {
        bytes = recv(*sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            printf("Error o cierre de conexión.\n");
            break;
        }

        buffer[bytes] = '\0';

        // Evitamos desbordar acumulador
        if (strlen(acumulador) + strlen(buffer) >= sizeof(acumulador) - 1) {
            printf("Advertencia: datos de ranking demasiado grandes, truncando.\n");
            strncat(acumulador, buffer, sizeof(acumulador) - strlen(acumulador) - 1);
            break;
        }

        strcat(acumulador, buffer);

        if (strstr(acumulador, "FIN_RANKING") != NULL)
            break;
    }

    printf("\n--- RANKING DE JUGADORES ---\n");

    // Procesar tokens separados por ';'
    char *token = strtok(acumulador, ";");
    while (token != NULL) {
        if (strstr(token, "FIN_RANKING") != NULL)
            break;

        strcpy(nombre, "");
        id = 0;
        puntaje = 0;

        if (sscanf(token, "%d|%49[^|]|%d", &id, nombre, &puntaje) == 3) {
            user.id_jugador = id;
            strncpy(user.nombre, nombre, sizeof(user.nombre) - 1);
            user.nombre[sizeof(user.nombre) - 1] = '\0';
            user.p_total = puntaje;

            // Se inserta una copia del usuario
            if (!insertarOrdenadoSinDup(&ranking, &user, sizeof(Usuario), cmpPuntajeDesc, NULL)) {
                printf("No se pudo insertar usuario %s\n", user.nombre);
            }
        } else {
            printf("Formato inválido: %s\n", token);
        }

        token = strtok(NULL, ";");
    }

    printf("----------------------------\n");
    printf("Lista ranking:\n");
    recorrerLista(&ranking, sizeof(Usuario), printUsuario);

    vaciarLista(&ranking);
}