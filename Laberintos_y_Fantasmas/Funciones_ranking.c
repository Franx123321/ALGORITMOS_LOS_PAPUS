#include "Funciones_ranking.h"

int cmpPuntajeDesc(const void *a, const void *b) {
    const Usuario *ua = (const Usuario *)a;
    const Usuario *ub = (const Usuario *)b;
    return ub->p_total - ua->p_total;
}


void printUsuario(const void *dato) {
    const Usuario *user = (const Usuario *)dato;
    printf("ID: %d | Nombre: %s | Puntaje Total: %d| PartidasJugadas: %d\n", user->id_jugador, user->nombre, user->p_total, user->partidas_jugadas);
}


void verRanking(tLista* ranking, SOCKET *sock) {
    char buffer[512], nombre[50], acumulador[4096] = "";
    int bytes, id, puntaje, cantPartidas;

    Usuario user;

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

    // Procesar tokens separados por ';'
    char *token = strtok(acumulador, ";");
    while (token != NULL) {
        if (strstr(token, "FIN_RANKING") != NULL)
            break;

        strcpy(nombre, "");
        id = 0;
        puntaje = 0;
        cantPartidas=0;

        if (sscanf(token, "%d|%49[^|]|%d|%d", &id, nombre, &puntaje,&cantPartidas) == 4) {
            user.id_jugador = id;
            strncpy(user.nombre, nombre, sizeof(user.nombre) - 1);
            user.nombre[sizeof(user.nombre) - 1] = '\0';
            user.p_total = puntaje;
            user.partidas_jugadas=cantPartidas;

            // Se inserta una copia del usuario
            if (!insertarOrdenadoSinDup(ranking, &user, sizeof(Usuario), cmpPuntajeDesc, NULL)) {
                printf("No se pudo insertar usuario %s\n", user.nombre);
            }
        } else {
            printf("Formato inválido: %s\n", token);
        }

        token = strtok(NULL, ";");
    }
}

