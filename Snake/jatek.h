#ifndef JATEK_H
#define JATEK_H

//Egy nev es egy pontszam van benne.
typedef struct Eredmenyek
{
    char nev[25];
    int pontszam;
} Eredmenyek;

//A lehetseges gyoztesek, elso jatekos (p1), masodik (p2) vagy dontetlen.
typedef enum Gyoztes
{
    p1, p2, dontetlen
} Gyoztes;

// A kigyo egy reszenek az aktualis x es y koordinatajat tartalmazza.
typedef struct Kigyo
{
    int x, y;
    struct Kigyo *kov;
} Kigyo;

/*A palya mezoinek lehetseges allapotai, ures, kigyo/kigyofej van rajta,
fal, etek van rajta.*/
typedef enum mezo
{
    m_ures, m_kigyo, m_kigyofej, m_fal, m_etek
} mezo;

// Pozicio a palyan, x es y koordinata.
typedef struct Pozicio
{
    int x, y;
} Pozicio;

// A mozgas iranya.
typedef enum Irany
{
    fel, le, jobbra, balra
} Irany;

/* A jatekrol minden tudnivalo. Tartalmaz egy 2D-s palya nevu tombot,
melynek ertekei a mezok allapotai lehetnek. Van meg benne egy (ket jatekos eseten ket)
kigyo, es az egy jatekos eseten beirt nev.*/
typedef struct Jatek
{
    mezo palya[24][36]; //egy mezo 20x20-as, igy a magassag 480/20, a szelesseg 720/20
    Kigyo *fej;
    Irany irany;
    char nev[25];
    int pontszam;
    Kigyo *fej2;
    Irany irany2;
} Jatek;

void palya_betolt(Jatek* j1, bool ketjatekos);

void palyamezo_rajz(int y, int x, SDL_Renderer *renderer, Jatek* j1);

void palya_torol(Jatek* j1);

void palya_kirajzol(Jatek* j1, SDL_Renderer *renderer, bool ketjatekos);

void alma_terem(Jatek* j1, SDL_Renderer *renderer, Pozicio *alma);

void egyjatekos_mod(SDL_Surface *felirat, SDL_Texture *felirat_t, TTF_Font *font, SDL_Renderer *renderer, SDL_Window *window);

void ketjatekos_mod(SDL_Surface *felirat, SDL_Texture *felirat_t, TTF_Font *font, SDL_Renderer *renderer, SDL_Window *window);

void mentes(Jatek* j1);

int top5_e(Eredmenyek* ranglista, Jatek* j1);

#endif
