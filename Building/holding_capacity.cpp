// File: building/holding_capacity.cpp
// Commit: Split holding capacity logic into dedicated source file

#include "building.h"

int directAttachmentSlotCapacity(HoldingType type)
{
    switch (type)
    {
        case HoldingType::castle: return 4;
        case HoldingType::city: return 4;
        case HoldingType::church: return 4;
        case HoldingType::tribe: return 4;
        case HoldingType::sacred_site: return 0;

        case HoldingType::noble_estate: return 2;
        case HoldingType::merchant_estate: return 2;
        case HoldingType::religious_estate: return 2;
        case HoldingType::tribal_estate: return 2;

        case HoldingType::smallholder: return 0;
        case HoldingType::tribal_smallholder: return 0;

        default: return 0;
    }
}

int buildingSlotCapacity(HoldingType type)
{
    if (type == HoldingType::sacred_site)
    {
        return 3;
    }

    if (isCapitalHoldingType(type))
    {
        return 7;
    }

    if (isEstateHoldingType(type))
    {
        return 5;
    }

    if (isSmallholderHoldingType(type))
    {
        return 3;
    }

    return 0;
}

bool canHostAttachedHoldings(HoldingType type)
{
    return directAttachmentSlotCapacity(type) > 0;
}

bool canHostBuildings(HoldingType type)
{
    return buildingSlotCapacity(type) > 0;
}