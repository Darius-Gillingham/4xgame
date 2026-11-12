// File: window_render.h
// Commit: Declare province overlay scroll-height and Army geometry helpers

#pragma once

#include "building/province_screen_types.h"
#include "game.h"
#include "map.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>

namespace WindowRender
{
    struct Color
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };

    struct MapShellRects
    {
        SDL_Rect topResourceBar{};
        SDL_Rect speedPanel{};
        SDL_Rect pauseButton{};
        SDL_Rect speedOneButton{};
        SDL_Rect speedTwoButton{};
        SDL_Rect speedThreeButton{};
        SDL_Rect speedFourButton{};
        SDL_Rect speedFiveButton{};
        SDL_Rect monthProgressBar{};
        SDL_Rect mapModePanel{};
        SDL_Rect terrainModeButton{};
        SDL_Rect regionsModeButton{};
        SDL_Rect provincesModeButton{};
        SDL_Rect escapeMenu{};
        SDL_Rect escapeCloseButton{};
        SDL_Rect escapeSaveButton{};
        SDL_Rect escapeLoadButton{};
        SDL_Rect escapeExitButton{};
        SDL_Rect provinceOverlayCloseButton{};
    };

    TTF_Font* loadFont(int size);

    void drawText(
        SDL_Renderer* renderer,
        TTF_Font* font,
        const std::string& text,
        int x,
        int y,
        SDL_Color color
    );

    void drawPanel(
        SDL_Renderer* renderer,
        const SDL_Rect& rect
    );

    void drawButton(
        SDL_Renderer* renderer,
        TTF_Font* font,
        const SDL_Rect& rect,
        const std::string& label,
        bool active
    );

    bool pointInRect(
        int x,
        int y,
        const SDL_Rect& rect
    );

    Color terrainColor(
        Terrain terrain
    );

    Color regionColor(
        Region region
    );

    Color provinceColor(
        const Tile& tile
    );

    int playerProvinceId(
        const Map& map,
        const GameState& gameState
    );

    bool isPlayerProvinceTile(
        const Map& map,
        const GameState& gameState,
        int tileX,
        int tileY
    );

    MapShellRects mapShellRects(
        int screenWidth,
        int screenHeight,
        int topPanelHeight,
        bool provinceOverlayOpen
    );

    SDL_Rect provinceOverlayRect(
        int screenWidth,
        int screenHeight,
        int topPanelHeight
    );

    SDL_Rect provinceBuildingsTabRect(
        const SDL_Rect& overlayRect
    );

    SDL_Rect provincePopulationTabRect(
        const SDL_Rect& overlayRect
    );

    SDL_Rect provinceArmyTabRect(
        const SDL_Rect& overlayRect
    );

    SDL_Rect provinceAddBuildingButtonRect(
        const SDL_Rect& overlayRect
    );

    SDL_Rect provinceAddBuildingButtonRectAtY(
        const SDL_Rect& overlayRect,
        int y
    );

    SDL_Rect provinceSacredSiteCardRect(
        const SDL_Rect& overlayRect
    );

    SDL_Rect provinceSubBuildingCardRect(
        const SDL_Rect& overlayRect,
        int cardIndex
    );

    SDL_Rect provinceArmyLevyRowRect(
        const SDL_Rect& overlayRect,
        int rowIndex,
        int scrollOffset
    );

    SDL_Rect provinceArmyLevyButtonRect(
        const SDL_Rect& overlayRect,
        int rowIndex,
        int scrollOffset
    );

    int provinceHoldingsContentHeight(
        TTF_Font* bodyFont,
        const SDL_Rect& overlayRect,
        const Province& province
    );

    int provincePopulationContentHeight(
        TTF_Font* bodyFont,
        const Map& map,
        int selectedProvinceId,
        const Province& province
    );

    int provinceArmyContentHeight(
        TTF_Font* bodyFont,
        const Province& province
    );

    void drawMapShellHud(
        SDL_Renderer* renderer,
        TTF_Font* titleFont,
        TTF_Font* bodyFont,
        const MapShellState& shell,
        const GameState& gameState,
        const MapShellRects& rects
    );

    void drawEscapeMenu(
        SDL_Renderer* renderer,
        TTF_Font* titleFont,
        TTF_Font* bodyFont,
        const MapShellState& shell,
        const MapShellRects& rects
    );

    void drawProvinceOverlay(
        SDL_Renderer* renderer,
        TTF_Font* titleFont,
        TTF_Font* bodyFont,
        const SDL_Rect& overlayRect,
        const ProvinceOverlayState& overlay,
        const GameState& gameState,
        const Map& map,
        int selectedProvinceId,
        bool selectingRootBuilding,
        int selectingSubBuildingParentId
    );
}