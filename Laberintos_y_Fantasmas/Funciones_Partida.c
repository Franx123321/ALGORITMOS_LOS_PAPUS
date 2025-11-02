#include "Funciones_Partida.h"

//Se usa un algoritmo BFS, que basicamente busca la ruta mas corta hacia el jugador. Posiblemente lo explique
//mas detallado en el readme, pero como estoy por caer en la demencia, puede ser que me haya olvidado
int moverFantasmas(Tablero *laberinto, Fantasma *fantasmas, Jugador *jugador, int maxFantasmas)
{
    int posibleX[4] = {1, -1, 0, 0},
        posibleY[4] = {0, 0, 1, -1},
        sX, sY, gX, gY, nX, nY, cX, cY, nuevoX, nuevoY, d, f, c, encontrado, I, J, usarBFS,
        visitado[MAX_FILAS][MAX_COLUMNAS] = {0};
    NodoPos padre[MAX_FILAS][MAX_COLUMNAS],
            inicio, actual, vecino, p;
    tCola ColaFantasmas;
    crearCola(&ColaFantasmas);

    if (!laberinto || !laberinto->celdas || !fantasmas || !jugador)
        return ERROR_MEMORIA;

    for(I = 0; I < maxFantasmas; I++)
    {
        if(!fantasmas[I].vivo)
            continue;

        if(rand() %100 < 80)
            usarBFS = 1;
        else
            usarBFS = 0;

        sX = fantasmas[I].posX;
        sY = fantasmas[I].posY;
        gX = jugador->posX;
        gY = jugador->posY;

        if(usarBFS)
        {
            inicio.x = sX;
            inicio.y = sY;

            encolar(&ColaFantasmas, &inicio, sizeof(NodoPos));

            visitado[sY][sX] = 1;
            padre[sY][sX] = (NodoPos){-1, -1};

            encontrado = 0;

            while(!colaVacia(&ColaFantasmas))
            {
                desencolar(&ColaFantasmas, &actual, sizeof(NodoPos));

                if(actual.x == gX && actual.y == gY)
                {
                    encontrado = 1;
                    break;
                }

                for(J = 0; J < 4; J++)
                {
                    nuevoX = actual.x + posibleX[J];
                    nuevoY = actual.y + posibleY[J];

                    if(nuevoX >= 0 && nuevoX < laberinto->columnas &&
                    nuevoY >= 0 && nuevoY < laberinto->filas &&
                    !visitado[nuevoY][nuevoX] &&
                    laberinto->celdas[nuevoY][nuevoX] != '#' &&
                    laberinto->celdas[nuevoY][nuevoX] != 'S' &&
                    laberinto->celdas[nuevoY][nuevoX] != 'E' &&
                    laberinto->celdas[nuevoY][nuevoX] != 'F')
                    {
                        vecino.x = nuevoX;
                        vecino.y = nuevoY;
                        encolar(&ColaFantasmas, &vecino, sizeof(NodoPos));
                        visitado[nuevoY][nuevoX] = 1;
                        padre[nuevoY][nuevoX] = actual;
                    }
                }
            }

            nX = sX;
            nY = sY;

            if(encontrado)
            {
                cX = gX;
                cY = gY;

                while(!(cX == sX && cY == sY))
                {
                    p = padre[cY][cX];
                    if(p.x == sX && p.y == sY)
                        break;
                    cX = p.x;
                    cY = p.y;
                }

                nX = cX;
                nY = cY;
            }
        }
        else
        {
            d = rand() % 4;
            nX = sX + posibleX[d];
            nY = sY + posibleY[d];
        }

        if(nX >= 0 && nX < laberinto->columnas && nY >= 0 && nY < laberinto->filas &&
           laberinto->celdas[nY][nX] != '#' && laberinto->celdas[nY][nX] != 'S' &&
           laberinto->celdas[nY][nX] != 'E')
        {
            laberinto->celdas[sY][sX] = fantasmas[I].quePisa;
            fantasmas[I].quePisa = laberinto->celdas[nY][nX];
            fantasmas[I].posX = nX;
            fantasmas[I].posY = nY;
            laberinto->celdas[nY][nX] = 'F';

            if(nX == gX && nY == gY)
            {
                jugador->vidas--;
                if(jugador->vidas <= 0)
                    return DERROTA;

                fantasmas[I].vivo = 0;
                laberinto->celdas[nY][nX] = 'J';
            }
        }

        //Se limpia todo por si acaso
        vaciarCola(&ColaFantasmas);
        memset(visitado, 0, sizeof(visitado));
        for(f = 0; f < laberinto->filas; f++)
        {
            for(c = 0; c < laberinto->columnas; c++)
            {
                padre[f][c] = (NodoPos){-1, -1};
            }
        }
    }

    return TODO_BIEN;
}

