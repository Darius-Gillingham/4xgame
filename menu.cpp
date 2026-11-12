// File: menu.cpp
// Commit: Reduce new game setup to map size only

#include "menu.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <string>

namespace
{
    constexpr const char* kDefaultSavePath = "save_game.txt";

    enum class MenuView
    {
        main_menu,
        new_game_setup
    };

    bool saveFileExists()
    {
        std::ifstream file(kDefaultSavePath);
        return file.good();
    }

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

    void drawWrappedText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, int wrapWidth, SDL_Color color)
    {
        if (!renderer || !font)
        {
            return;
        }

        SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), color, wrapWidth);
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

    void drawFieldBox(SDL_Renderer* renderer, const SDL_Rect& rect, bool active)
    {
        SDL_SetRenderDrawColor(renderer, 232, 220, 196, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, active ? 120 : 40, active ? 80 : 40, active ? 40 : 40, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }

    void drawButton(SDL_Renderer* renderer, TTF_Font* font, const SDL_Rect& rect, const std::string& label, bool active = false)
    {
        SDL_SetRenderDrawColor(renderer, 232, 220, 196, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, active ? 120 : 40, active ? 80 : 40, active ? 40 : 40, 255);
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

        drawText(
            renderer,
            font,
            label,
            rect.x + (rect.w - textWidth) / 2,
            rect.y + (rect.h - textHeight) / 2,
            SDL_Color{0, 0, 0, 255}
        );
    }

    void drawDisabledButton(SDL_Renderer* renderer, TTF_Font* font, const SDL_Rect& rect, const std::string& label)
    {
        SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
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

        drawText(
            renderer,
            font,
            label,
            rect.x + (rect.w - textWidth) / 2,
            rect.y + (rect.h - textHeight) / 2,
            SDL_Color{90, 90, 90, 255}
        );
    }

    bool pointInRect(int x, int y, const SDL_Rect& rect)
    {
        return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
    }

    void eraseLastCharacter(std::string& value)
    {
        if (!value.empty())
        {
            value.pop_back();
        }
    }

    int toClampedInt(const std::string& value, int fallback, int minValue, int maxValue)
    {
        try
        {
            return std::clamp(std::stoi(value), minValue, maxValue);
        }
        catch (...)
        {
            return std::clamp(fallback, minValue, maxValue);
        }
    }
}

