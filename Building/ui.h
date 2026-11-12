// File: ui.h
// Commit: Add sacred site root color to shared UI palette

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

struct UiColor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

namespace UiPalette
{
    inline constexpr UiColor background{239, 226, 198, 255};
    inline constexpr UiColor panelFill{239, 226, 198, 255};
    inline constexpr UiColor buttonFill{232, 220, 196, 255};
    inline constexpr UiColor border{40, 40, 40, 255};
    inline constexpr UiColor activeBorder{120, 80, 40, 255};
    inline constexpr UiColor text{0, 0, 0, 255};
    inline constexpr UiColor mutedText{55, 55, 55, 255};
    inline constexpr UiColor slotFill{235, 224, 202, 255};
    inline constexpr UiColor selectedFill{224, 212, 188, 255};
    inline constexpr UiColor drawerFill{234, 222, 198, 255};

    inline constexpr UiColor castleRoot{24, 48, 96, 255};
    inline constexpr UiColor estateRoot{40, 90, 170, 255};

    inline constexpr UiColor tribalRoot{110, 72, 32, 255};
    inline constexpr UiColor sacredSiteRoot{132, 96, 48, 255};

    inline constexpr UiColor cityRoot{72, 128, 72, 255};
    inline constexpr UiColor merchantEstateRoot{72, 128, 72, 255};

    inline constexpr UiColor churchRoot{236, 236, 236, 255};
    inline constexpr UiColor religiousEstateRoot{196, 196, 196, 255};

    inline constexpr UiColor smallholderRoot{190, 150, 102, 255};
}

TTF_Font* loadFontOrNull(int size);
void setRenderColor(SDL_Renderer* renderer, UiColor color);
void drawPanel(SDL_Renderer* renderer, const SDL_Rect& rect);
void drawPanelFilled(SDL_Renderer* renderer, const SDL_Rect& rect, UiColor fill, UiColor border);
void drawButton(SDL_Renderer* renderer, TTF_Font* font, const SDL_Rect& rect, const std::string& label, bool active);
void drawText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, UiColor color);
void drawWrappedText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, int wrapWidth, UiColor color);
bool pointInRect(int x, int y, const SDL_Rect& rect);