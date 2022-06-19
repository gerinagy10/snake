#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include <stdbool.h>
#include "kigyo.h"
#include "jatek.h"
#include "debugmalloc.h"

/*Kirajzolja a jatek aktualis allapotat, a palya minden mezojet.
Parametere a Jatek es a renderer pointere, nem ter vissza semmivel. */
void kigyo_kirajzol(Jatek* j1, SDL_Renderer* renderer)
{
    for(int i=0; i<24; ++i)
    {
        for(int o=0; o<36; ++o)
            palyamezo_rajz(i, o, renderer, j1);
    }
    SDL_RenderPresent(renderer);
}

/*A kigyo mozgatasat vegzo fuggveny, parameterkent kapja a fej es a palya pointeret,
illetve a haladasi iranyt. Nem ter vissza semmivel.*/
void kigyo_mozgat(Kigyo* fej, Irany irany, mezo (*palya)[36])
{
    Kigyo* mozgo = fej;

    int elozo_x, elozo_y, kov_x, kov_y;
    if(mozgo->kov == NULL)
        palya[fej->y][fej->x] = m_ures;
    switch(irany)
    {
    case jobbra:
        elozo_x = mozgo->x;
        elozo_y = mozgo->y;

        while(mozgo->kov != NULL)
        {
            kov_x = mozgo->kov->x;
            kov_y = mozgo->kov->y;
            palya[mozgo->kov->y][mozgo->kov->x] = m_ures;
            mozgo->kov->x = elozo_x;
            mozgo->kov->y = elozo_y;
            palya[mozgo->kov->y][mozgo->kov->x] = m_kigyo;
            elozo_x = kov_x;
            elozo_y = kov_y;
            mozgo = mozgo->kov;
        }
        fej->x += 1;
        palya[fej->y][fej->x] = m_kigyofej;
        break;

    case balra:
        elozo_x = mozgo->x;
        elozo_y = mozgo->y;

        while(mozgo->kov != NULL)
        {
            kov_x = mozgo->kov->x;
            kov_y = mozgo->kov->y;
            palya[mozgo->kov->y][mozgo->kov->x] = m_ures;
            mozgo->kov->x = elozo_x;
            mozgo->kov->y = elozo_y;
            palya[mozgo->kov->y][mozgo->kov->x] = m_kigyo;
            elozo_x = kov_x;
            elozo_y = kov_y;
            mozgo = mozgo->kov;
        }
        fej->x -= 1;
        palya[fej->y][fej->x] = m_kigyofej;
        break;

    case fel:
        elozo_x = mozgo->x;
        elozo_y = mozgo->y;

        while(mozgo->kov != NULL)
        {
            kov_x = mozgo->kov->x;
            kov_y = mozgo->kov->y;
            palya[mozgo->kov->y][mozgo->kov->x] = m_ures;
            mozgo->kov->x = elozo_x;
            mozgo->kov->y = elozo_y;
            palya[mozgo->kov->y][mozgo->kov->x] = m_kigyo;
            elozo_x = kov_x;
            elozo_y = kov_y;
            mozgo = mozgo->kov;
        }
        fej->y -= 1;
        palya[fej->y][fej->x] = m_kigyofej;
        break;

    case le:
        elozo_x = mozgo->x;
        elozo_y = mozgo->y;

        while(mozgo->kov != NULL)
        {
            kov_x = mozgo->kov->x;
            kov_y = mozgo->kov->y;
            palya[mozgo->kov->y][mozgo->kov->x] = m_ures;
            mozgo->kov->x = elozo_x;
            mozgo->kov->y = elozo_y;
            palya[mozgo->kov->y][mozgo->kov->x] = m_kigyo;
            elozo_x = kov_x;
            elozo_y = kov_y;
            mozgo = mozgo->kov;
        }
        fej->y += 1;
        palya[fej->y][fej->x] = m_kigyofej;
        break;
    }
}

