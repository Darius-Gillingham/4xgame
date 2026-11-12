// File: building/province_queries.cpp
// Commit: Migrate province holding queries and slot capacity helpers to holdings and buildings

#include "province.h"

std::vector<int> Province::rootHoldingIds() const
{
    std::vector<int> ids;

    for (const Holding& holding : holdings_)
    {
        if (holding.parentId < 0)
        {
            ids.push_back(holding.id);
        }
    }

    return ids;
}

std::vector<int> Province::directSmallholderIdsForParent(int parentId) const
{
    std::vector<int> ids;

    const Holding* parent =
        getHoldingById(parentId);

    if (!parent)
    {
        return ids;
    }

    for (int childId : parent->childIds)
    {
        const Holding* child =
            getHoldingById(childId);

        if (
            child &&
            isSmallholderHoldingType(child->type)
        )
        {
            ids.push_back(childId);
        }
    }

    return ids;
}

std::vector<int> Province::estateIdsForParent(int parentId) const
{
    std::vector<int> ids;

    const Holding* parent =
        getHoldingById(parentId);

    if (!parent)
    {
        return ids;
    }

    for (int childId : parent->childIds)
    {
        const Holding* child =
            getHoldingById(childId);

        if (
            child &&
            isEstateHoldingType(child->type)
        )
        {
            ids.push_back(childId);
        }
    }

    return ids;
}

int Province::directSmallholderSlotCapacityForHolding(int holdingId) const
{
    const Holding* holding =
        getHoldingById(holdingId);

    if (!holding)
    {
        return 0;
    }

    return directAttachmentSlotCapacity(
        holding->type
    );
}

int Province::estateSlotCapacityForHolding(int holdingId) const
{
    const Holding* holding =
        getHoldingById(holdingId);

    if (!holding)
    {
        return 0;
    }

    return directAttachmentSlotCapacity(
        holding->type
    );
}

int Province::buildingSlotCapacityForHolding(int holdingId) const
{
    const Holding* holding =
        getHoldingById(holdingId);

    if (!holding)
    {
        return 0;
    }

    return buildingSlotCapacity(
        holding->type
    );
}