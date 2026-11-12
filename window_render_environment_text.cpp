// File: window_render_environment_text.cpp
// Commit: Rename available sub-building environment text to available buildings

namespace WindowRender
{
    namespace
    {
        std::string environmentSummaryText(const Map& map, int selectedProvinceId)
        {
            const ProvinceEnvironmentSummary summary =
                summarizeProvinceEnvironment(map, selectedProvinceId);

            std::string text =
                "Environment\n"
                "Province: " +
                std::to_string(summary.provinceId) +
                "\nPhysical Tiles: " +
                std::to_string(summary.totalTiles) +
                "\nLand Tiles: " +
                std::to_string(summary.landTiles);

            text += "\n\nTerrain Counts:";

            if (summary.terrainCounts.empty())
            {
                text += "\nNone";
            }
            else
            {
                for (const ProvinceTerrainCount& count : summary.terrainCounts)
                {
                    text +=
                        "\n" +
                        std::string(terrainDisplayName(count.terrain)) +
                        ": " +
                        std::to_string(count.tileCount);
                }
            }

            text += "\n\nEcology Values:";

            if (summary.ecologyCapacities.empty())
            {
                text += "\nNone";
            }
            else
            {
                for (const ProvinceEcologyCapacity& capacity : summary.ecologyCapacities)
                {
                    text +=
                        "\n" +
                        std::string(provinceEcologyTypeName(capacity.type)) +
                        ": " +
                        formatEcologyValue(capacity.usedValue) +
                        "/" +
                        formatEcologyValue(capacity.totalValue);
                }
            }

            text += "\n\nAvailable Resources:";

            if (summary.availableResources.empty())
            {
                text += "\nNone";
            }
            else
            {
                for (ResourceType resourceType : summary.availableResources)
                {
                    text +=
                        "\n" +
                        std::string(resourceTypeName(resourceType));
                }
            }

            text += "\n\nAvailable Buildings:";

            if (summary.availableBuildings.empty())
            {
                text += "\nNone";
            }
            else
            {
                for (BuildingType buildingType : summary.availableBuildings)
                {
                    text +=
                        "\n" +
                        std::string(buildingTypeName(buildingType));
                }
            }

            text += "\n\nRegions:";

            if (summary.regionCounts.empty())
            {
                text += "\nNone";
            }
            else
            {
                for (const ProvinceRegionCount& count : summary.regionCounts)
                {
                    text +=
                        "\n" +
                        std::string(regionName(count.region)) +
                        ": " +
                        std::to_string(count.tileCount);
                }
            }

            return text;
        }
    }
}