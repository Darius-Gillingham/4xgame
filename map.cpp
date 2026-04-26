// File: map.cpp
// Commit: Add land tile region queries and polity ownership helpers to the core map implementation

#include "map.h"
#include "map_internal.h"

Map::Map(int width, int height, std::uint32_t seed)
    : width_(width), height_(height), seed_(seed), tiles_(static_cast<size_t>(width * height))
{
}

int Map::width() const
{
    return width_;
}

int Map::height() const
{
    return height_;
}

std::uint32_t Map::seed() const
{
    return seed_;
}

const Tile& Map::at(int x, int y) const
{
    return tiles_[static_cast<size_t>(index(x, y))];
}

std::vector<MapPosition> Map::landTilesInRegion(Region region) const
{
    std::vector<MapPosition> positions;

    for (int y = 0; y < height_; ++y)
    {
        for (int x = 0; x < width_; ++x)
        {
            const Tile& current = at(x, y);
            if (current.land && current.region == region)
            {
                positions.push_back(MapPosition{x, y});
            }
        }
    }

    return positions;
}

void Map::clearPolityOwnership()
{
    for (Tile& current : tiles_)
    {
        current.polityId = -1;
    }
}

void Map::setPolityOwner(int x, int y, int polityId)
{
    if (!inBounds(x, y))
    {
        return;
    }

    tile(x, y).polityId = polityId;
}

int Map::index(int x, int y) const
{
    return y * width_ + x;
}

bool Map::inBounds(int x, int y) const
{
    return x >= 0 && y >= 0 && x < width_ && y < height_;
}

Tile& Map::tile(int x, int y)
{
    return tiles_[static_cast<size_t>(index(x, y))];
}

void Map::generate()
{
    generateLand();
    MapState state = buildRegionLayout(*this, seed_);
    applyLandmask(*this, state);
    assignTerrainClusters(*this, seed_);
    smoothTerrainPasses(*this);
}

void Map::generateLand()
{
    for (Tile& current : tiles_)
    {
        current.land = false;
        current.region = Region::none;
        current.terrain = Terrain::ocean;
        current.polityId = -1;
    }
}