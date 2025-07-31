#pragma once

#include <SDL2/SDL.h>
#include <iostream>

class Board {
public:
    void StartNewGame() {
        std::cout << "Starting a new game" << std::endl;
    }

    void HandleEvent(SDL_Event& e) {
    }

    void Render(SDL_Surface* surface) {
        SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 30, 30, 30));
    }
};
