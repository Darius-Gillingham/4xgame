// File: map_province_environment.h
// Commit: Rename available sub-buildings to available buildings for the holding/building refactor

#pragma once

#include "building/building.h"
#include "map.h"

#include <vector>

enum class ProvinceEcologyType
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
    farmlands
};

struct ProvinceEcologyCapacity
{
    ProvinceEcologyType type = ProvinceEcologyType::meadow;
    float totalValue = 0.0f;
    float usedValue = 0.0f;
};

struct ProvinceTerrainCount
{
    Terrain terrain = Terrain::ocean;
    int tileCount = 0;
};

struct ProvinceRegionCount
{
    Region region = Region::scandinavia;
    int tileCount = 0;
};

struct ProvinceEnvironmentSummary
{
    int provinceId = -1;
    int totalTiles = 0;
    int landTiles = 0;
    std::vector<ProvinceTerrainCount> terrainCounts;
    std::vector<ProvinceRegionCount> regionCounts;
    std::vector<ProvinceEcologyCapacity> ecologyCapacities;
    std::vector<ResourceType> availableResources;
    std::vector<BuildingType> availableBuildings;
};

const char* provinceEcologyTypeName(ProvinceEcologyType type);
ProvinceEcologyType provinceEcologyTypeForTerrain(Terrain terrain);
float provinceEcologyValueForTerrain(Terrain terrain);

ProvinceEnvironmentSummary summarizeProvinceEnvironment(
    const Map& map,
    int provinceId
);