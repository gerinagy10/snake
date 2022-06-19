#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "jatek.h"
#include "debugmalloc.h"
#include "kigyo.h"
#include "megjelenites.h"

SDL_Color fekete = {0, 0, 0};

/* Torli a palyat, parameterkent egy Jatek pointert kap,
aminek a 2D-s palya tombjeben minden mezot uresre allit.
Nem ter vissza semmivel.*/
void palya_torol(Jatek* j1)
{
    for(int i=0; i<24; ++i)
    {
        for(int o=0; o<36; ++o)
        {
            j1->palya[i][o] = m_ures;
        }
    }
}
/* A palyat alaphelyzetbe allitja, eloszor letorli, az elozo fuggveny segitsegevel
majd a 2D-s tombben a falakat, illetve a kigyo kezdopoziciojat beallitja
(ketjatekos eseten mindekettot). Parametere a Jatek pointer,
illetve, hogy ketjatekos van-e. Nem ter vissza semmivel.*/
void palya_betolt(Jatek* j1, bool ketjatekos)
{
    palya_torol(j1);

    for(int i=0; i<24; ++i)
        j1->palya[i][0] = m_fal; //bal oldali fal

    for(int i=0; i<36; ++i)
        j1->palya[23][i] = m_fal; //also fal

    for(int i=0; i<24; ++i)
        j1->palya[i][35] = m_fal; //jobb oldali fal

    for(int i=0; i<36; ++i)
        j1->palya[0][i] = m_fal; //felso fal

    j1->palya[12][18] = m_kigyofej;

    if(ketjatekos)
        j1->palya[12][17] = m_kigyofej;
}

/* Egy darab mezo kirajzolasara kepes a kepernyore, parameterkent kapott
x es y koordinatatol fuggoen ellenorzi, hogy a 2D-s tombben milyen a mezo allapota,
es aszerint szinezi. Parameterkent tovabba a renderer pointeret es a Jatek pointert kapja.
Nem ter vissza semmivel. */
void palyamezo_rajz(int y, int x, SDL_Renderer *renderer, Jatek* j1)
{

    // Fal -> fekete
    if(j1->palya[y][x] == m_fal)
        boxRGBA(renderer, x*20, y*20, (x+1)*20, (y+1)*20, 0, 0, 0, 255);
    // Ures -> olive
    else if(j1->palya[y][x] == m_ures)
        boxRGBA(renderer, x*20, y*20, (x+1)*20, (y+1)*20, 204, 204, 0, 255);
    // Kigyo -> sotet zold
    else if(j1->palya[y][x] == m_kigyo)
        boxRGBA(renderer, x*20, y*20, (x+1)*20, (y+1)*20, 150, 150, 20, 255);
    // Etek -> piros
    else if(j1->palya[y][x] == m_etek)
        boxRGBA(renderer, x*20, y*20, (x+1)*20, (y+1)*20, 255, 0, 0, 255);

    else if(j1->palya[y][x] == m_kigyofej)
        boxRGBA(renderer, x*20, y*20, (x+1)*20, (y+1)*20, 102, 102, 0, 255);
}

