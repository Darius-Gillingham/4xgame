// File: province_screen_layout.h
// Commit: Keep province screen layout interfaces aligned with the unified Buildings panel and generic building tree sizing

#pragma once

#include "province.h"
#include <SDL2/SDL_ttf.h>
#include <string>

extern const int kOuterPadding;
extern const int kColumnGap;
extern const int kRowGap;
extern const int kHeaderHeight;
extern const int kDrawerWidth;
extern const int kSectionPad;
extern const int kSlotHeight;

int lineHeightForFont(TTF_Font* font, int fallback);
int measureWrappedHeight(TTF_Font* font, const std::string& text, int wrapWidth, int fallback);

int capitalSectionContentHeight(const Province& province, TTF_Font* bodyFont);
int normalBuildingsSectionContentHeight(TTF_Font* bodyFont);
int infrastructureSectionContentHeight(TTF_Font* bodyFont);
int popCountSectionContentHeight(TTF_Font* bodyFont);
int resourcesSectionContentHeight(TTF_Font* bodyFont);
int metricsSectionContentHeight(TTF_Font* bodyFont);