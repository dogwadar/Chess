#pragma once
#include <iostream>
#include <SDL2/SDL.h>

class Window {
public:
  Window() {
    SDLWindow = SDL_CreateWindow(
      "Chess",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      1920, 1080, 0
    );
  }

  void Render() {
    SDL_FillRect(
      GetSurface(),
      nullptr,
      SDL_MapRGB(
        GetSurface()->format, 0, 0, 0
      )
    );
  }

  void Update() {
    SDL_UpdateWindowSurface(SDLWindow);
  }

  SDL_Surface* GetSurface() const {
    return SDL_GetWindowSurface(SDLWindow);
  }

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  ~Window() {
    if (SDLWindow && SDL_WasInit(SDL_INIT_VIDEO)) {
      SDL_DestroyWindow(SDLWindow);
    }
  }

private:
  SDL_Window* SDLWindow{nullptr};
};
