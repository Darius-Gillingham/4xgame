// File: main.cpp
// Commit: Load saved province data into player province slot instead of removed singleton

#include "menu.h"
#include "window.h"
#include "map.h"
#include "map_internal.h"
#include "game.h"
#include "save_game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <array>
#include <iostream>
#include <string>

namespace
{
    constexpr const char* kDefaultSavePath = "save_game.txt";

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

    void drawCenteredText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int screenWidth, int screenHeight)
    {
        if (!renderer || !font)
        {
            return;
        }

        SDL_Color color{0, 0, 0, 255};
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
        if (!surface)
        {
            return;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture)
        {
            SDL_FreeSurface(surface);
            return;
        }

        SDL_Rect dst{
            (screenWidth - surface->w) / 2,
            (screenHeight - surface->h) / 2,
            surface->w,
            surface->h
        };

        SDL_FreeSurface(surface);
        SDL_RenderCopy(renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }

    void drawStatusScreen(SDL_Renderer* renderer, TTF_Font* font, const std::string& text)
    {
        int screenWidth = 0;
        int screenHeight = 0;
        SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

        SDL_SetRenderDrawColor(renderer, 239, 226, 198, 255);
        SDL_RenderClear(renderer);
        drawCenteredText(renderer, font, text, screenWidth, screenHeight);
        SDL_RenderPresent(renderer);
    }

    bool restoreMapFromSave(const SavedGameData& savedGameData, Map& map, std::string& reason)
    {
        if (savedGameData.mapWidth != map.width() || savedGameData.mapHeight != map.height())
        {
            reason = "Saved map dimensions do not match the destination map.";
            return false;
        }

        const int expectedTileCount = savedGameData.mapWidth * savedGameData.mapHeight;
        if (static_cast<int>(savedGameData.tiles.size()) != expectedTileCount)
        {
            reason = "Saved tile count does not match saved map dimensions.";
            return false;
        }

        for (int y = 0; y < savedGameData.mapHeight; ++y)
        {
            for (int x = 0; x < savedGameData.mapWidth; ++x)
            {
                const int index = y * savedGameData.mapWidth + x;
                const SavedTile& savedTile = savedGameData.tiles[static_cast<size_t>(index)];

                Tile tile;
                tile.land = savedTile.land;
                tile.terrain = savedTile.terrain;
                tile.region = savedTile.region;
                tile.polityId = savedTile.polityId;
                tile.provinceId = savedTile.provinceId;

                if (!map.setTile(x, y, tile))
                {
                    reason = "Failed to restore tile into map.";
                    return false;
                }
            }
        }

        reason = "Map restored from save.";
        return true;
    }

    bool loadSavedSession(Map& map, GameState& gameState, std::string& reason)
    {
        SavedGameData savedGameData;
        if (!loadGameFromFile(kDefaultSavePath, savedGameData, reason))
        {
            return false;
        }

        map = Map(savedGameData.mapWidth, savedGameData.mapHeight, savedGameData.mapSeed);

        if (!restoreMapFromSave(savedGameData, map, reason))
        {
            return false;
        }

        gameState = GameState{};

        if (!initializeGameState(map, savedGameData.playerCounty.setup, gameState))
        {
            reason = "Failed to restore province country state from saved map.";
            return false;
        }

        gameState.playerCounty = savedGameData.playerCounty;
        gameState.tickCount = savedGameData.tickCount;
        gameState.date = savedGameData.date;

        Province* province = provinceById(gameState, gameState.playerCounty.provinceId);

        if (!province)
        {
            reason = "Player province state could not be restored.";
            return false;
        }

        province->loadData(savedGameData.provinceData);

        reason = "Loaded saved game.";
        return true;
    }
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    if (TTF_Init() != 0)
    {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "4X Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        900,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
    );

    if (!window)
    {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer)
    {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* sharedFont = loadFont(22);

    bool appRunning = true;

    while (appRunning)
    {
        MenuSettings settings;

        if (!runMenu(window, renderer, sharedFont, settings))
        {
            break;
        }

        if (!settings.confirmed || settings.action == MenuAction::exit_game)
        {
            break;
        }

        Map map(1, 1, 0);
        GameState gameState;
        std::string reason;

        if (
            settings.action == MenuAction::continue_game ||
            settings.action == MenuAction::load_game
        )
        {
            drawStatusScreen(renderer, sharedFont, "Loading Save.");

            if (!loadSavedSession(map, gameState, reason))
            {
                std::cerr << reason << "\n";
                SDL_Delay(900);
                continue;
            }
        }
        else if (settings.action == MenuAction::new_game)
        {
            drawStatusScreen(renderer, sharedFont, "Generating Map.");
            SDL_Delay(50);

            map = Map(settings.mapWidth, settings.mapHeight, settings.seed);
            map.generate();

            if (!initializeGameState(map, settings.playerSetup, gameState))
            {
                std::cerr << "Failed to spawn player county in selected starting region.\n";
                break;
            }
        }
        else
        {
            continue;
        }

        MapState state;
        runWindow(window, renderer, sharedFont, map, state, gameState);
    }

    if (sharedFont)
    {
        TTF_CloseFont(sharedFont);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}