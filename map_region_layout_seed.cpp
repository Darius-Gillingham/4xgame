// File: map_region_layout_seed.cpp
// Commit: Move region seed creation into dedicated layout seed file

#include "map_region_layout_internal.h"

#include <algorithm>
#include <random>
#include <vector>

std::vector<RegionSeed> buildRegionSeeds(int width, int height, std::mt19937& rng)
{
    std::vector<RegionSeed> seeds;
    seeds.reserve(regionPlanCount());

    const RegionPlanBounds bounds = calculateRegionPlanBounds();

    for (const RegionPlan& plan : allRegionPlans())
    {
        const int baseX = normalizeRegionPlanCoordinate(plan.xPercent, bounds.minX, bounds.maxX, width);
        const int baseY = normalizeRegionPlanCoordinate(plan.yPercent, bounds.minY, bounds.maxY, height);
        const int jitterX = std::max(1, width / 80);
        const int jitterY = std::max(1, height / 80);

        RegionSeed seed{};
        seed.region = plan.region;
        seed.isBritain = plan.isBritain;
        seed.x = std::clamp(baseX + randInt(rng, -jitterX, jitterX), 2, std::max(2, width - 3));
        seed.y = std::clamp(baseY + randInt(rng, -jitterY, jitterY), 2, std::max(2, height - 3));

        seeds.push_back(seed);
    }

    for (int pass = 0; pass < 24; ++pass)
    {
        for (size_t i = 0; i < seeds.size(); ++i)
        {
            for (size_t j = i + 1; j < seeds.size(); ++j)
            {
                if (distanceSq(seeds[i].x, seeds[i].y, seeds[j].x, seeds[j].y) >= 64)
                {
                    continue;
                }

                const int pushX = (seeds[j].x >= seeds[i].x) ? 1 : -1;
                const int pushY = (seeds[j].y >= seeds[i].y) ? 1 : -1;

                seeds[j].x = std::clamp(seeds[j].x + pushX, 2, std::max(2, width - 3));
                seeds[j].y = std::clamp(seeds[j].y + pushY, 2, std::max(2, height - 3));
            }
        }
    }

    return seeds;
}
