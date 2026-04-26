// File: province_state.cpp
// Commit: Extract province constructor, pop totals, building collection accessors, and building lookup helpers

#include "province.h"

Province::Province()
    : nextBuildingId_(1), totalPeasants_(200)
{
}

int Province::totalPeasants() const
{
    return totalPeasants_;
}

int Province::freePeasants() const
{
    return totalPeasants_;
}

const std::vector<Building>& Province::buildings() const
{
    return buildings_;
}

const Building* Province::getBuildingById(int id) const
{
    for (const Building& building : buildings_)
    {
        if (building.id == id)
        {
            return &building;
        }
    }

    return nullptr;
}

Building* Province::getBuildingById(int id)
{
    for (Building& building : buildings_)
    {
        if (building.id == id)
        {
            return &building;
        }
    }

    return nullptr;
}