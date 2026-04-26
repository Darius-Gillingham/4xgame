// File: window.cpp
// Commit: Replace right-drag camera with keyboard and edge scrolling, add clickable tiles, black tile borders, and white hover highlight

#include "window.h"
#include "game.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <string>

namespace
{
    struct Color
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };

    TTF_Font* loadFont(int size)
    {
        const std::array<const char*, 5> paths = {
            "C:/Windows/Fonts/georgia.ttf",
            "C:/Windows/Fonts/times.ttf",
            "C:/Windows/Fonts/timesbd.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/calibri.ttf"
        };

        for (const char* path : paths)
        {
            if (TTF_Font* font = TTF_OpenFont(path, size))
            {
                return font;
            }
        }

        return nullptr;
    }

    void drawText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color)
    {
        if (!renderer || !font)
        {
            return;
        }

        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
        if (!surface)
        {
            return;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dst{x, y, surface->w, surface->h};
        SDL_FreeSurface(surface);

        if (!texture)
        {
            return;
        }

        SDL_RenderCopy(renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }

    void drawPanel(SDL_Renderer* renderer, const SDL_Rect& rect)
    {
        SDL_SetRenderDrawColor(renderer, 239, 226, 198, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }

    void drawButton(SDL_Renderer* renderer, TTF_Font* font, const SDL_Rect& rect, const std::string& label, bool active)
    {
        SDL_SetRenderDrawColor(renderer, 232, 220, 196, 255);
        SDL_RenderFillRect(renderer, &rect);

        if (active)
        {
            SDL_SetRenderDrawColor(renderer, 120, 80, 40, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        }

        SDL_RenderDrawRect(renderer, &rect);

        if (!font)
        {
            return;
        }

        int textWidth = 0;
        int textHeight = 0;
        if (TTF_SizeUTF8(font, label.c_str(), &textWidth, &textHeight) != 0)
        {
            textWidth = 0;
            textHeight = 0;
        }

        const int textX = rect.x + (rect.w - textWidth) / 2;
        const int textY = rect.y + (rect.h - textHeight) / 2;
        drawText(renderer, font, label, textX, textY, SDL_Color{0, 0, 0, 255});
    }

    bool pointInRect(int x, int y, const SDL_Rect& rect)
    {
        return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
    }

    Color terrainColor(Terrain terrain)
    {
        switch (terrain)
        {
            case Terrain::tundra:     return {235, 235, 225};
            case Terrain::boreal:     return {24, 52, 28};
            case Terrain::woods:      return {42, 78, 40};
            case Terrain::highlands:  return {108, 126, 104};
            case Terrain::hills:      return {140, 104, 72};
            case Terrain::mountains:  return {140, 140, 140};
            case Terrain::plains:     return {168, 186, 92};
            case Terrain::farmlands:  return {92, 210, 84};
            case Terrain::marsh:      return {120, 188, 168};
            case Terrain::steppe:     return {210, 206, 120};
            case Terrain::arid:       return {186, 150, 104};
            case Terrain::desert:     return {226, 190, 92};
            case Terrain::ocean:      return {18, 46, 110};
            default:                  return {255, 0, 255};
        }
    }

    Color regionColor(Region region)
    {
        switch (region)
        {
            case Region::scandinavia:   return {120, 160, 210};
            case Region::france:        return {120, 180, 140};
            case Region::british_isles: return {210, 160, 120};
            case Region::italy:         return {180, 120, 160};
            case Region::south_germany: return {170, 140, 120};
            case Region::lowlands:      return {120, 210, 210};
            case Region::north_germany: return {180, 180, 120};
            case Region::poland:        return {210, 180, 120};
            case Region::baltic:        return {120, 140, 210};
            case Region::russia:        return {150, 120, 210};
            case Region::pontic_steppe: return {210, 210, 120};
            case Region::danubia:       return {120, 210, 140};
            case Region::balkans:       return {210, 120, 140};
            case Region::iberia:        return {210, 120, 120};
            case Region::anatolia:      return {210, 150, 90};
            default:                    return {18, 46, 110};
        }
    }

    Color countyColor(const Tile& tile)
    {
        if (!tile.land)
        {
            return {18, 46, 110};
        }

        if (tile.polityId >= 0)
        {
            return {186, 138, 42};
        }

        return {205, 193, 165};
    }

    void clampCamera(CameraState& camera, int screenWidth, int screenHeight, int panelHeight, const Map& map)
    {
        const float mapWidthPixels = static_cast<float>(map.width()) * camera.zoom;
        const float mapHeightPixels = static_cast<float>(map.height()) * camera.zoom;
        const float viewportWidth = static_cast<float>(screenWidth);
        const float viewportHeight = static_cast<float>(screenHeight - panelHeight);

        float minOffsetX = viewportWidth - mapWidthPixels;
        float maxOffsetX = 0.0f;
        float minOffsetY = viewportHeight - mapHeightPixels;
        float maxOffsetY = 0.0f;

        if (mapWidthPixels < viewportWidth)
        {
            minOffsetX = maxOffsetX = (viewportWidth - mapWidthPixels) * 0.5f;
        }

        if (mapHeightPixels < viewportHeight)
        {
            minOffsetY = maxOffsetY = (viewportHeight - mapHeightPixels) * 0.5f;
        }

        camera.offsetX = std::clamp(camera.offsetX, minOffsetX, maxOffsetX);
        camera.offsetY = std::clamp(camera.offsetY, minOffsetY, maxOffsetY);
    }

    void applyZoom(CameraState& camera, float zoomFactor, int mouseX, int mouseY, int panelHeight, int screenWidth, int screenHeight, const Map& map)
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
        if (mouseY < panelHeight)
        {
            return false;
        }

        const float worldX = (static_cast<float>(mouseX) - camera.offsetX) / camera.zoom;
        const float worldY = (static_cast<float>(mouseY - panelHeight) - camera.offsetY) / camera.zoom;

        tileX = static_cast<int>(std::floor(worldX));
        tileY = static_cast<int>(std::floor(worldY));

        return tileX >= 0 && tileY >= 0 && tileX < map.width() && tileY < map.height();
    }
}

void runWindow(
    SDL_Window* window,
    SDL_Renderer* renderer,
    TTF_Font* sharedFont,
    const Map& map,
    const MapState& state)
{
    (void)window;
    (void)state;

    if (!renderer)
    {
        return;
    }

    TTF_Font* titleFont = loadFont(24);
    TTF_Font* bodyFont = loadFont(18);

    if (!titleFont)
    {
        titleFont = sharedFont;
    }

    if (!bodyFont)
    {
        bodyFont = sharedFont;
    }

    enum class ViewMode
    {
        terrain,
        regions,
        counties
    };

    ViewMode viewMode = ViewMode::terrain;
    bool running = true;

    CameraState camera;
    camera.zoom = 8.0f;
    camera.minZoom = 2.0f;
    camera.maxZoom = 48.0f;

    int screenWidth = 0;
    int screenHeight = 0;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    const int topPanelHeight = 120;
    const int edgeScrollMargin = 18;
    const float keyboardScrollSpeed = 12.0f;
    const float edgeScrollSpeed = 10.0f;

    int hoveredTileX = -1;
    int hoveredTileY = -1;
    int selectedTileX = -1;
    int selectedTileY = -1;

    clampCamera(camera, screenWidth, screenHeight, topPanelHeight, map);

    while (running)
    {
        SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
        clampCamera(camera, screenWidth, screenHeight, topPanelHeight, map);

        const SDL_Rect topPanel{0, 0, screenWidth, topPanelHeight};
        const SDL_Rect terrainButton{20, 68, 180, 36};
        const SDL_Rect regionButton{220, 68, 180, 36};
        const SDL_Rect countyButton{420, 68, 180, 36};
        const SDL_Rect backButton{screenWidth - 220, 68, 200, 36};

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                const int mouseX = event.button.x;
                const int mouseY = event.button.y;

                if (pointInRect(mouseX, mouseY, terrainButton))
                {
                    viewMode = ViewMode::terrain;
                }
                else if (pointInRect(mouseX, mouseY, regionButton))
                {
                    viewMode = ViewMode::regions;
                }
                else if (pointInRect(mouseX, mouseY, countyButton))
                {
                    viewMode = ViewMode::counties;
                }
                else if (pointInRect(mouseX, mouseY, backButton))
                {
                    running = false;
                }
                else
                {
                    int tileX = -1;
                    int tileY = -1;
                    if (screenToTile(mouseX, mouseY, topPanelHeight, camera, map, tileX, tileY))
                    {
                        selectedTileX = tileX;
                        selectedTileY = tileY;
                    }
                }
            }
            else if (event.type == SDL_MOUSEWHEEL)
            {
                int mouseX = 0;
                int mouseY = 0;
                SDL_GetMouseState(&mouseX, &mouseY);

                if (mouseY >= topPanelHeight)
                {
                    if (event.wheel.y > 0)
                    {
                        applyZoom(camera, 1.15f, mouseX, mouseY, topPanelHeight, screenWidth, screenHeight, map);
                    }
                    else if (event.wheel.y < 0)
                    {
                        applyZoom(camera, 1.0f / 1.15f, mouseX, mouseY, topPanelHeight, screenWidth, screenHeight, map);
                    }
                }
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])
        {
            camera.offsetY += keyboardScrollSpeed;
        }
        if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])
        {
            camera.offsetY -= keyboardScrollSpeed;
        }
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
        {
            camera.offsetX += keyboardScrollSpeed;
        }
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
        {
            camera.offsetX -= keyboardScrollSpeed;
        }

        int mouseX = 0;
        int mouseY = 0;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (mouseY >= topPanelHeight)
        {
            if (mouseX <= edgeScrollMargin)
            {
                camera.offsetX += edgeScrollSpeed;
            }
            else if (mouseX >= screenWidth - edgeScrollMargin)
            {
                camera.offsetX -= edgeScrollSpeed;
            }

            if (mouseY <= topPanelHeight + edgeScrollMargin)
            {
                camera.offsetY += edgeScrollSpeed;
            }
            else if (mouseY >= screenHeight - edgeScrollMargin)
            {
                camera.offsetY -= edgeScrollSpeed;
            }
        }

        clampCamera(camera, screenWidth, screenHeight, topPanelHeight, map);

        hoveredTileX = -1;
        hoveredTileY = -1;
        screenToTile(mouseX, mouseY, topPanelHeight, camera, map, hoveredTileX, hoveredTileY);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawPanel(renderer, topPanel);

        drawText(renderer, titleFont, "Map Viewer", 20, 12, SDL_Color{0, 0, 0, 255});
        drawText(
            renderer,
            bodyFont,
            "WASD or arrow keys move. Mouse at screen edge scrolls. Mouse wheel zooms. Left click selects a tile.",
            20,
            38,
            SDL_Color{0, 0, 0, 255}
        );

        drawButton(renderer, bodyFont, terrainButton, "Terrain", viewMode == ViewMode::terrain);
        drawButton(renderer, bodyFont, regionButton, "Regions", viewMode == ViewMode::regions);
        drawButton(renderer, bodyFont, countyButton, "Counties", viewMode == ViewMode::counties);
        drawButton(renderer, bodyFont, backButton, "Back to Menu", false);

        std::string hoverText = "Hover: none";
        if (hoveredTileX >= 0 && hoveredTileY >= 0)
        {
            const Tile& hovered = map.at(hoveredTileX, hoveredTileY);
            hoverText =
                "Hover: (" + std::to_string(hoveredTileX) + ", " + std::to_string(hoveredTileY) + ") " +
                regionName(hovered.region);
        }

        std::string selectedText = "Selected: none";
        if (selectedTileX >= 0 && selectedTileY >= 0)
        {
            const Tile& selected = map.at(selectedTileX, selectedTileY);
            selectedText =
                "Selected: (" + std::to_string(selectedTileX) + ", " + std::to_string(selectedTileY) + ") " +
                regionName(selected.region);
        }

        drawText(renderer, bodyFont, hoverText, 20, 92, SDL_Color{0, 0, 0, 255});
        drawText(renderer, bodyFont, selectedText, 420, 92, SDL_Color{0, 0, 0, 255});

        for (int y = 0; y < map.height(); ++y)
        {
            for (int x = 0; x < map.width(); ++x)
            {
                const Tile& tile = map.at(x, y);

                Color color{};
                if (viewMode == ViewMode::terrain)
                {
                    color = terrainColor(tile.terrain);
                }
                else if (viewMode == ViewMode::regions)
                {
                    color = regionColor(tile.region);
                }
                else
                {
                    color = countyColor(tile);
                }

                const float drawX = camera.offsetX + (static_cast<float>(x) * camera.zoom);
                const float drawY = static_cast<float>(topPanelHeight) + camera.offsetY + (static_cast<float>(y) * camera.zoom);
                const int drawSize = std::max(1, static_cast<int>(std::ceil(camera.zoom)));

                if (drawX + drawSize < 0.0f || drawY + drawSize < static_cast<float>(topPanelHeight) ||
                    drawX >= static_cast<float>(screenWidth) || drawY >= static_cast<float>(screenHeight))
                {
                    continue;
                }

                SDL_Rect rect{
                    static_cast<int>(std::floor(drawX)),
                    static_cast<int>(std::floor(drawY)),
                    drawSize,
                    drawSize
                };

                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
                SDL_RenderFillRect(renderer, &rect);

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &rect);

                if (x == selectedTileX && y == selectedTileY)
                {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderDrawRect(renderer, &rect);

                    SDL_Rect inner{
                        rect.x + 1,
                        rect.y + 1,
                        std::max(1, rect.w - 2),
                        std::max(1, rect.h - 2)
                    };
                    SDL_RenderDrawRect(renderer, &inner);
                }
                else if (x == hoveredTileX && y == hoveredTileY)
                {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderDrawRect(renderer, &rect);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    if (titleFont && titleFont != sharedFont)
    {
        TTF_CloseFont(titleFont);
    }

    if (bodyFont && bodyFont != sharedFont)
    {
        TTF_CloseFont(bodyFont);
    }
}