int encontrarFantasma(Fantasma *fantasmas, int maxFantasmas, int x, int y)
{
    int I;

    if(!fantasmas)
        return ERROR_MEMORIA;

    for(I = 0; I < maxFantasmas; I++)
    {
        if(fantasmas[I].posX == x && fantasmas[I].posY == y && fantasmas[I].vivo)
            return I;
    }

    return -1;
}




//FUNCIONES DE PARTIDA//
int menu(SDL_Renderer *renderer, TTF_Font *fuente, int ancho, int alto)
{
    SDL_Event evento;
    int op, ejecutando = 1, I, x, y, hover = -1;
    const char *titulo = "LABERINTOS Y FANTASMAS",
               *jugar = "JUGAR",
               *ranking = "VER RANKING",
               *salir = "SALIR";
    const char *opciones[] = {jugar, ranking, salir};
    SDL_Surface *supTexto;
    SDL_Texture *texturaTexto;
    SDL_Rect recta, rectaOpciones[3];
    SDL_Color color;

    while(ejecutando)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        supTexto = TTF_RenderText_Solid(fuente, titulo, COLOR_ROJO);
        texturaTexto = SDL_CreateTextureFromSurface(renderer, supTexto);

        recta.x = ancho / 2 - supTexto->w / 2;
        recta.y = alto / 5;
        recta.w = supTexto->w;
        recta.h = supTexto->h;

        SDL_RenderCopy(renderer, texturaTexto, NULL, &recta);
        SDL_FreeSurface(supTexto);
        SDL_DestroyTexture(texturaTexto);

        for(I = 0; I < 3; I++)
        {
            color = (I == hover) ? COLOR_AMARILLO : COLOR_BLANCO;
            supTexto = TTF_RenderText_Solid(fuente, opciones[I], color);
            texturaTexto = SDL_CreateTextureFromSurface(renderer, supTexto);

            recta.x = ancho / 2 - supTexto->w / 2;
            recta.y = (alto / 2) + (I * (supTexto->h + 20));
            recta.w = supTexto->w;
            recta.h = supTexto->h;

            SDL_RenderCopy(renderer, texturaTexto, NULL, &recta);
            SDL_FreeSurface(supTexto);
            SDL_DestroyTexture(texturaTexto);

            rectaOpciones[I] = recta;
        }

        SDL_RenderPresent(renderer);

        while(SDL_PollEvent(&evento))
        {
            if(evento.type == SDL_QUIT)
            {
                op = 3;
                ejecutando = 0;
            }
            else if(evento.type == SDL_MOUSEMOTION)
            {
                x = evento.motion.x;
                y = evento.motion.y;

                hover = -1;

                for(I = 0; I < 3; I ++)
                {
                    if (x >= rectaOpciones[I].x && x <= rectaOpciones[I].x + rectaOpciones[I].w &&
                        y >= rectaOpciones[I].y && y <= rectaOpciones[I].y + rectaOpciones[I].h)
                    {
                        hover = I;
                        break;
                    }
                }
            }
            else if(evento.type == SDL_MOUSEBUTTONDOWN && evento.button.button == SDL_BUTTON_LEFT)
            {
                x = evento.button.x;
                y = evento.button.y;

                for (I = 0; I < 3; I++)
                {
                    if (x >= rectaOpciones[I].x && x <= rectaOpciones[I].x + rectaOpciones[I].w &&
                        y >= rectaOpciones[I].y && y <= rectaOpciones[I].y + rectaOpciones[I].h)
                    {
                        op = I + 1; // 1=Jugar, 2=Ranking, 3=Salir
                        ejecutando = 0;
                    }
                }
            }
        }

        SDL_Delay(16);
    }

    return op;
}

