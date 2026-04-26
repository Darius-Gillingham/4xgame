// File: ui.cpp
// Commit: Implement shared styling and drawing helpers using the current C++ palette and window style

#include "ui.h"
#include <array>

TTF_Font* loadFontOrNull(int size)
{
    const std::array<const char*, 5> paths = {
        "C:/Windows/Fonts/georgia.ttf",
        "C:/Windows/Fonts/times.ttf",
        "C:/Windows/Fonts/timesbd.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };

    for (const char* path : paths)
    {
        if (TTF_Font* font = TTF_OpenFont(path, size))
        {
            return font;
        }
    }

    return nullptr;
}

void setRenderColor(SDL_Renderer* renderer, UiColor color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void drawPanel(SDL_Renderer* renderer, const SDL_Rect& rect)
{
    drawPanelFilled(renderer, rect, UiPalette::panelFill, UiPalette::border);
}

void drawPanelFilled(SDL_Renderer* renderer, const SDL_Rect& rect, UiColor fill, UiColor border)
{
    setRenderColor(renderer, fill);
    SDL_RenderFillRect(renderer, &rect);

    setRenderColor(renderer, border);
    SDL_RenderDrawRect(renderer, &rect);
}

void drawButton(SDL_Renderer* renderer, TTF_Font* font, const SDL_Rect& rect, const std::string& label, bool active)
{
    drawPanelFilled(renderer, rect, UiPalette::buttonFill, active ? UiPalette::activeBorder : UiPalette::border);

    if (!font)
    {
        return;
    }

    int textWidth = 0;
    int textHeight = 0;
    if (TTF_SizeUTF8(font, label.c_str(), &textWidth, &textHeight) != 0)
    {
        textWidth = 0;
        textHeight = 0;
    }

    drawText(
        renderer,
        font,
        label,
        rect.x + (rect.w - textWidth) / 2,
        rect.y + (rect.h - textHeight) / 2,
        UiPalette::text
    );
}

void drawText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, UiColor color)
{
    if (!renderer || !font)
    {
        return;
    }

    SDL_Color sdlColor{color.r, color.g, color.b, color.a};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), sdlColor);
    if (!surface)
    {
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dst{x, y, surface->w, surface->h};
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
}

void drawWrappedText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, int wrapWidth, UiColor color)
{
    if (!renderer || !font)
    {
        return;
    }

    SDL_Color sdlColor{color.r, color.g, color.b, color.a};
    SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), sdlColor, wrapWidth);
    if (!surface)
    {
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dst{x, y, surface->w, surface->h};
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
}

bool pointInRect(int x, int y, const SDL_Rect& rect)
{
    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
}