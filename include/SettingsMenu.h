#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include "UserEvents.h"
#include "Button.h"

class SettingsMenu {
 public:
  void HandleEvent(SDL_Event& E) {
    if (E.type == UserEvents::OPEN_SETTINGS ||
        E.type == UserEvents::CLOSE_SETTINGS) {
      HandleUserEvent(E.user);
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
      ));
  }

 private:
  void HandleUserEvent(SDL_UserEvent& E) {
    using namespace UserEvents;
    if (E.type == OPEN_SETTINGS) {
      isOpen = true;

      auto* Instigator{
        static_cast<Button*>(E.data1)
      };

     std::cout << "I was opened from a button in "
        << Instigator->GetLocation() << "\n";

      Rect.x = Instigator->GetConfig().x;
      Rect.y = Instigator->GetConfig().y;
      if (
        Instigator->GetConfig().Page ==
          SettingsPage::GAMEPLAY
      ) {
        std::cout << "Page: Gameplay Settings\n";
      }
    } else if (E.type == CLOSE_SETTINGS) {
      isOpen = false;
    }
  }

  bool isOpen{false};
  SDL_Rect Rect{100, 50, 200, 200};
  SDL_Color Color{150, 150, 150, 255};
};
