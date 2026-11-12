// File: window_state.h
// Commit: Add shared window loop state definitions

#pragma once

#include "building/province_screen_types.h"
#include "window_camera.h"
#include "window_render.h"

#include <string>

struct WindowLoopState
{
    MapShellState shell;
    ProvinceOverlayState provinceOverlay;
    CameraState camera;

    bool running = true;
    bool selectingRootBuilding = false;

    int selectingSubBuildingParentId = -1;

    int screenWidth = 0;
    int screenHeight = 0;

    int hoveredTileX = -1;
    int hoveredTileY = -1;

    int selectedTileX = -1;
    int selectedTileY = -1;
    int selectedProvinceId = -1;

    Uint32 lastMonthAdvanceMs = 0;
};