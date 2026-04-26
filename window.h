// File: window.h
// Commit: Remove bad map_state include and use map_internal for MapState in shared window interface

#pragma once

#include "map.h"
#include "map_internal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct CameraState
{
    float zoom = 1.0f;
    float minZoom = 0.5f;
    float maxZoom = 6.0f;

    float offsetX = 0.0f;
    float offsetY = 0.0f;

    bool dragging = false;
    int lastMouseX = 0;
    int lastMouseY = 0;
};

void runWindow(
    SDL_Window* window,
    SDL_Renderer* renderer,
    TTF_Font* sharedFont,
    const Map& map,
    const MapState& state
);