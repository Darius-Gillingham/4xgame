// File: window_render_primitives.cpp
// Commit: Move basic SDL text, panel, button, and hit-test rendering helpers

namespace WindowRender
{
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

        SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), color, 520);
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

    void drawPanel(
        SDL_Renderer* renderer,
        const SDL_Rect& rect)
    {
        SDL_SetRenderDrawColor(renderer, 239, 226, 198, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }

    void drawButton(
        SDL_Renderer* renderer,
        TTF_Font* font,
        const SDL_Rect& rect,
        const std::string& label,
        bool active)
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

    bool pointInRect(
        int x,
        int y,
        const SDL_Rect& rect)
    {
        return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
    }
}
