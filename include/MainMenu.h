#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <functional>
#include "Button.h"
#include "GameState.h"
#include "StateChangeCallback.h"

class MainMenu {
public:
    MainMenu()
    : HeadingFont(TTF_OpenFont("../font/abduction.ttf", 128)),
      ButtonFont(TTF_OpenFont("../font/terminal.ttf", 32)),
      NewGameButton(
        *this,
        SDL_Rect{1920/2-255/2, 1080/2, 255, 55},
        SDL_Color{255, 255, 255, 255},
        "Start Game",
        SDL_Color{0, 0, 0, 255},
        32,
        ButtonFont
      ),
      randomOpeningButton(
        *this,
        SDL_Rect{1920/2-255/2, 1080/2+55, 255, 40},
        selectedColor,
        "Random Opening",
        textColor,
        32,
        ButtonFont
      ),
      chooseOpeningButton(
        *this,
        SDL_Rect{1920/2-255/2, 1080/2+95, 255, 40},
        unselectedColor,
        "Choose Opening",
        textColor,
        32,
        ButtonFont
      )
    {
        if (!HeadingFont) {
            std::cerr << "Could not load heading font: " << TTF_GetError() << '\n';
        }
        if (!ButtonFont) {
            std::cerr << "Could not load button font: " << TTF_GetError() << '\n';
        }

        NewGameButton.SetCallback([this] {
            std::cout << "Start Game clicked. ";
            if (isRandomOpeningSelected) {
                std::cout << "Starting with random opening." << std::endl;
            } else {
                std::cout << "Starting with chosen opening." << std::endl;
            }
            if (OnStateChange) {
                OnStateChange(GameState::GAMEPLAY);
            }
        });

        randomOpeningButton.SetCallback([this] {
            SelectOpening(true);
            std::cout << "Random Opening selected" << std::endl;
        });

        chooseOpeningButton.SetCallback([this] {
            SelectOpening(false);
            std::cout << "Choose Opening selected" << std::endl;
        });

        isRandomOpeningSelected = true;
    }

    ~MainMenu() {
        if (HeadingFont) {
            TTF_CloseFont(HeadingFont);
        }
        if (ButtonFont) {
            TTF_CloseFont(ButtonFont);
        }
    }

    void SetStateChangeCallback(StateChangeCallback cb) {
        OnStateChange = std::move(cb);
    }

    void Render(SDL_Surface* Surface) const {
        if (HeadingFont) {
            SDL_Color color = {255, 255, 255, 255};
            SDL_Surface* textSurface = TTF_RenderUTF8_Blended(HeadingFont, "Dogwater's Chess", color);
            if (textSurface) {
                SDL_Rect destRect;
                destRect.w = textSurface->w;
                destRect.h = textSurface->h;
                destRect.x = (Surface->w - destRect.w) / 2;
                destRect.y = Surface->h / 8 - destRect.h / 2;

                SDL_BlitSurface(textSurface, nullptr, Surface, &destRect);
                SDL_FreeSurface(textSurface);
            }
        }

        NewGameButton.Render(Surface);
        randomOpeningButton.Render(Surface);
        chooseOpeningButton.Render(Surface);
    }

    void HandleEvent(SDL_Event& E) {
        if (NewGameButton.HandleEvent(E)) return;
        if (randomOpeningButton.HandleEvent(E)) return;
        if (chooseOpeningButton.HandleEvent(E)) return;
    }

private:
    void SelectOpening(bool selectRandom) {
        isRandomOpeningSelected = selectRandom;
        if (selectRandom) {
            randomOpeningButton.SetColor(selectedColor);
            chooseOpeningButton.SetColor(unselectedColor);
        } else {
            randomOpeningButton.SetColor(unselectedColor);
            chooseOpeningButton.SetColor(selectedColor);
        }
    }

private:
    TTF_Font* HeadingFont = nullptr;
    TTF_Font* ButtonFont = nullptr;

    const SDL_Color selectedColor = {255, 165, 0, 255}; // orange
    const SDL_Color unselectedColor = {200, 200, 200, 255}; // gray
    const SDL_Color textColor = {0, 0, 0, 255};

    Button NewGameButton;
    Button randomOpeningButton;
    Button chooseOpeningButton;

    StateChangeCallback OnStateChange;

    bool isRandomOpeningSelected = true;
};
