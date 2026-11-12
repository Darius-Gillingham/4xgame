// File: window_render_hud.cpp
// Commit: Move map shell HUD and escape menu rendering

namespace WindowRender
{
    void drawMapShellHud(
        SDL_Renderer* renderer,
        TTF_Font* titleFont,
        TTF_Font* bodyFont,
        const MapShellState& shell,
        const GameState& gameState,
        const MapShellRects& rects)
    {
        (void)titleFont;

        drawPanel(renderer, rects.topResourceBar);

        const Province* playerProvince = provinceById(gameState, gameState.playerCounty.provinceId);
        const int prestige = playerProvince ? playerProvince->prestige() : 0;

        drawText(
            renderer,
            bodyFont,
            "Prestige: " + std::to_string(prestige),
            rects.topResourceBar.x + 16,
            rects.topResourceBar.y + 11,
            SDL_Color{0, 0, 0, 255}
        );

        drawPanel(renderer, rects.speedPanel);

        drawText(
            renderer,
            bodyFont,
            "Time",
            rects.speedPanel.x + 14,
            rects.speedPanel.y + 12,
            SDL_Color{0, 0, 0, 255}
        );

        drawButton(renderer, bodyFont, rects.pauseButton, "||", shell.gameSpeed == 0);
        drawButton(renderer, bodyFont, rects.speedOneButton, "1", shell.gameSpeed == 1);
        drawButton(renderer, bodyFont, rects.speedTwoButton, "2", shell.gameSpeed == 2);
        drawButton(renderer, bodyFont, rects.speedThreeButton, "3", shell.gameSpeed == 3);
        drawButton(renderer, bodyFont, rects.speedFourButton, "4", shell.gameSpeed == 4);
        drawButton(renderer, bodyFont, rects.speedFiveButton, "5", shell.gameSpeed == 5);

        SDL_SetRenderDrawColor(renderer, 210, 198, 174, 255);
        SDL_RenderFillRect(renderer, &rects.monthProgressBar);

        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderDrawRect(renderer, &rects.monthProgressBar);

        const int safeTarget = std::max(1, shell.monthProgressTarget);
        const int clampedProgress = std::clamp(shell.monthProgress, 0, safeTarget);
        const int fillWidth = (rects.monthProgressBar.w * clampedProgress) / safeTarget;

        SDL_Rect fillRect{
            rects.monthProgressBar.x,
            rects.monthProgressBar.y,
            fillWidth,
            rects.monthProgressBar.h
        };

        SDL_SetRenderDrawColor(renderer, 120, 80, 40, 255);
        SDL_RenderFillRect(renderer, &fillRect);

        drawPanel(renderer, rects.mapModePanel);

        drawText(
            renderer,
            titleFont,
            "Map Mode",
            rects.mapModePanel.x + 12,
            rects.mapModePanel.y + 8,
            SDL_Color{0, 0, 0, 255}
        );

        drawButton(
            renderer,
            bodyFont,
            rects.terrainModeButton,
            "Terrain",
            shell.activeMapMode == MapViewMode::terrain
        );

        drawButton(
            renderer,
            bodyFont,
            rects.regionsModeButton,
            "Regions",
            shell.activeMapMode == MapViewMode::regions
        );

        drawButton(
            renderer,
            bodyFont,
            rects.provincesModeButton,
            "Provinces",
            shell.activeMapMode == MapViewMode::provinces
        );
    }


    void drawEscapeMenu(
        SDL_Renderer* renderer,
        TTF_Font* titleFont,
        TTF_Font* bodyFont,
        const MapShellState& shell,
        const MapShellRects& rects)
    {
        if (!shell.escapeMenuOpen)
        {
            return;
        }

        drawPanel(renderer, rects.escapeMenu);

        drawText(
            renderer,
            titleFont,
            "Game Menu",
            rects.escapeMenu.x + 24,
            rects.escapeMenu.y + 16,
            SDL_Color{0, 0, 0, 255}
        );

        drawButton(renderer, bodyFont, rects.escapeCloseButton, "X", false);
        drawButton(renderer, bodyFont, rects.escapeSaveButton, "Save Game", false);
        drawButton(renderer, bodyFont, rects.escapeLoadButton, "Load Game", false);
        drawButton(renderer, bodyFont, rects.escapeExitButton, "Exit Game", false);
    }

}