/* A fenti 3 fuggvenyt egyesiti egyben, kirajzolja az egesz palyat, illetve a kezdeti kigyot.
Parametere a Jatek pointer, ketjatekos bool, es a renderer pointere. Nem ter vissza semmivel. */
void palya_kirajzol(Jatek* j1, SDL_Renderer *renderer, bool ketjatekos)
{
    palya_betolt(j1, ketjatekos);
    for(int i=0; i<24; ++i)
    {
        for(int o=0; o<36; ++o)
            palyamezo_rajz(i, o, renderer, j1);
    }
    SDL_RenderPresent(renderer);
}
/* Egy random pozicioba kirajzolja az etket, es a helyenek megfelelo poziciot
atallitja a 2D-s tombben. Az etek csak ures mezon jelenhet meg, addig general random helyeket,
amig nem talal ilyet. */
void alma_terem(Jatek* j1, SDL_Renderer *renderer, Pozicio *alma)
{
    bool szabad = false;
    int y;
    int x;
    while(!szabad)
    {
        y = rand()%24;
        x = rand()%36;
        if(j1->palya[y][x] == m_ures)
        {
            szabad = true;
            j1->palya[y][x] = m_etek;
            palyamezo_rajz(y, x, renderer, j1);
            alma->x = x;
            alma->y = y;
        }
    }
}
/* Az egyjatekos modhoz kapcsolodo fuggvenyek es ertekadasok egy helyen.
Parametere a felirat pointere, felirat textura pointere, betutipus pointere, renderer pointere
es az ablak pointere. Nem ter vissza semmivel.*/
void egyjatekos_mod(SDL_Surface *felirat, SDL_Texture *felirat_t, TTF_Font *font, SDL_Renderer *renderer, SDL_Window *window)
{
    bool ketjatekos = false;
    Jatek j1;
    j1.pontszam = 0;
    j1.fej = (Kigyo*) malloc(sizeof(Kigyo));
    if(j1.fej == NULL)
    {
        printf("A memoriafoglalas nem sikerult!\n");
        return;
    }
    j1.fej->kov = NULL;
    j1.fej->x = 18;
    j1.fej->y = 12;
    j1.fej2 = NULL;
    j1.irany = jobbra;
    beolvas(window, renderer, font, &j1, felirat, felirat_t);
    palya_betolt(&j1, ketjatekos);
    palya_kirajzol(&j1, renderer, ketjatekos);
    iranyit(renderer, &j1, ketjatekos);
    egyjatekos_vege(felirat, felirat_t, font, renderer, &j1, window);
}

/* A ketjatekos modhoz kapcsolodo fuggvenyek es ertekadasok egy helyen.
Parametere a felirat pointere, felirat textura pointere, betutipus pointere, renderer pointere
es az ablak pointere. Nem ter vissza semmivel.*/
void ketjatekos_mod(SDL_Surface *felirat, SDL_Texture *felirat_t, TTF_Font *font, SDL_Renderer *renderer, SDL_Window *window)
{
    bool ketjatekos = true;
    Jatek j2;
    j2.fej = (Kigyo*) malloc(sizeof(Kigyo));
    j2.fej->kov = NULL;
    j2.fej->x = 18;
    j2.fej->y = 12;
    j2.fej2 = (Kigyo*) malloc(sizeof(Kigyo));
    j2.fej2->kov = NULL;
    j2.fej2->x = 17;
    j2.fej2->y = 12;
    j2.irany = jobbra;
    j2.irany2 = balra;
    palya_betolt(&j2, ketjatekos);
    palya_kirajzol(&j2, renderer, ketjatekos);
    Gyoztes gyoztes = iranyit(renderer, &j2, ketjatekos);
    ketjatekos_vege(felirat, felirat_t, font, renderer, &j2, window, gyoztes);
}

/* Elmenti a megadott nevet es az elert pontszamot, ha az a top 5-ben van.
Parametere a Jatek pointere, nem ter vissza semmivel*/
void mentes(Jatek* j1)
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

    int hanyadik = top5_e(ranglista, j1);

    if(hanyadik == -1)
        return;

    else
    {

        fp = fopen("ranglista.txt", "w");

        if (fp == NULL)
        {
            perror("Fájl megnyitása sikertelen");
            return;
        }

        if(hanyadik == 4)
        {
            strcpy(ranglista[4].nev, j1->nev);
            ranglista[4].pontszam = j1->pontszam;
        }

        else
        {
            for(int i = 4; i != hanyadik; --i)
            {
                strcpy(ranglista[i].nev, ranglista[i-1].nev);
                ranglista[i].pontszam = ranglista[i-1].pontszam;
            }
            strcpy(ranglista[hanyadik].nev, j1->nev);
            ranglista[hanyadik].pontszam = j1->pontszam;
        }

        for(int i = 0; i < 5; ++i)
            fprintf(fp, "%s %d\n", ranglista[i].nev, ranglista[i].pontszam);

        fclose(fp);
    }
}

/*Megvizsgalja, hogy az elert eredmeny top 5-ben van-e, ha igen,
visszater a helyezes indexevel, kulonben -1-gyel. Parametere a
ranglista pointere es a Jatek pointere.  */
int top5_e(Eredmenyek* ranglista, Jatek* j1)
{
    for(int i = 0; i < 5; ++i)
    {
        if(j1->pontszam > ranglista[i].pontszam)
            return i;
    }
    return -1;
}
