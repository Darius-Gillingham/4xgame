// File: window_render_map_shell_rects.cpp
// Commit: Remove duplicated province overlay close-button layout block outside mapShellRects

namespace WindowRender
{
    MapShellRects mapShellRects(
        int screenWidth,
        int screenHeight,
        int topPanelHeight,
        bool provinceOverlayOpen)
    {
        MapShellRects rects;

        rects.topResourceBar = SDL_Rect{0, 0, screenWidth, kTopResourceBarHeight};

        rects.speedPanel = SDL_Rect{
            screenWidth - 356,
            kTopResourceBarHeight + kShellPad,
            342,
            96
        };

        rects.pauseButton = SDL_Rect{
            rects.speedPanel.x + 14,
            rects.speedPanel.y + 48,
            42,
            kButtonHeight
        };

        rects.speedOneButton = SDL_Rect{
            rects.pauseButton.x + 50,
            rects.pauseButton.y,
            36,
            kButtonHeight
        };

        rects.speedTwoButton = SDL_Rect{
            rects.speedOneButton.x + 42,
            rects.pauseButton.y,
            36,
            kButtonHeight
        };

        rects.speedThreeButton = SDL_Rect{
            rects.speedTwoButton.x + 42,
            rects.pauseButton.y,
            36,
            kButtonHeight
        };

        rects.speedFourButton = SDL_Rect{
            rects.speedThreeButton.x + 42,
            rects.pauseButton.y,
            36,
            kButtonHeight
        };

        rects.speedFiveButton = SDL_Rect{
            rects.speedFourButton.x + 42,
            rects.pauseButton.y,
            36,
            kButtonHeight
        };

        rects.monthProgressBar = SDL_Rect{
            rects.speedPanel.x + 258,
            rects.speedPanel.y + 56,
            70,
            14
        };

        rects.mapModePanel = SDL_Rect{
            screenWidth - 178,
            screenHeight - 154,
            164,
            140
        };

        rects.terrainModeButton = SDL_Rect{
            rects.mapModePanel.x + 12,
            rects.mapModePanel.y + 38,
            140,
            kButtonHeight
        };

        rects.regionsModeButton = SDL_Rect{
            rects.mapModePanel.x + 12,
            rects.terrainModeButton.y + kButtonHeight + kButtonGap,
            140,
            kButtonHeight
        };

        rects.provincesModeButton = SDL_Rect{
            rects.mapModePanel.x + 12,
            rects.regionsModeButton.y + kButtonHeight + kButtonGap,
            140,
            kButtonHeight
        };

        rects.escapeMenu = centeredRect(screenWidth, screenHeight, 360, 292);

        rects.escapeCloseButton = SDL_Rect{
            rects.escapeMenu.x + rects.escapeMenu.w - 42,
            rects.escapeMenu.y + 10,
            28,
            28
        };

        rects.escapeSaveButton = SDL_Rect{
            rects.escapeMenu.x + 34,
            rects.escapeMenu.y + 72,
            rects.escapeMenu.w - 68,
            42
        };

        rects.escapeLoadButton = SDL_Rect{
            rects.escapeMenu.x + 34,
            rects.escapeSaveButton.y + 54,
            rects.escapeMenu.w - 68,
            42
        };

        rects.escapeExitButton = SDL_Rect{
            rects.escapeMenu.x + 34,
            rects.escapeLoadButton.y + 54,
            rects.escapeMenu.w - 68,
            42
        };

        if (provinceOverlayOpen)
        {
            const SDL_Rect overlay =
                provinceOverlayRect(screenWidth, screenHeight, topPanelHeight);

            rects.provinceOverlayCloseButton = SDL_Rect{
                overlay.x + overlay.w - 42,
                overlay.y + 10,
                28,
                28
            };
        }

        return rects;
    }
}