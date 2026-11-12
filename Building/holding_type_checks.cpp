// File: building/holding_type_checks.cpp
// Commit: Split holding type checks into dedicated source file

#include "building.h"

bool isCapitalHoldingType(HoldingType type)
{
    return
        type == HoldingType::castle ||
        type == HoldingType::city ||
        type == HoldingType::church ||
        type == HoldingType::tribe ||
        type == HoldingType::sacred_site;
}

bool isRegularHoldingType(HoldingType type)
{
    return !isCapitalHoldingType(type);
}

bool isEstateHoldingType(HoldingType type)
{
    return
        type == HoldingType::noble_estate ||
        type == HoldingType::merchant_estate ||
        type == HoldingType::religious_estate ||
        type == HoldingType::tribal_estate;
}

bool isSmallholderHoldingType(HoldingType type)
{
    return
        type == HoldingType::smallholder ||
        type == HoldingType::tribal_smallholder;
}

bool isRegularCapitalType(HoldingType type)
{
    return
        type == HoldingType::castle ||
        type == HoldingType::city ||
        type == HoldingType::church;
}

bool isTribalCapitalType(HoldingType type)
{
    return type == HoldingType::tribe;
}

bool isRegularSmallholderType(HoldingType type)
{
    return type == HoldingType::smallholder;
}

bool isTribalSmallholderType(HoldingType type)
{
    return type == HoldingType::tribal_smallholder;
}

bool canBuildAsRoot(HoldingType type)
{
    return type == HoldingType::sacred_site;
}