void mostrarRankingSDL(ContextoSDL *sdl, TTF_Font *fuente, tLista *ranking)
{
    SDL_Renderer *renderer = sdl->renderer;
    SDL_Event e;
    int ejecutando = 1;
    int scroll = 0;
    const int visible = 8;
    int total = 0;
    tNodo *act = *ranking;

    // Contar jugadores
    while (act) { total++; act = act->sig; }

    // --- Definición de columnas ---
    const float esc = 1.3f; // tabla 30% más ancha
    const int colRankW   = (int)(60 * esc);
    const int colIDW     = (int)(90 * esc);
    const int colNameW   = (int)(330 * esc);
    const int colGamesW  = (int)(140 * esc);
    const int colScoreW  = (int)(150 * esc);

    // Calcular ancho total y centrado
    const int tableWidth = colRankW + 10 + colIDW + 20 + colNameW + 20 + colGamesW + 20 + colScoreW;
    const int leftX = (sdl->ancho - tableWidth) / 2;
    const int centerX = sdl->ancho / 2;

    // Posiciones de columnas con más espacio entre PARTIDAS y PUNTAJE
    const int colRankX  = leftX;
    const int colIDX    = colRankX + colRankW + 10;
    const int colNameX  = colIDX + colIDW + 15;
    const int colGamesX = colNameX + colNameW - 15; // más atrás
    const int colScoreX = colGamesX + colGamesW + 45; // más adelante

    // Posiciones verticales
    const int headerY = 140;
    const int startY  = 215;
    const int rowHeight = 48;

    while (ejecutando)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // --- BOTÓN "X" ---
        const char *cerrar = "X";
        SDL_Surface *suf = TTF_RenderText_Solid(fuente, cerrar, COLOR_VERDE);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, suf);
        SDL_Rect rCerrar = { sdl->ancho - suf->w - 30, 30, suf->w, suf->h };
        SDL_RenderCopy(renderer, tex, NULL, &rCerrar);
        SDL_FreeSurface(suf);
        SDL_DestroyTexture(tex);

        // --- TÍTULO ---
        const char *titulo = "RANKING";
        suf = TTF_RenderText_Solid(fuente, titulo, COLOR_ROJO);
        tex = SDL_CreateTextureFromSurface(renderer, suf);
        SDL_Rect rTit = { centerX - suf->w / 2, 80, suf->w, suf->h };
        SDL_RenderCopy(renderer, tex, NULL, &rTit);
        SDL_FreeSurface(suf);
        SDL_DestroyTexture(tex);

        // --- CABECERAS ---
        SDL_Color hc = COLOR_AMARILLO;

        // ID
        suf = TTF_RenderText_Solid(fuente, "ID", hc);
        tex = SDL_CreateTextureFromSurface(renderer, suf);
        SDL_Rect r = { colIDX + (colIDW - suf->w) / 2, headerY, suf->w, suf->h };
        SDL_RenderCopy(renderer, tex, NULL, &r);
        SDL_FreeSurface(suf);
        SDL_DestroyTexture(tex);

        // NOMBRE
        suf = TTF_RenderText_Solid(fuente, "NOMBRE", hc);
        tex = SDL_CreateTextureFromSurface(renderer, suf);
        r = (SDL_Rect){ colNameX + (colNameW - suf->w) / 2, headerY, suf->w, suf->h };
        SDL_RenderCopy(renderer, tex, NULL, &r);
        SDL_FreeSurface(suf);
        SDL_DestroyTexture(tex);

        // N arriba
        suf = TTF_RenderText_Solid(fuente, "CANT", hc);
        tex = SDL_CreateTextureFromSurface(renderer, suf);
        SDL_Rect rN = { colGamesX + (colGamesW - suf->w) / 2, headerY - 10, suf->w, suf->h };
        SDL_RenderCopy(renderer, tex, NULL, &rN);
        SDL_FreeSurface(suf);
        SDL_DestroyTexture(tex);

        // PARTIDAS debajo
        suf = TTF_RenderText_Solid(fuente, "PARTIDAS", hc);
        tex = SDL_CreateTextureFromSurface(renderer, suf);
        SDL_Rect rPart = { colGamesX + (colGamesW - suf->w) / 2, headerY + 30, suf->w, suf->h };
        SDL_RenderCopy(renderer, tex, NULL, &rPart);
        SDL_FreeSurface(suf);
        SDL_DestroyTexture(tex);

        // PUNTAJE
        suf = TTF_RenderText_Solid(fuente, "PUNTAJE", hc);
        tex = SDL_CreateTextureFromSurface(renderer, suf);
        r = (SDL_Rect){ colScoreX + (colScoreW - suf->w) / 2, headerY, suf->w, suf->h };
        SDL_RenderCopy(renderer, tex, NULL, &r);
        SDL_FreeSurface(suf);
        SDL_DestroyTexture(tex);

        // --- FILAS ---
        act = *ranking;
        for (int k = 0; k < scroll && act; k++) act = act->sig;

        int y = startY;
        int index = scroll + 1;
        while (act && index <= scroll + visible)
        {
            Usuario *u = (Usuario *)act->dato;

            // RANK
            char sRank[16];
            snprintf(sRank, sizeof(sRank), "%d:", index);
            suf = TTF_RenderText_Solid(fuente, sRank, COLOR_BLANCO);
            tex = SDL_CreateTextureFromSurface(renderer, suf);
            SDL_Rect rc = { colRankX + (colRankW - suf->w) / 2, y, suf->w, suf->h };
            SDL_RenderCopy(renderer, tex, NULL, &rc);
            SDL_FreeSurface(suf);
            SDL_DestroyTexture(tex);

            // ID
            char sID[32];
            snprintf(sID, sizeof(sID), "%d", u->id_jugador);
            suf = TTF_RenderText_Solid(fuente, sID, COLOR_BLANCO);
            tex = SDL_CreateTextureFromSurface(renderer, suf);
            rc = (SDL_Rect){ colIDX + (colIDW - suf->w) / 2, y, suf->w, suf->h };
            SDL_RenderCopy(renderer, tex, NULL, &rc);
            SDL_FreeSurface(suf);
            SDL_DestroyTexture(tex);

            // NOMBRE (truncado)
            char nameTrunc[64];
            strncpy(nameTrunc, u->nombre, sizeof(nameTrunc) - 1);
            nameTrunc[sizeof(nameTrunc) - 1] = '\0';
            int textW = 0, textH = 0;
            TTF_SizeText(fuente, nameTrunc, &textW, &textH);
            int maxWidth = colGamesX - (colNameX + 10) - 10;

            if (textW > maxWidth) {
                int len = strlen(nameTrunc);
                while (len > 0) {
                    nameTrunc[len--] = '\0';
                    char temp[70];
                    snprintf(temp, sizeof(temp), "%s...", nameTrunc);
                    TTF_SizeText(fuente, temp, &textW, &textH);
                    if (textW <= maxWidth) {
                        strcpy(nameTrunc, temp);
                        break;
                    }
                }
            }

            suf = TTF_RenderText_Solid(fuente, nameTrunc, COLOR_BLANCO);
            tex = SDL_CreateTextureFromSurface(renderer, suf);
            rc = (SDL_Rect){ colNameX + 10, y, suf->w, suf->h };
            SDL_RenderCopy(renderer, tex, NULL, &rc);
            SDL_FreeSurface(suf);
            SDL_DestroyTexture(tex);

            // PARTIDAS
            char sGames[32];
            snprintf(sGames, sizeof(sGames), "%d", u->partidas_jugadas);
            suf = TTF_RenderText_Solid(fuente, sGames, COLOR_BLANCO);
            tex = SDL_CreateTextureFromSurface(renderer, suf);
            rc = (SDL_Rect){ colGamesX + (colGamesW - suf->w) / 2, y, suf->w, suf->h };
            SDL_RenderCopy(renderer, tex, NULL, &rc);
            SDL_FreeSurface(suf);
            SDL_DestroyTexture(tex);

            // PUNTAJE
            char sP[32];
            snprintf(sP, sizeof(sP), "%d", u->p_total);
            suf = TTF_RenderText_Solid(fuente, sP, COLOR_BLANCO);
            tex = SDL_CreateTextureFromSurface(renderer, suf);
            rc = (SDL_Rect){ colScoreX + (colScoreW - suf->w) / 2, y, suf->w, suf->h };
            SDL_RenderCopy(renderer, tex, NULL, &rc);
            SDL_FreeSurface(suf);
            SDL_DestroyTexture(tex);

            y += rowHeight;
            act = act->sig;
            index++;
        }

        // Scrollbar
        if (total > visible) {
            float trackH = visible * rowHeight;
            float barraH = (float)visible / total * trackH;
            float barraY = startY + ((float)scroll / (total - visible)) * (trackH - barraH);
            SDL_Rect barra = { leftX + tableWidth + 10, (int)barraY, 12, (int)barraH };
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &barra);
        }

        SDL_RenderPresent(renderer);

        // Eventos
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) ejecutando = 0;
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) ejecutando = 0;
            else if (e.type == SDL_MOUSEWHEEL)
            {
                if (e.wheel.y > 0 && scroll > 0) scroll--;
                else if (e.wheel.y < 0 && scroll + visible < total) scroll++;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            {
                int mx = e.button.x, my = e.button.y;
                if (mx >= rCerrar.x && mx <= rCerrar.x + rCerrar.w &&
                    my >= rCerrar.y && my <= rCerrar.y + rCerrar.h)
                    ejecutando = 0;
            }
        }

        SDL_Delay(16);
    }
}
