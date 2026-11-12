// File: building/province_build_attach.cpp
// Commit: Migrate estate and smallholder attachment construction from buildings to holdings

#include "province.h"

namespace
{
    constexpr int kAttachmentHoldingPrestigeCost = 10;
}

int Province::buildAttachedHolding(
    HoldingType type,
    int parentId
)
{
    const int newId = nextHoldingId_++;

    Holding child;
    child.id = newId;
    child.type = type;
    child.parentId = parentId;
    holdings_.push_back(child);

    Holding* parent = getHoldingById(parentId);

    if (parent)
    {
        parent->childIds.push_back(newId);
    }

    return newId;
}

AttachmentCounts Province::attachmentCountsForParent(int parentId) const
{
    AttachmentCounts counts;

    const Holding* parent = getHoldingById(parentId);

    if (!parent)
    {
        return counts;
    }

    for (int childId : parent->childIds)
    {
        const Holding* child = getHoldingById(childId);

        if (!child)
        {
            continue;
        }

        if (isSmallholderHoldingType(child->type))
        {
            ++counts.directSmallholders;
        }
        else if (isEstateHoldingType(child->type))
        {
            ++counts.estates;
        }
    }

    return counts;
}

bool Province::canBuildSmallholderAttachment(
    int parentId,
    std::string& reason
) const
{
    const Holding* parent = getHoldingById(parentId);

    if (!parent)
    {
        reason = "No valid parent selected.";
        return false;
    }

    const std::vector<HoldingType> allowedTypes =
        allowedSmallholderTypesForParent(parent->type);

    if (allowedTypes.empty())
    {
        reason =
            std::string(holdingTypeName(parent->type)) +
            " cannot host smallholders.";

        return false;
    }

    const AttachmentCounts counts =
        attachmentCountsForParent(parentId);

    const int capacity =
        directAttachmentSlotCapacity(parent->type);

    if (counts.directSmallholders + counts.estates >= capacity)
    {
        reason =
            holdingDisplayName(*parent) +
            " has no free attachment slots.";

        return false;
    }

    if (prestige() < kAttachmentHoldingPrestigeCost)
    {
        reason =
            std::string("Not enough Prestige. Requires ") +
            std::to_string(kAttachmentHoldingPrestigeCost) +
            " Prestige.";

        return false;
    }

    reason =
        std::string("Attach ") +
        holdingTypeName(allowedTypes.front()) +
        " to " +
        holdingDisplayName(*parent) +
        ". Cost: " +
        std::to_string(kAttachmentHoldingPrestigeCost) +
        " Prestige.";

    return true;
}

bool Province::buildSmallholderAttachment(
    int parentId,
    std::string& reason
)
{
    if (!canBuildSmallholderAttachment(parentId, reason))
    {
        return false;
    }

    const Holding* parent = getHoldingById(parentId);

    if (!parent)
    {
        reason = "No valid parent selected.";
        return false;
    }

    const std::vector<HoldingType> allowedTypes =
        allowedSmallholderTypesForParent(parent->type);

    if (allowedTypes.empty())
    {
        reason =
            std::string(holdingTypeName(parent->type)) +
            " cannot host smallholders.";

        return false;
    }

    if (!spendPrestige(kAttachmentHoldingPrestigeCost))
    {
        reason =
            std::string("Not enough Prestige. Requires ") +
            std::to_string(kAttachmentHoldingPrestigeCost) +
            " Prestige.";

        return false;
    }

    const int newId =
        buildAttachedHolding(
            allowedTypes.front(),
            parentId
        );

    const Holding* holding =
        getHoldingById(newId);

    reason =
        std::string("Built ") +
        (
            holding
                ? holdingDisplayName(*holding)
                : "Smallholder"
        ) +
        ". Spent " +
        std::to_string(kAttachmentHoldingPrestigeCost) +
        " Prestige.";

    return true;
}

bool Province::canBuildEstateAttachment(
    int parentId,
    std::string& reason
) const
{
    const Holding* parent = getHoldingById(parentId);

    if (!parent)
    {
        reason = "No valid parent selected.";
        return false;
    }

    const std::vector<HoldingType> allowedTypes =
        allowedEstateTypesForParent(parent->type);

    if (allowedTypes.empty())
    {
        reason =
            std::string(holdingTypeName(parent->type)) +
            " cannot host estates.";

        return false;
    }

    const AttachmentCounts counts =
        attachmentCountsForParent(parentId);

    const int capacity =
        directAttachmentSlotCapacity(parent->type);

    if (counts.directSmallholders + counts.estates >= capacity)
    {
        reason =
            holdingDisplayName(*parent) +
            " has no free attachment slots.";

        return false;
    }

    if (prestige() < kAttachmentHoldingPrestigeCost)
    {
        reason =
            std::string("Not enough Prestige. Requires ") +
            std::to_string(kAttachmentHoldingPrestigeCost) +
            " Prestige.";

        return false;
    }

    reason =
        std::string("Attach ") +
        holdingTypeName(allowedTypes.front()) +
        " to " +
        holdingDisplayName(*parent) +
        ". Cost: " +
        std::to_string(kAttachmentHoldingPrestigeCost) +
        " Prestige.";

    return true;
}

bool Province::buildEstateAttachment(
    int parentId,
    std::string& reason
)
{
    if (!canBuildEstateAttachment(parentId, reason))
    {
        return false;
    }

    const Holding* parent = getHoldingById(parentId);

    if (!parent)
    {
        reason = "No valid parent selected.";
        return false;
    }

    const std::vector<HoldingType> allowedTypes =
        allowedEstateTypesForParent(parent->type);

    if (allowedTypes.empty())
    {
        reason =
            std::string(holdingTypeName(parent->type)) +
            " cannot host estates.";

        return false;
    }

    if (!spendPrestige(kAttachmentHoldingPrestigeCost))
    {
        reason =
            std::string("Not enough Prestige. Requires ") +
            std::to_string(kAttachmentHoldingPrestigeCost) +
            " Prestige.";

        return false;
    }

    const int newId =
        buildAttachedHolding(
            allowedTypes.front(),
            parentId
        );

    const Holding* holding =
        getHoldingById(newId);

    reason =
        std::string("Built ") +
        (
            holding
                ? holdingDisplayName(*holding)
                : "Estate"
        ) +
        ". Spent " +
        std::to_string(kAttachmentHoldingPrestigeCost) +
        " Prestige.";

    return true;
}