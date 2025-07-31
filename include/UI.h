#pragma once
#include <SDL2/SDL.h>
#include "Button.h"
#include "SettingsMenu.h"

class UI {
public:
    void Render(SDL_Surface* Surface) const {
        SettingsButton.Render(Surface);
        Settings.Render(Surface);
    }

  void HandleEvent(SDL_Event& E) {
    SettingsButton.HandleEvent(E);
    Settings.HandleEvent(E);
  }

private:
  Button SettingsButton{*this, {50, 50, 255, 50}, {255, 255, 255, 255}, "Settings", {0, 0, 0, 255}, 32}; // pos & size (x,y,w,h), bg color (rgb), text, text color (rgb), font size
  SettingsMenu Settings;
};
