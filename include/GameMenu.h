#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include "UserEvents.h"
#include "Button.h"

class GameMenu {
public:
    GameMenu(UI& ui) : UIManager(ui) {}

    void AddButton(std::unique_ptr<Button> button) {
        Buttons.push_back(std::move(button));
    }

    void HandleEvent(SDL_Event& E) {
        if (E.type == UserEvents::OPEN_SETTINGS || E.type == UserEvents::CLOSE_SETTINGS) {
            HandleUserEvent(E.user);
        }

        if (isOpen) {
            for (auto& button: Buttons) {
                button->HandleEvent(E);
            }
        }
    }

    void Render(SDL_Surface* Surface) const {
        if (!isOpen) return;
        SDL_FillRect(
            Surface,
            &Rect,
            SDL_MapRGB(
                Surface->format,
                Color.r, Color.g, Color.b
            )
        );

        for (const auto& button: Buttons) {
            button->Render(Surface);
        }
    }

private:
    void HandleUserEvent(SDL_UserEvent& E) {
        using namespace UserEvents;
        if (E.type == OPEN_SETTINGS) {
            isOpen = true;
            auto* Instigator = static_cast<Button*>(E.data1);

            Rect.x = Instigator->GetConfig().x;
            Rect.y = Instigator->GetConfig().y;
        } else if (E.type == CLOSE_SETTINGS) {
            isOpen = false;
        }
    }

    bool isOpen{false};
    SDL_Rect Rect{100, 50, 255, 200};
    SDL_Color Color{150, 150, 150, 255};

    UI& UIManager;
    std::vector<std::unique_ptr<Button>> Buttons;
};
