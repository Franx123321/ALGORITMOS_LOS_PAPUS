#include "Funciones_SDL.h"

//FUNCIONES SDL//
int inicializarSDL(ContextoSDL *sdl, const Configuracion *config)
{
    float escalaX, escalaY, escala;
    int ancho, alto, base, tamMedio, flags, inicializado;
    SDL_Surface *surSprites;
    SDL_RWops *streamSprites;

    if(!sdl || !config)
    {
        printf("\nArgumentos invalidos.");
        return ERROR_SDL;
    }

    ancho = MAX(INI_ANCHO, config->columnas * TAM_CELDA);
    alto = MAX(INI_ALTO, config->filas * TAM_CELDA + MARGEN);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("\nERROR al inicializar el sistema grafico: %s", SDL_GetError());
        return ERROR_SDL;
    }

    if (TTF_Init() < 0)
    {
        printf("\nERROR al inicializar el sistema de fuentes: %s", TTF_GetError());
        SDL_Quit();
        return ERROR_SDL;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    sdl->ventana = SDL_CreateWindow("Laberintos y Fantasmas", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                ancho, alto, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!sdl->ventana)
    {
        printf("\nERROR al crear la ventana: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return ERROR_SDL;
    }

    sdl->renderer = SDL_CreateRenderer(sdl->ventana, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!sdl->renderer)
    {
        printf("\nERROR al crear renderizador: %s", SDL_GetError());
        SDL_DestroyWindow(sdl->ventana);
        TTF_Quit();
        SDL_Quit();
        return ERROR_SDL;
    }

    streamSprites = SDL_RWFromFile("assets/images/SpriteAtlas.bmp", "rb");
    if (!streamSprites)
    {
        printf("\nERROR al abrir archivo de sprites: %s", SDL_GetError());

        SDL_DestroyRenderer(sdl->renderer);
        SDL_DestroyWindow(sdl->ventana);
        TTF_Quit();
        SDL_Quit();
        return ERROR_SDL;
    }

    surSprites = SDL_LoadBMP_RW(streamSprites, 1);
    if (!surSprites)
    {
        printf("\nERROR al leer archivo de sprites: %s", SDL_GetError());

        SDL_RWclose(streamSprites);
        SDL_DestroyRenderer(sdl->renderer);
        SDL_DestroyWindow(sdl->ventana);
        TTF_Quit();
        SDL_Quit();
        return ERROR_SDL;
    }

    sdl->sprites = SDL_CreateTextureFromSurface(sdl->renderer, surSprites);
    SDL_FreeSurface(surSprites);

    flags = MIX_INIT_MP3 | MIX_INIT_OGG;
    inicializado = Mix_Init(flags);

    if((inicializado & flags) != flags)
    {
        printf("\nERROR al inicializar SDL_mixer: %s",Mix_GetError());
        SDL_DestroyTexture(sdl->sprites);
        SDL_DestroyRenderer(sdl->renderer);
        SDL_DestroyWindow(sdl->ventana);
        TTF_Quit();
        SDL_Quit();
        return ERROR_SDL;
    }

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        printf("\nERROR al inicializar el audio: %s",Mix_GetError());
        Mix_Quit();
        SDL_DestroyTexture(sdl->sprites);
        SDL_DestroyRenderer(sdl->renderer);
        SDL_DestroyWindow(sdl->ventana);
        TTF_Quit();
        SDL_Quit();
        return ERROR_SDL;
    }
/*
    if(ancho > 0 && alto > 0)
        SDL_RenderSetLogicalSize(sdl->renderer, ancho, alto);
*/
    SDL_RenderSetIntegerScale(sdl->renderer, SDL_FALSE);
    SDL_SetWindowMinimumSize(sdl->ventana, MIN_ANCHO, MIN_ALTO);

    base = MIN(ancho, alto);
    tamMedio = (config->columnas + config->filas) / 2;
    if(tamMedio <= 0)
        tamMedio = 1;

    sdl->ancho = ancho;
    sdl->alto = alto;
    sdl->tamFuente = (int) (base / (tamMedio * 0.2));
    sdl->tamFuenteHud = (int) (base / (tamMedio * 1.8));

    if(sdl->tamFuente < 12)
        sdl->tamFuente = 12; //Tamaño minimo de fuente para que se lea bien
    if(sdl->tamFuente > 48)
        sdl->tamFuente = 48; //Tamaño maximo de fuente para que no sea gigante
    if(sdl->tamFuenteHud < 8)
        sdl->tamFuenteHud = 8; //Lo mismo
    if(sdl->tamFuenteHud > 32)
        sdl->tamFuenteHud = 32; //Lo mismisimo

    sdl->fuente = TTF_OpenFont("assets/Sora-Bold.ttf", sdl->tamFuente);
    if(!sdl->fuente)
    {
        printf("\nERROR al cargar una fuente: %s", TTF_GetError());
        Mix_CloseAudio();
        SDL_DestroyTexture(sdl->sprites);
        SDL_DestroyRenderer(sdl->renderer);
        SDL_DestroyWindow(sdl->ventana);
        Mix_Quit();
        TTF_Quit();
        SDL_Quit();
        return ERROR_SDL;
    }

    sdl->fuenteHud = TTF_OpenFont("assets/Sora-Bold.ttf", sdl->tamFuenteHud);
    if(!sdl->fuenteHud)
    {
        printf("\nERROR al cargar una fuente: %s", TTF_GetError());

        TTF_CloseFont(sdl->fuente);
        sdl->fuente = NULL;

        Mix_CloseAudio();
        SDL_DestroyTexture(sdl->sprites);
        SDL_DestroyRenderer(sdl->renderer);
        SDL_DestroyWindow(sdl->ventana);
        Mix_Quit();
        TTF_Quit();
        SDL_Quit();
        return ERROR_SDL;
    }

    return TODO_BIEN;
}

