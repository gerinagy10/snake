#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "megjelenites.h"
#include "debugmalloc.h"
#include "jatek.h"

SDL_Color feher = {255, 255, 255};
SDL_Color olive = {204, 204, 0};

/* SDL inicializalasa as ablak megnyitasa, parameterkent kapja az ablak magassagat, szelesseget,
az ablakra mutato pointer es a rendererre mutato pointer pointeret. Hibauzenettel ter
vissza, ha valamelyik muvelet nem sikerult. Nem ter vissza semmivel.*/
void sdl_init(int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas, 0);
    if (window == NULL)
    {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL)
    {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }

    *pwindow = window;
    *prenderer = renderer;
}

/* A felirat hosszat szamolja ki, es ezzel ter vissza, parametere a felirat sztringje,
a felirat pointer es a megnyitott betutipus pointere.*/
int felirat_hossz(char const* mit, SDL_Surface *felirat, TTF_Font *font)
{
    felirat = TTF_RenderUTF8_Blended(font, mit, feher);
    return felirat->w;
}

/* A felirat kiirasat vegzi a kepernyore. Parametere a felirat sztringje,
felirat pointere, felirat textura pointere, betutipus pointere, renderer pointere
es a felirat pozicioja (x es y). Nem ter vissza semmivel.*/
void sdl_kiir(char const* mit, SDL_Surface *felirat, SDL_Texture *felirat_t, TTF_Font *font, SDL_Renderer *renderer, int x, int y)
{
    SDL_Rect hova = { 0, 0, 0, 0 };
    felirat = TTF_RenderUTF8_Blended(font, mit, feher);
    felirat_t = SDL_CreateTextureFromSurface(renderer, felirat);
    hova.x = x;
    hova.y = y;
    hova.w = felirat->w;
    hova.h = felirat->h;
    SDL_RenderCopy(renderer, felirat_t, NULL, &hova);
    SDL_FreeSurface(felirat);
    SDL_DestroyTexture(felirat_t);
}

