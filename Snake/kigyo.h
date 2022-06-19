#ifndef KIGYO_H
#define KIGYO_H

#include "jatek.h"

void kigyo_kirajzol(Jatek* j1, SDL_Renderer* renderer);

void kigyo_mozgat(Kigyo* fej, Irany irany, mezo (*palya)[36]);

void kigyo_felszabadit(Kigyo* fej);

bool fal_v_kigyo_e(Kigyo* fej, Irany irany, mezo (*palya)[36]);

bool dontetlen_e(Jatek* j1);

Uint32 idozit(Uint32 ms, void *param);

void kigyo_novel(Kigyo* fej, Irany irany, mezo (*palya)[36]);

Gyoztes iranyit(SDL_Renderer *renderer, Jatek* j1, bool ketjatekos);

#endif