bool runMenu(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* sharedFont, MenuSettings& settings)
{
    (void)window;

    if (!renderer)
    {
        return false;
    }

    TTF_Font* titleFont = loadFont(34);
    TTF_Font* labelFont = loadFont(26);
    TTF_Font* bodyFont = loadFont(20);

    if (!titleFont)
    {
        titleFont = sharedFont;
    }

    if (!labelFont)
    {
        labelFont = sharedFont;
    }

    if (!bodyFont)
    {
        bodyFont = sharedFont;
    }

    settings.confirmed = false;
    settings.action = MenuAction::none;

    MenuView view = MenuView::main_menu;

    std::string widthValue = std::to_string(settings.mapWidth);
    std::string heightValue = std::to_string(settings.mapHeight);

    enum class ActiveField
    {
        none,
        width,
        height
    };

    ActiveField activeField = ActiveField::none;
    bool running = true;

    const SDL_Rect continueButton{120, 190, 340, 58};
    const SDL_Rect loadButton{120, 270, 340, 58};
    const SDL_Rect newButton{120, 350, 340, 58};
    const SDL_Rect mainExitButton{120, 430, 340, 58};

    const SDL_Rect widthBox{780, 190, 220, 44};
    const SDL_Rect heightBox{780, 320, 220, 44};
    const SDL_Rect generateButton{120, 480, 280, 56};
    const SDL_Rect backButton{430, 480, 220, 56};

    SDL_StartTextInput();

    while (running)
    {
        if (view == MenuView::main_menu)
        {
            const bool saveExists = saveFileExists();

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    settings.action = MenuAction::exit_game;
                    settings.confirmed = false;
                    running = false;
                }
                else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
                {
                    const int mouseX = event.button.x;
                    const int mouseY = event.button.y;

                    if (pointInRect(mouseX, mouseY, continueButton) && saveExists)
                    {
                        settings.action = MenuAction::continue_game;
                        settings.confirmed = true;
                        running = false;
                    }
                    else if (pointInRect(mouseX, mouseY, loadButton) && saveExists)
                    {
                        settings.action = MenuAction::load_game;
                        settings.confirmed = true;
                        running = false;
                    }
                    else if (pointInRect(mouseX, mouseY, newButton))
                    {
                        view = MenuView::new_game_setup;
                        activeField = ActiveField::none;
                    }
                    else if (pointInRect(mouseX, mouseY, mainExitButton))
                    {
                        settings.action = MenuAction::exit_game;
                        settings.confirmed = false;
                        running = false;
                    }
                }
            }

            SDL_SetRenderDrawColor(renderer, 239, 226, 198, 255);
            SDL_RenderClear(renderer);

            drawText(renderer, titleFont, "4X Game", 120, 70, SDL_Color{0, 0, 0, 255});
            drawWrappedText(
                renderer,
                bodyFont,
                "Continue loads the default save. Load Game uses the same save path for now. New Game opens map size setup.",
                120,
                122,
                780,
                SDL_Color{0, 0, 0, 255}
            );

            if (saveExists)
            {
                drawButton(renderer, labelFont, continueButton, "Continue");
                drawButton(renderer, labelFont, loadButton, "Load Game");
            }
            else
            {
                drawDisabledButton(renderer, labelFont, continueButton, "Continue");
                drawDisabledButton(renderer, labelFont, loadButton, "Load Game");
            }

            drawButton(renderer, labelFont, newButton, "New Game");
            drawButton(renderer, labelFont, mainExitButton, "Exit");

            SDL_RenderPresent(renderer);
            continue;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                settings.action = MenuAction::exit_game;
                settings.confirmed = false;
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                const int mouseX = event.button.x;
                const int mouseY = event.button.y;

                if (pointInRect(mouseX, mouseY, widthBox))
                {
                    activeField = ActiveField::width;
                }
                else if (pointInRect(mouseX, mouseY, heightBox))
                {
                    activeField = ActiveField::height;
                }
                else if (pointInRect(mouseX, mouseY, generateButton))
                {
                    settings.mapWidth = toClampedInt(widthValue, settings.mapWidth, 64, 1000);
                    settings.mapHeight = toClampedInt(heightValue, settings.mapHeight, 48, 1000);
                    settings.seed = static_cast<std::uint32_t>(SDL_GetTicks());
                    settings.useRandomSeed = true;

                    settings.action = MenuAction::new_game;
                    settings.confirmed = true;
                    running = false;
                }
                else if (pointInRect(mouseX, mouseY, backButton))
                {
                    view = MenuView::main_menu;
                    activeField = ActiveField::none;
                }
                else
                {
                    activeField = ActiveField::none;
                }
            }
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE)
            {
                if (activeField == ActiveField::width)
                {
                    eraseLastCharacter(widthValue);
                }
                else if (activeField == ActiveField::height)
                {
                    eraseLastCharacter(heightValue);
                }
            }
            else if (event.type == SDL_TEXTINPUT)
            {
                const char c = event.text.text[0];

                if (activeField == ActiveField::width && c >= '0' && c <= '9' && widthValue.size() < 4)
                {
                    widthValue += c;
                }
                else if (activeField == ActiveField::height && c >= '0' && c <= '9' && heightValue.size() < 4)
                {
                    heightValue += c;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 239, 226, 198, 255);
        SDL_RenderClear(renderer);

        drawText(renderer, titleFont, "Map Setup", 120, 70, SDL_Color{0, 0, 0, 255});
        drawWrappedText(
            renderer,
            bodyFont,
            "Set the generated map size. All other starting setup is handled by game defaults.",
            120,
            122,
            900,
            SDL_Color{0, 0, 0, 255}
        );

        drawText(renderer, labelFont, "Map Width", 120, 190, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "How many tiles the map spans from left to right.", 120, 225, 560, SDL_Color{0, 0, 0, 255});
        drawFieldBox(renderer, widthBox, activeField == ActiveField::width);
        drawText(renderer, labelFont, widthValue.empty() ? "0" : widthValue, widthBox.x + 16, widthBox.y + 8, SDL_Color{0, 0, 0, 255});

        drawText(renderer, labelFont, "Map Height", 120, 320, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "How many tiles the map spans from top to bottom.", 120, 355, 560, SDL_Color{0, 0, 0, 255});
        drawFieldBox(renderer, heightBox, activeField == ActiveField::height);
        drawText(renderer, labelFont, heightValue.empty() ? "0" : heightValue, heightBox.x + 16, heightBox.y + 8, SDL_Color{0, 0, 0, 255});

        drawButton(renderer, labelFont, generateButton, "Generate Map");
        drawButton(renderer, labelFont, backButton, "Back");

        SDL_RenderPresent(renderer);
    }

    SDL_StopTextInput();

    if (titleFont && titleFont != sharedFont)
    {
        TTF_CloseFont(titleFont);
    }

    if (labelFont && labelFont != sharedFont)
    {
        TTF_CloseFont(labelFont);
    }

    if (bodyFont && bodyFont != sharedFont)
    {
        TTF_CloseFont(bodyFont);
    }

    return settings.confirmed;
}