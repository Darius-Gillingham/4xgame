// File: window_camera.h
// Commit: Extract map camera state and camera helpers from window module

#pragma once

#include "map.h"

#include <SDL2/SDL.h>

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

void clampCamera(
    CameraState& camera,
    int screenWidth,
    int screenHeight,
    int panelHeight,
    const Map& map
);

void applyZoom(
    CameraState& camera,
    float zoomFactor,
    int mouseX,
    int mouseY,
    int panelHeight,
    int screenWidth,
    int screenHeight,
    const Map& map
);

bool screenToTile(
    int mouseX,
    int mouseY,
    int panelHeight,
    const CameraState& camera,
    const Map& map,
    int& tileX,
    int& tileY
);