// File: map.h
// Commit: Add tile polity ownership and public region tile queries for county spawning

#pragma once
#include <cstdint>
#include <vector>

enum class Terrain
{
    tundra, boreal, woods, highlands, hills, mountains,
    plains, farmlands, marsh, steppe, arid, desert, ocean
};

enum class Region
{
    none, scandinavia, france, british_isles, italy,
    south_germany, lowlands, north_germany, poland, baltic,
    russia, pontic_steppe, danubia, balkans, iberia, anatolia
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