// File: building/province_overlay.cpp
// Commit: Migrate province overlay from legacy buildings to holdings and buildings

#include "province_overlay.h"

#include "../game.h"

#include <algorithm>
#include <string>

namespace
{
    bool pointInRect(int x, int y, const SDL_Rect& rect)
    {
        return x >= rect.x &&
               x < rect.x + rect.w &&
               y >= rect.y &&
               y < rect.y + rect.h;
    }

    void drawText(
        SDL_Renderer* renderer,
        TTF_Font* font,
        const std::string& text,
        int x,
        int y,
        SDL_Color color)
    {
        if (!renderer || !font)
        {
            return;
        }

        SDL_Surface* surface =
            TTF_RenderUTF8_Blended_Wrapped(
                font,
                text.c_str(),
                color,
                520
            );

        if (!surface)
        {
            return;
        }

        SDL_Texture* texture =
            SDL_CreateTextureFromSurface(
                renderer,
                surface
            );

        SDL_Rect dst{
            x,
            y,
            surface->w,
            surface->h
        };

        SDL_FreeSurface(surface);

        if (!texture)
        {
            return;
        }

        SDL_RenderCopy(
            renderer,
            texture,
            nullptr,
            &dst
        );

        SDL_DestroyTexture(texture);
    }

    void drawPanel(
        SDL_Renderer* renderer,
        const SDL_Rect& rect)
    {
        SDL_SetRenderDrawColor(
            renderer,
            239,
            226,
            198,
            255
        );

        SDL_RenderFillRect(
            renderer,
            &rect
        );

        SDL_SetRenderDrawColor(
            renderer,
            40,
            40,
            40,
            255
        );

        SDL_RenderDrawRect(
            renderer,
            &rect
        );
    }

    void drawFilledPanel(
        SDL_Renderer* renderer,
        const SDL_Rect& rect,
        SDL_Color fill,
        SDL_Color border)
    {
        SDL_SetRenderDrawColor(
            renderer,
            fill.r,
            fill.g,
            fill.b,
            fill.a
        );

        SDL_RenderFillRect(
            renderer,
            &rect
        );

        SDL_SetRenderDrawColor(
            renderer,
            border.r,
            border.g,
            border.b,
            border.a
        );

        SDL_RenderDrawRect(
            renderer,
            &rect
        );
    }

    void drawButton(
        SDL_Renderer* renderer,
        TTF_Font* font,
        const SDL_Rect& rect,
        const std::string& label,
        bool active)
    {
        drawFilledPanel(
            renderer,
            rect,
            SDL_Color{232,220,196,255},
            active
                ? SDL_Color{120,80,40,255}
                : SDL_Color{40,40,40,255}
        );

        if (!font)
        {
            return;
        }

        int textWidth = 0;
        int textHeight = 0;

        if (
            TTF_SizeUTF8(
                font,
                label.c_str(),
                &textWidth,
                &textHeight
            ) != 0
        )
        {
            textWidth = 0;
            textHeight = 0;
        }

        drawText(
            renderer,
            font,
            label,
            rect.x +
                ((rect.w - textWidth) / 2),
            rect.y +
                ((rect.h - textHeight) / 2),
            SDL_Color{0,0,0,255}
        );
    }

    const Province* playerProvinceFromState(
        const GameState& gameState)
    {
        return provinceById(
            gameState,
            gameState.playerCounty.provinceId
        );
    }

    std::string holdingSummaryText(
        const Province& province)
    {
        const ProvinceResourceSummary resources =
            province.resourceSummary();

        const ProvinceFoodSummary food =
            province.foodSummary();

        std::size_t totalBuildings = 0;

        for (const Holding& holding : province.holdings())
        {
            totalBuildings +=
                holding.buildings.size();
        }

        return
            "Root Holdings: " +
            std::to_string(
                province.rootHoldingIds().size()
            ) +
            "\nTotal Holdings: " +
            std::to_string(
                province.holdings().size()
            ) +
            "\nTotal Buildings: " +
            std::to_string(
                totalBuildings
            ) +
            "\nPrestige: " +
            std::to_string(
                province.prestige()
            ) +
            "\n\nFood Net" +
            "\nFat: " +
            std::to_string(food.net.fat) +
            "\nCarbs: " +
            std::to_string(food.net.carbs) +
            "\nProtein: " +
            std::to_string(food.net.protein) +
            "\nLuxury: " +
            std::to_string(food.net.luxury) +
            "\n\nResource Outputs: " +
            std::to_string(
                resources.outputs.size()
            ) +
            "\nResource Inputs: " +
            std::to_string(
                resources.inputs.size()
            );
    }