/*A kigyo noveleset vegzi, dinamikusan foglalt lancolt lista vegehez fuz
hozza ujabb elemeket. Felszabaditani a hivonak kell! Nem ter vissza semmivel.*/
void kigyo_novel(Kigyo* fej, Irany irany, mezo (*palya)[36])
{
    Kigyo* uj = (Kigyo*) malloc(sizeof(Kigyo));
    if(uj == NULL)
    {
        printf("A memoriafoglalas nem sikerult!");
        return;
    }
    Kigyo* mozgo = fej;

    while(mozgo->kov != NULL)
        mozgo = mozgo->kov;
    mozgo->kov = uj;
    uj->kov = NULL;

    switch(irany)
    {
    case jobbra:
        uj->x = mozgo->x-1;
        uj->y = mozgo->y;
        palya[uj->y][uj->x] = m_kigyo;
        break;

    case balra:
        uj->x = mozgo->x+1;
        uj->y = mozgo->y;
        palya[uj->y][uj->x] = m_kigyo;
        break;

    case fel:
        uj->x = mozgo->x;
        uj->y = mozgo->y+1;
        palya[uj->y][uj->x] = m_kigyo;
        break;

    case le:
        uj->x = mozgo->x;
        uj->y = mozgo->y-1;
        palya[uj->y][uj->x] = m_kigyo;
        break;
    }
}

/*A kigyot, azaz a dinamikusan foglalt lancolt listat szabaditja fel,
hogy ne legyen memoriaszivargas. Parametere a fej pointere, nem ter vissza semmivel.*/
void kigyo_felszabadit(Kigyo* fej)
{
    Kigyo* mozgo = fej;
    while (mozgo != NULL)
    {
        Kigyo *kov = mozgo->kov; /* következõ elem */
        free(mozgo);
        mozgo = kov;
    }
}

/* Megvizsgalja, hogy falnak vagy kigyonak utkozes tortent-e, eszerint ter
vissza igazzal vagy hamissal. Parametere a fej es a palya pointere, es
az aktualis irany.*/
bool fal_v_kigyo_e(Kigyo* fej, Irany irany, mezo (*palya)[36])
{
    switch(irany)
    {
    case jobbra:
        if(fej->x == 34 || palya[fej->y][fej->x + 1] == m_kigyo || palya[fej->y][fej->x + 1] == m_kigyofej) // megvizsgalja, hogy falhoz ert-e
            return true;
        break;
    case balra:
        if(fej->x == 1 || palya[fej->y][fej->x - 1] == m_kigyo || palya[fej->y][fej->x - 1] == m_kigyofej)
            return true;
        break;
    case fel:
        if(fej->y == 1 || palya[fej->y - 1][fej->x] == m_kigyo || palya[fej->y - 1][fej->x] == m_kigyofej)
            return true;
        break;
    case le:
        if(fej->y == 22 || palya[fej->y + 1][fej->x] == m_kigyo || palya[fej->y + 1][fej->x] == m_kigyofej)
            return true;
        break;
    }
    return false;
}

/*Ketjatekos mod eseten van meghivva, a ket fej esetleges szembol utkozeset vizsgalja.
Ha ilyen tortenik, akkor igazzal ter vissza, mert dontetlen, ha nem, akkor false.
Parametere a Jatek pointere. */
bool dontetlen_e(Jatek* j1)
{
    switch(j1->irany)
    {
    case fel:
        if(j1->irany2 == le && j1->fej->x == j1->fej2->x && j1->fej->y-1 == j1->fej2->y)
        {
            return true;
        }
        break;
    case le:
        if(j1->irany2 == fel && j1->fej->x == j1->fej2->x && j1->fej->y+1 == j1->fej2->y)
        {
            return true;
        }
        break;
    case jobbra:
        if(j1->irany2 == balra && j1->fej->y == j1->fej2->y && j1->fej->x+1 == j1->fej2->x)
        {
            return true;
        }
        break;
    case balra:
        if(j1->irany2 == jobbra && j1->fej->y == j1->fej2->y && j1->fej->x-1 == j1->fej2->x)
        {
            return true;
        }
        break;
    }
    return false;
}

/* Ez az SDL idozitoje altal meghivott fuggveny, parameterkent kapja, hogy milyen idokozokre lett beallitva,
es egy tetszoleges felhasznaloi parametert, ha ez nem kell a tovabbiakban, lehet NULL.
Visszateresi erteke egy egesz szam, hogy legkozelebb hany ezredmasodperc mulva hivodjon meg. */
Uint32 idozit(Uint32 ms, void *param)
{
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;   /* ujabb varakozas */
}