/* A nev beolvasasara szolgal, a nev neveztu karaktertombbe menti el.
Parameterei az ablak pointere, a renderer pointere, a betutipus pointere
es a jatek struktura pointere. Nincs visszateresi erteke. Benne meghivja a tenyleges
beolvasast vegzo fuggvenyt. */
void beolvas(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, Jatek* j1, SDL_Surface *felirat, SDL_Texture *felirat_t)
{
    char nev[15];

    SDL_Surface *screen = SDL_GetWindowSurface(window);
    SDL_Surface *background = SDL_CreateRGBSurface(0, SZELES, MAGAS, 32, 0, 0, 0, 0);

    /* hatter kirajzolasa, mentese */
    boxRGBA(renderer, 0, 0, 720, 480, 175, 175, 0, 255);
    int hossz = felirat_hossz("Adj meg egy nevet:", felirat, font);
    sdl_kiir("Adj meg egy nevet:", felirat, felirat_t, font, renderer, 360 - hossz/2, 100);
    SDL_BlitSurface(screen, NULL, background, NULL);
    SDL_RenderPresent(renderer);

    /* nev bekerese */
    SDL_Rect r = { 160, 160, 400, 40 };
    input_text(nev, 10, r, olive, feher, font, renderer);

    /* nev kirajzolasa */
    if (nev[0] != 0x0000)
    {
        SDL_BlitSurface(background, NULL, screen, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_FreeSurface(background);

    strcpy(j1->nev, nev);
}

/* Beolvas egy szoveget a billentyuzetrol.
 * A rajzolashoz hasznalt font es a megjelenito az utolso parameterek.
 * Az elso a tomb, ahova a beolvasott szoveg kerul.
 * A masodik a maximális hossz, ami beolvasható.
 * A visszateresi erteke logikai igaz, ha sikerult a beolvasas. */
bool input_text(char *dest, size_t hossz, SDL_Rect teglalap, SDL_Color hatter, SDL_Color szoveg, TTF_Font *font, SDL_Renderer *renderer)
{
    /* Ez tartalmazza az aktualis szerkesztest */
    char composition[SDL_TEXTEDITINGEVENT_TEXT_SIZE];
    composition[0] = '\0';
    /* Ezt a kirajzolas kozben hasznaljuk */
    char textandcomposition[hossz + SDL_TEXTEDITINGEVENT_TEXT_SIZE + 1];
    /* Max hasznalhato szelesseg */
    int maxw = teglalap.w - 2;
    int maxh = teglalap.h - 2;

    dest[0] = '\0';

    bool enter = false;
    bool kilep = false;

    SDL_StartTextInput();
    while (!kilep && !enter)
    {
        /* doboz kirajzolasa */
        boxRGBA(renderer, teglalap.x, teglalap.y, teglalap.x + teglalap.w - 1, teglalap.y + teglalap.h - 1, hatter.r, hatter.g, hatter.b, 255);
        rectangleRGBA(renderer, teglalap.x, teglalap.y, teglalap.x + teglalap.w - 1, teglalap.y + teglalap.h - 1, szoveg.r, szoveg.g, szoveg.b, 255);
        /* nev kirajzolasa */
        int w;
        strcpy(textandcomposition, dest);
        strcat(textandcomposition, composition);
        if (textandcomposition[0] != '\0')
        {
            SDL_Surface *felirat = TTF_RenderUTF8_Blended(font, textandcomposition, szoveg);
            SDL_Texture *felirat_t = SDL_CreateTextureFromSurface(renderer, felirat);
            SDL_Rect cel = { teglalap.x, teglalap.y, felirat->w < maxw ? felirat->w : maxw, felirat->h < maxh ? felirat->h : maxh };
            SDL_RenderCopy(renderer, felirat_t, NULL, &cel);
            SDL_FreeSurface(felirat);
            SDL_DestroyTexture(felirat_t);
            w = cel.w;
        }
        else
        {
            w = 0;
        }
        /* kurzor kirajzolasa */
        if (w < maxw)
        {
            vlineRGBA(renderer, teglalap.x + w + 2, teglalap.y + 2, teglalap.y + teglalap.h - 3, szoveg.r, szoveg.g, szoveg.b, 192);
        }
        /* megjeleniti a képernyon az eddig rajzoltakat */
        SDL_RenderPresent(renderer);

        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type)
        {
        /* Kulonleges karakter */
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_BACKSPACE)
            {
                int textlen = strlen(dest);
                do
                {
                    if (textlen == 0)
                    {
                        break;
                    }
                    if ((dest[textlen-1] & 0x80) == 0x00)
                    {
                        /* Egy bajt */
                        dest[textlen-1] = 0x00;
                        break;
                    }
                    if ((dest[textlen-1] & 0xC0) == 0x80)
                    {
                        /* Bajt, egy tobb-bajtos szekvenciabol */
                        dest[textlen-1] = 0x00;
                        textlen--;
                    }
                    if ((dest[textlen-1] & 0xC0) == 0xC0)
                    {
                        /* Egy tobb-bajtos szekvencia elso bajtja */
                        dest[textlen-1] = 0x00;
                        break;
                    }
                }
                while(true);
            }
            if (event.key.keysym.sym == SDLK_RETURN)
            {
                enter = true;
            }
            break;

        /* A feldolgozott szoveg bemenete */
        case SDL_TEXTINPUT:
            if (strlen(dest) + strlen(event.text.text) < hossz)
            {
                strcat(dest, event.text.text);
            }

            /* Az eddigi szerkesztes torolheto */
            composition[0] = '\0';
            break;

        /* Szoveg szerkesztese */
        case SDL_TEXTEDITING:
            strcpy(composition, event.edit.text);
            break;

        case SDL_QUIT:
            /* visszatesszuk a sorba ezt az eventet, mert
             * sok mindent nem tudunk vele kezdeni */
            SDL_PushEvent(&event);
            kilep = true;
            break;
        }
    }

    /* igaz jelzi a helyes beolvasast; = ha enter miatt allt meg a ciklus */
    SDL_StopTextInput();
    return enter;
}

/*Ha vege van egy jateknak, ahol egy jatekos volt, ez hivodik meg, kiirja a kepernyore
a nevet, es a szerzett pontszamot, tovabba az "uj jatek" es "kilepes" gombokat.
Kattintassal lehet valasztani kozuluk. Parametere a felirat pointere, felirat textura pointere,
betutipus pointere, renderer pointere, Jatek pointere es az ablak pointere.
Nem ter vissza semmivel.*/
void egyjatekos_vege(SDL_Surface *felirat, SDL_Texture *felirat_t, TTF_Font *font, SDL_Renderer *renderer, Jatek* j, SDL_Window *window)
{
    mentes(j);
    char gameover[60];
    sprintf(gameover, "game over %s, szerzett pont: %2d", j->nev, j->pontszam);

    boxRGBA(renderer, 0, 0, 720, 480, 175, 175, 0, 255);

    int hossz_g = felirat_hossz(gameover, felirat, font);
    sdl_kiir(gameover, felirat, felirat_t, font, renderer, 360 - hossz_g/2, 150);

    int hossz_u = felirat_hossz("uj jatek", felirat, font);
    sdl_kiir("Uj jatek", felirat, felirat_t, font, renderer, 360 - hossz_u - 50, 320);

    int hossz_k = felirat_hossz("Kilepes", felirat, font);
    sdl_kiir("Kilepes", felirat, felirat_t, font, renderer, 360 + 50, 320);

    SDL_RenderPresent(renderer);

    SDL_Event klikk;

    bool valaszt = false;

    while(!valaszt)
    {
        SDL_WaitEvent(&klikk);

        switch (klikk.type)
        {
        case SDL_MOUSEBUTTONDOWN:
            if((klikk.button.x >= 360-hossz_u-50 && klikk.button.x <= 360 - 50) && (klikk.button.y >= 320 && klikk.button.y <= 320 + 32))
            {
                valaszt = true;
                egyjatekos_mod(felirat, felirat_t, font, renderer, window);
            }
            else if((klikk.button.x >= 360 + 50 && klikk.button.x <= 360 + 50 + hossz_k) && (klikk.button.y >= 320 && klikk.button.y <= 320 + 32))
            {
                valaszt = true;
                SDL_Quit();
            }
            break;

        case SDL_QUIT:
            valaszt = true;
            SDL_Quit();
            break;
        }
    }
}