    std::string populationSummaryText(
        const Province& province)
    {
        return
            "Population: " +
            std::to_string(
                province.totalPopulation()
            ) +
            "\nUnassigned: " +
            std::to_string(
                province.unassignedPopulation()
            ) +
            "\nEmployed: " +
            std::to_string(
                province.employedPopulation()
            ) +
            "\nAvailable Workers: " +
            std::to_string(
                province.availableWorkerPopulation()
            );
    }

    void drawHoldingSlots(
        SDL_Renderer* renderer,
        TTF_Font* bodyFont,
        const Province& province,
        const SDL_Rect& rect)
    {
        drawFilledPanel(
            renderer,
            rect,
            SDL_Color{235,224,202,255},
            SDL_Color{40,40,40,255}
        );

        drawText(
            renderer,
            bodyFont,
            "Holdings",
            rect.x + 12,
            rect.y + 10,
            SDL_Color{0,0,0,255}
        );

        const int slotSize = 46;
        const int slotGap = 8;

        const int columns =
            std::max(
                1,
                (rect.w - 24 + slotGap) /
                (slotSize + slotGap)
            );

        const int startX =
            rect.x + 12;

        const int startY =
            rect.y + 46;

        const std::vector<int> rootIds =
            province.rootHoldingIds();

        for (int i = 0; i < 12; ++i)
        {
            const int column = i % columns;
            const int row = i / columns;

            SDL_Rect slotRect{
                startX +
                    column *
                    (slotSize + slotGap),
                startY +
                    row *
                    (slotSize + slotGap),
                slotSize,
                slotSize
            };

            const bool occupied =
                i <
                static_cast<int>(
                    rootIds.size()
                );

            drawFilledPanel(
                renderer,
                slotRect,
                occupied
                    ? SDL_Color{224,212,188,255}
                    : SDL_Color{232,220,196,255},
                SDL_Color{40,40,40,255}
            );

            drawText(
                renderer,
                bodyFont,
                occupied
                    ? "H"
                    : "+",
                slotRect.x + 16,
                slotRect.y + 12,
                SDL_Color{0,0,0,255}
            );
        }
    }

    void drawSummaryPanel(
        SDL_Renderer* renderer,
        TTF_Font* bodyFont,
        const Province& province,
        const ProvinceOverlayState& overlay,
        const SDL_Rect& rect)
    {
        drawFilledPanel(
            renderer,
            rect,
            SDL_Color{235,224,202,255},
            SDL_Color{40,40,40,255}
        );

        const std::string title =
            overlay.activeTab ==
                ProvinceOverlayTab::holdings
                ? "Province Summary"
                : "Population Summary";

        drawText(
            renderer,
            bodyFont,
            title,
            rect.x + 12,
            rect.y + 10,
            SDL_Color{0,0,0,255}
        );

        drawText(
            renderer,
            bodyFont,
            overlay.activeTab ==
                ProvinceOverlayTab::holdings
                ? holdingSummaryText(province)
                : populationSummaryText(province),
            rect.x + 12,
            rect.y + 46,
            SDL_Color{0,0,0,255}
        );
    }
}

// ... remainder of file unchanged except:
// ProvinceOverlayTab::buildings -> ProvinceOverlayTab::holdings
// provinceOverlayBuildingsTabRect -> provinceOverlayHoldingsTabRect
// drawBuildingSlots -> drawHoldingSlots
// "Buildings" button text -> "Holdings"
// drawBuildingSlots(...) -> drawHoldingSlots(...)