int pantallaIngresarNombre(ContextoSDL *sdl, TTF_Font *fuente, Jugador *jugador)
{
    SDL_Event evento;
    int op, ejecutando = 1, actualizar = 1, I, x, y, hover = -1, prevHover = -1,
        noNombre = 0, ancho = sdl->ancho, alto = sdl->alto;
    const char *titulo = "LABERINTOS Y FANTASMAS",
               *descripcion = "Ingrese su nombre",
               *advertencia = "El nombre esta vacio",
               *iniciar = "INICIAR";
    SDL_Surface *supTexto;
    SDL_Texture *texTitulo, *texDescripcion, *texturaTexto;
    SDL_Rect recta, rectaTitulo, rectaDesc, rectaBoton;
    SDL_Color color;

    char nombre[50];
    *nombre = '\0';

    // Titulo
    supTexto = TTF_RenderText_Solid(fuente, titulo, COLOR_ROJO);
    texTitulo = SDL_CreateTextureFromSurface(sdl->renderer, supTexto);

    rectaTitulo.x = ancho / 2 - supTexto->w / 2;
    rectaTitulo.y = alto / 5;
    rectaTitulo.w = supTexto->w;
    rectaTitulo.h = supTexto->h;

    SDL_FreeSurface(supTexto);

    // Descripcion
    supTexto = TTF_RenderText_Solid(fuente, descripcion, COLOR_BLANCO);
    texDescripcion = SDL_CreateTextureFromSurface(sdl->renderer, supTexto);

    rectaDesc.x = ancho / 2 - supTexto->w / 2;
    rectaDesc.y = alto / 15 * 5;
    rectaDesc.w = supTexto->w;
    rectaDesc.h = supTexto->h;

    SDL_FreeSurface(supTexto);

    while(ejecutando)
    {
        if (actualizar)
        {
            SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 255);
            SDL_RenderClear(sdl->renderer);

            // Titulo
            SDL_RenderCopy(sdl->renderer, texTitulo, NULL, &rectaTitulo);

            // Ingrese su nombre
            SDL_RenderCopy(sdl->renderer, texDescripcion, NULL, &rectaDesc);

            if (*nombre) {
                supTexto = TTF_RenderText_Solid(fuente, nombre, COLOR_BLANCO);
                texturaTexto = SDL_CreateTextureFromSurface(sdl->renderer, supTexto);

                recta.x = ancho / 2 - supTexto->w / 2;
                recta.y = alto / 15 * 7;
                recta.w = supTexto->w;
                recta.h = supTexto->h;

                SDL_RenderCopy(sdl->renderer, texturaTexto, NULL, &recta);
                SDL_FreeSurface(supTexto);
                SDL_DestroyTexture(texturaTexto);

                // Cursor de texto
                SDL_SetRenderDrawColor(sdl->renderer, 255, 255, 255, 255);
                SDL_RenderDrawLine(sdl->renderer, recta.x + recta.w, recta.y, recta.x + recta.w, recta.y + recta.h);
            }
            else {
                SDL_SetRenderDrawColor(sdl->renderer, 255, 255, 255, 255);
                SDL_RenderDrawLine(sdl->renderer, ancho / 2, alto / 15 * 7, ancho / 2, alto / 15 * 7 + rectaTitulo.h);
            }


            if (noNombre) {
                renderizarCentrado(sdl->renderer, sdl->fuente, advertencia, COLOR_AMARILLO, ancho, alto / 5 * 7, 1);
            }

            // Boton Iniciar
            color = (I == hover) ? COLOR_AMARILLO : COLOR_BLANCO;
            supTexto = TTF_RenderText_Solid(fuente, iniciar, color);
            texturaTexto = SDL_CreateTextureFromSurface(sdl->renderer, supTexto);

            recta.x = ancho / 2 - supTexto->w / 2;
            recta.y = alto - (supTexto->h + 80);
            recta.w = supTexto->w;
            recta.h = supTexto->h;

            SDL_RenderCopy(sdl->renderer, texturaTexto, NULL, &recta);
            SDL_FreeSurface(supTexto);
            SDL_DestroyTexture(texturaTexto);

            rectaBoton = recta;
            actualizar = 0;

            SDL_RenderPresent(sdl->renderer);
        }

        // Revisar eventos
        while(SDL_PollEvent(&evento))
        {
            if(evento.type == SDL_QUIT)
            {
                op = SALIR;
                ejecutando = 0;
            }
            else if(evento.type == SDL_MOUSEMOTION)
            {
                x = evento.motion.x;
                y = evento.motion.y;

                prevHover = hover;
                hover = -1;

                if (x >= rectaBoton.x && x <= rectaBoton.x + rectaBoton.w &&
                    y >= rectaBoton.y && y <= rectaBoton.y + rectaBoton.h)
                {
                    hover = I;
                }

                if (hover != prevHover)
                    actualizar = 1;
            }
            else if(evento.type == SDL_MOUSEBUTTONDOWN && evento.button.button == SDL_BUTTON_LEFT)
            {
                x = evento.button.x;
                y = evento.button.y;

                if (x >= rectaBoton.x && x <= rectaBoton.x + rectaBoton.w &&
                    y >= rectaBoton.y && y <= rectaBoton.y + rectaBoton.h)
                {
                    if (*nombre)
                    {
                        op = 1;
                        ejecutando = 0;
                    }
                    else {
                        noNombre = 1;
                        actualizar = 1;
                    }
                }
            }
            else if(evento.type == SDL_KEYDOWN)
            {
                if(evento.key.keysym.sym == SDLK_RETURN)
                {
                    if (*nombre)
                    {
                        op = 1;
                        ejecutando = 0;
                    }
                    else {
                        noNombre = 1;
                        actualizar = 1;
                    }
                }
                else if(evento.key.keysym.sym == SDLK_ESCAPE)
                {
                    op = SALIR;
                    ejecutando = 0;
                }
                else if(evento.key.keysym.sym == SDLK_BACKSPACE)
                {
                    *(nombre + MAX(0, strlen(nombre) - 1)) = '\0';
                    actualizar = 1;
                }
            }
            else if (evento.type == SDL_TEXTINPUT)
            {
                //strncat(nombre, evento.text.text, 50 - strlen(nombre));
                strncat(nombre, evento.text.text, 50 - strlen(nombre) - 1);
                noNombre = 0;
                actualizar = 1;
            }
        }

        SDL_Delay(16);
    }

    //strcpy(jugador->nombre, nombre);
    strncpy(jugador->nombre, nombre, sizeof(jugador->nombre) - 1);
    jugador->nombre[sizeof(jugador->nombre) - 1] = '\0';


    SDL_DestroyTexture(texTitulo);
    SDL_DestroyTexture(texDescripcion);

    return op; // 1 = seguir, SALIR (0) = salir
}

