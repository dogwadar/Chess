#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <memory>
#include "Button.h"
#include "GameMenu.h"
#include "GameState.h"
#include "StateChangeCallback.h"

class MainMenu {
public:
    MainMenu() {
        HeadingFont = TTF_OpenFont("../font/abduction.ttf", 128);
        if (!HeadingFont) {
            std::cerr << "Could not load heading font: " << TTF_GetError() << '\n';
        }

        int baseX = NewGameButton.GetRect().x;
        int baseY = NewGameButton.GetRect().y + NewGameButton.GetRect().h;
        int width = NewGameButton.GetRect().w;
        int height = 40;

        auto randomOpeningButton = std::make_unique<Button>(
            *this,
            SDL_Rect{baseX, baseY, width, height},
            SDL_Color{255, 165, 0, 255},
            "Random Opening",
            SDL_Color{0, 0, 0, 255},
            24
        );

        randomOpeningButton->SetCallback([this] {
            std::cout << "Playing game with random engine opening" << std::endl;
            if (OnStateChange) {
                OnStateChange(GameState::GAMEPLAY);
            }
        });

        auto chooseOpeningButton = std::make_unique<Button>(
            *this,
            SDL_Rect{baseX, baseY + height, width, height},
            SDL_Color{200, 200, 200, 255},
            "Choose Opening",
            SDL_Color{0, 0, 0, 255},
            24
        );

        chooseOpeningButton->SetCallback([this] {
            std::cout << "Player chooses preferred engine opening" << std::endl;
            if (OnStateChange) {
                OnStateChange(GameState::GAMEPLAY);
            }
        });

        Menu.AddButton(std::move(randomOpeningButton));
        Menu.AddButton(std::move(chooseOpeningButton));
    }

    ~MainMenu() {
        if (HeadingFont) {
            TTF_CloseFont(HeadingFont);
        }
    }

    void SetStateChangeCallback(StateChangeCallback cb) {
        OnStateChange = std::move(cb);
        Menu.SetStateChangeCallback(OnStateChange);
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
        Menu.Render(Surface);
    }

    void HandleEvent(SDL_Event& E) {
        NewGameButton.HandleEvent(E);
        Menu.HandleEvent(E);
    }

private:
    TTF_Font* HeadingFont = nullptr;

    Button NewGameButton{
        *this,
        {1920 / 2 - 255 / 2, 1080 / 2, 255, 50},
        {255, 255, 255, 255},
        "Start Game",
        {0, 0, 0, 255},
        32
    };

    GameMenu Menu;
    StateChangeCallback OnStateChange;
};