/* Az iranyitast ez biztositja. A kepernyore kirajzolo fuggvenyek
es a kigyo mozgasat biztosito fuggvenyek is itt hivodnak meg.
Egy- es ketjatekos mod eseten is hasznalhato. Az alma_terem fuggveny
is itt hivodik meg. Visszater a gyoztessel (ez ket jatekos eseten szukseges),
parameterei a renderer pointer, ketjatekos bool es a Jatek pointer. */
Gyoztes iranyit(SDL_Renderer *renderer, Jatek* j1, bool ketjatekos)
{
    Gyoztes gyoztes;
    Pozicio alma;
    alma_terem(j1, renderer, &alma);
    SDL_TimerID id = SDL_AddTimer(105, idozit, NULL);
    bool quit = false;
    bool meghal = false;
    while(!quit && !meghal)
    {
        SDL_Event event;
        SDL_WaitEvent(&event);

        // Ha az alma pozicioja megegyezik a kigyo fejenek poziciojaval, uj alma terem.
        if(alma.x == j1->fej->x && alma.y == j1->fej->y)
        {
            if(!ketjatekos)
                j1->pontszam += 1;
            kigyo_novel(j1->fej, j1->irany, j1->palya);
            alma_terem(j1, renderer, &alma);
        }

        else if(ketjatekos && alma.x == j1->fej2->x && alma.y == j1->fej2->y)
        {
            kigyo_novel(j1->fej2, j1->irany2, j1->palya);
            alma_terem(j1, renderer, &alma);
        }

        switch (event.type)
        {

        case SDL_USEREVENT: // felhasznaloi esemeny: ilyeneket general az idozito fuggveny

            if(ketjatekos)
            {
                if(dontetlen_e(j1))
                {
                    meghal = true;
                    gyoztes = dontetlen;
                    break;
                }

                else if (fal_v_kigyo_e(j1->fej2, j1->irany2, j1->palya))
                {
                    meghal = true;
                    gyoztes = p1;
                    break;
                }
                kigyo_mozgat(j1->fej2, j1->irany2, j1->palya);
            }

            if(fal_v_kigyo_e(j1->fej, j1->irany, j1->palya))
            {
                meghal = true;
                gyoztes = p2;
                break;
            }

            kigyo_mozgat(j1->fej, j1->irany, j1->palya);
            kigyo_kirajzol(j1, renderer);
            break;

        case SDL_KEYDOWN: // gombnyomas
            switch (event.key.keysym.sym)
            {
            case SDLK_w:
                if(j1->irany == le) // ha az aktualis irannyal ellentetes iranyu parancs erkezik, nem tortenik semmi
                    j1->irany = le;
                else
                    j1->irany = fel;
                break;

            case SDLK_a:
                if(j1->irany == jobbra)
                    j1->irany = jobbra;
                else
                    j1->irany = balra;
                break;

            case SDLK_s:
                if(j1->irany == fel)
                    j1->irany = fel;
                else
                    j1->irany=le;
                break;

            case SDLK_d:
                if(j1->irany == balra)
                    j1->irany = balra;
                else
                    j1->irany = jobbra;
                break;
                if(ketjatekos)
                {
                case SDLK_UP:
                    if(j1->irany2 == le)
                        j1->irany2 = le;
                    else
                        j1->irany2 = fel;
                    break;

                case SDLK_LEFT:
                    if(j1->irany2 == jobbra)
                        j1->irany2 = jobbra;
                    else
                        j1->irany2 = balra;
                    break;

                case SDLK_DOWN:
                    if(j1->irany2 == fel)
                        j1->irany2 = fel;
                    else
                        j1->irany2 = le;
                    break;

                case SDLK_RIGHT:
                    if(j1->irany2 == balra)
                        j1->irany2 = balra;
                    else
                        j1->irany2 = jobbra;
                    break;
                }
            }
            SDL_RenderPresent(renderer);
            break;
        case SDL_QUIT:
            quit = true;
            break;
        }
    }
    if(ketjatekos)
        kigyo_felszabadit(j1->fej2);
    kigyo_felszabadit(j1->fej);
    SDL_RemoveTimer(id);
    SDL_Delay(500);
    return gyoztes;
}
