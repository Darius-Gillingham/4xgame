// File: window_camera.cpp
// Commit: Remove horizontal camera clamping so map scrolling can wrap continuously

#include "window_camera.h"

#include <algorithm>
#include <cmath>

namespace
{
    int wrapTileX(int x, int width)
    {
        if (width <= 0)
        {
            return 0;
        }

        int wrapped = x % width;

        if (wrapped < 0)
        {
            wrapped += width;
        }

        return wrapped;
    }
}

void clampCamera(
    CameraState& camera,
    int screenWidth,
    int screenHeight,
    int panelHeight,
    const Map& map)
{
    const float mapHeightPixels = static_cast<float>(map.height()) * camera.zoom;
    const float viewportHeight = static_cast<float>(screenHeight - panelHeight);

    float minOffsetY = viewportHeight - mapHeightPixels;
    float maxOffsetY = 0.0f;

    if (mapHeightPixels < viewportHeight)
    {
        minOffsetY = maxOffsetY = (viewportHeight - mapHeightPixels) * 0.5f;
    }

    camera.offsetY = std::clamp(camera.offsetY, minOffsetY, maxOffsetY);

    (void)screenWidth;
}

void applyZoom(
    CameraState& camera,
    float zoomFactor,
    int mouseX,
    int mouseY,
    int panelHeight,
    int screenWidth,
    int screenHeight,
    const Map& map)
{
    const float oldZoom = camera.zoom;
    camera.zoom = std::clamp(camera.zoom * zoomFactor, camera.minZoom, camera.maxZoom);

    if (std::abs(camera.zoom - oldZoom) < 0.0001f)
    {
        return;
    }

    const float worldX = (static_cast<float>(mouseX) - camera.offsetX) / oldZoom;
    const float worldY = (static_cast<float>(mouseY - panelHeight) - camera.offsetY) / oldZoom;

    camera.offsetX = static_cast<float>(mouseX) - (worldX * camera.zoom);
    camera.offsetY = static_cast<float>(mouseY - panelHeight) - (worldY * camera.zoom);

    clampCamera(camera, screenWidth, screenHeight, panelHeight, map);
}

bool screenToTile(
    int mouseX,
    int mouseY,
    int panelHeight,
    const CameraState& camera,
    const Map& map,
    int& tileX,
    int& tileY)
{
    if (mouseY < panelHeight || map.width() <= 0)
    {
        return false;
    }

    const float worldX = (static_cast<float>(mouseX) - camera.offsetX) / camera.zoom;
    const float worldY = (static_cast<float>(mouseY - panelHeight) - camera.offsetY) / camera.zoom;

    tileX = wrapTileX(static_cast<int>(std::floor(worldX)), map.width());
    tileY = static_cast<int>(std::floor(worldY));

    return tileY >= 0 && tileY < map.height();
}