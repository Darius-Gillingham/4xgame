// File: building/province_build_root.cpp
// Commit: Remove province-level Prestige spending from root holding construction

#include "province.h"

bool Province::hasCapitalType(HoldingType type) const
{
    (void)type;
    return false;
}

bool Province::canBuildRootHolding(
    HoldingType type,
    std::string& reason
) const
{
    if (!canBuildAsRoot(type))
    {
        reason =
            std::string(holdingTypeName(type)) +
            " cannot be built as a root holding.";

        return false;
    }

    reason =
        std::string("Build ") +
        holdingTypeName(type) +
        " as a standalone root holding.";

    return true;
}

bool Province::buildRootHolding(
    HoldingType type,
    std::string& reason
)
{
    if (!canBuildRootHolding(type, reason))
    {
        return false;
    }

    Holding holding;
    holding.id = nextHoldingId_++;
    holding.type = type;
    holding.parentId = -1;
    holdings_.push_back(holding);

    const Holding* built =
        getHoldingById(holding.id);

    reason =
        std::string("Built ") +
        (
            built
                ? holdingDisplayName(*built)
                : std::string(holdingTypeName(type))
        ) +
        ".";

    return true;
}