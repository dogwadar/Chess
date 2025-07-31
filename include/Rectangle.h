#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdint>
#include <string>
#include <iostream>

class Rectangle {
public:
    Rectangle(const SDL_Rect &Rect) : Rect{Rect} {}

    void Render(SDL_Surface *Surface) const {
        auto [r, g, b, a]{isPointerHovering ? HoverColor : Color};

        SDL_FillRect(Surface, &Rect, SDL_MapRGB(Surface->format, r, g, b));

        if (!Text.empty() && Font) {
            SDL_Surface* TextSurface = TTF_RenderUTF8_Blended(Font, Text.c_str(), TextColor);
            if (TextSurface) {
              SDL_Rect TextRect;
              TextRect.x = Rect.x + (Rect.w - TextSurface->w) / 2;
              TextRect.y = Rect.y + (Rect.h - TextSurface->h) / 2;
              TextRect.w = TextSurface->w;
              TextRect.h = TextSurface->h;

              SDL_BlitSurface(TextSurface, nullptr, Surface, &TextRect);
              SDL_FreeSurface(TextSurface);
            }
        }
    }

    virtual void OnMouseEnter() {}
    virtual void OnMouseExit() {}
    virtual void OnLeftClick() {}

    void HandleEvent(SDL_Event &E) {
        if (E.type == SDL_MOUSEMOTION) {
            bool wasPointerHovering{isPointerHovering};
            isPointerHovering = isWithinRect(E.motion.x, E.motion.y);
            if (!wasPointerHovering && isPointerHovering) {
                OnMouseEnter();
            } else if (wasPointerHovering && !isPointerHovering) {
                OnMouseExit();
            }
        } else if (E.type == SDL_WINDOWEVENT && E.window.event == SDL_WINDOWEVENT_LEAVE) {
            if (isPointerHovering)
                OnMouseExit();
            isPointerHovering = false;
            } else if (E.type == SDL_MOUSEBUTTONDOWN) {
            if (isPointerHovering && E.button.button == SDL_BUTTON_LEFT) {
                OnLeftClick();
            }
        }
    }

    void SetText(const std::string &NewText) { Text = NewText; }

    void SetColor(const SDL_Color &NewColor) { Color = NewColor; }

    void SetTextColor(const SDL_Color &NewTextColor) { TextColor = NewTextColor; }

    void SetFontSize(int16_t &NewFontSize) {
        FontSize = NewFontSize;
        UpdateFont();
    }

    void SetFontPath(const std::string& path) {
        FontPath = path;
        UpdateFont();
      }

    SDL_Color GetColor() const { return Color; }

    void SetHoverColor(const SDL_Color &NewColor) { HoverColor = NewColor; }

    SDL_Color GetHoverColor() const { return HoverColor; }

protected:
    void UpdateFont() {
        if (Font) {
            TTF_CloseFont(Font);
        }

        Font = TTF_OpenFont(FontPath.c_str(), FontSize);
        if (!Font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << '\n';
        }
    }

    TTF_Font* GetFont() const {
        return Font;
    }

private:
    std::string Text;
    SDL_Color TextColor;
    int16_t FontSize = 32;
    std::string FontPath = "../font/terminal.ttf";
    TTF_Font* Font = nullptr;

    SDL_Rect Rect;
    SDL_Color Color{255, 0, 0};
    SDL_Color HoverColor{0, 0, 255};

    bool isPointerHovering{false};

    bool isWithinRect(int x, int y) {
        if (x < Rect.x)
            return false;
        if (x > Rect.x + Rect.w)
            return false;
        if (y < Rect.y)
            return false;
        if (y > Rect.y + Rect.h)
            return false;
        return true;
    }
};
