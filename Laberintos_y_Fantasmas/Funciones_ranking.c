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


void verRanking(tLista* ranking, SOCKET *sock) {
    char buffer[512], nombre[50], acumulador[4096] = "";
    int bytes, id, puntaje;

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

        if (sscanf(token, "%d|%49[^|]|%d", &id, nombre, &puntaje) == 3) {
            user.id_jugador = id;
            strncpy(user.nombre, nombre, sizeof(user.nombre) - 1);
            user.nombre[sizeof(user.nombre) - 1] = '\0';
            user.p_total = puntaje;

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
    const int colRankW = 60;    // columna para "1:"
    const int colIDW   = 80;    // ID
    const int colNameW = 360;   // NOMBRE
    const int colScoreW = 120;  // PUNTAJE

    // Calcular ancho total real y centrado
    const int tableWidth = colRankW + 10 + colIDW + 20 + colNameW + 20 + colScoreW;
    const int leftX = (sdl->ancho - tableWidth) / 2;
    const int centerX = sdl->ancho / 2;

    // Posiciones de cada columna
    const int colRankX  = leftX;
    const int colIDX    = colRankX + colRankW + 10;
    const int colNameX  = colIDX + colIDW + 20;
    const int colScoreX = colNameX + colNameW + 20;

    const int headerY = 140;
    const int startY  = 180;
    const int rowHeight = 48;

    while (ejecutando)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // --- BOTÓN "X" ARRIBA A LA DERECHA ---
        const char *cerrar = "X";
        SDL_Surface *suf = TTF_RenderText_Solid(fuente, cerrar, COLOR_VERDE);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, suf);
        SDL_Rect rCerrar = { sdl->ancho - suf->w - 30, 30, suf->w, suf->h };
        SDL_RenderCopy(renderer, tex, NULL, &rCerrar);
        SDL_FreeSurface(suf);
        SDL_DestroyTexture(tex);

        // --- TÍTULO "RANKING" ---
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
        r.x = colNameX + (colNameW - suf->w) / 2; r.y = headerY;
        r.w = suf->w; r.h = suf->h;
        SDL_RenderCopy(renderer, tex, NULL, &r);
        SDL_FreeSurface(suf);
        SDL_DestroyTexture(tex);

        // PUNTAJE
        suf = TTF_RenderText_Solid(fuente, "PUNTAJE", hc);
        tex = SDL_CreateTextureFromSurface(renderer, suf);
        r.x = colScoreX + (colScoreW - suf->w) / 2; r.y = headerY;
        r.w = suf->w; r.h = suf->h;
        SDL_RenderCopy(renderer, tex, NULL, &r);
        SDL_FreeSurface(suf);
        SDL_DestroyTexture(tex);

        // --- MOSTRAR FILAS ---
        act = *ranking;
        for (int k = 0; k < scroll && act; k++) act = act->sig;

        int y = startY;
        int index = scroll + 1;
        for (int i = 0; i < visible && act; i++, index++)
        {
            Usuario *u = (Usuario *)act->dato;

            // columna N° (ej: "1:")
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
            rc.x = colIDX + (colIDW - suf->w) / 2; rc.y = y;
            rc.w = suf->w; rc.h = suf->h;
            SDL_RenderCopy(renderer, tex, NULL, &rc);
            SDL_FreeSurface(suf);
            SDL_DestroyTexture(tex);

            // --- NOMBRE (truncado dinámicamente con "...") ---
            char nameTrunc[128];
            strncpy(nameTrunc, u->nombre, sizeof(nameTrunc) - 1);
            nameTrunc[sizeof(nameTrunc) - 1] = '\0';

            int textW = 0, textH = 0;
            TTF_SizeText(fuente, nameTrunc, &textW, &textH);

            // espacio disponible antes del puntaje
            int maxWidth = colScoreX - (colNameX + 10) - 10; // 10px margen

            if (textW > maxWidth)
            {
                // recortar hasta que entre el texto + "..."
                int len = strlen(nameTrunc);
                while (len > 0)
                {
                    nameTrunc[len] = '\0';
                    char temp[128];
                    snprintf(temp, sizeof(temp) + 3, "%s...", nameTrunc);
                    TTF_SizeText(fuente, temp, &textW, &textH);
                    if (textW <= maxWidth) {
                        strcpy(nameTrunc, temp);
                        break;
                    }
                    len--;
                }
            }

            suf = TTF_RenderText_Solid(fuente, nameTrunc, COLOR_BLANCO);
            tex = SDL_CreateTextureFromSurface(renderer, suf);
            rc.x = colNameX + 10; rc.y = y;
            rc.w = suf->w; rc.h = suf->h;
            SDL_RenderCopy(renderer, tex, NULL, &rc);
            SDL_FreeSurface(suf);
            SDL_DestroyTexture(tex);

            // PUNTAJE
            char sP[32];
            snprintf(sP, sizeof(sP), "%d", u->p_total);
            suf = TTF_RenderText_Solid(fuente, sP, COLOR_BLANCO);
            tex = SDL_CreateTextureFromSurface(renderer, suf);
            rc.x = colScoreX + (colScoreW - suf->w) / 2; rc.y = y;
            rc.w = suf->w; rc.h = suf->h;
            SDL_RenderCopy(renderer, tex, NULL, &rc);
            SDL_FreeSurface(suf);
            SDL_DestroyTexture(tex);

            y += rowHeight;
            act = act->sig;
        }

        // --- SCROLLBAR ---
        if (total > visible)
        {
            float trackH = visible * rowHeight;
            float barraH = (float)visible / total * trackH;
            float barraY = startY + ((float)scroll / (total - visible)) * (trackH - barraH);
            SDL_Rect barra = { leftX + tableWidth + 10, (int)barraY, 12, (int)barraH };
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &barra);
        }

        SDL_RenderPresent(renderer);

        // --- EVENTOS ---
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