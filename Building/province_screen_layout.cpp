// File: building/province_screen_layout.cpp
// Commit: Remove obsolete fullscreen province layout measurements

#include "province_screen_layout.h"

const int kSectionPad = 14;
const int kSlotHeight = 30;

int lineHeightForFont(
    TTF_Font* font,
    int fallback
)
{
    if (!font)
    {
        return fallback;
    }

    const int value =
        TTF_FontHeight(font);

    return
        value > 0
            ? value
            : fallback;
}