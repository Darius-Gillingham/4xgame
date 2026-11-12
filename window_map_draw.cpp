// File: window_map_draw.cpp
// Commit: Remove unwanted player province tile highlighting from map rendering

#include "window_map_draw.h"

#include "terrain_textures.h"
#include "window_helpers.h"
#include "window_render.h"

#include <algorithm>
#include <cmath>

void drawWindowMap(
    SDL_Renderer* renderer,
    const Map& map,
    const GameState& gameState,
    const WindowLoopState& windowState
)
{
    SDL_SetRenderDrawColor(
        renderer,
        0,
        0,
        0,
        255
    );

    SDL_RenderClear(renderer);

    const int tilePixelSize =
        std::max(
            1,
            static_cast<int>(
                windowState.camera.zoom + 1.0f
            )
        );

    const int firstWorldX =
        static_cast<int>(
            std::floor(
                (
                    0.0f -
                    windowState.camera.offsetX
                ) /
                windowState.camera.zoom
            )
        ) -
        1;

    const int lastWorldX =
        static_cast<int>(
            std::ceil(
                (
                    static_cast<float>(
                        windowState.screenWidth
                    ) -
                    windowState.camera.offsetX
                ) /
                windowState.camera.zoom
            )
        ) +
        1;

    const int firstWorldY =
        std::max(
            0,
            static_cast<int>(
                std::floor(
                    (
                        0.0f -
                        windowState.camera.offsetY
                    ) /
                    windowState.camera.zoom
                )
            ) -
                1
        );

    const int lastWorldY =
        std::min(
            map.height() - 1,
            static_cast<int>(
                std::ceil(
                    (
                        static_cast<float>(
                            windowState.screenHeight
                        ) -
                        windowState.camera.offsetY
                    ) /
                    windowState.camera.zoom
                )
            ) +
                1
        );

    for (
        int y = firstWorldY;
        y <= lastWorldY;
        ++y
    )
    {
        for (
            int worldX = firstWorldX;
            worldX <= lastWorldX;
            ++worldX
        )
        {
            const int tileX =
                wrapTileX(
                    worldX,
                    map.width()
                );

            const Tile& tile =
                map.at(
                    tileX,
                    y
                );

            WindowRender::Color color{};

            switch (
                windowState.shell.activeMapMode
            )
            {
                case MapViewMode::terrain:
                    color =
                        WindowRender::terrainColor(
                            tile.terrain
                        );
                    break;

                case MapViewMode::regions:
                    color =
                        WindowRender::regionColor(
                            tile.region
                        );
                    break;

                case MapViewMode::provinces:
                    color =
                        WindowRender::provinceColor(
                            tile
                        );
                    break;
            }

            const float drawX =
                windowState.camera.offsetX +
                static_cast<float>(
                    worldX
                ) *
                    windowState.camera.zoom;

            const float drawY =
                windowState.camera.offsetY +
                static_cast<float>(
                    y
                ) *
                    windowState.camera.zoom;

            SDL_Rect rect{
                static_cast<int>(
                    drawX
                ),
                static_cast<int>(
                    drawY
                ),
                tilePixelSize,
                tilePixelSize
            };

            SDL_Texture* terrainTexture =
                nullptr;

            if (
                windowState.shell.activeMapMode ==
                MapViewMode::terrain
            )
            {
                terrainTexture =
                    TerrainTextures::texture(
                        tile.terrain
                    );
            }

            if (terrainTexture)
            {
                SDL_RenderCopy(
                    renderer,
                    terrainTexture,
                    nullptr,
                    &rect
                );
            }
            else
            {
                SDL_SetRenderDrawColor(
                    renderer,
                    color.r,
                    color.g,
                    color.b,
                    255
                );

                SDL_RenderFillRect(
                    renderer,
                    &rect
                );
            }

            if (
                tile.provinceId >= 0 &&
                tile.provinceId ==
                    windowState.selectedProvinceId
            )
            {
                SDL_SetRenderDrawColor(
                    renderer,
                    0,
                    0,
                    0,
                    255
                );

                SDL_RenderDrawRect(
                    renderer,
                    &rect
                );
            }
        }
    }

    const int unitMarkerSize =
        std::max(
            6,
            static_cast<int>(
                windowState.camera.zoom *
                0.8f
            )
        );

    for (
        const Unit& unit :
        gameState.units
    )
    {
        if (
            unit.position.x < 0 ||
            unit.position.x >= map.width() ||
            unit.position.y < firstWorldY ||
            unit.position.y > lastWorldY
        )
        {
            continue;
        }

        const int firstUnitWorldX =
            unit.position.x +
            (
                (
                    firstWorldX -
                    unit.position.x
                ) /
                map.width()
            ) *
                map.width();

        for (
            int worldX =
                firstUnitWorldX -
                map.width();
            worldX <= lastWorldX;
            worldX += map.width()
        )
        {
            if (worldX < firstWorldX)
            {
                continue;
            }

            const float unitCenterX =
                windowState.camera.offsetX +
                (
                    static_cast<float>(
                        worldX
                    ) +
                    0.5f
                ) *
                    windowState.camera.zoom;

            const float unitCenterY =
                windowState.camera.offsetY +
                (
                    static_cast<float>(
                        unit.position.y
                    ) +
                    0.5f
                ) *
                    windowState.camera.zoom;

            SDL_Rect unitRect{
                static_cast<int>(
                    unitCenterX -
                    static_cast<float>(
                        unitMarkerSize
                    ) /
                        2.0f
                ),
                static_cast<int>(
                    unitCenterY -
                    static_cast<float>(
                        unitMarkerSize
                    ) /
                        2.0f
                ),
                unitMarkerSize,
                unitMarkerSize
            };

            if (
                unit.countryId ==
                gameState.playerCounty.countryId
            )
            {
                SDL_SetRenderDrawColor(
                    renderer,
                    255,
                    255,
                    255,
                    255
                );
            }
            else
            {
                SDL_SetRenderDrawColor(
                    renderer,
                    32,
                    32,
                    32,
                    255
                );
            }

            SDL_RenderFillRect(
                renderer,
                &unitRect
            );

            SDL_SetRenderDrawColor(
                renderer,
                0,
                0,
                0,
                255
            );

            SDL_RenderDrawRect(
                renderer,
                &unitRect
            );
        }
    }
}