/*Ha vege van egy jateknak, ahol ket jatekos volt, ez hivodik meg, kiirja a kepernyore
gyoztest vagy azt, hogy dontetlen, tovabba az "uj jatek" es "kilepes" gombokat.
Kattintassal lehet valasztani kozuluk. Parametere a felirat pointere, felirat textura pointere,
betutipus pointere, renderer pointere, Jatek pointere, a gyoztes es az ablak pointere.
Nem ter vissza semmivel. */
void ketjatekos_vege(SDL_Surface *felirat, SDL_Texture *felirat_t, TTF_Font *font, SDL_Renderer *renderer, Jatek* j, SDL_Window *window, Gyoztes gyoztes)
{

    boxRGBA(renderer, 0, 0, 720, 480, 175, 175, 0, 255);

    char gameover[60];

    switch(gyoztes)
    {
    case p1:
        sprintf(gameover, "Game over, player 1 (WASD) nyert");
        break;
    case p2:
        sprintf(gameover, "Game over, player 2 (nyilak) nyert");
        break;
    case dontetlen:
        sprintf(gameover, "Game over, dontetlen lett");
        break;
    }

    int hossz = felirat_hossz(gameover, felirat, font);
    sdl_kiir(gameover, felirat, felirat_t, font, renderer, 360 - hossz/2, 150);

    int hossz_u = felirat_hossz("uj jatek", felirat, font);
    sdl_kiir("Uj jatek", felirat, felirat_t, font, renderer, 360 - hossz_u - 50, 320);

    int hossz_k = felirat_hossz("Kilepes", felirat, font);
    sdl_kiir("Kilepes", felirat, felirat_t, font, renderer, 360 + 50, 320);

    SDL_RenderPresent(renderer);

    SDL_Event klikk;

    bool valaszt = false;

    while(!valaszt)
    {
        SDL_WaitEvent(&klikk);

        switch (klikk.type)
        {
        case SDL_MOUSEBUTTONDOWN:
            if((klikk.button.x >= 360-hossz_u-50 && klikk.button.x <= 360 - 50) && (klikk.button.y >= 320 && klikk.button.y <= 320 + 32))
            {
                valaszt = true;
                ketjatekos_mod(felirat, felirat_t, font, renderer, window);
            }
            else if((klikk.button.x >= 360 + 50 && klikk.button.x <= 360 + 50 + hossz_k) && (klikk.button.y >= 320 && klikk.button.y <= 320 + 32))
            {
                valaszt = true;
                SDL_Quit();
            }
            break;

        case SDL_QUIT:
            valaszt = true;
            SDL_Quit();
            break;
        }
    }
}

/*A ranglista megjeleniteset vegzi, a ranglista.txt fajlbol kiolvassa a top 5 eredmenyt,
es kiirja a kepernyore. Parametere a felirat pointere, felirat textura pointere,
betutipus pointere, renderer pointere es az ablak pointere. Nem ter vissza semmivel.*/
void ranglista_megjelenit(SDL_Surface *felirat, SDL_Texture *felirat_t, TTF_Font *font, SDL_Renderer *renderer, SDL_Window *window)
{

    FILE *fp;
    fp = fopen("ranglista.txt", "r");
    if (fp == NULL)
    {
        perror("Fájl megnyitása sikertelen");
        return;
    }

    Eredmenyek ranglista[5];

    for(int i = 0; i<5; ++i)
    {
        fscanf(fp, "%s", ranglista[i].nev);
        fscanf(fp, "%d", &ranglista[i].pontszam);
    }
    fclose(fp);

    boxRGBA(renderer, 0, 0, 720, 480, 175, 175, 0, 255);
    rectangleRGBA(renderer, 150, 100, 570, 380, 255, 255, 255, 255);
    int hossz_r = felirat_hossz("Ranglista", felirat, font);
    sdl_kiir("Ranglista", felirat, felirat_t, font, renderer, 360 - hossz_r/2, 56);

    char eredmeny[24];
    for(int i = 0; i < 5; ++i)
    {
        sprintf(eredmeny, "%s %2d", ranglista[i].nev, ranglista[i].pontszam);
        sdl_kiir(eredmeny, felirat, felirat_t, font, renderer, 180, 130+i*47);
    }

    SDL_RenderPresent(renderer);
}
