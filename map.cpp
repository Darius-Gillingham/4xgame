// File: map.cpp
// Commit: Add horizontal map wrapping and reuse province generation buffers

#include "map.h"
#include "map_internal.h"
#include "map_region_plan.h"

#include <algorithm>
#include <queue>
#include <random>
#include <vector>

namespace
{
    constexpr int kMinProvinceTiles = 6;
    constexpr int kMaxProvinceTiles = 18;
    constexpr int kTargetProvinceTiles = 12;

    int wrappedMapX(int x, int width)
    {
        if (width <= 0)
        {
            return 0;
        }

        int wrapped = x % width;

        if (wrapped < 0)
        {
            wrapped += width;
        }

        return wrapped;
    }

    bool validMapY(const Map& map, int y)
    {
        return y >= 0 && y < map.height();
    }

    bool sameProvinceSourceTile(const Tile& tile, Region region)
    {
        return tile.land && tile.region == region && tile.provinceId < 0;
    }

    int provinceTargetSize(int remainingTiles)
    {
        if (remainingTiles <= kMaxProvinceTiles)
        {
            return remainingTiles;
        }

        const int remainingAfterTarget = remainingTiles - kTargetProvinceTiles;

        if (remainingAfterTarget > 0 && remainingAfterTarget < kMinProvinceTiles)
        {
            return std::clamp(kTargetProvinceTiles - (kMinProvinceTiles - remainingAfterTarget), kMinProvinceTiles, kMaxProvinceTiles);
        }

        return kTargetProvinceTiles;
    }

    int rawMapIndex(const Map& map, int x, int y)
    {
        return y * map.width() + x;
    }

    bool rawInBounds(const Map& map, int x, int y)
    {
        return x >= 0 && y >= 0 && x < map.width() && y < map.height();
    }

    bool findNextUnassignedPosition(
        Map& map,
        const std::vector<MapPosition>& positions,
        size_t& searchStart,
        MapPosition& position)
    {
        while (searchStart < positions.size())
        {
            const MapPosition candidate = positions[searchStart];
            ++searchStart;

            if (sameProvinceSourceTile(map.at(candidate.x, candidate.y), map.at(candidate.x, candidate.y).region))
            {
                position = candidate;
                return true;
            }
        }

        return false;
    }

    std::vector<MapPosition> collectUnassignedComponent(
        Map& map,
        Region region,
        int startX,
        int startY,
        std::vector<int>& visited,
        int& visitedStamp)
    {
        std::vector<MapPosition> component;
        std::queue<MapPosition> frontier;

        ++visitedStamp;

        if (visitedStamp == 0)
        {
            std::fill(visited.begin(), visited.end(), 0);
            visitedStamp = 1;
        }

        frontier.push(MapPosition{startX, startY});
        visited[static_cast<size_t>(rawMapIndex(map, startX, startY))] = visitedStamp;

        while (!frontier.empty())
        {
            const MapPosition current = frontier.front();
            frontier.pop();

            const Tile& currentTile = map.at(current.x, current.y);
            if (!sameProvinceSourceTile(currentTile, region))
            {
                continue;
            }

            component.push_back(current);

            for (const auto& direction : kDirs)
            {
                const int nextX = current.x + direction[0];
                const int nextY = current.y + direction[1];

                if (!rawInBounds(map, nextX, nextY))
                {
                    continue;
                }

                const int nextIndex = rawMapIndex(map, nextX, nextY);
                if (visited[static_cast<size_t>(nextIndex)] == visitedStamp)
                {
                    continue;
                }

                visited[static_cast<size_t>(nextIndex)] = visitedStamp;

                const Tile& nextTile = map.at(nextX, nextY);
                if (sameProvinceSourceTile(nextTile, region))
                {
                    frontier.push(MapPosition{nextX, nextY});
                }
            }
        }

        return component;
    }

    MapPosition pickComponentSeed(const std::vector<MapPosition>& component, std::mt19937& rng)
    {
        const int index = std::uniform_int_distribution<int>(0, static_cast<int>(component.size()) - 1)(rng);
        return component[static_cast<size_t>(index)];
    }

