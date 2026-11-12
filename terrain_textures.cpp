// File: terrain_textures.cpp
// Commit: Load, provide, and release PNG textures for every terrain type

#include "terrain_textures.h"

#include <SDL2/SDL_image.h>

namespace TerrainTextures
{
    namespace
    {
        SDL_Texture* tundraTexture = nullptr;
        SDL_Texture* borealTexture = nullptr;
        SDL_Texture* woodsTexture = nullptr;
        SDL_Texture* jungleTexture = nullptr;
        SDL_Texture* jungleHighlandsTexture = nullptr;
        SDL_Texture* highlandsTexture = nullptr;
        SDL_Texture* hillsTexture = nullptr;
        SDL_Texture* mountainsTexture = nullptr;
        SDL_Texture* meadowTexture = nullptr;
        SDL_Texture* plainsTexture = nullptr;
        SDL_Texture* marshTexture = nullptr;
        SDL_Texture* floodplainTexture = nullptr;
        SDL_Texture* steppeTexture = nullptr;
        SDL_Texture* savannaTexture = nullptr;
        SDL_Texture* aridTexture = nullptr;
        SDL_Texture* desertTexture = nullptr;
        SDL_Texture* oasisTexture = nullptr;
        SDL_Texture* farmlandsTexture = nullptr;
        SDL_Texture* oceanTexture = nullptr;

        SDL_Texture* loadTexture(
            SDL_Renderer* renderer,
            const char* filePath)
        {
            return IMG_LoadTexture(
                renderer,
                filePath
            );
        }
    }

    bool load(
        SDL_Renderer* renderer)
    {
        if (!renderer)
        {
            return false;
        }

        unload();

        tundraTexture =
            loadTexture(
                renderer,
                "assets/tundra.png"
            );

        borealTexture =
            loadTexture(
                renderer,
                "assets/boreal.png"
            );

        woodsTexture =
            loadTexture(
                renderer,
                "assets/woods.png"
            );

        jungleTexture =
            loadTexture(
                renderer,
                "assets/jungle.png"
            );

        jungleHighlandsTexture =
            loadTexture(
                renderer,
                "assets/jungle_highlands.png"
            );

        highlandsTexture =
            loadTexture(
                renderer,
                "assets/highlands.png"
            );

        hillsTexture =
            loadTexture(
                renderer,
                "assets/hills.png"
            );

        mountainsTexture =
            loadTexture(
                renderer,
                "assets/mountains.png"
            );

        meadowTexture =
            loadTexture(
                renderer,
                "assets/meadow.png"
            );

        plainsTexture =
            loadTexture(
                renderer,
                "assets/plains.png"
            );

        marshTexture =
            loadTexture(
                renderer,
                "assets/marsh.png"
            );

        floodplainTexture =
            loadTexture(
                renderer,
                "assets/floodplain.png"
            );

        steppeTexture =
            loadTexture(
                renderer,
                "assets/steppe.png"
            );

        savannaTexture =
            loadTexture(
                renderer,
                "assets/savanna.png"
            );

        aridTexture =
            loadTexture(
                renderer,
                "assets/arid.png"
            );

        desertTexture =
            loadTexture(
                renderer,
                "assets/desert.png"
            );

        oasisTexture =
            loadTexture(
                renderer,
                "assets/oasis.png"
            );

        farmlandsTexture =
            loadTexture(
                renderer,
                "assets/farmlands.png"
            );

        oceanTexture =
            loadTexture(
                renderer,
                "assets/ocean.png"
            );

        const bool loadedAllTextures =
            tundraTexture &&
            borealTexture &&
            woodsTexture &&
            jungleTexture &&
            jungleHighlandsTexture &&
            highlandsTexture &&
            hillsTexture &&
            mountainsTexture &&
            meadowTexture &&
            plainsTexture &&
            marshTexture &&
            floodplainTexture &&
            steppeTexture &&
            savannaTexture &&
            aridTexture &&
            desertTexture &&
            oasisTexture &&
            farmlandsTexture &&
            oceanTexture;

        if (!loadedAllTextures)
        {
            unload();
            return false;
        }

        return true;
    }

    SDL_Texture* texture(
        Terrain terrain)
    {
        switch (terrain)
        {
            case Terrain::tundra:
                return tundraTexture;

            case Terrain::boreal:
                return borealTexture;

            case Terrain::woods:
                return woodsTexture;

            case Terrain::jungle:
                return jungleTexture;

            case Terrain::jungle_highlands:
                return jungleHighlandsTexture;

            case Terrain::highlands:
                return highlandsTexture;

            case Terrain::hills:
                return hillsTexture;

            case Terrain::mountains:
                return mountainsTexture;

            case Terrain::meadow:
                return meadowTexture;

            case Terrain::plains:
                return plainsTexture;

            case Terrain::marsh:
                return marshTexture;

            case Terrain::floodplain:
                return floodplainTexture;

            case Terrain::steppe:
                return steppeTexture;

            case Terrain::savanna:
                return savannaTexture;

            case Terrain::arid:
                return aridTexture;

            case Terrain::desert:
                return desertTexture;

            case Terrain::oasis:
                return oasisTexture;

            case Terrain::farmlands:
                return farmlandsTexture;

            case Terrain::ocean:
                return oceanTexture;

            default:
                return oceanTexture;
        }
    }

    void unload()
    {
        SDL_DestroyTexture(tundraTexture);
        SDL_DestroyTexture(borealTexture);
        SDL_DestroyTexture(woodsTexture);
        SDL_DestroyTexture(jungleTexture);
        SDL_DestroyTexture(jungleHighlandsTexture);
        SDL_DestroyTexture(highlandsTexture);
        SDL_DestroyTexture(hillsTexture);
        SDL_DestroyTexture(mountainsTexture);
        SDL_DestroyTexture(meadowTexture);
        SDL_DestroyTexture(plainsTexture);
        SDL_DestroyTexture(marshTexture);
        SDL_DestroyTexture(floodplainTexture);
        SDL_DestroyTexture(steppeTexture);
        SDL_DestroyTexture(savannaTexture);
        SDL_DestroyTexture(aridTexture);
        SDL_DestroyTexture(desertTexture);
        SDL_DestroyTexture(oasisTexture);
        SDL_DestroyTexture(farmlandsTexture);
        SDL_DestroyTexture(oceanTexture);

        tundraTexture = nullptr;
        borealTexture = nullptr;
        woodsTexture = nullptr;
        jungleTexture = nullptr;
        jungleHighlandsTexture = nullptr;
        highlandsTexture = nullptr;
        hillsTexture = nullptr;
        mountainsTexture = nullptr;
        meadowTexture = nullptr;
        plainsTexture = nullptr;
        marshTexture = nullptr;
        floodplainTexture = nullptr;
        steppeTexture = nullptr;
        savannaTexture = nullptr;
        aridTexture = nullptr;
        desertTexture = nullptr;
        oasisTexture = nullptr;
        farmlandsTexture = nullptr;
        oceanTexture = nullptr;
    }
}