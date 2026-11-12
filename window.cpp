// File: window.cpp
// Commit: Add F12 renderer screenshots for province UI development

#include "window.h"

#include "terrain_textures.h"
#include "window_camera.h"
#include "window_input.h"
#include "window_map_draw.h"
#include "window_render.h"
#include "window_state.h"
#include "window_update.h"

#include <SDL2/SDL_image.h>

namespace
{
    bool saveRendererScreenshot(
        SDL_Renderer* renderer)
    {
        if (!renderer)
        {
            return false;
        }

        int screenWidth = 0;
        int screenHeight = 0;

        if (
            SDL_GetRendererOutputSize(
                renderer,
                &screenWidth,
                &screenHeight
            ) != 0
        )
        {
            return false;
        }

        SDL_Surface* surface =
            SDL_CreateRGBSurfaceWithFormat(
                0,
                screenWidth,
                screenHeight,
                32,
                SDL_PIXELFORMAT_RGBA32
            );

        if (!surface)
        {
            return false;
        }

        if (
            SDL_RenderReadPixels(
                renderer,
                nullptr,
                surface->format->format,
                surface->pixels,
                surface->pitch
            ) != 0
        )
        {
            SDL_FreeSurface(surface);
            return false;
        }

        const bool saved =
            IMG_SavePNG(
                surface,
                "screenshot.png"
            ) == 0;

        SDL_FreeSurface(surface);

        return saved;
    }
}

void runWindow(
    SDL_Window* window,
    SDL_Renderer* renderer,
    TTF_Font* sharedFont,
    const Map& map,
    const MapState& state,
    GameState& gameState)
{
    (void)window;
    (void)state;

    if (!renderer)
    {
        return;
    }

    IMG_Init(
        IMG_INIT_PNG
    );

    SDL_SetHint(
        SDL_HINT_RENDER_SCALE_QUALITY,
        "0"
    );

    TerrainTextures::load(
        renderer
    );

    TTF_Font* titleFont = WindowRender::loadFont(24);
    TTF_Font* bodyFont = WindowRender::loadFont(18);

    if (!titleFont)
    {
        titleFont = sharedFont;
    }

    if (!bodyFont)
    {
        bodyFont = sharedFont;
    }

    WindowLoopState windowState;

    windowState.shell.activeMapMode = MapViewMode::terrain;
    windowState.shell.escapeMenuOpen = false;
    windowState.shell.gameSpeed = 1;
    windowState.shell.monthProgress = 0;
    windowState.shell.monthProgressTarget = 100;
    windowState.shell.statusText = "Months advance automatically.";

    windowState.camera.zoom = 8.0f;
    windowState.camera.minZoom = 2.0f;
    windowState.camera.maxZoom = 48.0f;

    windowState.selectedTileX = gameState.playerCounty.countyX;
    windowState.selectedTileY = gameState.playerCounty.countyY;
    windowState.selectedProvinceId =
        WindowRender::playerProvinceId(map, gameState);

    windowState.lastMonthAdvanceMs = SDL_GetTicks();

    const int mapPanelHeight = 0;
    const int edgeScrollMargin = 18;
    const float keyboardScrollSpeed = 12.0f;
    const float edgeScrollSpeed = 10.0f;

    bool screenshotKeyWasDown = false;

    SDL_GetRendererOutputSize(
        renderer,
        &windowState.screenWidth,
        &windowState.screenHeight
    );

    clampCamera(
        windowState.camera,
        windowState.screenWidth,
        windowState.screenHeight,
        mapPanelHeight,
        map
    );

    while (windowState.running)
    {
        SDL_GetRendererOutputSize(
            renderer,
            &windowState.screenWidth,
            &windowState.screenHeight
        );

        clampCamera(
            windowState.camera,
            windowState.screenWidth,
            windowState.screenHeight,
            mapPanelHeight,
            map
        );

        updateWindowSimulation(
            map,
            gameState,
            windowState
        );

        const WindowRender::MapShellRects shellRects =
            WindowRender::mapShellRects(
                windowState.screenWidth,
                windowState.screenHeight,
                mapPanelHeight,
                windowState.provinceOverlay.open
            );

        const SDL_Rect overlayRect =
            WindowRender::provinceOverlayRect(
                windowState.screenWidth,
                windowState.screenHeight,
                mapPanelHeight
            );

        processWindowEvents(
            renderer,
            bodyFont,
            map,
            gameState,
            shellRects,
            overlayRect,
            mapPanelHeight,
            windowState
        );

        updateWindowCamera(
            map,
            shellRects,
            overlayRect,
            mapPanelHeight,
            edgeScrollMargin,
            keyboardScrollSpeed,
            edgeScrollSpeed,
            windowState
        );

        drawWindowMap(
            renderer,
            map,
            gameState,
            windowState
        );

        WindowRender::drawMapShellHud(
            renderer,
            titleFont,
            bodyFont,
            windowState.shell,
            gameState,
            shellRects
        );

        if (
            windowState.provinceOverlay.open &&
            windowState.selectedProvinceId >= 0
        )
        {
            WindowRender::drawProvinceOverlay(
                renderer,
                titleFont,
                bodyFont,
                overlayRect,
                windowState.provinceOverlay,
                gameState,
                map,
                windowState.selectedProvinceId,
                windowState.selectingRootBuilding,
                windowState.selectingSubBuildingParentId
            );
        }

        if (windowState.shell.escapeMenuOpen)
        {
            WindowRender::drawEscapeMenu(
                renderer,
                titleFont,
                bodyFont,
                windowState.shell,
                shellRects
            );
        }

        const Uint8* keyboardState =
            SDL_GetKeyboardState(nullptr);

        const bool screenshotKeyDown =
            keyboardState &&
            keyboardState[SDL_SCANCODE_F12];

        if (
            screenshotKeyDown &&
            !screenshotKeyWasDown
        )
        {
            if (saveRendererScreenshot(renderer))
            {
                windowState.shell.statusText =
                    "Screenshot saved to screenshot.png.";
            }
            else
            {
                windowState.shell.statusText =
                    "Failed to save screenshot.";
            }
        }

        screenshotKeyWasDown =
            screenshotKeyDown;

        SDL_RenderPresent(renderer);
    }

    TerrainTextures::unload();

    IMG_Quit();

    if (titleFont && titleFont != sharedFont)
    {
        TTF_CloseFont(titleFont);
    }

    if (bodyFont && bodyFont != sharedFont)
    {
        TTF_CloseFont(bodyFont);
    }
}