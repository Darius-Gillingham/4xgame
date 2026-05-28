// File: province_state.cpp
// Commit: Initialize province pops and expose population totals without peasant terminology

#include "province.h"

const char* popTypeName(PopType type)
{
    switch (type)
    {
        case PopType::farmer: return "Farmer";
        case PopType::laborer: return "Laborer";
        case PopType::artisan: return "Artisan";
        case PopType::merchant: return "Merchant";
        case PopType::clergy: return "Clergy";
        case PopType::noble: return "Noble";
        case PopType::tribesman: return "Tribesman";
        default: return "Unknown";
    }
}

Province::Province()
    : nextBuildingId_(1), nextPopId_(1)
{
    Pop farmerPop;
    farmerPop.id = nextPopId_++;
    farmerPop.type = PopType::farmer;
    farmerPop.population = 200;
    farmerPop.assignedBuildingId = -1;
    pops_.push_back(farmerPop);
}

int Province::totalPopulation() const
{
    int total = 0;

    for (const Pop& pop : pops_)
    {
        total += pop.population;
    }

    return total;
}

int Province::unassignedPopulation() const
{
    int total = 0;

    for (const Pop& pop : pops_)
    {
        if (pop.assignedBuildingId < 0)
        {
            total += pop.population;
        }
    }

    return total;
}

const std::vector<Pop>& Province::pops() const
{
    return pops_;
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