    std::vector<MapPosition> carveProvinceTiles(
        Map& map,
        Region region,
        const MapPosition& seed,
        int targetSize,
        std::vector<int>& visited,
        int& visitedStamp)
    {
        std::vector<MapPosition> provinceTiles;
        std::queue<MapPosition> frontier;

        ++visitedStamp;

        if (visitedStamp == 0)
        {
            std::fill(visited.begin(), visited.end(), 0);
            visitedStamp = 1;
        }

        frontier.push(seed);
        visited[static_cast<size_t>(rawMapIndex(map, seed.x, seed.y))] = visitedStamp;

        while (!frontier.empty() && static_cast<int>(provinceTiles.size()) < targetSize)
        {
            const MapPosition current = frontier.front();
            frontier.pop();

            const Tile& currentTile = map.at(current.x, current.y);
            if (!sameProvinceSourceTile(currentTile, region))
            {
                continue;
            }

            provinceTiles.push_back(current);

            for (const auto& direction : kDirs)
            {
                const int nextX = current.x + direction[0];
                const int nextY = current.y + direction[1];

                if (!rawInBounds(map, nextX, nextY))
                {
                    continue;
                }

                const int nextIndex = rawMapIndex(map, nextX, nextY);
                if (visited[static_cast<size_t>(nextIndex)] == visitedStamp)
                {
                    continue;
                }

                visited[static_cast<size_t>(nextIndex)] = visitedStamp;

                const Tile& nextTile = map.at(nextX, nextY);
                if (sameProvinceSourceTile(nextTile, region))
                {
                    frontier.push(MapPosition{nextX, nextY});
                }
            }
        }

        return provinceTiles;
    }

    void assignProvinceToTiles(Map& map, const std::vector<MapPosition>& positions, int provinceId)
    {
        for (const MapPosition& position : positions)
        {
            Tile tile = map.at(position.x, position.y);
            tile.provinceId = provinceId;
            map.setTile(position.x, position.y, tile);
        }
    }

    void generateProvinceIds(Map& map, std::uint32_t seed)
    {
        int nextProvinceId = 1;
        std::mt19937 rng(seed + 9001u);

        std::vector<std::vector<MapPosition>> regionPositions(static_cast<size_t>(regionPlanCount()));
        std::vector<int> visited(static_cast<size_t>(map.width() * map.height()), 0);
        int visitedStamp = 0;

        for (int y = 0; y < map.height(); ++y)
        {
            for (int x = 0; x < map.width(); ++x)
            {
                const Tile& tile = map.at(x, y);

                if (!tile.land)
                {
                    continue;
                }

                const int regionIndex = regionPlanIndex(tile.region);

                if (regionIndex >= 0)
                {
                    regionPositions[static_cast<size_t>(regionIndex)].push_back(MapPosition{x, y});
                }
            }
        }

        for (const RegionPlan& plan : allRegionPlans())
        {
            const Region region = plan.region;
            const int regionIndex = regionPlanIndex(region);

            if (regionIndex < 0)
            {
                continue;
            }

            std::vector<MapPosition>& positions = regionPositions[static_cast<size_t>(regionIndex)];
            size_t searchStart = 0;
            MapPosition start;

            while (findNextUnassignedPosition(map, positions, searchStart, start))
            {
                const std::vector<MapPosition> component = collectUnassignedComponent(
                    map,
                    region,
                    start.x,
                    start.y,
                    visited,
                    visitedStamp
                );

                if (component.empty())
                {
                    continue;
                }

                const int targetSize = provinceTargetSize(static_cast<int>(component.size()));
                const MapPosition provinceSeed = pickComponentSeed(component, rng);

                std::vector<MapPosition> provinceTiles = carveProvinceTiles(
                    map,
                    region,
                    provinceSeed,
                    targetSize,
                    visited,
                    visitedStamp
                );

                if (provinceTiles.empty())
                {
                    provinceTiles = component;
                }

                assignProvinceToTiles(map, provinceTiles, nextProvinceId);
                ++nextProvinceId;
            }
        }
    }
}

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
    static const Tile fallbackTile{};

    if (!validMapY(*this, y) || width_ <= 0)
    {
        return fallbackTile;
    }

    return tiles_[static_cast<size_t>(index(wrappedMapX(x, width_), y))];
}

bool Map::setTile(int x, int y, const Tile& value)
{
    if (!validMapY(*this, y) || width_ <= 0)
    {
        return false;
    }

    tile(wrappedMapX(x, width_), y) = value;
    return true;
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
    if (!validMapY(*this, y) || width_ <= 0)
    {
        return;
    }

    tile(wrappedMapX(x, width_), y).polityId = polityId;
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
    generateProvinceIds(*this, seed_);
}

void Map::generateLand()
{
    for (Tile& current : tiles_)
    {
        current.land = false;
        current.region = Region::none;
        current.terrain = Terrain::ocean;
        current.polityId = -1;
        current.provinceId = -1;
    }
}