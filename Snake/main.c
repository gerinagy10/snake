#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "debugmalloc.h"
#include "megjelenites.h"
#include "jatek.h"
#include "kigyo.h"


int main(int argc, char *argv[]) {

    srand(time(0));

    SDL_Window *window;
    SDL_Renderer *renderer;

    // Ablak letrehozasa.
    sdl_init(720, 480, &window, &renderer);

    // Hatter
    boxRGBA(renderer, 0, 0, 720, 480, 204, 204, 0, 200);

    // Betutipus betoltese 84 pont magassaggal a nagy Snake felirathoz.
    TTF_Init();
    TTF_Font *font = TTF_OpenFont(FONT, 84);
    if (!font) {
        SDL_Log("Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(1);
    }

    SDL_Surface *felirat;
    SDL_Texture *felirat_t;

    // Snake felirat hosszanak megallapitasa, segitsegevel kozepre kiiras.
    int hossz_s = felirat_hossz("Snake", felirat, font);
    sdl_kiir("Snake", felirat, felirat_t, font, renderer, 360 - hossz_s/2, 80);

    // Betutipus betoltese 32 pont magassaggal a menupontok felirataihoz.
    font = TTF_OpenFont(FONT, 32);
    if (!font) {
        SDL_Log("Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(1);
    }

    /* Menu gombok feliratanak kiirasa hosszuk segitsegevel,
    magassaguk majd a kattintas helyenek erzekelesehez kell. */

    // Egyjatekos mod
    int hossz_1 = felirat_hossz("Egyjátékos mód", felirat, font);
    sdl_kiir("Egyjátékos mód", felirat, felirat_t, font, renderer, 360-hossz_1-50, 320);

    // Ketjatekos mod
    int hossz_2 = felirat_hossz("Kétjátkos mód", felirat, font);
    sdl_kiir("Kétjátkos mód", felirat, felirat_t, font, renderer, 360 + 50, 320);

    // Ranglista
    int hossz_r= felirat_hossz("Ranglista", felirat, font);
    sdl_kiir("Ranglista", felirat, felirat_t, font, renderer, 360 - hossz_r/2, 380);

    SDL_RenderPresent(renderer);

    // Menu

    SDL_Event klikk;

    bool valasztas = false;

    while(!valasztas){
        SDL_WaitEvent(&klikk);

        switch (klikk.type){
            case SDL_MOUSEBUTTONDOWN:
                // Egyjatekos modra kattintas
                if((klikk.button.x >= 360-hossz_1-50 && klikk.button.x <= 360 - 50) && (klikk.button.y >= 320 && klikk.button.y <= 320 + 32)){
                    valasztas = true;
                    egyjatekos_mod(felirat, felirat_t, font, renderer, window);
                }
                // Ketjatekos modra kattintas
                else if((klikk.button.x >= 360 + 50 && klikk.button.x <= 360 + 50 + hossz_2) && (klikk.button.y >= 320 && klikk.button.y <= 320 + 32)){
                    valasztas = true;
                    ketjatekos_mod(felirat, felirat_t, font, renderer, window);
                }

                // Ranglistara kattintas
                else if((klikk.button.x >= 360 - hossz_r/2 && klikk.button.x <= 360 - hossz_r/2 + hossz_r) && (klikk.button.y >= 380 && klikk.button.y <= 380 + 32)){
                    valasztas = true;
                    ranglista_megjelenit(felirat, felirat_t, font, renderer, window);
                }

                break;

            // Ablak bezarasa
            case SDL_QUIT:
                valasztas = true;
                SDL_Quit();
                break;
        }
    }

    // Betutipus bezarasa.
    TTF_CloseFont(font);

    SDL_Event event;
    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT) {
    }

    SDL_Quit();

    return 0;
}
