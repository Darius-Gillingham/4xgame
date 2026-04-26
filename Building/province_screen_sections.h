// File: province_screen_sections.h
// Commit: Remove split normal-buildings section helpers and keep only shared non-building panel render helpers

#pragma once

#include "province.h"
#include "province_screen_types.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

void drawSectionTitle(SDL_Renderer* renderer, TTF_Font* sectionFont, const SDL_Rect& rect, const std::string& title);

void drawPopCountSection(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SectionLayout& popSection,
    const Province& province
);

void drawResourcesSection(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SectionLayout& resourcesSection
);

void drawInfrastructureSection(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SectionLayout& infraSection
);

void drawMetricsSection(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SectionLayout& metricsSection
);