#pragma once
#include "Rectangle.h"
#include "UserEvents.h"
#include <SDL2/SDL.h>
#include <cstdint>
#include <string>
#include <functional>

class UI;

class Button : public Rectangle {
public:
    Button(
        UI &UIManager,
        const SDL_Rect &Rect,
        SDL_Color color,
        std::string text,
        SDL_Color text_color,
        int16_t font_size
    ): Rectangle{Rect}, UIManager{UIManager} {
        SetColor(color);
        SetText(text);
        SetTextColor(text_color);
        SetFontSize(font_size);
    }

    void HandleEvent(SDL_Event &E) {
        Rectangle::HandleEvent(E);
        using namespace UserEvents;
        if (E.type == CLOSE_SETTINGS) {
            isSettingsOpen = false;
        } else if (E.type == OPEN_SETTINGS) {
            isSettingsOpen = true;
        }
    }

    void SetCallback(std::function<void()> Callback) {
        OnClickCallback = std::move(Callback);
    }

    void OnLeftClick() override {
        using namespace UserEvents;
        SDL_Event Event{isSettingsOpen ? CLOSE_SETTINGS : OPEN_SETTINGS};

        if (Event.type == OPEN_SETTINGS) {
            Event.user.data1 = this;
        }

        SDL_PushEvent(&Event);

        if (OnClickCallback) {
                OnClickCallback();
        }
    }

    UserEvents::SettingsConfig GetConfig() { return Config; }

    std::string GetLocation() { return "Game Menu"; }

private:
    UserEvents::SettingsConfig Config{UserEvents::SettingsPage::GAMEPLAY, 1920/2-255/2, 1080/2+50};

    UI &UIManager;
    bool isSettingsOpen{false};
    std::function<void()> OnClickCallback;
};
