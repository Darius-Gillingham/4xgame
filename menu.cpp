// File: menu.cpp
// Commit: Refactor menu to run inside the shared fullscreen app window and renderer without creating its own SDL lifecycle

#include "menu.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <string>
#include <vector>

namespace
{
    constexpr std::array<Culture, 13> kCultures = {
        Culture::anglo_saxons,
        Culture::franks,
        Culture::frisians,
        Culture::norse,
        Culture::wends,
        Culture::saxons,
        Culture::goths,
        Culture::lombards,
        Culture::bulgars,
        Culture::magyars,
        Culture::byzantines,
        Culture::rus,
        Culture::poles
    };

    constexpr std::array<Religion, 3> kReligions = {
        Religion::catholic,
        Religion::orthodox,
        Religion::pagan
    };

    constexpr std::array<Government, 4> kGovernments = {
        Government::tribal,
        Government::aristocratic,
        Government::plutocratic,
        Government::theocratic
    };

    constexpr std::array<RulerTrait, 6> kTraits = {
        RulerTrait::martial,
        RulerTrait::builder,
        RulerTrait::pious,
        RulerTrait::trader,
        RulerTrait::scholar,
        RulerTrait::administrator
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
        if (!font)
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
        if (!font)
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

    void drawFieldBox(SDL_Renderer* renderer, const SDL_Rect& rect, bool active, bool enabled = true)
    {
        if (enabled)
        {
            SDL_SetRenderDrawColor(renderer, 232, 220, 196, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 216, 206, 186, 255);
        }

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
    }

    void drawButton(SDL_Renderer* renderer, TTF_Font* font, const SDL_Rect& rect, const std::string& label, bool active = false)
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

    std::uint32_t toSeedValue(const std::string& value, std::uint32_t fallback)
    {
        try
        {
            return static_cast<std::uint32_t>(std::stoul(value));
        }
        catch (...)
        {
            return fallback;
        }
    }

    SDL_Rect shiftedRect(const SDL_Rect& rect, int scrollOffset)
    {
        return SDL_Rect{rect.x, rect.y - scrollOffset, rect.w, rect.h};
    }

    int clampScrollOffset(int scrollOffset, int viewportHeight, int contentHeight)
    {
        const int maxScroll = std::max(0, contentHeight - viewportHeight);
        return std::clamp(scrollOffset, 0, maxScroll);
    }

    template <typename T, size_t N>
    T cycleValue(const std::array<T, N>& values, T current, int direction)
    {
        for (size_t i = 0; i < values.size(); ++i)
        {
            if (values[i] == current)
            {
                const int next = static_cast<int>(i) + direction;
                const int wrapped = (next + static_cast<int>(values.size())) % static_cast<int>(values.size());
                return values[static_cast<size_t>(wrapped)];
            }
        }

        return values.front();
    }

    Region cycleRegion(Culture culture, Region current, int direction)
    {
        const std::vector<Region>& regions = allowedStartingRegions(culture);
        if (regions.empty())
        {
            return Region::british_isles;
        }

        for (size_t i = 0; i < regions.size(); ++i)
        {
            if (regions[i] == current)
            {
                const int next = static_cast<int>(i) + direction;
                const int wrapped = (next + static_cast<int>(regions.size())) % static_cast<int>(regions.size());
                return regions[static_cast<size_t>(wrapped)];
            }
        }

        return regions.front();
    }

    bool isAllowedNameCharacter(char c)
    {
        return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == ' ' || c == '-' || c == '\'';
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

    if (!isAllowedStartingRegion(settings.playerSetup.culture, settings.playerSetup.startRegion))
    {
        settings.playerSetup.startRegion = defaultStartingRegion(settings.playerSetup.culture);
    }

    std::string widthValue = std::to_string(settings.mapWidth);
    std::string heightValue = std::to_string(settings.mapHeight);
    std::string tileValue = std::to_string(settings.tileSize);
    std::string seedValue = std::to_string(settings.seed);
    std::string rulerNameValue = settings.playerSetup.rulerName;
    std::string rulerAgeValue = std::to_string(settings.playerSetup.rulerAge);

    enum class ActiveField
    {
        none,
        width,
        height,
        tile,
        seed,
        ruler_name,
        ruler_age
    };

    ActiveField activeField = ActiveField::none;
    bool running = true;
    int scrollOffset = 0;

    const SDL_Rect widthBoxBase{780, 170, 220, 44};
    const SDL_Rect heightBoxBase{780, 300, 220, 44};
    const SDL_Rect tileBoxBase{780, 430, 220, 44};
    const SDL_Rect randomSeedBoxBase{780, 560, 220, 44};
    const SDL_Rect seedBoxBase{780, 690, 260, 44};

    const SDL_Rect cultureLeftBase{700, 860, 50, 44};
    const SDL_Rect cultureValueBase{760, 860, 300, 44};
    const SDL_Rect cultureRightBase{1070, 860, 50, 44};

    const SDL_Rect regionLeftBase{700, 990, 50, 44};
    const SDL_Rect regionValueBase{760, 990, 300, 44};
    const SDL_Rect regionRightBase{1070, 990, 50, 44};

    const SDL_Rect religionLeftBase{700, 1120, 50, 44};
    const SDL_Rect religionValueBase{760, 1120, 300, 44};
    const SDL_Rect religionRightBase{1070, 1120, 50, 44};

    const SDL_Rect governmentLeftBase{700, 1250, 50, 44};
    const SDL_Rect governmentValueBase{760, 1250, 300, 44};
    const SDL_Rect governmentRightBase{1070, 1250, 50, 44};

    const SDL_Rect rulerNameBoxBase{760, 1380, 360, 44};
    const SDL_Rect rulerAgeBoxBase{760, 1510, 220, 44};

    const SDL_Rect primaryTraitLeftBase{700, 1640, 50, 44};
    const SDL_Rect primaryTraitValueBase{760, 1640, 300, 44};
    const SDL_Rect primaryTraitRightBase{1070, 1640, 50, 44};

    const SDL_Rect secondaryTraitLeftBase{700, 1770, 50, 44};
    const SDL_Rect secondaryTraitValueBase{760, 1770, 300, 44};
    const SDL_Rect secondaryTraitRightBase{1070, 1770, 50, 44};

    const SDL_Rect generateButtonBase{120, 1910, 280, 56};
    const SDL_Rect exitButtonBase{430, 1910, 220, 56};
    const SDL_Rect scrollUpButtonBase{1130, 170, 120, 44};
    const SDL_Rect scrollDownButtonBase{1130, 224, 120, 44};

    const int contentHeight = 2050;
    const int scrollStep = 80;

    SDL_StartTextInput();

    while (running)
    {
        int viewportWidth = 0;
        int viewportHeight = 0;
        SDL_GetRendererOutputSize(renderer, &viewportWidth, &viewportHeight);

        scrollOffset = clampScrollOffset(scrollOffset, viewportHeight, contentHeight);

        const SDL_Rect widthBox = shiftedRect(widthBoxBase, scrollOffset);
        const SDL_Rect heightBox = shiftedRect(heightBoxBase, scrollOffset);
        const SDL_Rect tileBox = shiftedRect(tileBoxBase, scrollOffset);
        const SDL_Rect randomSeedBox = shiftedRect(randomSeedBoxBase, scrollOffset);
        const SDL_Rect seedBox = shiftedRect(seedBoxBase, scrollOffset);

        const SDL_Rect cultureLeft = shiftedRect(cultureLeftBase, scrollOffset);
        const SDL_Rect cultureValue = shiftedRect(cultureValueBase, scrollOffset);
        const SDL_Rect cultureRight = shiftedRect(cultureRightBase, scrollOffset);

        const SDL_Rect regionLeft = shiftedRect(regionLeftBase, scrollOffset);
        const SDL_Rect regionValue = shiftedRect(regionValueBase, scrollOffset);
        const SDL_Rect regionRight = shiftedRect(regionRightBase, scrollOffset);

        const SDL_Rect religionLeft = shiftedRect(religionLeftBase, scrollOffset);
        const SDL_Rect religionValue = shiftedRect(religionValueBase, scrollOffset);
        const SDL_Rect religionRight = shiftedRect(religionRightBase, scrollOffset);

        const SDL_Rect governmentLeft = shiftedRect(governmentLeftBase, scrollOffset);
        const SDL_Rect governmentValue = shiftedRect(governmentValueBase, scrollOffset);
        const SDL_Rect governmentRight = shiftedRect(governmentRightBase, scrollOffset);

        const SDL_Rect rulerNameBox = shiftedRect(rulerNameBoxBase, scrollOffset);
        const SDL_Rect rulerAgeBox = shiftedRect(rulerAgeBoxBase, scrollOffset);

        const SDL_Rect primaryTraitLeft = shiftedRect(primaryTraitLeftBase, scrollOffset);
        const SDL_Rect primaryTraitValue = shiftedRect(primaryTraitValueBase, scrollOffset);
        const SDL_Rect primaryTraitRight = shiftedRect(primaryTraitRightBase, scrollOffset);

        const SDL_Rect secondaryTraitLeft = shiftedRect(secondaryTraitLeftBase, scrollOffset);
        const SDL_Rect secondaryTraitValue = shiftedRect(secondaryTraitValueBase, scrollOffset);
        const SDL_Rect secondaryTraitRight = shiftedRect(secondaryTraitRightBase, scrollOffset);

        const SDL_Rect generateButton = shiftedRect(generateButtonBase, scrollOffset);
        const SDL_Rect exitButton = shiftedRect(exitButtonBase, scrollOffset);
        const SDL_Rect scrollUpButton = scrollOffset > 0 ? shiftedRect(scrollUpButtonBase, 0) : SDL_Rect{0, 0, 0, 0};
        const SDL_Rect scrollDownButton = (contentHeight > viewportHeight && scrollOffset < contentHeight - viewportHeight)
            ? shiftedRect(scrollDownButtonBase, 0)
            : SDL_Rect{0, 0, 0, 0};

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                settings.confirmed = false;
                running = false;
            }

            if (event.type == SDL_MOUSEWHEEL)
            {
                scrollOffset -= event.wheel.y * scrollStep;
                scrollOffset = clampScrollOffset(scrollOffset, viewportHeight, contentHeight);
            }

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                const int mouseX = event.button.x;
                const int mouseY = event.button.y;

                if (pointInRect(mouseX, mouseY, scrollUpButton))
                {
                    scrollOffset -= scrollStep;
                    scrollOffset = clampScrollOffset(scrollOffset, viewportHeight, contentHeight);
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, scrollDownButton))
                {
                    scrollOffset += scrollStep;
                    scrollOffset = clampScrollOffset(scrollOffset, viewportHeight, contentHeight);
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, widthBox))
                {
                    activeField = ActiveField::width;
                }
                else if (pointInRect(mouseX, mouseY, heightBox))
                {
                    activeField = ActiveField::height;
                }
                else if (pointInRect(mouseX, mouseY, tileBox))
                {
                    activeField = ActiveField::tile;
                }
                else if (pointInRect(mouseX, mouseY, randomSeedBox))
                {
                    settings.useRandomSeed = !settings.useRandomSeed;
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, seedBox) && !settings.useRandomSeed)
                {
                    activeField = ActiveField::seed;
                }
                else if (pointInRect(mouseX, mouseY, cultureLeft))
                {
                    settings.playerSetup.culture = cycleValue(kCultures, settings.playerSetup.culture, -1);
                    if (!isAllowedStartingRegion(settings.playerSetup.culture, settings.playerSetup.startRegion))
                    {
                        settings.playerSetup.startRegion = defaultStartingRegion(settings.playerSetup.culture);
                    }
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, cultureRight) || pointInRect(mouseX, mouseY, cultureValue))
                {
                    settings.playerSetup.culture = cycleValue(kCultures, settings.playerSetup.culture, 1);
                    if (!isAllowedStartingRegion(settings.playerSetup.culture, settings.playerSetup.startRegion))
                    {
                        settings.playerSetup.startRegion = defaultStartingRegion(settings.playerSetup.culture);
                    }
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, regionLeft))
                {
                    settings.playerSetup.startRegion = cycleRegion(settings.playerSetup.culture, settings.playerSetup.startRegion, -1);
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, regionRight) || pointInRect(mouseX, mouseY, regionValue))
                {
                    settings.playerSetup.startRegion = cycleRegion(settings.playerSetup.culture, settings.playerSetup.startRegion, 1);
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, religionLeft))
                {
                    settings.playerSetup.religion = cycleValue(kReligions, settings.playerSetup.religion, -1);
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, religionRight) || pointInRect(mouseX, mouseY, religionValue))
                {
                    settings.playerSetup.religion = cycleValue(kReligions, settings.playerSetup.religion, 1);
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, governmentLeft))
                {
                    settings.playerSetup.government = cycleValue(kGovernments, settings.playerSetup.government, -1);
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, governmentRight) || pointInRect(mouseX, mouseY, governmentValue))
                {
                    settings.playerSetup.government = cycleValue(kGovernments, settings.playerSetup.government, 1);
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, rulerNameBox))
                {
                    activeField = ActiveField::ruler_name;
                }
                else if (pointInRect(mouseX, mouseY, rulerAgeBox))
                {
                    activeField = ActiveField::ruler_age;
                }
                else if (pointInRect(mouseX, mouseY, primaryTraitLeft))
                {
                    settings.playerSetup.primaryTrait = cycleValue(kTraits, settings.playerSetup.primaryTrait, -1);
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, primaryTraitRight) || pointInRect(mouseX, mouseY, primaryTraitValue))
                {
                    settings.playerSetup.primaryTrait = cycleValue(kTraits, settings.playerSetup.primaryTrait, 1);
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, secondaryTraitLeft))
                {
                    settings.playerSetup.secondaryTrait = cycleValue(kTraits, settings.playerSetup.secondaryTrait, -1);
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, secondaryTraitRight) || pointInRect(mouseX, mouseY, secondaryTraitValue))
                {
                    settings.playerSetup.secondaryTrait = cycleValue(kTraits, settings.playerSetup.secondaryTrait, 1);
                    activeField = ActiveField::none;
                }
                else if (pointInRect(mouseX, mouseY, generateButton))
                {
                    settings.mapWidth = toClampedInt(widthValue, settings.mapWidth, 64, 1000);
                    settings.mapHeight = toClampedInt(heightValue, settings.mapHeight, 48, 1000);
                    settings.tileSize = toClampedInt(tileValue, settings.tileSize, 2, 64);
                    settings.playerSetup.rulerAge = toClampedInt(rulerAgeValue, settings.playerSetup.rulerAge, 16, 90);

                    if (rulerNameValue.empty())
                    {
                        rulerNameValue = "Ruler";
                    }

                    settings.playerSetup.rulerName = rulerNameValue;

                    if (!isAllowedStartingRegion(settings.playerSetup.culture, settings.playerSetup.startRegion))
                    {
                        settings.playerSetup.startRegion = defaultStartingRegion(settings.playerSetup.culture);
                    }

                    if (settings.useRandomSeed)
                    {
                        settings.seed = static_cast<std::uint32_t>(SDL_GetTicks());
                        seedValue = std::to_string(settings.seed);
                    }
                    else
                    {
                        settings.seed = toSeedValue(seedValue, settings.seed);
                    }

                    settings.confirmed = true;
                    running = false;
                }
                else if (pointInRect(mouseX, mouseY, exitButton))
                {
                    settings.confirmed = false;
                    running = false;
                }
                else
                {
                    activeField = ActiveField::none;
                }
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE)
            {
                if (activeField == ActiveField::width)
                {
                    eraseLastCharacter(widthValue);
                }
                else if (activeField == ActiveField::height)
                {
                    eraseLastCharacter(heightValue);
                }
                else if (activeField == ActiveField::tile)
                {
                    eraseLastCharacter(tileValue);
                }
                else if (activeField == ActiveField::seed)
                {
                    eraseLastCharacter(seedValue);
                }
                else if (activeField == ActiveField::ruler_name)
                {
                    eraseLastCharacter(rulerNameValue);
                }
                else if (activeField == ActiveField::ruler_age)
                {
                    eraseLastCharacter(rulerAgeValue);
                }
            }

            if (event.type == SDL_TEXTINPUT)
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
                else if (activeField == ActiveField::tile && c >= '0' && c <= '9' && tileValue.size() < 2)
                {
                    tileValue += c;
                }
                else if (activeField == ActiveField::seed && c >= '0' && c <= '9' && seedValue.size() < 10)
                {
                    seedValue += c;
                }
                else if (activeField == ActiveField::ruler_age && c >= '0' && c <= '9' && rulerAgeValue.size() < 2)
                {
                    rulerAgeValue += c;
                }
                else if (activeField == ActiveField::ruler_name && rulerNameValue.size() < 28 && isAllowedNameCharacter(c))
                {
                    rulerNameValue += c;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 239, 226, 198, 255);
        SDL_RenderClear(renderer);

        drawText(renderer, titleFont, "County Start Setup", 120, 50 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(
            renderer,
            bodyFont,
            "Set the map values, define your county ruler, choose a culture, then pick one of that culture's allowed starting regions. The exact county tile is assigned randomly from land inside that region when the map is generated.",
            120,
            100 - scrollOffset,
            980,
            SDL_Color{0, 0, 0, 255}
        );

        drawText(renderer, labelFont, "Map Width", 120, 170 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "How many tiles the map spans from left to right.", 120, 205 - scrollOffset, 560, SDL_Color{0, 0, 0, 255});
        drawFieldBox(renderer, widthBox, activeField == ActiveField::width);
        drawText(renderer, labelFont, widthValue.empty() ? "0" : widthValue, widthBox.x + 16, widthBox.y + 8, SDL_Color{0, 0, 0, 255});

        drawText(renderer, labelFont, "Map Height", 120, 300 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "How many tiles the map spans from top to bottom.", 120, 335 - scrollOffset, 560, SDL_Color{0, 0, 0, 255});
        drawFieldBox(renderer, heightBox, activeField == ActiveField::height);
        drawText(renderer, labelFont, heightValue.empty() ? "0" : heightValue, heightBox.x + 16, heightBox.y + 8, SDL_Color{0, 0, 0, 255});

        drawText(renderer, labelFont, "Tile Size", 120, 430 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "How large each tile is in the map viewer window.", 120, 465 - scrollOffset, 560, SDL_Color{0, 0, 0, 255});
        drawFieldBox(renderer, tileBox, activeField == ActiveField::tile);
        drawText(renderer, labelFont, tileValue.empty() ? "0" : tileValue, tileBox.x + 16, tileBox.y + 8, SDL_Color{0, 0, 0, 255});

        drawText(renderer, labelFont, "Random Seed", 120, 560 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "Turn this on to generate a fresh map seed each run. Turn it off to enter a fixed seed.", 120, 595 - scrollOffset, 560, SDL_Color{0, 0, 0, 255});
        drawFieldBox(renderer, randomSeedBox, false);
        drawText(renderer, labelFont, settings.useRandomSeed ? "On" : "Off", randomSeedBox.x + 16, randomSeedBox.y + 8, SDL_Color{0, 0, 0, 255});

        drawText(renderer, labelFont, "Fixed Seed", 120, 690 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "Only used when Random Seed is off.", 120, 725 - scrollOffset, 560, SDL_Color{0, 0, 0, 255});
        drawFieldBox(renderer, seedBox, activeField == ActiveField::seed, !settings.useRandomSeed);
        drawText(renderer, labelFont, seedValue.empty() ? "0" : seedValue, seedBox.x + 16, seedBox.y + 8, SDL_Color{0, 0, 0, 255});

        drawText(renderer, labelFont, "Starting Culture", 120, 860 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "Culture determines which starting regions can be chosen. Titles can transform culture later, but not yet.", 120, 895 - scrollOffset, 560, SDL_Color{0, 0, 0, 255});
        drawButton(renderer, labelFont, cultureLeft, "<");
        drawFieldBox(renderer, cultureValue, false);
        drawText(renderer, labelFont, cultureName(settings.playerSetup.culture), cultureValue.x + 16, cultureValue.y + 8, SDL_Color{0, 0, 0, 255});
        drawButton(renderer, labelFont, cultureRight, ">");

        drawText(renderer, labelFont, "Starting Region", 120, 990 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "The county itself is random inside this region. Only culture-valid regions can be selected.", 120, 1025 - scrollOffset, 560, SDL_Color{0, 0, 0, 255});
        drawButton(renderer, labelFont, regionLeft, "<");
        drawFieldBox(renderer, regionValue, false);
        drawText(renderer, labelFont, regionName(settings.playerSetup.startRegion), regionValue.x + 16, regionValue.y + 8, SDL_Color{0, 0, 0, 255});
        drawButton(renderer, labelFont, regionRight, ">");

        drawText(renderer, labelFont, "Religion", 120, 1120 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "Starting religion tag for the first county polity.", 120, 1155 - scrollOffset, 560, SDL_Color{0, 0, 0, 255});
        drawButton(renderer, labelFont, religionLeft, "<");
        drawFieldBox(renderer, religionValue, false);
        drawText(renderer, labelFont, religionName(settings.playerSetup.religion), religionValue.x + 16, religionValue.y + 8, SDL_Color{0, 0, 0, 255});
        drawButton(renderer, labelFont, religionRight, ">");

        drawText(renderer, labelFont, "Government", 120, 1250 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "Starting government form for the player county.", 120, 1285 - scrollOffset, 560, SDL_Color{0, 0, 0, 255});
        drawButton(renderer, labelFont, governmentLeft, "<");
        drawFieldBox(renderer, governmentValue, false);
        drawText(renderer, labelFont, governmentName(settings.playerSetup.government), governmentValue.x + 16, governmentValue.y + 8, SDL_Color{0, 0, 0, 255});
        drawButton(renderer, labelFont, governmentRight, ">");

        drawText(renderer, labelFont, "Ruler Name", 120, 1380 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "Type the starting ruler's name.", 120, 1415 - scrollOffset, 560, SDL_Color{0, 0, 0, 255});
        drawFieldBox(renderer, rulerNameBox, activeField == ActiveField::ruler_name);
        drawText(renderer, labelFont, rulerNameValue.empty() ? "Ruler" : rulerNameValue, rulerNameBox.x + 16, rulerNameBox.y + 8, SDL_Color{0, 0, 0, 255});

        drawText(renderer, labelFont, "Ruler Age", 120, 1510 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "Age matters because rulers can die later. This setup only stores the starting value for now.", 120, 1545 - scrollOffset, 560, SDL_Color{0, 0, 0, 255});
        drawFieldBox(renderer, rulerAgeBox, activeField == ActiveField::ruler_age);
        drawText(renderer, labelFont, rulerAgeValue.empty() ? "0" : rulerAgeValue, rulerAgeBox.x + 16, rulerAgeBox.y + 8, SDL_Color{0, 0, 0, 255});

        drawText(renderer, labelFont, "Primary Trait", 120, 1640 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "Traits are placeholder ruler tags for now so the start flow is wired before the deeper effects exist.", 120, 1675 - scrollOffset, 560, SDL_Color{0, 0, 0, 255});
        drawButton(renderer, labelFont, primaryTraitLeft, "<");
        drawFieldBox(renderer, primaryTraitValue, false);
        drawText(renderer, labelFont, rulerTraitName(settings.playerSetup.primaryTrait), primaryTraitValue.x + 16, primaryTraitValue.y + 8, SDL_Color{0, 0, 0, 255});
        drawButton(renderer, labelFont, primaryTraitRight, ">");

        drawText(renderer, labelFont, "Secondary Trait", 120, 1770 - scrollOffset, SDL_Color{0, 0, 0, 255});
        drawWrappedText(renderer, bodyFont, "Second placeholder ruler tag. This can be expanded into your real throughput, production method, and unit hooks later.", 120, 1805 - scrollOffset, 560, SDL_Color{0, 0, 0, 255});
        drawButton(renderer, labelFont, secondaryTraitLeft, "<");
        drawFieldBox(renderer, secondaryTraitValue, false);
        drawText(renderer, labelFont, rulerTraitName(settings.playerSetup.secondaryTrait), secondaryTraitValue.x + 16, secondaryTraitValue.y + 8, SDL_Color{0, 0, 0, 255});
        drawButton(renderer, labelFont, secondaryTraitRight, ">");

        drawButton(renderer, labelFont, generateButton, "Generate County Start");
        drawButton(renderer, labelFont, exitButton, "Exit");

        if (scrollOffset > 0)
        {
            drawButton(renderer, bodyFont, scrollUpButton, "Scroll Up");
        }

        if (contentHeight > viewportHeight && scrollOffset < contentHeight - viewportHeight)
        {
            drawButton(renderer, bodyFont, scrollDownButton, "Scroll Down");
        }

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