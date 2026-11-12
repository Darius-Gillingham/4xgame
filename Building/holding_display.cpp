// File: building/holding_display.cpp
// Commit: Split holding and building display name helpers into dedicated source file

#include "building.h"

std::string holdingDisplayName(const Holding& holding)
{
    return
        std::string(holdingTypeName(holding.type)) +
        " #" +
        std::to_string(holding.id);
}

std::string buildingDisplayName(const Building& building)
{
    return
        std::string(buildingTypeName(building.type)) +
        " #" +
        std::to_string(building.id);
}
