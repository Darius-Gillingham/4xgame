// File: map.h
// Commit: Add large Americas and Oceania regions to world region enum

#pragma once

#include <cstdint>
#include <vector>

enum class Terrain
{
    tundra,
    boreal,
    woods,
    jungle,
    jungle_highlands,
    highlands,
    hills,
    mountains,
    meadow,
    plains,
    marsh,
    floodplain,
    steppe,
    savanna,
    arid,
    desert,
    oasis,
    farmlands,
    ocean
};

enum class Region
{
    none,
    scandinavia,
    france,
    british_isles,
    italy,
    south_germany,
    lowlands,
    north_germany,
    poland,
    baltic,
    russia,
    pontic_steppe,
    danubia,
    balkans,
    iberia,
    anatolia,
    levant,
    mesopotamia,
    arabia,
    caucasus,
    persia,
    transoxiana,
    kazakhstan_steppe,
    tarim_basin,
    mongolia,
    northwest_india,
    gangetic_plain,
    deccan,
    bengal,
    sri_lanka,
    north_china,
    central_china,
    south_china,
    manchuria,
    korea,
    japan,
    indochina,
    mainland_southeast_asia,
    malay_peninsula,
    indonesia,
    philippines,
    egypt,
    libya,
    maghreb,
    nubia,
    horn_of_africa,
    ethiopian_highlands,
    swahili_coast,
    sahel,
    upper_niger,
    guinea_coast,
    congo_basin,
    southern_africa,
    arctic_america,
    western_canada,
    eastern_canada,
    pacific_northwest,
    great_plains,
    great_lakes,
    northeast_america,
    california,
    southwest_america,
    southeast_america,
    mexico,
    central_america,
    caribbean,
    andes,
    amazon,
    brazil,
    southern_cone,
    patagonia,
    australia,
    new_guinea,
    new_zealand,
    pacific_islands
};

struct MapPosition
{
    int x = 0;
    int y = 0;
};

struct Tile
{
    bool land = false;
    Terrain terrain = Terrain::ocean;
    Region region = Region::none;
    int polityId = -1;
    int provinceId = -1;
};

struct MapState;

class Map
{
public:
    Map(int width, int height, std::uint32_t seed);
    void generate();

    int width() const;
    int height() const;
    std::uint32_t seed() const;
    const Tile& at(int x, int y) const;
    bool setTile(int x, int y, const Tile& value);
    std::vector<MapPosition> landTilesInRegion(Region region) const;
    void clearPolityOwnership();
    void setPolityOwner(int x, int y, int polityId);

private:
    friend MapState buildRegionLayout(const Map& map, std::uint32_t seed);
    friend void applyLandmask(Map& map, MapState& state);
    friend void assignTerrainClusters(Map& map, std::uint32_t seed);
    friend void smoothTerrainPasses(Map& map);

    int width_;
    int height_;
    std::uint32_t seed_;
    std::vector<Tile> tiles_;

    int index(int x, int y) const;
    bool inBounds(int x, int y) const;
    Tile& tile(int x, int y);

    void generateLand();
};