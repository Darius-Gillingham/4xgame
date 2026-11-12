// File: window_render_helpers.cpp
// Commit: Move private province and terrain helper functions out of window_render.cpp

namespace WindowRender
{
    namespace
    {
        const int kTopResourceBarHeight = 42;
        const int kShellPad = 14;
        const int kButtonHeight = 32;
        const int kButtonGap = 8;

        bool isSelectedProvinceOwnedByPlayer(
            const GameState& gameState,
            int selectedProvinceId)
        {
            const ProvinceSlot* provinceSlot = provinceSlotById(gameState, selectedProvinceId);

            if (!provinceSlot)
            {
                return false;
            }

            return provinceSlot->countryId == gameState.playerCounty.countryId;
        }

        const Country* countryForProvince(
            const GameState& gameState,
            int selectedProvinceId)
        {
            const ProvinceSlot* provinceSlot = provinceSlotById(gameState, selectedProvinceId);

            if (!provinceSlot)
            {
                return nullptr;
            }

            return countryById(gameState, provinceSlot->countryId);
        }

        const char* terrainDisplayName(Terrain terrain)
        {
            switch (terrain)
            {
                case Terrain::tundra: return "Tundra";
                case Terrain::boreal: return "Boreal";
                case Terrain::woods: return "Woods";
                case Terrain::jungle: return "Jungle";
                case Terrain::jungle_highlands: return "Jungle Highlands";
                case Terrain::highlands: return "Highlands";
                case Terrain::hills: return "Hills";
                case Terrain::mountains: return "Mountains";
                case Terrain::meadow: return "Meadow";
                case Terrain::plains: return "Plains";
                case Terrain::marsh: return "Marsh";
                case Terrain::floodplain: return "Floodplain";
                case Terrain::steppe: return "Steppe";
                case Terrain::savanna: return "Savanna";
                case Terrain::arid: return "Arid";
                case Terrain::desert: return "Desert";
                case Terrain::oasis: return "Oasis";
                case Terrain::farmlands: return "Farmlands";
                case Terrain::ocean: return "Ocean";
                default: return "Unknown Terrain";
            }
        }

        std::string formatEcologyValue(float value)
        {
            std::ostringstream stream;
            stream << std::fixed << std::setprecision(2) << value;
            return stream.str();
        }

        SDL_Rect centeredRect(int screenWidth, int screenHeight, int width, int height)
        {
            return SDL_Rect{
                (screenWidth - width) / 2,
                (screenHeight - height) / 2,
                width,
                height
            };
        }
    }
}
