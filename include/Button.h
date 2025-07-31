#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <functional>
#include <iostream>

class MainMenu;

class Button {
public:
    Button(
        MainMenu& UIManager,
        const SDL_Rect& rect,
        SDL_Color color,
        const std::string& text,
        SDL_Color textColor,
        int16_t fontSize,
        TTF_Font* font
    )
    : UIManager(UIManager), Rect(rect), Color(color), Text(text),
      TextColor(textColor), FontSize(fontSize), Font(font)
    {
        if (!Font) {
            std::cerr << "Warning: Button created with null font pointer.\n";
        }
    }

    bool HandleEvent(SDL_Event& e) {
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int x = e.button.x;
            int y = e.button.y;
            if (x >= Rect.x && x <= Rect.x + Rect.w &&
                y >= Rect.y && y <= Rect.y + Rect.h) {
                OnLeftClick();
                return true;
            }
        }
        return false;
    }

    void Render(SDL_Surface* surface) const {
        SDL_FillRect(surface, &Rect, SDL_MapRGB(surface->format, Color.r, Color.g, Color.b));

        if (!Font) return;

        SDL_Surface* textSurface = TTF_RenderUTF8_Blended(Font, Text.c_str(), TextColor);
        if (!textSurface) {
            std::cerr << "Failed to render button text: " << TTF_GetError() << std::endl;
            return;
        }

        SDL_Rect textRect;
        textRect.w = textSurface->w;
        textRect.h = textSurface->h;
        textRect.x = Rect.x + (Rect.w - textRect.w) / 2;
        textRect.y = Rect.y + (Rect.h - textRect.h) / 2;

        SDL_BlitSurface(textSurface, nullptr, surface, &textRect);
        SDL_FreeSurface(textSurface);
    }

    void SetCallback(std::function<void()> callback) {
        OnClickCallback = std::move(callback);
    }

    void OnLeftClick() {
        if (OnClickCallback) {
            OnClickCallback();
        }
    }

    void SetColor(SDL_Color color) {
        Color = color;
    }

    SDL_Rect GetRect() const {
        return Rect;
    }

private:
    MainMenu& UIManager;

    SDL_Rect Rect;
    SDL_Color Color;
    std::string Text;
    SDL_Color TextColor;
    int16_t FontSize;
    TTF_Font* Font;

    std::function<void()> OnClickCallback;
};