int renderizarTexto(SDL_Renderer *renderer, TTF_Font *fuente, const char *mensaje, SDL_Color color, int ancho, int alto, int tiempo)
{
    SDL_Rect destino;
    SDL_Surface *superficie = NULL;
    SDL_Texture *texto = NULL;

    if(!renderer || !fuente || !mensaje)
        return ERROR_SDL;

    superficie = TTF_RenderText_Blended(fuente, mensaje, color);
    if(!superficie)
    {
        printf("\nERROR al renderizar un texto: %s", TTF_GetError());
        return ERROR_SDL;
    }

    texto = SDL_CreateTextureFromSurface(renderer, superficie);
    if(!texto)
    {
        printf("\nERROR al crear una textura: %s", SDL_GetError());
        SDL_FreeSurface(superficie);
        return ERROR_SDL;
    }


    destino.w = superficie->w > 0 ? superficie->w : 1;
    destino.h = superficie->h > 0 ? superficie->h : 1;
    destino.x = (ancho - destino.w) / 2;
    destino.y = (alto - destino.h) / 2;

    SDL_FreeSurface(superficie);
    SDL_SetTextureBlendMode(texto, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texto, NULL, &destino);
    SDL_RenderPresent(renderer);

    if(tiempo > 0)
        SDL_Delay(tiempo);

    SDL_DestroyTexture(texto);
    return TODO_BIEN;
}

void renderizarHUD(SDL_Renderer *renderer, TTF_Font *fuente, const char *mensaje, SDL_Color color, int x, int y, float escala)
{
    SDL_Surface *superficie = NULL;
    SDL_Texture *texto = NULL;
    SDL_Rect destino;
    int ancho_texto = 0, alto_texto = 0;

    if(!fuente || !mensaje || !renderer)
        return;


    //Renderizar texto con TTF blended para mejor calidad
    if(superficie)
        SDL_FreeSurface(superficie);
    superficie = TTF_RenderText_Blended(fuente, mensaje, color);

    texto = SDL_CreateTextureFromSurface(renderer, superficie);
    SDL_FreeSurface(superficie);
    if (!texto)
    {
        printf("\nERROR al crear una textura: %s", SDL_GetError());
        return;
    }

    SDL_SetTextureBlendMode(texto, SDL_BLENDMODE_BLEND);

    SDL_QueryTexture(texto, NULL, NULL, &ancho_texto, &alto_texto);

    //Ajustar posición y tamaño según escala requerida
    if(escala <= 0)
        escala = 1;

    destino.w = (int)(ancho_texto * escala);
    destino.h = (int)(alto_texto * escala);

    if(destino.w <= 0)
        destino.w = 1;
    if(destino.h <= 0)
        destino.h = 1;

    destino.x = x;
    destino.y = y;

    SDL_RenderCopy(renderer, texto, NULL, &destino);
    SDL_DestroyTexture(texto);
}

void renderizarCentrado(SDL_Renderer *renderer, TTF_Font *fuente, const char *texto, SDL_Color color, int ancho, int alto, float escala)
{
    int ancho_texto = 0, alto_texto = 0, x, y;

    if(!fuente || !texto)
        return;

    TTF_SizeText(fuente, texto, &ancho_texto, &alto_texto);

    if(ancho_texto <= 0)
        ancho_texto = 1;

    x = (ancho - (int)(ancho_texto * escala)) / 2;
    y = (alto - (int)(alto_texto * escala)) / 2;

    renderizarHUD(renderer, fuente, texto, color, x, y, escala);
}

void destruirSDL(ContextoSDL *sdl)
{
    // Es seguro cerrar fuentes NULL.
    TTF_CloseFont(sdl->fuente);
    sdl->fuente = NULL;

    TTF_CloseFont(sdl->fuenteHud);
    sdl->fuenteHud = NULL;

    if(sdl->sprites)
    {
        SDL_DestroyTexture(sdl->sprites);
        sdl->sprites = NULL;
    }

    if(sdl->renderer)
    {
        SDL_DestroyRenderer(sdl->renderer);
        sdl->renderer = NULL;
    }

    if(sdl->ventana)
    {
        SDL_DestroyWindow(sdl->ventana);
        sdl->ventana = NULL;
    }

    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}
