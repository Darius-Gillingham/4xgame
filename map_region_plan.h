// File: map_region_plan.h
// Commit: Add shared region plan declarations for continent-split region generation

#pragma once

#include "map.h"

#include <array>
#include <vector>

struct RegionPlan
{
    Region region = Region::none;
    bool isBritain = false;

    int xPercent = 50;
    int yPercent = 50;
    int weight = 1;

    int stretchX = 10;
    int stretchY = 10;
    int driftX = 0;
    int driftY = 0;

    std::array<Region, 8> neighbors{
        Region::none,
        Region::none,
        Region::none,
        Region::none,
        Region::none,
        Region::none,
        Region::none,
        Region::none
    };

    int neighborCount = 0;
};

const std::vector<RegionPlan>& europeRegionPlans();
const std::vector<RegionPlan>& asiaRegionPlans();
const std::vector<RegionPlan>& africaRegionPlans();
const std::vector<RegionPlan>& americasRegionPlans();
const std::vector<RegionPlan>& oceaniaRegionPlans();

const std::vector<RegionPlan>& allRegionPlans();

int regionPlanCount();
int regionPlanIndex(Region region);
const RegionPlan& regionPlan(Region region);

int totalRegionWeight();
int desiredTileCount(const Map& map, Region region);
int minTileCount(const Map& map, Region region);
int maxTileCount(const Map& map, Region region);

bool regionsAreAdjacentInRules(Region a, Region b);