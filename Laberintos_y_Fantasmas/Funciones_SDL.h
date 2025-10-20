#ifndef FUNCIONES_SDL_H_INCLUDED
#define FUNCIONES_SDL_H_INCLUDED

#include "Macros.h"

int inicializarSDL(ContextoSDL *sdl, const Configuracion *config);
void destruirSDL(ContextoSDL *sdl);
int renderizarTexto(SDL_Renderer *renderer, TTF_Font *fuente,
                    const char *mensaje, SDL_Color color, int ancho, int alto, int tiempo);
void renderizarHUD(SDL_Renderer *renderer, TTF_Font *fuente, const char *mensaje,
                    SDL_Color color, int x, int y, float escala);
void renderizarCentrado(SDL_Renderer *renderer, TTF_Font *fuente, const char *texto,
                        SDL_Color color, int ancho, int alto, float escala);


#endif // FUNCIONES_SDL_H_INCLUDED
