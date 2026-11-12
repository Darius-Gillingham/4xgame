// File: building/province_screen_layout.h
// Commit: Retain only live holding-tree layout declarations

#pragma once

#include <SDL2/SDL_ttf.h>

extern const int kSectionPad;
extern const int kSlotHeight;

int lineHeightForFont(
    TTF_Font* font,
    int fallback
);