#ifndef MEGJELENITES_H
#define MEGJELENITES_H

#define MAGAS 480
#define SZELES 720
#define FONT "alger.ttf"

#include "jatek.h"

void sdl_init(int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer);

int felirat_hossz(char const* mit, SDL_Surface *felirat, TTF_Font *font);

void sdl_kiir(char const* mit, SDL_Surface *felirat, SDL_Texture *felirat_t, TTF_Font *font, SDL_Renderer *renderer,int x, int y);

bool input_text(char *dest, size_t hossz, SDL_Rect teglalap, SDL_Color hatter, SDL_Color szoveg, TTF_Font *font, SDL_Renderer *renderer);

void beolvas(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Jatek* j1, SDL_Surface *felirat, SDL_Texture *felirat_t);

void egyjatekos_vege(SDL_Surface *felirat, SDL_Texture *felirat_t, TTF_Font *font, SDL_Renderer *renderer, Jatek* j, SDL_Window *window);

void ketjatekos_vege(SDL_Surface *felirat, SDL_Texture *felirat_t, TTF_Font *font, SDL_Renderer *renderer, Jatek* j, SDL_Window *window, Gyoztes gyoztes);

void ranglista_megjelenit(SDL_Surface *felirat, SDL_Texture *felirat_t, TTF_Font *font, SDL_Renderer *renderer, SDL_Window *window);

#endif
