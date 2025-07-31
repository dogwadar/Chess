#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <cstdio>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../include/Window.h"
#include "../include/GameStateController.h"


int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << '\n';
        return 1;
    }

    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init error: " << TTF_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    Window GameWindow;
    GameStateController Controller;

    SDL_Event E;
    while (true) {
        while (SDL_PollEvent(&E)) {
            if (E.type == SDL_QUIT) {
                IMG_Quit();
                TTF_Quit();
                SDL_Quit();
                return 0;
            }
            Controller.HandleEvent(E, GameWindow.GetSurface());
        }

        GameWindow.Render();
        Controller.Render(GameWindow.GetSurface());
        GameWindow.Update();
    }

    return 0;
}

int initalize_stockfish() {
    FILE *pipe = popen("./bin/stockfish", "w");

    if (!pipe) {
        std:std::cerr << "Failed to open pipe" << std::endl;
        return 1;
    }

    char buffer[128];
    while(fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::cout << buffer;
    }

    // STOCKFISH SETTINGS
    fprintf(pipe, "setoption name Threads value 6\n"); // set number of threads
    fprintf(pipe, "setoption name Hash value 1024\n"); // set hash table max size in MB
    fprintf(pipe, "ucinewgame\n"); // clear all caches
    fflush(pipe);

    pclose(pipe);

    return 0;
}