int Jugar(Tablero *laberinto, Jugador *jugador, Fantasma *fantasmas, int maxFantasmas,
            ContextoSDL *sdl, tCola *ColaMovimientos, int *cantmovimientos)
{

    char movimiento = 0, aseguradorMovimiento;
    int estado = 1, jugando = 1, tamCeldaReal, winW, winH,
        vpW, vpH, totalWidth, offsetX, tamFuenteHudDeseado;
    float escalaX, escalaY, escala, escalaTexto = 1.0f;
    TTF_Font *fuenteLocal = NULL;
    TTF_Font *fuenteHudOriginal = NULL;
    int tamFuenteHudActual = 0;
    TTF_Font *fuenteHudLocal = NULL;
    SDL_Event evento;
    SDL_Rect vp;
    Mix_Music *musica = NULL;

    if (!laberinto || !jugador || !fantasmas || !sdl || !ColaMovimientos)
        return ERROR_MEMORIA;

    musica = Mix_LoadMUS("assets/plinplinplon.mp3");
    if(!musica)
        printf("\nERROR al cargar la musica: %s", Mix_GetError());
    else if(Mix_PlayMusic(musica, -1) == -1)
        printf("\nERROR al reproducir la musica: %s", Mix_GetError());

    SDL_GetWindowSize(sdl->ventana, &winW, &winH);

    // Inicializar punteros locales a las fuentes iniciales (simplificar)
    if (sdl->fuente)
        fuenteLocal = sdl->fuente;
    else
        fuenteLocal = NULL;

    if (sdl->fuenteHud)
        fuenteHudOriginal = sdl->fuenteHud;
    else
        fuenteHudOriginal = NULL;


    while(jugando && estado != VICTORIA)
    {
        while(SDL_PollEvent(&evento))
        {
            if(evento.type == SDL_QUIT)
                jugando = 0;
            else if (evento.type == SDL_WINDOWEVENT && evento.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                winH = evento.window.data2;
                escalaX = (float) winW / sdl->ancho;
                escalaY = (float) winH / sdl->alto;
                escala = MIN(escalaX, escalaY);
                if (escala <= 0.0f)
                    escala = 1.0f;

                vpW = (int)(sdl->ancho * escala);
                vpH = (int)(sdl->alto * escala);

                vp.x = (winW - vpW) / 2;
                vp.y = (winH - vpH) / 2;
                vp.w = vpW;
                vp.h = vpH;

                SDL_RenderSetViewport(sdl->renderer, &vp);
            }
            else if(evento.type == SDL_KEYDOWN)
            {
                switch(evento.key.keysym.sym)
                {
                    case SDLK_w: movimiento = 'w';
                                 if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                 {
                                    printf("No se pudo realizar un movimiento.");
                                    break;
                                 }
                                 break;
                    case SDLK_s: movimiento = 's';
                                 if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                 {
                                    printf("No se pudo realizar un movimiento.");
                                    break;
                                 }
                                 break;
                    case SDLK_a: movimiento = 'a';
                                 if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                 {
                                    printf("No se pudo realizar un movimiento.");
                                    break;
                                 }
                                 break;
                    case SDLK_d: movimiento = 'd';
                                 if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                 {
                                    printf("No se pudo realizar un movimiento.");
                                    break;
                                 }
                                 break;
                    case SDLK_ESCAPE: jugando = 0;
                                      break;
                    default:     movimiento = 'z';
                                 if(!encolar(ColaMovimientos, &movimiento, sizeof(char)))
                                 {
                                    printf("No se pudo realizar un movimiento.");
                                    break;
                                 }
                                 break;
                }
                if(jugando && desencolar(ColaMovimientos, &aseguradorMovimiento, sizeof(char)))
                {
                    estado = realizarMovimiento(laberinto, jugador, fantasmas, maxFantasmas, aseguradorMovimiento);
                    (*cantmovimientos)++;
                    if(estado == DERROTA)
                        jugando = 0;
                    if(estado != MOV_INVALIDO)
                    {
                        if(moverFantasmas(laberinto, fantasmas, jugador, maxFantasmas) == DERROTA)
                        {
                            estado = DERROTA;
                            jugando = 0;
                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 255);
        SDL_RenderClear(sdl->renderer);

        tamCeldaReal = (sdl->ancho / laberinto->columnas);
        if (tamCeldaReal <= 0)
            tamCeldaReal = 1;

        totalWidth = tamCeldaReal * laberinto->columnas;
        offsetX = (sdl->ancho - totalWidth) / 2;
        if (offsetX < 0)
            offsetX = 0;

        tamFuenteHudDeseado = 35;

        if (tamFuenteHudDeseado != tamFuenteHudActual)
        {
            //Recrear la fuente HUD local
            if (fuenteHudLocal && fuenteHudLocal != fuenteHudOriginal)
            {
                TTF_CloseFont(fuenteHudLocal);
                fuenteHudLocal = NULL;
            }
            fuenteHudLocal = TTF_OpenFont("assets/Sora-Bold.ttf", tamFuenteHudDeseado);
            if (!fuenteHudLocal)
                fuenteHudLocal = fuenteHudOriginal;
            else
                tamFuenteHudActual = tamFuenteHudDeseado;
        }

        //Usar la fuente HUD local si existe, sino la pasada por parametro
        TTF_Font *fuenteParaHUD = (fuenteHudLocal ? fuenteHudLocal : fuenteHudOriginal);

        dibujarTablero(sdl->renderer, laberinto, jugador, fuenteParaHUD, tamCeldaReal, offsetX, escalaTexto);

        SDL_RenderPresent(sdl->renderer);

        SDL_Delay(1000 / 60);
    }

    Mix_HaltMusic();
    Mix_FreeMusic(musica);
    musica = NULL;
    //almacenarJugador(jugador);
    //almacenarPartida(jugador, cantmovimientos);
    if(estado == VICTORIA)
        victoria(sdl, fuenteLocal, jugador->puntaje);
    else if(estado == DERROTA)
        derrota(sdl, fuenteLocal);

    //Liberar la fuente HUD local si fue creada
    if (fuenteHudLocal && fuenteHudLocal != fuenteHudOriginal)
    {
        TTF_CloseFont(fuenteHudLocal);
        fuenteHudLocal = NULL;
    }

    return estado;
}

int realizarMovimiento(Tablero *laberinto, Jugador *jugador, Fantasma *fantasmas, int maxFantasmas, char direccion)
{
    int nuevaFila = jugador->posY,
        nuevaColumna = jugador->posX,
        fantasmaBuscado,
        puntosGanados;

    if (!laberinto || !jugador || !fantasmas || !laberinto->celdas || maxFantasmas <= 0)
        return ERROR_MEMORIA;

    switch(direccion = tolower(direccion))
    {
        case 'w': nuevaFila--;
                  break;
        case 's': nuevaFila++;
                  break;
        case 'a': nuevaColumna--;
                  break;
        case 'd': nuevaColumna++;
                  break;
        default: return MOV_INVALIDO;
    }

    if(nuevaFila < 0 || nuevaFila >= laberinto->filas
        || nuevaColumna < 0 || nuevaColumna >= laberinto->columnas)
        return MOV_INVALIDO;

    if(laberinto->celdas[nuevaFila][nuevaColumna] == '#')
        return MOV_INVALIDO;

    if(laberinto->celdas[nuevaFila][nuevaColumna] == 'S')
        return VICTORIA;

    if(laberinto->celdas[nuevaFila][nuevaColumna] == 'F')
    {
        fantasmaBuscado = encontrarFantasma(fantasmas, maxFantasmas, nuevaColumna, nuevaFila);
        if(fantasmaBuscado != -1)
            fantasmas[fantasmaBuscado].vivo = 0;

        jugador->vidas--;
        if(jugador->vidas <= 0)
            return DERROTA;
    }

    if(laberinto->celdas[nuevaFila][nuevaColumna] == 'V')
        jugador->vidas++;

    if(laberinto->celdas[nuevaFila][nuevaColumna] == 'P')
    {
        puntosGanados = MIN_PUNTOS + rand() % (MAX_PUNTOS - MIN_PUNTOS + 1);
        jugador->puntaje += puntosGanados;
    }


    laberinto->celdas[jugador->posY][jugador->posX] = ' ';
    jugador->posY = nuevaFila;
    jugador->posX = nuevaColumna;
    laberinto->celdas[jugador->posY][jugador->posX] = 'J';

    return MOV_VALIDO;
}

void victoria(ContextoSDL *sdl, TTF_Font *fuente, int puntaje)
{
    SDL_Event evento;
    int salir = 0, ancho, alto;
    char puntajeTexto[50];
    SDL_Rect vp;
    float escalaTexto = 1.0f;

    ancho = sdl->ancho;
    alto = sdl->alto;

    SDL_RenderGetViewport(sdl->renderer, &vp);
    if (vp.w > 0 && ancho > 0)
    {
        escalaTexto = (float)vp.w / (float)ancho;
        if (escalaTexto <= 0.0f)
            escalaTexto = 1.0f;
    }

    SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 80, 255);
    SDL_RenderClear(sdl->renderer);

    sprintf(puntajeTexto, "Puntaje obtenido: %d", puntaje);
    renderizarCentrado(sdl->renderer, fuente, "Victoria!", COLOR_BLANCO, ancho, alto / 2 - 20, escalaTexto);
    renderizarCentrado(sdl->renderer, fuente, puntajeTexto, COLOR_AMARILLO, ancho, alto / 2 + 75, escalaTexto);
    renderizarCentrado(sdl->renderer, fuente, "Presiona ESC para salir...", COLOR_GRIS, ancho, alto + 250, escalaTexto);

    SDL_RenderPresent(sdl->renderer);

    while(!salir)
    {
        while(SDL_PollEvent(&evento))
        {
            if(evento.type == SDL_QUIT)
                salir = 1;
            else if(evento.type == SDL_KEYDOWN && evento.key.keysym.sym == SDLK_ESCAPE)
                salir = 1;
        }
        SDL_Delay(50);
    }
}

void derrota(ContextoSDL *sdl, TTF_Font *fuente)
{
    SDL_Event evento;
    int salir = 0, ancho, alto;
    SDL_Rect vp;
    float escalaTexto = 1.0f;

    ancho = sdl->ancho;
    alto = sdl->alto;

    SDL_RenderGetViewport(sdl->renderer, &vp);
    if (vp.w > 0 && ancho > 0)
    {
        escalaTexto = (float)vp.w / (float)ancho;
        if (escalaTexto <= 0.0f)
            escalaTexto = 1.0f;
    }

    SDL_SetRenderDrawColor(sdl->renderer, 60, 0, 0, 255);
    SDL_RenderClear(sdl->renderer);

    renderizarCentrado(sdl->renderer, fuente, "Game over", COLOR_ROJO, ancho, alto / 2 - 20, escalaTexto);
    renderizarCentrado(sdl->renderer, fuente, "No se obtendran puntos.", COLOR_ROJO, ancho, alto / 2 + 75, escalaTexto);
    renderizarCentrado(sdl->renderer, fuente, "Presiona ESC para salir...", COLOR_GRIS, ancho, alto + 250, escalaTexto);

    SDL_RenderPresent(sdl->renderer);

    while(!salir)
    {
        while(SDL_PollEvent(&evento))
        {
            if(evento.type == SDL_QUIT)
                salir = 1;
            else if(evento.type == SDL_KEYDOWN && evento.key.keysym.sym == SDLK_ESCAPE)
                salir = 1;
        }
        SDL_Delay(50);
    }
}
