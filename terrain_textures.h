// File: terrain_textures.h
// Commit: Declare terrain PNG texture loading and lookup

#pragma once

#include "map.h"

#include <SDL2/SDL.h>

namespace TerrainTextures
{
    bool load(
        SDL_Renderer* renderer
    );

    SDL_Texture* texture(
        Terrain terrain
    );

    void unload();
}
