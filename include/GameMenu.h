#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "Button.h"
#include "GameState.h"
#include "StateChangeCallback.h"

class GameMenu {
public:
    GameMenu() = default;

    void AddButton(std::unique_ptr<Button> button) {
        Buttons.push_back(std::move(button));
    }

    void Render(SDL_Surface* surface) const {
        for (const auto& btn : Buttons) {
            btn->Render(surface);
        }
    }

    void HandleEvent(SDL_Event& e) {
        for (auto& btn : Buttons) {
            btn->HandleEvent(e);
        }
    }

    void SetStateChangeCallback(StateChangeCallback cb) {
        OnStateChange = std::move(cb);
        for (auto& btn : Buttons) {
            btn->SetCallback([this] {
                if (OnStateChange) {
                    OnStateChange(GameState::GAMEPLAY);
                }
            });
        }
    }

private:
    std::vector<std::unique_ptr<Button>> Buttons;
    StateChangeCallback OnStateChange;
};
