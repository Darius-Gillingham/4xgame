// File: window_render_selection.cpp
// Commit: Move province color and player province selection helpers

namespace WindowRender
{
    Color provinceColor(
        const Tile& tile)
    {
        if (!tile.land)
        {
            return {18, 46, 110};
        }

        if (tile.provinceId < 0)
        {
            return {205, 193, 165};
        }

        const int value = tile.provinceId * 37;
        const unsigned char r = static_cast<unsigned char>(110 + (value % 96));
        const unsigned char g = static_cast<unsigned char>(110 + ((value * 3) % 96));
        const unsigned char b = static_cast<unsigned char>(110 + ((value * 7) % 96));

        return {r, g, b};
    }


    int playerProvinceId(
        const Map& map,
        const GameState& gameState)
    {
        const int x = gameState.playerCounty.countyX;
        const int y = gameState.playerCounty.countyY;

        if (x < 0 || y < 0 || x >= map.width() || y >= map.height())
        {
            return -1;
        }

        return map.at(x, y).provinceId;
    }


    bool isPlayerProvinceTile(
        const Map& map,
        const GameState& gameState,
        int tileX,
        int tileY)
    {
        if (tileX < 0 || tileY < 0 || tileX >= map.width() || tileY >= map.height())
        {
            return false;
        }

        const int playerProvince = playerProvinceId(map, gameState);

        return playerProvince >= 0 && map.at(tileX, tileY).provinceId == playerProvince;
    }

}
