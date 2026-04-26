// File: province_screen.h
// Commit: Add province screen entry point for the direct launch shell UI

#pragma once

#include "province.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void runProvinceScreen(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* sharedFont, Province& province);