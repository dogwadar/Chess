#pragma once

#include <SDL2/SDL.h>
#include "MainMenu.h"
#include "Board.h"
#include "GameState.h"
#include "StateChangeCallback.h"

class GameStateController {
public:
    GameStateController()
        : CurrentState(GameState::MAIN_MENU)
        , Menu()
    {
        Menu.SetStateChangeCallback([this](GameState newState) {
            SetState(newState);
        });
    }

    void HandleEvent(SDL_Event& E, SDL_Surface* surface) {
        switch (CurrentState) {
            case GameState::MAIN_MENU:
                Menu.HandleEvent(E);
                break;
            case GameState::GAMEPLAY:
                Game.HandleEvent(E, surface);
                break;
            default:
                break;
        }
    }

    void Render(SDL_Surface* surface) {
        switch (CurrentState) {
            case GameState::MAIN_MENU:
                Menu.Render(surface);
                break;
            case GameState::GAMEPLAY:
                Game.Render(surface);
                break;
            default:
                break;
        }
    }

    void SetState(GameState newState) {
        if (CurrentState == newState) return;

        if (newState == GameState::GAMEPLAY) {
            Game.StartNewGame();
        }

        CurrentState = newState;
    }

    GameState GetState() const {
        return CurrentState;
    }

    MainMenu& GetUI() { return Menu; }

private:
    GameState CurrentState;
    MainMenu Menu;
    Board Game;
};
